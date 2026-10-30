param(
    [Parameter(Mandatory)][string]$RepoName,
    [Parameter(Mandatory)][string]$Version
)
$ErrorActionPreference = "Stop"
$PSNativeCommandUseErrorActionPreference = $true

$output = New-Item -ItemType Directory -Force -Path package-files

Push-Location $RepoName
try {
    # GitHub's own source download leaves out submodules, so this tarball is
    # the one that builds without a clone.
    ./autogen.sh
    ./configure --with-config=test
    make
    make dist distdir="$RepoName-$Version"
    Move-Item "$RepoName-$Version.tar.gz" $output
} finally {
    Pop-Location
}
