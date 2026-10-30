param(
    [string]$Name = "integration"
)
$ErrorActionPreference = "Stop"

$repo = (Get-Item $PSScriptRoot/..).FullName
$assets = "$PWD/assets"

# Repeat the tests with the larger data file, in the folder install-package.ps1
# built from the release tarball if there is one.
$build = (Test-Path "$repo/package-build") ? "$repo/package-build" : $repo
& $PSScriptRoot/run-vtc-tests.ps1 -BuildPath $build -SuiteName $Name `
    -ResultsFile "$repo/test-results/integration/$Name.xml" `
    -MakeArguments "datafile=$assets/TAC-HashV41.hash"
exit $LASTEXITCODE
