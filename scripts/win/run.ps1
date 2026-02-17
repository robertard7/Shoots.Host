$ErrorActionPreference = 'Stop'

$repoRoot = (git rev-parse --show-toplevel).Trim()
Set-Location $repoRoot

$env:SHOOTS_HOST_PORT = if ($env:SHOOTS_HOST_PORT) { $env:SHOOTS_HOST_PORT } else { '8787' }
$env:SHOOTS_HOST_BIND = if ($env:SHOOTS_HOST_BIND) { $env:SHOOTS_HOST_BIND } else { '127.0.0.1' }

$proc = Start-Process -FilePath ".\build\Release\ShootsHost.exe" -ArgumentList @('--bind', $env:SHOOTS_HOST_BIND, '--port', $env:SHOOTS_HOST_PORT) -PassThru
Start-Sleep -Seconds 1
Invoke-RestMethod -Uri "http://$($env:SHOOTS_HOST_BIND):$($env:SHOOTS_HOST_PORT)/healthz" | Out-Null
Stop-Process -Id $proc.Id
