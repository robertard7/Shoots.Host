$ErrorActionPreference = 'Stop'

$repoRoot = (git rev-parse --show-toplevel).Trim()
Set-Location $repoRoot

$bind = if ($env:SHOOTS_HOST_BIND) { $env:SHOOTS_HOST_BIND } else { '127.0.0.1' }
$port = if ($env:SHOOTS_HOST_PORT) { $env:SHOOTS_HOST_PORT } else { '8787' }
$logLevel = if ($env:SHOOTS_HOST_LOG_LEVEL) { $env:SHOOTS_HOST_LOG_LEVEL } else { 'info' }
$logFormat = if ($env:SHOOTS_HOST_LOG_FORMAT) { $env:SHOOTS_HOST_LOG_FORMAT } else { 'text' }
$maxBody = if ($env:SHOOTS_HOST_MAX_BODY_BYTES) { $env:SHOOTS_HOST_MAX_BODY_BYTES } else { '1048576' }
$reqTimeout = if ($env:SHOOTS_HOST_REQ_TIMEOUT_MS) { $env:SHOOTS_HOST_REQ_TIMEOUT_MS } else { '1000' }
$maxInflight = if ($env:SHOOTS_HOST_MAX_INFLIGHT) { $env:SHOOTS_HOST_MAX_INFLIGHT } else { '64' }
$shutdownDrain = if ($env:SHOOTS_HOST_SHUTDOWN_DRAIN_MS) { $env:SHOOTS_HOST_SHUTDOWN_DRAIN_MS } else { '2000' }
$metricsFormat = if ($env:SHOOTS_HOST_METRICS_FORMAT) { $env:SHOOTS_HOST_METRICS_FORMAT } else { 'json' }

$exe = Join-Path $repoRoot 'build\Release\ShootsHost.exe'
if (-not (Test-Path $exe)) {
  throw "Missing host binary: $exe"
}

$args = @(
  '--bind', $bind,
  '--port', $port,
  '--log-level', $logLevel,
  '--log-format', $logFormat,
  '--max-body-bytes', $maxBody,
  '--req-timeout-ms', $reqTimeout,
  '--max-inflight', $maxInflight,
  '--shutdown-drain-ms', $shutdownDrain,
  '--metrics-format', $metricsFormat
)

Write-Output "Starting ShootsHost on http://$bind`:$port"
& $exe @args
