#include "pch.h"
#include "CDebug.h"
#include ".\capstone\include\capstone.h"
#include "RegStruct.h"

//1. 包含头文件
#ifdef _WIN64 // 64位平台编译器会自动定义这个宏
#pragma 
comment(lib, "capstone/lib/capstone_x64.lib")
#else
#pragma comment(lib,".\\capstone\\lib\\capstone_x86.lib")
#endif // _64

BOOL CDebug::InitDebug(PWCHAR Path)
{
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION ps;
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

	gDATA.SetPS(&ps);
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
			dbg_status = OnExceptionHandler(&dbg_event);
			break;
		case CREATE_THREAD_DEBUG_EVENT: // 线程创建事件
			printf("线程创建事件触发\n");
			break;
		case CREATE_PROCESS_DEBUG_EVENT: {// 进程创建事件
			gDATA.OEP = dbg_event.u.CreateProcessInfo.lpStartAddress; // 程序入口
			printf("进程创建事件触发,OEP=%p\n", gDATA.OEP);
			this->DisASM(gDATA.OEP, 5);
			printf("增加OEP软件断点=%d\n",
				this->AddSoftPoint(gDATA.OEP));
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

	CloseHandle(ps.hThread);
	::TerminateProcess(ps.hProcess, 0);
	CloseHandle(ps.hProcess);
	return 0;
}

// 处理异常分发函数
DWORD CDebug::OnExceptionHandler(LPDEBUG_EVENT pDbg_event)
{
	// 获取异常地址
	LPVOID ExceptionAddress = pDbg_event->u.Exception.ExceptionRecord.ExceptionAddress;
	// 异常触发的错误码
	DWORD ExceptionCode = pDbg_event->u.Exception.ExceptionRecord.ExceptionCode;
	// 获取进程句柄
	//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pDbg_event->dwProcessId);
	// 线程句柄
	//HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, pDbg_event->dwThreadId);
	// 处理状态码
	DWORD status_code = DBG_EXCEPTION_NOT_HANDLED;
	printf("\t错误码：0x%lX,异常地址=%p", ExceptionCode, ExceptionAddress);

	// 区分异常类型
	switch (ExceptionCode)
	{
		// 常见3中异常
	case EXCEPTION_ACCESS_VIOLATION:      // 非法访问异常
		break;
	case EXCEPTION_BREAKPOINT:            // 断点异常
		if (ExceptionAddress == gDATA.OEP)
		{
			printf("\n\t检测到OEP断点，进行修复");
			SetBreakPoint(ExceptionAddress, defBP_断点暂停,false);
			//printf("已在OEP处断下，请处理：");
		}
		// 处理软件断点
		//status_code = OnException_BreakPoint(ExceptionAddress, hProcess, hThread);
		break;
	case EXCEPTION_SINGLE_STEP:           // 单步或硬件断点异常
		//status_code = OnException_SingleStep(ExceptionAddress, hProcess, hThread);
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT: // 内存对齐异常
	case EXCEPTION_ILLEGAL_INSTRUCTION:   // 无效指令
	case EXCEPTION_INT_DIVIDE_BY_ZERO:    // 除0错误
	case EXCEPTION_PRIV_INSTRUCTION:      // 指令不支持当前模式
		break;
	}
	this->DisASM(ExceptionAddress, 5);
	return DBG_CONTINUE;
}

DWORD CDebug::DisASM(LPVOID Address,DWORD ReadLen)
{
	csh handle;				// 反汇编引擎句柄
	cs_insn* pInsn;			// 保存反汇编得到的指令的缓冲区首地址
	DWORD dwSize = 0;		// 实际读取长度
	unsigned int count = 0; // 保存得到的反汇编的指令条数
	unsigned char szOpcode[200] = {0}; // 目标进程中的OPCODE
	PROCESS_INFORMATION& ps = gDATA.PS;
	CStringA strA;
	//初始化反汇编器句柄,(x86_64架构,32位模式,句柄)
	cs_open(CS_ARCH_X86,  /*x86指令集*/
		CS_MODE_32, /*使用32位模式解析opcode*/
		&handle /*输出的反汇编句柄*/
	);


	ReadProcessMemory(ps.hProcess, Address, szOpcode, 200, &dwSize);
	printf("\t读取指令长度=%lu\n", dwSize);
	count = cs_disasm(handle,		/*反汇编器句柄,从cs_open函数得到*/
		szOpcode,					/*需要反汇编的opcode的缓冲区首地址*/
		sizeof(szOpcode),			/*opcode的字节数*/
		(uint64_t)Address,			/*opcode的所在的内存地址*/
		ReadLen,				/*需要反汇编的指令条数,如果是0,则反汇编出全部*/
		&pInsn/*反汇编输出*/
	);
	size_t j;
	for (j = 0; j < count; j++) {
		strA = "";
		for (byte k = 0,by; k < pInsn[j].size; k++)
		{
			by = pInsn[j].bytes[k];
			strA.AppendFormat("%02X ", by);
		}
		printf("0x%p |%-16s|%s %s\n",
			(LPVOID)pInsn[j].address, /*指令地址*/
			strA.GetBuffer(),
			pInsn[j].mnemonic,/*指令操作码*/
			pInsn[j].op_str/*指令操作数*/
		);
	}
	// 释放保存指令的空间
	cs_free(pInsn, count);
	// 关闭句柄
	cs_close(&handle);
	return 0;
}

BOOL CDebug::AddSoftPoint(LPVOID Address)
{
	BYTE buff[16] = { 0 };
	BOOL bRet = false;
	BreakPoint tmp = this->mBreakPoint[Address];
	if (tmp.TYPES)
	{
		printf("断点已存在：%p", Address);
		return 0;
	}

	else
	{
		tmp = BreakPoint{
			defBP_软件执行,1,0,
			(DWORD_PTR)Address
		};
		buff[0] = 0xCC;

		if (NULL == ReadProcessMemory(gDATA.PS.hProcess,Address , &tmp.OLD, 1, 0))
			return 0;
		bRet = WriteProcessMemory(gDATA.PS.hProcess, Address, buff, 1, 0);
		this->mBreakPoint[Address] = tmp;
	}
	return bRet;
}

DWORD CDebug::SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak)
{
	BYTE buff[16] = { 0 };
	BOOL bRet = false;
	BreakPoint &tmp = this->mBreakPoint[Address];
	if (tmp.TYPES == 0)
	{
		printf("断点不存在，设置个鬼？？？%p\n", Address);
		return 0;
	}
	if (tmp.TYPES == Type)
	{
		printf("断点状态相同，设置个鬼？？？%p\n", Address);
		return 0;
	}

	switch (Type)
	{
	case 0:			//不存在
		puts("状态为空，设置个鬼？？？");
		return 0;
	case 1: {		//暂停
		//软件断点
		if (tmp.TYPES == defBP_软件执行) {
			bRet = WriteProcessMemory(gDATA.PS.hProcess, Address, &tmp.OLD, 1, 0);
			if (!bRet) {	//还原字节失败
				printf("写入目标进程%p->%lX 失败\n", Address, tmp.OLD);
				return 0;
			}
			//重新上软件断
			if (isBreak)
			{
				buff[0] = 0xCC;
				bRet = WriteProcessMemory(gDATA.PS.hProcess, Address, buff, 1, 0);
				if (!bRet)
				{
					printf("写入目标进程%p->INT3 失败\n", Address);
					return 0;
				}
			}

			// eip -1 将修复陷阱异常
			CONTEXT context = { CONTEXT_FULL };
			GetThreadContext(gDATA.PS.hThread, &context);
			context.Eip -= 1;
			SetThreadContext(gDATA.PS.hThread, &context);
			tmp.TYPES = defBP_断点暂停;
		}
	}break;
	default:
		break;
	}
	return bRet;
}
