#pragma once
#include <psapi.h>
#include <tlhelp32.h>
#include "CMyPoint.h"



constexpr PCHAR gszRegDrs[] = {
	"\nContextFlags",
	"Dr0",
	"Dr1",
	"Dr2",
	"Dr3",
	"Dr6",
	"Dr7"
};

constexpr PCHAR gszRegGs[] = {
	"\nSegGs",
	"SegFs",
	"SegEs",
	"SegDs"
};

constexpr PCHAR gszRegEs[] = {
	"\nEdi",
	"Esi",
	"Ebx",
	"Edx",
	"Ecx",
	"Eax",
	"\nEbp",
	"Eip",
	"SegCs",              // MUST BE SANITIZED
	"EFlags",             // MUST BE SANITIZED
	"Esp",
	"SegSs"
};

class CDebug :public CMyPoint
{
public:
	CDebug()
	{
		OutputDebugString(L"CDebug()\n");
		this->mWait = { TRUE };	
	}
	~CDebug()
	{
		OutputDebugString(L"~CDebug()\n");
		WaitForSingleObject(gDATA.PS.hProcess, -1);
		gDATA.SetPS();
		puts("调试者线程结束。");
	}
	BOOL InitDebug(PWCHAR Path);

	// 处理异常分发函数
	DWORD OnExceptionHandler(LPDEBUG_EVENT pDbg_event);
	//处理系统断点
	DWORD OnException_BreakPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event);
	//处理单步或硬件错误
	DWORD OnException_SingleStep(LPVOID Address, LPDEBUG_EVENT pDbg_event);
	//处理内存错误
	DWORD OnException_MemPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event);
	//处理键盘输入
	DWORD OnLine();
	
	//反汇编
	DWORD DisASM(LPVOID Address, DWORD ReadLen);
	BOOL GetSymName(LPVOID Address, CStringA& Str);

	// 软件断点处理函数
	DWORD SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak);
	// 硬件断点处理函数
	DWORD SetHardPoint(LPBreakPoint Point, WORD Type, BOOL isBreak);

	void ShowRegister();
	void ShowDlls(BYTE* Address = 0, int id = 0);
	void ShowMem(LPVOID Address);
};

