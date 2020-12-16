#pragma once
#define FilePath1 L"..\\Debug\\Ox02_DebugTest.exe"
#define FilePath L"..\\dbgtarget.exe"
#define PlugPath1 L"Debug"
#define PlugPath L"Plugs"


typedef struct _MAINWIND
{
	DWORD	wPID;
	DWORD	wTID;
	HWND	hwMFC;
	HWND	hwCON;
	HWND	hwOld;
	HWND	hwMEM;
}MAINWIND, * LPMAINWIND;

enum EM_DebugInfo
{
	DebugInfo_错误,
	DebugInfo_创建进程,
	DebugInfo_附加进程
};

typedef struct _DebugInfo
{
	PROCESS_INFORMATION PS;
	BOOL isCreate;
}DebugInfo, * LPDebugInfo;



class CDATA
{
public:
	LPVOID OEP;
	LPVOID CDEBUG;
	PROCESS_INFORMATION PS;
	BOOL isCreate;
public:
	CDATA();
	~CDATA();


	void SetPS(PROCESS_INFORMATION* ps = 0)
	{
		if (ps == 0)
			ZeroMemory(&this->PS, sizeof(PROCESS_INFORMATION));
		else
			memcpy(&this->PS, ps, sizeof(PROCESS_INFORMATION));
	}
private:
};


extern MAINWIND gINFO_mWind;
extern CDATA gDATA;
