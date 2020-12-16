#include "pch.h"
#include <cstdio>
#include "CHOOK.h"
#include "CProInfo2.h"

HMODULE ghModule = 0;
CHOOK* gHOOK = 0;

void* DLL_GetGC()
{
	return gHOOK;
}

int OnHook(HANDLE hProcess)
{
	LPVOID  pAddress = VirtualAllocEx(
		hProcess, 0, 4096,
		MEM_COMMIT, PAGE_READWRITE);

	if (!hProcess || !pAddress)
		return false;
	char buff[MAX_PATH];

	DWORD dwRealSize = 0, szLen =
		GetModuleFileNameA(ghModule, buff, MAX_PATH);	//得到模块路径
	//写入内存
	if (WriteProcessMemory(
		hProcess, pAddress,
		buff, szLen,
		&dwRealSize
	) == 0 || dwRealSize != szLen)
		return false;

	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)LoadLibraryA,
		(LPVOID)pAddress,
		0, 0);
	if (NULL == hThread)	return false;
	DWORD TID = GetThreadId(hThread), PID =
		GetProcessIdOfThread(hThread);
	printf_s("LodeTid=%ld\tPID=%ld\n", TID, PID);
	WaitForSingleObject(hThread, INFINITE);
	VirtualFreeEx(hProcess, pAddress, 0, MEM_RELEASE);
	return 0;
}

CHOOK::CHOOK(HMODULE hModule)
{
	mhModule = hModule;
	ghModule = hModule;
	gHOOK = this;
	printf("加载成功：%p。\n", hModule);
	GetInfo();
}

CHOOK::~CHOOK()
{
	printf("DLL~CHOOK()卸载了\n");
}

void CHOOK::GetInfo()
{
	printf("PID=%lu,TID=%lu\n", GetCurrentProcessId(), GetCurrentThreadId());
	bool bRet = cProinfo2.IsBeginDebuged();
	if (!bRet) return;
	LPBYTE pPEB;
	_asm
	{
		mov eax, fs: [0x30] ;	//找到PEB
		mov pPEB, eax;
	}
	pPEB[2] = 0x0;
	printf("被调试=%d；修改后%d\nAPI->HOOK：%s\n",
		bRet, cProinfo2.IsBeginDebuged(),
		cProinfo2.HOOK_NT() ? "成功" : "失败"
	);
}



