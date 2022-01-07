// AutoDec.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "AutoDec.h"
#include "AutoDecDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoDecApp

BEGIN_MESSAGE_MAP(CAutoDecApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoDecApp ����

CAutoDecApp::CAutoDecApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CAutoDecApp ����

CAutoDecApp theApp;


// CAutoDecApp ��ʼ��

BOOL CAutoDecApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	//�����д���,�������˺�Ҫ�˳�
	//ȡ����
	CString strFullName = m_lpCmdLine;
	
	TCHAR szPath[MAX_PATH] = {0};
	lstrcpy(szPath, strFullName);
	int nLen = lstrlen(szPath);
	if (szPath[0] == '"')
	{
		if (szPath[nLen - 1] == '"')
		{
			szPath[nLen - 1] = 0;
		}

		strFullName = szPath + 1;
	}

	DWORD dwAttrib = ::GetFileAttributes(strFullName);
	if (INVALID_FILE_ATTRIBUTES != dwAttrib)
	{
		if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
		{
			//����һ��Ŀ¼
			strFullName += _T("\\");
			AddFilesByPath(strFullName, FALSE, NULL);
		}
		else
		{
			//�����ļ�
			BOOL bRet = AddFilesByFileName(strFullName);
			if (!bRet)
			{
				AfxMessageBox(_T("������ʧ��!"));
			}
		}

		return FALSE;
	}

	CAutoDecDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

DWORD CAutoDecApp::AddFilesByPath(LPCTSTR lpszFileDir, BOOL bIncludeSub, LPCTSTR lpszFileExt)
{
	CString strFileNumber;
	CString strFileName;
	CString strExt;
	int nExtLength;
	CTime tFileTime;
	CFileFind finder;

	int nFileCount = 0;
	int nProgress = 0;

	CString strFilter;
	if (NULL == lpszFileExt)
	{
		strFilter.Format(_T("%s\\*.%s"), lpszFileDir, _T("*"));
	}
	else
	{
		strFilter.Format(_T("%s\\*.%s"), lpszFileDir, lpszFileExt);
	}

	BOOL bWorking = finder.FindFile(strFilter);
	while (bWorking)
	{
		nProgress++;

		bWorking = finder.FindNextFile();
		if (finder.IsDots())
		{
			continue;
		}

		//��Ϊ��Ŀ¼
		if (finder.IsDirectory())
		{
			//���������Ŀ¼����еݹ�
			if (bIncludeSub)
			{
				CString strSubDir = CString(lpszFileDir) + _T("\\") + finder.GetFileName();
				AddFilesByPath(strSubDir, TRUE, lpszFileExt);
			}	
		}
		else
		{
			//�����ļ�
			nExtLength = finder.GetFileName().GetLength() - finder.GetFileTitle().GetLength() - 1;
			strExt = finder.GetFileName().Right(nExtLength);

			//��չ������
			if (NULL != lpszFileExt)
			{
				if (strExt != lpszFileExt)
				{
					continue;
				}
			}			

			//ȫ·��
			strFileName = finder.GetFilePath();
			nFileCount++;

			AddFilesByFileName(strFileName);
		}
	}

	return nFileCount;
}

BOOL CAutoDecApp::AddFilesByFileName(LPCTSTR lpszFileName)
{
	//�ļ����ָ�
	TCHAR szDir[_MAX_DIR] = {0};
	TCHAR szDrive[_MAX_DRIVE] = {0};
	TCHAR szName[_MAX_FNAME] = {0};
	TCHAR szExt[_MAX_EXT] = {0};
	_tsplitpath(lpszFileName, szDrive, szDir, szName, szExt);

	CString strDstPath;
	strDstPath.Format(_T("%s%s%s%s_dec"), szDrive, szDir, szName, szExt);

	CAutoDecDlg dlg;
	BOOL bRet = dlg.ProcessFile(lpszFileName, strDstPath, FALSE, 0);

	return bRet;
}