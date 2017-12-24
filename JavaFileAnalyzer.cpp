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

void JavaFileAnalyzer::closeOpenFile(){
	//������ʱ���Ƿ���е�Log�ļ����
	pLogUtils->closeOpenLogFile();
}

void JavaFileAnalyzer::printResult(){
	if(mAnalyzeRlt.size() < 1)
		return;

	map<CString, JavaClass>::iterator iter;
	//�������ü���
	CString str;
	JavaClass javaClass;
	iter = mAnalyzeRlt.begin();
	while(iter != mAnalyzeRlt.end())
	{
		str = iter->first;
		javaClass = iter->second;
		scanReferencedClassVector(javaClass);
		iter++;
	}
	
	//��ӡ���
	int count = 0;
	iter = mAnalyzeRlt.begin();
	while(iter != mAnalyzeRlt.end())
	{
		count = iter->second.usedCount;
		if(0 < count){
			str.Format("�ࣺ%s �������� %d ��", iter->first, count);
			pLogUtils->d(str);
		} else {
			str.Format("�ࣺ%s �������� %d �� file:%s", iter->first, count, iter->second.filePath);
			pLogUtils->e(str);
		}

		iter++;
	}
}

void JavaFileAnalyzer::scanReferencedClassVector(JavaClass& javaClass){
	CString str = javaClass.packageName + "." + javaClass.className;
	pLogUtils->i(str, FALSE);

	const int count = javaClass.vReferencedClass.size();
	for (int i = 0; i < count; i++)
	{
		str = javaClass.vReferencedClass[i];
		pLogUtils->i(LINE_TABLE+LINE_TABLE+str, FALSE);

		int keyCount = mAnalyzeRlt.count(str);
		if(0 < keyCount){
			mAnalyzeRlt[javaClass.vReferencedClass[i]].usedCount += 1;
		}
	}
	pLogUtils->i(LINE_BREAK, FALSE);
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
	JavaClass* jClass = new JavaClass();
	jClass->init();
	jClass->filePath = file;
	jClass->className = GetFileNameWithoutSuffix(file);	
	jClass->className.TrimLeft();
	jClass->className.TrimRight();

	bool isNote = false;
	bool isPassPackage = false;
	bool isPassClassName = false;
	bool isContent = false;
	const int cPackageKeyLen = JAVA_FILE_PACKAGE_KEY.GetLength();
	const int cImprotKeyLen = JAVA_FILE_IMPROT_KEY.GetLength();
	const int cJavaNoteBeginFlgLen = JAVA_NOTE_FLG_BEGIN.GetLength();	
	const int cJavaNoteEndFlgLen = JAVA_NOTE_FLG_END.GetLength();	
	const int cJavaClassFlgLen = JAVA_FILE_CLASS_KEY.GetLength();
	
	CString readLine,log,prefix,importClass,tmp;
	
	int findPos = -1;
	int startPos = 0;
	int comparePos = -1;
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

				jClass->packageName = readLine.Mid(startPos, findPos - startPos);
				jClass->packageName.TrimLeft();
				jClass->packageName.TrimRight();

				//�����������������ȶ�
				mapKey = jClass->className;
				if(0 < mClassCache.count(mapKey)){
					//�Ѿ��������class���Ƶ���
					log.Format("�ࣺ%s ���ļ���\t%s \t�Ѿ�������[file:\n\t\t\t\t\t\t%s]", mapKey, file, mClassCache[mapKey]);
					pLogUtils->p(log);
				} else {
					mClassCache.insert(pair<CString, CString>(mapKey, jClass->filePath));
				}
			}
			
			continue;			
		}

		//���û�н���import�������ռ�
		//cJavaClassFlgLen
		if(!isPassClassName){
			//���һ�ȡ " class "
			findPos = readLine.Find(JAVA_FILE_CLASS_KEY, 0);

			//���� ��class �����������
			if(-1 == findPos){
				if(0 == readLine.Find(JAVA_FILE_CLASS_KEY_EX,0)){
					findPos = 0;
					startPos = findPos + JAVA_FILE_CLASS_KEY_EX.GetLength();
				}else if(-1 == readLine.Find(JAVA_FILE_IMPROT_KEY,0) && -1 < (findPos = readLine.Find(JAVA_FILE_INTERFACE_KEY, 0))){	//����java��interface�ļ������
					//û��passClass ȴû�ҵ�class��Ҳû��import�����ƾ���interface
					startPos = findPos + JAVA_FILE_INTERFACE_KEY.GetLength();
				}
			}
			else{
				startPos = findPos + cJavaClassFlgLen;
			}


			if(-1 < findPos){
				//if(0 == findPos){
				//	startPos = findPos + JAVA_FILE_CLASS_KEY_EX.GetLength();
				//} else {
				//	startPos = findPos + cJavaClassFlgLen;
				//}

				// Find ��������Ŀո�
				findPos = readLine.Find(SPACE_FLG, startPos);
				//�п�����һ��ģ����
				comparePos = readLine.Find(TEMPLATE_FLG_BEGIN, startPos);
				if(-1 < comparePos){
					if(-1 == findPos){
						findPos = comparePos;
					} else if(comparePos < findPos){
						findPos = comparePos;
					}
				}

				//�п���û�и���ͽӿ�ʵ��ֱ�ӽ�����"{"
				if(-1 == findPos && -1 < (comparePos = readLine.Find(JAVA_CLASS_BODY_BEGIN, startPos))){
					findPos = comparePos;
				}

				if(-1 == findPos){
					//�õ�����
					tmp = readLine.Mid(startPos);
					tmp.TrimLeft();
					tmp.TrimRight();
					if(tmp != jClass->className){
						log.Format("�ļ���%s ���ֵ�����[%s]���ļ�����һ��", file, tmp);
						pLogUtils->e(log);
						continue;
					}
				}
				else{
					if(findPos < startPos){
						//�������û�ҵ�class�ĺ���Ľ�β������ֱ�ӱ�����
						log.Format("�ļ���%s �У�%d ���ݣ�%s ��������ʧ��~�������﷨����...", file, lineCount, readLine);
						pLogUtils->e(log);
						
						readFile.Close();
						return;
					}
					//�õ�����
					tmp = readLine.Mid(startPos, findPos - startPos);
					tmp.TrimLeft();
					tmp.TrimRight();

					if(tmp != jClass->className){
						log.Format("�ļ���%s �״γ��ֵ�����[%s]���ļ�����һ��", file, tmp);
						pLogUtils->e(log);
						continue;
					}
					startPos = findPos + 1;
				}
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
								inheritanceRelationship+= (SPACE_FLG + readLine.Mid(0, findPos));
							}
							break;
						} else {
							inheritanceRelationship+= (SPACE_FLG + readLine);
						}
					}
					dillClassInheritanceRelationship(inheritanceRelationship, *jClass);
				} else {
					dillClassInheritanceRelationship(readLine.Mid(startPos, findPos - startPos + 1), *jClass);
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

				importClass = readLine.Mid(startPos, findPos - startPos);
				jClass->vReferencedClass.push_back(importClass);
			}
		}//if(!isPassClassName)

	}//while(readFile.ReadString(readLine))

	//д�����
	mapKey = jClass->packageName + "." + jClass->className;
	if(0 < mAnalyzeRlt.count(mapKey)){
		//�Ѿ��������class
		log.Format("���ļ���%s mapKey = %s �Ѿ�������[file:%s]", file, mapKey, mAnalyzeRlt[mapKey].filePath);
		pLogUtils->e(log);
	} else {
		mAnalyzeRlt.insert(pair<CString, JavaClass>(mapKey, *jClass));
	}
				
	readFile.Close();
}

