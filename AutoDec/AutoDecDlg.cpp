// AutoDecDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoDec.h"
#include "AutoDecDlg.h"
#include "AboutlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoDecDlg �Ի���

CAutoDecDlg::CAutoDecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoDecDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bDetect = FALSE;
}

void CAutoDecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILEINFOS, m_ctlInputFileList);
	DDX_Control(pDX, IDC_LIST_FILELIST, m_ctlDecompiledFileList);
}

BEGIN_MESSAGE_MAP(CAutoDecDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BTN_DECOMPILE, &CAutoDecDlg::OnBnClickedBtnDecompile)
	ON_BN_CLICKED(IDC_BTN_ADDTOMENU, &CAutoDecDlg::OnBnClickedBtnAddtomenu)
	ON_BN_CLICKED(IDC_BTN_SELALL, &CAutoDecDlg::OnBnClickedBtnSelall)
	ON_BN_CLICKED(IDC_BTN_SELREV, &CAutoDecDlg::OnBnClickedBtnSelrev)
	ON_BN_CLICKED(IDC_BTN_CLEAN, &CAutoDecDlg::OnBnClickedBtnClean)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILEINFOS, &CAutoDecDlg::OnNMClickListFileinfos)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILEINFOS, &CAutoDecDlg::OnNMDblclkListFileinfos)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_ABOUT, &CAutoDecDlg::OnBnClickedBtnAbout)
	ON_BN_CLICKED(IDC_BTN_DECCURRENT, &CAutoDecDlg::OnBnClickedBtnDeccurrent)
	ON_BN_CLICKED(IDOK, &CAutoDecDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_DELSELECT, &CAutoDecDlg::OnBnClickedBtnDelselect)
	ON_BN_CLICKED(IDC_BTN_DELCURRENT, &CAutoDecDlg::OnBnClickedBtnDelcurrent)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FILEINFOS, &CAutoDecDlg::OnLvnItemchangedListFileinfos)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FILELIST, &CAutoDecDlg::OnLvnItemchangedListFilelist)
END_MESSAGE_MAP()


// CAutoDecDlg ��Ϣ�������

BOOL CAutoDecDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetWindowText(_T("Autoit3 Decompiler V1.9 fix��Author:HexBoy  QQ:8724271��"));

	InitInputFileListCtl();
	InitDecompiledFileListCtl();

	if (!CheckInterStatus())
	{
		GetDlgItem(IDC_BTN_ADDTOMENU)->SetWindowText(_T("�Ҽ�����"));
	}
	else
	{
		GetDlgItem(IDC_BTN_ADDTOMENU)->SetWindowText(_T("ȡ���Ҽ�"));
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAutoDecDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutRetailDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAutoDecDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAutoDecDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoDecDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int nCharactersLen = ::DragQueryFile(hDropInfo, 0, NULL, 0);

	TCHAR szFilePath[MAX_PATH] = {0};
	int nFileNum = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	for (int i=0; i<nFileNum; i++)
	{
		::DragQueryFile(hDropInfo, i, szFilePath, MAX_PATH);
		int nLen = lstrlen(szFilePath);
		if (0 == nLen)
		{
			//·������
			continue;
		}

		//ȡ����
		DWORD dwAttrib = ::GetFileAttributes(szFilePath);
		if (INVALID_FILE_ATTRIBUTES == dwAttrib)
		{
			continue;
		}

		if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
		{
			szFilePath[nLen] = '\\';
		}

		//Ŀ¼�ļ��ֱ���
		if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
		{
			//����һ��Ŀ¼
			AddFilesByPath(szFilePath, FALSE, NULL);
		}
		else
		{
			//�����ļ�
			AddFilesByFileName(szFilePath);
		}
	}

	SetButtonStatus();

// 	if (1 == nFileNum)
// 	{
// 		int nItem = m_ctlInputFileList.GetItemCount() - 1;
// 		m_ctlInputFileList.SetFocus();
// 		m_ctlInputFileList.SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
// 	}

	CDialog::OnDropFiles(hDropInfo);
}

void CAutoDecDlg::InitInputFileListCtl()
{
	m_ctlInputFileList.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
	m_ctlInputFileList.InsertColumn(FILEINFOS_INDEX, _T("#"), LVCFMT_LEFT, 40);
	m_ctlInputFileList.InsertColumn(FILEINFOS_STATUS, _T("״̬"), LVCFMT_LEFT, 40);
	m_ctlInputFileList.InsertColumn(FILEINFOS_FILEPATH, _T("�ļ�·��"), LVCFMT_LEFT, 370);
	m_ctlInputFileList.InsertColumn(FILEINFOS_FILEFORMAT, _T("��ʽ"), LVCFMT_LEFT, 40);
	m_ctlInputFileList.InsertColumn(FILEINFOS_VERINFO, _T("Autoit�汾"), LVCFMT_LEFT, 80);
	m_ctlInputFileList.InsertColumn(FILEINFOS_PASSWORD, _T("����"), LVCFMT_LEFT, 100);
	m_ctlInputFileList.InsertColumn(FILEINFOS_UNICODE, _T("����"), LVCFMT_LEFT, 60);	
	m_ctlInputFileList.InsertColumn(FILEINFOS_X64PLATFORM, _T("ƽ̨"), LVCFMT_LEFT, 40);
}

