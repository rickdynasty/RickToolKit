// FileUtils.h: interface for the FileUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEUTILS_H__3E84A386_804E_4423_A9A0_C5064ED6AC17__INCLUDED_)
#define AFX_FILEUTILS_H__3E84A386_804E_4423_A9A0_C5064ED6AC17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FileAnalyzer.h"
#include "LogUtils.h"

class FileUtils  
{
public:
	FileUtils();
	virtual ~FileUtils();

private:
	void scanFolderForSuffix(CString folder, const CString targetSuffix);
	void scanFolderForLayout(CString folder);
	void createFileAnalyzer(const CString suffixFlg);
	void recycleFileAnalyzer();
	void recycleLogUtils();

public:
	CString analysisLazyClass(CString projectPath, CString additionalProjectPath="");
	void setClearRedundantFiles(bool needClear);
	void saveRouteClass(bool saveRouteClass);
private:
	LogUtils* pLogUtils;
	FileAnalyzer* pFileAnalyzer;
	FileAnalyzer* pSpecialFileAnalyzer;
	FileAnalyzer* pResFileAnalyzer;

private:
	bool mNeedClearRedundantFiles;
	bool mNeedSaveRouteClass;
};

#endif // !defined(AFX_FILEUTILS_H__3E84A386_804E_4423_A9A0_C5064ED6AC17__INCLUDED_)
