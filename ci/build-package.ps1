$ErrorActionPreference = "Stop"

New-Item -ItemType Directory -Force -Path package | Out-Null

# Each package requirement's artifact arrives in a folder of its own.
Get-ChildItem package-files -Recurse -Filter *.tar.gz | Copy-Item -Destination package
