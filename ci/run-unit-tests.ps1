param(
    [string]$Name = "unit",
    [bool]$MemCheck = $False
)
$ErrorActionPreference = "Stop"

$repo = (Get-Item $PSScriptRoot/..).FullName
$assets = "$PWD/assets"

# The submodule holds a pointer to this file rather than the file, as the
# clone in common-ci does not fetch large files.
& $PSScriptRoot/run-vtc-tests.ps1 -BuildPath $repo -SuiteName $Name `
    -ResultsFile "$repo/test-results/unit/$Name.xml" `
    -MakeArguments "datafile=$assets/51Degrees-LiteV4.1.hash" `
    -Target ($MemCheck ? "mem-check" : "check")
exit $LASTEXITCODE
