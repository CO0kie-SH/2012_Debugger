#pragma once
#include "resource.h"
#include "CDLG_MEM.h"
#include "CDebug.h"

#define defNum_MAX_�ϵ� 6
constexpr PWCHAR gszBreakPoring[] =
{
	L"ժҪ",L"����",/*L"�����",*/L"״̬",
	L"ģ��/��ǩ/�쳣",L"��ַ",L"����"
};


class CMyView
{
public:
	CMyView(CDialogEx* wMain);
	~CMyView();

	BOOL InitView();
	CMenu* GetPlug();
	void SetTime();
	void SetLS(map<LPVOID, BreakPoint>& BreakPoints);


	void DeleLSM1()
	{
		mLS_Main->DeleteAllItems();
	}
private:
	HWND mh_Wind;
	//�ؼ�ָ��
	CDialogEx*	mMain;
	CDLG_MEM*	mDLG_MEM;
	CListCtrl*	mLS_Mem;
	CListCtrl*	mLS_Stack;
	CListCtrl*	mLS_Main;
	int mSecond;

	//������ʱ����
	CString mstr;
	CTime t;
	//�ο�https://blog.csdn.net/HW140701/article/details/53995464
	CStatusBarCtrl m_StatusBar;
};

