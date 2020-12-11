#pragma once
#include "resource.h"
#include "CDLG_MEM.h"

class CMyView
{
public:
	CMyView(CDialogEx* wMain);
	~CMyView();

	BOOL InitView();
	void SetTime();
private:
	HWND mh_Wind;
	//控件指针
	CDialogEx* mMain;
	CDLG_MEM* mDLG_MEM;

	//创建临时对象
	CString mstr;
	CTime t;
	//参考https://blog.csdn.net/HW140701/article/details/53995464
	CStatusBarCtrl m_StatusBar;
};

