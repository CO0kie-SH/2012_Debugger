// CDLG_Main.cpp: 实现文件
//

#include "pch.h"
#include "CDLG_Main.h"
#include "afxdialogex.h"


// CDLG_Main 对话框

IMPLEMENT_DYNAMIC(CDLG_Main, CDialogEx)

CDLG_Main::CDLG_Main(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CDLG_Main::~CDLG_Main()
{
}

void CDLG_Main::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLG_Main, CDialogEx)
END_MESSAGE_MAP()


// CDLG_Main 消息处理程序
