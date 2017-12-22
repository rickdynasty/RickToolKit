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
	int dotPos = file.ReverseFind('\\');
	if(-1 == dotPos){
		dotPos = file.ReverseFind('/');
	}

	return file.Right(file.GetLength() - dotPos - 1);
}
