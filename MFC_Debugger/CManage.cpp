#include "pch.h"
#include "CManage.h"

CManage gcManage;
CString gmStr;
CMyView* gcView;

/*	�������̻߳ص������ڴ��������߳�
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


#pragma region ���ں���
CManage::CManage()
	:m_cConsole(0), m_Main(0), mh_Debug(0)
{
	
}

CManage::~CManage()
{
	OutputDebugString(L"~CManage()\n");
	
	//�رյ�����
	if (gDATA.CDEBUG)
	{
		this->MenuClick(ID_32775);
		WaitForSingleObject(this->mh_Debug, 1000);
	}
	//�رտ���̨
	delete m_cConsole;
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
	return true;
}

void CManage::MenuClick(UINT_PTR nID)
{
	switch (nID)
	{
	case ID_32771:	//���ļ�
		if (gDATA.CDEBUG)
			break;	//�Ѿ����ڵ���
		mh_Debug =(HANDLE) _beginthreadex(0, 0, (_beginthreadex_proc_type)ThreadProc, FilePath, 0, 0);
		break;
	case ID_32775:	//�˳�
		if (gDATA.CDEBUG) {
			printf("�˳�����%lu,%p��%d\n", gDATA.PS.dwProcessId, gDATA.PS.hProcess,
				::TerminateProcess(gDATA.PS.hProcess, 0));
		}break;
	default: break;
	}
}
#pragma endregion



