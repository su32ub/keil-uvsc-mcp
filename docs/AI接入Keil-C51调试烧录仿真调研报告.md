# AI 接入 Keil C51 对 MCU 进行调试 / 烧录 / 仿真 —— 调研报告

> 调研时间：2026-09-10。结论先行：**"AI 写代码 → 命令行编译 → 命令行烧录 → 串口反馈"的闭环在 8051 上已可落地；但 AI 在 8051 上做真机断点/单步级调试目前没有标准化通道，只有半自动化的绕行方案。**

---

## 一、总体结论（TL;DR）

1. **Keil C51 的全部构建与烧录动作都可以脱离 IDE 用命令行驱动**，这是 AI 接入的天然抓手：`UV4.exe -b`（编译）、`-f`（烧录）、`-d` + INI 脚本（调试/仿真批处理），退出码和日志均可机器解析。
2. **烧录自动化已完全解决**：STC 用开源 `stcgal`（协议逆向实现，★788），SiLabs / Nuvoton / AT89 各有官方命令行工具。
3. **AI 与硬件之间的"反馈通道"在 8051 上实际只能走串口**（printf / 命令回环），因为 8051 没有 GDB/OpenOCD 生态；真机断点调试只能通过 UVSOCK/UVSC 接口半自动化。
4. **已有现成开源参考实现**：`xhw949/keil_mcp`（经 UVSOCK 让 AI 读 Keil 调试器变量）、`dsh-keil-mcp`、`McuBuddy`、`Serial-Agent`（串口+编译+烧录闭环）。但均为 2025–2026 年新出现的早期项目（星标 2–37），需自行验证稳定性。
5. **Keil 官方零 AI 投入**：MDK v6 的 AI 路径是转向 VS Code + GitHub Copilot；Arm 官方 MCP Server 只面向服务器迁移，不覆盖 MCU。

---

## 二、Keil C51 自动化接口（AI 接入的底层通道）

### 2.1 µVision 命令行（UV4.exe）—— 完全文档化

语法：`UV4 [command] [projectfile] [options]`

| 命令 | 作用 | 示例 |
|---|---|---|
| `-b` | 增量构建后退出 | `UV4 -b PROJECT1.uvprojx` |
| `-r` | 全量重编译后退出 | `UV4 -r PROJECT1.uvprojx -t"Simulator"` |
| `-f` | 下载程序到 Flash 后退出（前提是工程 Options → Utilities 已配好下载驱动） | `UV4 -f PROJECT1.uvprojx -t"Flash"` |
| `-d` | 直接进入调试模式（配合 INI 初始化脚本可无人值守；脚本内用 `EXIT` 退出） | `UV4 -d PROJECT1.uvprojx` |
| `-j0` | 隐藏 GUI、抑制消息（批处理/CI 必加） | `start /wait UV4.exe -j0 -b x.uvprojx -o build.log` |
| `-o <file>` | 输出日志到文件 | 见上 |
| `-t <name>` | 指定 Target | 见上 |
| `-i` / `-n` | 按 XML/设备名创建更新工程（可脚本化建工程） | `UV4 MyProj.uvprojx -i MyImport.xml -n Device -t Flash` |

退出码（ERRORLEVEL）：0=成功无警告；1=仅警告；2=错误；3=致命错误；11=无法打开工程；12=设备名不在数据库；41=无法创建日志文件等。

注意事项：
- UV4 是 Windows GUI 程序，批处理中要用 `start /wait` 才能拿到 ERRORLEVEL。
- Flex 浮动许可可能被占满，自动化脚本需检测日志中的 "All Flex licenses are in use" 并重试。
- 命令行不能直接指定宏定义/编译选项，要靠多 Target 或直接改 `.uvprojx`（本质是 XML，AI 可直接编辑）。

来源：Keil 官方文档 uv4cl_commandline（https://www.keil.com/support/man/docs/uv4cl/uv4cl_commandline.htm ）。

### 2.2 C51 工具链纯命令行构建（完全不依赖 IDE）