//	���ݣ��� extends View implements ViewPager.OnPageChangeListener, OnTabItemCenterPosListener ��
//	�� extends WupBaseResult ��
//	�����ǿյ�
//	const CString JAVA_FILE_EXTENDS_KEY	= "extends ";
//	const CString JAVA_FILE_IMPLEMENTS_KEY	= "implements ";
void JavaFileAnalyzer::dillClassInheritanceRelationship(CString content, JavaClass& javaClass){
	content.TrimLeft();
	content.TrimRight();

	int findPos = -1;
	int startPos = 0;
	if(content.IsEmpty()){
		return;
	}

	//��ȡ����Name
	findPos = content.Find(JAVA_FILE_EXTENDS_KEY, startPos);
	if(-1 < findPos){
		startPos = findPos + JAVA_FILE_EXTENDS_KEY.GetLength();
		findPos = content.Find(SPACE_FLG, startPos);
		if(findPos < startPos){
			findPos = content.GetLength();
		}

		javaClass.parentClassName = content.Mid(startPos, findPos - startPos);
		startPos = findPos -1;
	}

	//��ȡʵ�ֵĽӿ�
	findPos = content.Find(JAVA_FILE_IMPLEMENTS_KEY, startPos);
	if(-1 < findPos){
		startPos = findPos + JAVA_FILE_IMPLEMENTS_KEY.GetLength();
		findPos = content.Find(SPACE_FLG, startPos);
		if(findPos < startPos){
			findPos = content.GetLength();
		}

		const int commaLen = COMMA_FLG.GetLength();
		CString implementsContent = content.Mid(startPos, findPos - startPos);
		implementsContent.Replace(SPACE_FLG, "");
		startPos = 0;
		findPos = implementsContent.Find(COMMA_FLG, startPos);
		while(-1 < findPos){
			javaClass.vImplementsInterfaces.push_back(implementsContent.Mid(startPos, findPos - startPos));
			startPos = findPos + commaLen;
			findPos = implementsContent.Find(COMMA_FLG, startPos);
		}
		javaClass.vImplementsInterfaces.push_back(implementsContent.Mid(startPos));
	}
}
