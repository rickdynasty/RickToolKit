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
	clear();
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
	mProStructure.clear();
	vJavaKeys.clear();
	mRltDes = ANALYSIS_RESULT_DEFAULT_DES;
	mForRes = false;
	pAMFData = NULL;
	mClearRedundantFiles = false;
}

void JavaFileAnalyzer::closeOpenFile(){

}

void JavaFileAnalyzer::printResult(){
	//map<CString, VECTOR> mProStructure;
	CString str;
	VECTOR vec;
	map<CString, VECTOR>::iterator it;
	if(0 < mProStructure.size()){
		it = mProStructure.begin();
		while(it != mProStructure.end())
		{
			str = it->first;
			vec = it->second;
			pLogUtils->d("Ŀ¼��"+str);
			for(int index=0; index < vec.values.size();index++){
				pLogUtils->d(LINE_TABLE+LINE_TABLE+"�ļ���"+vec.values[index]);
			}
			it++;
		}
	}

	if(mAnalyzeRlt.size() < 1)
		return;

	map<CString, JavaClass>::iterator iter;
	//�������ü���
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
	CString className;
	while(iter != mAnalyzeRlt.end())
	{
		className = iter->first;
		count = iter->second.usedCount;
		if(0 < count){
			str.Format("�ࣺ%s �������� %d ��", className, count);
			pLogUtils->d(str);
		} else {
			str.Format("�ࣺ%s �������� %d �� file:%s", className, count, iter->second.filePath);
			pLogUtils->e(str);
			
			if(mClearRedundantFiles){
				if(exceptPrefixInVector(className, vClearClassFileExceptPrefix)){
					//���������ݣ�����Ҫ����
				} else{
					//�����ļ���ֱ��delete
					//Write Code here ^
				}
			}
		}

		iter++;
	}
}

