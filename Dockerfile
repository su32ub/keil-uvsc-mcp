# Container image for MCP introspection / directory health checks (e.g. Glama).
#
# KeilUvscMcp.exe speaks MCP (JSON-RPC) over stdio. It starts and answers
# initialize / tools-list without Keil — UVSC64.dll is only loaded lazily when
# the keil_connect tool is actually called, which requires Windows + Keil
# µVision. Under Mono the binary still starts and serves introspection.
#
#   docker build -t keil-uvsc-mcp .
#   printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"c","version":"0"}}}' | docker run -i keil-uvsc-mcp
FROM mono:6.12

WORKDIR /app
COPY bin/KeilUvscMcp.exe /app/KeilUvscMcp.exe

CMD ["mono", "/app/KeilUvscMcp.exe"]
