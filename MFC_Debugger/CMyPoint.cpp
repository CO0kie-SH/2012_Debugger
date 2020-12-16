#include "pch.h"
#include "CMyPoint.h"
#include "CDLG_EDIT.h"

#pragma region CMyPoint
BOOL CMyPoint::AddSoftPoint(LPVOID Address, WORD Type, PWCHAR Text)
{
	BreakPoint tmp = this->mBreakPoint[Address];
	if (tmp.TYPES)
	{
		printf("断点已存在：%p", Address);
		return 0;
	}
	else
	{
		tmp = BreakPoint{ Type,1,0,0,Address };
		if (NULL == ReadMemory(Address, &tmp.OLD, 1))
			return 0;
		if (NULL == WriteMemory(Address, gszCC, 1))
			return 0;
		if (Text)		//如果有备注，则加备注
			tmp.str = Text;
		this->mBreakPoint[Address] = tmp;
		return TRUE;
	}
	return 0;
}

//BOOL CMyPoint::SetMemPoint(LPBreakPoint pPoint)
//{
//	BOOL bRet = 0;
//	DWORD dwSet = PAGE_NOACCESS;
//	switch (pPoint->TYPES)
//	{
//	case defBP_内存执行:
//		//dwSet = PAGE_READWRITE;
//		break;
//	default:
//		break;
//	}
//	return VirtualProtectEx(gDATA.PS.hProcess, pPoint->Address, 1, dwSet, &pPoint->OLD);
//}

BOOL CMyPoint::ReSetSoftPoint(LPBreakPoint pPoint)
{
	BOOL bRet = 0;
	if (pPoint->STATU == defBP_断点删除)
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
		printf("\n\t断点%p已存在!!!\n", Address);
		return 0;
	}
	CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
	GetThreadContext(gDATA.PS.hThread, &ct);


	tmp = BreakPoint{ Type,1,0,0,Address };
	PDBG_REG7 pDr7 = (PDBG_REG7)&ct.Dr7;

	switch (Type)
	{
	case defBP_硬件执行:
		bType = 0;
		bLen = 0;
		break;
	case defBP_硬件写入:
		bType = 1;
		bLen = 1;
		break;
	case defBP_硬件读写:
		bType = 3;
		bLen = 1;
		break;
	default:
		MessageBox(::GetFocus(), L"无可用断点设置", 0, 0);
		return 0;
	}

	if (ct.Dr0 == 0) {		//DR0没有被使用
		ct.Dr0 = (DWORD)Address;
		pDr7->RW0 = bType;
		pDr7->LEN0 = bLen;
		pDr7->L0 = 1;		//开启断点
		tmp.OLD = 70;
		bRet = true;
	}
	else if (ct.Dr1 == 0) {	//DR1没有被使用
		ct.Dr1 = (DWORD)Address;
		pDr7->RW1 = bType;
		pDr7->LEN1 = bLen;
		pDr7->L1 = 1;		//开启断点
		tmp.OLD = 71;
		bRet = true;
	}
	else if (ct.Dr2 == 0) {	//DR2没有被使用
		ct.Dr2 = (DWORD)Address;
		pDr7->RW2 = bType;
		pDr7->LEN2 = bLen;
		pDr7->L2 = 1;		//开启断点
		tmp.OLD = 72;
		bRet = true;
	}
	else if (ct.Dr3 == 0) {	//DR3没有被使用
		ct.Dr3 = (DWORD)Address;
		pDr7->RW3 = bType;
		pDr7->LEN3 = bLen;
		pDr7->L3 = 1;		//开启断点
		tmp.OLD = 73;
		bRet = true;
	}
	else
	{
		MessageBox(::GetFocus(), L"无可用硬件断点", 0, 0);
		return 0;
	}


	//判断设置成功了吗
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
	DWORD_PTR dwAdd = (DWORD_PTR)Address / 0x1000;
	dwAdd *= 0x1000;
	if (this->MemyPoint)
	{
		printf("\n\t断点已存在：%p\n", Address);
		return 0;
	}
	this->mBreakPoint[(LPVOID)dwAdd] = { Type,1,0,0,Address };
	return this->SetMemPoint((LPVOID)dwAdd);
}

BOOL CMyPoint::AddIFPoint(DWORD_PTR Address)
{
	if (this->mWait.IFPoint)
	{
		jstr = L"条件断点已存在。";
		return 0;
	}
	LPBreakPoint tmp = this->GetBP((LPVOID)Address);
	if (tmp == 0)
	{
		jstr = L"没有该断点。";
		return 0;
	}
	if (tmp->TYPES!=defBP_软件执行)
	{
		jstr = L"目前只支持软件断点的条件。";
		return 0;
	}
	jstr.Format(L"请设置%p的条件断点。", tmp->Address);
	CDLG_EDIT edit;
	edit.DoModal();
	if (jstr == L"")
	{
		jstr = L"没有输入字符串。";
		return 0;
	}
	CStringA strA(jstr);
	char *cmdline = (char*)(LPCSTR)strA,
		buff1[40] = {0},
		buff2[40] = {0};
	int i;
	sscanf_s(cmdline, "%s %s %d", buff1, 40, buff2, 40, &i);
	BreakPointIF tmpIF = {
		0,1,i,0,tmp
	};
	if (strcmp(buff1, "Cout") == 0 ||
		strcmp(buff1, "Count") == 0)
	{
		if (strcmp(buff2, "==") == 0)
		{
			return AddIFPoint(&tmpIF, TYPE_计次相等, "条件相等");
		}
	}
	jstr = L"无法识别条件语句。";
	return 0;
}

