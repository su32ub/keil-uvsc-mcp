import subprocess, json, time
from pathlib import Path
EXE = str(Path(__file__).resolve().parent.parent / 'bin' / 'KeilUvscMcp.exe')
def call(reqs):
    full=[{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"v","version":"0"}}},
          {"jsonrpc":"2.0","method":"notifications/initialized"},
          {"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"keil_connect","arguments":{"mode":"attach","port":4823}}}]+reqs+[
          {"jsonrpc":"2.0","id":99,"method":"tools/call","params":{"name":"keil_disconnect","arguments":{}}}]
    p=subprocess.Popen([EXE],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,encoding='utf-8',errors='replace')
    out,_=p.communicate("\n".join(json.dumps(r) for r in full)+"\n",timeout=60)
    res={}
    for line in out.splitlines():
        line=line.strip()
        if not line: continue
        try: obj=json.loads(line)
        except: continue
        rid=obj.get("id")
        if rid is not None: res[rid]=obj.get("result",{}).get("structuredContent",obj.get("result",{}))
    return res

# stop, set BP on main, RESET (so execution restarts and must hit main), then poll
r=call([
 {"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"keil_stop","arguments":{}}},
 {"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"keil_set_breakpoint","arguments":{"expression":"main"}}},
 {"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"keil_reset","arguments":{}}},
 {"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"keil_run","arguments":{}}},
])
tm=r.get(4,{}).get("tick_mark")
print("stop:",r.get(3,{}).get("execution_state"))
print("set_bp(main):",json.dumps(r.get(4),ensure_ascii=False))
print("reset:",json.dumps(r.get(5),ensure_ascii=False))
print("run:",json.dumps(r.get(6),ensure_ascii=False))

hit=False
for i in range(12):
    s=call([{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"keil_get_status","arguments":{}}}])
    if s.get(7,{}).get("execution_state")=="stopped":
        hit=True;print("poll %d: STOPPED (BP hit)"%i);break
    time.sleep(0.5)
if not hit: print("never stopped in ~6s")

ops=[{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"keil_eval_expression","arguments":{"expression":"PC"}}}]
if tm is not None:
    ops.append({"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"keil_clear_breakpoint","arguments":{"tick_mark":tm}}})
r2=call(ops)
pc=r2.get(8,{}).get("value")
print("PC:",pc,hex(pc) if isinstance(pc,int) else "","(main=0x97E5)")
print("clear_bp:",json.dumps(r2.get(9),ensure_ascii=False))
