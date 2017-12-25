// XmlUtils.cpp: implementation of the XmlUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RickToolKit.h"
#include "XmlUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XmlUtils::XmlUtils()
{
	
}

XmlUtils::~XmlUtils()
{
	
}

bool XmlUtils::loadFile(const CString file){
    HRESULT hr = pDoc.CreateInstance(__uuidof(xml::DOMDocument)) ; //�����ĵ�����
    if(!SUCCEEDED(hr)){
		pLogUtils->e("�޷�����DM����");
        return false ;
    }
	
    VARIANT_BOOL bFlag;
    bFlag = pDoc->load(_bstr_t(file)) ; //load xml�ļ�
    if(bFlag == S_FALSE){
        cout<<"��BitComet.xml����"<<endl ;
        return false ;
    }
	
    hr = pDoc->get_documentElement(&pElem) ; //��ȡ�����
    if(pElem == NULL){
        cout<<"��ȡ�����ʧ�ܣ�"<<endl ;
        return false ;
    }
	
    BSTR strTagName = NULL;
    hr = pElem->get_tagName(&strTagName) ;
    cout<<"------TagName------"<<endl ;
    wcout <<strTagName<<endl ;
    SysFreeString(strTagName) ;
    hr = pElem->get_childNodes(&pNodeList) ; //��ȡ�ӽ���б�
    long lCount;
    hr = pNodeList->get_length(&lCount) ;  
    for(long i=0; i<lCount; ++i){  
        xml::DOMNodeType NodeType ;
        xml::IXMLDOMNodeListPtr pChildNodeList ;
        hr = pNodeList->get_item(i, &pNode) ; //��ȡ���
        hr = pNode->get_nodeType(&NodeType) ; //��ȡ�����Ϣ������      
        if(xml::NODE_ELEMENT == NodeType){
            hr = pNode->get_childNodes(&pChildNodeList) ;
            long childLen ;
            pChildNodeList->get_length(&childLen) ;
            //���ÿ��ַ����������ڹ���)chs�����������..������msdn�ϲ�ѯ
            wcout.imbue(locale("chs")) ; 
            cout << "------NodeList------" << endl ;
            for (int j=0; j<childLen; ++j){
                xml::IXMLDOMNodePtr pChildNode ;
                BSTR strValue ;
                hr = pChildNodeList->get_item(j, &pChildNode) ;
                hr = pChildNode->get_nodeName(&strValue) ; //��ȡ�������
                wcout<<strValue<<endl ;
                SysFreeString(strValue) ;
                hr = pChildNode->get_text(&strValue) ; //��ȡ����ֵ
                wcout <<strValue<<endl<<endl ;
                SysFreeString(strValue) ;
                pChildNode.Release() ;
            }
        }
        pNode.Release() ;
        pChildNodeList.Release() ;  
    }
	
    pNodeList.Release() ;
    pElem.Release() ;
    pDoc.Release() ; 
    CoUninitialize() ;
	
    return true ;
}
