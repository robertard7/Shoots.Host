$ErrorActionPreference = 'Stop'

$repoRoot = (git rev-parse --show-toplevel).Trim()
Set-Location $repoRoot

$prefix = if ($env:SHOOTS_PREFIX) { $env:SHOOTS_PREFIX } else { "$repoRoot\_inst\shoots" }
$version = if ($env:SHOOTS_HOST_VERSION) { $env:SHOOTS_HOST_VERSION } else { "0.1.0" }
$stage = Join-Path $repoRoot "build\package-stage-win"
$distDir = Join-Path $repoRoot "dist"
$zipPath = Join-Path $distDir "shoots-host-win64-$version.zip"

cmake -S . -B build -DCMAKE_PREFIX_PATH="$prefix"
cmake --build build --config Release

if (Test-Path $stage) {
  Remove-Item -Recurse -Force $stage
}
cmake --install build --config Release --prefix $stage

New-Item -ItemType Directory -Force -Path $distDir | Out-Null
if (Test-Path $zipPath) {
  Remove-Item -Force $zipPath
}

$tempRoot = Join-Path $repoRoot "build\zip-root-win"
if (Test-Path $tempRoot) {
  Remove-Item -Recurse -Force $tempRoot
}
New-Item -ItemType Directory -Force -Path $tempRoot | Out-Null

$stagingRoot = Join-Path $tempRoot "shoots-host-win64-$version"
Copy-Item -Recurse -Force $stage $stagingRoot

Compress-Archive -Path "$stagingRoot\*" -DestinationPath $zipPath -CompressionLevel Optimal
Write-Output $zipPath
