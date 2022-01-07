#include "stdafx.h"
#include "reg.h"

/*================================================================ 
* ����:		CReg
* ����:		(HKEY hRootKey) 
* ����:		���캯��
* ����:
* ˵��:		������캯��������������ʹ��Ĭ�ϵĲ�����m_hRootKey����ʼ��
			ΪHKEY_CURRENT_USER, ������в����� m_hRootKeyΪָ����ֵ
================================================================*/ 
CReg::CReg(HKEY hRootKey) 
{
	m_hRootKey = hRootKey; 
}

CReg::~CReg() //�����������йرմ�ע�����
{
	Close();
}



/*================================================================ 
* ����:		VerifyKey
* ����:		(HKEY hRootKey, LPCTSTR pszPath) 
* ����:		�жϸ�����·���Ƿ���� (���д򿪵Ĺ���)
* ����:		BOOL
* ˵��:		�����һ������ΪNULL����ʹ��Ĭ�ϵĸ�·����
================================================================*/ 
BOOL CReg::VerifyKey (LPCTSTR pszPath)
{
 	LONG ReturnValue = ::RegOpenKeyEx (m_hRootKey, pszPath, 0L,	KEY_ALL_ACCESS, &m_hSubKey);
	return (ERROR_SUCCESS == ReturnValue);
}



/*================================================================ 
* ����:		VerifyValue
* ����:		(LPCTSTR pszValue)
* ����:		�жϸ����ļ��Ƿ����
* ����:		BOOL
* ˵��:		���ȵ���VerifyKey��Ȼ����ʹ�øú���
================================================================*/ 
BOOL CReg::VerifyValue (LPCTSTR pszValue)
{
	LONG lReturn = ::RegQueryValueEx(m_hSubKey, pszValue, NULL,	NULL, NULL, NULL);
	return (ERROR_SUCCESS == lReturn);
}



/*================================================================ 
* ����:		IsEqual
* ����:		(LPCTSTR pszValue,int nn)
* ����:		���ͼ���ֵ�Ƿ����ĳ��ֵ
* ����:		BOOL
* ˵��:
================================================================*/ 
BOOL CReg::IsEqual(LPCTSTR pszValue,int nn)
{
	int nTemp = 0;
	Read(pszValue,nTemp);

	return (nn == nTemp);
}

/*================================================================ 
* ����:		IsEqual
* ����:		(LPCTSTR pszValue,DWORD dw)
* ����:		�ж�DWORD����ֵ�Ƿ����ĳ��ֵ
* ����:		BOOL
* ˵��:
================================================================*/ 
BOOL CReg::IsEqual(LPCTSTR pszValue,DWORD dw)
{
	DWORD dwTemp = 0;
	Read(pszValue,dwTemp);

	return (dwTemp==dw);
}

/*================================================================ 
* ����:		IsEqual
* ����:		(LPCTSTR pszValue,LPCTSTR lpsz)
* ����:		�ж��ַ�������ֵ�Ƿ����ĳ��ֵ
* ����:		BOOL
* ˵��:
================================================================*/ 
BOOL CReg::IsEqual(LPCTSTR pszValue,LPCTSTR lpsz)
{
	CString str;
	Read(pszValue,str);
	
	return (0 == str.CompareNoCase(lpsz));
}



/*================================================================ 
* ����:		CreateKey
* ����:		(HKEY hRootKey, LPCTSTR pszPath)
* ����:		����·��
* ����:		BOOL
* ˵��:
================================================================*/ 
BOOL CReg::CreateKey (LPCTSTR pszPath)
{
	DWORD dw = 0;
	LONG ReturnValue = ::RegCreateKeyEx (m_hRootKey, pszPath, 0L, NULL,
							REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, 
							&m_hSubKey, &dw);

	return (ERROR_SUCCESS == ReturnValue);
}



/*================================================================ 
* ����:		Write
* ����:		(LPCTSTR lpszKeyName, int iVal)
* ����:		д������ֵ
* ����:		BOOL
* ˵��:
================================================================*/ 
BOOL CReg::Write (LPCTSTR lpszKeyName, int iVal)
{
	DWORD dwValue = (DWORD)iVal;
	LONG ReturnValue = ::RegSetValueEx (m_hSubKey, lpszKeyName, 0L, REG_DWORD,
		(CONST BYTE*) &dwValue, sizeof(DWORD));

	return (ERROR_SUCCESS == ReturnValue);
}

