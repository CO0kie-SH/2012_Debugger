#pragma once
#include "afxdialogex.h"
#include "resource.h"


// CDLG_Main 对话框

class CDLG_Main : public CDialogEx
{
	DECLARE_DYNAMIC(CDLG_Main)

public:
	CDLG_Main(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDLG_Main();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
