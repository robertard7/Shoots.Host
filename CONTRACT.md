# Shoots.Host API Contract

## Deterministic guarantees

- Stable JSON key ordering across all JSON responses.
- Monotonic job IDs as decimal strings.
- Deterministic error envelope shape and error code strings.
- Deterministic polling behavior (`/wait` loops with fixed cadence).

## Success envelope

```json
{"ok":true,"data":{}}
```

## Error envelope

```json
{"ok":false,"error":{"code":"...","message":"...","details":{"field":"...","reason":"..."}}}
```

## HTTP status mapping

- `200` success
- `400` request/schema validation error
- `404` unknown job
- `409` conflict states (`not_ready`, `already_taken`)
- `500` internal host failures

## Job lifecycle

1. `POST /v1/chat|tool|build` creates a monotonic job ID and provider request ID.
2. `GET /v1/jobs/{id}` polls provider and updates job terminal state.
3. `GET /v1/jobs/{id}/wait` polls in fixed cadence until terminal or timeout.
4. `POST /v1/jobs/{id}/take` returns result once, then reports `already_taken`.

## Retention

- Job store is bounded by `max_jobs`.
- List endpoint returns newest-first deterministic ordering.
