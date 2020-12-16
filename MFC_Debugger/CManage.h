#pragma once
#include "CConsole.h"
#include "CMyView.h"

class CManage
{
public:
	vector<DLLINFO> m_DLLs;
public:
	CManage()
		:m_cConsole(0), m_Main(0),
		mh_Debug(0), m_Plug(0),
		m_TAB_ID(0)
	{
	}
	~CManage();
	void MenuClick(UINT_PTR nID);
	void MenuClickDLL(UINT_PTR nID);
	void TabClick(int nID);
	void LSM1RClick(LPNMITEMACTIVATE pNMItemActivate);

	BOOL InitManage(CDialogEx* wMain);
	BOOL InitPlugs(CString& Path);
	void DebugCreate(PROCESS_INFORMATION* pPS, BOOL isCreate);
private:
	CConsole* m_cConsole;
	CDialogEx* m_Main;
	CMenu* m_Plug;
	//HWND mWind;
	HANDLE mh_Debug;
	int m_TAB_ID;
};

extern CManage gcManage;
extern CMyView* gcView;