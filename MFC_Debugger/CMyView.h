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
	//�ؼ�ָ��
	CDialogEx* mMain;
	CDLG_MEM* mDLG_MEM;

	//������ʱ����
	CString mstr;
	CTime t;
	//�ο�https://blog.csdn.net/HW140701/article/details/53995464
	CStatusBarCtrl m_StatusBar;
};

