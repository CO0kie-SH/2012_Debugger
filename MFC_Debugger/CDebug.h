#pragma once
#include "CMyPoint.h"


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
	//增加软件断点
	//BOOL AddMemPoint(LPVOID Address, WORD Type, PWCHAR Text = 0);


	// 软件断点处理函数
	DWORD SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak);
	// 硬件断点处理函数
	DWORD SetHardPoint(LPBreakPoint Point, WORD Type, BOOL isBreak);

	BOOL SetTFPoint(BOOL isSetFlag = TRUE);
};

