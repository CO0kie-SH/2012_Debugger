#include "pch.h"
#include "CMyView.h"

CMyView::CMyView(CDialogEx* wMain)
{
	m_StatusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0, 0, 0, 0), wMain, 0);
	CRect rect;
	wMain->GetWindowRect(rect);
	int strPartDim[3] = { rect.Width() / 4, rect.Width() / 3 * 2,rect.Width() / 3 * 3 }; //分割数量
	m_StatusBar.SetParts(3, strPartDim);

	m_StatusBar.SetText(L"就绪", 0, 0);
	m_StatusBar.SetText(L"当前范围", 1, 0);
	m_StatusBar.SetText(L"时间", 2, 0);
    //下面是在状态栏中加入图标
}

CMyView::~CMyView()
{
}