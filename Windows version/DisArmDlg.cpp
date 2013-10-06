
// DisArmDlg.cpp : implementation file
//

#include "stdafx.h"
#include <string>
#include <sstream>
#include "mydisarm.cpp"
#include "DisArm.h"
#include "DisArmDlg.h"
#include "afxdialogex.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDisArmDlg dialog




CDisArmDlg::CDisArmDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDisArmDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_edit1 = _T("You can drop a binary file here!");
	m_info = _T("Welcome!");
}

void CDisArmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_edit1);
	DDX_Text(pDX, IDC_INFOST, m_info);
}

BEGIN_MESSAGE_MAP(CDisArmDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SELECTBUT, &CDisArmDlg::OnBnClickedSelectbut)
//	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_SAVEBUT, &CDisArmDlg::OnClickedSavebut)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CDisArmDlg message handlers

BOOL CDisArmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDisArmDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDisArmDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDisArmDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDisArmDlg::OnBnClickedSelectbut()
{
	// TODO: Add your control notification handler code here
	CFileDialog* dlg = new CFileDialog(TRUE);
	if (dlg->DoModal() == IDOK) sendDisassembleMessage(dlg->GetPathName());
}


void CDisArmDlg::sendDisassembleMessage(CString binPath) {
		UpdateData(TRUE);
		m_info = _T("Disassembling, please wait...");
		UpdateData(FALSE);
		stringstream outputstream = doDisassemble(binPath);
		CString outText(outputstream.str().c_str());
		UpdateData(TRUE);
		if (outText != "error") {
			m_edit1 = outText;
			m_info = _T("Done!");
			CWnd* SaveAsBut = GetDlgItem(IDC_SAVEBUT);
			SaveAsBut->EnableWindow(TRUE);
		}
		else {
			m_edit1 = _T("");
			m_info = _T("Error!");
			CWnd* SaveAsBut = GetDlgItem(IDC_SAVEBUT);
			SaveAsBut->EnableWindow(FALSE);
		}
		UpdateData(FALSE);
}


void CDisArmDlg::OnClickedSavebut()
{
	// TODO: Add your control notification handler code here
	CFileDialog* dlg = new CFileDialog(FALSE);
	dlg->m_ofn.lpstrFilter = _T("Assembly File\0*.asm\0\0");
	dlg->m_ofn.lpstrDefExt = _T("asm");
	if (dlg->DoModal() == IDOK) {
		UpdateData(TRUE);
		m_info = _T("Saving...");
		UpdateData(FALSE);
		CString buf = dlg->GetPathName();
		CFile newFile;
		newFile.Open(buf, CFile::modeCreate | CFile::modeWrite);
		newFile.Write(m_edit1, m_edit1.GetLength() * 2);
		newFile.Close();
		UpdateData(TRUE);
		m_info = _T("File saved!");
		UpdateData(FALSE);
	}
}


void CDisArmDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnDropFiles(hDropInfo);
	UINT bufSize = DragQueryFile(hDropInfo, 0, NULL, 0) * 2;
	wchar_t* buf = new wchar_t[bufSize];
	DragQueryFile(hDropInfo, 0, buf, bufSize);
	CString filename(buf);
	sendDisassembleMessage(filename);
}
