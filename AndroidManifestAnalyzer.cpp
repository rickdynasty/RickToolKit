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

void AndroidManifestAnalyzer::printResult()	//��ӡ���
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
		//���������͵��ļ�
		pLogUtils->e("�ļ���"+file+" ������"+mSuffix+"���ļ�����ȷ������...");
		return;
	}
	
	CStdioFile readFile;
	if(FALSE == readFile.Open(file, CFile::modeRead)){
		//��ȡʧ��
		pLogUtils->e("Open�ļ���"+file+" ʧ��");
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
		
		//ע�ͻ�û����
		if(isNote){
			findPos = readLine.Find(XML_NOTE_FLG_END, 0);
			//������ע�͵Ľ�������-->��
			if(-1 < findPos){
				isNote = false;
				startPos = findPos + cXMLNoteEndLen;
				if(startPos + 1 < readLine.GetLength()){
					readLine = readLine.Mid(startPos); //���滹�����ݣ���...
					log.Format("�ļ���%s �У�%d �����˶���ע�ͺ�������ݵġ����淶д����", file, lineCount);
					pLogUtils->w(log);
				}
			}

			continue;
		}//if(isNote)
		
		//����ע�Ϳ�ʼ�����
		findPos = readLine.Find(XML_NOTE_FLG_BEGIN, 0);
		//������"<!--"
		while(-1 < findPos){
			isNote = true;			
			startPos = findPos + cXMLNoteBeginLen;
			findPos = readLine.Find(XML_NOTE_FLG_END, startPos);
			//��ͬһ���ַ�����ע�͵Ľ�������-->��
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
		
		//��ʼ����AndroidManifest.xml���ݿ�
		
		//���ж��Ƿ��ȡ����package
		if(!isPassPackage){
			if(-1 < readLine.Find(XML_DECLARATION_BEGIN,0)){
				continue;
			}

			prefix = readLine.Left(cPackageKeyLen);			
			if(prefix == XML_FILE_KEY_PACKAGE){
				//��package���
				isPassPackage = true;
				
				startPos = cPackageKeyLen;
				findPos = readLine.Find(QUOTATION_MARK, startPos + 1);
				if(findPos < startPos){
					log.Format("�ļ���%s �У�%d ���ݣ�%s ������package�ؼ��֣�ȴû�ҵ������������淶д����", file, lineCount, readLine);
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

		//���û���ҵ�Application�Ķ���
		if(!isPassApplicationDefine){
			//�ҵ���"<application"��ʶ
			if(-1 < (findPos = readLine.Find(MANIFEST_APPLICATION_BEGIN, 0))){
				startPos = MANIFEST_APPLICATION_BEGIN.GetLength();
				findPos = readLine.Find(MANIFEST_ATTRIBUTES_VALUE_KEY, startPos);
				endPos = readLine.Find(XML_ATTRIBUTES_NAME_END, startPos);
				//�����ǰ��û�н�����Ҳû��name�ĸ�ֵ���ͼ������¶�ȡ����
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
					//˵������application����
					startPos = findPos + cAtributesValueKeyLen;
					findPos = readLine.Find(QUOTATION_MARK, startPos);
					if(findPos < startPos){
						log.Format("�ļ���%s �У�%d ���ݣ�%s ���������Ը�ֵandroid:name=\"�ؼ��֣�ȴû�ҵ������������淶д����", file, lineCount, readLine);
						pLogUtils->e(log);
					}else{
						push_back(readLine.Mid(startPos, findPos - startPos), referencedClass);
					}
				}//if(-1 < findPos)

				//if(-1 < endPos){
				//����Ͳ��ж�endPos�ˣ�ֱ��������Ĺؼ���
				isPassApplicationDefine = true;
			}//if(-1 < (findPos = readLine.Find(MANIFEST_APPLICATION_BEGIN, 0)))

			continue;
		}//if(!isPassApplicationDefine)
		
		
		//��ʼ�ж����������
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

			//û�ҵ������ֱ��continue
			if(findPos < 0){
				continue;
			}

			isComponent = true;
			findPos = readLine.Find(MANIFEST_ATTRIBUTES_VALUE_KEY, startPos);
			while(findPos < 0){		//ֱ����ȡ���ˡ�MANIFEST_ATTRIBUTES_VALUE_KEY��android:name=" �������
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
				log.Format("�ļ���%s �У�%d ���ݣ�%s ���������Ը�ֵandroid:name=\"�ؼ��֣�ȴû�ҵ������������淶д����", file, lineCount, readLine);
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
	//���key����"."��ͷ����Ҫ��ǰ�油��packageName
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