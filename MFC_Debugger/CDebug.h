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
		puts("�������߳̽�����");
	}
	BOOL InitDebug(PWCHAR Path);

	// �����쳣�ַ�����
	DWORD OnExceptionHandler(LPDEBUG_EVENT pDbg_event);
	//����ϵͳ�ϵ�
	DWORD OnException_BreakPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event);
	//��������Ӳ������
	DWORD OnException_SingleStep(LPVOID Address, LPDEBUG_EVENT pDbg_event);
	//�����ڴ����
	DWORD OnException_MemPoint(LPVOID Address, LPDEBUG_EVENT pDbg_event);
	//�����������
	DWORD OnLine();
	
	//�����
	DWORD DisASM(LPVOID Address, DWORD ReadLen);
	BOOL GetSymName(LPVOID Address, CStringA& Str);

	// ����ϵ㴦����
	DWORD SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak);
	// Ӳ���ϵ㴦����
	DWORD SetHardPoint(LPBreakPoint Point, WORD Type, BOOL isBreak);

	void ShowRegister();
	void ShowDlls(BYTE* Address = 0, int id = 0);
	void ShowMem(LPVOID Address);
};

