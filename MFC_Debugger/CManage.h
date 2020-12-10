#pragma once
#include "CConsole.h"
#include "CMyView.h"

class CManage
{
public:
	CManage();
	~CManage();
	BOOL InitManage(CDialogEx* wMain);
private:
	CConsole* m_cConsole;
	CDialogEx* m_Main;
	HWND mWind;
};

extern CManage gcManage;
extern CMyView* gcView;