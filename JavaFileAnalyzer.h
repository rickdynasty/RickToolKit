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

//java�ļ�������
class JavaFileAnalyzer : public FileAnalyzer
{
public:
	JavaFileAnalyzer();
	virtual ~JavaFileAnalyzer();

    virtual void analyzerFile(const CString file);
	virtual void clear();
	void setForRes(bool forRes);
	virtual void printResult();	//��ӡ���
	virtual CString getAnalyzerRltDes();

private:
	void dillClassInheritanceRelationship(CString content, JavaClass& javaClass, vector<CString>& importRefClases);
	void scanReferencedClassVector(JavaClass& javaClass);

public:
	void receiveRefDatas(vector<XML_REF_STRUCT> refDate);
	void getProDirStructure(CString folder);
	void initFilter();

private:
	// �������<��ȫ��������ṹ��>
	map<CString, JavaClass> mAnalyzeRlt;
	//�����ļ�·����cache������ͳ��ͬ��
	map<CString, CString> mClassCache;
	//���ļ��У��ļ���������ࡷ
	map<CString, VECTOR> mProStructure;
	// java�ؼ���
	vector<CString> vJavaKeys;

	//�������ࡾǰ׺ƥ�䡿
	vector<CString> vClearClassFileExceptPrefix;

	//����������ȫ����(�ݿ�)
	vector<CString> vImprotClasseWhiteVec;

	bool mForRes;
	CString mRltDes;
};

#endif // !defined(AFX_JAVAFILEANALYZER_H__83F03EF7_0CC8_41F8_84E6_EAFD925AC683__INCLUDED_)
