#pragma once
#include <iostream>
#include <Windows.h>
#include <winternl.h>
#pragma comment(lib,"ntdll.lib")
using std::cout;
using std::endl;

//来源https://xz.aliyun.com/t/5339
//1.ProcessDebugPort 0x07，已在上面讨论过。
//2.ProcessDebugObjectHandle 0x1E
//3.ProcessDebugFlags 0x1F
//4.ProcessBasicInformation 0x00
//ProcessDebugObjectHandle

class CProInfo1
{
public:
	CProInfo1();
	BOOL IsBeginDebuged();
	BOOL NtGlobalFlag();
	BOOL NQIP_DebugPort();
	BOOL NQIP_ProcessDebugObjectHandle();
	BOOL NQIP_ProcessDebugFlag();
	DWORD NQIP_ParentPID();
	BOOL NQSI_SystemKernelDebuggerInformation();


public:
	DWORD PPID;
};