BOOL CMyPoint::AddIFPoint(LPBreakPointIF BP, WORD Type, PCHAR Text)
{
	LPBreakPointIF tmp = new BreakPointIF{ 0 };
	memcpy(tmp, BP, sizeof(BreakPointIF));
	tmp->TYPES = Type;
	tmp->str = Text;
	this->mWait.IFPoint = tmp;
	return TRUE;
}

BOOL CMyPoint::SetMemPoint(LPVOID Address, BOOL isRe/* = 0*/)
{
	BOOL bRet = 0, bSet = PAGE_NOACCESS;
	BreakPoint& tmp = this->mBreakPoint[Address];

	//通过内存属性修改变量
	switch (tmp.TYPES)
	{
	case defBP_内存写入: 
		bSet = PAGE_EXECUTE_READ; break;
	case defBP_内存读取: 
		bSet = PAGE_NOACCESS; break;
	case defBP_内存执行: 
		bSet = PAGE_NOACCESS; break;
	default:
		printf("联系作者添加功能。\n");
		this->Bug(BUG_内存异常错误);
		break;
	}
	if (tmp.STATU == defBP_断点删除 ||
		tmp.STATU == defBP_内存暂停) {
		bSet = tmp.OLD;
	}
	bRet = VirtualProtectEx(
		gDATA.PS.hProcess, Address, 1, bSet, &tmp.OLD);
	if (!bRet || tmp.STATU == defBP_断点删除)
	{
		this->mBreakPoint.erase(Address);
		this->MemyPoint = 0;
	}
	else
		this->MemyPoint = &tmp;
	return bRet;
}

/*	函数：设置单步断点
*/
BOOL CMyPoint::SetTFPoint(BOOL isSetFlag)
{
	if (isSetFlag)
		this->mWait.PassPoint = TRUE;
	// 线程上下文
	CONTEXT context = { CONTEXT_FULL };
	// 获取线程上下文
	GetThreadContext(gDATA.PS.hThread, &context);
	((PEFLAGS)&context.EFlags)->TF = 1;
	// 设置线程上下文
	return SetThreadContext(gDATA.PS.hThread, &context);
}

/*	函数：作废
*/
BOOL CMyPoint::OnPointIF(LPDEBUG_EVENT pDbg_event, LPDWORD dbg_status)
{
	BreakPointIF tmp = *this->mWait.IFPoint;

	//获取并设置单步
	CONTEXT context = { CONTEXT_FULL };
	GetThreadContext(gDATA.PS.hThread, &context);
	((PEFLAGS)&context.EFlags)->TF = 1;
	SetThreadContext(gDATA.PS.hThread, &context);
		
	if (context.Eip == 0x401072)
	{
		*dbg_status = DBG_EXCEPTION_NOT_HANDLED;
		return 7;
	}
	if (context.Eip & 0x70000000)
	{
		*dbg_status = DBG_CONTINUE;
		return 7;
	}

	LPDWORD es = &context.Edi;
	for (WORD i = 0; i < 8; i++)
	{
		//if (i == tmp.TYPES)
		//	printf("寄存器%s->0x%08lX==%s->%p\n",
		//		gszRegIFs[i], es[i], tmp.str, tmp.Address);
	}

	if (es[tmp.TYPES] == tmp.OLD)
	{
		printf("命中\n");
		*dbg_status = DBG_CONTINUE;
		return TRUE;
	}
	else
	{
	}
	return 0;
}

SIZE_T CMyPoint::ReadMemory(LPVOID Address, LPVOID ReadBuff, DWORD_PTR ReadLen)
{
	SIZE_T Read;
	if (NULL == ReadProcessMemory(gDATA.PS.hProcess, Address, ReadBuff, ReadLen, &Read))
	{
		printf("读取目标内存%p失败%lu。\n", Address, ReadLen);
	}
	return Read;
}

SIZE_T CMyPoint::WriteMemory(LPVOID Address, LPVOID WriteBuff, DWORD_PTR WriteLen)
{
	SIZE_T Len = 0;
	if (NULL == WriteProcessMemory(gDATA.PS.hProcess, Address, WriteBuff, WriteLen, &Len))
	{
		DWORD oldProtect;
		// 1.修改目标分页属性
		VirtualProtectEx(gDATA.PS.hProcess, Address, 1, PAGE_READWRITE, &oldProtect);
		WriteProcessMemory(gDATA.PS.hProcess, Address, WriteBuff, WriteLen, &Len);
		//4. 恢复目标分页属性
		VirtualProtectEx(gDATA.PS.hProcess, Address, 1, oldProtect, &oldProtect);
		if (Len == 0)
			printf("设置目标内存%p失败%lu。\n", Address, WriteLen);
	}
	return Len;
}
int CMyPoint::Bug(int ID)
{
	printf("错误码：%X，", ID);
	this->mWait.LineShow = "请联系作者修复：";
	//this->OnLine();
	::TerminateProcess(gDATA.PS.hProcess, 0);
	WaitForSingleObject(gDATA.PS.hProcess, 1000);
	return 0;
}
#pragma endregion