void CAutoDecDlg::InitDecompiledFileListCtl()
{
	m_ctlDecompiledFileList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
#if 1
	m_ctlDecompiledFileList.InsertColumn(0, _T("�ͷ�·��"), LVCFMT_LEFT, 650);
	m_ctlDecompiledFileList.InsertColumn(1, _T("���"), LVCFMT_LEFT, 40);
	m_ctlDecompiledFileList.InsertColumn(2, _T("��С(BYTE)"), LVCFMT_LEFT, 80);
#else
	m_ctlDecompiledFileList.InsertColumn(0, _T("�ͷ�·��"), LVCFMT_LEFT, 650);
#endif

	SHFILEINFO sfi = {0};

	TCHAR szSystemDir[MAX_PATH] = {0};
	GetWindowsDirectory(szSystemDir, MAX_PATH);

	HIMAGELIST himlSmall = (HIMAGELIST)SHGetFileInfo ((LPCTSTR)szSystemDir, 
		0, 
		&sfi, 
		sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON );

	HIMAGELIST himlLarge = (HIMAGELIST)SHGetFileInfo((LPCTSTR)szSystemDir, 
		0, 
		&sfi, 
		sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX | SHGFI_LARGEICON);

	if (himlSmall && himlLarge)
	{
		::SendMessage(m_ctlDecompiledFileList.m_hWnd, LVM_SETIMAGELIST,
			(WPARAM)LVSIL_SMALL, (LPARAM)himlSmall);
		::SendMessage(m_ctlDecompiledFileList.m_hWnd, LVM_SETIMAGELIST,
			(WPARAM)LVSIL_NORMAL, (LPARAM)himlLarge);
	}
}

DWORD CAutoDecDlg::AddFilesByPath(LPCTSTR lpszFileDir, BOOL bIncludeSub, LPCTSTR lpszFileExt)
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

DWORD CAutoDecDlg::AddFilesByFileName(LPCTSTR lpszFileName)
{
	DWORD dwFileCount = m_ctlInputFileList.GetItemCount();
	m_ctlInputFileList.InsertItem(dwFileCount, NULL);

	CString strInfos;
	strInfos.Format(_T("%d"), dwFileCount+1);
	m_ctlInputFileList.SetItemText(dwFileCount, FILEINFOS_INDEX, strInfos);

	strInfos.Format(_T("%s"), lpszFileName);
	m_ctlInputFileList.SetItemText(dwFileCount, FILEINFOS_FILEPATH, strInfos);

	//ִ������
	m_bDetect = TRUE;
	BOOL bRet = ProcessFile(lpszFileName, NULL, TRUE, dwFileCount);
	m_bDetect = FALSE;

	m_ctlInputFileList.SetCheck(dwFileCount, bRet);

	return 0;
}

void CAutoDecDlg::OnBnClickedBtnDecompile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nItemCount = m_ctlInputFileList.GetItemCount();
	if (0 == nItemCount)
	{
		return;
	}

	for (int nItem=0; nItem<nItemCount; nItem++)
	{
		BOOL bCheckStat = m_ctlInputFileList.GetCheck(nItem);
		if (bCheckStat)
		{
			//m_ctlFileList.SetItemState(nItem, LVIS_SELECTED, LVNI_SELECTED);
			CString strSrcFile = m_ctlInputFileList.GetItemText(nItem, FILEINFOS_FILEPATH);

			//�ļ����ָ�
			TCHAR szDir[_MAX_DIR] = {0};
			TCHAR szDrive[_MAX_DRIVE] = {0};
			TCHAR szName[_MAX_FNAME] = {0};
			TCHAR szExt[_MAX_EXT] = {0};
			_tsplitpath(strSrcFile, szDrive, szDir, szName, szExt);

			CString strDstPath;
			strDstPath.Format(_T("%s%s%s%s_dec"), szDrive, szDir, szName, szExt);

			BOOL bRet = ProcessFile(strSrcFile, strDstPath, FALSE, nItem);
			if (!bRet)
			{
				//�����б�ؼ�
				CString strTextInfo;
				strTextInfo.Format(_T("%s"), _T("ʧ��"));
				m_ctlInputFileList.SetItemText(nItem, FILEINFOS_STATUS, strTextInfo);
			}
		}
	}
}

/*===================================================================
* ����:		WinIntegrated
* ����:		strRegName �˵���ע����е�Ŀ¼���֣�ֻҪ�������еĳ�ͻ����
			strShowName ���Ҽ��˵�����ʾ������,
			strCmdFile �˵�ִ��ʱ�����ó���ľ����ļ���
* ����:		���ɲ˵���Ŀ¼����Ҽ�
* ˵��:		ִ�гɹ�����TRUE�����򷵻�FALSE
* ʱ��:		2007/2/25
* ����:		���໪
====================================================================*/
BOOL CAutoDecDlg::WinIntegrated(CString strRegName, CString strMenuName, CString strCmdFile)
{
	CReg regInfo(HKEY_CLASSES_ROOT);

	if (!regInfo.VerifyKey(_T("Folder\\shell\\") + strRegName))
	{
		if (!regInfo.CreateKey(_T("Folder\\shell\\") + strRegName))
		{
			return FALSE;
		}
	}

	if (!regInfo.Write(_T(""), strMenuName))
	{
		return FALSE;
	}

	if (!regInfo.VerifyKey(_T("Folder\\shell\\") + strRegName + _T("\\command")))
	{
		if (!regInfo.CreateKey(_T("Folder\\shell\\") + strRegName + _T("\\command")))
		{
			return FALSE;
		}
	}
	
	CString strRegCommand = _T("\"") + strCmdFile + _T("\"") + _T(" \"") + _T("%1") + _T("\"");
	if (!regInfo.Write(_T(""), strRegCommand))
	{
		return FALSE;
	}
	
	return TRUE;
}

