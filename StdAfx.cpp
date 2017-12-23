// stdafx.cpp : source file that includes just the standard includes
//	RickToolKit.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

CString GetCurrentProDirectory(){
	CString currentDirectory;
	//获取当前文件夹路径
	GetCurrentDirectory(MAX_PATH,currentDirectory.GetBuffer(MAX_PATH));
	currentDirectory.ReleaseBuffer();
	
	return currentDirectory;
}

CString GetLogDirectory(){
	return GetCurrentProDirectory() + ANALYSIS_RLT_LOG_POS_PREFIX;
}

CString GetFileSuffix(const CString fileName){
	int dotPos = fileName.ReverseFind('.');
	if(-1 == dotPos){
		return "";
	}
	
	CString suffix = fileName.Right(fileName.GetLength() - dotPos - 1);
	suffix.MakeLower();

	return suffix;
}

CString GetFileName(const CString file){
	int separatorPos = file.ReverseFind('\\');
	if(-1 == separatorPos){
		separatorPos = file.ReverseFind('/');
	}

	if(-1 == separatorPos){
		separatorPos = 0;
	}

	return file.Right(file.GetLength() - separatorPos - 1);
}

CString GetFileNameWithoutSuffix(const CString file){
	int separatorPos = file.ReverseFind('\\');
	if(-1 == separatorPos){
		separatorPos = file.ReverseFind('/');
	}

	if(-1 == separatorPos){
		separatorPos = 0;
	}
	
	int dotPos = file.ReverseFind('.');
	if(dotPos < separatorPos){
		dotPos = file.GetLength();
	}

	return file.Mid(separatorPos + 1, dotPos - separatorPos -1);
}