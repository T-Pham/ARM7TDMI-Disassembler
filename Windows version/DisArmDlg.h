
// DisArmDlg.h : header file
//

#pragma once


// CDisArmDlg dialog
class CDisArmDlg : public CDialogEx
{
// Construction
public:
	CDisArmDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DISARM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void sendDisassembleMessage(CString);
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedSelectbut();
	CString m_edit1;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnClickedSavebut();
	CString m_info;
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
