#pragma once



class CAPI
{
public:
	CAPI()
	{
		OutputDebugString(L"CAPI()\n");
	}
	static BOOL FindWindowByTID(DWORD TID);
};

extern CAPI gAPI;