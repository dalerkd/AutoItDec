#pragma once


// CAboutRetailDlg �Ի���

class CAboutRetailDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutRetailDlg)

public:
	CAboutRetailDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAboutRetailDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX_RETAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
