#pragma once
#include <map>
using std::map;


#define defBP_断点暂停 1
#define defBP_软件执行 2
#define defBP_单步步过 3
#define defBP_断点删除 4
#define defBP_硬件执行 4

constexpr LPVOID gszCC = "\xCC";


typedef struct _BreakPoint
{
	WORD TYPES;	//类型
	WORD STATU;	//状态
	DWORD OLD;	//旧的状态
	DWORD_PTR Address;	//地址
}BreakPoint, * LPBreakPoint;


class CDebug
{
public:
	BOOL IsSystemBreakPoint;
	BOOL mb_Wait;
	BOOL mb_Flagt;
	BOOL mb_Flagp;
	PCHAR msz_Line;
private:
	map<LPVOID, BreakPoint> mBreakPoint;
public:
	CDebug() :IsSystemBreakPoint(1),
		mb_Wait(1), mb_Flagt(0), mb_Flagp(0), msz_Line(0)
	{
		OutputDebugString(L"CDebug()\n");
	}
	~CDebug()
	{
		OutputDebugString(L"~CDebug()\n");
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
	//处理键盘输入
	DWORD OnLine();

	//反汇编
	DWORD DisASM(LPVOID Address, DWORD ReadLen);
	//增加软件断点
	BOOL AddSoftPoint(LPVOID Address, WORD Type);
	//增加硬件断点
	BOOL AddHardPoint(LPVOID Address, WORD Type);
	// 软件断点处理函数
	DWORD SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak);

	SIZE_T ReadMemory(LPVOID Address, LPVOID ReadBuff, DWORD_PTR ReadLen);
	SIZE_T WriteMemory(LPVOID Address, LPVOID Buff, DWORD_PTR ReadLen);
};

