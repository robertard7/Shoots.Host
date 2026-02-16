$ErrorActionPreference = 'Stop'

$prefix = 'C:\dev\_inst\Shoots'
$providerDir = "$env:TEMP\Shoots.Provider"
$providerBuild = "$providerDir\build"

if (!(Test-Path $providerDir)) {
  git clone --depth 1 https://github.com/robertard7/Shoots.Provider.git $providerDir
}

cmake -S $providerDir -B $providerBuild -DCMAKE_INSTALL_PREFIX=$prefix
cmake --build $providerBuild --config Release
cmake --install $providerBuild --config Release

cmake -S . -B build -DCMAKE_PREFIX_PATH=$prefix
cmake --build build --config Release

$proc = Start-Process -FilePath ".\build\Release\ShootsHost.exe" -PassThru
Start-Sleep -Seconds 1
Invoke-RestMethod -Uri 'http://127.0.0.1:8787/health'
Stop-Process -Id $proc.Id
