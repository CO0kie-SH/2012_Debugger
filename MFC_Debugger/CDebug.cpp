#include "pch.h"
#include "CDebug.h"
#include ".\capstone\include\capstone.h"
#include "CPE.h"
#include "CManage.h"

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
	PROCESS_INFORMATION ps = { 0 };
	// 以附加方式启动
	BOOL bRet = 0;
	// BOOL bRet = DebugActiveProcess(3008);
	if (gDATA.isCreate == FALSE)
		return 0;
	if (gDATA.isCreate == TRUE)
		//以调试方式启动进程
		bRet = CreateProcess(
			Path,											//	调试目标路径
			NULL,												//  命令行参数
			NULL,												//  进程安全属性	
			NULL,												//  线程安全属性	
			FALSE,												//  是否继承句柄表
			DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE,		//  以只调试方式启动进程
			NULL,												//  环境变量
			NULL,												//  工作目录
			&si, &ps);
	else if (gDATA.isCreate == 2)
	{
		printf("请输入PID：");
		char cmdline[20] = {};
		gets_s(cmdline, 20);
		sscanf_s(cmdline, "%lu", &ps.dwProcessId);
		bRet = DebugActiveProcess(ps.dwProcessId);
	}
	if (bRet == FALSE)
	{
		MessageBox(0, L"进程创建失败", 0, 0);
		return 0;
	}
	if (!gDATA.SetPS(&ps))
	{
		MessageBox(0, L"进程信息获取失败", 0, 0);
		return 0;
	}
	gcManage.DebugCreate(&ps, gDATA.isCreate);
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
			//printf("增加OEP硬件断点=%d\n", this->AddHardPoint(gDATA.OEP, defBP_硬件执行));
			printf("增加OEP软件断点=%d\n", this->AddSoftPoint(gDATA.OEP, defBP_软件执行, L"OEP入口"));
			//printf("增加OEP内存断点=%d\n", this->AddMemPoint(gDATA.OEP, defBP_内存执行, L"OEP入口"));
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
	// 处理状态码
	DWORD status_code = DBG_EXCEPTION_NOT_HANDLED;
	printf("\t错误码：0x%lX,异常地址=%p", ExceptionCode, ExceptionAddress);

	// 区分异常类型
	switch (ExceptionCode)
	{
		// 常见3中异常
	case EXCEPTION_ACCESS_VIOLATION:      // 非法访问异常
		status_code = OnException_MemPoint(ExceptionAddress, pDbg_event);
		break;
	case EXCEPTION_BREAKPOINT:            // 断点异常
		status_code = OnException_BreakPoint(ExceptionAddress, pDbg_event);
		break;
	case EXCEPTION_SINGLE_STEP:           // 单步或硬件断点异常
		status_code = OnException_SingleStep(ExceptionAddress, pDbg_event);
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT: // 内存对齐异常
	case EXCEPTION_ILLEGAL_INSTRUCTION:   // 无效指令
	case EXCEPTION_INT_DIVIDE_BY_ZERO:    // 除0错误
	case EXCEPTION_PRIV_INSTRUCTION:      // 指令不支持当前模式
		break;
	default:
		this->mWait.LineShow = "未知错误断下：请处理：";
		this->DisASM(ExceptionAddress, 5);
		this->OnLine();
		break;
	}
	return status_code;
}

