using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using System.Web.Script.Serialization;
using Microsoft.Win32;

namespace KeilUvscMcp
{
    internal static class NativeMethods
    {
        internal const int Success = 0;
        internal const int VttVoid = 0;
        internal const int VttBit = 1;
        internal const int VttChar = 2;
        internal const int VttUChar = 3;
        internal const int VttInt = 4;
        internal const int VttUInt = 5;
        internal const int VttShort = 6;
        internal const int VttUShort = 7;
        internal const int VttLong = 8;
        internal const int VttULong = 9;
        internal const int VttFloat = 10;
        internal const int VttDouble = 11;
        internal const int VttPointer = 12;
        internal const int VttUnion = 13;
        internal const int VttStruct = 14;
        internal const int VttFunction = 15;
        internal const int VttString = 16;
        internal const int VttEnum = 17;
        internal const int VttField = 18;
        internal const int VttInt64 = 19;
        internal const int VttUInt64 = 20;

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool SetDllDirectory(string lpPathName);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void UVSC_Version(out uint uvscVersion, out uint uvsockVersion);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_Init(int uvMinPort, int uvMaxPort);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_UnInit();

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern int UVSC_OpenConnection(
            string name,
            out int connectionHandle,
            ref int port,
            string uvCommand,
            int uvRunMode,
            IntPtr callback,
            IntPtr callbackCustom,
            string logFileName,
            int logFileAppend,
            IntPtr logCallback);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_CloseConnection(int connectionHandle, int terminate);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_STATUS(int connectionHandle, out int executionStatus);

        // Phase 3: run control. All take only the connection handle and return a status.
        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_START_EXECUTION(int connectionHandle);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_STOP_EXECUTION(int connectionHandle);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_RESET(int connectionHandle);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_STEP_INSTRUCTION(int connectionHandle);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_STEP_HLL(int connectionHandle);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_ENTER(int connectionHandle);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_EXIT(int connectionHandle);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_CREATE_BP(int connectionHandle, IntPtr bkptSet, int bkptSetLen, IntPtr bkptRsp, ref int bkptRspLen);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_ENUMERATE_BP(int connectionHandle, IntPtr bkptRsp, IntPtr bkptIndexes, ref int bkptCount);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_CHANGE_BP(int connectionHandle, IntPtr bkptChg, int bkptChgLen, IntPtr bkptRsp, ref int bkptRspLen);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_MEM_WRITE(int connectionHandle, IntPtr memory, int memoryLength);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_PRJ_BUILD(int connectionHandle, int rebuild);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_GetBuildOutputSize(int connectionHandle, out int buildOutputSize);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern int UVSC_GetBuildOutput(int connectionHandle, IntPtr buildOutput, int buildOutputLength);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_PRJ_LOAD(
            int connectionHandle,
            IntPtr projectData,
            int projectDataLength);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_GEN_SHOW(int connectionHandle);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_CALC_EXPRESSION(
            int connectionHandle,
            IntPtr vset,
            int vsetLength);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_EVAL_EXPRESSION_TO_STR(
            int connectionHandle,
            IntPtr vset,
            int vsetLength);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_MEM_READ(
            int connectionHandle,
            IntPtr memory,
            int memoryLength);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_DBG_EXEC_CMD(
            int connectionHandle,
            IntPtr command,
            int commandLength);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_GetCmdOutputSize(
            int connectionHandle,
            out int commandOutputSize);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int UVSC_GetCmdOutput(
            int connectionHandle,
            IntPtr commandOutput,
            int commandOutputLength);

        [DllImport("UVSC64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern int UVSC_GetLastError(
            int connectionHandle,
            out int messageType,
            out int uvStatus,
            StringBuilder errorText,
            int maxStringLength);
    }

    internal sealed class UvscSession : IDisposable
    {
        // UVSOCK protocol structures use x86-compatible 4-byte packing even in UVSC64.
        // VSET = TVAL (int type + 8-byte value) + SSTR (int length + bytes).
        private const int VsetStringLengthOffset = 12;
        private const int VsetStringOffset = 16;
        private const int VsetBufferSize = 4096;
        // EXECCMD = UINT flags (bEcho is bit 0) + UINT nRes[7] + SSTR.
        // The SDK's SSTR is int nLen + char szStr[256]. These offsets are easy
        // to get wrong from managed code because the public C API uses bitfields.
        private const int ExecCommandStringLengthOffset = 32;
        private const int ExecCommandStringOffset = 36;
        private const int ExecCommandStringCapacity = 256;
        private const int ExecCommandBufferSize = ExecCommandStringOffset + ExecCommandStringCapacity;
        // AMEM (from UVSOCK.h): xU64 nAddr @0, UINT nBytes @8, xU64 ErrAddr @12,
        // UINT nErr @20, BYTE aBytes[] @24. 4-byte packing even in UVSC64.
        // (The reference Program.cs wrongly used 4x4-byte fields / data@16, which is
        //  why raw memory reads returned 0 bytes on this 2017 UVSC64.dll.)
        private const int AmemAddrOffset = 0;    // xU64
        private const int AmemBytesOffset = 8;   // UINT
        private const int AmemErrAddrOffset = 12;// xU64
        private const int AmemErrOffset = 20;    // UINT
        private const int AmemDataOffset = 24;   // aBytes[]
        private const int AmemMaxReadBytes = 256;
        // BKPARM (UVSOCK.h): BKTYPE type@0, UINT count@4, UINT accSize@8,
        //   UINT nExpLen@12, UINT nCmdLen@16, char szBuffer[1024]@20. 4-byte packing.
        private const int BkparmSize = 20 + 1024;
        private const int BktypeExec = 1;
        // BKRSP: type@0, count@4, enabled@8, nTickMark@12, nAddress(xU64)@16, nExpLen@24, szBuffer[512]@28.
        private const int BkrspSize = 28 + 512;
        private const int BkrspTickMarkOffset = 12;
        private const int BkrspAddressOffset = 16;
        // BKCHG: CHG_TYPE type@0, UINT nTickMark@4.
        private const int BkchgSize = 8;
        private const int ChgKillBp = 1;
        // Keep a separate allocator range so the official UVSC Tester can remain open.
        // UVSC accepts a pool of at most UVSC_MAX_CLIENTS (10) ports.
        // Keep this separate from the SDK Tester's default 5101..5110 range.
        private const int DefaultMinPort = 5201;
        private const int DefaultMaxPort = 5210;
        private const int DefaultAttachPort = 4823;

        private static readonly Regex SafeVariablePattern = new Regex(
            @"^[A-Za-z_][A-Za-z0-9_]*(?:(?:\.|->)[A-Za-z_][A-Za-z0-9_]*|\[(?:0[xX][0-9A-Fa-f]+|[0-9]+)\])*$",
            RegexOptions.CultureInvariant | RegexOptions.Compiled);

        private static readonly Regex FunctionCallPattern = new Regex(
            @"[A-Za-z_][A-Za-z0-9_]*\s*\(",
            RegexOptions.CultureInvariant | RegexOptions.Compiled);

        private bool initialized;
        private bool connected;
        private bool autoStarted;
        private int connectionHandle = -1;
        private int port;
        private string keilPath;
        private string dllDirectory;
        private string keilDriverDirectory;
        private string uvscLogFile;
        private bool childDriverPathInjected;
        private string loadedProject;
        private string configurationSource;

        internal UvscSession()
        {
            uvscLogFile = Environment.GetEnvironmentVariable("KEIL_UVSC_LOG_FILE");
            ConfigureKeilPaths(null);
        }        private void ConfigureKeilPaths(string requestedKeilPath)
        {
            string source = null;
            string directory = null;

            if (!String.IsNullOrWhiteSpace(requestedKeilPath))
            {
                directory = ResolveUv4Directory(requestedKeilPath);
                source = directory == null ? "explicit_keil_path_invalid" : "explicit_keil_path";
            }
            if (directory == null)
            {
                string envPath = Environment.GetEnvironmentVariable("KEIL_UV4_PATH");
                directory = ResolveUv4Directory(envPath);
                if (directory != null) { source = "KEIL_UV4_PATH"; }
            }
            if (directory == null)
            {
                string envDirectory = Environment.GetEnvironmentVariable("KEIL_UV4_DIR");
                directory = ResolveUv4Directory(envDirectory);
                if (directory != null) { source = "KEIL_UV4_DIR"; }
            }
            if (directory == null)
            {
                directory = DiscoverUv4DirectoryFromRegistry();
                if (directory != null) { source = "registry"; }
            }
            if (directory == null)
            {
                string programFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
                string programFilesX86 = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86);
                string[] candidates = new string[]
                {
                    @"C:\Keil_v5\UV4", @"C:\Keil\UV4", @"D:\Keil_v5\UV4", @"D:\Keil\UV4", @"D:\tools\keil\UV4",
                    Path.Combine(programFiles, "Keil_v5", "UV4"), Path.Combine(programFilesX86, "Keil_v5", "UV4")
                };
                foreach (string candidate in candidates)
                {
                    directory = ResolveUv4Directory(candidate);
                    if (directory != null) { source = "common_path"; break; }
                }
            }

