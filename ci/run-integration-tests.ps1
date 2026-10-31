param(
    [string]$Name = "integration"
)
$ErrorActionPreference = "Stop"

$repo = (Get-Item $PSScriptRoot/..).FullName
$assets = "$PWD/assets"

# Repeat the tests with the larger data file.
& $PSScriptRoot/run-vtc-tests.ps1 -BuildPath $repo -SuiteName $Name `
    -ResultsFile "$repo/test-results/integration/$Name.xml" `
    -MakeArguments "datafile=$assets/TAC-HashV41.hash"
exit $LASTEXITCODE
