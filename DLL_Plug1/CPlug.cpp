#include "pch.h"
#include "CPlug.h"
#include <atlstr.h>
HMODULE ghModule = 0;
CPlug gcPlug;
CString gstr;

#pragma region ���⺯��
void InitPlugin(void* pStr)
{
	CString* str = (CString*)pStr;
	str->Format(L"�����1");
	printf("InitPlugin()->��ʼ�������\n");
}

void MenuSetting(int hWnd)
{
	
}
#pragma endregion


#pragma region ���ں���
void CPlug::InitCPlug(HMODULE hModule)
{
	ghModule = hModule;
	printf("CPlug()->�ұ���ʼ����%p��\n", hModule);
}

void CPlug::ExitCPlug(HMODULE hModule)
{

}
#pragma endregion

