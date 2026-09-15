import subprocess, json, sys
from pathlib import Path

EXE = str(Path(__file__).resolve().parent.parent / 'bin' / 'KeilUvscMcp.exe')

reqs = [
    {"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"verify","version":"0.1"}}},
    {"jsonrpc":"2.0","method":"notifications/initialized"},
    {"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"keil_connect","arguments":{"mode":"attach","port":4823}}},
    {"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"keil_get_status","arguments":{}}},
    {"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"keil_read_variable","arguments":{"variable":"g_tick"}}},
    {"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"keil_read_variable","arguments":{"variable":"g_led"}}},
    {"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"keil_eval_expression","arguments":{"expression":"g_tick"}}},
    {"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"keil_read_memory","arguments":{"address":"0x90","length":1}}},
    {"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"keil_disconnect","arguments":{}}},
]

p = subprocess.Popen([EXE], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding='utf-8', errors='replace')
inp = "\n".join(json.dumps(r) for r in reqs) + "\n"
out, _ = p.communicate(inp, timeout=40)

for line in out.splitlines():
    line = line.strip()
    if not line:
        continue
    try:
        obj = json.loads(line)
    except Exception:
        print("RAW:", line[:300]); continue
    rid = obj.get("id")
    res = obj.get("result", obj)
    sc = res.get("structuredContent") if isinstance(res, dict) else None
    if rid is None:
        continue
    print("=== id=%s ===" % rid)
    if sc is not None:
        print(json.dumps(sc, ensure_ascii=False, indent=1))
    else:
        print(json.dumps(res, ensure_ascii=False)[:800])
