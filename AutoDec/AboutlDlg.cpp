// AboutlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoDec.h"
#include "AboutlDlg.h"


// CAboutRetailDlg �Ի���

IMPLEMENT_DYNAMIC(CAboutRetailDlg, CDialog)

CAboutRetailDlg::CAboutRetailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutRetailDlg::IDD, pParent)
{

}

CAboutRetailDlg::~CAboutRetailDlg()
{
}

void CAboutRetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutRetailDlg, CDialog)
END_MESSAGE_MAP()


// CAboutRetailDlg ��Ϣ�������

BOOL CAboutRetailDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString strVersion;
	strVersion = CString(_T("Build:")) + CString(__TIMESTAMP__);
	SetDlgItemText(IDC_STATIC_VERSION, strVersion);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
