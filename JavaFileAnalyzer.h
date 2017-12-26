// JavaFileAnalyzer.h: interface for the JavaFileAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JAVAFILEANALYZER_H__83F03EF7_0CC8_41F8_84E6_EAFD925AC683__INCLUDED_)
#define AFX_JAVAFILEANALYZER_H__83F03EF7_0CC8_41F8_84E6_EAFD925AC683__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <map>
#include "FileAnalyzer.h"

using namespace std;

//java文件分析器
class JavaFileAnalyzer : public FileAnalyzer
{
public:
	JavaFileAnalyzer();
	virtual ~JavaFileAnalyzer();
	virtual void setSuffix(CString suffix);
	virtual void clear();
	void setForRes(bool forRes);
	virtual void printResult();	//打印结果
	virtual CString getAnalyzerRltDes();
	virtual void closeOpenFile();
	void getProDirStructure(CString folder);

private:
	void recycleLogUtils();
	void dillClassInheritanceRelationship(CString content, JavaClass& javaClass, vector<CString>& importRefClases);
	void scanReferencedClassVector(JavaClass& javaClass);

public:
    virtual void analyzerFile(const CString file);
	void receiveAMFData(vector<AMF_STRUCT> amfDate);

private:
	map<CString, JavaClass> mAnalyzeRlt;
	map<CString, CString> mClassCache;
	map<CString, VECTOR> mProStructure;
	vector<CString> vJavaKeys;

	bool mForRes;
	CString mRltDes;
	AMF_STRUCT *pAMFData;
};

#endif // !defined(AFX_JAVAFILEANALYZER_H__83F03EF7_0CC8_41F8_84E6_EAFD925AC683__INCLUDED_)