            dllDirectory = directory;
            configurationSource = source ?? "not_found";
            if (!String.IsNullOrWhiteSpace(requestedKeilPath) && File.Exists(requestedKeilPath))
            {
                keilPath = Path.GetFullPath(requestedKeilPath);
            }
            else
            {
                keilPath = directory == null ? null : Path.Combine(directory, "UV4.exe");
            }

            ConfigureDriverPath();
            if (!String.IsNullOrWhiteSpace(dllDirectory)) { NativeMethods.SetDllDirectory(dllDirectory); }
        }

        private static string ResolveUv4Directory(string candidate)
        {
            if (String.IsNullOrWhiteSpace(candidate)) { return null; }
            try
            {
                candidate = Environment.ExpandEnvironmentVariables(candidate.Trim().Trim('"'));
                if (File.Exists(candidate)) { candidate = Path.GetDirectoryName(Path.GetFullPath(candidate)); }
                if (!Directory.Exists(candidate)) { return null; }
                string parent = Directory.GetParent(candidate) == null ? null : Directory.GetParent(candidate).FullName;
                string[] attempts = new string[]
                {
                    candidate, Path.Combine(candidate, "UV4"), Path.Combine(candidate, "Keil", "UV4"),
                    parent == null ? null : Path.Combine(parent, "UV4")
                };
                foreach (string attempt in attempts)
                {
                    if (!String.IsNullOrWhiteSpace(attempt) && File.Exists(Path.Combine(attempt, "UV4.exe")) && File.Exists(Path.Combine(attempt, "UVSC64.dll")))
                    {
                        return Path.GetFullPath(attempt);
                    }
                }
            }
            catch { }
            return null;
        }

        private static string DiscoverUv4DirectoryFromRegistry()
        {
            string[] keys = new string[] { @"SOFTWARE\WOW6432Node\Keil\Products\MDK", @"SOFTWARE\Keil\Products\MDK" };
            foreach (string keyName in keys)
            {
                try
                {
                    using (RegistryKey key = Registry.LocalMachine.OpenSubKey(keyName))
                    {
                        if (key == null) { continue; }
                        foreach (string valueName in new string[] { "Path", "InstallPath", "Folder" })
                        {
                            string directory = ResolveUv4Directory(Convert.ToString(key.GetValue(valueName), CultureInfo.InvariantCulture));
                            if (directory != null) { return directory; }
                        }
                    }
                }
                catch { }
            }
            return null;
        }
        private void ConfigureDriverPath()
        {
            keilDriverDirectory = null;
            childDriverPathInjected = false;
            if (String.IsNullOrWhiteSpace(dllDirectory)) { return; }
            DirectoryInfo parent = Directory.GetParent(dllDirectory.TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar));
            if (parent == null) { return; }
            keilDriverDirectory = Path.Combine(parent.FullName, "ARM", "Segger");
            if (!Directory.Exists(keilDriverDirectory)) { return; }

            string currentPath = Environment.GetEnvironmentVariable("PATH") ?? String.Empty;
            string[] pathEntries = currentPath.Split(new[] { Path.PathSeparator }, StringSplitOptions.RemoveEmptyEntries);
            bool alreadyPresent = Array.Exists(pathEntries, delegate(string entry)
            {
                return String.Equals(entry.Trim().TrimEnd('\\'), keilDriverDirectory.TrimEnd('\\'), StringComparison.OrdinalIgnoreCase);
            });
            if (!alreadyPresent)
            {
                Environment.SetEnvironmentVariable("PATH", keilDriverDirectory + Path.PathSeparator + currentPath);
                childDriverPathInjected = true;
            }
        }

        private void EnsureUvscAvailable()
        {
            if (String.IsNullOrWhiteSpace(dllDirectory) || !File.Exists(Path.Combine(dllDirectory, "UVSC64.dll")))
            {
                ConfigureKeilPaths(null);
            }
            if (String.IsNullOrWhiteSpace(dllDirectory) || !File.Exists(Path.Combine(dllDirectory, "UVSC64.dll")))
            {
                throw new InvalidOperationException("Keil UVSC64.dll was not found. Set KEIL_UV4_DIR / KEIL_UV4_PATH, or install Keil in a standard location.");
            }
            NativeMethods.SetDllDirectory(dllDirectory);
        }
        internal IDictionary<string, object> GetInfo()
        {
            string uvscVersion = null;
            string uvsockVersion = null;
            string loadError = null;
            try
            {
                EnsureUvscAvailable();
                uint uvsc;
                uint uvsock;
                NativeMethods.UVSC_Version(out uvsc, out uvsock);
                uvscVersion = FormatVersion(uvsc);
                uvsockVersion = FormatVersion(uvsock);
            }
            catch (Exception ex) { loadError = ex.Message; }

            return new Dictionary<string, object>
            {
                { "server_version", Program.ServerVersion },
                { "uvsc_available", loadError == null }, { "uvsc_version", uvscVersion }, { "uvsock_version", uvsockVersion },
                { "configuration_source", configurationSource }, { "dll_directory", dllDirectory },
                { "uvsc_dll_exists", !String.IsNullOrWhiteSpace(dllDirectory) && File.Exists(Path.Combine(dllDirectory, "UVSC64.dll")) },
                { "keil_path", keilPath }, { "keil_path_exists", !String.IsNullOrWhiteSpace(keilPath) && File.Exists(keilPath) },
                { "keil_driver_directory", keilDriverDirectory }, { "uvsc_log_file", uvscLogFile },
                { "child_driver_path_injected", childDriverPathInjected }, { "load_error", loadError },
                { "connected", connected }, { "connection_handle", connected ? (object)connectionHandle : null },
                { "port", connected ? (object)port : null }, { "connection_mode", connected ? (autoStarted ? "auto" : "attach") : null },
                { "project_file", loadedProject },
                { "safety", "read/inspect, run control (run/stop/reset/step), breakpoints, memory write, and project build; no flash download tool is exposed (GLink+ does not support it)" }
            };
        }
        internal IDictionary<string, object> Connect(
            string mode,
            int requestedPort,
            string requestedKeilPath,
            string projectFile)
        {
            if (connected)
            {
                if (autoStarted && !String.IsNullOrWhiteSpace(projectFile))
                {
                    LoadProject(projectFile);
                }
                return ConnectionResult("already_connected");
            }

            if (!String.IsNullOrWhiteSpace(requestedKeilPath))
            {
                ConfigureKeilPaths(requestedKeilPath);
                if (String.IsNullOrWhiteSpace(dllDirectory)) { throw new ArgumentException("keil_path does not point to a Keil UV4 installation containing UV4.exe and UVSC64.dll"); }
            }

            EnsureInitialized();

            string normalizedMode = String.IsNullOrWhiteSpace(mode) ? "attach" : mode.Trim().ToLowerInvariant();
            string uvCommand;
            int selectedPort;

            if (normalizedMode == "auto")
            {
                if (!File.Exists(keilPath))
                {
                    throw new InvalidOperationException("Keil executable not found: " + keilPath);
                }
                autoStarted = true;
                selectedPort = 0;
                uvCommand = keilPath;
            }
            else if (normalizedMode == "attach")
            {
                if (requestedPort == 0) { requestedPort = DefaultAttachPort; }
                if (requestedPort < 1 || requestedPort > 65535)
                {
                    throw new ArgumentException("attach mode requires port in range 1..65535");
                }
                autoStarted = false;
                selectedPort = requestedPort;
                uvCommand = null;
            }
            else
            {
                throw new ArgumentException("mode must be 'auto' or 'attach'");
            }

            int handle;
            int status = NativeMethods.UVSC_OpenConnection(
                "KeilUvscMcp",
                out handle,
                ref selectedPort,
                uvCommand,
                0,
                IntPtr.Zero,
                IntPtr.Zero,
                String.IsNullOrWhiteSpace(uvscLogFile) ? null : uvscLogFile,
                0,
                IntPtr.Zero);

            ThrowOnStatus(status, "UVSC_OpenConnection", handle);

            connectionHandle = handle;
            port = selectedPort;
            connected = true;
            if (autoStarted && !String.IsNullOrWhiteSpace(projectFile))
            {
                LoadProject(projectFile);
            }
            if (autoStarted)
            {
                ShowWindow();
            }
            return ConnectionResult("connected");
        }

        internal IDictionary<string, object> Disconnect()
        {
            return DisconnectCore(false);
        }

        internal IDictionary<string, object> Reconnect(string mode, int requestedPort, string requestedKeilPath, string projectFile)
        {
            if (connected) { DisconnectCore(true); }
            return Connect(mode, requestedPort, requestedKeilPath, projectFile);
        }

