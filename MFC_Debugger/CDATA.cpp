#include "pch.h"
#include "CDATA.h"



MAINWIND gINFO_mWind;
CDATA gDATA;



CDATA::CDATA() :OEP(0), CDEBUG(0), isCreate(0)
{
	OutputDebugString(L"CDATA()\n");
	gINFO_mWind = {
		GetCurrentProcessId(),
		GetCurrentThreadId()
	};
	ZeroMemory(&this->PS, sizeof(PROCESS_INFORMATION));
}

CDATA::~CDATA()
{
	OutputDebugString(L"~CDATA()\n");
}

BOOL CDATA::SetPS(PROCESS_INFORMATION* ps)
{
	if (ps == 0)
		ZeroMemory(&this->PS, sizeof(PROCESS_INFORMATION));
	else if (this->isCreate == 1)
		memcpy(&this->PS, ps, sizeof(PROCESS_INFORMATION));
	else if (this->isCreate == 2)
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ps->dwProcessId);
		if (!hProcess)	return 0;
		DWORD TID;		//线程句柄
		if (!gAPI.GetTIDByPID(ps->dwProcessId, &TID))
			return 0;
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, TID);
		if (!hThread)	return 0;
		this->PS = PROCESS_INFORMATION{ hProcess,hThread,ps->dwProcessId,TID };
		memcpy(ps, &this->PS, sizeof(PROCESS_INFORMATION));
	}
	else
	{
		MessageBox(0, L"不存在此情况。错误代码0x7", 0, 0);
		return 0;
	}
	return TRUE;
}
