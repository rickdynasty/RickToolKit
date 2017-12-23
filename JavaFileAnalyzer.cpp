// JavaFileAnalyzer.cpp: implementation of the JavaFileAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RickToolKit.h"
#include "JavaFileAnalyzer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

JavaFileAnalyzer::JavaFileAnalyzer()
{
	mAnalyzeRlt.clear();
	pLogUtils = new LogUtils();
	mForRes = false;
}

JavaFileAnalyzer::~JavaFileAnalyzer()
{
	recycleLogUtils();
}

void JavaFileAnalyzer::recycleLogUtils(){
	if(NULL != pLogUtils){
		delete pLogUtils;
		pLogUtils = NULL;
	}
}

void JavaFileAnalyzer::setForRes(bool forRes){
	mForRes = forRes;
}

void JavaFileAnalyzer::setSuffix(CString suffix){
	mSuffix = suffix;
}

void JavaFileAnalyzer::clear(){
	mAnalyzeRlt.clear();
	mClassCache.clear();
	mRltDes = ANALYSIS_RESULT_DEFAULT_DES;
	mForRes = false;
}

void JavaFileAnalyzer::printResult(){
}

CString JavaFileAnalyzer::getAnalyzerRltDes(){
	//	map<CString, JavaClass> mAnalyzeRlt;
	//  map<CString, CString> mClassCache;
	mRltDes = "";
	if(0 < mAnalyzeRlt.size()){
		mRltDes.Format("һ��%d��java���ļ�", mAnalyzeRlt.size());
	}

	if(mAnalyzeRlt.size() != mClassCache.size()){
		CString tmpDes;
		tmpDes.Format(",�������ȴ�ǣ�%d��", mClassCache.size());
		mRltDes += tmpDes;
	}

	if(0 < mClassCache.size()){
		map<CString, CString>::iterator it = mClassCache.begin();
		while(it != mClassCache.end())
		{
			pLogUtils->d(it->first);
			it ++;         
		}
	}

	return mRltDes;
}

