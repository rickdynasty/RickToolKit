// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A5760DD5_1B9D_4561_8C3A_D37E00F43D1D__INCLUDED_)
#define AFX_STDAFX_H__A5760DD5_1B9D_4561_8C3A_D37E00F43D1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <vector>
#endif // _AFX_NO_AFXCMN_SUPPORT

using namespace std;

typedef struct{
	void init(){
		usedCount = 0;
		vReferencedClass.clear();
		vImplementsInterfaces.clear();
		vMethods.clear();
	}
	CString filePath;						//文件全路径
	CString packageName;					//包名
	vector<CString> vReferencedClass;		//引用的类【用于标记那些类在使用】
	CString className;						//类名
	CString parentClassName;				//父类
	vector<CString> vImplementsInterfaces;	//实现的接口
	vector<CString> vMethods;				//方法
	int usedCount;						//被引用的次数
}JavaClass;

#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010

const CString ANALYSIS_RESULT_DEFAULT_DES = "Analysis completed";

const CString LINE_BREAK = "\n";
const CString LINE_TABLE = "\t";

//文件后缀
const CString SUFFIX_PNG	="png";
const CString SUFFIX_XML	="xml";
const CString SUFFIX_JAVA	= "java";
const CString SUFFIX_CONFIG	="config";

//java类的注释
const CString JAVA_NOTE_ONELINE_FLG ="//";
const CString JAVA_NOTE_FLG_BEGIN ="/*";
const CString JAVA_NOTE_FLG_END ="*/";

//
const CString JAVA_FILE_PACKAGE_KEY = "package ";
const CString JAVA_FILE_IMPROT_KEY	= "import ";
const CString JAVA_FILE_CLASS_KEY	= " class ";
const CString JAVA_FILE_CLASS_KEY_EX	= "class ";
const CString JAVA_FILE_INTERFACE_KEY	= " interface ";
const CString JAVA_FILE_EXTENDS_KEY	= "extends ";
const CString JAVA_FILE_IMPLEMENTS_KEY	= "implements ";
const CString PACKAGE_or_IMPROT_EDN_FLG = ";";
const CString SPACE_FLG				= " ";
const CString TEMPLATE_FLG_BEGIN	= "<";
const CString COMMA_FLG				= ",";
const CString JAVA_CLASS_BODY_BEGIN = "{";

//属性关键字
const CString KEYWORD_PUBLIC		= "public";
const CString KEYWORD_PRIVATE		= "private";
const CString KEYWORD_PROTECTED		= "protected";
//关键字 class
const CString KEYWORD_CLASS			= "class";

//Log 
const CString ANALYSIS_RLT_LOG_POS_PREFIX = "\\Log";
const CString ANALYSIS_RESULT_LOG = "\\log.txt";
const CString ANALYSIS_OUT_LOG = "\\log_ex.txt";
const CString ANALYSIS_PROB_LOG = "\\log_problematic.txt";
const CString ANALYSIS_ERROR_LOG = "\\log_error.txt";
const CString ANALYSIS_WARN_LOG = "\\log_warn.txt";


CString GetCurrentProDirectory();
CString GetLogDirectory();
CString GetFileSuffix(const CString fileName);
CString GetFileName(const CString file);
CString GetFileNameWithoutSuffix(const CString file);
bool IsDirExist(const CString & csDir);
void ensureDir(const CString & csDir);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A5760DD5_1B9D_4561_8C3A_D37E00F43D1D__INCLUDED_)
