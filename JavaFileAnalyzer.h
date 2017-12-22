// JavaFileAnalyzer.h: interface for the JavaFileAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JAVAFILEANALYZER_H__83F03EF7_0CC8_41F8_84E6_EAFD925AC683__INCLUDED_)
#define AFX_JAVAFILEANALYZER_H__83F03EF7_0CC8_41F8_84E6_EAFD925AC683__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)
#pragma warning(disable : 4652)

#include <iostream>
#include <map>
#include "FileAnalyzer.h"
#include "LogUtils.h"

using namespace std;

//javaÎÄ¼þ·ÖÎöÆ÷
class JavaFileAnalyzer : public FileAnalyzer
{
public:
	JavaFileAnalyzer();
	virtual ~JavaFileAnalyzer();
	virtual void setSuffix(CString suffix);
	
private:
	void recycleLogUtils();
public:
    virtual void analyzerFile(const CString file);
	
private:
	LogUtils* pLogUtils;
	map<CString, JavaClass> mAnalyzeRlt;
};

#endif // !defined(AFX_JAVAFILEANALYZER_H__83F03EF7_0CC8_41F8_84E6_EAFD925AC683__INCLUDED_)
