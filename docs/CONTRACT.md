# Shoots.Host Contract (Frozen)

## Endpoints
- `/`
- `/livez`
- `/healthz`
- `/readyz`
- `/metrics`
- `/status`
- `/v1/*` API routes

## Envelope
- Success: `{"ok":true,"result":...}`
- Error: `{"ok":false,"error":{"code":"...","message":"...","details":{...}}}`

## Headers
- `Content-Type: application/json` on JSON responses
- `X-Request-Id: req-000001` deterministic monotonic per process

## Auth/CORS
- API key gate optional via `SHOOTS_HOST_API_KEY` / `--api-key`
- Non-health endpoints require `X-Api-Key` when enabled
- CORS disabled unless `SHOOTS_HOST_CORS_ORIGIN` is set

## Not guaranteed
- Wall-clock timestamps
- External provider network behavior
