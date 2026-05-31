# Caro Room Server Deployment

These templates deploy the raw TCP room server behind `nginx` while keeping the
game process bound to loopback only.

Files:

- `caro-room-server.service`
- `nginx-caro-http.conf`
- `nginx-caro-stream.conf`

Replace placeholders before installing:

- `__CARO_SERVICE_USER__`
- `__CARO_SERVICE_GROUP__`
- `__CARO_WORKDIR__`
- `__CARO_BIN__`
- `__CARO_PUBLIC_DOMAIN__`
- `__CARO_PUBLIC_IP__`

Expected layout:

- game server listens on `127.0.0.1:34567`
- health server listens on `127.0.0.1:34569`
- `nginx` HTTPS site serves `/`, `/healthz`, and `/status`
- `nginx` stream proxy exposes `<domain>:34567`