        private IDictionary<string, object> DisconnectCore(bool ignoreErrors)
        {
            if (!connected)
            {
                return new Dictionary<string, object> { { "state", "already_disconnected" } };
            }

            int oldHandle = connectionHandle;
            int oldPort = port;
            int status = NativeMethods.UVSC_CloseConnection(connectionHandle, 0);
            connected = false;
            autoStarted = false;
            connectionHandle = -1;
            port = 0;
            loadedProject = null;
            if (!ignoreErrors) { ThrowOnStatus(status, "UVSC_CloseConnection", oldHandle); }
            return new Dictionary<string, object>
            {
                { "state", status == NativeMethods.Success ? "disconnected" : "disconnected_with_close_error" },
                { "previous_connection_handle", oldHandle }, { "previous_port", oldPort }, { "close_status", status }
            };
        }
        internal IDictionary<string, object> ShowWindow()
        {
            EnsureConnected();
            int status = NativeMethods.UVSC_GEN_SHOW(connectionHandle);
            ThrowOnStatus(status, "UVSC_GEN_SHOW", connectionHandle);

            return new Dictionary<string, object>
            {
                { "state", "window_shown" },
                { "connection_handle", connectionHandle },
                { "port", port },
                { "safety", "uVision window visibility changed only; Debug mode, build, download, reset, and execution were not started" }
            };
        }

        internal IDictionary<string, object> LoadProject(string projectFile)
        {
            EnsureConnected();
            if (String.IsNullOrWhiteSpace(projectFile))
            {
                throw new ArgumentException("project_file must not be empty");
            }

            string fullPath = Path.GetFullPath(projectFile);
            if (!File.Exists(fullPath))
            {
                throw new FileNotFoundException("Keil project not found", fullPath);
            }

            string extension = Path.GetExtension(fullPath).ToLowerInvariant();
            if (extension != ".uvprojx" && extension != ".uvproj" && extension != ".uv2" && extension != ".mpw")
            {
                throw new ArgumentException("project_file must be a Keil .uvprojx, .uvproj, .uv2, or .mpw file");
            }

            byte[] pathBytes = Encoding.Default.GetBytes(fullPath);
            int bufferLength = 8 + pathBytes.Length + 1;
            IntPtr buffer = Marshal.AllocHGlobal(bufferLength);
            try
            {
                // PRJDATA = int nLen + int nCode + zero-terminated char szNames[].
                Marshal.WriteInt32(buffer, 0, pathBytes.Length + 1);
                Marshal.WriteInt32(buffer, 4, 0);
                Marshal.Copy(pathBytes, 0, IntPtr.Add(buffer, 8), pathBytes.Length);
                Marshal.WriteByte(buffer, 8 + pathBytes.Length, 0);

                int status = NativeMethods.UVSC_PRJ_LOAD(connectionHandle, buffer, bufferLength);
                ThrowOnStatus(status, "UVSC_PRJ_LOAD", connectionHandle);
                loadedProject = fullPath;
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }

            return new Dictionary<string, object>
            {
                { "state", "project_loaded" },
                { "project_file", loadedProject },
                { "safety", "project opened only; Debug mode, build, download, reset, and execution were not started" },
                { "next_step", "Enter Debug mode manually in Keil when it is safe to use the target, then call keil_get_status or keil_read_variable." }
            };
        }

        internal IDictionary<string, object> GetStatus()
        {
            EnsureConnected();
            int executionStatus;
            int status = NativeMethods.UVSC_DBG_STATUS(connectionHandle, out executionStatus);
            ThrowOnStatus(status, "UVSC_DBG_STATUS", connectionHandle);

            return new Dictionary<string, object>
            {
                { "connected", true },
                { "connection_handle", connectionHandle },
                { "port", port },
                { "project_file", loadedProject },
                { "execution_status_raw", executionStatus },
                { "execution_state", executionStatus == 0 ? "stopped" : "running" }
            };
        }

        // Phase 3: run control. These change target state, so they are NOT read-only.
        private IDictionary<string, object> RunControl(string op, Func<int> invoke)
        {
            EnsureConnected();
            int status = invoke();
            ThrowOnStatus(status, op, connectionHandle);
            // Read back the execution state so the caller sees the result of the action.
            int executionStatus;
            NativeMethods.UVSC_DBG_STATUS(connectionHandle, out executionStatus);
            return new Dictionary<string, object>
            {
                { "operation", op },
                { "ok", true },
                { "execution_state", executionStatus == 0 ? "stopped" : "running" }
            };
        }

        internal IDictionary<string, object> Run()
        {
            return RunControl("UVSC_DBG_START_EXECUTION", () => NativeMethods.UVSC_DBG_START_EXECUTION(connectionHandle));
        }

        internal IDictionary<string, object> Stop()
        {
            return RunControl("UVSC_DBG_STOP_EXECUTION", () => NativeMethods.UVSC_DBG_STOP_EXECUTION(connectionHandle));
        }

        internal IDictionary<string, object> Reset()
        {
            return RunControl("UVSC_DBG_RESET", () => NativeMethods.UVSC_DBG_RESET(connectionHandle));
        }

        internal IDictionary<string, object> StepInstruction()
        {
            return RunControl("UVSC_DBG_STEP_INSTRUCTION", () => NativeMethods.UVSC_DBG_STEP_INSTRUCTION(connectionHandle));
        }

        internal IDictionary<string, object> StepHll()
        {
            return RunControl("UVSC_DBG_STEP_HLL", () => NativeMethods.UVSC_DBG_STEP_HLL(connectionHandle));
        }

        internal IDictionary<string, object> EnterDebug()
        {
            EnsureConnected();
            int status = NativeMethods.UVSC_DBG_ENTER(connectionHandle);
            ThrowOnStatus(status, "UVSC_DBG_ENTER", connectionHandle);
            return new Dictionary<string, object> { { "operation", "UVSC_DBG_ENTER" }, { "ok", true }, { "debug_mode", true } };
        }

        internal IDictionary<string, object> ExitDebug()
        {
            EnsureConnected();
            int status = NativeMethods.UVSC_DBG_EXIT(connectionHandle);
            ThrowOnStatus(status, "UVSC_DBG_EXIT", connectionHandle);
            return new Dictionary<string, object> { { "operation", "UVSC_DBG_EXIT" }, { "ok", true }, { "debug_mode", false } };
        }

        // Phase 3: execution breakpoint. BKPARM holds the expression (symbol or address
        // string); uVision resolves it. Returns the tick-mark that identifies the BP.
        internal IDictionary<string, object> SetBreakpoint(string expression)
        {
            if (String.IsNullOrWhiteSpace(expression))
            {
                throw new ArgumentException("breakpoint expression must not be empty");
            }
            EnsureConnected();

            byte[] exprBytes = Encoding.ASCII.GetBytes(expression);
            if (exprBytes.Length + 1 > 1024)
            {
                throw new ArgumentException("breakpoint expression is too long");
            }

            IntPtr parm = Marshal.AllocHGlobal(BkparmSize);
            IntPtr rsp = Marshal.AllocHGlobal(BkrspSize);
            try
            {
                Marshal.Copy(new byte[BkparmSize], 0, parm, BkparmSize);
                Marshal.Copy(new byte[BkrspSize], 0, rsp, BkrspSize);

                Marshal.WriteInt32(parm, 0, BktypeExec);                 // type = EXEC
                Marshal.WriteInt32(parm, 4, 1);                          // count = 1 (break every hit)
                Marshal.WriteInt32(parm, 8, 0);                          // accSize = 0 for exec
                Marshal.WriteInt32(parm, 12, exprBytes.Length + 1);      // nExpLen incl NUL
                Marshal.WriteInt32(parm, 16, 0);                         // nCmdLen = 0
                Marshal.Copy(exprBytes, 0, IntPtr.Add(parm, 20), exprBytes.Length);
                Marshal.WriteByte(parm, 20 + exprBytes.Length, 0);

                int rspLen = BkrspSize;
                int status = NativeMethods.UVSC_DBG_CREATE_BP(connectionHandle, parm, BkparmSize, rsp, ref rspLen);
                ThrowOnStatus(status, "UVSC_DBG_CREATE_BP", connectionHandle);

                int tickMark = Marshal.ReadInt32(rsp, BkrspTickMarkOffset);
                long address = Marshal.ReadInt64(rsp, BkrspAddressOffset);
                return new Dictionary<string, object>
                {
                    { "operation", "UVSC_DBG_CREATE_BP" },
                    { "ok", true },
                    { "expression", expression },
                    { "tick_mark", tickMark },
                    { "address", String.Format(CultureInfo.InvariantCulture, "0x{0:X}", address) }
                };
            }
            finally
            {
                Marshal.FreeHGlobal(parm);
                Marshal.FreeHGlobal(rsp);
            }
        }

