import subprocess, json, time
from pathlib import Path

EXE = str(Path(__file__).resolve().parent.parent / 'bin' / 'KeilUvscMcp.exe')

def run_session(extra):
    reqs = [
        {"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"v","version":"0"}}},
        {"jsonrpc":"2.0","method":"notifications/initialized"},
        {"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"keil_connect","arguments":{"mode":"attach","port":4823}}},
    ] + extra + [
        {"jsonrpc":"2.0","id":99,"method":"tools/call","params":{"name":"keil_disconnect","arguments":{}}},
    ]
    p = subprocess.Popen([EXE], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding='utf-8', errors='replace')
    out, _ = p.communicate("\n".join(json.dumps(r) for r in reqs) + "\n", timeout=40)
    res={}
    for line in out.splitlines():
        line=line.strip()
        if not line: continue
        try: obj=json.loads(line)
        except: continue
        rid=obj.get("id")
        if rid is None: continue
        res[rid]=obj.get("result",{}).get("structuredContent", obj.get("result",{}))
    return res

# Phase A: ensure running, read live
a = run_session([
    {"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"keil_run","arguments":{}}},
    {"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"keil_get_status","arguments":{}}},
])
print("run ->", json.dumps(a.get(10),ensure_ascii=False))
print("status ->", a.get(11,{}).get("execution_state"))

time.sleep(3)

# Phase B: read live vars while running, then stop, then read again (should freeze)
b = run_session([
    {"jsonrpc":"2.0","id":20,"method":"tools/call","params":{"name":"keil_read_variables","arguments":{"variables":["wAverageInputPower_B","b_GeneralTime_Status","P0"]}}},
    {"jsonrpc":"2.0","id":21,"method":"tools/call","params":{"name":"keil_stop","arguments":{}}},
    {"jsonrpc":"2.0","id":22,"method":"tools/call","params":{"name":"keil_get_status","arguments":{}}},
    {"jsonrpc":"2.0","id":23,"method":"tools/call","params":{"name":"keil_read_variables","arguments":{"variables":["wAverageInputPower_B","b_GeneralTime_Status","P0"]}}},
])
print("T(run) vars ->", json.dumps(b.get(20),ensure_ascii=False))
print("stop ->", json.dumps(b.get(21),ensure_ascii=False))
print("status ->", b.get(22,{}).get("execution_state"))
print("T(stopped) vars ->", json.dumps(b.get(23),ensure_ascii=False))
