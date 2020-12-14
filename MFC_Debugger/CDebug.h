#pragma once
#include <map>
using std::map;


#define defBP_断点暂停 1
#define defBP_软件执行 2
#define defBP_单步步过 3
#define defBP_断点删除 4
#define defBP_硬件执行 5
#define defBP_硬件写入 6
#define defBP_硬件读写 7
#define defBP_硬件暂停 8
#define defBP_内存执行 9
#define defBP_内存写入 10
#define defBP_内存读取 11
#define defBP_内存读写 12
#define defBP_内存暂停 13
#define defBP_内存属性 14
#define defBP_内存复写 15

constexpr LPVOID gszCC = "\xCC\xCC";
constexpr PWCHAR gszBP[] = {
	L"断点错误",
	L"断点暂停",
	L"软件断点",
	L"单步步过",
	L"断点删除",
	L"硬件执行",
	L"硬件写入",
	L"硬件读写",
	L"硬件暂停",
	L"内存执行",
	L"内存写入",
	L"内存读写",
	L"内存暂停",
	L"内存属性",
	L"内存复写"
};


typedef struct _BreakPoint
{
	WORD TYPES = 0;	//类型
	WORD STATU = 0;	//状态
	DWORD OLD = 0;	//旧的状态
	DWORD Cout = 0;	//命中次数
	LPVOID Address = 0;	//地址
	PWCHAR str = 0;
}BreakPoint, * LPBreakPoint;

typedef struct _WaitPoint	//断点结构体：用于判断是否是断点
{
	BYTE SYSPoint;			//系统载入断点
	BYTE OEPPoint;			//进程载入断点
	BYTE PassPoint;			//单步断点
	BYTE StepPoint;			//步过断点
	LPBreakPoint SoftPoint;	//软件断点
	LPBreakPoint HardPoint;	//硬件断点
	LPBreakPoint MemyPoint;	//内存断点
	PCHAR LineShow;			//断点标志字
}WaitPoint, * LPWaitPoint;




class CMyPoint
{
public:
	WaitPoint mWait;
	map<LPVOID, BreakPoint> mBreakPoint;
public:
	//增加软件断点
	BOOL AddSoftPoint(LPVOID Address, WORD Type, PWCHAR Text = 0);
	//设置内存断点
	BOOL SetMemPoint(LPBreakPoint pPoint);
	//重设软件断点
	BOOL ReSetSoftPoint(LPBreakPoint pPoint);
	//添加硬件断点
	BOOL AddHardPoint(LPVOID Address, WORD Type, PWCHAR Text = 0);

	SIZE_T ReadMemory(LPVOID Address, LPVOID ReadBuff, DWORD_PTR ReadLen);

	SIZE_T WriteMemory(LPVOID Address, LPVOID WriteBuff, DWORD_PTR WriteLen);
	int Bug(int ID)
	{
		printf("错误码：%X，", ID);
		this->mWait.LineShow = "请联系作者修复：";
		//this->OnLine();
		::TerminateProcess(gDATA.PS.hProcess, 0);
		WaitForSingleObject(gDATA.PS.hProcess, 1000);
		return 0;
	}
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
	//增加软件断点
	BOOL AddMemPoint(LPVOID Address, WORD Type, PWCHAR Text = 0);


	// 软件断点处理函数
	DWORD SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak);
	// 硬件断点处理函数
	DWORD SetHardPoint(LPBreakPoint Point, WORD Type, BOOL isBreak);

	BOOL SetTFPoint(BOOL isSetFlag = TRUE);
};