/*===================================================================
* ����:		WinIntegrated
* ����:		strRegName �˵���ע����е�Ŀ¼���֣�ֻҪ�������еĳ�ͻ����
			strMenuName ���Ҽ��˵�����ʾ������,
			strCmdFile �˵�ִ��ʱ�����ó���ľ����ļ���
			strFileExt Ҫ���ɲ˵����ļ���չ��(�������)
			strFileTypeDetail ����ļ�û�г�����֮����ʱ�����ɺ���ע����е��ļ���������
* ����:		���ɲ˵����ļ�����Ҽ�
* ˵��:		ִ�гɹ�����TRUE�����򷵻�FALSE
* ʱ��:		2007/2/25
* ����:		���໪
====================================================================*/
BOOL CAutoDecDlg::WinIntegrated(CString strRegName, CString strMenuName, CString strCmdFile, CString strFileExt, CString strFileTypeDetail)
{
	CReg regInfo(HKEY_CLASSES_ROOT);
	CString strRegKey(_T(""));
	
	if (!regInfo.VerifyKey(_T(".") + strFileExt))
	{
		if (!regInfo.CreateKey(_T(".") + strFileExt))
		{
			return FALSE;
		}
		
		if (!regInfo.Write(_T(""), strFileTypeDetail))
		{
			return FALSE;
		}

		strRegKey = strFileTypeDetail;
	}
	else
	{
		if (!regInfo.Read(_T(""), strRegKey))
		{
			return FALSE;
		}
	}
	
	if (!regInfo.VerifyKey(strRegKey + _T("\\Shell\\") + strRegName))
	{
		if (!regInfo.CreateKey(strRegKey + _T("\\Shell\\") + strRegName))
		{
			return FALSE;
		}
	}
	
	if (!regInfo.Write(_T(""), strMenuName))
	{
		return FALSE;
	}

	if (!regInfo.VerifyKey(strRegKey + _T("\\Shell\\") + strRegName + _T("\\command")))
	{
		if (!regInfo.CreateKey(strRegKey + _T("\\Shell\\") + strRegName + _T("\\command")))
		{
			return FALSE;
		}
	}

	CString strRegCommand = _T("\"") + strCmdFile + _T("\"") + _T(" \"") + _T("%1") + _T("\"");
	if (!regInfo.Write(_T(""), strRegCommand))
	{
		return FALSE;
	}

	return TRUE;
}

/*===================================================================
* ����:		GetAppFullName
* ����:
* ����:		��ȡ������(��������·��)
* ˵��:		����ֵ��Ϊ������
* ʱ��:		2007/2/25
* ����:		���໪
====================================================================*/
CString CAutoDecDlg::GetAppFullName()
{
	HMODULE module = GetModuleHandle(NULL);
	
	TCHAR buf[MAX_PATH] = {0};
	GetModuleFileName(module, buf, sizeof(buf));
	CString strAppName = CString(buf);
	
	return strAppName;
}

