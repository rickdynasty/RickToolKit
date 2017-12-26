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

	//暂时不清楚为啥不能delete
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
	//如果解析器已经存在，且类型已经是对的就不管了
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
	}else if(suffixFlg == SUFFIX_XML){	//xml的解析器，write later
		recycleFileAnalyzer();
	} else{								//其他类型的暂时没处理
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
		return "空文件路径，o(╯□╰)o请先选择有效的工程目录...";
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
	
	//结束的时候是否持有的Log文件句柄
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
			//这里过滤掉 配置&缓存文件夹：.git .gradle 输出等
			if("build" == fileName || "gradle" == fileName || '.' == fileName.GetAt(0) || "Log" == fileName){
				continue;
			}
			
			scanFolderForSuffix(filePath, targetSuffix);
			continue;
		}
		
		if(targetSuffix == GetFileSuffix(fileName)){
			//扫描到了目标文件
			//Write code here^
			pFileAnalyzer->analyzerFile(filePath);
		}else if(FILE_ANDROID_MANIFEST == fileName){
			pSpecialFileAnalyzer->analyzerFile(filePath);
		} else {
			continue;//不是目标文件，直接略过...
		}
	}
	
	fileFind.Close();
}
