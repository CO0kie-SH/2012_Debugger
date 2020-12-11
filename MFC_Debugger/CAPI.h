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
};

extern CAPI gAPI;
extern CString jstr;