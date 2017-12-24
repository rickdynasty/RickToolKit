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
	bLogProbOpen = false;
	bWarnLogOpen = false;
	bLogExOpen = false;

	iErrorCount = 0;
	iWarnCount = 0;
	
	//获取当前文件夹路径
	mLogFolder = GetLogDirectory();
	ensureDir(mLogFolder);
	
	if(!folderName.IsEmpty()){
		mLogFolder += ("\\"+folderName);
		ensureDir(mLogFolder);
	}

	d("Log文件：" + mLogFolder + ANALYSIS_RESULT_LOG);
}

LogUtils::~LogUtils()
{
	closeOpenLogFile();
}

void LogUtils::closeOpenLogFile(){
	if(bLogOpen){
		mLogFile.Close();
		bLogOpen = false;
	}

	if(bLogExOpen){
		mLogExFile.Close();
		bLogExOpen = false;
	}

	if(bLogProbOpen){
		mLogProbFile.Close();
		bLogProbOpen = false;
	}
	
	if(bErrorLogOpen){
		mErrorLogFile.Close();
		bErrorLogOpen = false;
	}
	
	if(bWarnLogOpen){
		mWarnLogFile.Close();
		bWarnLogOpen = false;
	}
}

CString LogUtils::GetCurrentTime(){
	return CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
}

//Log
void LogUtils::d(CString log, bool printTime){
	if(false == bLogOpen){
		::DeleteFile(mLogFolder + ANALYSIS_RESULT_LOG);
		bLogOpen = mLogFile.Open(mLogFolder + ANALYSIS_RESULT_LOG, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	}

	if(bLogOpen){
		if(printTime){
			mLogFile.WriteString(GetCurrentTime()+" "+log + LINE_BREAK);
		} else {
			mLogFile.WriteString(log + LINE_BREAK);
		}
	}
}

void LogUtils::i(CString log, bool printTime){
	if(false == bLogExOpen){
		::DeleteFile(mLogFolder + ANALYSIS_OUT_LOG);
		bLogExOpen = mLogExFile.Open(mLogFolder + ANALYSIS_OUT_LOG, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	}

	if(bLogExOpen){
		if(printTime){
			mLogExFile.WriteString(GetCurrentTime()+" "+log + LINE_BREAK);
		} else {
			mLogExFile.WriteString(log + LINE_BREAK);
		}
	}
}

void LogUtils::p(CString log, bool printTime){
	if(false == bLogProbOpen){
		::DeleteFile(mLogFolder + ANALYSIS_PROB_LOG);
		bLogProbOpen = mLogProbFile.Open(mLogFolder + ANALYSIS_PROB_LOG, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	}

	if(bLogProbOpen){
		if(printTime){
			mLogProbFile.WriteString(GetCurrentTime()+" "+log + LINE_BREAK);
		} else {
			mLogProbFile.WriteString(log + LINE_BREAK);
		}
	}
}

void LogUtils::e(CString log, bool printTime){
	if(false == bErrorLogOpen){
		::DeleteFile(mLogFolder + ANALYSIS_ERROR_LOG);
		bErrorLogOpen = mErrorLogFile.Open(mLogFolder + ANALYSIS_ERROR_LOG, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	}
	
	if(bErrorLogOpen){
		if(printTime){
			mErrorLogFile.WriteString(GetCurrentTime()+" "+log + LINE_BREAK);
		} else {
			mErrorLogFile.WriteString(log + LINE_BREAK);
		}

		++iErrorCount;
	}
}

void LogUtils::w(CString log, bool printTime){
	if(false == bWarnLogOpen){
		::DeleteFile(mLogFolder + ANALYSIS_WARN_LOG);
		bWarnLogOpen = mWarnLogFile.Open(mLogFolder + ANALYSIS_WARN_LOG, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	}

	if(bWarnLogOpen){
		if(printTime){
			mWarnLogFile.WriteString(GetCurrentTime()+" "+log + LINE_BREAK);
		} else {
			mWarnLogFile.WriteString(log + LINE_BREAK);
		}

		++iWarnCount;
	}
}

int LogUtils::getErrorCount(){
	return iErrorCount;
}

int LogUtils::getWarnCount(){
	return iWarnCount;
}
