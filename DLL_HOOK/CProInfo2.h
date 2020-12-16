#pragma once


class CProInfo2
{
private:
	BYTE _bNewCode[5] = { 0xE9 },
		_bOldCode[5] = { 0 },
		_bisHook = FALSE;
	LPVOID _Add = 0;
public:
	BOOL IsBeginDebuged();
	BOOL HOOK_NT();
	BOOL HOOK_NT2();
	BOOL HOOK_NTQ(LPDWORD Add);
	BOOL HOOK_On();
	BOOL HOOK_Of();
};

extern CProInfo2 cProinfo2;