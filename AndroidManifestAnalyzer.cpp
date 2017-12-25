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

void AndroidManifestAnalyzer::printResult()	//��ӡ���
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
		//���������͵��ļ�
		pLogUtils->e("�ļ���"+file+" ������java���ļ�����ȷ������...");
		return;
	}
	
	CStdioFile readFile;
	if(FALSE == readFile.Open(file, CFile::modeRead)){
		//��ȡʧ��
		pLogUtils->e("Open�ļ���"+file+" ʧ��");
		return;
	}
	
	// Write Code Here^
	
	readFile.Close();
}

