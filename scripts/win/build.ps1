$ErrorActionPreference = 'Stop'

$prefix = "$PWD\_inst\Shoots"
$providerDir = "$PWD\_deps\Shoots.Provider"
$providerBuild = "$providerDir\build"

if (!(Test-Path $providerDir)) {
  git clone --depth 1 https://github.com/robertard7/Shoots.Provider.git $providerDir
}

cmake -S $providerDir -B $providerBuild -DCMAKE_INSTALL_PREFIX=$prefix
cmake --build $providerBuild --config Release
cmake --install $providerBuild --config Release

cmake -S . -B build -DCMAKE_PREFIX_PATH=$prefix
cmake --build build --config Release
