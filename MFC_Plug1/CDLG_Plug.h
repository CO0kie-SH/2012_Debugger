#pragma once
#include "afxdialogex.h"
#include "CData.h"


// CDLG_Plug 对话框

class CDLG_Plug : public CDialogEx
{
	DECLARE_DYNAMIC(CDLG_Plug)

public:
	CDLG_Plug(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDLG_Plug();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
