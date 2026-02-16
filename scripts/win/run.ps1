$ErrorActionPreference = 'Stop'

$env:SHOOTS_HOST_PORT = '8787'

$proc = Start-Process -FilePath ".\build\Release\ShootsHost.exe" -PassThru
Start-Sleep -Seconds 1
Invoke-RestMethod -Uri 'http://127.0.0.1:8787/health'
Stop-Process -Id $proc.Id
