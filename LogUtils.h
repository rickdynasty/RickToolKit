// LogUtils.h: interface for the LogUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGUTILS_H__590D3F5F_3EA1_4602_8C7C_937486F48B3F__INCLUDED_)
#define AFX_LOGUTILS_H__590D3F5F_3EA1_4602_8C7C_937486F48B3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LogUtils  
{
public:
	LogUtils(CString folderName = "");
	virtual ~LogUtils();

private:	
	CString GetCurrentTime();

private:
	CString mLogFolder;
	int iErrorCount;
	int iWarnCount;	

private:
	CStdioFile mLogFile;
	CStdioFile mErrorLogFile;
	CStdioFile mWarnLogFile;
	BOOL bLogOpen;
	BOOL bErrorLogOpen;
	BOOL bWarnLogOpen;
	
public:
	void d(CString log);
	void e(CString log);
	void w(CString log);

	int getErrorCount();
	int getWarnCount();
};

#endif // !defined(AFX_LOGUTILS_H__590D3F5F_3EA1_4602_8C7C_937486F48B3F__INCLUDED_)
