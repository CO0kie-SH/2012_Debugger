#include "pch.h"
#include "CPE.h"
CPE::CPE(CMyPoint* pcPoint)
{
	mHeap = HeapCreate(0, 0, 0);
	this->mcPoint = pcPoint;
}

CPE::~CPE()
{
	HeapDestroy(mHeap);
}

void CPE::ShowImports(LPVOID Add, DWORD Size)
{

	if (!mHeap) return;
	//1 获取到导入表结构
	char* MOD = (char*)::HeapAlloc(mHeap, NULL, Size);
	if (!MOD || !mcPoint->ReadMemory(Add, MOD, Size))
	{
		printf("\n无法得到内存。\n");
		HeapDestroy(mHeap);
		return;
	}
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)MOD;
	//1 获取到导入表的数据目录结构
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + MOD);
	//1 获取到导入表的数据目录结构
	PIMAGE_DATA_DIRECTORY dwImportDir = &pNt->OptionalHeader.DataDirectory[1];
	//1 获取到导入表结构
	PIMAGE_IMPORT_DESCRIPTOR pImportTable =
		(PIMAGE_IMPORT_DESCRIPTOR)(MOD + dwImportDir->VirtualAddress);
	//2 解析导入表
	printf_s("IAT表地址 %p\n", pImportTable);

	//2 开始解析
	while (pImportTable->Name != 0)
	{
		printf("\n\t模块：%s\n", pImportTable->Name + MOD);	//导入的DLL名
		PIMAGE_THUNK_DATA32 pNameTable = (PIMAGE_THUNK_DATA32)
			(pImportTable->OriginalFirstThunk + MOD);
		while (pNameTable->u1.Ordinal != 0)
		{
			//2.3.1 判断最高位是不是1
			if (IMAGE_SNAP_BY_ORDINAL32(pNameTable->u1.Ordinal) == 1)
			{
				//只有序号
				printf("%05ld\t符号：-\n", pNameTable->u1.Ordinal & 0x7FFFFFFF);
			}
			else
			{
				//既有名字，又有序号
				PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)
					(pNameTable->u1.AddressOfData + MOD);
				printf("%05ld\t符号：%s\n", pName->Hint, pName->Name);
			}
			pNameTable++;
		}
		pImportTable++;
	}
	return;
}

void CPE::ShowExports(LPVOID Add, DWORD Size)
{
	if (!mHeap) return;
	//1 获取到导出表结构
	char* MOD = (char*)::HeapAlloc(mHeap, NULL, Size);
	if (!MOD || !mcPoint->ReadMemory(Add, MOD, Size))
	{
		printf("\n无法得到内存。\n");
		HeapDestroy(mHeap);
		return;
	}
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)MOD;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + MOD);
	//1 获取到导出表的数据目录结构
	PIMAGE_DATA_DIRECTORY dwImportDir = &pNt->OptionalHeader.DataDirectory[0];
	//1 获取到导出表结构
	PIMAGE_EXPORT_DIRECTORY pExport =
		(PIMAGE_EXPORT_DIRECTORY)(MOD + dwImportDir->VirtualAddress);
	printf("名称导出数量为:0x%X,ID导出数量为:0x%X,共0x%X\n",
		pExport->NumberOfNames, pExport->NumberOfFunctions,
		pExport->NumberOfFunctions + pExport->NumberOfNames);
	PDWORD	pEat = (PDWORD)(MOD + pExport->AddressOfFunctions);
	PDWORD	pEnt = (PDWORD)(MOD + pExport->AddressOfNames);
	PWORD	pEot = (PWORD)(MOD + pExport->AddressOfNameOrdinals);
	for (DWORD i = 0; i < pExport->NumberOfFunctions; i++)
	{
		char* name = 0;
		for (DWORD j = 0; j < pExport->NumberOfFunctions; j++)
		{
			if (i == pEot[j]) {
				name = MOD + pEnt[j];
				break;
			}
		}
		printf("%05ld\t地址=%p\t符号：%s\n", i + 1,
			MOD + pEat[i] + (DWORD)Add,
			(name ? name : "-"));
	}
}

void CPE::DUMP(LPVOID Add, DWORD Size, PWCHAR Name)
{
	if (!mHeap) return;
	//1 获取到导出表结构
	char* MOD = (char*)::HeapAlloc(mHeap, HEAP_ZERO_MEMORY, Size);
	if (!MOD || !mcPoint->ReadMemory(Add, MOD, Size))
	{
		printf("\n无法得到内存。\n");
		HeapDestroy(mHeap);
		return;
	}
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)MOD;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + MOD);

	//修改对齐
	pNt->OptionalHeader.FileAlignment = pNt->OptionalHeader.SectionAlignment;

	//找到区段表头
	IMAGE_SECTION_HEADER* pScn = IMAGE_FIRST_SECTION(pNt);
	while (pScn->VirtualAddress != 0)
	{
		pScn->PointerToRawData = pScn->VirtualAddress;
		pScn++;
	}
	CString str;
	str.Format(L"D:\\%s", Name);
	HANDLE hFile = CreateFileW(
		str,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		0
	);
	if (hFile == INVALID_HANDLE_VALUE)
		return;
	DWORD dwTemp = 0;
	WriteFile(hFile, MOD, Size, &dwTemp, 0);
	//关闭句柄
	CloseHandle(hFile);
}
