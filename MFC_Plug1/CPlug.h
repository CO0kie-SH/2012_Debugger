#pragma once
#include "CDLG_Plug.h"

extern CDLG_Plug gdlg;
extern unsigned __stdcall ThreadProc();

EXTERN_C AFX_API_EXPORT void InitPlug(void* pStr);
EXTERN_C AFX_API_EXPORT void MenuSetting();

class CPlug
{
public:
	~CPlug();
	void InitCPlug(HINSTANCE hInstance);
	void ChangeData() {
		printf("数据更改。");
	};
};
extern CPlug gcPlug;
