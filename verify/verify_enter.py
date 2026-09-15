import subprocess, json
from pathlib import Path

EXE = str(Path(__file__).resolve().parent.parent / 'bin' / 'KeilUvscMcp.exe')
reqs = [
    {"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"v","version":"0"}}},
    {"jsonrpc":"2.0","method":"notifications/initialized"},
    {"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"keil_connect","arguments":{"mode":"attach","port":4823}}},
    {"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"keil_stop","arguments":{}}},
    {"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"keil_read_variables","arguments":{"variables":["wAverageInputPower_B","b_GeneralTime_Status","P0"]}}},
    {"jsonrpc":"2.0","id":99,"method":"tools/call","params":{"name":"keil_disconnect","arguments":{}}},
]
p = subprocess.Popen([EXE], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding='utf-8', errors='replace')
out, _ = p.communicate("\n".join(json.dumps(r) for r in reqs) + "\n", timeout=60)
for line in out.splitlines():
    line=line.strip()
    if not line: continue
    try: obj=json.loads(line)
    except: continue
    rid=obj.get("id")
    if rid in (3,4):
        sc=obj.get("result",{}).get("structuredContent", obj.get("result",{}))
        print("id=%d: %s" % (rid, json.dumps(sc, ensure_ascii=False)))
