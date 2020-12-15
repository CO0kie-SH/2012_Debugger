
// MFC_DebuggerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFC_Debugger.h"
#include "MFC_DebuggerDlg.h"
#include "afxdialogex.h"
#include "CManage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCDebuggerDlg 对话框



CMFCDebuggerDlg::CMFCDebuggerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_DEBUGGER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCDebuggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCDebuggerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND_RANGE(ID_32771, ID_32789, &CMFCDebuggerDlg::OnMenuClick)
	ON_COMMAND_RANGE(5000, 5999, &CMFCDebuggerDlg::OnMenuClick)
	ON_WM_TIMER()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABM1, &CMFCDebuggerDlg::OnTcnSelchangeTabm1)
//	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &CMFCDebuggerDlg::OnHdnItemdblclickListm1)
ON_NOTIFY(NM_RCLICK, IDC_LISTM1, &CMFCDebuggerDlg::OnNMRClickListm1)
END_MESSAGE_MAP()


// CMFCDebuggerDlg 消息处理程序

BOOL CMFCDebuggerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CTabCtrl* CTAB = (CTabCtrl*)GetDlgItem(IDC_TABM1);
	CTAB->InsertItem(0, L"反汇编");
	CTAB->InsertItem(1, L"日志");
	CTAB->InsertItem(2, L"备注");
	CTAB->InsertItem(3, L"断点");
	CTAB->InsertItem(4, L"内存映射");

	if (gcManage.InitManage(this))
		SetTimer(1, 1, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCDebuggerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCDebuggerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCDebuggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCDebuggerDlg::OnMenuClick(UINT_PTR nID)
{
	gcManage.MenuClick(nID);
}


void CMFCDebuggerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	static BOOL isFirst = true;
	if (isFirst)
	{
		SetTimer(1, 999, NULL);
		isFirst = false;
		gcView->InitView();
	}
	else
	{
		gcView->SetTime();
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CMFCDebuggerDlg::OnTcnSelchangeTabm1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	if (pNMHDR->idFrom == IDC_TABM1)	//表示TAB控件
	{
		CTabCtrl* cTab = (CTabCtrl*)this->GetDlgItem(IDC_TABM1);
		gcManage.TabClick(cTab->GetCurSel() + 1);
	}
	*pResult = 0;
}


//void CMFCDebuggerDlg::OnHdnItemdblclickListm1(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
//	// TODO: 在此添加控件通知处理程序代码
//	*pResult = 0;
//}


void CMFCDebuggerDlg::OnNMRClickListm1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pNMItemActivate->iItem != -1)
	{
		gcManage.LSM1RClick(pNMItemActivate);
	}
	*pResult = 0;
}
