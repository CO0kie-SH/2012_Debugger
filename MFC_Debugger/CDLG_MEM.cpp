// CDLG_MEM.cpp: 实现文件
//

#include "pch.h"
#include "MFC_Debugger.h"
#include "CDLG_MEM.h"
#include "afxdialogex.h"


// CDLG_MEM 对话框

IMPLEMENT_DYNAMIC(CDLG_MEM, CDialogEx)

CDLG_MEM::CDLG_MEM(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CDLG_MEM::~CDLG_MEM()
{
}

void CDLG_MEM::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLG_MEM, CDialogEx)
END_MESSAGE_MAP()


// CDLG_MEM 消息处理程序


BOOL CDLG_MEM::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CTabCtrl* CTAB_MEM = (CTabCtrl*)GetDlgItem(IDC_TAB1);
	CTAB_MEM->InsertItem(0, L"内存1");
	CTAB_MEM->InsertItem(1, L"内存2");
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
