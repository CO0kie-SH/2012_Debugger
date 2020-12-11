#include "pch.h"
#include "CManage.h"

CManage gcManage;
CString gmStr;
CMyView* gcView;

/*	函数：线程回调，用于创建调试线程
*/
unsigned __stdcall ThreadProc(PWCHAR Path)
{
	CDebug* debug = new CDebug();
	gDATA.CDEBUG = debug;
	debug->InitDebug(Path);
	delete debug;
	gDATA.CDEBUG = 0;
	return 0;
}


#pragma region 类内函数
CManage::CManage()
	:m_cConsole(0), m_Main(0), mh_Debug(0)
{
	
}

CManage::~CManage()
{
	OutputDebugString(L"~CManage()\n");
	
	//关闭调试器
	if (gDATA.CDEBUG)
	{
		this->MenuClick(ID_32775);
		WaitForSingleObject(this->mh_Debug, 1000);
	}
	//关闭控制台
	delete m_cConsole;
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
		if (gDATA.CDEBUG)
			break;	//已经存在调试
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
#pragma endregion



