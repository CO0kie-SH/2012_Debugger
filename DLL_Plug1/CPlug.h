#pragma once

EXTERN_C void InitPlugin(void* pStr);
EXTERN_C void MenuSetting(int hWnd);

class CPlug
{
public:
	void InitCPlug(HMODULE hModule);
	void ExitCPlug(HMODULE hModule);
};

extern CPlug gcPlug;