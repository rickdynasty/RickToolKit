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
				jClass.packageName = readLine.Mid(startPos, findPos - startPos);
				jClass.className = GetFileNameWithoutSuffix(file);

				mapKey = jClass.packageName + "." + jClass.className;
				if(0 < mAnalyzeRlt.count(mapKey)){
					//已经存在这个class
					log.Format("文件：%s mapKey = %s 已经存在于[file:%s]", file, mapKey, mAnalyzeRlt[mapKey]);
					pLogUtils->e(log);
					
					readFile.Close();
					return;
				}
				else{
					mAnalyzeRlt.insert(pair<CString, JavaClass>(mapKey, jClass));
				}
				
				//将类名缓存起来做比对
				mapKey = jClass.className;
				if(0<mClassCache.count(mapKey)){
					//已经存在这个class名称的类
					log.Format("文件：%s className = %s 已经存在于[file:%s]", file, jClass.className, mClassCache[mapKey]);
					pLogUtils->w(log);
				}else{
					mClassCache.insert(pair<CString, CString>(mapKey, jClass.filePath));
				}
			}
			
			continue;			
		}

		//如果没有结束import，就先收集
		//cJavaClassFlgLen
		if(!isPassClassName){
			//查找获取 " class "
			findPos = readLine.Find(JAVA_FILE_CLASS_KEY, 0);
			if(-1 < findPos){
				startPos = findPos + cJavaClassFlgLen;
				// Find 类名后面的空格
				findPos = readLine.Find(SPACE_FLG, startPos);
				if(findPos < startPos){
					//如果这里没找到class的后面的结尾符，就直接报错返回
					log.Format("文件：%s 行：%d 内容：%s 解析类名失败~，请检测语法规则...", file, lineCount, readLine);
					pLogUtils->e(log);

					readFile.Close();
					return;
				}
				//得到类名
				jClass.className = readLine.Mid(startPos, findPos - startPos + 1);
				startPos = findPos + 1;
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
				jClass.vReferencedClass.push_back(readLine.Mid(startPos, findPos - startPos + 1));
			}
		}//if(!isPassClassName)

	}//while(readFile.ReadString(readLine))
	
	readFile.Close();
}

//内容：“ extends View implements ViewPager.OnPageChangeListener, OnTabItemCenterPosListener ”
//“ extends WupBaseResult ”
//或者是空的
void JavaFileAnalyzer::dillClassInheritanceRelationship(const CString content, JavaClass& javaClass){

}