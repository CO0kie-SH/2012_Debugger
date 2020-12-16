// CDLG_Plug.cpp: 实现文件
//

#include "pch.h"
#include "CDLG_Plug.h"
#include "CPlug.h"


// CDLG_Plug 对话框

IMPLEMENT_DYNAMIC(CDLG_Plug, CDialogEx)

CDLG_Plug::CDLG_Plug(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CDLG_Plug::~CDLG_Plug()
{
}

void CDLG_Plug::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLG_Plug, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDLG_Plug::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDLG_Plug 消息处理程序


void CDLG_Plug::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	gcPlug.ChangeData();
	CDialogEx::OnOK();
}


void CDLG_Plug::PreInitDialog()
{
	// TODO: 在此添加专用代码和/或调用基类
	gData.InitDLG((LPVOID)this);
	CDialogEx::PreInitDialog();
}


void CDLG_Plug::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (gData.Info.PS.hProcess)
	{
		DLL_HOOK((int)gData.Info.PS.hProcess);
	}
	this->PostMessageW(WM_CLOSE);
}
