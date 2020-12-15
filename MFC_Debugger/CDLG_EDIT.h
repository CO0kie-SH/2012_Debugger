#pragma once


// CDLG_EDIT 对话框

class CDLG_EDIT : public CDialogEx
{
	DECLARE_DYNAMIC(CDLG_EDIT)

public:
	CDLG_EDIT(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDLG_EDIT();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CEdit CVEdit;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
