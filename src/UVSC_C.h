/** @file  UVSC_C.h
  * @brief UVSOCK Client DLL Interface
  * @version V2.08
  *
  * API for the UVSC DLL, to facilitate 3rd party application communication with 
  * uVision.
  *
  * <b>General API Rules</b>
  *
  * @li All 'reserved' data fields must be written as 0 and ignored when reading.
  * @li Non-relevant data fields in requests must be written as 0.
  * @li The UVSC DLL must be loaded from the installed UV3 folder to guarantee compatibility with
  *     UV3.exe. The UVSC DLL must not be included in your distributed application, unless your application
  *     is guaranteed to be used only with the specific version of KEIL tools from which the UVSC DLL   
  *     was taken.
  *
  *
  * <b>Version History</b>
  *
  * Author, Date, Version
  * @li RAS, 08.02.2008, V2.02
  * @li RAS, 20.02.2008, V2.03
  * @li RAS, 18.03.2008, V2.04
  * @li RAS, 22.04.2008, V2.05
  * @li RAS, 25.04.2008, V2.06
  * @li RAS, 09.05.2008, V2.07
  * @li RAS, 25.09.2008, V2.08
  *
  * <b>Change Log</b>
  *
  * Version 2.02:
  * @li initial release
  * @li version in line with UVSOCK interface
  *
  * Version 2.03:
  * @li Additional error codes (UVSC_STATUS_INVALID_CONTEXT, UVSC_STATUS_INVALID_PARAM, 
  *                             UVSC_STATUS_BUFFER_TOO_SMALL, UVSC_STATUS_CALLBACK_IN_USE, 
  *                             UVSC_STATUS_COMMAND_ERROR)
  * @li Build and progress bar callbacks added (UVSC_CB_BUILD_OUTPUT_MSG, UVSC_CB_PROGRESS_BAR_MSG)
  *
  * Version 2.04:
  * @li Bug fix:     Increased size of internal message queue to 200
  * @li Bug fix:     Removed buffer overrun from UVSC logging
  * @li Enhancement: Logging completely disabled when NULL passed to both logFileName and
  *                  logCallback in UVSC_OpenConnection
  *
  * Version 2.05:
  * @li Bug fix:     Replaced bool type with xBOOL, as bool is C++ only type. API is still 
  *                  binary compatible.
  *
  * Version 2.06:
  * @li Enhancement: Modified Message Box blocking mechanism, reducing the amount of messages 
  *                  required to complete several commands
  * @li Baseline:    Backwards compatibility for UVSC_C.h API from this version
  *
  * Version 2.07:
  * @li Bug Fix:     Timeouts changed (Message Response = 10 seconds, Async Response = 3 minutes)
  * @li Bug Fix:     Removed occasional deadlock in UVSC_DBG_EXIT()
  * @li Bug Fix:     Calling UVSC_LogControl() no longer returns an error if logging is not enabled
  *
  * Version 2.08:
  * @li Enhancement: Added UV_DBG_EXEC_CMD command. It allows any command line to be executed via UVSOCK
  * @li Enhancement: Added UV_PRG_GET_OUTPUTNAME command. It allows the current executable name to be retrieved
  *
  * Version 2.09:
  * @li Enhancement: Changed display of Breakpoint-command responses to show BP_DELETED, BP_ENABLED etc.
  *                  This requires to look at 'cmdRsp.status' for exact Bp-action.
  *
  * Version 2.10:
  * @li Enhancement: Added #UVSC_PSTAMP, #UV_DBG_POWERSCALE_SHOWCODE, #UV_DBG_POWERSCALE_SHOWPOWER, #UVSC_POWERSCALE_SHOWCODE
  *                  to support Hitex PowerScale.
  *
  * Version 2.11:
  * @li Enhancement: Added #UV_DBG_EVAL_EXPRESSION_TO_STR and  #UV_DBG_FILELINE_TO_ADR
  *                  to support Eclipse client 
  * Version 2.12:
  * @li Enhancement: Added #UV_DBG_ENUM_REGISTER_GROUPS, #UV_DBG_ENUM_REGISTERS, #UV_DBG_READ_REGISTERS and #UV_DBG_REGISTER_SET
  *                  to support Eclipse client 
  * Version 2.13:
  * @li Enhancement: Added #UV_DBG_DSM_READ
  *                  to support Eclipse client 
  *
  * Version 2.14:
  * @li Correction:  Mantis #4789: removed 'UVSC_PSTAMP' from response created by OkToClientGeneric()
  *
  * Version 2.15:
  * @li Bugfix:      Writing beyond buffer size is fixed in UV_DBG_ENUM_STACK
  *
  * Version 2.17:
  * @li Enhancement: Added #UV_DBG_EVAL_WATCH_EXPRESSION, #UV_DBG_REMOVE_WATCH_EXPRESSION
  *                        #UV_DBG_ENUM_VARIABLES,  #UV_DBG_VARIABLE_SET
  *                  to support Eclipse client 
  *
  * Version 2.18:
  * @li Enhancement: Added #UV_DBG_ENUM_TASKS
  *                  to support Eclipse client 
  *
  * Version 2.19:
  * @li Enhancement: Added #UV_DBG_SERIAL_OUTPUT
  *                  to support Eclipse client 
  *
  * Version 2.20:
  * @li Enhancement: Added #UV_DBG_ENUM_MENUS, #UV_DBG_MENU_EXEC
  *                  to support Eclipse client 
  *
  * Version 2.21:
  * @li Enhancement: Struct VARINO is extended with number of flags 
  *                  to support Eclipse client 
  *
  * Version 2.22:
  * @li Correction:  Comments are changed to match implementation 
  *
  *
  * Version 2.23:
  * @li Enahncement: Added UV_GEN_SET_OPTIONS to set general UVSC options  
  *
  */ 

/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2008 KEIL - An ARM Company. All rights reserved.             */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#ifndef __UVSC_C_H__
#define __UVSC_C_H__

#ifdef WIN32
#include <windows.h>
#endif

#include "UVSOCK.h"

