#pragma once
#include "CConsole.h"
#include "CMyView.h"
#include "CDebug.h"

class CManage
{
public:
	CManage();
	~CManage();
	void MenuClick(UINT_PTR nID);

	BOOL InitManage(CDialogEx* wMain);
private:
	CConsole* m_cConsole;
	CDialogEx* m_Main;
	//HWND mWind;
	HANDLE mh_Debug;
};

extern CManage gcManage;
extern CMyView* gcView;