void JavaFileAnalyzer::analyzerFile(const CString file){
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
	
	CString mapKey;
	JavaClass jClass;
	jClass.filePath = file;

	bool isNote = false;
	bool isPassPackage = false;
	bool isPassClassName = false;
	bool isContent = false;
	const int cPackageKeyLen = JAVA_FILE_PACKAGE_KEY.GetLength();
	const int cImprotKeyLen = JAVA_FILE_IMPROT_KEY.GetLength();
	const int cJavaNoteBeginFlgLen = JAVA_NOTE_FLG_BEGIN.GetLength();	
	const int cJavaNoteEndFlgLen = JAVA_NOTE_FLG_END.GetLength();	
	const int cJavaClassFlgLen = JAVA_FILE_CLASS_KEY.GetLength();
	
	CString readLine,log,prefix;
	
	int findPos = -1;
	int startPos = 0;
	int lineCount = 0;
	int strLen = 0;
	while(readFile.ReadString(readLine)) {
		++lineCount;
		readLine.TrimLeft();
		readLine.TrimRight();
		if(readLine.IsEmpty()){
			continue;
		}
		
		//������ ע�͵���� 
		findPos = readLine.Find(JAVA_NOTE_ONELINE_FLG,0);
		if(-1 < findPos){
			if(findPos != 0){
				readLine = readLine.Left(findPos);
			}
			else{
				continue;
			}
		}
		
		//����ע�ͻ�û����
		if(isNote){
			findPos = readLine.Find(JAVA_NOTE_FLG_END, 0);
			//������ע�͵Ľ�������*/��
			if(-1 < findPos){
				isNote = false;
				startPos = findPos + cJavaNoteEndFlgLen;
				if(startPos + 1 < readLine.GetLength()){
					readLine = readLine.Mid(startPos); //���滹�����ݣ���...
					log.Format("�ļ���%s �У�%d �����˶���ע�ͺ���д���ġ����淶д����", file, lineCount);
					pLogUtils->w(log);
				}
			}else{
				continue;
			}
		}
		
		//�������ע�Ϳ�ʼ�����
		findPos = readLine.Find(JAVA_NOTE_FLG_BEGIN, 0);
		//������"/*"
		while(-1 < findPos){
			isNote = true;			
			startPos = findPos + cJavaNoteBeginFlgLen;
			findPos = readLine.Find(JAVA_NOTE_FLG_END, startPos);
			//��ͬһ���ַ�����ע�͵Ľ�������*/��
			if(-1 < findPos){
				isNote = false;
			}else{
				break;
			}
			
			startPos = findPos + cJavaNoteEndFlgLen;
			findPos = readLine.Find(JAVA_NOTE_FLG_BEGIN, startPos);
		}
		
		if(isNote){
			continue;
		}

		//��ʼ����java���ݿ�
		
		//���ж��Ƿ��ȡ����package
		if(!isPassPackage){
			prefix = readLine.Left(cPackageKeyLen);			
			if(prefix == JAVA_FILE_PACKAGE_KEY){
				//��package���
				isPassPackage = true;
				
				startPos = cPackageKeyLen;
				findPos = readLine.Find(PACKAGE_or_IMPROT_EDN_FLG, startPos);
				if(findPos < startPos){
					log.Format("�ļ���%s �У�%d ���ݣ�%s ������package�ؼ��֣�ȴû�ҵ������������淶д����", file, lineCount, readLine);
					pLogUtils->w(log);

					readFile.Close();
					return;
				}
				jClass.packageName = readLine.Mid(startPos, findPos - startPos);
				jClass.className = GetFileNameWithoutSuffix(file);

				mapKey = jClass.packageName + "." + jClass.className;
				if(0 < mAnalyzeRlt.count(mapKey)){
					//�Ѿ��������class
					log.Format("�ļ���%s mapKey = %s �Ѿ�������[file:%s]", file, mapKey, mAnalyzeRlt[mapKey]);
					pLogUtils->e(log);
					
					readFile.Close();
					return;
				}
				else{
					mAnalyzeRlt.insert(pair<CString, JavaClass>(mapKey, jClass));
				}
				
				//�����������������ȶ�
				mapKey = jClass.className;
				if(0<mClassCache.count(mapKey)){
					//�Ѿ��������class���Ƶ���
					log.Format("�ļ���%s className = %s �Ѿ�������[file:%s]", file, jClass.className, mClassCache[mapKey]);
					pLogUtils->w(log);
				}else{
					mClassCache.insert(pair<CString, CString>(mapKey, jClass.filePath));
				}
			}
			
			continue;			
		}

		//���û�н���import�������ռ�
		//cJavaClassFlgLen
		if(!isPassClassName){
			//���һ�ȡ " class "
			findPos = readLine.Find(JAVA_FILE_CLASS_KEY, 0);
			if(-1 < findPos){
				startPos = findPos + cJavaClassFlgLen;
				// Find ��������Ŀո�
				findPos = readLine.Find(SPACE_FLG, startPos);
				if(findPos < startPos){
					//�������û�ҵ�class�ĺ���Ľ�β������ֱ�ӱ�����
					log.Format("�ļ���%s �У�%d ���ݣ�%s ��������ʧ��~�������﷨����...", file, lineCount, readLine);
					pLogUtils->e(log);

					readFile.Close();
					return;
				}
				//�õ�����
				jClass.className = readLine.Mid(startPos, findPos - startPos + 1);
				startPos = findPos + 1;
				//���Բ������ʵ�ֿ�ʼ��{��
				findPos = readLine.Find(JAVA_CLASS_BODY_BEGIN, startPos);
				if(findPos < 0){
					//�ڲ��������¶�ȡֱ���ҵ��ˡ�{��
					CString inheritanceRelationship = readLine.Mid(startPos);
					while(readFile.ReadString(readLine)){
						++lineCount;
						readLine.TrimLeft();
						readLine.TrimRight();
						findPos = readLine.Find(JAVA_CLASS_BODY_BEGIN, 0);
						if(-1 <findPos){
							if(0 < findPos){
								inheritanceRelationship+= readLine.Mid(0, findPos);
							}
							break;
						} else {
							inheritanceRelationship+= readLine;
						}
					}
					dillClassInheritanceRelationship(inheritanceRelationship, jClass);
				} else {
					dillClassInheritanceRelationship(readLine.Mid(startPos, findPos - startPos + 1), jClass);
				}

				isPassClassName = true;
				continue;	//������û�ҵ������Ľ�������{����������Ҫ���������ǰ���ˣ�ֱ��continue ����һ��
			} else {
				//��Ȼû���ҵ� �������Ǿͼ����ռ�improt
				findPos = readLine.Find(JAVA_FILE_IMPROT_KEY, 0);
				if(findPos < 0) continue;

				startPos = findPos + cImprotKeyLen;
				findPos = readLine.Find(PACKAGE_or_IMPROT_EDN_FLG, startPos);
				if(findPos < startPos){
					//�������û�ҵ�class�ĺ���Ľ�β������ֱ�ӱ�����
					log.Format("�ļ���%s �У�%d ���ݣ�%s û���ҵ�import�������������﷨����...", file, lineCount, readLine);
					pLogUtils->e(log);
					
					readFile.Close();
					return;
				}
				jClass.vReferencedClass.push_back(readLine.Mid(startPos, findPos - startPos + 1));
			}
		}//if(!isPassClassName)

	}//while(readFile.ReadString(readLine))
	
	readFile.Close();
}

//���ݣ��� extends View implements ViewPager.OnPageChangeListener, OnTabItemCenterPosListener ��
//�� extends WupBaseResult ��
//�����ǿյ�
void JavaFileAnalyzer::dillClassInheritanceRelationship(const CString content, JavaClass& javaClass){

}