<#
.SYNOPSIS
Runs make check in a build folder and writes a JUnit file with one test case
for each .vtc file, as varnishtest has no such output of its own.

.PARAMETER BuildPath
The folder to run make check in.

.PARAMETER ResultsFile
The JUnit file to write.

.PARAMETER SuiteName
The name of the test suite in the JUnit file.

.PARAMETER MakeArguments
Further arguments for make, such as the data file to test with.
#>
param(
    [Parameter(Mandatory)][string]$BuildPath,
    [Parameter(Mandatory)][string]$ResultsFile,
    [Parameter(Mandatory)][string]$SuiteName,
    [string[]]$MakeArguments = @()
)
$ErrorActionPreference = "Stop"

New-Item -ItemType Directory -Force -Path (Split-Path $ResultsFile) | Out-Null

Push-Location $BuildPath
try {
    $output = & make -k check @MakeArguments 2>&1 | ForEach-Object { "$_" }
    $makeExit = $LASTEXITCODE
} finally {
    Pop-Location
}
$output | Write-Output

$cases = foreach ($line in $output) {
    if ($line -match '^#\s+top\s+TEST\s+(\S+)\s+(passed|FAILED|skipped)\s+\(([0-9.]+)\)') {
        [pscustomobject]@{
            Name = Split-Path -Leaf $Matches[1]
            Result = $Matches[2]
            Seconds = $Matches[3]
        }
    }
}

$failed = @($cases | Where-Object Result -eq 'FAILED').Count
$skipped = @($cases | Where-Object Result -eq 'skipped').Count
$xml = [System.Text.StringBuilder]::new()
[void]$xml.AppendLine('<?xml version="1.0" encoding="UTF-8"?>')
[void]$xml.AppendLine("<testsuite name=`"$SuiteName`" tests=`"$(@($cases).Count)`" failures=`"$failed`" skipped=`"$skipped`">")
foreach ($case in $cases) {
    [void]$xml.Append("  <testcase classname=`"$SuiteName`" name=`"$($case.Name)`" time=`"$($case.Seconds)`"")
    switch ($case.Result) {
        'FAILED' { [void]$xml.AppendLine('><failure message="varnishtest reported a failure, see the job log"/></testcase>') }
        'skipped' { [void]$xml.AppendLine('><skipped/></testcase>') }
        default { [void]$xml.AppendLine('/>') }
    }
}
[void]$xml.AppendLine('</testsuite>')
Set-Content -Path $ResultsFile -Value $xml.ToString()

if (@($cases).Count -eq 0) {
    Write-Output "make check ran no tests"
    exit 1
}
exit (($makeExit -ne 0 -or $failed -gt 0) ? 1 : 0)