//处理断点异常
DWORD CDebug::OnException_BreakPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event)
{
	BOOL isASM = true, isWait = true;
	if (this->mWait.SYSPoint)
	{
		printf("\n\t检测到系统载入断点,");
		this->mWait.SYSPoint = FALSE;
		this->mWait.LineShow = "已在载入点断下，请继续：";
	}
	else if (Address == gDATA.OEP)
	{
		printf("\n\t检测到OEP断点，\t");
		SetBreakPoint(Address, defBP_断点暂停, FALSE);
		this->mWait.LineShow = "已在OEP处断下，请继续：";
	}
	else if (this->mWait.StepPoint)
	{
		printf("\n\t单步步过，进行修复");
		LPBreakPoint tmp = &this->mBreakPoint[Address];
		tmp->STATU = defBP_断点删除;
		this->ReSetSoftPoint(tmp);
		this->mWait.StepPoint = false;
		this->mWait.LineShow = "单步步入，请继续：";
	}
	else
	{
		LPBreakPoint tmp = this->GetBP(Address);
		if (tmp == 0)
			printf("出错啦，不存在该断点%p\n", Address);
		else if (tmp->TYPES != defBP_软件执行)
			printf("出错啦，无法判断该断点%p\n", Address);
		else
		{
			printf("\n\t检测到软件断点%p", Address);
			SetBreakPoint(Address, defBP_断点暂停, TRUE);
			if (this->mWait.IFPoint)
			{
				switch (this->mWait.IFPoint->TYPES)
				{
				case TYPE_计次相等:
					if (this->mWait.IFPoint->OLD == tmp->Cout)
					{
						printf("【Cout==%lu】", tmp->Cout);
						this->mWait.LineShow = "条件断点断下，请继续：";
					}
					else
						isWait = false;
					break;
				default:
					this->mWait.LineShow = "未知错误断下，请继续：";
					break;
				}
			}
			else
			{
				this->mWait.LineShow = "软件执行断下，请继续：";
			}
		}
	}

	if(isASM)
		this->DisASM(Address, 5);
	if (isWait)
		this->OnLine();
	return DBG_CONTINUE;
}

//处理单步、硬件、永久软件断点
DWORD CDebug::OnException_SingleStep(LPVOID Address, LPDEBUG_EVENT pDbg_event)
{
	bool isASM = true, isWait = true, isRet = false, isNULL = false;
	BreakPoint& tmp = this->mBreakPoint[Address];
	//判断是否为永久断点断下
	if (this->mWait.SoftPoint)			//如果有软件标志
	{
		printf("\n\t检测到软件断点复写，再次写入CC\n");
		if (NULL == this->WriteMemory(					//则还原指令
			this->mWait.SoftPoint->Address, gszCC, 1))
			return this->Bug(2);
		this->mWait.SoftPoint->TYPES = defBP_软件执行;
		this->mWait.SoftPoint = 0;
		isRet = true;
	}
	if (this->mWait.HardPoint)			//如果有硬件标志
	{
		printf("\n\t检测到硬件断点%p->%lu复写，再次启用\n",
			this->mWait.HardPoint->Address, this->mWait.HardPoint->OLD);
		SetHardPoint(this->mWait.HardPoint, this->mWait.HardPoint->TYPES, FALSE);
		this->mWait.HardPoint = 0;
		isRet = true;
	}
	if (this->mWait.MemyPoint)			//如果有内存标志
	{
		DWORD_PTR dwAdd = (DWORD_PTR)
			this->mWait.MemyPoint->Address;
		printf("\n\t遇到内存重写0x%lX\n", dwAdd);
		dwAdd /= 0x1000;
		dwAdd *= 0x1000;
		this->SetMemPoint((LPVOID)dwAdd);
		isRet = true;
	}

	//修复永久断点后，判断是否返回
	if (isRet)
		return DBG_CONTINUE;

	// 判断是否是普通单步
	if (this->mWait.PassPoint)
	{
		isNULL = this->mWait.StepPoint;
		printf("\n\t检测到单步%s,\t", isNULL ? "步过" : "步入");
		this->mWait.PassPoint = FALSE;
		this->mWait.StepPoint = isNULL ? TRUE : FALSE;
		this->mWait.LineShow = isNULL ? "单步步过，请继续：" : "已经单步断下，请继续：";
	}
	else if (tmp.TYPES == defBP_硬件执行)
	{
		printf("\n\t检测到硬件执行断点");
		this->SetHardPoint(&tmp, defBP_断点暂停, TRUE);
		this->mWait.LineShow = "硬件执行断下，请继续：";
	}
	if (isASM)
		this->DisASM(Address, 5);
	if (isWait)
		this->OnLine();
	return DBG_CONTINUE;
}

