// CDLG_Plug.cpp: 实现文件
//

#include "pch.h"
#include "CDLG_Plug.h"


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
END_MESSAGE_MAP()


// CDLG_Plug 消息处理程序
