#pragma once
#include <map>
using std::map;


#define defBP_断点暂停 1
#define defBP_软件执行 2

typedef struct _BreakPoint
{
	WORD TYPES;	//类型
	WORD STATU;	//状态
	DWORD OLD;	//旧的状态
	DWORD_PTR Address;	//地址
}BreakPoint, * LPBreakPoint;


class CDebug
{
private:
	map<LPVOID, BreakPoint> mBreakPoint;
public:
	~CDebug()
	{
		gDATA.SetPS();
		puts("调试者线程结束。");
	}
	BOOL InitDebug(PWCHAR Path);

	// 处理异常分发函数
	DWORD OnExceptionHandler(LPDEBUG_EVENT pDbg_event);
	//反汇编
	DWORD DisASM(LPVOID Address, DWORD ReadLen);
	//增加软件断点
	BOOL AddSoftPoint(LPVOID Address);
	// 软件断点处理函数
	DWORD SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak);
};

