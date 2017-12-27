// XmlFileAnalyzer.cpp: implementation of the XmlFileAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XmlFileAnalyzer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XmlFileAnalyzer::XmlFileAnalyzer()
{
	
}

XmlFileAnalyzer::~XmlFileAnalyzer()
{
	clear();
}

void XmlFileAnalyzer::clear()
{
	mLayoutRefClasses.clear();
}

void XmlFileAnalyzer::setForRes(bool forRes)
{
	
}

void XmlFileAnalyzer::printResult()	//打印结果
{
	
}

CString XmlFileAnalyzer::getAnalyzerRltDes()
{
	return ANALYSIS_RESULT_DEFAULT_DES;
}

void XmlFileAnalyzer::analyzerFile(const CString file){
}

const vector<XML_REF_STRUCT> XmlFileAnalyzer::getLayoutRefClasses(){
	return mLayoutRefClasses;
}

//收集类的引用
void XmlFileAnalyzer::collectRefClasses(const CString file){
	if(0 != mSuffix.CompareNoCase(GetFileSuffix(file)))
	{
		//不符合类型的文件
		pLogUtils->e("文件："+file+" 并不是xml文件，请确认输入...");
		return;
	}
	
	CStdioFile readFile;
	if(FALSE == readFile.Open(file, CFile::modeRead)){
		//读取失败
		pLogUtils->e("Open文件："+file+" 失败");
		return;
	}
	
	const int cXmlNoteBeginFlgLen = XML_NOTE_FLG_BEGIN.GetLength();
	const int cXmlNoteEndFlgLen = XML_NOTE_FLG_END.GetLength();
	bool isNote = false;
	bool isPassHeadFlg = false;

	XML_REF_STRUCT referencedClass;
	referencedClass.init();
	referencedClass.key = GetFileNameWithoutSuffix(file);
	referencedClass.key.TrimLeft();
	referencedClass.key.TrimRight();
	
	CString readLine,log, refClass;
	int findPos = -1;
	int startPos = 0;
	int comparePos = -1;
	int lineCount = 0;
	int lineLen = 0;
	while(readFile.ReadString(readLine)) {
		++lineCount;		
		readLine.TrimLeft();
		readLine.TrimRight();
		if(readLine.IsEmpty()){
			continue;
		}

		/*
		const CString XML_NOTE_FLG_BEGIN ="<!--";
		const CString XML_NOTE_FLG_END ="-->";
		*/
		
		lineLen = readLine.GetLength();
		if(isNote){
			findPos = readLine.Find(XML_NOTE_FLG_END, 0);
			//发现了注释的结束符“*/”
			if(-1 < findPos){
				isNote = false;
				startPos = findPos + cXmlNoteEndFlgLen;
				if(startPos + 1 < lineLen){
					readLine = readLine.Mid(startPos); //后面还有内容，囧...
					log.Format("文件：%s 行：%d 出现了注释后跟有内容【不规范写法】", file, lineCount);
					pLogUtils->w(log);
				}
			}else{
				continue;
			}
		}
		
		//处理注释开始的情况
		findPos = readLine.Find(XML_NOTE_FLG_BEGIN, 0);
		//发现了"/*"
		while(-1 < findPos){
			isNote = true;			
			startPos = findPos + cXmlNoteBeginFlgLen;
			findPos = readLine.Find(XML_NOTE_FLG_END, startPos);
			//在同一行又发现了注释的结束符“*/”
			if(-1 < findPos){
				isNote = false;
			}else{
				break;
			}
			
			startPos = findPos + cXmlNoteEndFlgLen;
			findPos = readLine.Find(XML_NOTE_FLG_BEGIN, startPos);
		}
		
		if(isNote){
			continue;
		}

		//查找标签的开始
		findPos = readLine.Find(XML_ITEM_BEGIN_FLG,0);
		if(findPos < 0){
			continue; //XML_LAYOUT_HEAD_FLG也必须要有XML_ITEM_BEGIN_FLG符号
		}

		startPos = findPos + XML_ITEM_BEGIN_FLG.GetLength();
		findPos = readLine.Find(SPACE_FLG,startPos);
		if(findPos < startPos){
			findPos = lineLen;
		}

		if(findPos - startPos < 2){
			for(int index = findPos; index < lineLen; index++){
				if(' ' != readLine.GetAt(index)){
					startPos = index;
					break;
				}
			}
			findPos = readLine.Find(SPACE_FLG, startPos);
		}

		//手机layout对工程class的引用
		refClass = readLine.Mid(startPos, findPos - startPos);
		if(-1 < refClass.Find(DOT_FLG, 1)){
			//这种情况才可能是xml引用类
			if(!dataIsExistInVector(refClass, referencedClass.vReferencedClass)){
				referencedClass.vReferencedClass.push_back(refClass);
			}
		}

		if(!isPassHeadFlg){
			if(-1 < readLine.Find(XML_LAYOUT_HEAD_FLG, 0)){
				isPassHeadFlg = true;
			}

			continue;
		}

		//refClass

	}//while(readFile.ReadString(readLine))

	mLayoutRefClasses.push_back(referencedClass);
	
	readFile.Close();
}