#ifdef __cplusplus
  extern "C"  {
#endif

#ifdef WIN32
#ifdef _IN_DLL_
#define _UVSC_FUNC_  extern __declspec(dllexport)
#else
#define _UVSC_FUNC_  extern __declspec(dllimport)
#endif
#else /* #ifdef WIN32 */
#define _UVSC_FUNC_ 
#endif /* #ifdef WIN32 */

/** Maximum number of clients UVSC can support
  *
  * UVSC can support this many clients.
  */
#define UVSC_MAX_CLIENTS          10

/** Maximum string size
  *
  * Maximum string size where strings are passed to or returned from API
  * functions such as #UVSC_OpenConnection.
  */
#define UVSC_MAX_API_STR_SIZE     1024

/** Auto port
  *
  * This define is used to indicate that automatic port selection is 
  * wanted.
  */
#define UVSC_PORT_AUTO            0

/** Minimum auto port value
  *
  * The minimum port passed to #UVSC_Init must not be less than this.
  */
#define UVSC_MIN_AUTO_PORT        1

/** Maximum auto port value
  *
  * The maximum port passed to #UVSC_Init must not be more than this.
  */
#define UVSC_MAX_AUTO_PORT        65535


/** UVSC status codes
  *
  * UVSC status codes are returned by UVSC API functions, and indicate the 
  * success or failure of the called API function.
  */
typedef enum _tag_UVSC_STATUS {
  UVSC_STATUS_SUCCESS             = 0,  ///< Success
  UVSC_STATUS_FAILED              = 1,  ///< General failure
  UVSC_STATUS_NOT_SUPPORTED       = 2,  ///< Request for an unsupported operation
  UVSC_STATUS_NOT_INIT            = 3,  ///< UVSC not initialised
  UVSC_STATUS_TIMEOUT             = 4,  ///< Operation timed-out
  UVSC_STATUS_INVALID_CONTEXT     = 5,  ///< Function called from an invalid context (most likely the callback function)
  UVSC_STATUS_INVALID_PARAM       = 6,  ///< Function called with one or more invalid parameters
  UVSC_STATUS_BUFFER_TOO_SMALL    = 7,  ///< Function called with a buffer that was not big enough to hold the result from uVision
  UVSC_STATUS_CALLBACK_IN_USE     = 8,  ///< Function cannot be used when the callback is in use
  UVSC_STATUS_COMMAND_ERROR       = 9,  ///< The command failed - call #UVSC_GetLastError to get more information on how the command failed
  UVSC_STATUS_END                       ///< Always at end
} UVSC_STATUS;

/** uVision Runmode
  *
  * Indicates the mode uVision will be started in if it is auto-started.
  */
typedef enum _tag_UVSC_RUNMODE {
  UVSC_RUNMODE_NORMAL             = 0,  ///< Normal uVision operation
  UVSC_RUNMODE_LABVIEW 	          = 1,  ///< LabVIEW operation
  UVSC_RUNMODE_END     	          = 2,  ///< Always at end
} UVSC_RUNMODE;

/** Progress bar operations
  *
  * Progress bar operations as returned by #UVSC_ReadPBarQ.
  */
typedef enum _tag_UVSC_PBAR {
  UVSC_PBAR_INIT                  = 0,  ///< Initialise progress bar
  UVSC_PBAR_TEXT                  = 1,  ///< Set progress bar text
  UVSC_PBAR_POS                   = 2,  ///< Set progress bar position
  UVSC_PBAR_STOP                  = 3,  ///< Stop progress bar
} UVSC_PBAR;

/** UVSC callback type
  *
  * Indicates the type of message retuned in the UVSC callback function #uvsc_cb
  * configured through #UVSC_OpenConnection.
  */
typedef enum _tag_UVSC_CB_TYPE {
  UVSC_CB_ERROR                   = 0,  ///< Error notification (not used)
  UVSC_CB_ASYNC_MSG               = 1,  ///< Asynchronous message received (called from UVSC internal thread)  
  UVSC_CB_DISCONNECTED            = 2,  ///< uVision has disconnected (called from UVSC internal thread)
  UVSC_CB_BUILD_OUTPUT_MSG        = 3,  ///< Called from the #UVSC_PRJ_BUILD function - indicates a line of build output (called from API function callers own thread)  
  UVSC_CB_PROGRESS_BAR_MSG        = 4,  ///< Called from function that cause a progress bar in uVision - indicates the progress bar state (called from API function callers own thread)  
  UVSC_CB_CMD_OUTPUT_MSG          = 5,  ///< Called from the #UVSC_DBG_EXEC_CMD function - indicates a line of command output (called from API function callers own thread)  
} UVSC_CB_TYPE;

/** UVSC callback data
  *
  * Contains the data retuned in the UVSC callback function #uvsc_cb configured  
  * through #UVSC_OpenConnection.
  */
typedef union _tag_UVSC_CB_DATA {
  UVSOCK_CMD msg;                       ///< UVSOCK fromatted message (returned when UVSC_CB_TYPE is #UVSC_CB_ASYNC_MSG, #UVSC_CB_BUILD_OUTPUT_MSG, #UVSC_CB_CMD_OUTPUT_MSG or #UVSC_CB_PROGRESS_BAR_MSG)
  UVSC_STATUS err;                      ///< Error notification (returned when UVSC_CB_TYPE is #UVSC_CB_ERROR)
  int iConnHandle;                      ///< UVSC connection handle (returned when UVSC_CB_TYPE is #UVSC_CB_DISCONNECTED)
} UVSC_CB_DATA;

/** UVSC callback function pointer definition
  *
  * Defines the format to which the UVSC callback function configured through 
  * #UVSC_OpenConnection must correspond.
  */
typedef void (*uvsc_cb)(void* cb_custom, UVSC_CB_TYPE type, UVSC_CB_DATA *data);

/** UVSC logging callback function pointer definition
  *
  * Defines the format to which the UVSC logging callback function configured
  * through #UVSC_OpenConnection must correspond.
  */
typedef void (*log_cb)(const char* msg, int msgLen);

// Control functions

/** Get the library and interface versions
  * @param pUVSCVersion Pointer to a variable to contain the UVSC library version
  * @param pUVSOCKVersion Pointer to a variable to contain the UVSOCK interface version
  *
  * Returns the UVSC library and UVSOCK interface versions for this library.
  *
  * The UINT version is converted to an X.YY version number by the following formula:
  * <pre> X.YY = UV3_SOCKIF_VERS / 100 </pre>
  *
  * For example: 
  * <pre> *pUVSOCKVersion = 201 ==> V2.01 </pre>
  */
_UVSC_FUNC_ void UVSC_Version(UINT *pUVSCVersion,
                              UINT *pUVSOCKVersion);

/** Initialise UVSC
  * @param uvMinPort Minimum port avialable to the automatic port allocator. This must not be smaller than #UVSC_MIN_AUTO_PORT
  * @param uvMaxPort Maximum port avialable to the automatic port allocator. This must not be larger than #UVSC_MAX_AUTO_PORT
  *
  * Call this function to initialise UVSC. This function must be called before any other
  * UVSC function except #UVSC_Version.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_Init(int uvMinPort, 
                                  int uvMaxPort);

/** Uninitialise UVSC
  *
  * @pre #UVSC_Init should have been called successfully
  *
  * Call this function to uninitialise UVSC. This function should be called before
  * the UVSC library is unloaded from memory. All connected sessions will be automatically
  * disconnected.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_UnInit(void);

/** Open a connection to uVision session
  * @param name Pointer to a zero terminated string containing a unique connection name.
  *        This name can be used later to retrieve the connection handle (@a iConnHandle) 
  *        via the #UVSC_ConnHandleFromConnName function. If a connection name is not
  *        required this parameter should be NULL
  * @param iConnHandle Pointer to a variable to contain the handle assigned to this connection.
  *        The value assigned will be invalid if the function returns an error. This 
  *        pointer must not be NULL
  * @param pPort Pointer to a variable containing the port of an existing uVision session 
  *        to connect, or the value #UVSC_PORT_AUTO to auto-start a uVision session for 
  *        this connection. If the function returns successfully, this variable will contain 
  *        the port that was used for the connection. This pointer must not be NULL.
  * @param uvCmd Pointer to a zero terminated string containing the path to a uVision
  *        executable to be started automatically for this connection. If 
  *        uVision is not being auto-started this parameter is ignored and should be
  *        NULL
  * @param uvRunmode Indicates the mode uVision should be started in when
  *        auto-started. If uVision is not being auto-started this parameter is ignored
  * @param callback Pointer to a function to receive callbacks from UVSC. The callbacks
  *        provide notification of asynchronous UVSOCK messages, connection events,
  *        and errors relating to the connection. NOTE: UVSC_xxx API functions must
  *        not be called from the callback. To call a UVSC_xxx function in response to the
  *        callback, a notification must be passed another thread, which can then call the
  *        UVSC_xxx function.
  * @param cb_custom This pointer will be returned in the calls to the callback function.
  *        It may be used for any purpose, but a common usage is for keeping track of the
  *        'this' pointer of a C++ object relating to the connection
  * @param logFileName Pointer to a zero terminated string containing the path to a 
  *        file to use for logging UVSOCK messages for this connection for debug
  *        purposes. If a debugging log file is not required, this parameter should be NULL
  * @param logFileAppend Pass 'xTRUE' to append to the log file specified in @a logFileName,
  *        or 'xFALSE' to overwrite the file. If @a logFileName is NULL, this parameter is
  *        ignored
  * @param logCallback Pointer to a function to receive logging callbacks. These
  *        callbacks contain the same information that is entered into the log file.
  *        If this callback is not required, this parameter should be NULL
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_Init must have been called successfully
  *
  * This function opens a connection to a uVision session. The session can be either an existing
  * session, or UVSC can automatically start a new session.
  *
  * <b>To auto-start a new uVision session (recommended):</b>
  *
  * @li @a pPort must be #UVSC_PORT_AUTO
  * @li @a uvCmd must be the path to a valid uVision executable that supports UVSOCK
  *
  * Auto-starting uVision has the following advantages:
  *
  * @li uVision is in a known state from the beginning of the connection
  * @li uVision can be started in a special mode, that restricts how a user can
  *     interact with the remotely controlled session
  * @li If the uVision session is terminated by an application other than UVSC,
  *     UVSC automatically restarts the uVision session
  * @li uVision starts hidden from the user, and can remain that way for the whole
  *     session, operating silently in the background, if necessary
  * @li The uVision session is a sub-process of UVSC, and can therefore be reliably
  *     terminated by UVSC when the connection is closed
  *
  * <b>To connect to an exisiting uVision session (not recommended):</b>
  * 
  * @li @a pPort must be the UVSOCK port number of the existing uVision session
  * @li @a uvCmd must be NULL  
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_OpenConnection(char *name, 
                                                int *iConnHandle, 
                                                int *pPort,
                                                char *uvCmd,
                                                UVSC_RUNMODE uvRunmode,                                                
                                                uvsc_cb callback, 
                                                void* cb_custom, 
                                                char *logFileName,
                                                xBOOL logFileAppend, 
                                                log_cb logCallback);

/** Close a connection to a uVision session
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param terminate Pass 'xTRUE' to terminate uVision. This parameter is ignored and
  *        the session is automatically terminated if it was auto-started
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * This function closes a connection to a uVision session. If the session was 
  * auto-started it is automatically terminated. If it was not auto-started, it
  * can be terminated, or left running.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_CloseConnection(int iConnHandle, xBOOL terminate);

/** Get a connection handle (@a iConnHandle) from a connection name
  * @param name Pointer to a zero terminated string containing a unique connection name.
  *        The name should correspond to a name that was previously passed to 
  *        #UVSC_OpenConnection. This pointer must not be NULL
  * @param iConnHandle Pointer to a variable to contain the handle assigned to this connection.
  *        The value assigned will be invalid if the function returns an error. This 
  *        pointer must not be NULL
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to convert a connection name to a connection handle (@a iConnHandle)
  * to be used with other UVSC API functions.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_ConnHandleFromConnName(char *name, int *iConnHandle);

/** Retreive detailed information on the last UVSOCK error.
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param msgType Pointer to a variable to contain the UVSOCK message that caused
  *        the error
  * @param status Pointer to a variable to contain status code of the UVSOCK message
  *        that caused the error
  * @param str Pointer to a buffer to contain the zero terminated error string of 
  *        the UVSOCK message that caused the error
  * @param maxStrLen Size of the buffer pointer to by @a str (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre The last call to UVSC_XXXX should have returned an error
  *
  * Use this function to get extended error information when a UVSC command function
  * (eg #UVSC_DBG_CALC_EXPRESSION) fails. It is thread-safe and returns the last error for
  * the calling thread, not the last error for the entire connection, so should be
  * called from the same thread as the UVSC command function that caused the error.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GetLastError(int iConnHandle, UV_OPERATION *msgType, UV_STATUS *status, char *str, int maxStrLen);

/** Control message logging
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param enableRaw Pass 'xTRUE' to enable logging of raw UVSOCK data, or 'xFALSE' to 
  *        disable it. The default option on connection is 'xFALSE'
  * @param enableTrace Pass 'xTRUE' to enable logging of UVSC function trace, or 'xFALSE' to 
  *        disable it. The default option on connection is 'xTRUE'
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre Logging should be enabled for this connection handle (ie one or both of 
  *      @a logFileName and @a logCallback passed to #UVSC_OpenConnection should not be NULL)
  *
  * Use this function to control what information is logged via the log file or
  * logging callback.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_LogControl (int iConnHandle, xBOOL enableRaw, xBOOL enableTrace);


// Messaging functions

/** Set UVSOCK options 
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pOptions pointer UVSOCK_OPTIONS contaning option flags 
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre The call can be issued only once after #UVSC_OpenConnection
  *
  * Use this function to set UVSOCK options.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GEN_SET_OPTIONS (int iConnHandle, UVSOCK_OPTIONS *pOptions);


/** Get the uVision UVSOCK interface version
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pMajor Pointer to a variable to contain the major version number
  * @param pMinor Pointer to a variable to contain the minor version number
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to get the uVision UVSOCK interface version. The version number
  * is of the format:
  *
  * <pre> UVSOCK Version = V*pMajor.*pMinor</pre>
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GEN_UVSOCK_VERSION(int iConnHandle, UINT *pMajor, UINT *pMinor);

/** Get extended uVision versions
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pVersions Pointer to a structure to contain the extended version information. This is
  *        a variable length structure, and must be large enough to contain the returned
  *        data, otherwise an error code will be returned from the function
  * @param pVersionsLen Pointer to a variable containing the size of the @a pVersions data (bytes). This variable will be 
  *        updated with the actual length of @a pVersions returned.
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to get uVision version information as strings.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GEN_EXT_VERSION(int iConnHandle, EXTVERS *pVersions, UINT *pVersionsLen);

/** Hide the uVision window
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to completely hide the uVision window. uVision continues
  * to run as normal, but the uVision window is no longer visible.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GEN_HIDE(int iConnHandle);

/** Show the uVision window
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to show the uVision window. The uVision window will be brought
  * to the front of the windows Z-order.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GEN_SHOW(int iConnHandle);

/** Maximize the uVision window
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to maximize the uVision window. This command performs the
  * same operation as clicking the window [MAXIMIZE] button. If the uVision window is hidden
  * it will be shown.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GEN_MAXIMIZE(int iConnHandle);

/** Minimize the uVision window
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to minimize the uVision window. This command performs the
  * same operation as clicking the window [_] button. If the uVision window is hidden
  * it will be shown.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GEN_MINIMIZE(int iConnHandle);

/** Restore the uVision window
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to restore the uVision window. This command performs the
  * same operation as clicking the window [RESTORE] button. If the uVision window is hidden
  * it will be shown.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GEN_RESTORE(int iConnHandle);

/** Lock the uVision window
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to completely lock the uVision window. uVision continues
  * to run as normal, but the uVision window cannot be interacted with.
  * All message boxes that would normally be shown to the user are also
  * supressed. In the case where a message box has multiple options, the
  * default option will be automatically selected.
  *
  * UI locking is performed automatically for all UVSC functions
  * where it is required.
  *
  * NOTE: UI locking is reference counted within uVision, therefore
  * calls to #UVSC_GEN_UI_LOCK and #UVSC_GEN_UI_UNLOCK must be balanced.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GEN_UI_LOCK(int iConnHandle);

/** Unlock the uVision window
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to unlock the uVision window after it has
  * been locked with #UVSC_GEN_UI_LOCK.
  *
  * NOTE: UI locking is reference counted within uVision, therefore
  * calls to #UVSC_GEN_UI_LOCK and #UVSC_GEN_UI_UNLOCK must be balanced.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GEN_UI_UNLOCK(int iConnHandle);

/** Get uVision license status
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pLicInfo Pointer to a structure to contain the uVision license information
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to get the uVision license status.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GEN_CHECK_LICENSE(int iConnHandle, UVLICINFO *pLicInfo);

/** Load a project into uVision
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pProjectFile Pointer to a structure containing the path and filename of the 
  *        project to load. This is a variable length structure
  * @param projectFileLen Size of the @a pProjectFile data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is not building, downloading, or debugging
  *
  * Use this function to load a project into uVision. The current project (if any) will be
  * saved and closed.
  * 
  * This is the equivalent of clicking the 'Project-->Open Project...' menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_LOAD(int iConnHandle, PRJDATA *pProjectFile, int projectFileLen);

/** Close the current uVision project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to close the current uVision project, saving all the project
  * and files. 
  * 
  * This is the equivalent of clicking the 'Project-->Close Project' menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_CLOSE(int iConnHandle);

/** Add a file to a group within the current uVision project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pGroupFiles Pointer to a structure containing the group name, and filename(s) for
  *        the files to add to the group. This is a variable length structure
  * @param groupFilesLen Size of the @a pGroupFiles data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid, writable project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to add a file to a group within the current uVision project.
  *
  * This is the equivalent of clicking the 'Add Files to Group 'x'' group context menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_ADD_FILE(int iConnHandle, PRJDATA *pGroupFiles, int groupFilesLen);

/** Remove a file from a group within the current uVision project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pGroupFiles Pointer to a structure containing the group name, and filename(s) for
  *        the files to remove from the group. This is a variable length structure
  * @param groupFilesLen Size of the @a pGroupFiles data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid, writable project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to remove a file from a group within the current uVision project.
  * 
  * This is the equivalent of clicking the 'Remove File 'x'' file context menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_DEL_FILE(int iConnHandle, PRJDATA *pGroupFiles, int groupFilesLen);

/** Enumerate the files within a project group
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pGroup Pointer to a structure containing the group for which to enumerate the files
  * @param groupLen Length of the @a pGroup data
  * @param pFiles Pointer to a structure to contain the file(s)
  * @param pFileIndexes Pointer to a variable to contain the file index(es) within the
  *        @a pFiles data.
  * @param pFileCount Pointer to a variable to containing the number of items
  *        it is possible to fit into the @a pFiles and @a pFileIndexes structures. This
  *        variable will be updated with the actual number of files returned.
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to get a list of all the files that exist within the
  * specified group.
  *
  * The pointers @a pFiles and @a pFileIndexes must point to memory which is a multiple of 
  * the size of these structures. This multiple must be passed in the variable @a pFileCount.
  * The multiple must make the @a pFiles memory space large enough to contain the maximum
  * number of files that may be returned. If the memory area is not large enough for the
  * returned data, the function will return an error. A recommended value is 256.
  *
  * The #SSTR structure is designed to contain variable length data, therefore it may
  * be the case that one record within @a pFiles actually takes up more than 1 index.
  * Because of this, the returned data should be interpreted in the following way:
  *
  * <pre>  
    for (i=0; i<*pFileCount; i++) {
      pointer_to_ith_file = pFiles[pFileIndexes[i]];
    }
  * </pre>
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_ENUM_FILES(int iConnHandle, SSTR *pGroup, int groupLen, SSTR *pFiles, int *pFileIndexes, int *pFileCount);

/** Add a group to the current uVision project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pGroups Pointer to a structure containing the group name(s) for
  *        the groups to add to the uVision project. This is a variable length structure
  * @param groupsLen Size of the @a pGroupFiles data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid, writable project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to add a group to the current uVision project.
  *
  * This is the equivalent of clicking the 'New Group' Project context menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_ADD_GROUP(int iConnHandle, PRJDATA *pGroups, int groupsLen);

/** Remove a group from the current uVision project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pGroups Pointer to a structure containing the group name(s) for
  *        the groups to remove from the uVision project. This is a variable length structure
  * @param groupsLen Size of the @a pGroupFiles data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid, writable project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to remove a group from the current uVision project.
  * 
  * This is the equivalent of clicking the 'Remove Group 'x' and its Files' group context menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_DEL_GROUP(int iConnHandle, PRJDATA *pGroups, int groupsLen);

/** Select a target in the current uVision project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pTargets Pointer to a structure containing the target name for
  *        the target to set active in the uVision project. This is a variable length structure
  * @param targetsLen Size of the @a pGroupFiles data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid, writable project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to set a target in the current uVision project to active.
  *
  * This is the equivalent of clicking the 'Target' Combo Box in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_SET_TARGET(int iConnHandle, PRJDATA *pTargets, int targetsLen);


/** Sets a output name in the current uVision project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pTargets Pointer to a structure containing the target name for
  *        the target to set output name in the uVision project. This is a variable length structure
  * @param targetsLen Size of the @a pGroupFiles data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid, writable project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to set an output name in the current uVision project.
  *
  * This is the equivalent of setting the output name in Target Options dialog.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_SET_OUTPUTNAME(int iConnHandle, PRJDATA *pTargets, int targetsLen);



/** Enumerate the groups within the current project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pGroups Pointer to a structure to contain the group(s)
  * @param pGroupIndexes Pointer to a variable to contain the group index(es) within the
  *        @a pGroups data.
  * @param pGroupCount Pointer to a variable to containing the number of items
  *        it is possible to fit into the @a pGroups and @a pGroupIndexes structures. This
  *        variable will be updated with the actual number of groups returned.
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to get a list of all the groups that exist within the
  * current project.
  *
  * The pointers @a pGroups and @a pGroupIndexes must point to memory which is a multiple of 
  * the size of these structures. This multiple must be passed in the variable @a pGroupCount.
  * The multiple must make the @a pGroups memory space large enough to contain the maximum
  * number of groups that may be returned. If the memory area is not large enough for the
  * returned data, the function will return an error. A recommended value is 256.
  *
  * The #SSTR structure is designed to contain variable length data, therefore it may
  * be the case that one record within @a pGroups actually takes up more than 1 index.
  * Because of this, the returned data should be interpreted in the following way:
  *
  * <pre>  
    for (i=0; i<*pGroupCount; i++) {
      pointer_to_ith_group = pGroups[pGroupIndexes[i]];
    }
  * </pre>
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_ENUM_GROUPS(int iConnHandle, SSTR *pGroups, int *pGroupIndexes, int *pGroupCount);

/** Enumerate the targets within the current project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pTargets Pointer to a structure to contain the target(s)
  * @param pTargetIndexes Pointer to a variable to contain the target index(es) within the
  *        @a pTarget data.
  * @param pTargetCount Pointer to a variable to containing the number of items
  *        it is possible to fit into the @a pTargets and @a pTargetIndexes structures. This
  *        variable will be updated with the actual number of targets returned.
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to get a list of all the targets that exist within the
  * current project.
  *
  * The pointers @a pTargets and @a pTargetIndexes must point to memory which is a multiple of 
  * the size of these structures. This multiple must be passed in the variable @a pTargetCount.
  * The multiple must make the @a pTargets memory space large enough to contain the maximum
  * number of target that may be returned. If the memory area is not large enough for the
  * returned data, the function will return an error. A recommended value is 256.
  *
  * The #SSTR structure is designed to contain variable length data, therefore it may
  * be the case that one record within @a pGroups actually takes up more than 1 index.
  * Because of this, the returned data should be interpreted in the following way:
  *
  * <pre>  
    for (i=0; i<*pTargetCount; i++) {
      pointer_to_ith_target = pTargets[pTargetIndexes[i]];
    }
  * </pre>
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_ENUM_TARGETS(int iConnHandle, SSTR *pTargets, int *pTargetIndexes, int *pTargetCount);


/** Get the number of active files in the current project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pActiveFileCount Pointer to a variable to contain the active file count
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to get the number of active files in the current project. This
  * is the number of files that will be compiled or assembled when a rebuild is
  * performed.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_ACTIVE_FILES(int iConnHandle, UINT *pActiveFileCount);

/** Build the current uVision project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param rebuild 'xTRUE':= a rebuild is performed, 'xFALSE':= a build is performed
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to build or rebuild the current uVision project. 
  *
  * This is the equivalent of clicking the 'Project-->Build', or 'Project-->Rebuild all target files' menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_BUILD(int iConnHandle, xBOOL rebuild);

/** Cancel a build that is currently in progress
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A project build is in progress
  *
  * Use this function to cancel a build that is currently in progress. In the case
  * of cancelling a UVSC initiated build, it must be called from a different thread
  * than the #UVSC_PRJ_BUILD function, because the build function will not return
  * until the build is complete. 
  * 
  * This is the equivalent of clicking the 'Project-->Stop Build' menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_BUILD_CANCEL(int iConnHandle);

/** Clean the current uVision project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to clean the current uVision project. 
  *
  * This is the equivalent of clicking the 'Project-->Clean target' menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_CLEAN(int iConnHandle);

/** Download the currently built executable to FLASH
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, has been built successfully, and uVision is not building, downloading, or debugging
  *
  * Use this function to download the currently built executable to FLASH. 
  * 
  * This is the equivalent of clicking the 'Flash-->Download' menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_FLASH_DOWNLOAD(int iConnHandle);

/** Get a uVision project option
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pTrnOpt Pointer to a structure containing the option item to get. The
  *        structure will be updated with the option item value. This is
  *        a variable length structure, and must be large enough to contain the returned
  *        data, otherwise an error code will be returned from the function, see #OPT_LMISC
  * @param trnOptLen Size of the @a pTrnOpt data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to get the value of a uVision project option. 
  * 
  * This is the equivalent of modifying settings in the 'Options for XYZ' dialogs in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_GET_OPTITEM(int iConnHandle, TRNOPT *pTrnOpt, int trnOptLen);

/** Set a uVision project option
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pTrnOpt Pointer to a structure containing the option item to set, and the
  *        value to set it to. This is a variable length structure, see optsel in UVSOCK.h
  * @param trnOptLen Size of the @a pTrnOpt data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid, writable project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to set the value of a uVision project option. 
  * 
  * This is the equivalent of reading settings in the 'Options for XYZ' dialogs in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_SET_OPTITEM(int iConnHandle, TRNOPT *pTrnOpt, int trnOptLen);

/** Set the debug uVision target configuration
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pDbgTgtOpt Pointer to a structure to contain the debug target configuration
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded
  *
  * Use this function to get the debug uVision target configuration
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_GET_DEBUG_TARGET(int iConnHandle, DBGTGTOPT *pDbgTgtOpt);

/** Get the debug uVision target configuration
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pDbgTgtOpt Pointer to a structure containing the debug target configuration to set
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid, writable project is loaded, and uVision is not building, downloading, or debugging
  *
  * Use this function to set the debug uVision target configuration
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_SET_DEBUG_TARGET(int iConnHandle, DBGTGTOPT *pDbgTgtOpt);

/** Control the uVision UI progress bar
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pPgress Pointer to a structure containing the progress bar data set. 
  *        This is a variable length structure
  * @param pgressLen Size of the @a pPgress data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to control the uVision UI progress bar. This can be useful to 
  * display a progress bar relating to time consuming client operations via the uVision
  * progress bar.
  * 
  * This is the equivalent of reading settings in the 'Options for XYZ' dialogs in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_CMD_PROGRESS(int iConnHandle, PGRESS *pPgress, int pgressLen);

/** Get the executable/library output object name for the current project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param piPathRq Pointer to a structure containing the path request options
  * @param pName Pointer to a structure to containing the output object name
  * @param nameLen Length of the @a pName data
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid, writable project is loaded, and uVision is not building, downloading, or debugging
  *
  * The #SSTR structure is designed to contain variable length data, therefore it may
  * be the case that the record within @a piPathRq actually takes up more than the static structure size.
  *
  * Use this function to retrieve the path of the executable or library created by uVision when a project is built
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_GET_OUTPUTNAME(int iConnHandle, iPATHREQ *piPathRq, SSTR *pName, int nameLen);

/** Get the current active target name for the current project
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param piPathRq Pointer to a structure containing the options (Set to 0 for now, unused)
  * @param pName Pointer to a structure to containing the current active target name
  * @param nameLen Length of the @a pName data
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid, writable project is loaded, and uVision is not building, downloading, or debugging
  *
  * The #SSTR structure is designed to contain variable length data, therefore it may
  * be the case that the record within @a piPathRq actually takes up more than the static structure size.
  *
  * Use this function to retrieve the current active target name in uVision
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_PRJ_GET_CUR_TARGET(int iConnHandle, iPATHREQ *piPathRq, SSTR *pName, int nameLen);


/** Enter debug mode
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, a binary has been built, and uVision is not building, downloading, or debugging
  *
  * Use this function to put uVision into debug mode.
  *
  * This is the equivalent of clicking the 'Debug-->Start/Stop Debug Session' menu item in uVision (when not in debug mode).
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_ENTER(int iConnHandle);

/** Exit debug mode
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to exit debug mode.
  *
  * This is the equivalent of clicking the 'Debug-->Start/Stop Debug Session' menu item in uVision (when in debug mode).
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_EXIT(int iConnHandle);

/** Start execution
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  *
  * Use this function to start the target or simulation execution.
  *
  * This is the equivalent of clicking the 'Debug-->Run' menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_START_EXECUTION(int iConnHandle);

/** Start execution and run to specified address
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param address (xU64) address to run to
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  *
  * Use this function to start the target or simulation execution.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_RUN_TO_ADDRESS(int iConnHandle, xU64 address);


/** Stop execution
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and executing
  *
  * Use this function to stop the target or simulation execution.
  *
  * This is the equivalent of clicking the 'Debug-->Stop Running' menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_STOP_EXECUTION(int iConnHandle);

/** Get execution status
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pStatus Pointer to a variable to contain the execution status. 1:= the
  *        target is executing, 0:= the target is stopped.
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to find out if the target or simulator is executing, or
  * is stopped.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_STATUS(int iConnHandle, int *pStatus);

/** Step one High Level Language (HLL) code line
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  *
  * Use this function to step one High Level Language (HLL) code line.
  *
  * This is the equivalent of clicking the 'Debug-->Step Over' menu item in uVision (when viewing HLL code).
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_STEP_HLL(int iConnHandle);

/** Step N High Level Language (HLL) code lines
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param count (UINT) - number of steps to perform
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  *
  * Use this function to N High Level Language (HLL) code lines.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_STEP_HLL_N(int iConnHandle, UINT count);


/** Step one ASM instruction
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  *
  * Use this function to step one ASM instruction.
  *
  * This is the equivalent of clicking the 'Debug-->Step' menu item in uVision (when viewing ASM code).
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_STEP_INSTRUCTION(int iConnHandle);

/** Step N ASM instructions
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  * @param count (UINT) - number of steps to perform
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  *
  * Use this function to step N ASM instructions.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_STEP_INSTRUCTION_N(int iConnHandle, UINT count);


/** Step into the current High Level Language (HLL) function
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  *
  * Use this function to step into the current High Level Language (HLL) function.
  *
  * This is the equivalent of clicking the 'Debug-->Step' menu item in uVision (when viewing HLL code).
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_STEP_INTO(int iConnHandle);

/** Step into the current High Level Language (HLL) function
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  * @param count (UINT) - number of steps to perform
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  *
  * Use this function to step into the current High Level Language (HLL) function.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_STEP_INTO_N(int iConnHandle, UINT count);


/** Step out of the current High Level Language (HLL) function
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  *
  * Use this function to step out of the current High Level Language (HLL) function.
  *
  * This is the equivalent of clicking the 'Debug-->Step out of current function' menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_STEP_OUT(int iConnHandle);

/** Reset the target
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to reset the target processor or simulation.
  *
  * This is the equivalent of clicking the 'Peripherals-->Reset CPU' menu item in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_RESET(int iConnHandle);

/** Read memory
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pMem Pointer to a structure containing the information about the memory to read.
  *        This variable will be updated with the data read from memory. This is
  *        a variable length structure, and must be large enough to contain the returned
  *        data, otherwise an error code will be returned from the function
  * @param memLen Size of the @a pMem data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to read target memory.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_MEM_READ(int iConnHandle, AMEM *pMem, int memLen);

/** Write memory
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pMem Pointer to a structure containing the information about the memory to write,
  *        including the data to write. This is a variable length structure
  * @param memLen Size of the @a pMem data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to write target memory.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_MEM_WRITE(int iConnHandle, AMEM *pMem, int memLen);

/** Create a breakpoint
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pBkptSet Pointer to a structure containing the new breakpoint data. This is
  *        a variable length structure.
  * @param bkptSetLen Size of the @a pBkptSet data (bytes)
  * @param pBkptRsp Pointer to a structure to contain the new breakpoint information. This is
  *        a variable length structure, and must be large enough to contain the returned
  *        data, otherwise an error code will be returned from the function.
  * @param pBkptRspLen Size of the @a pBkptRsp data (bytes). This variable will be
  *        updated with the actual length of data written to the @a pBkptRsp structure
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to create a new breakpoint in uVision. The information returned
  * in the @a pBkptRsp structure contains @a nTickMark which can be used to identify this
  * breakpoint in the future to other UVSC command functions.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_CREATE_BP(int iConnHandle, BKPARM *pBkptSet, int bkptSetLen, BKRSP *pBkptRsp, int *pBkptRspLen);

/** Modify an existing breakpoint
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pBkptChg Pointer to a structure containing the breakpoint to change. This is
  *        a variable length structure.
  * @param bkptChgLen Size of the @a pBkptChg data (bytes)
  * @param pBkptRsp Pointer to a structure to contain the new breakpoint information. This is
  *        a variable length structure, and must be large enough to contain the returned
  *        data, otherwise an error code will be returned from the function.
  * @param pBkptRspLen Size of the @a pBkptRsp data (bytes). This variable will be
  *        updated with the actual length of data written to the @a pBkptRsp structure
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to modify an exisiting breakpoint in uVision.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_CHANGE_BP(int iConnHandle, BKCHG *pBkptChg, int bkptChgLen, BKRSP *pBkptRsp, int *pBkptRspLen);

/** Enumerate all breakpoints
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pBkptRsp Pointer to a structure to contain the breakpoint(s) information
  * @param pBkptIndexes Pointer to a variable to contain the breakpoint index(es) within the
  *        @a pBkptRsp data.
  * @param pBkptCount Pointer to a variable to containing the number of items
  *        it is possible to fit into the @a pBkptRsp and @a pBkptIndexes structures. This
  *        variable will be updated with the actual number of breakpoints returned.
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to get a list of all the breakpoints that exist in uVision, and
  * their current status. 
  *
  * The pointers @a pBkptRsp and @a pBkptIndexes must point to memory which is a multiple of 
  * the size of these structures. This multiple must be passed in the variable @a pBkptCount.
  * The multiple must make the @a pBkptRsp memory space large enough to contain the maximum
  * number of breakpoints that may be returned. If the memory area is not large enough for the
  * returned data, the function will return an error. A recommended value is 256.
  *
  * The #BKRSP structure is designed to contain variable length data, therefore it may
  * be the case that one record within @a pBkptRsp actually takes up more than 1 index.
  * Because of this, the returned data should be interpreted in the following way:
  *
  * <pre>  
    for (i=0; i<*pBkptCount; i++) {
      pointer_to_ith_Breakpoint = pBkptRsp[pBkptIndexes[i]];
    }
  * </pre>
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_ENUMERATE_BP(int iConnHandle, BKRSP *pBkptRsp, int *pBkptIndexes, int *pBkptCount);

/** DEPRECATED: use DBG_SERIAL_OUTPUT response
  * Get terminal data from a uVision terminal window
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pSerIO Pointer to a structure containing the information about the terminal window to 
  *        get data from. This variable will be updated with the data read from the terminal window. This is
  *        a variable length structure, and must be large enough to contain the returned
  *        data, otherwise an error code will be returned from the function.
  * @param serIOLen Size of the @a pSerIO data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to get terminal data from a uVision terminal window.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_SERIAL_GET(int iConnHandle, SERIO *pSerIO, int serIOLen);

/** Send terminal data to a uVision terminal window
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pSerIO Pointer to a structure containing the information about the terminal window to write
  *        data to, including the data to write. This is a variable length structure
  * @param serIOLen Size of the @a pSerIO data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to send terminal data to a uVision terminal window.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_SERIAL_PUT(int iConnHandle, SERIO *pSerIO, int serIOLen);

/** Calculate the value of an expression
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pVSet Pointer to a structure containing the expression to calculate. This
  *        variable will be updated with the result of the expression calculation. This is
  *        a variable length structure.
  * @param vSetLen Size of the @a pVSet data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to calculate the value of an expression, in the same way
  * an expression would be calculated via the uVision command window. This function
  * can also be used to read and write real and virtual registers.
  *
  * Example 1: Evaluate the address of the function 'main()'
  * <pre> Set pVSet->str to "&main" </pre>
  *
  * Example 2: Read the value of R0
  * <pre> Set pVSet->str to "R0" </pre>
  *
  * Example 3: Modify the value of PC to 0x99
  * <pre> Set pVSet->str to "PC = 0x99" </pre>
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_CALC_EXPRESSION(int iConnHandle, VSET *pVSet, int vSetLen);

/** Evaluate watch expression
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pVSet pointer to a VSET structure:
  *             TVAL member: stack frame address (VTT_uint64) (reserved)
  *             SSTR contains watch expression.
  * @param vSetLen Size of the @a pVSet data (bytes)
  * @param pVarInfo Pointer to a variable to receive watch expression result 
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  * @pre Extended stack mode is set using #UVSC_GEN_SET_OPTIONS call
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_EVAL_WATCH_EXPRESSION(int iConnHandle, VSET *pVSet, int vSetLen, VARINFO* pVarInfo);


/** Remove watch expression added by UVSC_DBG_EVAL_WATCH_EXPRESSION
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pVSet pointer to a VSET structure:
  *             SSTR contains watch expression.
  * @param vSetLen Size of the @a pVSet data (bytes)
  
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  * @pre Extended stack mode is set using #UVSC_GEN_SET_OPTIONS call
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_REMOVE_WATCH_EXPRESSION(int iConnHandle, VSET *pVSet, int vSetLen);


/** Enumerarte variables of stack frame, watch expression or variable members/array elements 
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param iVarEnum - pointer to IVARENUM structure containing task, stack frame and variable ID together with request flags
  * @param pVarInfo pointer to array of VARINFO sturctures to receive member infos 
  * @param nVarInfoCount pointer variable containing nember of VARINFO to receive member infos 
           on return its value is updated with actual number read
  
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  * @pre Extended stack mode is set using #UVSC_GEN_SET_OPTIONS call
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_ENUM_VARIABLES(int iConnHandle, IVARENUM* iVarEnum, VARINFO* pVarInfo, int* nVarInfoCount);


/** Set variable value 
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pVar    pointer to a VARVAL structure that contains:
  *                variable member contains full variable ID as IVARENUM structure
  *                value member contains variable value as string expression in SSTR cturcture
  * @param vVarLen size of VARVAL stucture
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is debugging
  * @pre Extended stack mode is set using #UVSC_GEN_SET_OPTIONS call
  *
  * Use this function to set a new value to a variable
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_VARIABLE_SET(int iConnHandle, VARVAL *pVar, int vVarLen);



/** Enumerate the device Virtual Registers (VTRs)
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param piVtrEnum Pointer to a structure containing the type of VTR enumeration to perform
  * @param paVTR Pointer to a structure to contain the VTR(s)
  * @param pVtrIndexes Pointer to a variable to contain the VTR index(es) within the
  *        @a paVTR data.
  * @param pVtrCount Pointer to a variable to containing the number of items
  *        it is possible to fit into the @a paVTR and @a pVtrIndexes structures. This
  *        variable will be updated with the actual number of VTRs returned
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  *
  * Use this function to get a list of all the VTRs that exist within the target
  * device. Optionally their values can also be retreived
  *
  * The pointers @a paVTR and @a pVtrIndexes must point to memory which is a multiple of 
  * the size of these structures. This multiple must be passed in the variable @a pVtrCount.
  * The multiple must make the @a paVTR memory space large enough to contain the maximum
  * number of VTRs that may be returned. If the memory area is not large enough for the
  * returned data, the function will return an error. A recommended value is 256.
  *
  * The #AVTR structure is designed to contain variable length data, therefore it may
  * be the case that one record within @a paVTR actually takes up more than 1 index.
  * Because of this, the returned data should be interpreted in the following way:
  *
  * <pre>  
    for (i=0; i<*pVtrCount; i++) {
      pointer_to_ith_VTR = paVTR[pVtrIndexes[i]];
    }
  * </pre>
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_ENUM_VTR(int iConnHandle, iVTRENUM *piVtrEnum, AVTR *paVTR, int *pVtrIndexes, int *pVtrCount);

/** Get the value of a virtual register (VTR)
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pVSet Pointer to a structure containing the VTR to get. This structure will
  *        be updated with the VTR value. This is a variable length structure
  * @param vSetLen Size of the @a pVSet data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to get the value of a virtual register (VTR).
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_VTR_GET(int iConnHandle, VSET *pVSet, int vSetLen);

/** Set the value of a virtual register (VTR)
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pVSet Pointer to a structure containing the VTR to set, and the value
  *        to set it to. This is a variable length structure
  * @param vSetLen Size of the @a pVSet data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to set the value of a virtual register (VTR).
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_VTR_SET(int iConnHandle, VSET *pVSet, int vSetLen);

/** Enumerate stack frames
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param piStkEnum Pointer to a structure containing the type of stack enumeration to perform
  * @param pStackEnum Pointer to a structure to contain the stack frame(s)
  * @param pStackCount Pointer to a variable to containing the number of items
  *        it is possible to fit into the @a pStackEnum structure. This variable will be 
  *        updated with the actual number of stack frames returned
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  *
  * Use this function to get a list of the current stack frames.
  *
  * The pointer @a pStackEnum must point to memory which is a multiple of the size of 
  * the #STACKENUM structure. This multiple must be passed in the variable @a pStackCount.
  * The multiple must be at least equal to the maximum number of stack frames that may be
  * returned. A recommended value is 256.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_ENUM_STACK(int iConnHandle, iSTKENUM *piStkEnum, STACKENUM *pStackEnum, int *pStackCount);


/** Enumerate task list
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pTaskEnum Pointer to a TASKENUM structure to contain the task items
  * @param pTaskCount Pointer to a variable to containing the number of items
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and not executing
  * @pre Extended stack mode is set using #UVSC_GEN_SET_OPTIONS call
  *
  * Use this function to get a list of the current RTX task
  *          in non-RTX mode default implicit main thread is returned
  *
  * The pointer @a pTaskEnum must point to memory which is a multiple of the size of 
  * the #TASKENUM structure. This multiple must be passed in the variable @a pTaskCount.
  * The multiple must be at least equal to the maximum number of task items that may be
  * returned. A recommended value is 256.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_ENUM_TASKS(int iConnHandle, TASKENUM *pTaskEnum, int *pTaskCount);


/** Enumerate dynamic menus
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pMenuID Pointer to a structure containing the type of menu enumeration to perform
  * @param pViewEnum Pointer to a structure to contain the menu items
  * @param pMenuCount Pointer to a variable to containing the number of items
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to get a list of the available uVision dynamic menus to execute remotely 
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_ENUM_MENUS(int iConnHandle, MENUID *pMenuID, MENUENUM *pViewEnum, int *pMenuCount);


/** Execute meu item obtained by calling UVSC_DBG_ENUM_MENUS
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pMenuID Pointer to a structure containing menu type and its ID 
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_MENU_EXEC(int iConnHandle, MENUID *pMenuID);


/** Convert a code address to a High Level Language (HLL) file, line and function
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pAdrMtfl Pointer to a structure containing the type of HLL information to retreive
  * @param pAflMap Pointer to a structure to contain the HLL code information. This is
  *        a variable length structure, and must be large enough to contain the returned
  *        data, otherwise an error code will be returned from the function
  * @param pAflMapLen Pointer to a variable containing the size of the @a pAflMap data (bytes).
  *        This variable will be updated with the length of the @a pAflMap data returned
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to convert a code address to a High Level Language (HLL) 
  * file, line and function.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_ADR_TOFILELINE(int iConnHandle, ADRMTFL *pAdrMtfl, AFLMAP *pAflMap, int *pAflMapLen);

/** Show code at a specific address within uVision
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param piShowSync Pointer to a structure containing the type of code to show
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to show code at a specific address within uVision. Either
  * ASM code, HLL code, or both can shown.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_ADR_SHOWCODE(int iConnHandle, iSHOWSYNC *piShowSync);

/** Show code at a specific time stamp within uVision
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pPowerScaleData Pointer to a UVSC_PSTAMP structure containing the time stamp to show code and trace
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to show code at a specific time stamp within uVision. The time stamp
  * consists of ticks since reset and time since last tick.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_POWERSCALE_SHOWCODE(int iConnHandle, UVSC_PSTAMP *pPowerScaleData);   // [TdB: 22.11.2010] Show Code for PowerScale


/** Wake the simulation [SIMULATOR ONLY]
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param piInterval Pointer to a structure containing the information on how long
  *        the simulation should wake for.
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and is targetting the simulator
  *
  * Use this function to simulate I/O to and from the uVision simulator in a synchronous 
  * manner. 
  * 
  * The function call can run the simulation for a specified time, after which the simulation
  * is put to sleep and a notification generated. At this point, simulation virtual
  * registers (ie device IO) can be read and written, and then this message can be sent again to 
  * wake the simulation again. This sequence can be used to simulate I/O to and from the 
  * simulator in a synchronous manner.
  *
  * When the simulator is asleep, simulation is paused, but uVision still shows the simulation as 
  * running.
  *
  * NOTE: The simulator will automatically wake up after 500ms if it is not told
  * to wake up by another #UVSC_DBG_WAKE command.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_WAKE(int iConnHandle, iINTERVAL *piInterval);

/** Sleep the simulation [SIMULATOR ONLY]
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode, and is targetting the simulator
  *
  * Use this function to put the simulation to sleep. When the simulator is
  * asleep, simulation is paused, but uVision still shows the simulation as 
  * running.
  *
  * NOTE: The simulator will automatically wake up after 500ms if it is not told
  * to wake up by a #UVSC_DBG_WAKE command.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_SLEEP(int iConnHandle);

/** Execute a command (as if via the command line)
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pCmd Pointer to a structure containing the command line to execute in uVision. 
  *        This is a variable length structure
  * @param cmdLen Length of the data within the @a pCmd
  * @param cmdLen Size of the structure pointed to by @a pCmd.
  *        
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to execute a command in the same way as typing the command in the uVision
  * Command Window.
  *
  * If required, the command output text can be retrieved via the #UVSC_GetCmdOutputSize and
  * #UVSC_GetCmdOutput functions.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_EXEC_CMD(int iConnHandle, EXECCMD *pCmd, int cmdLen);


// Build Output Functions

/** Get the size (bytes) of the latest build output text
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pBuildOutputSize Pointer to a variable to contain the build output text size (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to get the size (bytes) of the latest build output text that will
  * be returned by the #UVSC_GetBuildOutput function.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GetBuildOutputSize(int iConnHandle, int *pBuildOutputSize);

/** Get the latest build output text
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pBuildOutput Pointer to a buffer to contain the build output text
  * @param buildOutputLen Size of the @a pBuildOutput buffer (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A build must have been performed, otherwise no data will be returned
  * @pre #UVSC_GetBuildOutputSize must have been called successfully
  *
  * Use this function to get the latest build output text. First call #UVSC_GetBuildOutputSize
  * so that a correctly sized buffer can be passed to this function.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GetBuildOutput(int iConnHandle, char *pBuildOutput, int buildOutputLen);

// Command Output Functions

/** Get the size (bytes) of the latest command output text (result of #UVSC_DBG_EXEC_CMD)
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pCmdOutputSize Pointer to a variable to contain the command output text size (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to get the size (bytes) of the latest command output text that will
  * be returned by the #UVSC_GetCmdOutput function.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GetCmdOutputSize(int iConnHandle, int *pCmdOutputSize);

/** Get the latest command output text (result of #UVSC_DBG_EXEC_CMD)
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pCmdOutput Pointer to a buffer to contain the command output text
  * @param cmdOutputLen Size of the @a pCmdOutput buffer (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A command must have been performed (#UVSC_DBG_EXEC_CMD), otherwise no data will be returned
  * @pre #UVSC_GetCmdOutputSize must have been called successfully
  *
  * Use this function to get the latest command output text. First call #UVSC_GetCmdOutputSize
  * so that a correctly sized buffer can be passed to this function.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_GetCmdOutput(int iConnHandle, char *pCmdOutput, int cmdOutputLen);


// Queue functions

/** Read build messages from the build queue
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pOutputLine Pointer to a buffer to hold the build output line. The buffer
  *        should be at least #UVSC_MAX_API_STR_SIZE bytes
  * @param timeout Time to wait for a message on the queue before giving up
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to read build messages from the build queue. Build messages
  * are build output lines that contain 'compiling' or 'assembling' or 'linking'
  * or build conclusion information. The messages on this queue can be used by the
  * client to indicate build progress to the user, without the obtrusive warning and
  * error messages. This is especially useful when uVision is being operated in the 
  * background. This queue is flushed automatically at the start of a build.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_ReadBuildQ(int iConnHandle, char *pOutputLine, int timeout);

/** Read progress bar messages from the progress bar queue
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pBar Pointer to an enumeration variable to hold the progress bar info type
  * @param pStr Pointer to a buffer to hold the progress bar information. The buffer
  *        should be at least #UVSC_MAX_API_STR_SIZE bytes
  * @param timeout Time to wait for a message on the queue before giving up
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to read progress bar messages from the progress bar queue. Progress
  * bar messages reflect the progress bar situated at the bottom left corner of the uVision
  * window. They can be used by the client to indicate operation progress. This is
  * especially useful when uVision is being operated in the background. This function could
  * be combined with the #UVSC_PRJ_ACTIVE_FILES and #UVSC_GetBuildOutput functions to 
  * provide the user with a build progress bar. This queue is flushed automatically
  * automatically at the start of a build.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_ReadPBarQ(int iConnHandle, UVSC_PBAR *pBar, char *pStr, int timeout);

/** Read a message from the UVSOCK asynchronous message queue
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param iQueueNo The asynchronous queue number to read
  * @param buf Pointer to a structure to hold the asynchronous UVSOCK message data
  * @param timeout Time to wait for a message on the queue before giving up
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre NULL was passed as the @a callback parameter to #UVSC_OpenConnection.
  *
  * Use this function to read a message from the UVSOCK asynchronous message queue. 
  * Asynchronous UVSOCK messages received will be available via this queue, if 
  * (and only if) NULL is passed as the @a callback parameter to #UVSC_OpenConnection.
  *
  * If the queues are available, there are 3 queues (0, 1 and 2). This is to provide
  * access to the aysnchronous messages for up to 3 tasks within the client application.
  * Queues 1 and 2 will contain all UVSOCK asynchronous messages. Queue 0 ignores
  * #UV_DBG_WAKE and #UV_DBG_SLEEP messages.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_ReadAsyncQ(int iConnHandle, int iQueueNo, UVSOCK_CMD *buf, int timeout);

/** Flush the UVSOCK asynchronous message queue
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param iQueueNo The asynchronous queue number to flush
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre NULL was passed as the @a callback parameter to #UVSC_OpenConnection.
  *
  * Use this function to flush the UVSOCK asynchronous message queue.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_FlushAsyncQ(int iConnHandle, int iQueueNo);


// Advanced functions

/** Send a raw UVSOCK message, and retreive the response
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param buf Pointer to a structure containing the UVSOCK request message data. This
  *        will be replaced by the response message data
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  *
  * Use this function to send a raw UVSOCK message, and retreive the response. This
  * is useful when implementing UVSOCK features that are not yet fully supported in
  * UVSC. This is an advanced feature, and normally is not required.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_TxRxRaw(int iConnHandle, UVSOCK_CMD *buf);

/** Create a UVSOCK message
  * @param buf Pointer to a structure to hold the UVSOCK message data
  * @param eCmd Mssage command to add to the message
  * @param nLen Length of the data in @a pData
  * @param pData Pointer to the data to add to the message
  * @return Equal to the input parameter @a buf
  *
  * Use this function to create a correctly formatted UVSOCK message. This function is
  * useful only in conjunction with the #UVSC_TxRxRaw function. This is an advanced 
  * feature, and normally is not required.
  */
_UVSC_FUNC_ UVSOCK_CMD *UVSC_CreateMsg (UVSOCK_CMD *buf, UV_OPERATION eCmd, int nLen, const void *pData);


/** Evaluate expession and retur result as string 
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pVSet Pointer to a structure containing the expression to calculate. This
  *        variable will be updated with the result of the expression calculation. 
  *        input : pVSet->val   : stack frame address (VTT_uint64)
  *        pVSet-str    : expression to avaluate
  * @param vSetLen Size of the @a pVSet data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to get stack frame based expression evaluation, for example to display a variable value in a tooltip 
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_EVAL_EXPRESSION_TO_STR(int iConnHandle, VSET *pVSet, int vSetLen);


/** Convert  High Level Language (HLL) file and line to an address
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pAflMap Pointer to a structure to contain the HLL code information about file and line 
  * @param nAflMapLen size of the @a pAflMap data (bytes).
  * @param pVSet Pointer to a structure containing calculated address. This variable will be updated with the result of the calculation. 
  * @param vSetLen Size of the @a pVSet data (bytes)
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre uVision is in debug mode
  *
  * Use this function to convert a code address to a High Level Language (HLL) 
  * file, line and function.
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_FILELINE_TO_ADR(int iConnHandle, AFLMAP *pAflMap, int nAflMapLen, VSET *pVSet, int vSetLen);


/** Enumerate register groups 
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pGroups Pointer to a structure to contain the group(s)
  * @param pGroupCount Pointer to a variable to containing the number of items
  *        it is possible to fit into the @a pGroups structures. 
  *        This variable will be updated with the actual number of groups returned.
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is debugging
  *
  * Use this function to get a list of all register groups that exist within the
  * current project.
  *
  * The pointer @a pGroups must point to memory which is a multiple of 
  * the size of these structures. This multiple must be passed in the variable @a pGroupCount.
  * The multiple must make the @a pGroups memory space large enough to contain the maximum
  * number of groups that may be returned. If the memory area is not large enough for the
  * returned data, the function will return an error. A recommended value is 256.
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_ENUM_REGISTER_GROUPS(int iConnHandle, SSTR *pGroups, int *pGroupCount);

/** Enumerate registers
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pRegisters    Pointer to a structure to contain the registers
  * @param pRegisterCount Pointer to a variable containing the number of items
  *        it is possible to fit into the @a pRegisters. 
  *        This variable will be updated with the actual number of registers returned.
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is debugging
  *
  * Use this function to get a list of all register groups that exist within the
  * current project.
  *
  * The pointer @a pRegisters must point to memory which is a multiple of 
  * the size of these structures. This multiple must be passed in the variable @a pRegisterCount.
  * The multiple must make the @a pRegisters memory space large enough to contain the maximum
  * number of registers that may be returned. If the memory area is not large enough for the
  * returned data, the function will return an error. A recommended value is 256.
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_ENUM_REGISTERS(int iConnHandle, REGENUM *pRegisters, int *pRegisterCount);


/** Read all register values 
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pBuf   pointer to a string buffer to contain the register values
  * @param pBufLen pointer to a variable containing the size of pBuf in bytes
  *        This variable will be updated with the actual number of bytes returned.
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is debugging
  *
  * Use this function to get a list of all register groups that exist within the
  * current project.
  *
  * The pointer @a pBuf  must point to memory which is a multiple of the 32 bytes (32*register_count). 
  * The length of this buffer be passed in the variable @a pBufLen.
  * The multiple must make the @a pBuf memory space large enough to contain the maximum
  * number of register values that may be returned. If the memory area is not large enough for the
  * returned data, the function will return an error. A recommended value is 4096.
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_READ_REGISTERS(int iConnHandle, char *pBuf, int *pBufLen);


/** Set register value 
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
  * @param pVSet  pointer to a VSET structure that contains:
  *                TVAL member contains register index as integer
  *                SSTR contains register value as string expression 
  * @param vSetLen size of VSET stucture
  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is debugging
  *
  * Use this function to set a new value to a register
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_REGISTER_SET(int iConnHandle, VSET *pVSet, int vSetLen);

/** Get block of disassembly lines for given address
  * @param iConnHandle Handle for the connection, as returned by #UVSC_OpenConnection
    * @param pMem Pointer to a structure containing the information about the address of disassembly to read.
  *        This variable will be updated with block of disassembly lines. This is
  *        a variable length structure, and must be large enough to contain the returned
  *        data, otherwise an error code will be returned from the function
  * @param memLen Size of the @a pMem data (bytes)

  * @return #UVSC_STATUS_SUCCESS on success, or an error code
  *
  * @pre #UVSC_OpenConnection must have been called successfully
  * @pre A valid project is loaded, and uVision is debugging
  *
  * Use this function to get a block of disassembly lines starting with given address
  * current project.
  *
  */
_UVSC_FUNC_ UVSC_STATUS UVSC_DBG_DSM_READ(int iConnHandle, AMEM *pMem, int memLen);



#ifdef __cplusplus
  }
#endif

#endif /* #define __UVSC_C_H__ */