void JavaFileAnalyzer::scanReferencedClassVector(JavaClass& javaClass){
	CString str = javaClass.packageName + "." + javaClass.className;
	pLogUtils->i(str);

	int count = javaClass.vReferencedClass.size();
	for (int i = 0; i < count; i++)
	{
		str = javaClass.vReferencedClass[i];
		pLogUtils->i(LINE_TABLE+LINE_TABLE+str);

		int keyCount = mAnalyzeRlt.count(str);
		if(0 < keyCount){
			mAnalyzeRlt[str].usedCount += 1;
		}
	}

	count = javaClass.vReferencedClassEx.size();
	for ( i = 0; i < count; i++)
	{
		str = javaClass.vReferencedClassEx[i];
		pLogUtils->i(LINE_TABLE+LINE_TABLE+LINE_TABLE+"[�ڲ�Ӧ����]��"+str);
	}
	pLogUtils->i(LINE_BREAK);
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

void JavaFileAnalyzer::getProDirStructure(CString folder){
	vJavaKeys.clear();
	vJavaKeys.push_back("String");
	vJavaKeys.push_back("Object");
	vJavaKeys.push_back("TAG");
	vJavaKeys.push_back("Class");
	vJavaKeys.push_back("ClassLoader");
	vJavaKeys.push_back("Boolean");
	vJavaKeys.push_back("Throwable");	
	vJavaKeys.push_back("IllegalArgumentException");
	vJavaKeys.push_back("CloneNotSupportedException");
	vJavaKeys.push_back("InterruptedException");
	vJavaKeys.push_back("IOException");
	vJavaKeys.push_back("IllegalAccessException");
	vJavaKeys.push_back("RemoteException");
	vJavaKeys.push_back("NoSuchAlgorithmException");
	vJavaKeys.push_back("NoSuchMethodException");
	vJavaKeys.push_back("ClassNotFoundException");
	vJavaKeys.push_back("Exception");
	vJavaKeys.push_back("RuntimeException");
	vJavaKeys.push_back("Runnable");
	vJavaKeys.push_back("IllegalStateException");
	vJavaKeys.push_back("NumberFormatException");
	vJavaKeys.push_back("InstantiationException");

	vClearClassFileExceptPrefix.clear();
	vClearClassFileExceptPrefix.push_back("com.tencent.tws.sharelib.");
	vClearClassFileExceptPrefix.push_back("qrom.component.config.");

	CString fileName,filePath;
	CFileFind fileFind;
	BOOL hasFind = fileFind.FindFile(folder+"\\*.*");

	VECTOR vContent;
	vContent.values.clear();
	const CString key = folder;
	while(hasFind)  
	{
		hasFind = fileFind.FindNextFile();  
		if(fileFind.IsDots())
			continue;
		
		filePath = fileFind.GetFilePath();
		fileName = fileFind.GetFileName();

		if(fileFind.IsDirectory()){
			//������˵� ����&�����ļ��У�.git .gradle �����
			if("build" == fileName || "gradle" == fileName || '.' == fileName.GetAt(0) || "Log" == fileName || "assets" == fileName || "libs" == fileName || "res" == fileName){
				continue;
			}
			
			getProDirStructure(filePath);
			continue;
		}
		
		if(mSuffix == GetFileSuffix(fileName)){
			//ɨ�赽��Ŀ���ļ�
			fileName = GetFileNameWithoutSuffixEx(fileName);
			vContent.values.push_back(fileName);
		}
	}
	
	if(0 < vContent.values.size()){
		mProStructure.insert(pair<CString, VECTOR>(key, vContent));
	}

	fileFind.Close();
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

	//����improt��
	vector<CString> importRefClases;
	vector<CString> innerImportRefClases;
	importRefClases.clear();
	innerImportRefClases.clear();
	char ch;
	bool isStrStart = false;
	bool isRefInner = false;
	bool discarded = false;
	bool isBracket = false;
	CString mayKey;
	int singleBraceCount = 0;
	bool isFullPackageRef = false;
	bool isJavaKey = false;
	bool isExistInVec = false;
	bool isImportClass = false;
	
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
					dillClassInheritanceRelationship(inheritanceRelationship, *jClass, importRefClases);
				} else {
					dillClassInheritanceRelationship(readLine.Mid(startPos, findPos - startPos + 1), *jClass, importRefClases);
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
				tmp = GetClassName(importClass);
				importRefClases.push_back(tmp);
				
				//û���ҵ����� �Ͳ����������ֱ�Ӷ�ȡ��һ��
				continue;
			}
		}//if(!isPassClassName)

		
		// debug
		if(-1 < readLine.Find("public TRom.BrushParam stBrushParam = null;",0)){
			int idsfa = 0;
			++idsfa;
			idsfa += 2;
		}
		//�ռ���ʵ���е�����
		////////////////////////////////////// ����readLine ��ȡ�ڲ����õ����� //////////////////////////////////////////
		//collectReferencedClass(readLine, *jClass);
		isStrStart = false;
		discarded = false;
		isBracket = false;
		isRefInner = false;
		isJavaKey = false;
		isExistInVec = false;
		isImportClass = false;
		startPos = 0;
		comparePos = 0;
		findPos = -1;
		for(int index=0; index < readLine.GetLength(); index++){
			ch  = readLine.GetAt(index);
			
			if(startPos == index){
				if('A' <= ch && ch <= 'Z'){
					//��������� �������Ǵ�д��ʼ��
					discarded = false;
				}else{
					discarded = true;
					isFullPackageRef = false;//�Ǵ�д������Ϊ������ȫpackage����
				}
			}
			
			if(isStrStart){
				if('"' == ch){
					isStrStart = false;
					startPos = index + 1;
				}
					
				continue;
			}
			
			if('"' == ch){
				isStrStart = true;
				comparePos = -1;	//����ȫpackage�����ÿ�ʼλ��
				isFullPackageRef = false;
				findPos = index;
				if(!discarded && !isRefInner){
					mayKey = readLine.Mid(startPos, findPos - startPos);
					isExistInVec = dataIsExistInVector(mayKey, jClass->vReferencedClassEx);
					isJavaKey = dataIsExistInVector(mayKey, vJavaKeys);
					isImportClass = dataIsExistInVector(mayKey, importRefClases);
					if(1 < mayKey.GetLength() && mayKey != jClass->className && !isJavaKey && !isImportClass && !isExistInVec){
						jClass->vReferencedClassEx.push_back(mayKey);
					}
				}
				startPos = index + 1;

				continue;
			}

			if(isBracket){
				if(']' == ch){
					isBracket = false;
					startPos = index + 1;
				}
				
				continue;
			}

			if('[' == ch){
				isBracket = true;
				comparePos = -1;	//����ȫpackage�����ÿ�ʼλ��
				isFullPackageRef = false;
				findPos = index;
				if(!discarded && !isRefInner){
					mayKey = readLine.Mid(startPos, findPos - startPos);
					isExistInVec = dataIsExistInVector(mayKey, jClass->vReferencedClassEx);
					isJavaKey = dataIsExistInVector(mayKey, vJavaKeys);
					isImportClass = dataIsExistInVector(mayKey, importRefClases);
					if(1 < mayKey.GetLength() && mayKey != jClass->className && !isJavaKey && !isImportClass && !isExistInVec){
						jClass->vReferencedClassEx.push_back(mayKey);
					}
				}
				startPos = findPos + 1;

				continue;
			}

			if('.' == ch){
				findPos = index;
				isRefInner = true;
				if(isFullPackageRef){	//���֮ǰ��¼�˿�����ȫpakcage���ã��Ͳ����¼�¼startPos
					continue;
				}
				
				if(!discarded){
					mayKey = readLine.Mid(startPos, findPos - startPos);
					isImportClass = dataIsExistInVector(mayKey, importRefClases);
					isJavaKey = dataIsExistInVector(mayKey, vJavaKeys);
					isExistInVec = dataIsExistInVector(mayKey, jClass->vReferencedClassEx);
					discarded = true; //��.����������ݶ�����Ҫ����¼��
					if(mayKey == jClass->className || isJavaKey || isImportClass || isExistInVec){
						isFullPackageRef = false;
						continue;
					}
					
					jClass->vReferencedClassEx.push_back(mayKey);
					if(!isFullPackageRef){
						//�����ϴε�λ��
						comparePos = startPos;
						isFullPackageRef = true;	//�����¼һ�£����´�ƥ���д�Ե�ʱ����������Ͼ͸���������
					}
				}
				continue; //��.��û��Ҫ��¼λ����
			} else {
				isRefInner = false;
				if(' ' == ch || '(' == ch || ')' == ch || '{' == ch || '}' == ch || '=' == ch || ';' == ch || ',' == ch || '&' == ch || '|' == ch || '<' == ch || '>' == ch || ':' == ch){
					findPos = index;
					if(!discarded){						
						mayKey = readLine.Mid(startPos, findPos - startPos);
						isExistInVec = dataIsExistInVector(mayKey, jClass->vReferencedClassEx);
						isJavaKey = dataIsExistInVector(mayKey, vJavaKeys);
						isImportClass = dataIsExistInVector(mayKey, importRefClases);
						if(1 < mayKey.GetLength() && mayKey != jClass->className && !isJavaKey && !isImportClass && !isExistInVec){
							jClass->vReferencedClassEx.push_back(mayKey);
						}
					}
					
					//�����ʶ֮ǰ��������ȫ·���ģ�������Ҫ��һ�´���
					if(isFullPackageRef && -1 < comparePos && comparePos + 2 < index)
					{
						importClass = readLine.Mid(comparePos, findPos - comparePos);
						isExistInVec = dataIsExistInVector(importClass, innerImportRefClases);
						if(!isExistInVec){
							innerImportRefClases.push_back(importClass);
						}
					}
					comparePos = -1;	//����ȫpackage�����ÿ�ʼλ��
					isFullPackageRef = false;
				}//if(' ' == ch || '(' == ch || ')' == ch || '{' == ch || '}' == ch
			}
			
			startPos = findPos + 1;
		}//for(int index=0; index < content.GetLength(); index++)
		////////////////////////////////////// ����readLine ��ȡ�ڲ����õ����� end //////////////////////////////////////////

	}//while(readFile.ReadString(readLine))

	//д�����
	mapKey = jClass->packageName + "." + jClass->className;
	if(0 < mAnalyzeRlt.count(mapKey)){
		//�Ѿ��������class
		log.Format("���ļ���%s mapKey = %s �Ѿ�������[file:%s]", file, mapKey, mAnalyzeRlt[mapKey].filePath);
		pLogUtils->e(log);
	} else {
		//���ڲ�������ӵ�import���������������ü�����
		for(int index=0; index < jClass->vReferencedClassEx.size(); index++){
			importClass = jClass->packageName+"."+jClass->vReferencedClassEx[index];
			jClass->vReferencedClass.push_back(importClass);
		}

		for(index=0; index < innerImportRefClases.size(); index++){
			importClass = innerImportRefClases[index];
			jClass->vReferencedClass.push_back(importClass);
		}

		mAnalyzeRlt.insert(pair<CString, JavaClass>(mapKey, *jClass));
	}
				
	readFile.Close();
}

