// XmlUtils.h: interface for the XmlUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLUTILS_H__DCA5EA19_52B4_426C_97D8_882217CB7CC9__INCLUDED_)
#define AFX_XMLUTILS_H__DCA5EA19_52B4_426C_97D8_882217CB7CC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <iostream>
#import "msxml.dll" rename_namespace("xml")
#include "LogUtils.h"

using namespace std;

class XmlUtils  
{
public:
	XmlUtils();
	virtual ~XmlUtils();
	bool loadFile(const CString file);
	void setLogUtils(LogUtils* logUtils){ pLogUtils = logUtils;}
protected:
	xml::IXMLDOMDocumentPtr pDoc ;
    xml::IXMLDOMElementPtr  pElem ;
    xml::IXMLDOMNodePtr     pNode ;
    xml::IXMLDOMNodeListPtr pNodeList ;

private:
	LogUtils* pLogUtils;
};

#endif // !defined(AFX_XMLUTILS_H__DCA5EA19_52B4_426C_97D8_882217CB7CC9__INCLUDED_)