/*===================================================================
* ����:		UnWinIntegrated
* ����:		strRegName �˵���ע����е�Ŀ¼����
* ����:		ж������Ҽ����ɲ˵�
* ˵��:		ִ�гɹ�����TRUE�����򷵻�FALSE
* ʱ��:		2007/2/25
* ����:		���໪
====================================================================*/
BOOL CAutoDecDlg::UnWinIntegrated(CString strRegName)
{
	if (SHDeleteKey(HKEY_CLASSES_ROOT, _T("Folder\\Shell\\") + strRegName) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

/*===================================================================
* ����:		UnWinIntegrated
* ����:		strRegName �˵���ע����е�Ŀ¼����
			strFileExt Ҫж�ؼ��ɲ˵����ļ���չ��(�������)
* ����:		ж������Ҽ����ɲ˵�
* ˵��:		ִ�гɹ�����TRUE�����򷵻�FALSE
* ʱ��:		2007/2/25
* ����:		���໪
====================================================================*/
BOOL CAutoDecDlg::UnWinIntegrated(CString strRegName, CString strFileExt)
{
	CString strRegKey;
	CReg regInfo(HKEY_CLASSES_ROOT);

	if (!regInfo.VerifyKey(_T(".") + strFileExt))
	{
		return FALSE;
	}
	
	if (!regInfo.Read(_T(""), strRegKey))
	{
		return FALSE;
	}
	
	if (SHDeleteKey(HKEY_CLASSES_ROOT, strRegKey + _T("\\Shell\\") + strRegName) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAutoDecDlg::CheckInterStatus()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString m_strAutoDecA3X = _T("Autoit3 A3X Decompiler");
	CString m_strAutoDecEXE = _T("Autoit3 EXE Decompiler");
	CString m_strAutoDecPath = _T("Autoit3 PATH Decompiler");

	BOOL bFolderIntegrated = FALSE;
	BOOL bExeIntegrated = FALSE;
	BOOL bA3XIntegrated = FALSE;

	CString strRegKey(_T(""));
	CReg regInfo(HKEY_CLASSES_ROOT);

	//����Ƿ񼯳ɵ�Ŀ¼�Ҽ��˵�
	bFolderIntegrated = regInfo.VerifyKey(_T("Folder\\Shell\\") + m_strAutoDecPath + _T("\\command"));

	//����Ƿ񼯳ɵ�*.exe�ļ��Ҽ��˵�
	if (regInfo.VerifyKey(_T(".exe")))
	{
		regInfo.Read(_T(""), strRegKey);
		bExeIntegrated = regInfo.VerifyKey(strRegKey + _T("\\Shell\\") + m_strAutoDecEXE + _T("\\command"));
		strRegKey.Empty();
	}

	//����Ƿ񼯳ɵ�*.a3x�ļ��Ҽ��˵�
	if (regInfo.VerifyKey(_T(".a3x")))
	{
		regInfo.Read(_T(""), strRegKey);
		bA3XIntegrated = regInfo.VerifyKey(strRegKey + _T("\\Shell\\") + m_strAutoDecA3X + _T("\\command"));
		strRegKey.Empty();
	}

	return (bExeIntegrated && bA3XIntegrated && bFolderIntegrated);
}

void CAutoDecDlg::OnBnClickedBtnAddtomenu()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString m_strAutoDecA3X = _T("Autoit3 A3X Decompiler");
	CString m_strAutoDecEXE = _T("Autoit3 EXE Decompiler");
	CString m_strAutoDecPath = _T("Autoit3 PATH Decompiler");

	if (CheckInterStatus())
	{
		UnWinIntegrated(m_strAutoDecPath);
		UnWinIntegrated(m_strAutoDecEXE, _T("exe"));
		UnWinIntegrated(m_strAutoDecA3X, _T("a3x"));
	}
	else
	{
		BOOL bPath = WinIntegrated(m_strAutoDecPath, _T("Autoit3 Decompile"), GetAppFullName());
		BOOL bExe = WinIntegrated(m_strAutoDecEXE, _T("Autoit3 Decompile"), GetAppFullName(), _T("exe"), _T("Ӧ�ó���"));
		BOOL bA3X = WinIntegrated(m_strAutoDecA3X, _T("Autoit3 Decompile"), GetAppFullName(), _T("a3x"), _T("Autoit3��������"));
		if (!bPath || !bExe || !bA3X)
		{
			AfxMessageBox(_T("�˵�����ʧ�ܣ������Ƿ�������Ȩ�ޣ�"), MB_OK | MB_ICONSTOP);
		}
	}

	if (!CheckInterStatus())
	{
		GetDlgItem(IDC_BTN_ADDTOMENU)->SetWindowText(_T("�Ҽ�����"));
	}
	else
	{
		GetDlgItem(IDC_BTN_ADDTOMENU)->SetWindowText(_T("ȡ���Ҽ�"));
	}
}

void CAutoDecDlg::DoPreview(CAutoDecDlg *pThis, BYTE *lpBuffer, DWORD dwSize, BOOL bUnicode)
{
	//����UNICODE�����ֽڱ�ʶ
	BYTE *lpNewPtr = new BYTE[dwSize + 2];
	ZeroMemory(lpNewPtr, dwSize + 2);
	CopyMemory(lpNewPtr, lpBuffer, dwSize);

	BYTE *lpTextBuf = lpNewPtr;
	if (bUnicode)
	{
		lpTextBuf += 2;
		::SetDlgItemTextW(pThis->m_hWnd, IDC_EDIT_PREVIEW, (WCHAR *)lpTextBuf);
	}
	else
	{
		::SetDlgItemTextA(pThis->m_hWnd, IDC_EDIT_PREVIEW, (CHAR *)lpTextBuf);
	}

	delete[] lpNewPtr;
}

BOOL CAutoDecDlg::ProcessFile(LPCTSTR lpszSrcFile, LPCTSTR lpszDstPath, BOOL bPreview, int nItem)
{
	HANDLE hFileAuto = CreateFile(lpszSrcFile, 
		GENERIC_READ, 
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if (INVALID_HANDLE_VALUE == hFileAuto)
	{
		return FALSE;
	}

	DWORD dwFileTotalSize = GetFileSize(hFileAuto, NULL);
	BYTE *lpFileBuf = new BYTE[dwFileTotalSize];

	DWORD dwRead = 0;
	BOOL bRet = ReadFile(hFileAuto, lpFileBuf, dwFileTotalSize, &dwRead, NULL);
	CloseHandle(hFileAuto);
	if (!bRet || dwFileTotalSize != dwRead)
	{
		delete[] lpFileBuf;
		return FALSE;
	}

	bRet = FALSE;
	HANDLE hAutoHandle = AUTOIT_Init(lpFileBuf, dwFileTotalSize);
	if (NULL != hAutoHandle)
	{
		CALLBACK_CONTENT_INGUI content = {0};
		content.bPreview = bPreview;
		content.pThis = this;
		content.lpszSavePath = lpszDstPath;
		content.nItem = nItem;
		bRet = AUTOIT_DoDecompile(hAutoHandle, &CAutoDecDlg::ProcessDecompiledData, (DWORD)&content);
		AUTOIT_UnInit(hAutoHandle);
	}

	delete[] lpFileBuf;

	return bRet;
}

void CAutoDecDlg::OnBnClickedBtnSelall()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nItemCount = m_ctlInputFileList.GetItemCount();
	if (0 == nItemCount)
	{
		return;
	}

	for (int nItem=0; nItem<nItemCount; nItem++)
	{
		m_ctlInputFileList.SetCheck(nItem, TRUE);
	}
}

void CAutoDecDlg::OnBnClickedBtnSelrev()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL bCheckStat = FALSE;
	int nItemCount = m_ctlInputFileList.GetItemCount();
	if (0 == nItemCount)
	{
		return;
	}

	for (int nItem=0; nItem<nItemCount; nItem++)
	{
		bCheckStat = !m_ctlInputFileList.GetCheck(nItem);
		m_ctlInputFileList.SetCheck(nItem, bCheckStat);
	}
}

void CAutoDecDlg::OnBnClickedBtnClean()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL bCheckStat = FALSE;
	int nItemCount = m_ctlInputFileList.GetItemCount();
	if (0 == nItemCount)
	{
		return;
	}

	for (int nItem=0; nItem<nItemCount; nItem++)
	{
		AUTOIT3_DECINFORMATIONS *lpInfoPtr = (AUTOIT3_DECINFORMATIONS *)m_ctlInputFileList.GetItemData(nItem);
		if (NULL != lpInfoPtr)
		{
			delete lpInfoPtr;
			m_ctlInputFileList.SetItemData(nItem, NULL);
		}
	}

	m_ctlInputFileList.DeleteAllItems();

	nItemCount = m_ctlDecompiledFileList.GetItemCount();
	for (int nItem=0; nItem<nItemCount; nItem++)
	{
		DECOMPILED_FILEBUFFER_INFO *lpInfo = (DECOMPILED_FILEBUFFER_INFO *)m_ctlDecompiledFileList.GetItemData(nItem);
		if (NULL != lpInfo)
		{
			if (NULL != lpInfo->lpBuffer)
			{
				delete[] lpInfo->lpBuffer;
			}

			delete lpInfo;
			m_ctlDecompiledFileList.SetItemData(nItem, NULL);
		}
	}

	m_ctlDecompiledFileList.DeleteAllItems();

	SetButtonStatus();
}

