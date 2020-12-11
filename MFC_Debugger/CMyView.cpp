#include "pch.h"
#include "CMyView.h"

CMyView::CMyView(CDialogEx* wMain)/*:mDLG_MEM(0)*/
{
	m_StatusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0, 0, 0, 0), wMain, 0);
	CRect rect;
	wMain->GetWindowRect(rect);
	int strPartDim[3] = { rect.Width() / 4, rect.Width() / 3 * 2,rect.Width() / 3 * 3 }; //�ָ�����
	m_StatusBar.SetParts(3, strPartDim);

	m_StatusBar.SetText(L"����", 0, 0);
	m_StatusBar.SetText(L"��ǰ��Χ", 1, 0);
	m_StatusBar.SetText(L"ʱ��", 2, 0);
	this->mMain = wMain;
	this->mh_Wind = wMain->GetSafeHwnd();
}

CMyView::~CMyView()
{
	delete mDLG_MEM;
}

BOOL CMyView::InitView()
{
	RECT rect;
	HWND hWnd = gINFO_mWind.hwCON;
	::GetWindowRect(this->mh_Wind, &rect);

	//���ÿ���̨
	::MoveWindow(hWnd, 0, 55, rect.right-rect.left-24, rect.bottom-rect.top /*- 120*/, 1);
	::UpdateWindow(hWnd);
	//::SetWindowText(hWnd, L"�ȴ����Խ��̽�����");

	//��ʼ���ڴ��Ӵ���
	mDLG_MEM = new CDLG_MEM();
	mDLG_MEM->Create(IDD_DIALOG1, this->mMain);
	mDLG_MEM->ShowWindow(SW_SHOW);

	return true;
}

void CMyView::SetTime()
{
	t = CTime::GetCurrentTime();
	mstr.Format(L"%04d/%02d/%02d  %02d:%02d:%02d", t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
	m_StatusBar.SetText(mstr, 2, 0);
}
