#pragma once
#define FilePath1 L"..\\Debug\\Ox02_DebugTest.exe"
#define FilePath L"..\\dbgtarget.exe"
#define FilePath3 L"D:\\Downloads\\Mon\\202012\\3.exe"
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
	DebugInfo_����,
	DebugInfo_��������,
	DebugInfo_���ӽ���
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


	BOOL SetPS(PROCESS_INFORMATION* ps = 0);
private:
};


extern MAINWIND gINFO_mWind;
extern CDATA gDATA;
