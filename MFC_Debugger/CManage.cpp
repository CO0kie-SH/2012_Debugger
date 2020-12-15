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
		str.Format(L"%s\\%s\\*.DLL", buff, PlugPath);		//��ʽ��·��
		printf("\t���·��\n%S\n", str.GetString());			//��ӡ·��
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
		printf("��׺��%S\t%S\n", pszExtension, FileInfo.cFileName);
	} while (FindNextFile(FindHandle, &FileInfo));
	return 0;
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
		str.Format(L"���������ϵ�%s��\n%s",
			cDebug->AddIFPoint(ls->GetItemData(pNMItemActivate->iItem))
			? L"�ɹ�" : L"ʧ��", jstr);
		AfxMessageBox(str);
	}
}
#pragma endregion



