# 本机 Keil C51 AI 调试桥（keil_mcp 式）搭建 Plan

> 版本 v1 · 2026-09-10 · 本文件仅为计划，**未开始实施**。确认后按 Phase 顺序执行。

---

## 0. 目标与范围

在本机搭建一个 MCP 调试桥，让 AI（Kimi / Claude 等 MCP 客户端）能连接本机 Keil µVision 的调试器，对 C51（8051）目标进行**变量读取、表达式求值、内存读取**（只读安全基线），并按需扩展到**运行控制、断点、烧录**。

**范围内**：C51 工程（.uvproj/.uvprojx）、µVision 软件仿真器（首选验证环境，无需硬件）、后续可接真机调试驱动（STC USB 仿真 / SiLabs C2 等）。
**范围外**：修改 Keil 本身、跨机远程调试（UVSC 官方仅支持本机回环）、Proteus 联动。

---

## 1. 本机环境核查结果（已实测）

| 前提 | 状态 | 事实 |
|---|---|---|
| Keil µVision | ✅ | `C:\Keil_v5\UV4\UV4.exe` 存在；另有旧版 `C:\Keil`（UV2/UV3） |
| UVSC 接口 DLL | ✅ | `C:\Keil_v5\UV4\UVSC64.dll`（246 KB，2017-02 版）+ `UVSC.dll`（32 位备用） |
| C51 工具链 | ✅ | `C:\Keil_v5\C51\BIN\` 下 C51.exe / CX51 / BL51 / LX51 / OH51 齐全 |
| C# 编译器 | ✅ | Windows 自带 `csc.exe` 4.8.9221.0（Framework64\v4.0.30319），零依赖可编译 |
| 参考实现源码 | ✅ | xhw949/keil_mcp 的 `Program.cs`（约 1464 行）已下载到工作区，可直接编译 |
| Keil 侧 UVSOCK 开关 | ⏳ 待手动 | 需在 µVision GUI 勾选：`Edit → Configuration → Other → UVSOCK → Enable`，端口 4823 |
| 验证用 C51 工程 | ⏳ 待创建 | 将建一个最小 C51 工程（LED 闪烁 + 全局计数器变量），Target 用 **Simulator**，保证无硬件也能验证全链路 |

**结论：所有硬性前提已满足，无任何需采购/安装的组件。**

---

## 2. 技术路线选型

| 方案 | 说明 | 评估 |
|---|---|---|
| **A. 直接编译参考实现**（xhw949/keil_mcp） | 用本机 csc.exe 编译工作区已有的 `Program.cs`，得到 `KeilUvscMcp.exe` | ✅ **首选**。零外部依赖、零 NuGet；只读语义安全；最快跑通链路 |
| B. 自研 Python（ctypes 加载 UVSC64.dll + MCP SDK） | 重写 | 灵活但工作量大，且要重新踩结构体对齐的坑（见 §4 风险）；作为后续扩展备选 |
| C. 裸 TCP 连 4823 | 不用 DLL | ❌ UVSOCK 协议未公开，逆向成本高，放弃 |

**决策：Phase 0 用方案 A 跑通；Phase 2 起在 Program.cs 基础上增量扩展（仍是单文件 C#，方案 B 仅在需要重写时再评估）。**

架构（与参考实现一致）：

```
MCP 客户端（Kimi / Claude Desktop / Claude Code）
   │ stdio · JSON-RPC（initialize / tools/list / tools/call）
   ▼
KeilUvscMcp.exe（C# 单文件，本机 csc 编译）
   │ P/Invoke（Cdecl），SetDllDirectory(C:\Keil_v5\UV4)
   ▼
UVSC64.dll ──内部 TCP──▶ UV4.exe（需开 UVSOCK，已进入 Debug）
                            ▼
                    S8051.DLL 仿真器 / 硬件调试驱动 → C51 目标
