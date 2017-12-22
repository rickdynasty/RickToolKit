// FileAnalyzer.h: interface for the FileAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEANALYZER_H__91661F81_F3AF_4935_9567_BA937E188B06__INCLUDED_)
#define AFX_FILEANALYZER_H__91661F81_F3AF_4935_9567_BA937E188B06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class FileAnalyzer  
{
public:
	FileAnalyzer();
	virtual ~FileAnalyzer();
	
protected:
	CString mSuffix;
	
public:
    virtual void analyzerFile(const CString file) = 0;//´¿Ðéº¯Êý
	virtual void setSuffix(CString suffix) = 0;//´¿Ðéº¯Êý
	virtual CString getSuffix(){ return mSuffix;}
};

#endif // !defined(AFX_FILEANALYZER_H__91661F81_F3AF_4935_9567_BA937E188B06__INCLUDED_)
