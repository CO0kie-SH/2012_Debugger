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
	str->Format(L"插件名1");
	printf("InitPlug()->初始化插件。\n");
}

void MenuSetting()
{
	gdlg.DoModal();
	gData.hDLG = 0;
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
