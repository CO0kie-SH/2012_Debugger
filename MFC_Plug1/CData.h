#pragma once

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



extern HINSTANCE ghInstance;
class CData
{
public:
	HWND hDLG;
	DebugInfo Info;
	CStringA  Path;
public:
	CData():hDLG(0)
	{
		ZeroMemory(&Info, sizeof(DebugInfo));
	}
	void InitDLG(LPVOID DLG);
};

extern CData gData;