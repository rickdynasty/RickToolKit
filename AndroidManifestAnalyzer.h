// AndroidManifestAnalyzer.h: interface for the AndroidManifestAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANDROIDMANIFESTANALYZER_H__DA983FC7_F570_4AC5_B8F5_F2813BC20F14__INCLUDED_)
#define AFX_ANDROIDMANIFESTANALYZER_H__DA983FC7_F570_4AC5_B8F5_F2813BC20F14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include "FileAnalyzer.h"

using namespace std;

class AndroidManifestAnalyzer : public FileAnalyzer
{
public:
	AndroidManifestAnalyzer();
	virtual ~AndroidManifestAnalyzer();

	virtual void analyzerFile(const CString file);
	virtual void setSuffix(CString suffix);
	virtual void clear();
	void setForRes(bool forRes);
	virtual void printResult();	//��ӡ���
	virtual CString getAnalyzerRltDes();
	virtual void closeOpenFile();

private:
	vector<AMF_STRUCT> mManifestReferenceds;
	void push_back(CString ref, AMF_STRUCT& value);

public:
	const vector<AMF_STRUCT> getManifestReferenceds();
};

#endif // !defined(AFX_ANDROIDMANIFESTANALYZER_H__DA983FC7_F570_4AC5_B8F5_F2813BC20F14__INCLUDED_)
