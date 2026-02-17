# TLS Deployment Stance

Shoots.Host does not provide built-in TLS termination.

## Recommended pattern
Use a reverse proxy (for example, Caddy, Nginx, or Envoy) to terminate TLS and forward plaintext HTTP to Shoots.Host bound on loopback.

## Minimal guidance
- Keep `SHOOTS_HOST_BIND=127.0.0.1` by default.
- Expose public TLS only at the reverse proxy layer.
- Forward `X-Request-Id` if downstream observability depends on it.
