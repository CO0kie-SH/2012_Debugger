#include "pch.h"
#include "CDebug.h"

BOOL CDebug::InitDebug(PWCHAR Path)
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION ps = {};
	// 以附加方式启动
	// BOOL bRet = DebugActiveProcess(3008);

	//以调试方式启动进程
	BOOL bRet = CreateProcess(
		Path,											//	调试目标路径
		NULL,												//  命令行参数
		NULL,												//  进程安全属性	
		NULL,												//  线程安全属性	
		FALSE,												//  是否继承句柄表
		DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE,		//  以只调试方式启动进程
		NULL,												//  环境变量
		NULL,												//  工作目录
		&si, &ps);
	if (bRet == FALSE)
	{
		MessageBox(0, L"进程创建失败", 0, 0);
		return 0;
	}

	DEBUG_EVENT dbg_event;
	BOOL Loop_Debug = TRUE;
	DWORD dbg_status = DBG_CONTINUE;  //异常处理了
	// 调试循序
	while (Loop_Debug)
	{
		// 等待调试事件
		WaitForDebugEvent(&dbg_event, -1);

		// 处理调试消息
		switch (dbg_event.dwDebugEventCode)/*判断事件代码*/
		{
		case EXCEPTION_DEBUG_EVENT:     // 异常调试事件
			printf("异常调试事件触发\n");
			//dbg_status = OnExceptionHandler(&dbg_event);
			break;
		case CREATE_THREAD_DEBUG_EVENT: // 线程创建事件
			printf("线程创建事件触发\n");
			break;
		case CREATE_PROCESS_DEBUG_EVENT: {// 进程创建事件
			LPVOID g_Oep = dbg_event.u.CreateProcessInfo.lpStartAddress; // 程序入口
			printf("进程创建事件触发,OEP=%p\n", g_Oep);
		}break;
		case EXIT_THREAD_DEBUG_EVENT:   // 退出线程事件
			printf("线程退出事件触发\n");
			break;
		case EXIT_PROCESS_DEBUG_EVENT:  // 退出进程事件
			Loop_Debug = FALSE;
			printf("进程退出事件触发\n");
			break;
		case LOAD_DLL_DEBUG_EVENT:      // 映射DLL事件
			printf("DLL加载事件触发\n");
			break;
		case UNLOAD_DLL_DEBUG_EVENT:    // 卸载DLL事件 
			printf("DLL卸载事件触发\n");
			break;
		case OUTPUT_DEBUG_STRING_EVENT: // 调试字符串输出事件
			printf("字符串输出事件触发\n");
			break;
		case RIP_EVENT:                 // RIP事件(内部错误)
			break;
		}

		// 回复调试子系统程序处理
		ContinueDebugEvent(
			dbg_event.dwProcessId,   //调试进程ID,必须从DEBUG_EVNET中获取
			dbg_event.dwThreadId,    //调试线程ID,必须从DEBUG_EVNET中获取
			dbg_status);			 //异常是否处理，只对异常有效

	}

	// 关闭线程句柄，线程句柄
	CloseHandle(ps.hProcess);
	CloseHandle(ps.hThread);
	return 0;
}
