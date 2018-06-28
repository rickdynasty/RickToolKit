// FileUtils.cpp: implementation of the FileUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RickToolKit.h"
#include "FileUtils.h"
#include "JavaFileAnalyzer.h"
#include "XmlFileAnalyzer.h"
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
	pSpecialFileAnalyzer = NULL;
	pResFileAnalyzer = NULL;

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

	if(NULL != pSpecialFileAnalyzer){
		delete pSpecialFileAnalyzer;
		pSpecialFileAnalyzer = NULL;
	}

	if(NULL != pResFileAnalyzer){
		delete pResFileAnalyzer;
		pResFileAnalyzer = NULL;
	}
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
	
	pFileAnalyzer = new JavaFileAnalyzer();
	pFileAnalyzer->setSuffix(SUFFIX_JAVA);
	pFileAnalyzer->setLogUtils(pLogUtils);

	//xml的解析器，write later		
	pResFileAnalyzer = new XmlFileAnalyzer();
	pResFileAnalyzer->setSuffix(SUFFIX_XML);
	pResFileAnalyzer->setLogUtils(pLogUtils);
	
	pSpecialFileAnalyzer = new AndroidManifestAnalyzer();
	pSpecialFileAnalyzer->setSuffix(SUFFIX_XML);
	pSpecialFileAnalyzer->setLogUtils(pLogUtils);
}

void FileUtils::setClearRedundantFiles(bool needClear)
{ 
	mNeedClearRedundantFiles = needClear;
}

void FileUtils::saveRouteClass(bool saveRouteClass){
	mNeedSaveRouteClass = saveRouteClass;
}

/**
 * 开始扫描类文件
 */
CString FileUtils::analysisLazyClass(CString projectPath, CString additionalProjectPath)
{
	if(projectPath.IsEmpty()){
		return "空文件路径，o(╯□╰)o请先选择有效的工程目录...";
	}
	
	//构建 java文件分析器
	createFileAnalyzer(SUFFIX_JAVA);
	//初始清空
	pFileAnalyzer->clear();
	pSpecialFileAnalyzer->clear();

	//初始过滤器【java 关键字、】
	((JavaFileAnalyzer*)pFileAnalyzer)->initFilter();
	// 以folder作为key,将folder下面的目标文件vector<filepath>储存起来
	((JavaFileAnalyzer*)pFileAnalyzer)->getProDirStructure(projectPath);

	//开始扫描
	scanFolderForSuffix(projectPath, SUFFIX_JAVA);
	if(!additionalProjectPath.IsEmpty()){
		scanFolderForSuffix(additionalProjectPath, SUFFIX_JAVA);
	}

	((JavaFileAnalyzer*)pFileAnalyzer)->receiveRefDatas(((AndroidManifestAnalyzer*)pSpecialFileAnalyzer)->getManifestRefClasses());
	((JavaFileAnalyzer*)pFileAnalyzer)->receiveRefDatas(((XmlFileAnalyzer*)pResFileAnalyzer)->getLayoutRefClasses());
	
	pFileAnalyzer->setClearRedundantFiles(mNeedClearRedundantFiles);
	pFileAnalyzer->printResult();
	pSpecialFileAnalyzer->printResult();
	pResFileAnalyzer->printResult();

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

			if("res" == fileName){
				scanFolderForLayout(filePath);
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

void FileUtils::scanFolderForLayout(CString folder){
	CString fileName,filePath;
	CFileFind fileFind;
	BOOL hasFind = fileFind.FindFile(folder+"\\*.*");
	const CString targetSuffix = SUFFIX_XML;
	while(hasFind)  
	{
		hasFind = fileFind.FindNextFile();  
		if(fileFind.IsDots())
			continue;
		
		filePath = fileFind.GetFilePath();
		fileName = fileFind.GetFileName();

		if(fileFind.IsDirectory()){
			//这里过滤掉不是layout*的文件夹
			if(0 != fileName.Find("layout",0)){
				continue;
			}
			
			scanFolderForLayout(filePath);
			continue;
		}
		
		if(targetSuffix == GetFileSuffix(fileName)){
			//扫描到了目标文件
			//Write code here^
			((XmlFileAnalyzer*)pResFileAnalyzer)->collectRefClasses(filePath);
		} else {
			continue;//不是目标文件，直接略过...
		}
	}
	
	fileFind.Close();
}