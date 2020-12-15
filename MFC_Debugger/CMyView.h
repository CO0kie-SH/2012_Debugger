#pragma once
#include "resource.h"
#include "CDLG_MEM.h"
#include "CDLG_EDIT.h"
#include "CDebug.h"
#include <vector>
using std::vector;

#define defNum_MAX_断点 6
constexpr PWCHAR gszBreakPoring[] =
{
	L"摘要",L"命中",/*L"反汇编",*/L"状态",
	L"模块/标签/异常",L"地址",L"类型"
};

typedef struct _DLLINFO
{
	DWORD MenuID;
	HMODULE hDLL;
	CString name;
}DLLINFO, * LPDLLINFO;

class CMyView
{
public:
	CMyView(CDialogEx* wMain);
	~CMyView();

	BOOL InitView();
	CMenu* GetPlug();
	void SetTime();
	void SetLS(map<LPVOID, BreakPoint>& BreakPoints);
	void SetMenu(CMenu* m_Plug, vector<DLLINFO>& INFOs);


	void DeleLSM1()
	{
		mLS_Main->DeleteAllItems();
	}
	CListCtrl* GetLSM1()
	{
		return mLS_Main;
	}
private:
	HWND mh_Wind;
	//控件指针
	CDialogEx*	mMain;
	CDLG_MEM*	mDLG_MEM;
	CListCtrl*	mLS_Mem;
	CListCtrl*	mLS_Stack;
	CListCtrl*	mLS_Main;
	int mSecond;

	//创建临时对象
	CString mstr;
	CTime t;
	//参考https://blog.csdn.net/HW140701/article/details/53995464
	CStatusBarCtrl m_StatusBar;
};

