// AndroidManifestAnalyzer.cpp: implementation of the AndroidManifestAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RickToolKit.h"
#include "AndroidManifestAnalyzer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AndroidManifestAnalyzer::AndroidManifestAnalyzer()
{
	clear();
}

AndroidManifestAnalyzer::~AndroidManifestAnalyzer()
{
	
}

void AndroidManifestAnalyzer::clear()
{
	mManifestRefClasses.clear();
}

void AndroidManifestAnalyzer::setForRes(bool forRes)
{
	
}

void AndroidManifestAnalyzer::printResult()	//打印结果
{
	for(int i = 0; i < mManifestRefClasses.size(); i++){
		pLogUtils->i("[Project:"+ mManifestRefClasses[i].key);
		for(int index=0; index < mManifestRefClasses[i].vReferencedClass.size();index++)
		{
			pLogUtils->i(LINE_TABLE + mManifestRefClasses[i].vReferencedClass[index]);
		}
		pLogUtils->i(LINE_BREAK);	
	}
}

CString AndroidManifestAnalyzer::getAnalyzerRltDes()
{
	return ANALYSIS_RESULT_DEFAULT_DES;
}

void AndroidManifestAnalyzer::analyzerFile(const CString file){
	if(0 != mSuffix.CompareNoCase(GetFileSuffix(file)))
	{
		//不符合类型的文件
		pLogUtils->e("文件："+file+" 并不是"+mSuffix+"类文件，请确认输入...");
		return;
	}
	
	CStdioFile readFile;
	if(FALSE == readFile.Open(file, CFile::modeRead)){
		//读取失败
		pLogUtils->e("Open文件："+file+" 失败");
		return;
	}
	
	/*
	const CString XML_NOTE_FLG_BEGIN ="<!--";
	const CString XML_NOTE_FLG_END ="-->";
	*/
	bool isNote = false;
	bool isPassPackage = false;
	bool isPassApplicationDefine = false;
	bool isComponent = false;
	const int cXMLNoteBeginLen = XML_NOTE_FLG_BEGIN.GetLength();
	const int cXMLNoteEndLen = XML_NOTE_FLG_END.GetLength();
	const int cPackageKeyLen = XML_FILE_KEY_PACKAGE.GetLength();
	const int cAtributesValueKeyLen = MANIFEST_ATTRIBUTES_VALUE_KEY.GetLength();
	const int cActivityKeyLen	= MANIFEST_ACTIVITY_BEGIN.GetLength();
	const int cProviderKeyLen	= MANIFEST_PROVIDER_BEGIN.GetLength();
	const int cServiceKeyLen	= MANIFEST_SERVICE_BEGIN.GetLength();
	const int cReceiverKeyLen	= MANIFEST_RECEIVER_BEGIN.GetLength();
	CString readLine,log,prefix,tmp;

	XML_REF_STRUCT referencedClass;
	referencedClass.init();
	
	int findPos = -1;
	int startPos = 0;
	int lineCount = 0;
	int strLen = 0;
	int endPos = -1;
	while(readFile.ReadString(readLine)) {
		++lineCount;
		readLine.TrimLeft();
		readLine.TrimRight();
		if(readLine.IsEmpty()){
			continue;
		}
		
		//注释还没结束
		if(isNote){
			findPos = readLine.Find(XML_NOTE_FLG_END, 0);
			//发现了注释的结束符“-->”
			if(-1 < findPos){
				isNote = false;
				startPos = findPos + cXMLNoteEndLen;
				if(startPos + 1 < readLine.GetLength()){
					readLine = readLine.Mid(startPos); //后面还有内容，囧...
					log.Format("文件：%s 行：%d 出现了多行注释后跟有内容的【不规范写法】", file, lineCount);
					pLogUtils->w(log);
				}
			}

			continue;
		}//if(isNote)
		
		//处理注释开始的情况
		findPos = readLine.Find(XML_NOTE_FLG_BEGIN, 0);
		//发现了"<!--"
		while(-1 < findPos){
			isNote = true;			
			startPos = findPos + cXMLNoteBeginLen;
			findPos = readLine.Find(XML_NOTE_FLG_END, startPos);
			//在同一行又发现了注释的结束符“-->”
			if(-1 < findPos){
				isNote = false;
			}else{
				break;
			}
			
			startPos = findPos + cXMLNoteEndLen;
			findPos = readLine.Find(XML_NOTE_FLG_BEGIN, startPos);
		}//while(-1 < findPos)
		
		if(isNote){
			continue;
		}
		
		//开始处理AndroidManifest.xml内容块
		
		//先判断是否获取到了package
		if(!isPassPackage){
			if(-1 < readLine.Find(XML_DECLARATION_BEGIN,0)){
				continue;
			}

			prefix = readLine.Left(cPackageKeyLen);			
			if(prefix == XML_FILE_KEY_PACKAGE){
				//是package语句
				isPassPackage = true;
				
				startPos = cPackageKeyLen;
				findPos = readLine.Find(QUOTATION_MARK, startPos + 1);
				if(findPos < startPos){
					log.Format("文件：%s 行：%d 内容：%s 出现了package关键字，却没找到结束符【不规范写法】", file, lineCount, readLine);
					pLogUtils->w(log);

					readFile.Close();
					return;
				}

				referencedClass.key = readLine.Mid(startPos, findPos - startPos);
				referencedClass.key.TrimLeft();
				referencedClass.key.TrimRight();
			}
			
			continue;			
		}//if(!isPassPackage)

		//如果没有找到Application的定义
		if(!isPassApplicationDefine){
			//找到了"<application"标识
			if(-1 < (findPos = readLine.Find(MANIFEST_APPLICATION_BEGIN, 0))){
				startPos = MANIFEST_APPLICATION_BEGIN.GetLength();
				findPos = readLine.Find(MANIFEST_ATTRIBUTES_VALUE_KEY, startPos);
				endPos = readLine.Find(XML_ATTRIBUTES_NAME_END, startPos);
				//如果当前行没有结束符也没有name的赋值，就继续往下读取内容
				while(findPos < 0 && endPos < 0){
					readFile.ReadString(readLine);
					++lineCount;
					readLine.TrimLeft();
					readLine.TrimRight();
					if(readLine.IsEmpty()){
						continue;
					}

					findPos = readLine.Find(MANIFEST_ATTRIBUTES_VALUE_KEY, 0);
					endPos = readLine.Find(XML_ATTRIBUTES_NAME_END, 0);
				}//while(findPos < 0)

				if(-1 < findPos){
					//说明存在application定义
					startPos = findPos + cAtributesValueKeyLen;
					findPos = readLine.Find(QUOTATION_MARK, startPos);
					if(findPos < startPos){
						log.Format("文件：%s 行：%d 内容：%s 出现了属性赋值android:name=\"关键字，却没找到结束符【不规范写法】", file, lineCount, readLine);
						pLogUtils->e(log);
					}else{
						push_back(readLine.Mid(startPos, findPos - startPos), referencedClass);
					}
				}//if(-1 < findPos)

				//if(-1 < endPos){
				//这里就不判断endPos了，直接走下面的关键字
				isPassApplicationDefine = true;
			}//if(-1 < (findPos = readLine.Find(MANIFEST_APPLICATION_BEGIN, 0)))

			continue;
		}//if(!isPassApplicationDefine)
		
		
		//开始判断组件的声明
		// Write Code Here^
		/*
		
		  const CString MANIFEST_ACTIVITY_BEGIN = "<activity";
		  const CString MANIFEST_PROVIDER_BEGIN = "<provider";
		  const CString MANIFEST_SERVICE_BEGIN = "<service";
		  const CString MANIFEST_RECEIVER_BEGIN = "<receiver";
		  const CString MANIFEST_ATTRIBUTES_VALUE_KEY = "android:name=\"";
		  cAtributesValueKeyLen
		  const int cActivityKeyLen	= MANIFEST_ACTIVITY_BEGIN.GetLength();
		  const int cProviderKeyLen	= MANIFEST_PROVIDER_BEGIN.GetLength();
		  const int cServiceKeyLen	= MANIFEST_SERVICE_BEGIN.GetLength();
		  const int cReceiverKeyLen	= MANIFEST_RECEIVER_BEGIN.GetLength();
		*/
		//.....
		if(!isComponent){
			//activity
			findPos = readLine.Find(MANIFEST_ACTIVITY_BEGIN, 0);
			startPos = findPos + cActivityKeyLen;
			
			if(findPos < 0){
				//provider
				findPos = readLine.Find(MANIFEST_PROVIDER_BEGIN, 0);
				startPos = findPos + cProviderKeyLen;
			}
			
			if(findPos < 0){
				//service
				findPos = readLine.Find(MANIFEST_SERVICE_BEGIN, 0);
				startPos = findPos + cServiceKeyLen;
			}
			
			if(findPos < 0){
				//receiver
				findPos = readLine.Find(MANIFEST_RECEIVER_BEGIN, 0);
				startPos = findPos + cReceiverKeyLen;
			}

			//没找到组件就直接continue
			if(findPos < 0){
				continue;
			}

			isComponent = true;
			findPos = readLine.Find(MANIFEST_ATTRIBUTES_VALUE_KEY, startPos);
			while(findPos < 0){		//直到读取到了【MANIFEST_ATTRIBUTES_VALUE_KEY】android:name=" 这个属性
				readFile.ReadString(readLine);
				++lineCount;
				readLine.TrimLeft();
				readLine.TrimRight();
				if(readLine.IsEmpty()){
					continue;
				}

				findPos = readLine.Find(MANIFEST_ATTRIBUTES_VALUE_KEY, 0);
			}
			isComponent = false;
			startPos = findPos + cAtributesValueKeyLen;
			findPos = readLine.Find(QUOTATION_MARK, startPos);
			if(findPos < startPos){
				log.Format("文件：%s 行：%d 内容：%s 出现了属性赋值android:name=\"关键字，却没找到结束符【不规范写法】", file, lineCount, readLine);
				pLogUtils->e(log);
			} else {
				push_back(readLine.Mid(startPos, findPos - startPos), referencedClass);
			}

			continue;
		}//if(!isComponent)
	}//while(readFile.ReadString(readLine))

	mManifestRefClasses.push_back(referencedClass);
	
	readFile.Close();
}

void AndroidManifestAnalyzer::push_back(CString ref, XML_REF_STRUCT& value){
	//如果key是以"."开头，需要在前面补上packageName
	ref.TrimLeft();
	ref.TrimRight();
	if(0 == ref.Find(".", 0)){
		ref = value.key + ref;
	}

	value.vReferencedClass.push_back(ref);
}

const vector<XML_REF_STRUCT> AndroidManifestAnalyzer::getManifestRefClasses(){
	return mManifestRefClasses;
}