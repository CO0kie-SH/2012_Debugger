#pragma once
#include "CDLG_Plug.h"

extern CDLG_Plug gdlg;
extern unsigned __stdcall ThreadProc();

class CPlug
{
public:
	~CPlug();
	void InitCPlug(HINSTANCE hInstance);
};
extern CPlug gcPlug;
