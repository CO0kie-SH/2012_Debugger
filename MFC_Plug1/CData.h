#pragma once

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