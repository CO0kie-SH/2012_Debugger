#pragma once

extern HINSTANCE ghInstance;
class CData
{
public:
	HWND hDLG = 0;
	void InitDLG(LPVOID DLG);
};

extern CData gData;