```

---

## 3. 分阶段计划

### Phase 0 — 编译与连通性验证（预计最快，纯本机）

1. 检查 `Program.cs` 与本机 UVSC64.dll（2017 版）的函数兼容性——2017 版 DLL 可能缺少新版 `UVSC_C.h`（V2.23）中的部分导出，先 `dumpbin` 或等效手段列出 DLL 导出函数核对。
2. 用本机 `csc.exe /platform:x64` 编译 → `bin\KeilUvscMcp.exe`。
3. 写 `test-protocol.ps1` 风格的冒烟脚本：向 exe 的 stdin 喂 `initialize` / `tools/list`，确认 JSON-RPC 应答正常。
4. **验收标准**：exe 能启动、能列出不小于 11 个工具、无异常退出。

### Phase 1 — Keil 侧配置 + 仿真器闭环验证（关键里程碑）

1. 建最小 C51 验证工程 `DemoC51.uvproj`：Target = Simulator，源码含易观测的全局变量（如 `unsigned int g_tick` 在定时器中断里递增、`unsigned char g_led`）。
2. 手动在 µVision 勾选 UVSOCK（端口 4823），打开工程并进入 Debug（仿真器）。
3. 通过桥依次调用：`keil_uvsc_info` → `keil_connect`(attach, 4823) → `keil_get_status` → `keil_read_variable("g_tick")` → `keil_eval_expression` → `keil_read_memory`（读 P1 口地址 0x90）。
4. 在 µVision 里 Run 几秒后再读 `g_tick`，确认值变化——证明读到的是活的调试数据。
5. **验收标准**：attach 成功、变量/表达式/内存三类读取返回值与 µVision Watch/Memory 窗口显示一致。

### Phase 2 — 注册到 MCP 客户端 + 实用化

1. 生成客户端注册配置（stdio）：
   - Claude Desktop / Claude Code：`claude_desktop_config.json` / `.mcp.json` 加 `"command": "<工作区>\\bin\\KeilUvscMcp.exe"`；
   - Kimi 侧：评估用 plugin-builder 包装成本地插件（可选，单独立项）。
2. 固化 Keil 侧准备步骤为一页 SOP（开 UVSOCK → 开工程 → 进 Debug → 再让 AI attach）。
3. **验收标准**：在真实 MCP 客户端对话里让 AI 读出 `g_tick` 并解释含义。

### Phase 3 —（可选，确认后另行评估）扩展到调试/烧录闭环

参考实现刻意只做只读。底层 UVSC64.dll 实际还支持：`UVSC_DBG_ENTER/EXIT`（进/出调试）、`UVSC_DBG_MEM_WRITE`、运行控制、断点、`UVSC_PRJ_BUILD`（编译）、`UVSC_PRJ_FLASH_DOWNLOAD`（烧录）。
- 逐项增加 DllImport + 手工组包，**每项加人工确认门槛**（写内存/烧录属危险操作）。
- 烧录另一路更稳的方案是 `UV4 -f` 命令行或 stcgal（STC 真机），作为对照路径保留。
- **验收标准**：AI 能完成「编译 → 烧录 → 进 Debug → 读变量 → halt/run」完整闭环；危险操作均有确认。

---

## 4. 风险与注意事项（复刻时最容易踩的坑）

1. **结构体对齐**：UVSOCK 结构体在 64 位下仍是 4 字节对齐——VSET = TVAL(int 类型 @0 + 8 字节值 @4) + SSTR(int 长度 @12 + 字符串 @16)；AMEM 读内存传长度只算 16 字节头；EXECCMD 的 SSTR 在偏移 32/36。参考实现已踩平，**直接沿用其组包代码，不要自行"优化"**。
2. **表达式无前导空格**：`UVSC_DBG_CALC_EXPRESSION` 遇前导空格会报 `UV_STATUS_PARSE_ERROR`。
3. **端口冲突**：`UVSC_Init` 用 5201–5210 池，刻意避开官方 Tester 的 5101–5110；attach 模式固定连 4823，需与 Keil GUI 设置一致。
4. **2017 版 DLL 兼容性**：本机 UVSC64.dll 较旧，Phase 0 第 1 步必须先核对导出函数；若缺新版函数，只影响 Phase 3 扩展项，不影响只读基线。
5. **客户端数量上限**：UVSC 最多 10 个客户端共享。
6. **许可**：UVSC64.dll 属 Keil 专有、不分发；本方案仅本机调用，无分发问题。
7. **C51 仿真器限制**：只仿真器件数据库内建模的外设；STC 等 1T 增强型仅内核级仿真——验证工程选标准 8051 型号（如 AT89C51/ generic 8052）避开此坑。

---

## 5. 交付物清单（实施完成后）

| 交付物 | 位置 |
|---|---|
| 编译产物 | `<工作区>\bin\KeilUvscMcp.exe` |
| 验证工程 | `<工作区>\DemoC51\`（.uvproj + main.c，Simulator Target） |
| MCP 客户端配置示例 | `<工作区>\mcp-config\claude_desktop_config.json` |
| Keil 侧操作 SOP | `<工作区>\SOP-Keil-UVSOCK.md` |
| 验证记录 | `<工作区>\验证记录.md`（每 Phase 的调用样例与返回值截图/日志） |

---

## 6. 待确认事项（开工前请拍板）

1. **范围**：先做到 Phase 2（只读调试桥 + 客户端可用）即可，还是直接包含 Phase 3（运行控制 + 烧录）？建议先 Phase 0–2。
2. **MCP 客户端**：主要给谁用——Kimi、Claude Desktop、还是 Claude Code？影响 Phase 2 注册方式。
3. **验证工程芯片型号**：默认用通用 8052/AT89C51 仿真；如你有具体目标芯片（STC8H / SiLabs C8051F / N76E003…），仿真外设保真度不同，可指明。
