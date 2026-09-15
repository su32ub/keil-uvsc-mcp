# Keil 侧 UVSOCK 开启与调试桥使用 SOP

> 目标：让 AI（经 `bin\KeilUvscMcp.exe`）能 attach 到本机 Keil µVision，对 C51 目标做**只读**调试（读变量/表达式/内存）。
> 每次想让 AI 读调试数据前，按本页把 Keil 侧备好。

---

## 一次性配置（只需做一次）

1. 打开 µVision（UV4.exe）。
2. 菜单：**Edit → Configuration → Other** 选项卡。
3. 找到 **UVSOCK** 区，勾选 **Enable**，**Port 填 `4823`**，确定。
   - 该设置写入 `HKCU\Software\Keil\µVision5`（值名 `SocketPort`），重启 µVision 后仍保留。

> ⚠️ 注意：仅靠写注册表值（`SocketPort=4823`）**不足以**启动监听——"Enable" 布尔位由该对话框单独写入。所以这一步必须走一次 GUI。

---

## 每次调试会话的准备（顺序不能乱）

1. **确认 UVSOCK 已开**（上面一次性配置；可用 `netstat -ano | findstr 4823` 在进 Debug 后应有 LISTENING）。
2. 打开目标工程（如 `DemoC51\DemoC51.uvproj`）。
3. **进入 Debug**：`Debug → Start/Stop Debug Session`（或工具栏放大镜 / Ctrl+F5）。
   - Target 为 Simulator 时无需硬件。
4. （可选）点 **Run**（F5）让目标跑起来，便于观察活数据。
5. 现在让 AI attach：调 `keil_connect`（mode=attach, port=4823）。

> 顺序要点：**先开 UVSOCK → 再进 Debug → 最后 attach**。UVSOCK 服务在进 Debug 后才开始监听 4823。

---

## 验证连通（命令行冒烟）

在 `<工作区>` 下：

```bash
printf '%s\n' \
 '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"s","version":"0"}}}' \
 '{"jsonrpc":"2.0","method":"notifications/initialized"}' \
 '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"keil_connect","arguments":{"mode":"attach","port":4823}}}' \
 | ./bin/KeilUvscMcp.exe
```

- 看到 `"id":2` 返回无 `error`（含 `connection_handle`）即 attach 成功。
- 若返回 `UVSC_OpenConnection failed with UVSC status 1`：UVSOCK 未开 / 未进 Debug / 端口不符。

---

## 退出

- AI 侧调 `keil_disconnect` 仅断开客户端通道，**不会**退出 Debug、不改工程。
- 在 µVision 里 `Debug → Start/Stop Debug Session` 退出调试。

---

## 安全边界

本桥为**只读**：不暴露写内存 / 复位 / 运行控制 / 编译 / 烧录。
读到的失败值**不可**当作 0（可能是符号未解析或目标在跑）。需要多变量同一时刻快照时，先在断点处停下目标。
