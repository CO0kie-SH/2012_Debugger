#include "pch.h"
#include "CProInfo2.h"
#include <winternl.h>
#include <cstdio>
#pragma comment(lib,"ntdll.lib")



CProInfo2 cProinfo2;

BOOL CProInfo2::IsBeginDebuged()
{
	bool bRet = TRUE;
	_asm
	{
		mov eax, fs: [0x30] ;//找到PEB
		mov al, byte ptr ds : [eax + 0x2] ; // 取出一个字节
		mov bRet, al;
	}
	return bRet;
}

NTSTATUS
NTAPI
MyNtQueryInformationProcess(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
) {

	if (ProcessInformationClass == 0x07)
		return 0;
	if (ProcessInformationClass == 0x1E)
		return 0;
	if (ProcessInformationClass == 0x1F)
		return 0;
	if (ProcessInformationClass == 0x00)
		return 0;
	//printf("【NTQ】查询->%02lX\n", (DWORD)ProcessInformationClass);
	//cProinfo2.HOOK_Of();
	NTSTATUS result = NtQueryInformationProcess(
		ProcessHandle,
		ProcessInformationClass,
		ProcessInformation,
		ProcessInformationLength,
		ReturnLength
	);
	//cProinfo2.HOOK_On();
	return result;
}

BOOL CProInfo2::HOOK_NT()
{
	char* _ModAdd = (char*)GetModuleHandleW(0), * NTQ = 0, * ntdll = 0;
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)_ModAdd;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + _ModAdd);
	//1 获取到导入表的数据目录结构
	PIMAGE_DATA_DIRECTORY dwImportDir = &pNt->OptionalHeader.DataDirectory[1];
	//1 获取到导入表结构
	PIMAGE_IMPORT_DESCRIPTOR pImportTable =
		(PIMAGE_IMPORT_DESCRIPTOR)(_ModAdd + dwImportDir->VirtualAddress);
	//2 解析导入表
	printf_s("IAT表地址 %p\n", pImportTable);
	PIMAGE_THUNK_DATA pIat = nullptr;
	while (pImportTable->Name != 0 && NTQ == 0)
	{
		char* pDllName = (pImportTable->Name + _ModAdd);
		if (strcmp(pDllName, "ntdll.dll") != 0)
		{
			pImportTable++;
			continue;
		}
		HMODULE hModule = GetModuleHandleA(pDllName);
		printf_s("Name0x%p;Add=0x%p\t%s\n",
			pDllName, hModule, pDllName);

		//循环IAT地址表
		pIat = (PIMAGE_THUNK_DATA32)
			(pImportTable->FirstThunk + _ModAdd);

		//循环名字表
		PIMAGE_THUNK_DATA32 pNameTable = (PIMAGE_THUNK_DATA32)
			(pImportTable->OriginalFirstThunk + _ModAdd);
		while (pNameTable->u1.Ordinal != 0)
		{
			//2.3.1 判断最高位是不是1
			if (IMAGE_SNAP_BY_ORDINAL32(pNameTable->u1.Ordinal) == 1)
			{
			}
			else
			{
				//既有名字，又有序号
				PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)
					(pNameTable->u1.AddressOfData + _ModAdd);
				if (strcmp(pName->Name, "NtQueryInformationProcess") == 0)
				{
					DWORD dwOldProtect, * add = (DWORD*)0x001EE180,
						my = (DWORD)MyNtQueryInformationProcess;
					printf("准备HOOK,%p->%lX,%lX\n", add, *add, my);
					VirtualProtect(add, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
					*add = my;
					return 1;
					NTQ = (char*)pIat;
					printf("IAT=0x%p指向%08lX\t%05ld\t符号：%s\n",
						NTQ, pIat->u1.Function, pName->Hint, pName->Name);
					break;
				}
			}
			pNameTable++;
		}
		pImportTable++;
	}
	if (NTQ)		//成功找到IAT
	{
		for (BYTE i = 0, j; i < 5; i++)
		{
			j = NTQ[i];
			printf("%02X ", j);
		}
		LPVOID Add = GetModuleHandleA("ntdll");
		if (Add)
			Add = GetProcAddress(
				(HMODULE)Add,
				"NtQueryInformationProcess"
			);
		printf("函数地址=%p", Add);
		return HOOK_NTQ((LPDWORD)NTQ);
	}
	else
	{
		//使用普通内存HOOK
		//return HOOK_NT2();
	}
	return 0;
}



BOOL CProInfo2::HOOK_NT2()
{
	LPVOID Add = GetModuleHandleA("ntdll"),
		My = (LPVOID)MyNtQueryInformationProcess;
	if (Add)
		Add = GetProcAddress(
			(HMODULE)Add,
			"NtQueryInformationProcess"
		);
	Add = (LPVOID)((DWORD)Add + 5);
	if (!Add)	return 0;
	_Add = Add;
	printf("得到地址%p\n", Add);
	memcpy(this->_bOldCode, Add, 5);
	memcpy(this->_bNewCode + 1, My, 4);
	printf("\n原指令：");
	for (BYTE i = 0; i < 5; i++)
	{
		printf("%02X ", _bOldCode[i]);
	}
	printf("\t目标指令：");
	for (BYTE i = 0; i < 5; i++)
	{
		printf("%02X ", _bNewCode[i]);
	}
	printf("NTQ=%p\n", Add);
	return this->HOOK_On();
}



BOOL CProInfo2::HOOK_NTQ(LPDWORD Add)
{
	DWORD newAdd = (DWORD)MyNtQueryInformationProcess, dwOldProtect;
	printf("原地址：%08lX\t", *Add);
	VirtualProtect(Add, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memcpy(Add, &newAdd, 4);		//写入MyBoxW
	VirtualProtect(Add, 4, dwOldProtect, &dwOldProtect);
	printf("现地址：%08lX\n", *Add);
	return *Add == newAdd;
}

BOOL CProInfo2::HOOK_On()
{
	if (!this->_Add)	return 0;
	//1 修改Hook点的属性
	DWORD dwOldProtect = 0;
	VirtualProtect(_Add, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	//2 修改指令
	memcpy(_Add, _bNewCode, 5);
	//3 属性还原
	VirtualProtect(_Add, 5, dwOldProtect, &dwOldProtect);
	this->_bisHook = 1;
	return this->_bisHook;
}

BOOL CProInfo2::HOOK_Of()
{
	if (!this->_Add)	return 0;
	//1 修改Hook点的属性
	DWORD dwOldProtect = 0;
	VirtualProtect(_Add, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	//2 修改指令
	memcpy(_Add, _bOldCode, 5);
	//3 属性还原
	VirtualProtect(_Add, 5, dwOldProtect, &dwOldProtect);
	this->_bisHook = 1;
	return this->_bisHook;
}