//处理内存异常
DWORD CDebug::OnException_MemPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event)
{
	bool isASM = true, isWait = true, isRet = false, isNULL = false;
	DWORD dwAdd = (DWORD)Address / 0x1000, dwtmp;
	dwAdd *= 0x1000;

	if (Address == (LPVOID)0x401072)
	{
		printf("\n\t检测到SEH处理，跳过\n");
		return DBG_EXCEPTION_NOT_HANDLED;
	}
	int errINFO = pDbg_event->u.Exception.
		ExceptionRecord.ExceptionInformation[0];
	printf("\n\t内存%s错误", errINFO == 0 ? "读取" :
		(errINFO == 1 ? "写入" : (errINFO == 8 ? "执行" : "未知")));
	switch (this->MemyPoint->TYPES)
	{
	case defBP_内存写入:
		if (errINFO == 1)
		{
			printf("【命中】");
			this->MemyPoint->Cout++;
			this->mWait.MemyPoint = this->MemyPoint;
			this->SetTFPoint(0);
		}break;
	case defBP_内存读取:
		if (errINFO == 0)
		{
			printf("【命中】");
			this->MemyPoint->Cout++;
		}
		else
		{
			printf("【未命中】");
			isASM = 0; isWait = 0;
		}
		this->mWait.MemyPoint = this->MemyPoint;
		this->SetTFPoint(0);
		break;
	case defBP_内存执行:
		if (errINFO == 8 &&
			this->MemyPoint->Address == Address)
		{
			printf("【命中】");
			this->MemyPoint->Cout++;
		}
		else
		{
			printf("【未命中】");
			isASM = 0; isWait = 0;
		}
		this->mWait.MemyPoint = this->MemyPoint;
		this->SetTFPoint(0);
		break;
	default:
		break;
	}
	VirtualProtectEx(
		gDATA.PS.hProcess,
		this->MemyPoint->Address, 1,
		this->MemyPoint->OLD, &dwtmp);

	if (isASM)
		this->DisASM(Address, 5);
	if (isWait)
		this->OnLine();
	return DBG_CONTINUE;
}

