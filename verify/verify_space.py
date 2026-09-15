import subprocess, json
from pathlib import Path

EXE = str(Path(__file__).resolve().parent.parent / 'bin' / 'KeilUvscMcp.exe')
reqs = [
    {"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"v","version":"0"}}},
    {"jsonrpc":"2.0","method":"notifications/initialized"},
    {"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"keil_connect","arguments":{"mode":"attach","port":4823}}},
    {"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"keil_read_variable","arguments":{"variable":"g_led"}}},
]
# sweep: space selector in bits16-23 AND bits24-31
idmap = {}
rid = 100
for pos in ('b16','b24'):
    for sp in range(0x00, 0x08):
        addr = (sp << 16) | 0x0A if pos=='b16' else (sp << 24) | 0x0A
        idmap[rid] = (pos, sp, addr)
        reqs.append({"jsonrpc":"2.0","id":rid,"method":"tools/call","params":{"name":"keil_read_memory","arguments":{"address":hex(addr),"length":1}}})
        rid += 1
reqs.append({"jsonrpc":"2.0","id":999,"method":"tools/call","params":{"name":"keil_disconnect","arguments":{}}})

p = subprocess.Popen([EXE], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding='utf-8', errors='replace')
out, _ = p.communicate("\n".join(json.dumps(r) for r in reqs) + "\n", timeout=60)
for line in out.splitlines():
    line=line.strip()
    if not line: continue
    try: obj=json.loads(line)
    except: continue
    rid=obj.get("id")
    if rid is None: continue
    res=obj.get("result",{})
    sc=res.get("structuredContent", res)
    if rid==3:
        print("g_led symbol =", sc.get("value"))
    elif rid in idmap:
        pos,sp,addr = idmap[rid]
        if "error" in sc:
            print("%s sp=0x%02X addr=0x%06X -> ERR %s" % (pos,sp,addr,sc["error"][:40]))
        else:
            print("%s sp=0x%02X addr=0x%06X -> hex=%s returned=%s" % (pos,sp,addr,sc.get("hex"),sc.get("bytes_returned")))
