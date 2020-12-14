#include "pch.h"
#include "CManage.h"

CManage gcManage;
CString gmStr;
CMyView* gcView;

/*	函数：线程回调，用于创建调试线程
*/
unsigned __stdcall ThreadProc(PWCHAR Path)
{
	CDebug debug;
	gDATA.CDEBUG = &debug;
	debug.InitDebug(Path);
	gDATA.CDEBUG = 0;
	return 0;
}


#pragma region 类内函数
CManage::~CManage()
{
	OutputDebugString(L"~CManage()\n");
	//关闭调试器
	if (gDATA.CDEBUG)
	{
		this->MenuClick(ID_32775);
		WaitForSingleObject(this->mh_Debug, 9000);
	}
	//关闭控制台
	delete m_cConsole;
	//关闭视图
	delete gcView;
}


BOOL CManage::InitManage(CDialogEx* wMain)
{
	HWND hWnd= wMain->GetSafeHwnd();
	gINFO_mWind.hwMFC = hWnd;

	//初始化成员变量
	//this->mWind = hWnd;
	this->m_Main = wMain;
	this->m_cConsole = new CConsole();

	//初始化视图类
	gcView = new CMyView(wMain);
	return true;
}

void CManage::MenuClick(UINT_PTR nID)
{
	switch (nID)
	{
	case ID_32771:	//打开文件
		if (gDATA.CDEBUG)	//已经存在调试
		{
			puts("已经有调试线程了。");
			return;
		}
		mh_Debug =(HANDLE) _beginthreadex(0, 0, (_beginthreadex_proc_type)ThreadProc, FilePath, 0, 0);
		break;
	case ID_32775:	//退出
		if (gDATA.CDEBUG) {
			printf("退出进程%lu,%p：%d\n", gDATA.PS.dwProcessId, gDATA.PS.hProcess,
				::TerminateProcess(gDATA.PS.hProcess, 0));
		}break;
	default: break;
	}
}

void CManage::TabClick(int nID)
{
	CDebug* cDebug = (CDebug*)gDATA.CDEBUG;
	if (!cDebug) return;	//没有调试器
	if (nID == 4)			//断点管理
	{
		gcView->SetLS(cDebug->mBreakPoint);
	}
	else
	{
		gcView->DeleLSM1();
	}
}
#pragma endregion



