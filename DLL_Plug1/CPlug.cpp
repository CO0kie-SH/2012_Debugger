#include "pch.h"
#include "CPlug.h"
#include <atlstr.h>
HMODULE ghModule = 0;
CPlug gcPlug;
CString gstr;

#pragma region 类外函数
void InitPlugin(void* pStr)
{
	CString* str = (CString*)pStr;
	str->Format(L"插件名1");
	printf("InitPlugin()->初始化插件。\n");
}

void MenuSetting(int hWnd)
{
	
}
#pragma endregion


#pragma region 类内函数
void CPlug::InitCPlug(HMODULE hModule)
{
	ghModule = hModule;
	printf("CPlug()->我被初始化了%p。\n", hModule);
}

void CPlug::ExitCPlug(HMODULE hModule)
{

}
#pragma endregion

