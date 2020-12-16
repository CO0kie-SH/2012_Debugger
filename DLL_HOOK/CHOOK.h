#pragma once

extern __declspec(dllexport) void* DLL_GetGC();
extern __declspec(dllexport) int OnHook(HANDLE hProcess);


class CHOOK
{
public:
	CHOOK(HMODULE hModule);
	~CHOOK();
	void GetInfo();
private:
	HMODULE mhModule;
};

extern CHOOK* gHOOK;