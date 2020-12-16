#include "pch.h"
#include "CPlug.h"
#include "..\DLL_HOOK\CHOOK.h"
#pragma comment(lib,"..\\Debug\\DLL_HOOK.lib")

CDLG_Plug gdlg;
CPlug gcPlug;
CString gstr;
CHOOK* ghook = 0;

int DLL_HOOK(int hPid)
{
	return OnHook((HANDLE)hPid);
}

unsigned __stdcall ThreadProc()
{
	gdlg.DoModal();
	return 0;
}

#pragma region 插件函数
void InitPlug(void* pStr)
{
	CString* str = (CString*)pStr;
	str->Format(L"反反调试");
	printf("InitPlug()->初始化插件。\n");
}

void DebugCreate(void* pInfo)
{
	DebugInfo& Info = gData.Info;
	memcpy(&Info, pInfo, sizeof(DebugInfo));

	//CONTEXT context = { CONTEXT_FULL };
	//GetThreadContext(Info.PS.hThread, &context);
	return;
}

void MenuSetting()
{
	gdlg.DoModal();
	gData.hDLG = 0;
}
#pragma endregion

void CPlug::InitCPlug(HINSTANCE hInstance)
{
	ghInstance = hInstance;
	printf("%p，我被加载啦。\n", hInstance);
	ghook = (CHOOK*)DLL_GetGC();
}

void CPlug::ChangeData()
{

}

CPlug::~CPlug()
{
	printf("%p，我被卸载啦。\n", ghInstance);
}