工具链位于 `C:\Keil_v5\C51\BIN\`：

```bat
SET C51INC=C:\Keil_v5\C51\Inc
SET C51LIB=C:\Keil_v5\C51\Lib
C51.EXE  main.c                 :: C 编译 → main.obj（大内存芯片用 CX51）
A51.EXE  startup.a51            :: 汇编（扩展版用 AX51）
BL51.EXE main.obj,startup.obj TO main   :: 链接定位（扩展芯片用 LX51）
OH51.EXE main                   :: 生成 main.hex（Intel Hex-386 用 OHX51）
```

- 每个工具都返回 ERRORLEVEL，批处理可读；多文件用逗号分隔。
- 已验证可在 Linux 下用 Wine 运行（非官方支持）。
- **许可约束**：PK51 是商业许可，免费评估版代码限制约 2KB。

### 2.3 调试自动化接口 —— AI 接入的核心通道

#### (a) 调试脚本（INI 文件 + 类 C 调试函数语言）—— 完全文档化，最实用

- 工程 Debug 选项卡指定 Initialization File（.ini），`UV4 -d` 启动时自动执行，脚本末尾 `EXIT` 即无人值守退出。
- 支持内置函数、用户函数、Signal 函数（后台模拟外部信号）、断点触发函数。
- **VTREG 虚拟寄存器**是脚本驱动外设输入的关键：`PORTx`（端口电平）、`AINx`（ADC 电压）、`SIN/SOUT`（UART）、`CLOCK` 等，命令窗口可直接赋值 `PORT0=0xAA55`。
- 参考：Keil 应用笔记 AN161（完整 8051 仿真脚本实例）。

**这意味着：AI 可以生成 INI 调试脚本 → `UV4 -d` 跑仿真 → 从日志读回变量/外设状态，构成"仿真闭环"。**

#### (b) UVSOCK / UVSC —— 官方支持但半文档化的 TCP/IP 远程控制接口

- µVision ≥3.5 内置，`Edit → Configuration → Other → UVSOCK` 启用并设端口（常用 4823）。
- 第三方程序经 TCP/IP 可**控制和监视 IDE 与调试器**：运行控制、断点、读写变量/内存、加载工程。
- 客户端通过 Keil 安装目录的 UVSC64.dll（头文件 UVSC_C.h）编程；协议文档稀缺，主要靠 DLL 头文件 + 社区逆向。
- **已有 AI 桥接实例**：开源项目 **Keil UVSC MCP**（xhw949/keil_mcp）——stdio MCP server，经 UVSC64.dll 向 AI 暴露读变量、表达式求值、读内存等工具（当前只读，无运行控制/烧录）。前提：Windows x64 + UVSOCK 开启 + 已进入 Debug 会话。
- 学术上也被用于驱动 µVision 8051 仿真器做故障注入实验。

#### (c) AGDI / AGSI —— 面向驱动开发者的文档化 API

- AGDI：µVision 调试器与目标调试驱动（CPU DLL，如 S8051.DLL）之间的接口，可自写硬件调试驱动。
- AGSI：仿真侧 DLL 接口，可实现自定义外设模型/协同仿真。
- 适合深度定制，不适合快速搭 AI 闭环。

---

## 三、烧录自动化（8051 各家族）

| 目标 | 工具 | 命令行示例 | 说明 |
|---|---|---|---|
| **STC** 89/90/10/11/12/15/8/32 | 开源 **stcgal**（官方 STC-ISP 无 CLI） | `pip install stcgal` → `stcgal -P stc89 -p COM3 firmware.hex` | UART/USB BSL，协议逆向实现，★788，支持选项字节/EEPROM/DTR 自动冷启动复位；跨平台。新型号用社区分支 area-8051/stcgal-patched |
| **Silicon Labs** C8051F / EFM8 | 官方 Flash Programming Utility（装 GUI 时可选装 **32-bit Command-line** 版） | 官方 CLI + USB Debug Adapter | 8051 阵营中可编程性最好的一支；Linux 无官方工具 |
| **Nuvoton** N76E003 / MS51 / ML51 | 官方 **Nu-Link Command Tool**（CLI）；开源 nuvoprog / NUMicro-8051-prog | Nu-Link CLI 脚本化 ICP 烧录 | 开源方案甚至可用树莓派 GPIO / Arduino 桥烧录 |
| **Atmel/Microchip AT89**（RB2/RC2/RD2/ED2） | 官方 FLIP 附带 **batchisp**（DOS 命令行版） | `batchisp -device AT89C51ED2 -hardware usb -operation erase f loadbuffer firmware.hex program verify start reset 0` | 走 RS232/USB DFU 引导 |

---

## 四、仿真方案

### 4.1 µVision 内置仿真器（配 AI 最现实的路径）

- 周期精确仿真 8051 内核 + 片上外设（I/O、UART、I²C、SPI、A/D、中断等，取决于器件数据库中的型号建模）；含逻辑分析仪、代码覆盖率。
- 局限：STC 等 1T 增强型 8051 通常只有内核级仿真；无 GUI 批量仿真靠 `-d` + INI 脚本。
- **AI 接入方式**：AI 生成测试用的 INI 脚本（赋值 VTREG、设断点、printf 变量）→ `UV4 -j0 -d` 批处理运行 → 解析日志。这是当前唯一不需要额外硬件的"AI 仿真调试闭环"。

### 4.2 第三方 8051 仿真器

- **Proteus VSM**：周期精确、完整电路协同仿真，可与 Keil 联调；商业软件，无 AI/自动化接口。
- **MCU 8051 IDE**：开源跨平台，内置汇编器+仿真器。
- **EdSim51**：免费教学向。
- **SDCC 自带 ucsim/s51**：命令行 8051 仿真器，最适合 CI 无 GUI 跑固件测试（开源、跨平台、天然适合 AI 驱动）。

---

## 五、AI 接入的现有方案与生态（2025–2026）

### 5.1 直接可用的开源项目

| 项目 | 链接 | 状态 | 能力 |
|---|---|---|---|
| **keil_mcp**（xhw949） | github.com/xhw949/keil_mcp（lobehub 收录） | 早期 | 经 UVSOCK/UVSC64.dll 让 AI 读 Keil 调试器的变量/表达式/内存（只读）——**与 C51 最相关的现成实现** |
| **keil-mcp-server / dsh-keil-mcp**（ZMC1011） | PyPI: `keil-mcp-server` | ★2，活跃 | Keil 工程的 edit→build→flash→debug→fix 闭环：解析 UV4 编译错误、解释错误码、烧录、UV4 -d + ini 脚本调试（主要面向 STM32/MDK，思路完全适用于 C51） |
| **McuBuddy**（cunjun） | github.com/cunjun/McuBuddy | ★6，Alpha | 通用 MCU 调试 MCP：Keil UV4 构建 + 多探针后端 + 符号/外设寄存器 + 串口/RTT 日志 |
| **Serial-Agent**（Rance-OwO） | github.com/Rance-OwO/Serial-Agent | ★37 | VS Code 插件 + MCP + Skill：串口监视 + 触发 Keil 编译 + JLink/DAP 烧录，**串口反馈闭环——8051 场景最贴近的模式** |
| **jlink-mcp / openocd-mcp** | github.com/Klievan/jlink-mcp 等 | ★30/★4 | ARM Cortex-M 侧最成熟，不适用于 8051，但架构可作参考 |

### 5.2 官方动向（诚实结论）

- **µVision 没有任何内置 AI 功能**；MDK v6 的 AI 路径 = VS Code（Keil Studio 扩展）+ GitHub Copilot。
- Arm 官方 MCP Server（2025-10 发布）面向服务器/云迁移，**不覆盖 MCU 调试/Keil**。
- C51 产品线处于维护状态，无 AI 投入。

### 5.3 中文社区实践

- CSDN/B 站已有"VS Code + EIDE/Keil Assistant + 通义灵码/Cursor/Trae"开发 51/STM32 的大量教程（AI 只写代码，编译烧录靠插件按钮）。
- 2026-09 出现首篇完整案例：《用 TraeWork 智能体高效开发 STC 单片机全流程实践》——"Keil C51 编译 HEX → STC-ISP/stcgal 烧录 → 串口/示波器验证"交给 AI 智能体跑。
- TRAE 官方论坛有用户提议"物理 MCP bridge"打通"编译→烧录→串口反馈→改代码"闭环，反映社区共识痛点：**AI 写代码与真实硬件"两张皮"**。

---

## 六、推荐的落地架构（按 8051 现实约束设计）

```
┌─────────────┐   编辑代码 (.c/.h)     ┌──────────────┐
│             │ ────────────────────▶ │   C51 源码    │
│   AI Agent  │                       └──────┬───────┘
│ (Claude/    │                              │ UV4 -j0 -b（或 C51/BL51/OH51）
│  Kimi 等,   │ ◀────── 解析 build.log ──────┤
│  经 MCP /   │   （错误码→解释→自动修复）     ▼
│  Shell 工具)│                          firmware.hex
│             │                              │ stcgal / batchisp / Nu-Link CLI
│             │ ◀──── 串口日志（printf/     ▼
│             │       命令回环）反馈        目标 MCU
│             │                              ▲
│             │ ── UV4 -d + INI 脚本 ────────┤（仿真/在线调试，
└─────────────┘    或 UVSOCK 读变量           │ 半自动化）
                   （keil_mcp 模式）          └─ STC8H/STC32G USB 仿真
                                              或 SiLabs C2/JTAG 适配器
