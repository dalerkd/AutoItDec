// AutoDecDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


// CAutoDecDlg �Ի���
class CAutoDecDlg : public CDialog
{
// ����
public:
	CAutoDecDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AUTODEC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void CAutoDecDlg::InitInputFileListCtl();
	void CAutoDecDlg::InitDecompiledFileListCtl();
	afx_msg void OnDropFiles(HDROP hDropInfo);

	DWORD AddFilesByPath(LPCTSTR lpszFileDir, BOOL bIncludeSub, LPCTSTR lpszFileExt);
	DWORD AddFilesByFileName(LPCTSTR lpszFileName);

private:
	enum
	{
		FILEINFOS_INDEX		= 0,	//�б�����
		FILEINFOS_STATUS,			//����״̬
		FILEINFOS_FILEPATH,			//�ļ�·��

		FILEINFOS_FILEFORMAT,		//�ļ���ʽ(�Ƿ������A3X����)
		FILEINFOS_VERINFO,			//�汾��Ϣ(��һ����)
		FILEINFOS_PASSWORD,			//��������(��һ����)
		FILEINFOS_UNICODE,			//UNICODE��ʶ
		FILEINFOS_X64PLATFORM,		//64λƽ̨
	};

	CListCtrl m_ctlInputFileList;
	CListCtrl m_ctlDecompiledFileList;

	BOOL m_bDetect;

public:
	typedef struct _CALLBACK_CONTENT_INGUI
	{
		CAutoDecDlg *pThis;
		BOOL bPreview;
		LPCTSTR lpszSavePath;
		int nItem;
	}CALLBACK_CONTENT_INGUI, *PCALLBACK_CONTENT_INGUI;

	afx_msg void OnBnClickedBtnDecompile();
	afx_msg void OnBnClickedBtnAddtomenu();
	
	afx_msg void OnBnClickedBtnSelall();
	afx_msg void OnBnClickedBtnSelrev();
	afx_msg void OnBnClickedBtnClean();

	//������󱣴浽����ؼ�����Ϣ
	typedef struct _DECOMPILED_FILEBUFFER_INFO
	{
		BOOL bScript;	//�Ƿ�ű�
		BOOL bUnicode;	//Ϊ�ű�ʱ�Ƿ�UNICODE����
		BYTE *lpBuffer;	//����ָ��
		DWORD dwSize;	//�����С
	}DECOMPILED_FILEBUFFER_INFO, *PDECOMPILED_FILEBUFFER_INFO;

	static BOOL ProcessDecompiledData(AUTOIT3_DECINFORMATIONS *lpDecInfos, DWORD dwContent);
	static void DoPreview(CAutoDecDlg *pThis, BYTE *lpBuffer, DWORD dwSize, BOOL bUnicode);

	typedef void (*PREVIEW_CALLFUN)(CAutoDecDlg *pThis, BYTE *lpBuffer, DWORD dwSize, BOOL bUnicode);
	typedef struct _PREVIEW_CALLCONTENT 
	{
		DWORD dwContent;
		PREVIEW_CALLFUN lpfnCallFun;
	}PREVIEW_CALLCONTENT, *PPREVIEW_CALLCONTENT;

	static int GetFileIconIndex(LPCTSTR lpszPath);

	//���ﴫ�ݵ��Ǳ���Ŀ¼
	BOOL ProcessFile(LPCTSTR lpszSrcFile, LPCTSTR lpszDstPath, BOOL bPreview, int nItem);

	void SetButtonStatus();
	BOOL CheckInterStatus();	//��⼯��״̬
	CString GetAppFullName();	//��ǰʵ������·��

	//ж��WINDOWS�˵�����
	BOOL UnWinIntegrated(CString strRegName);	
	BOOL UnWinIntegrated(CString strRegName, CString strFileExt);

	//WINDOWS�˵�����
	BOOL WinIntegrated(CString strRegName, CString strMenuName, CString strCmdFile);
	BOOL WinIntegrated(CString strRegName, CString strMenuName, CString strCmdFile, CString strFileExt, CString strFileTypeDetail);
	afx_msg void OnNMClickListFileinfos(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListFileinfos(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	void DoDecompilePreview();
	afx_msg void OnBnClickedBtnAbout();
	afx_msg void OnBnClickedBtnDeccurrent();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnDelselect();
	afx_msg void OnBnClickedBtnDelcurrent();
	afx_msg void OnLvnItemchangedListFileinfos(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListFilelist(NMHDR *pNMHDR, LRESULT *pResult);
};
