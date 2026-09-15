import subprocess, json
from pathlib import Path

EXE = str(Path(__file__).resolve().parent.parent / 'bin' / 'KeilUvscMcp.exe')
def session(extra):
    reqs=[{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"v","version":"0"}}},
          {"jsonrpc":"2.0","method":"notifications/initialized"},
          {"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"keil_connect","arguments":{"mode":"attach","port":4823}}}]+extra+[
          {"jsonrpc":"2.0","id":99,"method":"tools/call","params":{"name":"keil_disconnect","arguments":{}}}]
    p=subprocess.Popen([EXE],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,encoding='utf-8',errors='replace')
    out,_=p.communicate("\n".join(json.dumps(r) for r in reqs)+"\n",timeout=90)
    res={}
    for line in out.splitlines():
        line=line.strip()
        if not line: continue
        try: obj=json.loads(line)
        except: continue
        rid=obj.get("id")
        if rid is not None: res[rid]=obj.get("result",{}).get("structuredContent",obj.get("result",{}))
    return res

VAR="b_GeneralTime_Status"
# 1) enter debug + halt + baseline + encoded address
r=session([
 {"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"keil_enter_debug","arguments":{}}},
 {"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"keil_stop","arguments":{}}},
 {"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"keil_read_variable","arguments":{"variable":VAR}}},
 {"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"keil_eval_expression","arguments":{"expression":"&"+VAR}}},
])
print("enter_debug:",json.dumps(r.get(3),ensure_ascii=False))
print("stop:",r.get(4,{}).get("execution_state"))
base=r.get(5,{}).get("value")
print("baseline %s = %s (0x%02X)"%(VAR,base,base if isinstance(base,int) else 0))
addrval=r.get(6,{})
print("&%s -> %s"%(VAR,json.dumps(addrval,ensure_ascii=False)))
addr=addrval.get("value")
if isinstance(addr,str):
    try: addr=int(addr,0)
    except: addr=None
if not isinstance(addr,int):
    print("!! could not resolve address; abort"); raise SystemExit
print("encoded address = 0x%X"%addr)

newval=(base ^ 0xA5) & 0xFF
# 2) write flipped value, read back via variable AND raw memory
r2=session([
 {"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"keil_stop","arguments":{}}},
 {"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"keil_write_memory","arguments":{"address":"0x%X"%addr,"bytes":[newval]}}},
 {"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"keil_read_variable","arguments":{"variable":VAR}}},
 {"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"keil_read_memory","arguments":{"address":"0x%X"%addr,"length":1}}},
])
print("write_memory:",json.dumps(r2.get(7),ensure_ascii=False))
print("after write, read_variable =",r2.get(8,{}).get("value"),"(expect 0x%02X)"%newval)
print("after write, read_memory   =",json.dumps(r2.get(9),ensure_ascii=False))

# 3) restore original, confirm
r3=session([
 {"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"keil_stop","arguments":{}}},
 {"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"keil_write_memory","arguments":{"address":"0x%X"%addr,"bytes":[base & 0xFF]}}},
 {"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"keil_read_variable","arguments":{"variable":VAR}}},
])
print("restore write:",json.dumps(r3.get(7),ensure_ascii=False))
print("after restore, read_variable =",r3.get(8,{}).get("value"),"(expect 0x%02X)"%(base&0xFF))