DWORD CDebug::OnLine()
{
	char cmdline[200] = {};
	char cmd[100] = {};
	DWORD address;
	while (true)
	{
		if (!this->mWait.LineShow)
			this->mWait.LineShow = "请继续：";
		std::cout << this->mWait.LineShow;
		// 获取命令
		gets_s(cmdline, 200); // bp 0x3333333

		// 解析命令
		if (sscanf_s(cmdline, "%s", cmd, 100))
		{
			// 判断是否是g命令
			if (strcmp(cmd, "e") == 0)
			{
				::TerminateProcess(gDATA.PS.hProcess, 0);
				break;
			}
			else if (strcmp(cmd, "g") == 0)
			{
				break; // 运行程序
			}
			// 判断是否是断点命令
			else if (strcmp(cmd, "bp") == 0)
			{
				sscanf_s(cmdline, "%s %x", cmd, 100, &address);
				while (true)
				{
					printf("确定要下软件断点至 0x%lX吗？y/n：", address);
					gets_s(cmdline, 200);
					if (strcmp(cmdline, "n") == 0)
						break;
					else if (strcmp(cmdline, "y") == 0)
					{
						// 设置软件断点
						wsprintfA(cmd, "设置断点0x%lX %s，请继续：", address,
							this->AddSoftPoint((LPVOID)address, defBP_软件执行) ?
							"成功" : "失败");
						this->mWait.LineShow = cmd;
						break;
					}
				}
				printf("\n");
			}
			// 设置单步断点
			else if (strcmp(cmd, "t") == 0)
			{
				this->SetTFPoint();
				break;
			}
			else if (strcmp(cmd, "p") == 0)
			{
				std::cout << "\t正在设置下一条断点";
				this->mWait.StepPoint = TRUE;
				CONTEXT context = { CONTEXT_FULL };
				GetThreadContext(gDATA.PS.hThread, &context);
				//向反汇编提供EIP
				this->DisASM((LPVOID)context.Eip, 5);
				break;
			}
			else if (strcmp(cmd, "u") == 0)
			{
				std::cout << "\t反汇编如下";
				CONTEXT context = { CONTEXT_FULL };
				GetThreadContext(gDATA.PS.hThread, &context);
				//向反汇编提供EIP
				this->DisASM((LPVOID)context.Eip, 5);
				continue;
			}
			else if (strcmp(cmd, "r") == 0)
			{
				std::cout << "\n\t寄存器信息如下\n";
				this->ShowRegister();
				this->mWait.LineShow = 0;
				continue;
			}
			else if (strcmp(cmd, "lm") == 0)
			{
				std::cout << "\n\t模块信息如下\n";
				this->ShowDlls();
				this->mWait.LineShow = 0;
				continue;
			}
			else if (strcmp(cmd, "im") == 0)
			{
				sscanf_s(cmdline, "%s %x", cmd, 100, &address);
				this->ShowDlls((BYTE*)address, 1);
			}
			else if (strcmp(cmd, "ex") == 0)
			{
				sscanf_s(cmdline, "%s %x", cmd, 100, &address);
				this->ShowDlls((BYTE*)address, 2);
			}
			else if (strcmp(cmd, "dump") == 0)
			{
				sscanf_s(cmdline, "%s %x", cmd, 100, &address);
				this->ShowDlls((BYTE*)address, 3);
			}
			else if (strcmp(cmd, "d") == 0)
			{
				sscanf_s(cmdline, "%s %x", cmd, 100, &address);
				if (address == 0xCCCCCCCC)
				{
					this->mWait.LineShow = 0;
					continue;
				}
				while (address)
				{
					printf("确定要查看内存0x%lX吗？r/w/n：", address);
					gets_s(cmdline, 200);
					if (strcmp(cmdline, "n") == 0)
					{
						this->mWait.LineShow = 0;
						break;
					}
					else if (strcmp(cmdline, "r") == 0)
					{
						this->ShowMem((LPVOID)address);
						this->mWait.LineShow = 0;
						continue;
					}
					else if (strcmp(cmdline, "w") == 0)
					{
						this->ShowMem((LPVOID)address);
						printf("请输入修改的值：");
						gets_s(cmdline, 200);
						DWORD dw = 0;
						sscanf_s(cmdline, "%x", &dw);
						this->mWait.LineShow = 0;
						this->WriteMemory((LPVOID)address, &dw, 4);
						this->ShowMem((LPVOID)address);
						continue;
					}
				}
			}
			else if (strcmp(cmd, "ba") == 0)
			{
				sscanf_s(cmdline, "%s %x", cmd, 100, &address);
				while (true)
				{
					printf("确定要下硬件断点至 0x%lX吗？r/w/e/n：", address);
					gets_s(cmdline, 200);
					if (strcmp(cmdline, "n") == 0)
						break;
					else if (strcmp(cmdline, "e") == 0)
					{
						// 设置硬件执行断点
						wsprintfA(cmd, "设置断点0x%lX %s，请继续：", address,
							this->AddHardPoint((LPVOID)address, defBP_硬件执行) ?
							"成功" : "失败");
						this->mWait.LineShow = cmd;
						break;
					}
					else if (strcmp(cmdline, "w") == 0)
					{
						// 设置硬件写入断点
						wsprintfA(cmd, "设置断点0x%lX %s，请继续：", address,
							this->AddHardPoint((LPVOID)address, defBP_硬件写入) ?
							"成功" : "失败");
						this->mWait.LineShow = cmd;
						break;
					}
					else if (strcmp(cmdline, "r") == 0)
					{
						// 设置硬件写入断点
						wsprintfA(cmd, "设置断点0x%lX %s，请继续：", address,
							this->AddHardPoint((LPVOID)address, defBP_硬件读写) ?
							"成功" : "失败");
						this->mWait.LineShow = cmd;
						break;
					}
				}
				printf("\n");
			}
			else if (strcmp(cmd, "bm") == 0)
			{
				sscanf_s(cmdline, "%s %x", cmd, 100, &address);
				while (true)
				{
					printf("确定要下软件断点至 0x%lX吗？r/w/e/n：", address);
					gets_s(cmdline, 200);
					if (strcmp(cmdline, "n") == 0)
						break;
					else if (strcmp(cmdline, "e") == 0)
					{
						// 设置软件执行断点
						wsprintfA(cmd, "设置断点0x%lX %s，请继续：", address,
							this->AddMemPoint((LPVOID)address, defBP_内存执行) ?
							"成功" : "失败");
						this->mWait.LineShow = cmd;
						break;
					}
					else if (strcmp(cmdline, "w") == 0)
					{
						// 设置软件执行断点
						wsprintfA(cmd, "设置断点0x%lX %s，请继续：", address,
							this->AddMemPoint((LPVOID)address, defBP_内存写入) ?
							"成功" : "失败");
						this->mWait.LineShow = cmd;
						break;
					}
					else if (strcmp(cmdline, "r") == 0)
					{
						// 设置软件执行断点
						wsprintfA(cmd, "设置断点0x%lX %s，请继续：", address,
							this->AddMemPoint((LPVOID)address, defBP_内存读取) ?
							"成功" : "失败");
						this->mWait.LineShow = cmd;
						break;
					}
				}
				printf("\n");
			}
		}
		else {
			printf("命令输入错误\n");
		}
	}
	this->mWait.LineShow = 0;
	return 0;
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
	count = cs_disasm(handle,		/*反汇编器句柄,从cs_open函数得到*/
		szOpcode,					/*需要反汇编的opcode的缓冲区首地址*/
		sizeof(szOpcode),			/*opcode的字节数*/
		(uint64_t)Address,			/*opcode的所在的内存地址*/
		ReadLen,				/*需要反汇编的指令条数,如果是0,则反汇编出全部*/
		&pInsn/*反汇编输出*/
	);
	if (this->mWait.StepPoint)			//设置单步补过
	{
		if (/*strcmp(pInsn[0].mnemonic, "jmp") == 0 ||*/
			strcmp(pInsn[0].mnemonic, "call") == 0)
		{
			std::cout << "(CC)";
			szOpcode[pInsn[0].size] = 0xCC;
			count = cs_disasm(handle,		/*反汇编器句柄,从cs_open函数得到*/
				szOpcode,					/*需要反汇编的opcode的缓冲区首地址*/
				sizeof(szOpcode),			/*opcode的字节数*/
				(uint64_t)Address,			/*opcode的所在的内存地址*/
				ReadLen,					/*需要反汇编的指令条数,如果是0,则反汇编出全部*/
				&pInsn						/*反汇编输出*/
			);
			DWORD_PTR add = pInsn[0].size + (DWORD_PTR)Address;
			this->AddSoftPoint((LPVOID)add, defBP_单步步过);
		}
		else {
			std::cout << "(TF)\n";
			this->SetTFPoint();
		}
	}
	printf("\t读取指令长度=%lu\n", dwSize);
	for (size_t j = 0; j < count; j++) {
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

DWORD CDebug::SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak)
{
	BreakPoint& tmp = this->mBreakPoint[Address];
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
	case defBP_断点暂停: {		//暂停
		if (tmp.TYPES == defBP_软件执行) {
			tmp.Cout++;
			//还原字节失败
			if (!this->WriteMemory(Address, &tmp.OLD, 1))
				return 0;

			//获取线程上下文
			CONTEXT context = { CONTEXT_FULL };
			GetThreadContext(gDATA.PS.hThread, &context);

			if (isBreak == FALSE)					//设置断点标志为暂停
				tmp.TYPES = defBP_断点暂停;
			//else if (isBreak == defBP_断点删除)	//单步步过时删除该临时断点
			//	this->mBreakPoint.erase(Address);
			else if (isBreak == TRUE) {			//永久性断点，出发单步后设置CC
				this->mWait.SoftPoint = &tmp;	//保存断点标志
				((PEFLAGS)&context.EFlags)->TF = 1;
			}
			context.Eip -= 1;
			SetThreadContext(gDATA.PS.hThread, &context);
		}
	}break;
	default: break;
	}
	return 0;
}

