import subprocess, json, time
from pathlib import Path

EXE = str(Path(__file__).resolve().parent.parent / 'bin' / 'KeilUvscMcp.exe')
reqs = [
    {"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"v","version":"0"}}},
    {"jsonrpc":"2.0","method":"notifications/initialized"},
    {"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"keil_connect","arguments":{"mode":"attach","port":4823}}},
    {"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"keil_get_status","arguments":{}}},
    {"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"keil_reset","arguments":{}}},
    {"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"keil_run","arguments":{}}},
    {"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"keil_get_status","arguments":{}}},
]
p = subprocess.Popen([EXE], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding='utf-8', errors='replace')
out, _ = p.communicate("\n".join(json.dumps(r) for r in reqs) + "\n", timeout=40)
# phase 1 done; now wait while target runs, then a second session to read g_tick and stop
for line in out.splitlines():
    line=line.strip()
    if not line: continue
    try: obj=json.loads(line)
    except: continue
    rid=obj.get("id")
    if rid is None or rid<3: continue
    res=obj.get("result",{})
    sc=res.get("structuredContent", res)
    print("id=%s: %s" % (rid, json.dumps(sc, ensure_ascii=False)))

time.sleep(3)

reqs2 = [
    {"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"v","version":"0"}}},
    {"jsonrpc":"2.0","method":"notifications/initialized"},
    {"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"keil_connect","arguments":{"mode":"attach","port":4823}}},
    {"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"keil_read_variable","arguments":{"variable":"g_tick"}}},
    {"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"keil_stop","arguments":{}}},
    {"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"keil_get_status","arguments":{}}},
    {"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"keil_read_variable","arguments":{"variable":"g_tick"}}},
    {"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"keil_disconnect","arguments":{}}},
]
p = subprocess.Popen([EXE], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding='utf-8', errors='replace')
out, _ = p.communicate("\n".join(json.dumps(r) for r in reqs2) + "\n", timeout=40)
for line in out.splitlines():
    line=line.strip()
    if not line: continue
    try: obj=json.loads(line)
    except: continue
    rid=obj.get("id")
    if rid is None or rid<7: continue
    res=obj.get("result",{})
    sc=res.get("structuredContent", res)
    print("id=%s: %s" % (rid, json.dumps(sc, ensure_ascii=False)))
