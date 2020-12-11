#pragma once
#define FilePath L"..\\Debug\\Ox02_DebugTest.exe"

typedef struct _MAINWIND
{
	DWORD	wPID;
	DWORD	wTID;
	HWND	hwMFC;
	HWND	hwCON;
}MAINWIND, * LPMAINWIND;



class CDATA
{
public:
	char* OEP;
	PROCESS_INFORMATION ps;
public:
	CDATA();
	~CDATA();
private:
};


extern MAINWIND gINFO_mWind;
extern CDATA gDATA;
