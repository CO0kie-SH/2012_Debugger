#pragma once
#include "resource.h"

class CMyView
{
public:
	CMyView(CDialogEx* wMain);
	~CMyView();

	BOOL InitView();
	void SetTime();
private:
	HWND mWind;
	CDialogEx* mMain;

	CString mstr;
	CTime t;
	//²Î¿¼https://blog.csdn.net/HW140701/article/details/53995464
	CStatusBarCtrl m_StatusBar;
};