void CAutoDecDlg::OnNMClickListFileinfos(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	//OnBnClickedBtnPreview();
}

void CAutoDecDlg::OnNMDblclkListFileinfos(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	
	//˫��Ԥ��
	/*˫��ʱ���ô�����
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	POSITION pos = m_ctlInputFileList.GetFirstSelectedItemPosition();
	int nItem = m_ctlInputFileList.GetNextSelectedItem(pos);
	if (0xFFFFFFFF == nItem)
	{
		return;
	}

	CString strFilePath = m_ctlInputFileList.GetItemText(nItem, FILEINFOS_FILEPATH);
	if (strFilePath.IsEmpty())
	{
		return;
	}

	CString strCmd;
	strCmd.Format(_T("explorer /n, /select, %s"), strFilePath);

	TCHAR szCommand[MAX_PATH + 20] = {0};
	lstrcpy(szCommand, strCmd);

	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	BOOL bRet = CreateProcess(NULL, szCommand, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (bRet)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	*/
}

void CAutoDecDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	OnBnClickedBtnClean();
}

void CAutoDecDlg::DoDecompilePreview()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SetDlgItemText(IDC_EDIT_PREVIEW, _T(""));

	POSITION pos = m_ctlInputFileList.GetFirstSelectedItemPosition();
	int nItem = m_ctlInputFileList.GetNextSelectedItem(pos);
	if (0xFFFFFFFF == nItem)
	{
		return;
	}

	CString strFilePath = m_ctlInputFileList.GetItemText(nItem, FILEINFOS_FILEPATH);
	if (strFilePath.IsEmpty())
	{
		return;
	}

	BOOL bRet = ProcessFile(strFilePath, NULL, TRUE, nItem);
	if (!bRet)
	{
		SetDlgItemText(IDC_EDIT_PREVIEW, _T("������ʧ��,�޷�Ԥ��!"));
	}
}

int CAutoDecDlg::GetFileIconIndex(LPCTSTR lpszPath)
{
	DWORD dwValue = FILE_ATTRIBUTE_NORMAL;

	DWORD dwAttrib = ::GetFileAttributes(lpszPath);
	if (INVALID_FILE_ATTRIBUTES != dwAttrib)
	{
		if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
		{
			dwValue = FILE_ATTRIBUTE_DIRECTORY;
		}
	}

	SHFILEINFO sfi = {0};
	SHGetFileInfo(lpszPath, 
		dwValue, 
		&sfi, 
		sizeof(sfi), 
		SHGFI_LARGEICON | SHGFI_SYSICONINDEX |
		SHGFI_USEFILEATTRIBUTES | SHGFI_OPENICON);

	return sfi.iIcon;
}