        internal IDictionary<string, object> ClearBreakpoint(int tickMark)
        {
            EnsureConnected();
            IntPtr chg = Marshal.AllocHGlobal(BkchgSize);
            IntPtr rsp = Marshal.AllocHGlobal(BkrspSize);
            try
            {
                Marshal.Copy(new byte[BkchgSize], 0, chg, BkchgSize);
                Marshal.Copy(new byte[BkrspSize], 0, rsp, BkrspSize);
                Marshal.WriteInt32(chg, 0, ChgKillBp);
                Marshal.WriteInt32(chg, 4, tickMark);
                int rspLen = BkrspSize;
                int status = NativeMethods.UVSC_DBG_CHANGE_BP(connectionHandle, chg, BkchgSize, rsp, ref rspLen);
                ThrowOnStatus(status, "UVSC_DBG_CHANGE_BP", connectionHandle);
                return new Dictionary<string, object>
                {
                    { "operation", "UVSC_DBG_CHANGE_BP(KILL)" },
                    { "ok", true },
                    { "tick_mark", tickMark }
                };
            }
            finally
            {
                Marshal.FreeHGlobal(chg);
                Marshal.FreeHGlobal(rsp);
            }
        }

        // Phase 3: build. PRJ_BUILD blocks until the build finishes. Precondition: a project
        // is loaded and uVision is NOT in Debug mode (exit Debug first or the call fails).
        internal IDictionary<string, object> Build(bool rebuild)
        {
            EnsureConnected();
            int status = NativeMethods.UVSC_PRJ_BUILD(connectionHandle, rebuild ? 1 : 0);
            ThrowOnStatus(status, "UVSC_PRJ_BUILD", connectionHandle);

            // Pull the build log so the caller can parse errors/warnings.
            string output = "";
            int size;
            int sizeStatus = NativeMethods.UVSC_GetBuildOutputSize(connectionHandle, out size);
            if (sizeStatus == 0 && size > 0 && size <= 4 * 1024 * 1024)
            {
                IntPtr buf = Marshal.AllocHGlobal(size);
                try
                {
                    Marshal.Copy(new byte[size], 0, buf, size);
                    if (NativeMethods.UVSC_GetBuildOutput(connectionHandle, buf, size) == 0)
                    {
                        int textLength = FindNullTerminator(buf, 0, size);
                        byte[] bytes = new byte[textLength];
                        Marshal.Copy(buf, bytes, 0, bytes.Length);
                        output = Encoding.Default.GetString(bytes);
                    }
                }
                finally
                {
                    Marshal.FreeHGlobal(buf);
                }
            }

            int errorCount = CountOccurrences(output, " error ");
            int warningCount = CountOccurrences(output, " warning ");
            return new Dictionary<string, object>
            {
                { "operation", rebuild ? "rebuild" : "build" },
                { "ok", true },
                { "errors", errorCount },
                { "warnings", warningCount },
                { "output", output }
            };
        }

        private static int CountOccurrences(string haystack, string needle)
        {
            if (String.IsNullOrEmpty(haystack) || String.IsNullOrEmpty(needle))
            {
                return 0;
            }
            int count = 0;
            int idx = 0;
            while ((idx = haystack.IndexOf(needle, idx, StringComparison.OrdinalIgnoreCase)) >= 0)
            {
                count++;
                idx += needle.Length;
            }
            return count;
        }

        internal IDictionary<string, object> ReadVariable(string expression)
        {
            ValidateVariable(expression);
            EnsureConnected();

            // The expression is sent without any leading space. Earlier builds
            // (0.1.7) prepended a space based on a misreading of the official
            // Tester's log, which caused uVision to return UV_STATUS_PARSE_ERROR
            // ("Error in expression") for an otherwise valid symbol path.
            byte[] expressionBytes = Encoding.ASCII.GetBytes(expression);
            if (expressionBytes.Length + 1 > VsetBufferSize - VsetStringOffset)
            {
                throw new ArgumentException("variable expression is too long");
            }

            object value;
            int valueType;
            int status;
            IntPtr buffer = Marshal.AllocHGlobal(VsetBufferSize);
            try
            {
                Marshal.Copy(new byte[VsetBufferSize], 0, buffer, VsetBufferSize);
                Marshal.WriteInt32(buffer, 0, NativeMethods.VttVoid);
                Marshal.WriteInt64(buffer, 4, 0L);
                Marshal.WriteInt32(buffer, VsetStringLengthOffset, expressionBytes.Length);
                Marshal.Copy(
                    expressionBytes,
                    0,
                    IntPtr.Add(buffer, VsetStringOffset),
                    expressionBytes.Length);
                Marshal.WriteByte(buffer, VsetStringOffset + expressionBytes.Length, 0);

                status = NativeMethods.UVSC_DBG_CALC_EXPRESSION(
                    connectionHandle,
                    buffer,
                    VsetStringOffset + expressionBytes.Length + 1);
                ThrowOnStatus(status, "UVSC_DBG_CALC_EXPRESSION", connectionHandle);

                valueType = Marshal.ReadInt32(buffer, 0);
                value = ReadTvalValue(buffer, valueType);
                if (value == null)
                {
                    throw new InvalidOperationException(
                        "uVision returned unsupported value type " +
                        GetTvalTypeName(valueType));
                }
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }

            string display = Convert.ToString(value, CultureInfo.InvariantCulture);

            return new Dictionary<string, object>
            {
                { "expression", expression },
                { "value", value },
                { "display", display },
                { "value_type", GetTvalTypeName(valueType) },
                { "value_type_raw", valueType },
                { "uvsc_status", status }
            };
        }

        internal IDictionary<string, object> EvalExpression(string expression, bool leadingSpace)
        {
            if (String.IsNullOrWhiteSpace(expression))
            {
                throw new ArgumentException("expression must not be empty");
            }
            if (expression.Length > 512)
            {
                throw new ArgumentException("expression is too long");
            }
            if (expression.IndexOf('=') >= 0 || expression.IndexOf(';') >= 0)
            {
                throw new ArgumentException("assignments and statement separators are not allowed");
            }
            if (FunctionCallPattern.IsMatch(expression))
            {
                throw new ArgumentException("function calls are not allowed");
            }
            EnsureConnected();

            string sent = leadingSpace ? " " + expression : expression;
            byte[] expressionBytes = Encoding.ASCII.GetBytes(sent);
            if (expressionBytes.Length + 1 > VsetBufferSize - VsetStringOffset)
            {
                throw new ArgumentException("expression is too long");
            }

            int status;
            int valueType = NativeMethods.VttVoid;
            object value = null;
            string errorDetail = null;
            IntPtr buffer = Marshal.AllocHGlobal(VsetBufferSize);
            try
            {
                Marshal.Copy(new byte[VsetBufferSize], 0, buffer, VsetBufferSize);
                Marshal.WriteInt32(buffer, 0, NativeMethods.VttVoid);
                Marshal.WriteInt64(buffer, 4, 0L);
                Marshal.WriteInt32(buffer, VsetStringLengthOffset, expressionBytes.Length);
                Marshal.Copy(
                    expressionBytes,
                    0,
                    IntPtr.Add(buffer, VsetStringOffset),
                    expressionBytes.Length);
                Marshal.WriteByte(buffer, VsetStringOffset + expressionBytes.Length, 0);

                status = NativeMethods.UVSC_DBG_CALC_EXPRESSION(
                    connectionHandle,
                    buffer,
                    VsetStringOffset + expressionBytes.Length + 1);

                if (status == NativeMethods.Success)
                {
                    valueType = Marshal.ReadInt32(buffer, 0);
                    value = ReadTvalValue(buffer, valueType);
                }
                else
                {
                    errorDetail = GetLastErrorDetail(connectionHandle);
                }
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }

            string display = value == null ? null : Convert.ToString(value, CultureInfo.InvariantCulture);

            return new Dictionary<string, object>
            {
                { "expression", expression },
                { "expression_sent", sent },
                { "leading_space", leadingSpace },
                { "status_ok", status == NativeMethods.Success },
                { "status", status },
                { "value", value },
                { "display", display },
                { "value_type", status == NativeMethods.Success ? GetTvalTypeName(valueType) : null },
                { "value_type_raw", status == NativeMethods.Success ? (object)valueType : null },
                { "uvsc_error", errorDetail }
            };
        }

