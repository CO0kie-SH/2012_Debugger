#include "pch.h"
#include "CManage.h"

CManage gcManage;
CString gmStr;
CMyView* gcView;

/*	�������̻߳ص������ڴ��������߳�
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


#pragma region ���ں���
CManage::~CManage()
{
	OutputDebugString(L"~CManage()\n");
	//�رյ�����
	if (gDATA.CDEBUG)
	{
		this->MenuClick(ID_32775);
		WaitForSingleObject(this->mh_Debug, 9000);
	}
	//�رտ���̨
	delete m_cConsole;
	//�ر���ͼ
	delete gcView;
}


BOOL CManage::InitManage(CDialogEx* wMain)
{
	HWND hWnd= wMain->GetSafeHwnd();
	gINFO_mWind.hwMFC = hWnd;

	//��ʼ����Ա����
	//this->mWind = hWnd;
	this->m_Main = wMain;
	this->m_cConsole = new CConsole();

	//��ʼ����ͼ��
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
	case ID_32771:	//���ļ�
		if (gDATA.CDEBUG)	//�Ѿ����ڵ���
		{
			puts("�Ѿ��е����߳��ˡ�");
			return;
		}
		mh_Debug =(HANDLE) _beginthreadex(0, 0, (_beginthreadex_proc_type)ThreadProc, FilePath, 0, 0);
		break;
	case ID_32775:	//�˳�
		if (gDATA.CDEBUG)
		{
			printf("�˳�����%lu,%p��%d\n", gDATA.PS.dwProcessId, gDATA.PS.hProcess,
				::TerminateProcess(gDATA.PS.hProcess, 0));
			WaitForSingleObject(this->mh_Debug, 1000);
		}
		this->m_Main->PostMessageW(WM_CLOSE);
		break;
	case ID_32789:{	//���
		int count = this->m_Plug->GetMenuItemCount();		//��ʼ�������
		for (int i = count; --i;)
			this->m_Plug->DeleteMenu(i, MF_BYPOSITION);		//����ɵĲ˵�
		
		WCHAR buff[MAX_PATH]; CString str;					//���建����
		count = GetModuleFileName(NULL, buff, MAX_PATH);	//�õ�ģ��·��
		if (!count) break;
		PathRemoveFileSpecW(buff);							//ȥ���ļ���
		str.Format(L"%s\\%s\\", buff, PlugPath);			//��ʽ��·��
		printf("  ���·��\n%S\n", str.GetString());			//��ӡ·��
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

		//����LoadLibrary�����ļ�
		Handle = LoadLibrary(Path + FileInfo.cFileName);
		if (Handle == 0) continue;
		printf("���%p\t��׺��%S\t%S\n", Handle, pszExtension, FileInfo.cFileName);


		//����GetProcAddressȡ��ָ���Ĳ����ʼ��������ַ
		nAddress = (DWORD)GetProcAddress(Handle, "InitPlug");

		//���������ȡʧ��
		if (nAddress == 0)
		{
			//FreeLibrary(Handle);
			Handle = nullptr;
			continue;
		}
		//���ó�ʼ������
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
	DebugInfo Info, * pInfo = &Info;					//��ʼ���ṹ��
	Info.isCreate = isCreate;							//��ʼ�����̴�����ʽ
	memcpy(pInfo, pPS, sizeof(PROCESS_INFORMATION));	//��ʼ��������Ϣ

	for (size_t i = 0, max = m_DLLs.size(); i < max; i++)	//ѭ������
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
	//����GetProcAddressȡ��ָ���Ĳ����ʼ��������ַ
	nAddress = (DWORD)GetProcAddress(tmp.hDLL, "MenuSetting");
	if (nAddress == 0) return;
	//���ò˵�����
	__asm call nAddress;
}

void CManage::TabClick(int nID)
{
	this->m_TAB_ID = nID;
	if (nID == 4)			//�ϵ����
	{
		CDebug* cDebug = (CDebug*)gDATA.CDEBUG;
		if (!cDebug) return;	//û�е�����
		gcView->SetLS(cDebug->mBreakPoint);
	}
	else
	{
		gcView->DeleLSM1();
	}
}

void CManage::LSM1RClick(LPNMITEMACTIVATE pNMItemActivate)
{
	//printf("�����ڵ�%d�б��Ҽ���\n", pNMItemActivate->iItem + 1);
	CDebug* cDebug = (CDebug*)gDATA.CDEBUG;
	CListCtrl* ls = gcView->GetLSM1();
	CString str;
	if (this->m_TAB_ID == 4)			//�ϵ����
	{
		str.Format(L"���������ϵ�%s��\n",
			cDebug->AddIFPoint(ls->GetItemData(pNMItemActivate->iItem))
			? L"�ɹ�" : L"ʧ��");
		str += jstr;
		AfxMessageBox(str);
	}
}
#pragma endregion



