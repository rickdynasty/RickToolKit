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

    virtual void analyzerFile(const CString file);
	virtual void clear();
	void setForRes(bool forRes);
	virtual void printResult();	//打印结果
	virtual CString getAnalyzerRltDes();

private:
	void dillClassInheritanceRelationship(CString content, JavaClass& javaClass, vector<CString>& importRefClases);
	void scanReferencedClassVector(JavaClass& javaClass);

public:
	void receiveRefDatas(vector<XML_REF_STRUCT> refDate);
	void getProDirStructure(CString folder);
	void initFilter();

private:
	// 分析结果<类全名，结果结构体>
	map<CString, JavaClass> mAnalyzeRlt;
	//类与文件路径的cache，方便统计同类
	map<CString, CString> mClassCache;
	//《文件夹，文件夹下面的类》
	map<CString, VECTOR> mProStructure;
	// java关键字
	vector<CString> vJavaKeys;

	//白名单类【前缀匹配】
	vector<CString> vClearClassFileExceptPrefix;

	//白名单【类全名】(暂空)
	vector<CString> vImprotClasseWhiteVec;

	bool mForRes;
	CString mRltDes;
};

#endif // !defined(AFX_JAVAFILEANALYZER_H__83F03EF7_0CC8_41F8_84E6_EAFD925AC683__INCLUDED_)
