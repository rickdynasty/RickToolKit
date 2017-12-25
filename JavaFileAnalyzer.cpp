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
			pLogUtils->d("目录："+str);
			for(int index=0; index < vec.values.size();index++){
				pLogUtils->d(LINE_TABLE+LINE_TABLE+"文件："+vec.values[index]);
			}
			it++;
		}
	}


	if(mAnalyzeRlt.size() < 1)
		return;

	map<CString, JavaClass>::iterator iter;
	//处理引用计数
	JavaClass javaClass;
	iter = mAnalyzeRlt.begin();
	while(iter != mAnalyzeRlt.end())
	{
		str = iter->first;
		javaClass = iter->second;
		scanReferencedClassVector(javaClass);
		iter++;
	}
	
	//打印结果
	int count = 0;
	iter = mAnalyzeRlt.begin();
	while(iter != mAnalyzeRlt.end())
	{
		count = iter->second.usedCount;
		if(0 < count){
			str.Format("类：%s 被引用了 %d 次", iter->first, count);
			pLogUtils->d(str);
		} else {
			str.Format("类：%s 被引用了 %d 次 file:%s", iter->first, count, iter->second.filePath);
			pLogUtils->e(str);
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
		pLogUtils->i(LINE_TABLE+LINE_TABLE+LINE_TABLE+"[内部应用类]："+str);
	}
	pLogUtils->i(LINE_BREAK);
}

