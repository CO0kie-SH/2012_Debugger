#pragma once


class CAPI
{
public:
	CAPI()
	{
		OutputDebugString(L"CAPI()\n");
	}
	~CAPI()
	{
		OutputDebugString(L"~CAPI()\n");
	}
	static BOOL FindWindowByTID(DWORD TID);
	static BOOL GetTIDByPID(DWORD PID, DWORD* TID = 0);
};

extern CAPI gAPI;
extern CString jstr;