//�������������,DEMO�������ֻ�����ű��ļ�
BOOL CAutoDecDlg::ProcessDecompiledData(AUTOIT3_DECINFORMATIONS *lpDecInfos, DWORD dwContent)
{
	//�ص�������
	CALLBACK_CONTENT_INGUI *lpContent = (CALLBACK_CONTENT_INGUI *)dwContent;

	//����ؼ����ļ�ʶ����Ϣ����
	if (NULL != lpContent->pThis->m_hWnd)
	{
		UINT nItem = lpContent->nItem;
		CListCtrl *lpctlFileList = &(lpContent->pThis->m_ctlInputFileList);

		CString strTextInfo;

		//������ģʽ�Ŵ���
		if (!lpContent->bPreview)
		{
			//�ɹ�״̬
			strTextInfo.Format(_T("%s"), _T("�ɹ�"));
			lpctlFileList->SetItemText(nItem, FILEINFOS_STATUS, strTextInfo);
		}

		//�ļ���ʽ
		strTextInfo.Format(_T("%s"), lpDecInfos->bA3XFormat ? _T("A3X") : _T("EXE"));
		lpctlFileList->SetItemText(nItem, FILEINFOS_FILEFORMAT, strTextInfo);

		//�������汾
		USES_CONVERSION;
		CString strVer = A2W(lpDecInfos->szAutoitVer); 
		strTextInfo.Format(_T("%s"), strVer);
		lpctlFileList->SetItemText(nItem, FILEINFOS_VERINFO, strTextInfo);

		CString strPwd = A2W(lpDecInfos->szPassword); 
		strTextInfo.Format(_T("%s"), strPwd);
		lpctlFileList->SetItemText(nItem, FILEINFOS_PASSWORD, strTextInfo);

		strTextInfo.Format(_T("%s"), lpDecInfos->bScriptUnicode ? _T("UNICODE") : _T("ANSI"));
		lpctlFileList->SetItemText(nItem, FILEINFOS_UNICODE, strTextInfo);

		strTextInfo.Format(_T("%s"), lpDecInfos->bX64 ? _T("X64") : _T("X86"));
		lpctlFileList->SetItemText(nItem, FILEINFOS_X64PLATFORM, strTextInfo);
	}

	if (lpContent->pThis->m_bDetect)
	{
		return TRUE;
	}

	//ģʽ�ж�
	if (lpContent->bPreview)
	{
		USES_CONVERSION;
		CString strReleasePath;
		if (lpDecInfos->bPathUnicode)
		{
			strReleasePath = (WCHAR *)lpDecInfos->szReleasePath;	
		}
		else
		{
			strReleasePath = A2W((CHAR *)lpDecInfos->szReleasePath); 
		}

		//Ԥ��ģʽ,��������ļ��б���ʾ��Ϣ
		CListCtrl *lpctlDecompiledFileList = &(lpContent->pThis->m_ctlDecompiledFileList);
		if (NULL != lpctlDecompiledFileList->m_hWnd)
		{
			int nIcon = GetFileIconIndex(strReleasePath);
			if (lpDecInfos->bScript)
			{
				nIcon = GetFileIconIndex(_T("script.au3"));
			}

			lpctlDecompiledFileList->InsertItem(lpDecInfos->dwFileIndex, NULL, nIcon);

			CString strIndex;
#if 1
			lpctlDecompiledFileList->SetItemText(lpDecInfos->dwFileIndex, 0, strReleasePath);

			strIndex.Format(_T("%04d"), lpDecInfos->dwFileIndex);
			lpctlDecompiledFileList->SetItemText(lpDecInfos->dwFileIndex, 1, strIndex);

			CString strSizeInfo;
			strSizeInfo.Format(_T("%d"), lpDecInfos->dwResultSize);
			lpctlDecompiledFileList->SetItemText(lpDecInfos->dwFileIndex, 2, strSizeInfo);
#else
			//���÷��������ļ�·����Ϣ
			lpctlDecompiledFileList->SetItemText(lpDecInfos->dwFileIndex, 0, strReleasePath);
#endif
			//������COPY�������б���
			DECOMPILED_FILEBUFFER_INFO *lpInfo = new DECOMPILED_FILEBUFFER_INFO;
			ZeroMemory(lpInfo, sizeof(DECOMPILED_FILEBUFFER_INFO));
			lpInfo->bUnicode = lpDecInfos->bScriptUnicode;
			lpInfo->bScript = lpDecInfos->bScript;
			lpInfo->dwSize = lpDecInfos->dwResultSize;
			lpInfo->lpBuffer = NULL;

			lpInfo->lpBuffer = new BYTE[lpInfo->dwSize + 2];
			ZeroMemory(lpInfo->lpBuffer, lpInfo->dwSize + 2);
			CopyMemory(lpInfo->lpBuffer, lpDecInfos->lpResultBuf, lpInfo->dwSize);
			lpctlDecompiledFileList->SetItemData(lpDecInfos->dwFileIndex, (DWORD)lpInfo);
		}
	}
	else
	{
		//���Ŀ¼�Ƿ����
		if (!PathFileExists(lpContent->lpszSavePath))
		{
			//�����洢Ŀ¼
			int nResult = SHCreateDirectoryEx(NULL, lpContent->lpszSavePath, NULL);
			if (ERROR_SUCCESS != nResult)
			{
				return FALSE;
			}
		}

		//�ű��Ĵ���
		if (lpDecInfos->bScript)
		{
			//д����ļ���
			CString strFileName;
			strFileName.Format(_T("%s\\%04d.au3"), lpContent->lpszSavePath, lpDecInfos->dwFileIndex);

			//������������ļ�
			HANDLE hFileOut = CreateFile(strFileName, 
				GENERIC_WRITE, 
				FILE_SHARE_WRITE, 
				NULL, 
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
			if (INVALID_HANDLE_VALUE != hFileOut)
			{
				DWORD dwWrite = 0;
				BOOL bRet = WriteFile(hFileOut, lpDecInfos->lpResultBuf, lpDecInfos->dwResultSize, &dwWrite, NULL);
				if (!bRet || lpDecInfos->dwResultSize != dwWrite)
				{
					DeleteFile(strFileName);
				}

				CloseHandle(hFileOut);
			}
		}
		else
		{
			//�ǽű��Ķ����ƴ���
			//д����ļ���
			CString strFileName;
			strFileName.Format(_T("%s\\%04d.bin"), lpContent->lpszSavePath, lpDecInfos->dwFileIndex);

			//������ļ�
			HANDLE hFileOut = CreateFile(strFileName, 
				GENERIC_WRITE, 
				FILE_SHARE_WRITE, 
				NULL, 
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
			if (INVALID_HANDLE_VALUE != hFileOut)
			{
				DWORD dwWrite = 0;
				BOOL bRet = WriteFile(hFileOut, lpDecInfos->lpResultBuf, lpDecInfos->dwResultSize, &dwWrite, NULL);
				if (!bRet || lpDecInfos->dwResultSize != dwWrite)
				{
					DeleteFile(lpContent->lpszSavePath);
				}

				CloseHandle(hFileOut);
			}
		}

		//��������Ϣ�ļ��Ĵ���
		CString strInfoFile;
		strInfoFile.Format(_T("%s\\%04d_decinfo.txt"), lpContent->lpszSavePath, lpDecInfos->dwFileIndex);
		HANDLE hInfoFile = CreateFile(strInfoFile, 
			GENERIC_WRITE, 
			FILE_SHARE_WRITE, 
			NULL, 
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
		if (INVALID_HANDLE_VALUE != hInfoFile)
		{
			DWORD dwWrite = 0;
			DWORD dwPathByteSize = 0;
			if (lpDecInfos->bPathUnicode)
			{
				BYTE cbHeader[2] = {0xFF, 0xFE};
				WriteFile(hInfoFile, cbHeader, 2, &dwWrite, NULL);

				WCHAR *lpInfoBeginW = L"Release path in autoit3:\r\n";
				WriteFile(hInfoFile, lpInfoBeginW, lstrlenW(lpInfoBeginW)*sizeof(WCHAR), &dwWrite, NULL);

				WCHAR *lpPathW = (WCHAR *)lpDecInfos->szReleasePath;
				dwPathByteSize = lstrlenW(lpPathW) * sizeof(WCHAR);
			}
			else
			{
				CHAR *lpInfoBeginA = "Release path in autoit3:\r\n";
				WriteFile(hInfoFile, lpInfoBeginA, lstrlenA(lpInfoBeginA)*sizeof(CHAR), &dwWrite, NULL);

				CHAR *lpPathA = (CHAR *)lpDecInfos->szReleasePath;
				dwPathByteSize = lstrlenA((lpPathA)) * sizeof(CHAR);
			}

			BOOL bRet = WriteFile(hInfoFile, lpDecInfos->szReleasePath, dwPathByteSize, &dwWrite, NULL);
			if (!bRet || dwPathByteSize != dwWrite)
			{
				DeleteFile(strInfoFile);
			}

			CloseHandle(hInfoFile);
		}
	}

	return TRUE;
}

void CAutoDecDlg::OnBnClickedBtnAbout()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CAboutRetailDlg dlg;
	dlg.DoModal();
}

void CAutoDecDlg::OnBnClickedBtnDeccurrent()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	POSITION pos = m_ctlInputFileList.GetFirstSelectedItemPosition();
	int nItem = m_ctlInputFileList.GetNextSelectedItem(pos);
	if (0xFFFFFFFF == nItem)
	{
		return;
	}

	CString strSrcFile = m_ctlInputFileList.GetItemText(nItem, FILEINFOS_FILEPATH);
	if (strSrcFile.IsEmpty())
	{
		return;
	}

	//�ļ����ָ�
	TCHAR szDir[_MAX_DIR] = {0};
	TCHAR szDrive[_MAX_DRIVE] = {0};
	TCHAR szName[_MAX_FNAME] = {0};
	TCHAR szExt[_MAX_EXT] = {0};
	_tsplitpath(strSrcFile, szDrive, szDir, szName, szExt);

	CString strDstPath;
	strDstPath.Format(_T("%s%s%s%s_dec"), szDrive, szDir, szName, szExt);

	BOOL bRet = ProcessFile(strSrcFile, strDstPath, FALSE, nItem);
	if (!bRet)
	{
		//�����б�ؼ�
		CString strTextInfo;
		strTextInfo.Format(_T("%s"), _T("ʧ��"));
		m_ctlInputFileList.SetItemText(nItem, FILEINFOS_STATUS, strTextInfo);
	}
}

void CAutoDecDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

void CAutoDecDlg::OnBnClickedBtnDelselect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nLastItem = 0;
	do 
	{
		int nItemCount = m_ctlInputFileList.GetItemCount();
		if (0 == nItemCount)
		{
			break;
		}

		BOOL bCheckStat = FALSE;
		for (int nItem=0; nItem<nItemCount; nItem++)
		{
			bCheckStat = m_ctlInputFileList.GetCheck(nItem);
			if (bCheckStat)
			{
				m_ctlInputFileList.DeleteItem(nItem);
				if (nItem != 0)
				{
					nLastItem = nItem - 1;
				}

				break;
			}
		}

		if (!bCheckStat)
		{
			break;
		}
	} while (TRUE);

	m_ctlInputFileList.SetItemState(nLastItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	SetButtonStatus();
}

void CAutoDecDlg::SetButtonStatus()
{
	BOOL bStatus =  (0 != m_ctlInputFileList.GetItemCount());
	GetDlgItem(IDC_BTN_DECOMPILE)->EnableWindow(bStatus);
	//GetDlgItem(IDC_BTN_DECCURRENT)->EnableWindow(bStatus);
	GetDlgItem(IDC_BTN_SELALL)->EnableWindow(bStatus);
	GetDlgItem(IDC_BTN_SELREV)->EnableWindow(bStatus);
	//GetDlgItem(IDC_BTN_DELSELECT)->EnableWindow(bStatus);
	GetDlgItem(IDC_BTN_CLEAN)->EnableWindow(bStatus);
	GetDlgItem(IDC_BTN_DELCURRENT)->EnableWindow(bStatus);
}

void CAutoDecDlg::OnBnClickedBtnDelcurrent()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	POSITION pos = m_ctlInputFileList.GetFirstSelectedItemPosition();
	int nItem = m_ctlInputFileList.GetNextSelectedItem(pos);
	if (0xFFFFFFFF == nItem)
	{
		return;
	}

	AUTOIT3_DECINFORMATIONS *lpInfoPtr = (AUTOIT3_DECINFORMATIONS *)m_ctlInputFileList.GetItemData(nItem);
	if (NULL != lpInfoPtr)
	{
		delete lpInfoPtr;
		m_ctlInputFileList.SetItemData(nItem, NULL);
	}

	m_ctlInputFileList.DeleteItem(nItem);
	if (nItem != 0)
	{
		nItem = nItem - 1;
	}
	m_ctlInputFileList.SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	SetButtonStatus();
}

