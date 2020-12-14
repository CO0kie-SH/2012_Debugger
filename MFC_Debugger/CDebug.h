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
	//��������ϵ�
	//BOOL AddMemPoint(LPVOID Address, WORD Type, PWCHAR Text = 0);


	// ����ϵ㴦����
	DWORD SetBreakPoint(LPVOID Address, WORD Type, BOOL isBreak);
	// Ӳ���ϵ㴦����
	DWORD SetHardPoint(LPBreakPoint Point, WORD Type, BOOL isBreak);

	BOOL SetTFPoint(BOOL isSetFlag = TRUE);
};