/*================================================================ 
* ����:		Write
* ����:		(LPCTSTR lpszKeyName, DWORD dwVal)
* ����:		д��DWORDֵ
* ����:		BOOL
* ˵��:
================================================================*/ 
BOOL CReg::Write (LPCTSTR lpszKeyName, DWORD dwVal)
{
	return ::RegSetValueEx (m_hSubKey, lpszKeyName, 0L, REG_DWORD,
		(CONST BYTE*) &dwVal, sizeof(DWORD));
}

/*================================================================ 
* ����:		Write
* ����:		(LPCTSTR lpszKeyName, LPCTSTR pszData)
* ����:		д���ַ���ֵ
* ����:		BOOL
* ˵��:
================================================================*/ 
BOOL CReg::Write (LPCTSTR lpszKeyName, LPCTSTR pszData)
{
	LONG ReturnValue = ::RegSetValueEx (m_hSubKey, lpszKeyName, 0L, REG_SZ,
		(BYTE*)pszData, sizeof(TCHAR)*(lstrlen(pszData) + 1));

	return (ERROR_SUCCESS == ReturnValue);
}



/*================================================================ 
* ����:		Read
* ����:		(LPCTSTR lpszKeyName, int& iVal) 
* ����:		��ȡ����
* ����:		BOOL
* ˵��:		��2������ͨ�����ô��ݣ������ں������޸�ʵ��
================================================================*/ 
BOOL CReg::Read(LPCTSTR lpszKeyName, int& iVal)
{
	DWORD dwType = 0;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwDest = 0;

	LONG lReturn = ::RegQueryValueEx(m_hSubKey, (LPTSTR)lpszKeyName, NULL,
		&dwType, (BYTE *) &dwDest, &dwSize);
	if (ERROR_SUCCESS == lReturn)
	{
		iVal = (int)dwDest;
		return TRUE;
	}

	return FALSE;
}

/*================================================================ 
* ����:		Read
* ����:		(LPCTSTR lpszKeyName, DWORD& dwVal) 
* ����:		��ȡDWORDֵ
* ����:		BOOL
* ˵��:		��2������ͨ�����ô��ݣ������ں������޸�ʵ��
================================================================*/ 
BOOL CReg::Read(LPCTSTR lpszKeyName, DWORD& dwVal)
{
	DWORD dwType = 0;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest = 0;

	LONG lReturn = ::RegQueryValueEx(m_hSubKey, (LPTSTR)lpszKeyName, NULL, 
		&dwType, (BYTE *) &dwDest, &dwSize);
	if (ERROR_SUCCESS == lReturn)
	{
		dwVal = dwDest;
		return TRUE;
	}

	return FALSE;
}

/*================================================================ 
* ����:		Read
* ����:		(LPCTSTR lpszKeyName, CString& sVal) 
* ����:		��ȡ�ַ���ֵ
* ����:		BOOL
* ˵��:		��2������ͨ�����ô��ݣ������ں������޸�ʵ��
================================================================*/ 
BOOL CReg::Read(LPCTSTR lpszKeyName, CString& sVal)
{
	DWORD dwType = 0;
	DWORD dwSize = 200;
	TCHAR szString[255] = {0};

	LONG lReturn = ::RegQueryValueEx(m_hSubKey, (LPTSTR) lpszKeyName, NULL,
		&dwType, (BYTE *)szString, &dwSize);
	if (ERROR_SUCCESS == lReturn)
	{
		sVal = szString;
		return TRUE;
	}

	return FALSE;
}



/*================================================================ 
* ����:		DeleteValue
* ����:		(LPCTSTR pszValue) 
* ����:		ɾ����
* ����:		BOOL
* ˵��:
================================================================*/ 
BOOL CReg::DeleteValue(LPCTSTR pszValue)
{
	return (ERROR_SUCCESS == ::RegDeleteValue(m_hSubKey, pszValue));	
}



/*================================================================ 
* ����:		DeleteKey
* ����:		(HKEY hRootKey, LPCTSTR pszPath) 
* ����:		ɾ��·��
* ����:		BOOL
* ˵��:
================================================================*/ 
BOOL CReg::DeleteKey(LPCTSTR pszPath)
{
	return (::RegDeleteKey(m_hRootKey, pszPath) == ERROR_SUCCESS);
}



/*================================================================ 
* ����:		Close
* ����:	
* ����:		�ر�ע���
* ����:
* ˵��:
================================================================*/ 
 void CReg::Close()
{
	if (m_hSubKey)
	{
		::RegCloseKey (m_hSubKey);
		m_hSubKey = NULL;
	}
}

