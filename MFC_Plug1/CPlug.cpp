#include "pch.h"
#include "CPlug.h"

CDLG_Plug gdlg;
CPlug gcPlug;

unsigned __stdcall ThreadProc()
{
	gdlg.DoModal();
	return 0;
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
