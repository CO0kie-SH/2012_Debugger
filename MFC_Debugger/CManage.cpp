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
	this->m_Plug = gcView->GetPlug();
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
		if (gDATA.CDEBUG)
		{
			printf("退出进程%lu,%p：%d\n", gDATA.PS.dwProcessId, gDATA.PS.hProcess,
				::TerminateProcess(gDATA.PS.hProcess, 0));
			WaitForSingleObject(this->mh_Debug, 1000);
		}
		this->m_Main->PostMessageW(WM_CLOSE);
		break;
	case ID_32789:{	//插件
		int count = this->m_Plug->GetMenuItemCount();		//初始化插件数
		for (int i = count; --i;)
			this->m_Plug->DeleteMenu(i, MF_BYPOSITION);		//清除旧的菜单
		
		WCHAR buff[MAX_PATH]; CString str;					//定义缓冲区
		count = GetModuleFileName(NULL, buff, MAX_PATH);	//得到模块路径
		if (!count) break;
		PathRemoveFileSpecW(buff);							//去除文件名
		str.Format(L"%s\\%s\\*.DLL", buff, PlugPath);		//格式化路径
		printf("\t插件路径\n%S\n", str.GetString());			//打印路径
		InitPlugs(str);
	}break;
	default: break;
	}
}

BOOL CManage::InitPlugs(CString& Path)
{
	WIN32_FIND_DATA FileInfo = { 0 };
	HANDLE FindHandle = FindFirstFile(Path, &FileInfo);
	if (FindHandle == INVALID_HANDLE_VALUE) return 0;
	do{
		LPTSTR pszExtension = PathFindExtension(FileInfo.cFileName);
		printf("后缀：%S\t%S\n", pszExtension, FileInfo.cFileName);
	} while (FindNextFile(FindHandle, &FileInfo));
	return 0;
}

void CManage::TabClick(int nID)
{
	this->m_TAB_ID = nID;
	if (nID == 4)			//断点管理
	{
		CDebug* cDebug = (CDebug*)gDATA.CDEBUG;
		if (!cDebug) return;	//没有调试器
		gcView->SetLS(cDebug->mBreakPoint);
	}
	else
	{
		gcView->DeleLSM1();
	}
}

void CManage::LSM1RClick(LPNMITEMACTIVATE pNMItemActivate)
{
	//printf("主窗口第%d行被右键了\n", pNMItemActivate->iItem + 1);
	CDebug* cDebug = (CDebug*)gDATA.CDEBUG;
	CListCtrl* ls = gcView->GetLSM1();
	CString str;
	if (this->m_TAB_ID == 4)			//断点管理
	{
		str.Format(L"增加条件断点%s。\n%s",
			cDebug->AddIFPoint(ls->GetItemData(pNMItemActivate->iItem))
			? L"成功" : L"失败", jstr);
		AfxMessageBox(str);
	}
}
#pragma endregion



