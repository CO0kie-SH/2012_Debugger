#include "pch.h"
#include "CMyPoint.h"

#pragma region CMyPoint
BOOL CMyPoint::AddSoftPoint(LPVOID Address, WORD Type, PWCHAR Text)
{
	BreakPoint tmp = this->mBreakPoint[Address];
	if (tmp.TYPES)
	{
		printf("�ϵ��Ѵ��ڣ�%p", Address);
		return 0;
	}
	else
	{
		tmp = BreakPoint{ Type,1,0,0,Address };
		if (NULL == ReadMemory(Address, &tmp.OLD, 1))
			return 0;
		if (NULL == WriteMemory(Address, gszCC, 1))
			return 0;
		if (Text)		//����б�ע����ӱ�ע
			tmp.str = Text;
		this->mBreakPoint[Address] = tmp;
		return TRUE;
	}
	return 0;
}

BOOL CMyPoint::SetMemPoint(LPBreakPoint pPoint)
{
	BOOL bRet = 0;
	DWORD dwSet = PAGE_NOACCESS;
	switch (pPoint->TYPES)
	{
	case defBP_�ڴ�ִ��:
		//dwSet = PAGE_READWRITE;
		break;
	default:
		break;
	}
	return VirtualProtectEx(gDATA.PS.hProcess, pPoint->Address, 1, dwSet, &pPoint->OLD);
}

BOOL CMyPoint::ReSetSoftPoint(LPBreakPoint pPoint)
{
	BOOL bRet = 0;
	if (pPoint->STATU == defBP_�ϵ�ɾ��)
	{
		CONTEXT context = { CONTEXT_FULL };
		GetThreadContext(gDATA.PS.hThread, &context);
		context.Eip -= 1;
		SetThreadContext(gDATA.PS.hThread, &context);
		bRet = this->WriteMemory(pPoint->Address, &pPoint->OLD, 1);
		this->mBreakPoint.erase(pPoint->Address);
	}
	return bRet;
}

BOOL CMyPoint::AddHardPoint(LPVOID Address, WORD Type, PWCHAR Text)
{
	BOOL bRet = 0, bpD7 = 0, bType = 0, bLen = 0;
	BreakPoint tmp = this->mBreakPoint[Address];
	if (tmp.TYPES)
	{
		printf("\n\t�ϵ�%p�Ѵ���!!!\n", Address);
		return 0;
	}
	CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
	GetThreadContext(gDATA.PS.hThread, &ct);


	tmp = BreakPoint{ Type,1,0,0,Address };
	PDBG_REG7 pDr7 = (PDBG_REG7)&ct.Dr7;

	switch (Type)
	{
	case defBP_Ӳ��ִ��:
		bType = 0;
		bLen = 0;
		break;
	case defBP_Ӳ��д��:
		bType = 0;
		bLen = 1;
		break;
	case defBP_Ӳ����д:
		bType = 0;
		bLen = 1;
		break;
	default:
		MessageBox(::GetFocus(), L"�޿��öϵ�����", 0, 0);
		return 0;
	}

	if (ct.Dr0 == 0) {		//DR0û�б�ʹ��
		ct.Dr0 = (DWORD)Address;
		pDr7->RW0 = bType;
		pDr7->LEN0 = bLen;
		pDr7->L0 = 1;		//�����ϵ�
		tmp.OLD = 70;
		bRet = true;
	}
	else if (ct.Dr1 == 0) {	//DR1û�б�ʹ��
		ct.Dr1 = (DWORD)Address;
		pDr7->RW1 = bType;
		pDr7->LEN1 = bLen;
		pDr7->L1 = 1;		//�����ϵ�
		tmp.OLD = 71;
		bRet = true;
	}
	else if (ct.Dr2 == 0) {	//DR2û�б�ʹ��
		ct.Dr2 = (DWORD)Address;
		pDr7->RW2 = bType;
		pDr7->LEN2 = bLen;
		pDr7->L2 = 1;		//�����ϵ�
		tmp.OLD = 72;
		bRet = true;
	}
	else if (ct.Dr3 == 0) {	//DR3û�б�ʹ��
		ct.Dr3 = (DWORD)Address;
		pDr7->RW3 = bType;
		pDr7->LEN3 = bLen;
		pDr7->L3 = 1;		//�����ϵ�
		tmp.OLD = 73;
		bRet = true;
	}
	else
	{
		MessageBox(::GetFocus(), L"�޿���Ӳ���ϵ�", 0, 0);
		return 0;
	}


	//�ж����óɹ�����
	if (bRet) {
		SetThreadContext(gDATA.PS.hThread, &ct);
		if (Text)
			tmp.str = Text;
		this->mBreakPoint[Address] = tmp;
	}
	return bRet;
}

BOOL CMyPoint::AddMemPoint(LPVOID Address, WORD Type, PWCHAR Text)
{

	BOOL bRet = 0, bpD7 = 0, bType = 0, bLen = 0;
	BreakPoint tmp = this->mBreakPoint[Address];
	if (tmp.TYPES)
	{
		printf("�ϵ��Ѵ��ڣ�%p", Address);
		return 0;
	}
	else if (Type == defBP_�ڴ�ִ��)
	{
		tmp = BreakPoint{ Type,1,0,0,Address };
		if (this->SetMemPoint(&tmp))
		{
			if (Text)		//����б�ע����ӱ�ע
				tmp.str = Text;
			this->mBreakPoint[Address] = tmp;
			BreakPoint tmp2 = BreakPoint{ defBP_�ڴ�����,1 };

			bRet = TRUE;
		}
	}
	return bRet;
}

SIZE_T CMyPoint::ReadMemory(LPVOID Address, LPVOID ReadBuff, DWORD_PTR ReadLen)
{
	SIZE_T Read;
	if (NULL == ReadProcessMemory(gDATA.PS.hProcess, Address, ReadBuff, ReadLen, &Read))
	{
		printf("��ȡĿ���ڴ�%pʧ��%lu��\n", Address, ReadLen);
	}
	return Read;
}

SIZE_T CMyPoint::WriteMemory(LPVOID Address, LPVOID WriteBuff, DWORD_PTR WriteLen)
{
	SIZE_T Len = 0;
	if (NULL == WriteProcessMemory(gDATA.PS.hProcess, Address, WriteBuff, WriteLen, &Len))
	{
		printf("����Ŀ���ڴ�%pʧ��%lu��\n", Address, WriteLen);
	}
	return Len;
}
#pragma endregion