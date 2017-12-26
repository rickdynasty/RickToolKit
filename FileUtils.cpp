// FileUtils.cpp: implementation of the FileUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RickToolKit.h"
#include "FileUtils.h"
#include "JavaFileAnalyzer.h"
#include "AndroidManifestAnalyzer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FileUtils::FileUtils()
{
	pFileAnalyzer = NULL;
	pLogUtils = new LogUtils();
	mNeedClearRedundantFiles = false;
}

FileUtils::~FileUtils()
{
	recycleFileAnalyzer();
	recycleLogUtils();
}

void FileUtils::recycleFileAnalyzer(){
	if(NULL != pFileAnalyzer){
		delete pFileAnalyzer;
		pFileAnalyzer = NULL;
	}

	//��ʱ�����Ϊɶ����delete
	//if(NULL != pSpecialFileAnalyzer){
	//	delete pSpecialFileAnalyzer;
	//	pSpecialFileAnalyzer = NULL;
	//}
}

void FileUtils::recycleLogUtils(){
	if(NULL != pLogUtils){
		delete pLogUtils;
		pLogUtils = NULL;
	}
}

void FileUtils::createFileAnalyzer(const CString suffixFlg){
	//����������Ѿ����ڣ��������Ѿ��ǶԵľͲ�����
	if(NULL != pFileAnalyzer && suffixFlg == pFileAnalyzer->getSuffix()){
		return;
	}
	
	recycleFileAnalyzer();
	
	if(suffixFlg == SUFFIX_JAVA){
		pFileAnalyzer = new JavaFileAnalyzer();
		pFileAnalyzer->setSuffix(SUFFIX_JAVA);
		pFileAnalyzer->setLogUtils(pLogUtils);

		pSpecialFileAnalyzer = new AndroidManifestAnalyzer();
		pSpecialFileAnalyzer->setSuffix(SUFFIX_XML);
		pSpecialFileAnalyzer->setLogUtils(pLogUtils);
	}else if(suffixFlg == SUFFIX_XML){	//xml�Ľ�������write later
		recycleFileAnalyzer();
	} else{								//�������͵���ʱû����
		recycleFileAnalyzer();
	}
}

void FileUtils::setClearRedundantFiles(bool needClear)
{ 
	mNeedClearRedundantFiles = needClear;
}

CString FileUtils::analysisLazyClass(CString projectPath, CString additionalProjectPath)
{
	if(projectPath.IsEmpty()){
		return "���ļ�·����o(�s���t)o����ѡ����Ч�Ĺ���Ŀ¼...";
	}
	
	createFileAnalyzer(SUFFIX_JAVA);
	pFileAnalyzer->clear();
	pSpecialFileAnalyzer->clear();

	((JavaFileAnalyzer*)pFileAnalyzer)->getProDirStructure(projectPath);

	scanFolderForSuffix(projectPath, SUFFIX_JAVA);
	if(!additionalProjectPath.IsEmpty()){
		scanFolderForSuffix(additionalProjectPath, SUFFIX_JAVA);
	}

	((JavaFileAnalyzer*)pFileAnalyzer)->receiveAMFData(((AndroidManifestAnalyzer*)pSpecialFileAnalyzer)->getManifestReferenceds());
	
	pFileAnalyzer->setClearRedundantFiles(mNeedClearRedundantFiles);
	pFileAnalyzer->printResult();
	pSpecialFileAnalyzer->printResult();

	CString analyzerRlt = pFileAnalyzer->getAnalyzerRltDes();
	
	//������ʱ���Ƿ���е�Log�ļ����
	pLogUtils->closeOpenLogFile();

	return analyzerRlt;
}

void FileUtils::scanFolderForSuffix(CString folder, const CString targetSuffix){
	//java & xml	
	CString fileName,filePath;
	CFileFind fileFind;
	BOOL hasFind = fileFind.FindFile(folder+"\\*.*");
	while(hasFind)  
	{
		hasFind = fileFind.FindNextFile();  
		if(fileFind.IsDots())
			continue;
		
		filePath = fileFind.GetFilePath();
		fileName = fileFind.GetFileName();

		if(fileFind.IsDirectory()){
			//������˵� ����&�����ļ��У�.git .gradle �����
			if("build" == fileName || "gradle" == fileName || '.' == fileName.GetAt(0) || "Log" == fileName){
				continue;
			}
			
			scanFolderForSuffix(filePath, targetSuffix);
			continue;
		}
		
		if(targetSuffix == GetFileSuffix(fileName)){
			//ɨ�赽��Ŀ���ļ�
			//Write code here^
			pFileAnalyzer->analyzerFile(filePath);
		}else if(FILE_ANDROID_MANIFEST == fileName){
			pSpecialFileAnalyzer->analyzerFile(filePath);
		} else {
			continue;//����Ŀ���ļ���ֱ���Թ�...
		}
	}
	
	fileFind.Close();
}
