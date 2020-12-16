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
	//1 ��ȡ�������ṹ
	char* MOD = (char*)::HeapAlloc(mHeap, NULL, Size);
	if (!MOD || !mcPoint->ReadMemory(Add, MOD, Size))
	{
		printf("\n�޷��õ��ڴ档\n");
		HeapDestroy(mHeap);
		return;
	}
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)MOD;
	//1 ��ȡ������������Ŀ¼�ṹ
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + MOD);
	//1 ��ȡ������������Ŀ¼�ṹ
	PIMAGE_DATA_DIRECTORY dwImportDir = &pNt->OptionalHeader.DataDirectory[1];
	//1 ��ȡ�������ṹ
	PIMAGE_IMPORT_DESCRIPTOR pImportTable =
		(PIMAGE_IMPORT_DESCRIPTOR)(MOD + dwImportDir->VirtualAddress);
	//2 ���������
	printf_s("IAT���ַ %p\n", pImportTable);

	//2 ��ʼ����
	while (pImportTable->Name != 0)
	{
		printf("\n\tģ�飺%s\n", pImportTable->Name + MOD);	//�����DLL��
		PIMAGE_THUNK_DATA32 pNameTable = (PIMAGE_THUNK_DATA32)
			(pImportTable->OriginalFirstThunk + MOD);
		while (pNameTable->u1.Ordinal != 0)
		{
			//2.3.1 �ж����λ�ǲ���1
			if (IMAGE_SNAP_BY_ORDINAL32(pNameTable->u1.Ordinal) == 1)
			{
				//ֻ�����
				printf("%05ld\t���ţ�-\n", pNameTable->u1.Ordinal & 0x7FFFFFFF);
			}
			else
			{
				//�������֣��������
				PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)
					(pNameTable->u1.AddressOfData + MOD);
				printf("%05ld\t���ţ�%s\n", pName->Hint, pName->Name);
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
	//1 ��ȡ��������ṹ
	char* MOD = (char*)::HeapAlloc(mHeap, NULL, Size);
	if (!MOD || !mcPoint->ReadMemory(Add, MOD, Size))
	{
		printf("\n�޷��õ��ڴ档\n");
		HeapDestroy(mHeap);
		return;
	}
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)MOD;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + MOD);
	//1 ��ȡ�������������Ŀ¼�ṹ
	PIMAGE_DATA_DIRECTORY dwImportDir = &pNt->OptionalHeader.DataDirectory[0];
	//1 ��ȡ��������ṹ
	PIMAGE_EXPORT_DIRECTORY pExport =
		(PIMAGE_EXPORT_DIRECTORY)(MOD + dwImportDir->VirtualAddress);
	printf("���Ƶ�������Ϊ:0x%X,ID��������Ϊ:0x%X,��0x%X\n",
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
		printf("%05ld\t��ַ=%p\t���ţ�%s\n", i + 1,
			MOD + pEat[i] + (DWORD)Add,
			(name ? name : "-"));
	}
}

void CPE::DUMP(LPVOID Add, DWORD Size, PWCHAR Name)
{
	if (!mHeap) return;
	//1 ��ȡ��������ṹ
	char* MOD = (char*)::HeapAlloc(mHeap, HEAP_ZERO_MEMORY, Size);
	if (!MOD || !mcPoint->ReadMemory(Add, MOD, Size))
	{
		printf("\n�޷��õ��ڴ档\n");
		HeapDestroy(mHeap);
		return;
	}
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)MOD;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + MOD);

	//�޸Ķ���
	pNt->OptionalHeader.FileAlignment = pNt->OptionalHeader.SectionAlignment;

	//�ҵ����α�ͷ
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
	//�رվ��
	CloseHandle(hFile);
}