```

**分层建议**：

1. **编译层**（成熟，立即可用）：AI 调 `UV4 -j0 -b proj.uvprojx -o build.log`，解析 ERRORLEVEL + 日志中的 `file(line): error Cxxx`，自动修代码迭代。
2. **烧录层**（成熟）：STC 用 stcgal（支持 DTR 自动冷启动，天然无人值守）；SiLabs/Nuvoton/AT89 用各自官方 CLI。
3. **仿真层**（可行）：AI 生成 INI 调试脚本（VTREG 激励 + 断点 + printf），`UV4 -d` 批处理跑仿真，读日志验证逻辑——**不依赖硬件的回归测试**。
4. **真机反馈层**（现实主力）：固件里做串口 shell / printf 日志，AI 读串口输出判断行为（Serial-Agent 模式）。
5. **真机调试层**（实验性）：keil_mcp（UVSOCK）读变量；STC8H/STC32G 的 USB 在线仿真、SiLabs C8051F 的 C2 调试都走 Keil 私有驱动，只能经 UV4 -d + 脚本半自动化。

**诚实的能力边界**：
- ❌ 8051 无 GDB/OpenOCD 支持，AI **无法**像在 ARM 上那样做标准化断点/单步/变量级真机调试。
- ❌ STC ISP 协议未公开，依赖逆向项目（stcgal），新芯片支持滞后。
- ❌ Proteus 无 AI 接口；Keil 官方无 AI 投入。
- ⚠️ 现有 MCP 项目均为早期（星标 <40），生产使用前需自行验证。

---

## 七、主要参考来源

- Keil µVision 命令行官方文档：https://www.keil.com/support/man/docs/uv4cl/uv4cl_commandline.htm
- Keil 应用笔记 AN161（8051 仿真脚本）：https://www.keil.com/appnotes/files/apnt_161.pdf
- stcgal：https://github.com/grigorig/stcgal ，https://pypi.org/project/stcgal/
- keil_mcp（UVSOCK AI 桥）：https://lobehub.com/mcp/xhw949-keil_mcp
- keil-mcp-server：https://glama.ai/mcp/servers/ZMC1011/dsh-keil-mcp
- McuBuddy：https://github.com/cunjun/McuBuddy
- Serial-Agent：https://github.com/Rance-OwO/Serial-Agent
- jlink-mcp：https://github.com/Klievan/jlink-mcp ；openocd-mcp：https://github.com/microhenrio/openocd-mcp
- Arm MCP Server 官方博客：https://newsroom.arm.com/blog/arm-mcp-server
- TRAE 论坛"物理 MCP bridge"提议：https://forum.trae.cn/t/topic/178794
- C51 命令行实践（国芯论坛）：https://www.stcaimcu.com/thread-8320-1-1.html
