#include "pch.h"
#include "CPlug.h"

CDLG_Plug gdlg;
CPlug gcPlug;

unsigned __stdcall ThreadProc()
{
	return 0;
}

void InitPlug(void* pStr)
{
	CString* str = (CString*)pStr;
	str->Format(L"�����1");
	printf("InitPlug()->��ʼ�������\n");
}

void MenuSetting()
{
	gdlg.DoModal();
	gData.hDLG = 0;
}

void CPlug::InitCPlug(HINSTANCE hInstance)
{
	ghInstance = hInstance;
	printf("%p���ұ���������\n", hInstance);
}

CPlug::~CPlug()
{
	printf("%p���ұ�ж������\n", ghInstance);
}
