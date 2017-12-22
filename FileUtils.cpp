// FileUtils.cpp: implementation of the FileUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RickToolKit.h"
#include "FileUtils.h"
#include "JavaFileAnalyzer.h"

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
}

FileUtils::~FileUtils()
{
	recycleFileAnalyzer();
}

void FileUtils::recycleFileAnalyzer(){
	if(NULL != pFileAnalyzer){
		delete pFileAnalyzer;
		pFileAnalyzer = NULL;
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
	}else if(suffixFlg == SUFFIX_XML){	//xml�Ľ�������write later
		recycleFileAnalyzer();
	} else{								//�������͵���ʱû����
		recycleFileAnalyzer();
	}
}

CString FileUtils::analysisLazyClass(CString projectPath, CString additionalProjectPath)
{
	createFileAnalyzer(SUFFIX_JAVA);
	scanFolderForSuffix(projectPath, SUFFIX_JAVA);
	if(NULL != additionalProjectPath && !additionalProjectPath.IsEmpty()){
		scanFolderForSuffix(additionalProjectPath, SUFFIX_JAVA);
	}

	return "finish";
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
			if("build" == fileName || "gradle" == fileName || '.' == fileName.GetAt(0)){
				continue;
			}
			
			scanFolderForSuffix(filePath, targetSuffix);
			continue;
		}
		
		if(targetSuffix == GetFileSuffix(fileName)){
			//ɨ�赽��Ŀ���ļ�
			//Write code here^
			pFileAnalyzer->analyzerFile(fileName);
		}else{
			continue;//����Ŀ���ļ���ֱ���Թ�...
		}
	}
	
	fileFind.Close();
}
