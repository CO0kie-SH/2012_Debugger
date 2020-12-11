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
	debug.InitDebug(Path);
	return 0;
}


#pragma region 类内函数
CManage::CManage()
	:m_cConsole(0), m_Main(0)/*, mWind(0)*/
{

}

CManage::~CManage()
{
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
		_beginthreadex(0, 0, (_beginthreadex_proc_type)ThreadProc, FilePath, 0, 0);
		break;
	case ID_32775:	//退出
		break;
	default: break;
	}
}
#pragma endregion



