param(
    [string]$VarnishVersion = "6.0.17"
)
$ErrorActionPreference = "Stop"
$PSNativeCommandUseErrorActionPreference = $true

# The publish workflow can call this more than once in a job.
if ((Get-Command varnishd -ErrorAction SilentlyContinue) -and
    ((varnishd -V 2>&1 | Out-String) -match "varnish-$([regex]::Escape($VarnishVersion)) ")) {
    Write-Output "Varnish $VarnishVersion is already installed"
    exit 0
}

# Varnish uses version 2 of the regular expression library from 7.0.
$RegexLibrary = ([version]$VarnishVersion).Major -ge 7 ? "libpcre2-dev" : "libpcre3-dev"

Write-Output "Installing the packages needed to build Varnish $VarnishVersion"
sudo apt-get -y update
sudo apt-get -y install make automake autotools-dev autoconf-archive libtool `
    pkg-config cpio libedit-dev libncurses-dev python3-docutils $RegexLibrary

$Source = New-Item -ItemType Directory -Force -Path ([IO.Path]::Combine([IO.Path]::GetTempPath(), "varnish-source"))
Push-Location $Source
try {
    Write-Output "Building Varnish $VarnishVersion"
    curl -fLsS --retry 5 -o varnish.tgz "https://varnish-cache.org/downloads/varnish-$VarnishVersion.tgz"
    tar xzf varnish.tgz
    Push-Location "varnish-$VarnishVersion"
    try {
        ./configure --without-jemalloc --with-sphinx-build=true
        make -j ([Environment]::ProcessorCount)
        sudo make install
        sudo ldconfig
    } finally {
        Pop-Location
    }
} finally {
    Pop-Location
    Remove-Item -Recurse -Force $Source
}

varnishd -V