DWORD CDebug::SetHardPoint(LPBreakPoint Point, WORD Type, BOOL isBreak)
{
	BOOL bRet = false;
	CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
	GetThreadContext(gDATA.PS.hThread, &ct);
	PDBG_REG7 pDr7 = (PDBG_REG7)&ct.Dr7;

	if (Point->TYPES == Type)	//修复硬断
	{
		if (Point->OLD == 70)	//寄存器1
		{
			pDr7->L0 = 1;		//启动断点
		}
		else if (Point->OLD == 71)	//寄存器2
		{
			pDr7->L1 = 1;		//启动断点
		}
	}
	else
	{
		switch (Type)
		{
		case 0: this->Bug(3); return 0;
		case defBP_断点暂停:
			if (Point->OLD == 70)	//寄存器1
			{
				pDr7->L0 = 0;		//暂停断点
			}
			if (isBreak == TRUE)
			{
				this->mWait.HardPoint = Point;
				this->SetTFPoint(0);
			}
			break;
		default:
			break;
		}
	}
	return (DWORD)SetThreadContext(gDATA.PS.hThread, &ct);
}

void CDebug::ShowRegister()
{
	CONTEXT context = { CONTEXT_FULL };
	// 获取线程上下文
	GetThreadContext(gDATA.PS.hThread, &context);

	//按照 CONTEXT 格式输出 相应寄存器
	for (DWORD i = 0, *p = &context.ContextFlags; i < 7; i++)
	{
		printf("%s\t%3s0x%08lX\n", gszRegDrs[i], "：", p[i]);
	}
	for (DWORD i = 0, *p = &context.SegGs; i < 4; i++)
	{
		printf("%s\t%3s0x%04lX\n", gszRegGs[i], "：", p[i]);
	}
	for (DWORD i = 0, *p = &context.Edi; i < 12; i++)
	{
		printf("%s\t%3s0x%08lX\n", gszRegEs[i], "：", p[i]);
	}
	DWORD mem = context.Esp, end = context.Ebp, max = end - mem+4;
	BYTE* buff = new BYTE[max];
	this->ReadMemory((LPVOID)mem, buff, max);
	printf("\nESP=0x%08lX\tEBP=0x%08lX\n", mem, end);
	for (DWORD i = 0, *p = (PDWORD)buff; i < max; p++)
	{
		printf("地址：0x%lX\t", mem + i);
		for (BYTE j = 4; j--; )
		{
			printf("%02X ", buff[i++]);
		}
		printf("|四字节：0x%08lX\n", *p);
	}
	delete[] buff;
}

