param(
    [Parameter(Mandatory)][string]$RepoName,
    [string]$Name = "performance"
)
$ErrorActionPreference = "Stop"
$PSNativeCommandUseErrorActionPreference = $true

$repo = (Get-Item $RepoName).FullName
$assets = "$PWD/assets"
$common = $PWD

Write-Output "Installing what the benchmark tool needs"
sudo apt-get -y install cmake libapr1-dev libaprutil1-dev

# Configurations named _IPI measure IP Intelligence, and the others device
# detection, so each has its own graph.
$ipi = $Name -like '*_IPI'

# The module folder install-package.ps1 built from the release tarball if there
# is one, otherwise the one build-project.ps1 built.
$env:BUILD_PATH = (Test-Path "$repo/package-build") ? "$repo/package-build" : $repo
if ($ipi) {
    $env:ENGINE = "ipi"
    $env:DATA_FILE = "$repo/cxx/ip-intelligence-data/51Degrees-IPIV4AsnIpiV41.ipi"
    $env:EVIDENCE = "$repo/cxx/ip-intelligence-data/evidence.csv"
} else {
    $env:ENGINE = "hash"
    $env:DATA_FILE = "$assets/TAC-HashV41.hash"
    $env:EVIDENCE = "$assets/20000 Evidence Records.yml"
}

$build = New-Item -ItemType Directory -Force -Path "$repo/tests/performance/build"
Push-Location $build
try {
    cmake ..
    cmake --build .
    ./runPerf.sh
    $results = "$PWD/results.json"
} finally {
    Pop-Location
}

& $common/steps/publish-performance-results.ps1 -SourceFile $results -Name $Name -RepoName $RepoName