//	���ݣ��� extends View implements ViewPager.OnPageChangeListener, OnTabItemCenterPosListener ��
//	�� extends WupBaseResult ��
//	�����ǿյ�
//	const CString JAVA_FILE_EXTENDS_KEY	= "extends ";
//	const CString JAVA_FILE_IMPLEMENTS_KEY	= "implements ";
void JavaFileAnalyzer::dillClassInheritanceRelationship(CString content, JavaClass& javaClass, vector<CString>& importRefClases){
	content.TrimLeft();
	content.TrimRight();

	int findPos = -1;
	int startPos = 0;
	if(content.IsEmpty() || content.GetLength() < 8){ //�ؼ��ֵĳ��ȶ���ֹ8
		return;
	}

	bool isImportClass = false;
	//��ȡ����Name
	findPos = content.Find(JAVA_FILE_EXTENDS_KEY, startPos);
	if(-1 < findPos){
		startPos = findPos + JAVA_FILE_EXTENDS_KEY.GetLength();
		findPos = content.Find(SPACE_FLG, startPos);
		if(findPos < startPos){
			findPos = content.GetLength();
		}

		javaClass.parentClassName = content.Mid(startPos, findPos - startPos);
		//�������ȫpackage���� ��ҪУ���Ƿ���improt����
		if(javaClass.parentClassName.Find(DOT_FLG, 0) < 0){
			isImportClass = dataIsExistInVector(javaClass.parentClassName, importRefClases);
			if(!isImportClass){
				javaClass.vReferencedClass.push_back(javaClass.packageName+DOT_FLG+javaClass.parentClassName);
				importRefClases.push_back(javaClass.parentClassName);
			}
		}

		startPos = findPos -1;
	}

	//��ȡʵ�ֵĽӿ�
	findPos = content.Find(JAVA_FILE_IMPLEMENTS_KEY, startPos);
	if(-1 < findPos){
		CString implementsStr;
		const int cImplementsKeyLen = JAVA_FILE_IMPLEMENTS_KEY.GetLength();
		startPos = findPos + cImplementsKeyLen;
		findPos = content.Find(SPACE_FLG, startPos);
		if(findPos < startPos){
			findPos = content.GetLength();
		}
		//const CString COMMA_FLG				= ",";
		const int commaLen = COMMA_FLG.GetLength();
		CString implementsContent = content.Mid(startPos, findPos - startPos); //implements��ȫ������
		implementsContent.Replace(SPACE_FLG, "");//�������Ŀո�
		//��ʼͨ����,�����ָ��ʵ�ֽӿ�
		startPos = 0;
		findPos = implementsContent.Find(COMMA_FLG, startPos);
		while(-1 < findPos){
			implementsStr = implementsContent.Mid(startPos, findPos - startPos);
			//�������ȫpackage���� ��ҪУ���Ƿ���improt����
			if(implementsStr.Find(DOT_FLG, 0) < 0){
				isImportClass = dataIsExistInVector(implementsStr, importRefClases);
				if(!isImportClass){
					javaClass.vReferencedClass.push_back(javaClass.packageName + DOT_FLG + implementsStr);
					importRefClases.push_back(implementsStr);
				}
			}
			javaClass.vImplementsInterfaces.push_back(implementsStr);
			startPos = findPos + commaLen;
			findPos = implementsContent.Find(COMMA_FLG, startPos);
		}
		implementsStr = implementsContent.Mid(startPos, implementsContent.GetLength() - startPos);
		//�������ȫpackage���� ��ҪУ���Ƿ���improt����
		if(implementsStr.Find(DOT_FLG, 0) < 0){
			isImportClass = dataIsExistInVector(implementsStr, importRefClases);
			if(!isImportClass){
				javaClass.vReferencedClass.push_back(javaClass.packageName + DOT_FLG + implementsStr);
				importRefClases.push_back(implementsStr);
			}
		}
		javaClass.vImplementsInterfaces.push_back(implementsStr);
	}
}

void JavaFileAnalyzer::receiveAMFData(vector<AMF_STRUCT> amfDate){
	CString str;
	for(int i = 0; i < amfDate.size(); i++){
		for(int index=0; index < amfDate[i].vReferencedClass.size();index++)
		{
			str = amfDate[i].vReferencedClass[index];
			
			int keyCount = mAnalyzeRlt.count(str);
			if(0 < keyCount){
				mAnalyzeRlt[str].usedCount += 1;
			}
		}
	}
}
