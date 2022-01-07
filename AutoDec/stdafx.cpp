// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// AutoDec.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"


//ת��ʮ�����ƵĻ���Ϊ�ַ�����ʽ
void WINAPI ConvertHex2String(BYTE *lpInBuffer, int nInSize, LPTSTR lpOutString, BOOL bOneLine)
{
	int nSrcPos = 0;
	int nDstPos = 0; 
	while (nSrcPos < nInSize)
	{
		TCHAR szTmpString[4] = {0};
		::wsprintf(szTmpString, _T("%02X "), *(BYTE *)(lpInBuffer + nSrcPos));
		::RtlCopyMemory(lpOutString + nDstPos, szTmpString, 4*sizeof(TCHAR));
		nDstPos += 3;
		nSrcPos++;

		if (!bOneLine)
		{
			if (0 == nSrcPos % 16)
			{
				lpOutString[nDstPos++] = 0x0D;
				lpOutString[nDstPos++] = 0x0A;
			}
		}
	}

	int nLen = ::lstrlen(lpOutString);
	if (0 == nLen)
	{
		return;
	}

	if ('\n' == lpOutString[nLen-1])
	{
		lpOutString[nLen-1] = 0;

		if (nLen < 2)
		{
			return;
		}

		if (' ' == lpOutString[nLen-2])
		{
			lpOutString[nLen-2] = 0;
		}
	}
	else if (' ' == lpOutString[nLen-1])
	{
		lpOutString[nLen-1] = 0;
	}
}