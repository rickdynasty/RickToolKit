// RickToolKitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RickToolKit.h"
#include "RickToolKitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRickToolKitDlg dialog

CRickToolKitDlg::CRickToolKitDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRickToolKitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRickToolKitDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRickToolKitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRickToolKitDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRickToolKitDlg, CDialog)
//{{AFX_MSG_MAP(CRickToolKitDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BTN_ANALYSIS_LAZY_CLASS, OnBtnAnalysisLazyClass)
ON_BN_CLICKED(IDC_BTN_ANALYSIS_LAZY_RES, OnBtnAnalysisLazyRes)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRickToolKitDlg message handlers

BOOL CRickToolKitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	pFileUtils = new FileUtils();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRickToolKitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRickToolKitDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRickToolKitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//开始分析冗余类
void CRickToolKitDlg::OnBtnAnalysisLazyClass() 
{
	if(!IsDirExist(mPath)){
		//MessageBox("请先选择有效的工程文件夹进行分析");
		choosePath();
		return;
	}
	
	if(NULL == pFileUtils){
		pFileUtils = new FileUtils();
	}

	MessageBox(pFileUtils->analysisLazyClass(mPath));
}

void CRickToolKitDlg::OnBtnAnalysisLazyRes() 
{
	if(!IsDirExist(mPath)){
		//MessageBox("请先选择有效的工程文件夹进行分析");
		choosePath();
		return;
	}
}

// 判断文件夹是否存在
BOOL CRickToolKitDlg::IsDirExist(const CString & csDir)
{
    DWORD dwAttrib = GetFileAttributes(csDir);
    return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 != (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}


void CRickToolKitDlg::choosePath(){
    TCHAR path[127];
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = ("选择要分析的工程路径~");
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
    
    if ( pidl != 0 )
    {
        // 获取目录路径
        SHGetPathFromIDList ( pidl, path );
		mPath = path;
		
		//设置为当前路径
        SetCurrentDirectory ( path );
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_PATH);
		pEdit->SetWindowText(mPath);
		
        // 释放内存
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }
	}
}