CString JavaFileAnalyzer::getAnalyzerRltDes(){
	//	map<CString, JavaClass> mAnalyzeRlt;
	//  map<CString, CString> mClassCache;
	mRltDes = "";
	if(0 < mAnalyzeRlt.size()){
		mRltDes.Format("一共%d个java类文件", mAnalyzeRlt.size());
	}

	if(mAnalyzeRlt.size() != mClassCache.size()){
		CString tmpDes;
		tmpDes.Format(",类的数量却是：%d个", mClassCache.size());
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
			//这里过滤掉 配置&缓存文件夹：.git .gradle 输出等
			if("build" == fileName || "gradle" == fileName || '.' == fileName.GetAt(0) || "Log" == fileName || "assets" == fileName || "libs" == fileName || "res" == fileName){
				continue;
			}
			
			getProDirStructure(filePath);
			continue;
		}
		
		if(mSuffix == GetFileSuffix(fileName)){
			//扫描到了目标文件
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

	//提前加入同级目录下面的其他类
	vector<CString> mayRefClases;
	mayRefClases.clear();
	char ch;
	bool isStrStart = false;
	bool isRefInner = false;
	bool discarded = false;
	bool isBracket = false;
	CString mayKey;
	int singleBraceCount = 0;
	
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
		
		//处理单行 注释的情况 
		findPos = readLine.Find(JAVA_NOTE_ONELINE_FLG,0);
		if(-1 < findPos){
			if(findPos != 0){
				readLine = readLine.Left(findPos);
			}
			else{
				continue;
			}
		}
		
		//多行注释还没结束
		if(isNote){
			findPos = readLine.Find(JAVA_NOTE_FLG_END, 0);
			//发现了注释的结束符“*/”
			if(-1 < findPos){
				isNote = false;
				startPos = findPos + cJavaNoteEndFlgLen;
				if(startPos + 1 < readLine.GetLength()){
					readLine = readLine.Mid(startPos); //后面还有内容，囧...
					log.Format("文件：%s 行：%d 出现了多行注释后跟有代码的【不规范写法】", file, lineCount);
					pLogUtils->w(log);
				}
			}else{
				continue;
			}
		}
		
		//处理多行注释开始的情况
		findPos = readLine.Find(JAVA_NOTE_FLG_BEGIN, 0);
		//发现了"/*"
		while(-1 < findPos){
			isNote = true;			
			startPos = findPos + cJavaNoteBeginFlgLen;
			findPos = readLine.Find(JAVA_NOTE_FLG_END, startPos);
			//在同一行又发现了注释的结束符“*/”
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

		//开始处理java内容块
		
		//先判断是否获取到了package
		if(!isPassPackage){
			prefix = readLine.Left(cPackageKeyLen);			
			if(prefix == JAVA_FILE_PACKAGE_KEY){
				//是package语句
				isPassPackage = true;
				
				startPos = cPackageKeyLen;
				findPos = readLine.Find(PACKAGE_or_IMPROT_EDN_FLG, startPos);
				if(findPos < startPos){
					log.Format("文件：%s 行：%d 内容：%s 出现了package关键字，却没找到结束符【不规范写法】", file, lineCount, readLine);
					pLogUtils->w(log);

					readFile.Close();
					return;
				}

				jClass->packageName = readLine.Mid(startPos, findPos - startPos);
				jClass->packageName.TrimLeft();
				jClass->packageName.TrimRight();

				//将类名缓存起来做比对
				mapKey = jClass->className;
				if(0 < mClassCache.count(mapKey)){
					//已经存在这个class名称的类
					log.Format("类：%s 在文件：\t%s \t已经存在于[file:\n\t\t\t\t\t\t%s]", mapKey, file, mClassCache[mapKey]);
					pLogUtils->p(log);
				} else {
					mClassCache.insert(pair<CString, CString>(mapKey, jClass->filePath));
				}
			}
			
			continue;			
		}

		//如果没有结束import，就先收集
		//cJavaClassFlgLen
		if(!isPassClassName){
			//查找获取 " class "
			findPos = readLine.Find(JAVA_FILE_CLASS_KEY, 0);

			//处理 “class 类名”的情况
			if(-1 == findPos){
				if(0 == readLine.Find(JAVA_FILE_CLASS_KEY_EX,0)){
					findPos = 0;
					startPos = findPos + JAVA_FILE_CLASS_KEY_EX.GetLength();
				}else if(-1 == readLine.Find(JAVA_FILE_IMPROT_KEY,0) && -1 < (findPos = readLine.Find(JAVA_FILE_INTERFACE_KEY, 0))){	//处理java中interface文件的情况
					//没有passClass 却没找到class，也没有import，估计就是interface
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

				// Find 类名后面的空格
				findPos = readLine.Find(SPACE_FLG, startPos);
				//有可能是一个模板类
				comparePos = readLine.Find(TEMPLATE_FLG_BEGIN, startPos);
				if(-1 < comparePos){
					if(-1 == findPos){
						findPos = comparePos;
					} else if(comparePos < findPos){
						findPos = comparePos;
					}
				}

				//有可能没有父类和接口实现直接接上了"{"
				if(-1 == findPos && -1 < (comparePos = readLine.Find(JAVA_CLASS_BODY_BEGIN, startPos))){
					findPos = comparePos;
				}

				if(-1 == findPos){
					//得到类名
					tmp = readLine.Mid(startPos);
					tmp.TrimLeft();
					tmp.TrimRight();
					if(tmp != jClass->className){
						log.Format("文件：%s 出现的类名[%s]和文件名不一致", file, tmp);
						pLogUtils->e(log);
						continue;
					}
				}
				else{
					if(findPos < startPos){
						//如果这里没找到class的后面的结尾符，就直接报错返回
						log.Format("文件：%s 行：%d 内容：%s 解析类名失败~，请检测语法规则...", file, lineCount, readLine);
						pLogUtils->e(log);
						
						readFile.Close();
						return;
					}
					//得到类名
					tmp = readLine.Mid(startPos, findPos - startPos);
					tmp.TrimLeft();
					tmp.TrimRight();

					if(tmp != jClass->className){
						log.Format("文件：%s 首次出现的类名[%s]和文件名不一致", file, tmp);
						pLogUtils->e(log);
						continue;
					}
					startPos = findPos + 1;
				}
				//尝试查找类的实现开始“{”
				findPos = readLine.Find(JAVA_CLASS_BODY_BEGIN, startPos);
				if(findPos < 0){
					//内部继续往下读取直到找到了“{”
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
				continue;	//不管找没找到类名的结束符“{”，都不需要继续解读当前行了，直接continue 走下一行
			} else {
				//既然没有找到 类名，那就继续收集improt
				findPos = readLine.Find(JAVA_FILE_IMPROT_KEY, 0);
				if(findPos < 0) continue;

				startPos = findPos + cImprotKeyLen;
				findPos = readLine.Find(PACKAGE_or_IMPROT_EDN_FLG, startPos);
				if(findPos < startPos){
					//如果这里没找到class的后面的结尾符，就直接报错返回
					log.Format("文件：%s 行：%d 内容：%s 没发找到import结束符，请检查语法规则...", file, lineCount, readLine);
					pLogUtils->e(log);
					
					readFile.Close();
					return;
				}

				importClass = readLine.Mid(startPos, findPos - startPos);
				jClass->vReferencedClass.push_back(importClass);
				tmp = GetClassName(importClass);
				mayRefClases.push_back(tmp);
				
				//没有找到类名 就不往后继续，直接读取下一行
				continue;
			}
		}//if(!isPassClassName)

		/*
		// debug
		if(-1 < readLine.Find("throw new IllegalArgumentException(\"Unknown URI",0)){
			int idsfa = 0;
			++idsfa;
			idsfa += 2;
		}
		*/
		//收集类实现中的引用
		////////////////////////////////////// 遍历readLine 获取内部引用到的类 //////////////////////////////////////////
		//collectReferencedClass(readLine, *jClass);
		isStrStart = false;
		discarded = false;
		isBracket = false;
		isRefInner = false;
		startPos = 0;
		findPos = -1;
		for(int index=0; index < readLine.GetLength(); index++){
			ch  = readLine.GetAt(index);
			
			if(startPos == index){
				if('A' <= ch && ch <= 'Z'){
					//对类的命名 基本都是大写开始的
					discarded = false;
				}else{
					discarded = true;
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
				findPos = index;
				if(!discarded && !isRefInner){
					mayKey = readLine.Mid(startPos, findPos - startPos);
					if(1 < mayKey.GetLength() && mayKey != jClass->className && !dataIsExistInVector(mayKey, vJavaKeys) && !dataIsExistInVector(mayKey, mayRefClases) && !dataIsExistInVector(mayKey, jClass->vReferencedClassEx)){
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
				findPos = index;
				if(!discarded && !isRefInner){
					mayKey = readLine.Mid(startPos, findPos - startPos);
					if(1 < mayKey.GetLength() && mayKey != jClass->className && !dataIsExistInVector(mayKey, vJavaKeys) && !dataIsExistInVector(mayKey, mayRefClases) && !dataIsExistInVector(mayKey, jClass->vReferencedClassEx)){
						jClass->vReferencedClassEx.push_back(mayKey);
					}
				}
				startPos = findPos + 1;

				continue;
			}

			if(' ' == ch || '(' == ch || ')' == ch || '.' == ch || '{' == ch || '}' == ch || '=' == ch || ';' == ch || ',' == ch || '&' == ch || '|' == ch || '<' == ch || '>' == ch){
				findPos = index;
				if(!discarded && !isRefInner){
					mayKey = readLine.Mid(startPos, findPos - startPos);
					if(1 < mayKey.GetLength() && mayKey != jClass->className && !dataIsExistInVector(mayKey, vJavaKeys) && !dataIsExistInVector(mayKey, mayRefClases) && !dataIsExistInVector(mayKey, jClass->vReferencedClassEx)){
						jClass->vReferencedClassEx.push_back(mayKey);
					}
				}
				startPos = findPos + 1;

				if('.' == ch){
					isRefInner = true;
				}
				else{
					isRefInner = false;
				}
			}
		}//for(int index=0; index < content.GetLength(); index++)
		////////////////////////////////////// 遍历readLine 获取内部引用到的类 end //////////////////////////////////////////

	}//while(readFile.ReadString(readLine))

	//写到最后
	mapKey = jClass->packageName + "." + jClass->className;
	if(0 < mAnalyzeRlt.count(mapKey)){
		//已经存在这个class
		log.Format("类文件：%s mapKey = %s 已经存在于[file:%s]", file, mapKey, mAnalyzeRlt[mapKey].filePath);
		pLogUtils->e(log);
	} else {
		mAnalyzeRlt.insert(pair<CString, JavaClass>(mapKey, *jClass));
	}
				
	readFile.Close();
}

//	内容：“ extends View implements ViewPager.OnPageChangeListener, OnTabItemCenterPosListener ”
//	“ extends WupBaseResult ”
//	或者是空的
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

	//获取父类Name
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

	//获取实现的接口
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
