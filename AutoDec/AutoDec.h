// AutoDec.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAutoDecApp:
// �йش����ʵ�֣������ AutoDec.cpp
//

class CAutoDecApp : public CWinApp
{
public:
	CAutoDecApp();

// ��д
	public:
	virtual BOOL InitInstance();

	DWORD AddFilesByPath(LPCTSTR lpszFileDir, BOOL bIncludeSub, LPCTSTR lpszFileExt);
	BOOL AddFilesByFileName(LPCTSTR lpszFileName);

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAutoDecApp theApp;