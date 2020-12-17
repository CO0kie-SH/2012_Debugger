#pragma once
#include <psapi.h>
#include <tlhelp32.h>
#include <sstream>
#include "CMyPoint.h"
#include <vector>
using std::vector;



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


typedef struct _CodeLine		//源码结构体
{
	DWORD Address;				//地址
	std::string str;			//源码对应行
}CodeLine,*LPCodeLine;


class CDebug :public CMyPoint
{
private:
	vector<CodeLine> m_codeline;	//源码数组
	CStringA mPath;					//主文件路径
public:
	CDebug();
	~CDebug();
	//初始化调试器
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
	//获得符号名
	BOOL GetSymName(LPVOID Address, char* Str);
	//加载源码
	BOOL LoadCode(LPCCH szPath);
	//显示源码
	BOOL ShowCode(DWORD Address = 0);

	// 软件断点处理函数
	DWORD SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak);
	// 硬件断点处理函数
	DWORD SetHardPoint(LPBreakPoint Point, WORD Type, BOOL isBreak);

	//显示寄存器
	void ShowRegister();
	//显示模块
	void ShowDlls(BYTE* Address = 0, int id = 0);
	//显示内存
	void ShowMem(LPVOID Address);
};

