#pragma once
#include "CConsole.h"
#include "CMyView.h"
#include "CDebug.h"

class CManage
{
public:
	CManage()
		:m_cConsole(0), m_Main(0), mh_Debug(0)
	{
	}
	~CManage();
	void MenuClick(UINT_PTR nID);
	void TabClick(int nID);

	BOOL InitManage(CDialogEx* wMain);
private:
	CConsole* m_cConsole;
	CDialogEx* m_Main;
	//HWND mWind;
	HANDLE mh_Debug;
};

extern CManage gcManage;
extern CMyView* gcView;