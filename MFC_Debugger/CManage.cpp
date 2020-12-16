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
	gDATA.isCreate = 0;
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
	if (nID >= 5000 && nID < 6000)
	{
		this->MenuClickDLL(nID - 5002);
		return;
	}
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
		str.Format(L"%s\\%s\\", buff, PlugPath);			//格式化路径
		printf("  插件路径\n%S\n", str.GetString());			//打印路径
		if (InitPlugs(str))
			gcView->SetMenu(m_Plug, m_DLLs);
	}break;
	default: break;
	}
}

BOOL CManage::InitPlugs(CString& Path)
{
	WIN32_FIND_DATA FileInfo = { 0 };
	HMODULE Handle = nullptr;
	HANDLE FindHandle = FindFirstFile(Path + _T("*.dll"), &FileInfo);
	DWORD nAddress = 0, nID = 1;
	CString str, * pstr = &str;
	if (FindHandle == INVALID_HANDLE_VALUE) return 0;
	do{
		LPTSTR pszExtension = PathFindExtension(FileInfo.cFileName);

		//调用LoadLibrary加载文件
		Handle = LoadLibrary(Path + FileInfo.cFileName);
		if (Handle == 0) continue;
		printf("句柄%p\t后缀：%S\t%S\n", Handle, pszExtension, FileInfo.cFileName);


		//调用GetProcAddress取出指定的插件初始化函数地址
		nAddress = (DWORD)GetProcAddress(Handle, "InitPlug");

		//如果函数获取失败
		if (nAddress == 0)
		{
			//FreeLibrary(Handle);
			Handle = nullptr;
			continue;
		}
		//调用初始化函数
		str = L"";
		__asm
		{
			push pstr;
			call nAddress;
			add esp, 0x4;
		}
		m_DLLs.push_back({ ++nID,Handle,str });
	} while (FindNextFile(FindHandle, &FileInfo));
	return m_DLLs.size() > 0;
}

void CManage::DebugCreate(PROCESS_INFORMATION* pPS, BOOL isCreate)
{
	DebugInfo Info, * pInfo = &Info;					//初始化结构体
	Info.isCreate = isCreate;							//初始化进程创建方式
	memcpy(pInfo, pPS, sizeof(PROCESS_INFORMATION));	//初始化进程信息

	for (size_t i = 0, max = m_DLLs.size(); i < max; i++)	//循环容器
	{
		auto &tmp = m_DLLs[i];
		DWORD nAddress = (DWORD)GetProcAddress(tmp.hDLL, "DebugCreate");
		if (nAddress == 0) continue;
		__asm
		{
			push pInfo;
			call nAddress;
			add esp, 0x4;
		}
	}
}

void CManage::MenuClickDLL(UINT_PTR nID)
{
	DLLINFO& tmp = this->m_DLLs[nID];
	DWORD nAddress = 0;
	CString str, * pstr = &str;
	//调用GetProcAddress取出指定的插件初始化函数地址
	nAddress = (DWORD)GetProcAddress(tmp.hDLL, "MenuSetting");
	if (nAddress == 0) return;
	//调用菜单函数
	__asm call nAddress;
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
		str.Format(L"增加条件断点%s。\n",
			cDebug->AddIFPoint(ls->GetItemData(pNMItemActivate->iItem))
			? L"成功" : L"失败");
		str += jstr;
		AfxMessageBox(str);
	}
}
#pragma endregion



