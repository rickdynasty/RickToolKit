// XmlFileAnalyzer.h: interface for the XmlFileAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLFILEANALYZER_H__B27EF3AE_D040_4DBE_AE25_906D06016D8A__INCLUDED_)
#define AFX_XMLFILEANALYZER_H__B27EF3AE_D040_4DBE_AE25_906D06016D8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <map>
#include "FileAnalyzer.h"

using namespace std;

class XmlFileAnalyzer : public FileAnalyzer
{
public:
	XmlFileAnalyzer();
	virtual ~XmlFileAnalyzer();
	virtual void setSuffix(CString suffix);
	virtual void clear();
	void setForRes(bool forRes);
	virtual void printResult();	//´òÓ¡½á¹û
	virtual CString getAnalyzerRltDes();
	virtual void closeOpenFile();

};

#endif // !defined(AFX_XMLFILEANALYZER_H__B27EF3AE_D040_4DBE_AE25_906D06016D8A__INCLUDED_)
