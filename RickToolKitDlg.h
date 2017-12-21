// RickToolKitDlg.h : header file
//

#if !defined(AFX_RICKTOOLKITDLG_H__0BA9EF3A_2185_46DE_B11E_337CC6107012__INCLUDED_)
#define AFX_RICKTOOLKITDLG_H__0BA9EF3A_2185_46DE_B11E_337CC6107012__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRickToolKitDlg dialog

class CRickToolKitDlg : public CDialog
{
// Construction
public:
	CRickToolKitDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRickToolKitDlg)
	enum { IDD = IDD_RICKTOOLKIT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRickToolKitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	
private:
	BOOL IsDirExist(const CString & csDir);
	void choosePath();

protected:
	HICON m_hIcon;
	CString mPath;

	// Generated message map functions
	//{{AFX_MSG(CRickToolKitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnAnalysis();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICKTOOLKITDLG_H__0BA9EF3A_2185_46DE_B11E_337CC6107012__INCLUDED_)