void CDebug::ShowDlls(BYTE* Address,int id)
{
	HANDLE hToolHelp = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, gDATA.PS.dwProcessId);
	if (hToolHelp == INVALID_HANDLE_VALUE)	return;
	//结构体[模块信息][tagMODULEENTRY32W]说明
/*
typedef struct tagMODULEENTRY32W
{
	DWORD   th32ProcessID;      // 所属进程ID
	BYTE  * modBaseAddr;        // 模块的加载基地址
	DWORD   modBaseSize;        // 模块的大小
	HMODULE hModule;            // 模块的句柄(加载基址)
	WCHAR   szModule[MAX_MODULE_NAME32 + 1];	// 模块名
	WCHAR   szExePath[MAX_PATH];				// 所在路径
} MODULEENTRY32W;
	*/
	MODULEENTRY32 info = { sizeof(MODULEENTRY32) };
	if (Module32First(hToolHelp, &info))
	{
		do {
			printf("模块0x%p\t大小0x%08X\t%15S\t%S\n",
				info.modBaseAddr, info.modBaseSize, info.szModule, info.szExePath);
			if (Address && Address == info.modBaseAddr)
			{
				CPE cPE(CMyPoint().GetCMyPoint());
				if (id == 1)
					cPE.ShowImports(Address, info.modBaseSize);
				else if (id == 2)
					cPE.ShowExports(Address, info.modBaseSize);
				else if (id == 3)
					cPE.DUMP(Address, info.modBaseSize,info.szModule);
				break;
			}
		} while (Module32Next(hToolHelp, &info));
	}
	CloseHandle(hToolHelp);
	return;
}

void CDebug::ShowMem(LPVOID Address)
{
	BYTE buff[4];
	this->ReadMemory(Address, buff, 4);
	for (DWORD i = 0, *p = (PDWORD)buff; i < 1; p++)
	{
		printf("地址：0x%p\t", Address);
		for (BYTE j = 4; j--; )
		{
			printf("%02X ", buff[i++]);
		}
		printf("|四字节：0x%08lX\n", *p);
	}
}
