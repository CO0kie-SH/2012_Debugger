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
	printf("%p，我被加载啦。\n", hInstance);
}

CPlug::~CPlug()
{
	printf("%p，我被卸载啦。\n", ghInstance);
}
