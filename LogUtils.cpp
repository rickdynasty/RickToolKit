// LogUtils.cpp: implementation of the LogUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RickToolKit.h"
#include "LogUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogUtils::LogUtils(CString folderName)
{
	bLogOpen = false;
	bErrorLogOpen = false;
	bWarnLogOpen = false;
	iErrorCount = 0;
	iWarnCount = 0;
	
	//获取当前文件夹路径
	mLogFolder = GetLogDirectory();
	if(!folderName.IsEmpty()){
		mLogFolder += ("\\"+folderName);
	}

	d("Log文件：" + mLogFolder + ANALYSIS_RESULT_LOG);
}

LogUtils::~LogUtils()
{
	
}

CString LogUtils::GetCurrentTime(){
	return CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
}

//Log
void LogUtils::d(CString log){
	if(false == bLogOpen){
		::DeleteFile(mLogFolder + ANALYSIS_RESULT_LOG);
		bLogOpen = mLogFile.Open(mLogFolder + ANALYSIS_RESULT_LOG, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	}

	if(bLogOpen){
		mLogFile.WriteString(GetCurrentTime()+" "+log + LINE_BREAK);
	}
}

void LogUtils::e(CString log){
	if(false == bErrorLogOpen){
		::DeleteFile(mLogFolder + ANALYSIS_ERROR_LOG);
		bErrorLogOpen = mErrorLogFile.Open(mLogFolder + ANALYSIS_ERROR_LOG, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	}
	
	if(bErrorLogOpen){
		mErrorLogFile.WriteString(GetCurrentTime()+" "+log + LINE_BREAK);
		++iErrorCount;
	}
}

void LogUtils::w(CString log){
	if(false == bWarnLogOpen){
		::DeleteFile(mLogFolder + ANALYSIS_WARN_LOG);
		bWarnLogOpen = mWarnLogFile.Open(mLogFolder + ANALYSIS_WARN_LOG, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	}

	if(bWarnLogOpen){
		mWarnLogFile.WriteString(GetCurrentTime()+" "+log + LINE_BREAK);
		++iWarnCount;
	}
}

int LogUtils::getErrorCount(){
	return iErrorCount;
}

int LogUtils::getWarnCount(){
	return iWarnCount;
}
