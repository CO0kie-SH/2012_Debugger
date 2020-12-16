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
		mov eax, fs: [0x30] ;//�ҵ�PEB
		mov al, byte ptr ds : [eax + 0x2] ; // ȡ��һ���ֽ�
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
	//printf("��NTQ����ѯ->%02lX\n", (DWORD)ProcessInformationClass);
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
	//1 ��ȡ������������Ŀ¼�ṹ
	PIMAGE_DATA_DIRECTORY dwImportDir = &pNt->OptionalHeader.DataDirectory[1];
	//1 ��ȡ�������ṹ
	PIMAGE_IMPORT_DESCRIPTOR pImportTable =
		(PIMAGE_IMPORT_DESCRIPTOR)(_ModAdd + dwImportDir->VirtualAddress);
	//2 ���������
	printf_s("IAT���ַ %p\n", pImportTable);
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

		//ѭ��IAT��ַ��
		pIat = (PIMAGE_THUNK_DATA32)
			(pImportTable->FirstThunk + _ModAdd);

		//ѭ�����ֱ�
		PIMAGE_THUNK_DATA32 pNameTable = (PIMAGE_THUNK_DATA32)
			(pImportTable->OriginalFirstThunk + _ModAdd);
		while (pNameTable->u1.Ordinal != 0)
		{
			//2.3.1 �ж����λ�ǲ���1
			if (IMAGE_SNAP_BY_ORDINAL32(pNameTable->u1.Ordinal) == 1)
			{
			}
			else
			{
				//�������֣��������
				PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)
					(pNameTable->u1.AddressOfData + _ModAdd);
				if (strcmp(pName->Name, "NtQueryInformationProcess") == 0)
				{
					DWORD dwOldProtect, * add = (DWORD*)0x001EE180,
						my = (DWORD)MyNtQueryInformationProcess;
					printf("׼��HOOK,%p->%lX,%lX\n", add, *add, my);
					VirtualProtect(add, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
					*add = my;
					return 1;
					NTQ = (char*)pIat;
					printf("IAT=0x%pָ��%08lX\t%05ld\t���ţ�%s\n",
						NTQ, pIat->u1.Function, pName->Hint, pName->Name);
					break;
				}
			}
			pNameTable++;
		}
		pImportTable++;
	}
	if (NTQ)		//�ɹ��ҵ�IAT
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
		printf("������ַ=%p", Add);
		return HOOK_NTQ((LPDWORD)NTQ);
	}
	else
	{
		//ʹ����ͨ�ڴ�HOOK
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
	printf("�õ���ַ%p\n", Add);
	memcpy(this->_bOldCode, Add, 5);
	memcpy(this->_bNewCode + 1, My, 4);
	printf("\nԭָ�");
	for (BYTE i = 0; i < 5; i++)
	{
		printf("%02X ", _bOldCode[i]);
	}
	printf("\tĿ��ָ�");
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
	printf("ԭ��ַ��%08lX\t", *Add);
	VirtualProtect(Add, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memcpy(Add, &newAdd, 4);		//д��MyBoxW
	VirtualProtect(Add, 4, dwOldProtect, &dwOldProtect);
	printf("�ֵ�ַ��%08lX\n", *Add);
	return *Add == newAdd;
}

BOOL CProInfo2::HOOK_On()
{
	if (!this->_Add)	return 0;
	//1 �޸�Hook�������
	DWORD dwOldProtect = 0;
	VirtualProtect(_Add, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	//2 �޸�ָ��
	memcpy(_Add, _bNewCode, 5);
	//3 ���Ի�ԭ
	VirtualProtect(_Add, 5, dwOldProtect, &dwOldProtect);
	this->_bisHook = 1;
	return this->_bisHook;
}

BOOL CProInfo2::HOOK_Of()
{
	if (!this->_Add)	return 0;
	//1 �޸�Hook�������
	DWORD dwOldProtect = 0;
	VirtualProtect(_Add, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	//2 �޸�ָ��
	memcpy(_Add, _bOldCode, 5);
	//3 ���Ի�ԭ
	VirtualProtect(_Add, 5, dwOldProtect, &dwOldProtect);
	this->_bisHook = 1;
	return this->_bisHook;
}
