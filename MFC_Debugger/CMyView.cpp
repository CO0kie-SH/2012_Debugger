#include "pch.h"
#include "CMyView.h"

CMyView::CMyView(CDialogEx* wMain)
{
	m_StatusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0, 0, 0, 0), wMain, 0);
	CRect rect;
	wMain->GetWindowRect(rect);
	int strPartDim[3] = { rect.Width() / 4, rect.Width() / 3 * 2,rect.Width() / 3 * 3 }; //�ָ�����
	m_StatusBar.SetParts(3, strPartDim);

	m_StatusBar.SetText(L"����", 0, 0);
	m_StatusBar.SetText(L"��ǰ��Χ", 1, 0);
	m_StatusBar.SetText(L"ʱ��", 2, 0);
    //��������״̬���м���ͼ��
}

CMyView::~CMyView()
{
}