void CAutoDecDlg::OnLvnItemchangedListFileinfos(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
	SetDlgItemText(IDC_EDIT_PREVIEW, _T(""));

	if ((pNMLV->uOldState & LVIS_SELECTED) && !(pNMLV->uNewState & LVIS_SELECTED))
	{
		//ʧȥ����
		int nItemCount = m_ctlDecompiledFileList.GetItemCount();
		for (int nItem=0; nItem<nItemCount; nItem++)
		{
			DECOMPILED_FILEBUFFER_INFO *lpInfo = (DECOMPILED_FILEBUFFER_INFO *)m_ctlDecompiledFileList.GetItemData(nItem);
			if (NULL != lpInfo)
			{
				if (NULL != lpInfo->lpBuffer)
				{
					delete[] lpInfo->lpBuffer;
				}

				delete lpInfo;
				m_ctlDecompiledFileList.SetItemData(nItem, NULL);
			}
		}

		m_ctlDecompiledFileList.DeleteAllItems();
		GetDlgItem(IDC_BTN_DECCURRENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_DELSELECT)->EnableWindow(FALSE);
	}
	else if (!(pNMLV->uOldState & LVIS_SELECTED) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		//��ý���
		GetDlgItem(IDC_BTN_DECCURRENT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_DELSELECT)->EnableWindow(TRUE);

		int nItem = pNMLV->iItem;
		CString strFilePath = m_ctlInputFileList.GetItemText(nItem, FILEINFOS_FILEPATH);
		if (strFilePath.IsEmpty())
		{
			return;
		}

		BOOL bRet = ProcessFile(strFilePath, NULL, TRUE, nItem);
		if (!bRet)
		{
			SetDlgItemText(IDC_EDIT_PREVIEW, _T("������ʧ��,�޷�Ԥ��!"));
		}
	}
}

