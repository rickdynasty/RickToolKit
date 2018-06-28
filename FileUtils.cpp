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
	//����������Ѿ����ڣ��������Ѿ��ǶԵľͲ�����
	if(NULL != pFileAnalyzer && suffixFlg == pFileAnalyzer->getSuffix()){
		return;
	}
	
	recycleFileAnalyzer();
	
	pFileAnalyzer = new JavaFileAnalyzer();
	pFileAnalyzer->setSuffix(SUFFIX_JAVA);
	pFileAnalyzer->setLogUtils(pLogUtils);

	//xml�Ľ�������write later		
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
 * ��ʼɨ�����ļ�
 */
CString FileUtils::analysisLazyClass(CString projectPath, CString additionalProjectPath)
{
	if(projectPath.IsEmpty()){
		return "���ļ�·����o(�s���t)o����ѡ����Ч�Ĺ���Ŀ¼...";
	}
	
	//���� java�ļ�������
	createFileAnalyzer(SUFFIX_JAVA);
	//��ʼ���
	pFileAnalyzer->clear();
	pSpecialFileAnalyzer->clear();

	//��ʼ��������java �ؼ��֡���
	((JavaFileAnalyzer*)pFileAnalyzer)->initFilter();
	// ��folder��Ϊkey,��folder�����Ŀ���ļ�vector<filepath>��������
	((JavaFileAnalyzer*)pFileAnalyzer)->getProDirStructure(projectPath);

	//��ʼɨ��
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

			if("res" == fileName){
				scanFolderForLayout(filePath);
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
			//������˵�����layout*���ļ���
			if(0 != fileName.Find("layout",0)){
				continue;
			}
			
			scanFolderForLayout(filePath);
			continue;
		}
		
		if(targetSuffix == GetFileSuffix(fileName)){
			//ɨ�赽��Ŀ���ļ�
			//Write code here^
			((XmlFileAnalyzer*)pResFileAnalyzer)->collectRefClasses(filePath);
		} else {
			continue;//����Ŀ���ļ���ֱ���Թ�...
		}
	}
	
	fileFind.Close();
}