$ErrorActionPreference = "Stop"
$PSNativeCommandUseErrorActionPreference = $true

Push-Location $PSScriptRoot/..
try {
    ./autogen.sh
    ./configure --with-config=test
    make
} finally {
    Pop-Location
}
