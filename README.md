# keil-uvsc-mcp

**让 AI 通过 MCP 协议直接驱动 Keil µVision 调试 8051/C51 单片机。**
**An MCP server that lets AI assistants drive Keil µVision (via the UVSOCK/UVSC interface) to debug 8051/C51 targets.**

[中文](#中文) · [English](#english)

---

## 中文

AI 编程助手（Claude Code / Claude Desktop / Kimi 等 MCP 客户端）连接本服务器后，可以在 Keil 调试会话中：**编译工程、进入/退出 Debug、运行控制（run/stop/reset/单步）、设断点、读变量/内存/表达式、写内存**——全程不用切回 Keil 界面。

共 **22 个 MCP 工具**，单文件 EXE，零第三方依赖，开箱即用。

> ⚠️ 本项目与 Arm / Keil 官方无关，仅为基于 µVision 公开 UVSOCK/UVSC 接口的个人开源工具。

### 为什么做这个

8051/C51 没有 pyOCD / OpenOCD 那样的开源调试生态，Keil 是事实标准，但 Keil 没有 AI 接口。本工具通过 µVision 原生的 **UVSOCK socket + UVSC DLL** 接口 attach 到调试会话，把 Keil 的调试能力以标准 MCP 协议暴露给 AI。支持 **纯 Simulator 仿真（无需硬件）**，也支持 GLink+ / ULINK 等在线调试器。

### 与同类项目对比

| | keil-uvsc-mcp（本项目） | debug-keil-uvsc | Keil_mcp | McuBuddy | mcs51-mcp |
|---|---|---|---|---|---|
| 标准 MCP server | ✅ | ❌（CLI + Codex skill） | ✅ | ✅ | ✅ |
| 8051/C51 支持 | ✅ 专注 | ❌（面向 MDK/ARM） | ❌（MDK） | ❌（MDK） | ✅（但用 SDCC，不走 Keil） |
| Keil Simulator 仿真 | ✅ | ❌（硬件调试） | – | ❌ | ❌ |
| 读变量/内存/表达式 | ✅ | ✅ | ✅（只读） | ✅ | – |
| 写内存/寄存器 | ✅ | ✅ | ❌ | ✅ | – |
| 运行控制 + 断点 | ✅ | ✅ | ❌ | ✅ | – |
| 编译工程 | ✅ | ✅（部分） | ❌ | ✅ | ✅（SDCC） |
| 实测验证文档 | ✅ 10 个冒烟脚本 + 踩坑记录 | 部分 | – | – | – |

### 快速开始

**前置条件**：Windows + Keil µVision v5（`UV4\UVSC64.dll` 存在）；.NET Framework 4.x（Win10/11 自带）。

1. **开启 UVSOCK**（一次性，必须走 GUI）：µVision → Edit → Configuration → Other → UVSOCK 勾选 Enable，端口 `4823`。
2. **注册 MCP**（以 Claude Code 为例）：

   ```bash
   claude mcp add keil-uvsc "<解压目录>\bin\KeilUvscMcp.exe"
   ```

   Claude Desktop 配置模板见 `mcp-config/`。
3. **每次调试会话的顺序**：µVision 打开工程 → 进 Debug（Ctrl+F5，Simulator 无需硬件）→ AI 侧 `keil_connect(mode=attach, port=4823)`。

然后就可以对 AI 说：「编译一下」「在 main 下断点跑起来」「读 g_tick 的值」。

### 工具清单（22 个）

| 分组 | 工具 |
|---|---|
| 连接 | `keil_connect` `keil_disconnect` `keil_reconnect` `keil_uvsc_info` |
| 工程 | `keil_load_project` `keil_build` `keil_show_window` |
| Debug 会话 | `keil_enter_debug` `keil_exit_debug` |
| 运行控制 | `keil_run` `keil_stop` `keil_reset` `keil_get_status` `keil_step_instruction` `keil_step_over` |
| 断点 | `keil_set_breakpoint` `keil_clear_breakpoint` |
| 读取 | `keil_read_variable` `keil_read_variables`（批量≤64） `keil_eval_expression` `keil_read_memory` |
| 写入 | `keil_write_memory`（危险操作，改 RAM/寄存器） |

**无烧录功能**（UVSOCK 不提供）。烧录请搭配 [stcgal](https://github.com/grigorig/stcgal) / [stc8prog](https://github.com/IOsetting/stc8prog) 等 ISP 工具。

### 验证

`verify/` 下有 10 个 Python 冒烟脚本，JSON-RPC over stdio 直连 EXE，不依赖任何 MCP 客户端：

```bash
cd verify
python verify_read.py     # 读变量（需 Keil 已在 Debug）
python verify_bp.py       # 断点
python verify_runctl.py   # 运行控制
python verify_build.py    # 编译
```

### 实测踩坑（别处看不到的）

- **8051 地址编码 `0xSSSS_AAAA`**：高 16 位是空间选择（CODE=`0xFF00`、XDATA=`0x0100`），裸地址默认落 CODE 空间。稳妥做法：先 `keil_eval_expression &变量名` 拿带前缀的地址再读写。
- **GLink+ 同时只能存在 1 个断点**，下新断点前必须先清旧的，否则 UVSC 超时卡死整个 µVision。
- **GLink+ 运行中不能读变量/设断点**，须先 halt。
- `keil_exit_debug` 会崩掉 UV4 进程——要重编译请直接关掉 µVision 以编辑模式重开。

完整记录见 [docs/验证记录.md](docs/验证记录.md) 与 [部署指南.md](部署指南.md)。

### 实战案例

QI 无线充电工程「断开错误码」抓取：在错误处理唯一汇聚点下单断点 → AI 轮询 `keil_get_status` 等命中 → 读错误码与 EPT 包解析断开原因。详见 [部署指南.md](部署指南.md) 第 8 节。

### 重编译

```cmd
C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe /nologo /platform:x64 /out:bin\KeilUvscMcp.exe src\Program.cs
```

### 目录

```
bin/KeilUvscMcp.exe   MCP 服务器（.NET Framework x64 单文件）
src/Program.cs        全部源码（单文件）
src/UVSC_C.h UVSOCK.h Keil UVSC/UVSOCK 协议头文件（参考）
mcp-config/           Claude Code / Claude Desktop 注册模板
verify/               10 个冒烟验证脚本
docs/                 调研报告 / 实现方案 / SOP / 验证记录
DemoC51/              C51 演示工程（Simulator 冒烟目标）
```

### License

[MIT](LICENSE)

---

## English

An MCP (Model Context Protocol) server that attaches to a live **Keil µVision** debug session through the native **UVSOCK socket + UVSC DLL** interface and exposes it to AI assistants (Claude Code, Claude Desktop, Kimi, any MCP client) as **22 structured tools**: build the project, enter/exit debug, run control (run/halt/reset/step), breakpoints, and read/write variables, expressions and target memory — focused on **8051/C51** targets, with support for both the **Keil Simulator (no hardware needed)** and in-circuit debuggers such as GLink+ / ULINK.

> ⚠️ Not affiliated with Arm / Keil. Built on the documented UVSOCK/UVSC interface of µVision.

### Quick start

1. Enable UVSOCK once in µVision: **Edit → Configuration → Other → UVSOCK → Enable, port `4823`**.
2. Register the server, e.g. for Claude Code:

   ```bash
   claude mcp add keil-uvsc "<install-dir>\bin\KeilUvscMcp.exe"
   ```

   Templates for Claude Desktop live in `mcp-config/`.
3. Session order matters: open the project in µVision → enter Debug (Ctrl+F5) → then call `keil_connect(mode=attach, port=4823)` from the AI side.

**No flashing support** (UVSOCK does not expose it) — pair with an ISP tool such as [stcgal](https://github.com/grigorig/stcgal). Windows + Keil µVision v5 only. See [部署指南.md](部署指南.md) (Chinese) for the full deployment guide, hard-won pitfalls (8051 `0xSSSS_AAAA` address-space encoding, GLink+ single-breakpoint limit, `exit_debug` crash), a real-world Qi wireless-charger debugging case, and smoke-test scripts in `verify/`.

### License

[MIT](LICENSE)
