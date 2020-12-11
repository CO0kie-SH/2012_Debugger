#pragma once
#define FilePath L"..\\Debug\\Ox02_DebugTest.exe"

typedef struct _MAINWIND
{
	DWORD	wPID;
	DWORD	wTID;
	HWND	hwMFC;
	HWND	hwCON;
	HWND	hwOld;
	HWND	hwMEM;
}MAINWIND, * LPMAINWIND;



class CDATA
{
public:
	LPVOID OEP;
	LPVOID CDEBUG;
	PROCESS_INFORMATION PS;
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
