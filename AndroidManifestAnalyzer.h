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
	virtual void printResult();	//´òÓ¡½á¹û
	virtual CString getAnalyzerRltDes();
	virtual void closeOpenFile();

private:
	CString mPackageName;
	vector<CString> vClassKeys;
};

#endif // !defined(AFX_ANDROIDMANIFESTANALYZER_H__DA983FC7_F570_4AC5_B8F5_F2813BC20F14__INCLUDED_)
