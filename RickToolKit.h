// RickToolKit.h : main header file for the RICKTOOLKIT application
//

#if !defined(AFX_RICKTOOLKIT_H__0B37833D_B180_4A89_9898_A4A0970AE681__INCLUDED_)
#define AFX_RICKTOOLKIT_H__0B37833D_B180_4A89_9898_A4A0970AE681__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRickToolKitApp:
// See RickToolKit.cpp for the implementation of this class
//

class CRickToolKitApp : public CWinApp
{
public:
	CRickToolKitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRickToolKitApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRickToolKitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICKTOOLKIT_H__0B37833D_B180_4A89_9898_A4A0970AE681__INCLUDED_)
