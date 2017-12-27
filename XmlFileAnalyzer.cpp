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

void XmlFileAnalyzer::printResult()	//��ӡ���
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

//�ռ��������
void XmlFileAnalyzer::collectRefClasses(const CString file){
	if(0 != mSuffix.CompareNoCase(GetFileSuffix(file)))
	{
		//���������͵��ļ�
		pLogUtils->e("�ļ���"+file+" ������xml�ļ�����ȷ������...");
		return;
	}
	
	CStdioFile readFile;
	if(FALSE == readFile.Open(file, CFile::modeRead)){
		//��ȡʧ��
		pLogUtils->e("Open�ļ���"+file+" ʧ��");
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
			//������ע�͵Ľ�������*/��
			if(-1 < findPos){
				isNote = false;
				startPos = findPos + cXmlNoteEndFlgLen;
				if(startPos + 1 < lineLen){
					readLine = readLine.Mid(startPos); //���滹�����ݣ���...
					log.Format("�ļ���%s �У�%d ������ע�ͺ�������ݡ����淶д����", file, lineCount);
					pLogUtils->w(log);
				}
			}else{
				continue;
			}
		}
		
		//����ע�Ϳ�ʼ�����
		findPos = readLine.Find(XML_NOTE_FLG_BEGIN, 0);
		//������"/*"
		while(-1 < findPos){
			isNote = true;			
			startPos = findPos + cXmlNoteBeginFlgLen;
			findPos = readLine.Find(XML_NOTE_FLG_END, startPos);
			//��ͬһ���ַ�����ע�͵Ľ�������*/��
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

		//���ұ�ǩ�Ŀ�ʼ
		findPos = readLine.Find(XML_ITEM_BEGIN_FLG,0);
		if(findPos < 0){
			continue; //XML_LAYOUT_HEAD_FLGҲ����Ҫ��XML_ITEM_BEGIN_FLG����
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

		//�ֻ�layout�Թ���class������
		refClass = readLine.Mid(startPos, findPos - startPos);
		if(-1 < refClass.Find(DOT_FLG, 1)){
			//��������ſ�����xml������
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