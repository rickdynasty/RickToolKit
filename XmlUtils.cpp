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
    HRESULT hr = pDoc.CreateInstance(__uuidof(xml::DOMDocument)) ; //创建文档对象
    if(!SUCCEEDED(hr)){
		pLogUtils->e("无法创建DM对象！");
        return false ;
    }
	
    VARIANT_BOOL bFlag;
    bFlag = pDoc->load(_bstr_t(file)) ; //load xml文件
    if(bFlag == S_FALSE){
        cout<<"打开BitComet.xml出错！"<<endl ;
        return false ;
    }
	
    hr = pDoc->get_documentElement(&pElem) ; //获取根结点
    if(pElem == NULL){
        cout<<"获取根结点失败！"<<endl ;
        return false ;
    }
	
    BSTR strTagName = NULL;
    hr = pElem->get_tagName(&strTagName) ;
    cout<<"------TagName------"<<endl ;
    wcout <<strTagName<<endl ;
    SysFreeString(strTagName) ;
    hr = pElem->get_childNodes(&pNodeList) ; //获取子结点列表
    long lCount;
    hr = pNodeList->get_length(&lCount) ;  
    for(long i=0; i<lCount; ++i){  
        xml::DOMNodeType NodeType ;
        xml::IXMLDOMNodeListPtr pChildNodeList ;
        hr = pNodeList->get_item(i, &pNode) ; //获取结点
        hr = pNode->get_nodeType(&NodeType) ; //获取结点信息的类型      
        if(xml::NODE_ELEMENT == NodeType){
            hr = pNode->get_childNodes(&pChildNodeList) ;
            long childLen ;
            pChildNodeList->get_length(&childLen) ;
            //设置宽字符区域（类似于国域)chs代表简体中文..可以在msdn上查询
            wcout.imbue(locale("chs")) ; 
            cout << "------NodeList------" << endl ;
            for (int j=0; j<childLen; ++j){
                xml::IXMLDOMNodePtr pChildNode ;
                BSTR strValue ;
                hr = pChildNodeList->get_item(j, &pChildNode) ;
                hr = pChildNode->get_nodeName(&strValue) ; //获取结点名字
                wcout<<strValue<<endl ;
                SysFreeString(strValue) ;
                hr = pChildNode->get_text(&strValue) ; //获取结点的值
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
