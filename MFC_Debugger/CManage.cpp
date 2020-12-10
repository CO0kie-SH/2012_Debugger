#include "pch.h"
#include "CManage.h"

CManage gcManage;
CMyView* gcView = 0;


#pragma region ÀàÄÚº¯Êý


CManage::CManage()
	:m_cConsole(0), mWind(0), m_Main(0)
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

	this->mWind = hWnd;
	this->m_Main = wMain;
	this->m_cConsole = new CConsole();

	gcView = new CMyView(wMain);
	return true;
}
#pragma endregion



