#pragma once
#include "CDLG_Plug.h"

EXTERN_C AFX_API_EXPORT int DLL_HOOK(int hPid);
//��ȫ�̻߳ص�
extern unsigned __stdcall ThreadProc();

//�����������
EXTERN_C AFX_API_EXPORT void InitPlug(void* pStr);
EXTERN_C AFX_API_EXPORT void DebugCreate(void* pInfo);
EXTERN_C AFX_API_EXPORT void MenuSetting();

class CPlug
{
public:
	~CPlug();
	void InitCPlug(HINSTANCE hInstance);
	void ChangeData();
};
extern CPlug gcPlug;