        internal IDictionary<string, object> ReadMemory(object address, int length)
        {
            EnsureConnected();
            if (length < 1 || length > AmemMaxReadBytes)
            {
                throw new ArgumentException("length must be between 1 and " + AmemMaxReadBytes + " bytes");
            }

            ulong start = ParseAddress(address);

            // Allocate enough room for the AMEM header (24 bytes) plus the returned data.
            // Pass the full buffer size as memoryLength (the 2017 UVSC64.dll treats it as
            // the total writable buffer size, not just the header offset).
            int bufferSize = AmemDataOffset + length + 8;
            IntPtr buffer = Marshal.AllocHGlobal(bufferSize);
            try
            {
                Marshal.Copy(new byte[bufferSize], 0, buffer, bufferSize);
                Marshal.WriteInt64(buffer, AmemAddrOffset, (long)start);   // xU64 nAddr @ 0
                Marshal.WriteInt32(buffer, AmemBytesOffset, length);        // UINT nBytes @ 8
                Marshal.WriteInt64(buffer, AmemErrAddrOffset, 0L);          // xU64 ErrAddr @ 12
                Marshal.WriteInt32(buffer, AmemErrOffset, 0);               // UINT nErr @ 20

                int status = NativeMethods.UVSC_DBG_MEM_READ(
                    connectionHandle,
                    buffer,
                    bufferSize);
                ThrowOnStatus(status, "UVSC_DBG_MEM_READ", connectionHandle);

                int bytesReturned = Marshal.ReadInt32(buffer, AmemBytesOffset);
                int errFlag = Marshal.ReadInt32(buffer, AmemErrOffset);
                long errAddr = Marshal.ReadInt64(buffer, AmemErrAddrOffset);

                int avail = bytesReturned;
                if (avail < 0 || avail > length) { avail = length; }
                byte[] data = new byte[avail];
                Marshal.Copy(IntPtr.Add(buffer, AmemDataOffset), data, 0, avail);

                List<int> bytes = new List<int>(avail);
                for (int i = 0; i < avail; i++)
                {
                    bytes.Add(data[i]);
                }

                StringBuilder hex = new StringBuilder();
                for (int i = 0; i < bytes.Count; i++)
                {
                    if (i > 0)
                    {
                        hex.Append(' ');
                    }
                    hex.Append(bytes[i].ToString("X2", CultureInfo.InvariantCulture));
                }

                return new Dictionary<string, object>
                {
                    { "address", String.Format(CultureInfo.InvariantCulture, "0x{0:X8}", start) },
                    { "length", length },
                    { "bytes_returned", bytesReturned },
                    { "hex", hex.ToString() },
                    { "bytes", bytes },
                    { "read_error", errFlag != 0 },
                    { "error_address", errFlag != 0
                        ? (object)String.Format(CultureInfo.InvariantCulture, "0x{0:X8}", errAddr)
                        : null }
                };
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

        // Phase 3: memory write. Reuses the corrected AMEM layout. Writes to a variable's
        // data address (e.g. from the map file) are the safe use case; on 8051 the bare
        // numeric address space is driver-default, so prefer a data/XDATA address.
        internal IDictionary<string, object> WriteMemory(object address, IList<object> bytes)
        {
            EnsureConnected();
            if (bytes == null || bytes.Count < 1 || bytes.Count > AmemMaxReadBytes)
            {
                throw new ArgumentException("bytes must contain between 1 and " + AmemMaxReadBytes + " elements");
            }

            ulong start = ParseAddress(address);
            int length = bytes.Count;
            int bufferSize = AmemDataOffset + length + 8;
            IntPtr buffer = Marshal.AllocHGlobal(bufferSize);
            try
            {
                Marshal.Copy(new byte[bufferSize], 0, buffer, bufferSize);
                Marshal.WriteInt64(buffer, AmemAddrOffset, (long)start);
                Marshal.WriteInt32(buffer, AmemBytesOffset, length);
                Marshal.WriteInt64(buffer, AmemErrAddrOffset, 0L);
                Marshal.WriteInt32(buffer, AmemErrOffset, 0);
                byte[] data = new byte[length];
                for (int i = 0; i < length; i++)
                {
                    data[i] = (byte)(Convert.ToInt32(bytes[i], CultureInfo.InvariantCulture) & 0xFF);
                }
                Marshal.Copy(data, 0, IntPtr.Add(buffer, AmemDataOffset), length);

                int status = NativeMethods.UVSC_DBG_MEM_WRITE(connectionHandle, buffer, bufferSize);
                ThrowOnStatus(status, "UVSC_DBG_MEM_WRITE", connectionHandle);

                int written = Marshal.ReadInt32(buffer, AmemBytesOffset);
                int errFlag = Marshal.ReadInt32(buffer, AmemErrOffset);
                return new Dictionary<string, object>
                {
                    { "address", String.Format(CultureInfo.InvariantCulture, "0x{0:X8}", start) },
                    { "length", length },
                    { "bytes_written", written },
                    { "write_error", errFlag != 0 },
                    { "ok", errFlag == 0 }
                };
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

        private static ulong ParseAddress(object address)
        {
            ulong result;
            if (address is int || address is long || address is uint || address is ulong)
            {
                result = Convert.ToUInt64(address, CultureInfo.InvariantCulture);
            }
            else
            {
                string text = Convert.ToString(address, CultureInfo.InvariantCulture);
                if (String.IsNullOrWhiteSpace(text))
                {
                    throw new ArgumentException("address must not be empty");
                }
                text = text.Trim();
                if (text.StartsWith("0x", StringComparison.OrdinalIgnoreCase) || text.StartsWith("0X", StringComparison.OrdinalIgnoreCase))
                {
                    result = UInt64.Parse(text.Substring(2), NumberStyles.AllowHexSpecifier, CultureInfo.InvariantCulture);
                }
                else
                {
                    result = UInt64.Parse(text, NumberStyles.Integer, CultureInfo.InvariantCulture);
                }
            }

            if (result > 0xFFFFFFFFul)
            {
                throw new ArgumentException("address must fit in 32 bits");
            }
            return result;
        }

        private string ExecuteReadOnlyCommand(string command)
        {
            byte[] commandBytes = Encoding.ASCII.GetBytes(command);
            if (commandBytes.Length + 2 > ExecCommandStringCapacity)
            {
                throw new ArgumentException("uVision command is too long");
            }

            IntPtr commandBuffer = Marshal.AllocHGlobal(ExecCommandBufferSize);
            try
            {
                Marshal.Copy(new byte[ExecCommandBufferSize], 0, commandBuffer, ExecCommandBufferSize);
                Marshal.WriteInt32(commandBuffer, 0, 0); // bEcho = false
                Marshal.WriteInt32(
                    commandBuffer,
                    ExecCommandStringLengthOffset,
                    commandBytes.Length + 1); // SDK includes the terminating NUL
                Marshal.Copy(
                    commandBytes,
                    0,
                    IntPtr.Add(commandBuffer, ExecCommandStringOffset),
                    commandBytes.Length);
                Marshal.WriteByte(commandBuffer, ExecCommandStringOffset + commandBytes.Length, 0);
                Marshal.WriteByte(commandBuffer, ExecCommandStringOffset + commandBytes.Length + 1, 0);

                int status = NativeMethods.UVSC_DBG_EXEC_CMD(
                    connectionHandle,
                    commandBuffer,
                    ExecCommandBufferSize);
                ThrowOnStatus(status, "UVSC_DBG_EXEC_CMD", connectionHandle);
            }
            finally
            {
                Marshal.FreeHGlobal(commandBuffer);
            }

            int outputSize;
            int outputSizeStatus = NativeMethods.UVSC_GetCmdOutputSize(connectionHandle, out outputSize);
            ThrowOnStatus(outputSizeStatus, "UVSC_GetCmdOutputSize", connectionHandle);
            if (outputSize <= 0)
            {
                throw new InvalidOperationException("uVision returned no command output");
            }
            if (outputSize > 1024 * 1024)
            {
                throw new InvalidOperationException("uVision command output is unexpectedly large");
            }

            IntPtr outputBuffer = Marshal.AllocHGlobal(outputSize);
            try
            {
                Marshal.Copy(new byte[outputSize], 0, outputBuffer, outputSize);
                int outputStatus = NativeMethods.UVSC_GetCmdOutput(
                    connectionHandle,
                    outputBuffer,
                    outputSize);
                ThrowOnStatus(outputStatus, "UVSC_GetCmdOutput", connectionHandle);

                int textLength = FindNullTerminator(outputBuffer, 0, outputSize);
                byte[] outputBytes = new byte[textLength];
                Marshal.Copy(outputBuffer, outputBytes, 0, outputBytes.Length);
                return Encoding.Default.GetString(outputBytes);
            }
            finally
            {
                Marshal.FreeHGlobal(outputBuffer);
            }
        }

        private static object ReadTvalValue(IntPtr buffer, int valueType)
        {
            const int valueOffset = 4;
            byte[] raw = new byte[8];
            Marshal.Copy(IntPtr.Add(buffer, valueOffset), raw, 0, raw.Length);

            switch (valueType)
            {
                case NativeMethods.VttBit:
                    return (BitConverter.ToInt32(raw, 0) & 1) != 0;
                case NativeMethods.VttChar:
                    return unchecked((sbyte)raw[0]);
                case NativeMethods.VttUChar:
                    return raw[0];
                case NativeMethods.VttInt:
                case NativeMethods.VttLong:
                case NativeMethods.VttEnum:
                    return BitConverter.ToInt32(raw, 0);
                case NativeMethods.VttUInt:
                case NativeMethods.VttULong:
                    return BitConverter.ToUInt32(raw, 0);
                case NativeMethods.VttShort:
                    return BitConverter.ToInt16(raw, 0);
                case NativeMethods.VttUShort:
                    return BitConverter.ToUInt16(raw, 0);
                case NativeMethods.VttFloat:
                    return BitConverter.ToSingle(raw, 0).ToString("R", CultureInfo.InvariantCulture);
                case NativeMethods.VttDouble:
                    return BitConverter.ToDouble(raw, 0).ToString("R", CultureInfo.InvariantCulture);
                case NativeMethods.VttPointer:
                    return String.Format(CultureInfo.InvariantCulture, "0x{0:X8}", BitConverter.ToUInt32(raw, 0));
                case NativeMethods.VttInt64:
                    return BitConverter.ToInt64(raw, 0);
                case NativeMethods.VttUInt64:
                    return BitConverter.ToUInt64(raw, 0);
                default:
                    return null;
            }
        }

        private static string GetTvalTypeName(int valueType)
        {
            switch (valueType)
            {
                case NativeMethods.VttVoid: return "VTT_void";
                case NativeMethods.VttBit: return "VTT_bit";
                case NativeMethods.VttChar: return "VTT_char";
                case NativeMethods.VttUChar: return "VTT_uchar";
                case NativeMethods.VttInt: return "VTT_int";
                case NativeMethods.VttUInt: return "VTT_uint";
                case NativeMethods.VttShort: return "VTT_short";
                case NativeMethods.VttUShort: return "VTT_ushort";
                case NativeMethods.VttLong: return "VTT_long";
                case NativeMethods.VttULong: return "VTT_ulong";
                case NativeMethods.VttFloat: return "VTT_float";
                case NativeMethods.VttDouble: return "VTT_double";
                case NativeMethods.VttPointer: return "VTT_ptr";
                case NativeMethods.VttUnion: return "VTT_union";
                case NativeMethods.VttStruct: return "VTT_struct";
                case NativeMethods.VttFunction: return "VTT_func";
                case NativeMethods.VttString: return "VTT_string";
                case NativeMethods.VttEnum: return "VTT_enum";
                case NativeMethods.VttField: return "VTT_field";
                case NativeMethods.VttInt64: return "VTT_int64";
                case NativeMethods.VttUInt64: return "VTT_uint64";
                default: return "VTT_unknown_" + valueType.ToString(CultureInfo.InvariantCulture);
            }
        }

        internal IDictionary<string, object> ReadVariables(IList<object> expressions)
        {
            if (expressions == null || expressions.Count == 0)
            {
                throw new ArgumentException("expressions must contain at least one variable");
            }
            if (expressions.Count > 64)
            {
                throw new ArgumentException("a batch is limited to 64 variables");
            }

            List<object> results = new List<object>();
            foreach (object item in expressions)
            {
                string expression = item as string;
                if (String.IsNullOrWhiteSpace(expression))
                {
                    results.Add(new Dictionary<string, object>
                    {
                        { "expression", item },
                        { "error", "variable must be a non-empty string" }
                    });
                    continue;
                }

                try
                {
                    results.Add(ReadVariable(expression));
                }
                catch (Exception ex)
                {
                    results.Add(new Dictionary<string, object>
                    {
                        { "expression", expression },
                        { "error", ex.Message }
                    });
                }
            }
            return new Dictionary<string, object>
            {
                { "count", results.Count },
                { "variables", results }
            };
        }

        public void Dispose()
        {
            if (connected)
            {
                try { DisconnectCore(true); }
                catch { }
            }
            if (initialized)
            {
                try { NativeMethods.UVSC_UnInit(); }
                catch { }
                initialized = false;
            }
        }
        private void EnsureInitialized()
        {
            if (initialized)
            {
                return;
            }

            EnsureUvscAvailable();
            int status = NativeMethods.UVSC_Init(DefaultMinPort, DefaultMaxPort);
            ThrowOnStatus(status, "UVSC_Init", -1);
            initialized = true;
        }

        private void EnsureConnected()
        {
            if (!connected)
            {
                throw new InvalidOperationException("not connected; call keil_connect first");
            }
        }

        private static void ValidateVariable(string expression)
        {
            if (String.IsNullOrWhiteSpace(expression))
            {
                throw new ArgumentException("variable must not be empty");
            }
            if (expression.Length > 512)
            {
                throw new ArgumentException("variable is too long");
            }
            if (!SafeVariablePattern.IsMatch(expression))
            {
                throw new ArgumentException(
                    "only read-only variable paths are allowed, for example Sys_globaldata.u16BatterySocReal or packs[0].soc");
            }
        }

        private IDictionary<string, object> ConnectionResult(string state)
        {
            string nextStep;
            if (autoStarted && loadedProject == null) { nextStep = "Load a project with keil_load_project, then enter Debug mode manually."; }
            else if (autoStarted) { nextStep = "Enter Debug mode manually, then call keil_get_status or a read tool."; }
            else { nextStep = "Attach uses the project already open in Keil. Enter Debug mode manually, then call keil_get_status or a read tool."; }
            return new Dictionary<string, object>
            {
                { "state", state }, { "connection_handle", connectionHandle }, { "port", port },
                { "mode", autoStarted ? "auto" : "attach" }, { "project_file", loadedProject }, { "next_step", nextStep }
            };
        }
        private static int FindNullTerminator(IntPtr buffer, int offset, int maximum)
        {
            for (int i = 0; i < maximum; i++)
            {
                if (Marshal.ReadByte(buffer, offset + i) == 0)
                {
                    return i;
                }
            }
            return maximum;
        }

        private static string FormatVersion(uint version)
        {
            return String.Format("{0}.{1:00}", version / 100, version % 100);
        }

        private static void ThrowOnStatus(int status, string operation, int handle)
        {
            if (status == NativeMethods.Success)
            {
                return;
            }

            string detail = GetLastErrorDetail(handle);

            string message = operation + " failed with UVSC status " + status;
            if (!String.IsNullOrWhiteSpace(detail))
            {
                message += " (" + detail + ")";
            }
            throw new InvalidOperationException(message);
        }

        private static string GetLastErrorDetail(int handle)
        {
            if (handle < 0)
            {
                return null;
            }

            try
            {
                int messageType;
                int uvStatus;
                StringBuilder text = new StringBuilder(1024);
                int errorStatus = NativeMethods.UVSC_GetLastError(
                    handle,
                    out messageType,
                    out uvStatus,
                    text,
                    text.Capacity);
                if (errorStatus == NativeMethods.Success)
                {
                    return String.Format("UV status {0}, message type {1}: {2}", uvStatus, messageType, text);
                }
            }
            catch
            {
            }
            return null;
        }
    }

    internal static class Program
    {
        internal const string ServerVersion = "0.2.0";
        private static readonly JavaScriptSerializer Json = new JavaScriptSerializer();
        private static readonly UvscSession Session = new UvscSession();

        public static int Main(string[] args)
        {
            Console.InputEncoding = Encoding.UTF8;
            Console.OutputEncoding = new UTF8Encoding(false);
            Console.SetOut(new StreamWriter(Console.OpenStandardOutput(), new UTF8Encoding(false)) { AutoFlush = true });

            try
            {
                string line;
                while ((line = Console.ReadLine()) != null)
                {
                    if (String.IsNullOrWhiteSpace(line))
                    {
                        continue;
                    }
                    HandleMessage(line);
                }
                return 0;
            }
            finally
            {
                Session.Dispose();
            }
        }

        private static void HandleMessage(string line)
        {
            IDictionary<string, object> request;
            object id = null;
            try
            {
                request = Json.DeserializeObject(line) as IDictionary<string, object>;
                if (request == null)
                {
                    throw new InvalidDataException("request must be a JSON object");
                }

                request.TryGetValue("id", out id);
                string method = GetString(request, "method");
                if (String.IsNullOrWhiteSpace(method))
                {
                    throw new InvalidDataException("request method is missing");
                }

                // Notifications have no id and must not receive a response.
                if (id == null)
                {
                    return;
                }

                object result;
                switch (method)
                {
                    case "initialize":
                        result = Initialize(request);
                        break;
                    case "ping":
                        result = new Dictionary<string, object>();
                        break;
                    case "tools/list":
                        result = new Dictionary<string, object> { { "tools", BuildTools() } };
                        break;
                    case "tools/call":
                        result = CallTool(request);
                        break;
                    default:
                        WriteError(id, -32601, "method not found: " + method);
                        return;
                }

                WriteResult(id, result);
            }
            catch (Exception ex)
            {
                if (id != null)
                {
                    WriteError(id, -32603, ex.Message);
                }
            }
        }

        private static object Initialize(IDictionary<string, object> request)
        {
            IDictionary<string, object> parameters = GetDictionary(request, "params");
            string protocolVersion = parameters == null ? null : GetString(parameters, "protocolVersion");
            if (String.IsNullOrWhiteSpace(protocolVersion))
            {
                protocolVersion = "2025-06-18";
            }

            return new Dictionary<string, object>
            {
                { "protocolVersion", protocolVersion },
                { "capabilities", new Dictionary<string, object> { { "tools", new Dictionary<string, object>() } } },
                { "serverInfo", new Dictionary<string, object> { { "name", "keil-uvsc-mcp" }, { "version", ServerVersion } } },
                { "instructions", "Keil inspection and run control through UVSC64.dll. Keil paths are auto-detected; the safe default is attach port 4823, which keeps the project already open in Keil. Enter Debug mode manually before reading symbols or running. Never infer that a failed read means zero. Read tools: keil_read_variable(s), keil_eval_expression, keil_read_memory. Run-control tools (keil_run, keil_stop, keil_reset, keil_step_instruction, keil_step_over), breakpoints (keil_set_breakpoint, keil_clear_breakpoint), and keil_write_memory change target state; keil_build compiles the project (uVision must not be in Debug). On GLink+ halt the target before reading variables or setting breakpoints. No flash download tool is exposed." }
            };
        }

        private static IList<object> BuildTools()
        {
            List<object> tools = new List<object>();
            tools.Add(Tool("keil_uvsc_info",
                "Inspect automatic Keil discovery, UVSC/UVSOCK versions, configured paths, connection state, and the read-only safety policy. Use this first on a new machine or when UVSC64.dll cannot be loaded.",
                Schema(new Dictionary<string, object>())));
            tools.Add(Tool("keil_connect",
                "Connect to Keil through UVSOCK. The safe default is attach on port 4823, which uses the project already open in Keil and ignores project_file. Auto mode starts a dedicated Keil instance and may load project_file, but never enters Debug or downloads firmware.",
                Schema(new Dictionary<string, object>
                {
                    { "mode", new Dictionary<string, object> { { "type", "string" }, { "enum", new object[] { "attach", "auto" } }, { "default", "attach" } } },
                    { "port", new Dictionary<string, object> { { "type", "integer" }, { "minimum", 1 }, { "maximum", 65535 }, { "default", 4823 }, { "description", "UVSOCK port for attach mode. Default 4823." } } },
                    { "keil_path", new Dictionary<string, object> { { "type", "string" }, { "description", "Optional UV4.exe path. Usually omitted because standard paths, environment variables, and the Keil registry are auto-detected." } } },
                    { "project_file", new Dictionary<string, object> { { "type", "string" }, { "description", "Used only in auto mode. Ignored in attach mode so the current Keil project is never replaced." } } }
                })));
            tools.Add(Tool("keil_disconnect",
                "Close the current UVSOCK client connection without entering/exiting Debug, resetting, running, downloading, or changing the Keil project. Use before reconnecting to a restarted or different Keil instance.",
                Schema(new Dictionary<string, object>())));
            tools.Add(Tool("keil_reconnect",
                "Discard the current or stale UVSOCK connection and connect again. Defaults to attach port 4823; attach keeps the project already open in Keil. This only reconnects the client channel and performs no target run-control or download.",
                Schema(new Dictionary<string, object>
                {
                    { "mode", new Dictionary<string, object> { { "type", "string" }, { "enum", new object[] { "attach", "auto" } }, { "default", "attach" } } },
                    { "port", new Dictionary<string, object> { { "type", "integer" }, { "minimum", 1 }, { "maximum", 65535 }, { "default", 4823 } } },
                    { "keil_path", new Dictionary<string, object> { { "type", "string" } } },
                    { "project_file", new Dictionary<string, object> { { "type", "string" }, { "description", "Auto mode only; ignored by attach." } } }
                })));
            tools.Add(Tool("keil_load_project",
                "Open an existing Keil project in an auto-started connected uVision session. Do not use this after attach when the user's current project should remain open. This does not build, enter Debug, download, reset, or run the target.",
                Schema(new Dictionary<string, object> { { "project_file", new Dictionary<string, object> { { "type", "string" }, { "description", "Absolute .uvprojx/.uvproj/.uv2/.mpw path." } } } }, new object[] { "project_file" })));
            tools.Add(Tool("keil_show_window",
                "Show and foreground the connected uVision window. This changes window visibility only and performs no target operation.",
                Schema(new Dictionary<string, object>())));
            tools.Add(Tool("keil_get_status",
                "Read whether the connected Keil debugger reports the target running or stopped. Keil must already be in Debug mode; this does not stop or run the target.",
                Schema(new Dictionary<string, object>())));
            tools.Add(Tool("keil_read_variable",
                "Read one C global/member/array variable by symbol through the current Keil project. It works while running when Keil supports the read; stop at a breakpoint only when a coherent multi-variable snapshot is required. Assignments, calls, casts, and arbitrary expressions are rejected.",
                Schema(new Dictionary<string, object> { { "variable", new Dictionary<string, object> { { "type", "string" }, { "description", "Variable path, for example Bms_data_info.pack_num." } } } }, new object[] { "variable" })));
            tools.Add(Tool("keil_read_variables",
                "Read up to 64 validated C variable paths in one request. Each item succeeds or fails independently; use this for related values and stop the target first only when they must represent one coherent instant.",
                Schema(new Dictionary<string, object> { { "variables", new Dictionary<string, object> { { "type", "array" }, { "minItems", 1 }, { "maxItems", 64 }, { "items", new Dictionary<string, object> { { "type", "string" } } } } } }, new object[] { "variables" })));
            tools.Add(Tool("keil_eval_expression",
                "Diagnostic read-only C expression evaluation through UVSC_DBG_CALC_EXPRESSION. Use to diagnose symbol resolution or inspect addresses; assignments, statement separators, and function calls are rejected. Prefer keil_read_variable for normal symbol reads.",
                Schema(new Dictionary<string, object>
                {
                    { "expression", new Dictionary<string, object> { { "type", "string" }, { "description", "Read-only C expression." } } },
                    { "leading_space", new Dictionary<string, object> { { "type", "boolean" }, { "default", false }, { "description", "Diagnostic compatibility switch; normally false." } } }
                }, new object[] { "expression" })));
            tools.Add(Tool("keil_read_memory",
                "Read 1..256 target bytes in one UVSC_DBG_MEM_READ call without using the expression parser. Use an address derived from the exact AXF currently loaded in Keil; stopping the target gives the most consistent snapshot.",
                Schema(new Dictionary<string, object>
                {
                    { "address", new Dictionary<string, object> { { "type", "string" }, { "description", "32-bit start address, for example 0x2000AD08." } } },
                    { "length", new Dictionary<string, object> { { "type", "integer" }, { "minimum", 1 }, { "maximum", 256 } } }
                }, new object[] { "address", "length" })));
            // Phase 3: run control (state-changing, confirmed by caller before use).
            tools.Add(Tool("keil_run",
                "Start target execution (Debug > Run). Changes target state. Keil must be in Debug and stopped. Use keil_get_status to confirm it is running afterwards.",
                Schema(new Dictionary<string, object>())));
            tools.Add(Tool("keil_stop",
                "Halt a running target (Debug > Stop). Changes target state. Keil must be in Debug and running.",
                Schema(new Dictionary<string, object>())));
            tools.Add(Tool("keil_reset",
                "Reset the target (Debug > Reset). Changes target state and clears run-time state. Keil must be in Debug.",
                Schema(new Dictionary<string, object>())));
            tools.Add(Tool("keil_step_instruction",
                "Step one assembly instruction (Debug > Step). Changes target state. Keil must be in Debug and stopped.",
                Schema(new Dictionary<string, object>())));
            tools.Add(Tool("keil_step_over",
                "Step over one high-level source line (Debug > Step Over). Changes target state. Keil must be in Debug and stopped.",
                Schema(new Dictionary<string, object>())));
            tools.Add(Tool("keil_build",
                "Build (or rebuild) the current project and return the build log with error/warning counts. Blocks until the build finishes. uVision must NOT be in Debug mode — open the project in edit mode (or restart uVision) rather than calling keil_exit_debug, which is unstable on GLink+. Use to close the edit->compile->inspect loop.",
                Schema(new Dictionary<string, object>
                {
                    { "rebuild", new Dictionary<string, object> { { "type", "boolean" }, { "default", false }, { "description", "true = Rebuild all target files; false = incremental Build." } } }
                })));
            tools.Add(Tool("keil_enter_debug",
                "Enter Debug mode (Debug > Start Debug Session). Changes Keil session state. Requires a built binary loaded. Equivalent to Ctrl+F5.",
                Schema(new Dictionary<string, object>())));
            tools.Add(Tool("keil_exit_debug",
                "Exit Debug mode (Debug > Stop Debug Session). WARNING: on GLink+ (GPMCUTarget.dll) this has been observed to terminate the whole uVision process. Prefer closing and reopening uVision in edit mode to rebuild. Use with caution.",
                Schema(new Dictionary<string, object>())));
            tools.Add(Tool("keil_set_breakpoint",
                "Set an execution breakpoint on a symbol or address expression (e.g. a function name like \"timer0_isr\" or a code address). Keil must be in Debug. Returns a tick_mark that identifies the breakpoint. Changes target state.",
                Schema(new Dictionary<string, object> { { "expression", new Dictionary<string, object> { { "type", "string" }, { "description", "Symbol or address where execution should break." } } } }, new object[] { "expression" })));
            tools.Add(Tool("keil_clear_breakpoint",
                "Delete a breakpoint by the tick_mark returned from keil_set_breakpoint. Changes target state.",
                Schema(new Dictionary<string, object> { { "tick_mark", new Dictionary<string, object> { { "type", "integer" }, { "description", "Breakpoint tick_mark to delete." } } } }, new object[] { "tick_mark" })));
            tools.Add(Tool("keil_write_memory",
                "Write 1..256 bytes to target memory at a 32-bit address (UVSC_DBG_MEM_WRITE). DANGEROUS: changes target RAM/registers. Prefer a variable's data address from the map file. On 8051 the bare numeric address space is driver-default. Requires Debug mode; halt the target first on GLink+.",
                Schema(new Dictionary<string, object>
                {
                    { "address", new Dictionary<string, object> { { "type", "string" }, { "description", "32-bit start address, for example 0x122." } } },
                    { "bytes", new Dictionary<string, object> { { "type", "array" }, { "minItems", 1 }, { "maxItems", 256 }, { "items", new Dictionary<string, object> { { "type", "integer" }, { "minimum", 0 }, { "maximum", 255 } } } } }
                }, new object[] { "address", "bytes" })));
            return tools;
        }
        private static object CallTool(IDictionary<string, object> request)
        {
            IDictionary<string, object> parameters = GetDictionary(request, "params");
            string name = parameters == null ? null : GetString(parameters, "name");
            IDictionary<string, object> arguments = parameters == null ? null : GetDictionary(parameters, "arguments");
            if (arguments == null)
            {
                arguments = new Dictionary<string, object>();
            }

            try
            {
                object data;
                switch (name)
                {
                    case "keil_uvsc_info":
                        data = Session.GetInfo();
                        break;
                    case "keil_connect":
                        data = Session.Connect(
                            GetString(arguments, "mode"), GetInt(arguments, "port", 0),
                            GetString(arguments, "keil_path"), GetString(arguments, "project_file"));
                        break;
                    case "keil_disconnect":
                        data = Session.Disconnect();
                        break;
                    case "keil_reconnect":
                        data = Session.Reconnect(
                            GetString(arguments, "mode"), GetInt(arguments, "port", 0),
                            GetString(arguments, "keil_path"), GetString(arguments, "project_file"));
                        break;
                    case "keil_load_project":
                        data = Session.LoadProject(GetString(arguments, "project_file"));
                        break;
                    case "keil_show_window":
                        data = Session.ShowWindow();
                        break;
                    case "keil_get_status":
                        data = Session.GetStatus();
                        break;
                    case "keil_read_variable":
                        data = Session.ReadVariable(GetString(arguments, "variable"));
                        break;
                    case "keil_read_variables":
                        data = Session.ReadVariables(GetList(arguments, "variables"));
                        break;
                    case "keil_eval_expression":
                        data = Session.EvalExpression(
                            GetString(arguments, "expression"),
                            GetBool(arguments, "leading_space", false));
                        break;
                    case "keil_read_memory":
                        data = Session.ReadMemory(
                            GetValue(arguments, "address"),
                            GetInt(arguments, "length", 0));
                        break;
                    case "keil_run":
                        data = Session.Run();
                        break;
                    case "keil_stop":
                        data = Session.Stop();
                        break;
                    case "keil_reset":
                        data = Session.Reset();
                        break;
                    case "keil_step_instruction":
                        data = Session.StepInstruction();
                        break;
                    case "keil_step_over":
                        data = Session.StepHll();
                        break;
                    case "keil_build":
                        data = Session.Build(GetBool(arguments, "rebuild", false));
                        break;
                    case "keil_enter_debug":
                        data = Session.EnterDebug();
                        break;
                    case "keil_exit_debug":
                        data = Session.ExitDebug();
                        break;
                    case "keil_set_breakpoint":
                        data = Session.SetBreakpoint(GetString(arguments, "expression"));
                        break;
                    case "keil_clear_breakpoint":
                        data = Session.ClearBreakpoint(GetInt(arguments, "tick_mark", 0));
                        break;
                    case "keil_write_memory":
                        data = Session.WriteMemory(
                            GetValue(arguments, "address"),
                            GetList(arguments, "bytes"));
                        break;
                    default:
                        throw new ArgumentException("unknown tool: " + name);
                }
                return ToolResult(data, false);
            }
            catch (Exception ex)
            {
                return ToolResult(new Dictionary<string, object> { { "error", ex.Message } }, true);
            }
        }

        private static object Tool(string name, string description, object inputSchema)
        {
            bool changesKeilSession = name == "keil_connect" || name == "keil_disconnect" || name == "keil_reconnect" || name == "keil_load_project" || name == "keil_show_window" || name == "keil_enter_debug" || name == "keil_exit_debug";
            bool changesTargetState = name == "keil_run" || name == "keil_stop" || name == "keil_reset" || name == "keil_step_instruction" || name == "keil_step_over" || name == "keil_set_breakpoint" || name == "keil_clear_breakpoint" || name == "keil_write_memory";
            bool buildAction = name == "keil_build";
            bool readOnly = !changesKeilSession && !changesTargetState && !buildAction;
            return new Dictionary<string, object>
            {
                { "name", name },
                { "description", description },
                { "inputSchema", inputSchema },
                { "annotations", new Dictionary<string, object> { { "readOnlyHint", readOnly }, { "destructiveHint", changesTargetState }, { "idempotentHint", readOnly }, { "openWorldHint", false } } }
            };
        }

        private static object Schema(IDictionary<string, object> properties)
        {
            return Schema(properties, new object[0]);
        }

        private static object Schema(IDictionary<string, object> properties, object[] required)
        {
            return new Dictionary<string, object>
            {
                { "type", "object" },
                { "properties", properties },
                { "required", required },
                { "additionalProperties", false }
            };
        }

        private static object ToolResult(object data, bool isError)
        {
            string text = Json.Serialize(data);
            return new Dictionary<string, object>
            {
                { "content", new object[] { new Dictionary<string, object> { { "type", "text" }, { "text", text } } } },
                { "structuredContent", data },
                { "isError", isError }
            };
        }

        private static void WriteResult(object id, object result)
        {
            WriteJson(new Dictionary<string, object>
            {
                { "jsonrpc", "2.0" },
                { "id", id },
                { "result", result }
            });
        }

        private static void WriteError(object id, int code, string message)
        {
            WriteJson(new Dictionary<string, object>
            {
                { "jsonrpc", "2.0" },
                { "id", id },
                { "error", new Dictionary<string, object> { { "code", code }, { "message", message } } }
            });
        }

        private static void WriteJson(object value)
        {
            Console.WriteLine(Json.Serialize(value));
        }

        private static IDictionary<string, object> GetDictionary(IDictionary<string, object> source, string key)
        {
            object value;
            if (source != null && source.TryGetValue(key, out value))
            {
                return value as IDictionary<string, object>;
            }
            return null;
        }

        private static IList<object> GetList(IDictionary<string, object> source, string key)
        {
            object value;
            if (source != null && source.TryGetValue(key, out value))
            {
                object[] array = value as object[];
                if (array != null)
                {
                    return array;
                }
                ArrayList list = value as ArrayList;
                if (list != null)
                {
                    return list.ToArray();
                }
            }
            return null;
        }

        private static string GetString(IDictionary<string, object> source, string key)
        {
            object value;
            return source != null && source.TryGetValue(key, out value) && value != null ? Convert.ToString(value) : null;
        }

        private static int GetInt(IDictionary<string, object> source, string key, int defaultValue)
        {
            object value;
            if (source != null && source.TryGetValue(key, out value) && value != null)
            {
                return Convert.ToInt32(value);
            }
            return defaultValue;
        }

        private static bool GetBool(IDictionary<string, object> source, string key, bool defaultValue)
        {
            object value;
            if (source != null && source.TryGetValue(key, out value) && value != null)
            {
                return Convert.ToBoolean(value);
            }
            return defaultValue;
        }

        private static object GetValue(IDictionary<string, object> source, string key)
        {
            object value;
            return source != null && source.TryGetValue(key, out value) ? value : null;
        }
    }
}
