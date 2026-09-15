import subprocess, json, time
from pathlib import Path

EXE = str(Path(__file__).resolve().parent.parent / 'bin' / 'KeilUvscMcp.exe')

def session(actions):
    """actions: list of (id, tool, args). Returns {id: structuredContent}."""
    reqs = [
        {"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"v","version":"0"}}},
        {"jsonrpc":"2.0","method":"notifications/initialized"},
        {"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"keil_connect","arguments":{"mode":"attach","port":4823}}},
    ]
    for rid, tool, args in actions:
        reqs.append({"jsonrpc":"2.0","id":rid,"method":"tools/call","params":{"name":tool,"arguments":args}})
    reqs.append({"jsonrpc":"2.0","id":99,"method":"tools/call","params":{"name":"keil_disconnect","arguments":{}}})
    p = subprocess.Popen([EXE], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding='utf-8', errors='replace')
    out, _ = p.communicate("\n".join(json.dumps(r) for r in reqs) + "\n", timeout=40)
    res = {}
    for line in out.splitlines():
        line=line.strip()
        if not line: continue
        try: obj=json.loads(line)
        except: continue
        rid=obj.get("id")
        if rid is None: continue
        r=obj.get("result",{})
        res[rid]=r.get("structuredContent", r)
    return res

# snapshot 1
s1 = session([(10,"keil_get_status",{}),(11,"keil_read_variables",{"variables":["g_tick","g_led"]}),(12,"keil_eval_expression",{"expression":"P1"})])
print("T0 status:", s1.get(10,{}).get("execution_state"))
print("T0 vars:", json.dumps(s1.get(11), ensure_ascii=False))
print("T0 P1:", s1.get(12,{}).get("display"))

time.sleep(2.5)

# snapshot 2
s2 = session([(10,"keil_get_status",{}),(11,"keil_read_variables",{"variables":["g_tick","g_led"]}),(12,"keil_eval_expression",{"expression":"P1"})])
print("T1 status:", s2.get(10,{}).get("execution_state"))
print("T1 vars:", json.dumps(s2.get(11), ensure_ascii=False))
print("T1 P1:", s2.get(12,{}).get("display"))

def gv(s, name):
    for v in s.get(11,{}).get("variables",[]):
        if v["expression"]==name: return v.get("value")
    return None
t0, t1 = gv(s1,"g_tick"), gv(s2,"g_tick")
print("g_tick T0=%s T1=%s -> %s" % (t0, t1, "LIVE (changed)" if t0!=t1 else "unchanged (target running? press F5)"))
