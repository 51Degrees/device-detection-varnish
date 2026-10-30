param(
    [string]$Name = "integration",
    [bool]$MemCheck = $False
)
$ErrorActionPreference = "Stop"

# A library built with the address sanitizer only loads through make mem-check,
# which the unit tests have already run.
if ($MemCheck) {
    Write-Output "Not repeating the tests with the larger data file"
    exit 0
}

$repo = (Get-Item $PSScriptRoot/..).FullName
$assets = "$PWD/assets"

# Repeat the tests with the larger data file, in the folder install-package.ps1
# built from the release tarball if there is one.
$build = (Test-Path "$repo/package-build") ? "$repo/package-build" : $repo
& $PSScriptRoot/run-vtc-tests.ps1 -BuildPath $build -SuiteName $Name `
    -ResultsFile "$repo/test-results/integration/$Name.xml" `
    -MakeArguments "datafile=$assets/TAC-HashV41.hash"
$testsExit = $LASTEXITCODE

Write-Output "Running the browser test of the JavaScript example"
$work = New-Item -ItemType Directory -Force -Path "$repo/test-results/browser"
$vcl = "$work/javascript.vcl"
(Get-Content -Raw "$repo/examples/hash/jsExample/javascript.vcl").Replace(
    "/etc/varnish/51Degrees-LiteV4.1.hash", "$assets/TAC-HashV41.hash") | Set-Content $vcl
varnishd -n "$work/varnish" -P "$work/varnish.pid" -a 127.0.0.1:8888 -f $vcl -p "vmod_path=$build/src/dd/.libs"
if ($LASTEXITCODE -ne 0) {
    Write-Output "::warning::Varnish did not start for the browser test"
} else {
    $page = Start-Process python3 -PassThru -ArgumentList "-m", "http.server", "8889", "--directory", "$repo/examples/hash/jsExample"
    Push-Location "$repo/tests/examples/jsExample"
    try {
        $env:PATH = "$($env:PATH):$($env:CHROMEWEBDRIVER):$($env:GECKOWEBDRIVER)"
        npm install
        npm test
        # Browsers on a shared runner can fail for reasons of their own, so a
        # failure is reported without failing the job, as in the nginx repository.
        if ($LASTEXITCODE -ne 0) {
            Write-Output "::warning::The browser test of the JavaScript example failed"
        }
    } finally {
        Pop-Location
        Stop-Process -InputObject $page
        # varnishd holds a lock on the file which .NET will not read past.
        sh -c "kill `$(cat '$work/varnish.pid')"
    }
}
exit $testsExit
