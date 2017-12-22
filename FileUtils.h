// FileUtils.h: interface for the FileUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEUTILS_H__3E84A386_804E_4423_A9A0_C5064ED6AC17__INCLUDED_)
#define AFX_FILEUTILS_H__3E84A386_804E_4423_A9A0_C5064ED6AC17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FileAnalyzer.h"

class FileUtils  
{
public:
	FileUtils();
	virtual ~FileUtils();

private:
	void scanFolderForSuffix(CString folder, const CString targetSuffix);
	void createFileAnalyzer(const CString suffixFlg);
	void recycleFileAnalyzer();

public:
	CString analysisLazyClass(CString projectPath, CString additionalProjectPath="");

private:
	FileAnalyzer* pFileAnalyzer;
};

#endif // !defined(AFX_FILEUTILS_H__3E84A386_804E_4423_A9A0_C5064ED6AC17__INCLUDED_)
