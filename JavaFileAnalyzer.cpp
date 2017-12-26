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
	mClearRedundantFiles = false;
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
	
	int count = 0;
	for(int index=0; index < vImprotClasseWhiteVec.size();index++){
		str = vImprotClasseWhiteVec[index];
		count = mAnalyzeRlt.count(str);
		if(0 < count){
			mAnalyzeRlt[str].usedCount += 1;
		}
	}
	
	//打印结果
	iter = mAnalyzeRlt.begin();
	CString className;
	while(iter != mAnalyzeRlt.end())
	{
		className = iter->first;
		count = iter->second.usedCount;
		if(0 < count){
			str.Format("类：%s 被引用了 %d 次", className, count);
			pLogUtils->d(str);
		} else {
			
			if(mClearRedundantFiles){
				if(exceptPrefixInVector(className, vClearClassFileExceptPrefix)){
					//白名单内容，不需要清理
					str.Format("类：%s 被引用了 %d 次 file:%s |白名单类，不清理", className, count, iter->second.filePath);
					pLogUtils->e(str);
				} else{
					str.Format("类：%s 被引用了 %d 次 file:%s |直接清理了", className, count, iter->second.filePath);
					pLogUtils->e(str);
					//冗余文件，直接delete
					::DeleteFile(iter->second.filePath);
				}
			}
			else{
				str.Format("类：%s 被引用了 %d 次 file:%s |直接清理", className, count, iter->second.filePath);
				pLogUtils->e(str);
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
	vClearClassFileExceptPrefix.push_back("com.tencent.tws.assistant.");
	
	vImprotClasseWhiteVec.clear();
	vClearClassFileExceptPrefix.push_back("com.tws.plugin.aidl.PaceInfo");
	vClearClassFileExceptPrefix.push_back("com.tencent.tws.phoneside.cover.UriToPath");
	//vClearClassFileExceptPrefix.push_back("TRom.E_ROM_TOKEN_TYPE");
	//vClearClassFileExceptPrefix.push_back("OPT.E_QUBE_LANG");
	//vClearClassFileExceptPrefix.push_back("com.tencent.tws.watchside.pay.wechat.rpc.IRPC"); //这个是模板类，暂时没做解析
	//vClearClassFileExceptPrefix.push_back("com.airbnb.lottie.BaseAnimatableValue"); //这个是模板类，暂时没做解析
	
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

	//debug
	if("AlarmSettingActivity" == jClass->className){
		int i = 0;
		i += 2;
		i = 4;
	}
	
	bool isNote = false;
	bool isPassPackage = false;
	bool isPassClassName = false;
	bool isContent = false;
	bool isLonelyPkg = false;
	const int cPackageKeyLen = JAVA_FILE_PACKAGE_KEY.GetLength();
	const int cImprotKeyLen = JAVA_FILE_IMPROT_KEY.GetLength();
	const int cJavaNoteBeginFlgLen = JAVA_NOTE_FLG_BEGIN.GetLength();	
	const int cJavaNoteEndFlgLen = JAVA_NOTE_FLG_END.GetLength();	
	const int cJavaClassFlgLen = JAVA_FILE_CLASS_KEY.GetLength();
	
	CString readLine,log,prefix,importClass,tmp;
	
	//加入improt类
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
	bool nextIsMayFullRef = false;
	int lineLen = 0;

	//对R的layout引用 REF_RESOURCE_FLG
	bool isRefRes = false;
	
	int findPos = -1;
	int startPos = 0;
	int comparePos = -1;
	int lineCount = 0;
	int strLen = 0;
	while(readFile.ReadString(readLine)) {
		++lineCount;
		//debug
		if(23 == lineCount){
			int i = 0;
			i += 2;
			i = 4;
		}

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
				if(startPos + 1 < lineLen){
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
		
		//注意 lineLen每次只在这里进行赋值
		lineLen = readLine.GetLength();
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
				isLonelyPkg = (-1 == jClass->packageName.Find(DOT_FLG, 0));
				
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
					dillClassInheritanceRelationship(inheritanceRelationship, *jClass, importRefClases);
				} else {
					dillClassInheritanceRelationship(readLine.Mid(startPos, findPos - startPos + 1), *jClass, importRefClases);
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
				
				if(-1 < readLine.Find(REF_RESOURCE_FLG, 0)){
					isRefRes = true;
				}else{ //暂时都标志为true
					isRefRes = true;
				}
				importClass = readLine.Mid(startPos, findPos - startPos);
				importClass.Replace(SPACE_FLG, "");//清除里面的空格

				jClass->vReferencedClass.push_back(importClass);
				tmp = GetClassName(importClass);
				importRefClases.push_back(tmp);
				
				//没有找到类名 就不往后继续，直接读取下一行
				continue;
			}
		}//if(!isPassClassName)
		
		// debug
		if(-1 < readLine.Find("setContentView(",0)){
			int idsfa = 0;
			++idsfa;
			idsfa += 2;
		}
		//先收集对资源Layout的引用,然后在遍历查找类引用
		if(isRefRes){
			findPos = readLine.Find(REF_RES_LAYOUT_FLG,0);
			if(-1 < findPos){ //存在了对layout的引用,结束符一般只会是[; , )]3中
				startPos = findPos + REF_RES_LAYOUT_FLG.GetLength();
				int endPos = lineLen;
				findPos = readLine.Find(COMMA_FLG,startPos);
				if(-1 < findPos && findPos < endPos){
					endPos = findPos;
				}
				
				comparePos = readLine.Find(SEMICOLON_FLG,startPos);
				if(-1 < comparePos && comparePos < endPos){
					endPos = comparePos;
				}
				
				int parenthesesPos = readLine.Find(")",startPos);
				if(-1 < parenthesesPos && parenthesesPos < endPos){
					endPos = parenthesesPos;
				}
				
				tmp = readLine.Mid(startPos, endPos - startPos);
				if(!dataIsExistInVector(tmp, jClass->vRefLayoutRes)){
					jClass->vRefLayoutRes.push_back(tmp);
				}
			}
		}

		//收集类实现中的引用
		////////////////////////////////////// 遍历readLine 获取内部引用到的类 //////////////////////////////////////////
		//collectReferencedClass(readLine, *jClass);
		isStrStart = false;
		discarded = false;
		isBracket = false;
		isRefInner = false;
		isJavaKey = false;
		isExistInVec = false;
		isImportClass = false;
		nextIsMayFullRef = false;
		startPos = 0;
		comparePos = 0;
		findPos = -1;
		for(int index=0; index < lineLen; index++){
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
				comparePos = -1;	//用于全package的引用开始位置
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
				comparePos = -1;	//用于全package的引用开始位置
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
				if(isFullPackageRef){	//如果之前记录了可能是全pakcage引用，就不重新记录startPos
					//这里不考虑是否是discarded的情况
					if(nextIsMayFullRef){ //上次记录的，其实就是当前的						
						importClass = readLine.Mid(comparePos, findPos - comparePos);
						isExistInVec = dataIsExistInVector(importClass, innerImportRefClases);
						if(!isExistInVec){
							innerImportRefClases.push_back(importClass);
						}					
					}//if(nextIsMayFullRef)
					
					
					
					//对下一个字符提前判断一下，如果为大写，可能后面的就会是一个全package类引用
					if(index+1 < lineLen){
						ch  = readLine.GetAt(index + 1);
						if('A' <= ch && ch <= 'Z'){
							//对类的命名 基本都是大写开始的
							nextIsMayFullRef = true;
						} else {
							nextIsMayFullRef = false;
						}
					} else {
						nextIsMayFullRef = false;
					}	
					
					continue;
				}
				
				if(!discarded){
					mayKey = readLine.Mid(startPos, findPos - startPos);
					isImportClass = dataIsExistInVector(mayKey, importRefClases);
					isJavaKey = dataIsExistInVector(mayKey, vJavaKeys);
					isExistInVec = dataIsExistInVector(mayKey, jClass->vReferencedClassEx);
					discarded = true; //“.”后面的内容都不需要单独录入
					if(mayKey == jClass->className || isJavaKey || isImportClass){// || isExistInVec 这里不需要监测是否在容器里面，类的多级的，如果在开始就被过来了，子级别的很多都可能被pass了
						isFullPackageRef = false;
						continue;
					}
					
					if(!isExistInVec){
						jClass->vReferencedClassEx.push_back(mayKey);
					}
				}
				
				if(!isFullPackageRef){
					//保留上次的位置
					comparePos = startPos;
					isFullPackageRef = true;	//这里记录一下，在下次匹配大写对的时候，如果不符合就给纠正过来
				}
				
				//对下一个字符提前判断一下，如果为大写，可能后面的就会是一个全package类引用
				if(index+1 < lineLen){
					ch  = readLine.GetAt(index + 1);
					if('A' <= ch && ch <= 'Z'){
						//对类的命名 基本都是大写开始的
						nextIsMayFullRef = true;
					} else {
						nextIsMayFullRef = false;
					}
				} else {
					nextIsMayFullRef = false;
				}
				
				continue; //“.”没必要记录位置了
			} else { // 字符判断 if('.' == ch)
				isRefInner = false;
				if(' ' == ch || '(' == ch || ')' == ch || '{' == ch || '}' == ch || '=' == ch || ';' == ch || ',' == ch || '&' == ch || '|' == ch || '<' == ch || '>' == ch || ':' == ch || '!' == ch || '<' == ch || '>' == ch){
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
					
					//如果标识之前的内容是全路径的，这里需要做一下处理
					if(isFullPackageRef && nextIsMayFullRef && -1 < comparePos && comparePos + 2 < index)
					{
						importClass = readLine.Mid(comparePos, findPos - comparePos);
						isExistInVec = dataIsExistInVector(importClass, innerImportRefClases);
						if(!isExistInVec){
							innerImportRefClases.push_back(importClass);
						}
					}
					comparePos = -1;	//用于全package的引用开始位置
					isFullPackageRef = false;
				}//if(' ' == ch || '(' == ch || ')' == ch || '{' == ch || '}' == ch

			} //else { // 字符判断 if('.' == ch)
			
			startPos = findPos + 1;
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
		//将内部引用添加到import里面用作计算引用计数的
		for(int index=0; index < jClass->vReferencedClassEx.size(); index++){
			tmp = jClass->vReferencedClassEx[index];
			if(isLonelyPkg){
				if(jClass->packageName == tmp){
					;//啥也不做...
				} else {
					importClass = jClass->packageName+"."+jClass->vReferencedClassEx[index];
					jClass->vReferencedClass.push_back(importClass);
				}
			} else {
				importClass = jClass->packageName+"."+jClass->vReferencedClassEx[index];
				jClass->vReferencedClass.push_back(importClass);
			}
		}
		
		//处理内部全package引用的集合
		for(index=0; index < innerImportRefClases.size(); index++){
			importClass = innerImportRefClases[index];
			jClass->vReferencedClass.push_back(importClass);
		}
		
		mAnalyzeRlt.insert(pair<CString, JavaClass>(mapKey, *jClass));
	}
				
	readFile.Close();
}

//	内容：“ extends View implements ViewPager.OnPageChangeListener, OnTabItemCenterPosListener ”
//	“ extends WupBaseResult ”
//	或者是空的
//	const CString JAVA_FILE_EXTENDS_KEY	= "extends ";
//	const CString JAVA_FILE_IMPLEMENTS_KEY	= "implements ";
void JavaFileAnalyzer::dillClassInheritanceRelationship(CString content, JavaClass& javaClass, vector<CString>& importRefClases){
	content.TrimLeft();
	content.TrimRight();
	
	int findPos = -1;
	int startPos = 0;
	int tmplatePos = -1;
	if(content.IsEmpty() || content.GetLength() < 8){ //关键字的长度都不止8
		return;
	}
	
	bool isImportClass = false;
	//获取父类Name
	findPos = content.Find(JAVA_FILE_EXTENDS_KEY, startPos);
	if(-1 < findPos){
		startPos = findPos + JAVA_FILE_EXTENDS_KEY.GetLength();
		findPos = content.Find(SPACE_FLG, startPos);
		int contentLen = content.GetLength();
		
		if(findPos < startPos){
			findPos = content.GetLength();
		} else{			
			if(findPos - startPos < 2){
				for(int index = findPos; index < contentLen; index++){
					if(' ' != content.GetAt(index)){
						startPos = index;
						break;
					}
				}
				findPos = content.Find(SPACE_FLG, startPos);
			}
		}
		
		javaClass.parentClassName = content.Mid(startPos, findPos - startPos);
		contentLen = javaClass.parentClassName.GetLength();
		if('{' == javaClass.parentClassName.GetAt(contentLen-1)){
			javaClass.parentClassName = javaClass.parentClassName.Left(contentLen-1);
			contentLen = javaClass.parentClassName.GetLength();
		}

		if(-1 < (tmplatePos = javaClass.parentClassName.Find(TEMPLATE_FLG_BEGIN, 0))){
			javaClass.parentClassName = javaClass.parentClassName.Left(tmplatePos);
		}
		
		//如果不是全package引用 需要校验是否在improt里面
		if(javaClass.parentClassName.Find(DOT_FLG, 0) < 0){
			isImportClass = dataIsExistInVector(javaClass.parentClassName, importRefClases);
			if(!isImportClass){
				javaClass.vReferencedClass.push_back(javaClass.packageName+DOT_FLG+javaClass.parentClassName);
				importRefClases.push_back(javaClass.parentClassName);
			}
		}
		
		startPos = findPos -1;
	}
	
	//获取实现的接口
	findPos = content.Find(JAVA_FILE_IMPLEMENTS_KEY, startPos);
	if(-1 < findPos){
		CString implementsStr;
		const int cImplementsKeyLen = JAVA_FILE_IMPLEMENTS_KEY.GetLength();
		startPos = findPos + cImplementsKeyLen;
		//从关键字开始截取，所有内容都保留下来
		CString implementsContent = content.Mid(startPos); //implements的全部内容
		implementsContent.Replace(SPACE_FLG, "");//清除里面的空格
		int contentLen = implementsContent.GetLength();
		if('{' == implementsContent.GetAt(contentLen-1)){
			implementsContent = implementsContent.Left(contentLen-1);
			contentLen = implementsContent.GetLength();
		}

		//开始通过“,”开分割多实现接口
		startPos = 0;
		const int commaLen = COMMA_FLG.GetLength();
		findPos = implementsContent.Find(COMMA_FLG, startPos);
		while(-1 < findPos){
			implementsStr = implementsContent.Mid(startPos, findPos - startPos);
			if(-1 < (tmplatePos = implementsStr.Find(TEMPLATE_FLG_BEGIN, 0))){
				implementsStr = implementsStr.Left(tmplatePos);
			}
			
			//如果不是全package引用 需要校验是否在improt里面
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
		}//while(-1 < findPos)
		implementsStr = implementsContent.Mid(startPos, implementsContent.GetLength() - startPos);
		if(-1 < (tmplatePos = implementsStr.Find(TEMPLATE_FLG_BEGIN, 0))){
			implementsStr = implementsStr.Left(tmplatePos);
		}
		//如果不是全package引用 需要校验是否在improt里面
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

void JavaFileAnalyzer::receiveAMFData(vector<XML_REF_STRUCT> refDate){
	CString str;
	for(int i = 0; i < refDate.size(); i++){
		for(int index=0; index < refDate[i].vReferencedClass.size();index++)
		{
			str = refDate[i].vReferencedClass[index];
			
			int keyCount = mAnalyzeRlt.count(str);
			if(0 < keyCount){
				mAnalyzeRlt[str].usedCount += 1;
			}
		}
	}
}
