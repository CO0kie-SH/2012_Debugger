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
		if (gDATA.CDEBUG) {
			printf("�˳�����%lu,%p��%d\n", gDATA.PS.dwProcessId, gDATA.PS.hProcess,
				::TerminateProcess(gDATA.PS.hProcess, 0));
		}break;
	case ID_32789:{	//���
		int count = this->m_Plug->GetMenuItemCount();
		//����ɵĲ˵�
		for (int i = count; --i;)
			this->m_Plug->DeleteMenu(i, MF_BYPOSITION);
		WCHAR buff[MAX_PATH];
		
		count = GetCurrentDirectoryW(MAX_PATH, buff);
		CString str;
		str.Format(L"%s\\Plugs\\", buff);
		printf("\t���·��\n%S\n", str.GetString());
	}break;
	default: break;
	}
}

void CManage::TabClick(int nID)
{
	CDebug* cDebug = (CDebug*)gDATA.CDEBUG;
	if (!cDebug) return;	//û�е�����
	if (nID == 4)			//�ϵ����
	{
		gcView->SetLS(cDebug->mBreakPoint);
	}
	else
	{
		gcView->DeleLSM1();
	}
}
#pragma endregion