void CAutoDecDlg::OnLvnItemchangedListFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
	SetDlgItemText(IDC_EDIT_PREVIEW, _T(""));
	
	int nItem = pNMLV->iItem;
	if (!(pNMLV->uOldState & LVIS_SELECTED) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		//��ý���
		DECOMPILED_FILEBUFFER_INFO *lpInfo = (DECOMPILED_FILEBUFFER_INFO *)m_ctlDecompiledFileList.GetItemData(nItem);
		if (NULL != lpInfo)
		{
			if (lpInfo->bScript)
			{
				//�ű�Ԥ������
				BYTE *lpTextBuf = lpInfo->lpBuffer;
				if (NULL != lpTextBuf)
				{
					if (lpInfo->bUnicode)
					{
						//����UNICODE�����ֽڱ�ʶ
						lpTextBuf += 2;
						::SetDlgItemTextW(m_hWnd, IDC_EDIT_PREVIEW, (WCHAR *)lpTextBuf);
					}
					else
					{
						::SetDlgItemTextA(m_hWnd, IDC_EDIT_PREVIEW, (CHAR *)lpTextBuf);
					}
				}
			}
			else
			{
				//�ļ�Ԥ������
				//HEX��ʾ�ļ�����
				if (NULL != lpInfo->lpBuffer)
				{
					TCHAR *lpBufFileText = new TCHAR[lpInfo->dwSize * 8];
					ZeroMemory(lpBufFileText, lpInfo->dwSize * 8);
					ConvertHex2String(lpInfo->lpBuffer, lpInfo->dwSize, lpBufFileText, FALSE);
					SetDlgItemText(IDC_EDIT_PREVIEW, lpBufFileText);
					delete[] lpBufFileText;
				}
			}
		}
	}
}