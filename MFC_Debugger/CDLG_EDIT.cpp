// CDLG_EDIT.cpp: 实现文件
//

#include "pch.h"
#include "MFC_Debugger.h"
#include "CDLG_EDIT.h"
#include "afxdialogex.h"


// CDLG_EDIT 对话框

IMPLEMENT_DYNAMIC(CDLG_EDIT, CDialogEx)

CDLG_EDIT::CDLG_EDIT(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{
	
}

CDLG_EDIT::~CDLG_EDIT()
{
}

void CDLG_EDIT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, CVEdit);
}


BEGIN_MESSAGE_MAP(CDLG_EDIT, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDLG_EDIT::OnBnClickedButton1)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CDLG_EDIT 消息处理程序


void CDLG_EDIT::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CVEdit.GetWindowTextW(jstr);
	this->PostMessageW(WM_CLOSE);
}


int CDLG_EDIT::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	this->SetWindowTextW(jstr);
	return 0;
}
