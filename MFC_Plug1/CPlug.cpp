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

#pragma region �������
void InitPlug(void* pStr)
{
	CString* str = (CString*)pStr;
	str->Format(L"��������");
	printf("InitPlug()->��ʼ�������\n");
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
	printf("%p���ұ���������\n", hInstance);
	ghook = (CHOOK*)DLL_GetGC();
}

void CPlug::ChangeData()
{

}

CPlug::~CPlug()
{
	printf("%p���ұ�ж������\n", ghInstance);
}
