// AndroidManifestAnalyzer.cpp: implementation of the AndroidManifestAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RickToolKit.h"
#include "AndroidManifestAnalyzer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AndroidManifestAnalyzer::AndroidManifestAnalyzer()
{
	
}

AndroidManifestAnalyzer::~AndroidManifestAnalyzer()
{
	
}

void AndroidManifestAnalyzer::setSuffix(CString suffix)
{
	mSuffix = suffix;
}

void AndroidManifestAnalyzer::clear()
{
	
}

void AndroidManifestAnalyzer::setForRes(bool forRes)
{
	
}

void AndroidManifestAnalyzer::printResult()	//打印结果
{
	
}

CString AndroidManifestAnalyzer::getAnalyzerRltDes()
{
	return ANALYSIS_RESULT_DEFAULT_DES;
}

void AndroidManifestAnalyzer::closeOpenFile()
{
	
}

void AndroidManifestAnalyzer::analyzerFile(const CString file){
	if(0 != mSuffix.CompareNoCase(GetFileSuffix(file)))
	{
		//不符合类型的文件
		pLogUtils->e("文件："+file+" 并不是java类文件，请确认输入...");
		return;
	}
	
	CStdioFile readFile;
	if(FALSE == readFile.Open(file, CFile::modeRead)){
		//读取失败
		pLogUtils->e("Open文件："+file+" 失败");
		return;
	}
	
	// Write Code Here^
	
	readFile.Close();
}

