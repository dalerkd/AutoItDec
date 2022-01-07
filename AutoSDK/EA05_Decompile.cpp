#include "StdAfx.h"
#include "EA05_Decompile.h"
#include "EANEW_UnCompress.h"

typedef struct _AUTOIT_EA05_DECRYPTHANDLE
{
	UINT *next;
	UINT items;
	UINT mt[624];
}AUTOIT_EA05_DECRYPTHANDLE, *PAUTOIT_EA05_DECRYPTHANDLE;

BYTE AUTOIT_EA05_GetNext(AUTOIT_EA05_DECRYPTHANDLE *lpHandle) 
{
	if (!--lpHandle->items) 
	{
		UINT *mt = lpHandle->mt;
		lpHandle->items = 624;
		lpHandle->next = mt;

		int i = 0;
		for (i=0; i<227; i++)
		{
			mt[i] = ((((mt[i] ^ mt[i+1])&0x7ffffffe)^mt[i])>>1)^((0-(mt[i+1]&1))&0x9908b0df)^mt[i+397];
		}

		for (; i<623; i++)
		{
			mt[i] = ((((mt[i] ^ mt[i+1])&0x7ffffffe)^mt[i])>>1)^((0-(mt[i+1]&1))&0x9908b0df)^mt[i-227];
		}

		mt[623] = ((((mt[623] ^ mt[0])&0x7ffffffe)^mt[623])>>1)^((0-(mt[0]&1))&0x9908b0df)^mt[i-227];
	}

	UINT r = *(lpHandle->next++);
	r ^= (r >> 11);
	r ^= ((r & 0xff3a58ad) << 7);
	r ^= ((r & 0xffffdf8c) << 15);
	r ^= (r >> 18);

	return (BYTE)(r >> 1);
}

void AUTOIT_EA05_DeCrypt(BYTE *lpBuffer, UINT nSize, UINT nSeed)
{
	AUTOIT_EA05_DECRYPTHANDLE hDecHandle = {0};
	UINT *mt = hDecHandle.mt;

	*mt = nSeed;
	for(int i=1; i<624; i++)
	{
		mt[i] = i + 0x6c078965*((mt[i-1]>>30) ^ mt[i-1]);
	}

	hDecHandle.items = 1;

	while (nSize--)
	{
		*lpBuffer++ ^= AUTOIT_EA05_GetNext(&hDecHandle);
	}
}

//�жϺ���
BOOL AUTOIT_EA05_IsEA05(AUTOIT3_DECOMPILER *lpDecHandle)
{
	//16�ֽ�GUID
	static const BYTE cbAutoGUID[] = 
	{
		0xA3, 0x48, 0x4B, 0xBE, 0x98, 0x6C, 0x4A, 0xA9, 
		0x99, 0x4C, 0x53, 0x0A, 0x86, 0xD6, 0x48, 0x7D
	};

	//����ָ��
	BYTE *lpDataPtr = lpDecHandle->lpDataPtr;

	//GUID��ʼ��ʶ�Ƚ�
	int nRet = memcmp(lpDataPtr, cbAutoGUID, 16);
	if (0 != nRet)
	{
		return FALSE;
	}
	lpDataPtr += 16;

	//ѹ���汾"AU3!EA05",ĩβҲ�������ʶ,����У��
	static const BYTE cbAutoVersion[] = 
	{
		0x41, 0x55, 0x33, 0x21, 0x45, 0x41, 0x30, 0x35
	};

	//ѹ���汾�Ƚ�
	nRet = memcmp(lpDataPtr, cbAutoVersion, sizeof(cbAutoVersion));
	if (0 != nRet)
	{
		return FALSE;
	}

	return TRUE;
}

//�����ļ�У���
DWORD AUTOIT_EA05_CalcCheckSum(BYTE *lpBuffer, DWORD dwSize)
{
	DWORD dwChkValue = 0;
	if ( dwSize > 0 )
	{
		DWORD dwValue = 1;
		UINT nTmpValue1 = (WORD)dwValue;
		UINT nTmpValue2 = dwValue >> 16;

		DWORD dwIndex = 0;
		do
		{
			nTmpValue1 = (nTmpValue1 + lpBuffer[dwIndex++]) % 0xFFF1;
			nTmpValue2 = (nTmpValue1 + nTmpValue2) % 0xFFF1;
		}
		while (dwIndex < dwSize);

		dwChkValue = nTmpValue1 + (nTmpValue2 << 16);
	}

	return dwChkValue;
}

BOOL AUTOIT_EA05_Decompile(AUTOIT3_DECOMPILER *lpDecHandle, DECOMPILE_CALLBACK lpfnDecCallback, DWORD dwContent)
{
	//����16�ֽ�GUID��8�ֽڰ汾��ʶ
	BYTE *lpDataBufPtr = lpDecHandle->lpDataPtr + (16 + 8);

	//16�ֽڵ���������,���ڼ����������(�Ӹð汾��ʼ�Ѿ������ùٷ����߷�������)
	DWORD dwPwdSeed = 0;
	for (int i=0; i<16; i++)
	{
		dwPwdSeed += lpDataBufPtr[i];
	}
	lpDataBufPtr += 16;

	//����ѭ���������ݽ��ܴ���,�ű�����λ����ǰ��
	BOOL bRet = FALSE;
	DWORD dwFileCount = 0;
	do
	{
		//�����ļ���ʶ�ַ���"FILE"
		BYTE *lpFileFlagPtr = lpDataBufPtr;
		AUTOIT_EA05_DeCrypt(lpFileFlagPtr, 4, 0x16FA);
		lpDataBufPtr += 4;

		CHAR *lpFileFlag = "FILE";
		if (0 != memcmp(lpFileFlag, lpFileFlagPtr, 4))
		{
			break;
		}

		//�ļ����
		lpDecHandle->decInfos.dwFileIndex = dwFileCount;
		dwFileCount++;

		//�ű���ʶ������
		DWORD dwScriptFlagLen = *(DWORD *)lpDataBufPtr;
		dwScriptFlagLen ^= 0x29BC;
		lpDataBufPtr += 4;

		//���ܱ�ʶ�ַ���
		BYTE *lpFlagPtrScript = lpDataBufPtr;
		AUTOIT_EA05_DeCrypt(lpFlagPtrScript, dwScriptFlagLen, dwScriptFlagLen + 0xA25E);
		lpDataBufPtr += dwScriptFlagLen;

		//·�����ݳ���
		DWORD dwPathLen = *(DWORD *)lpDataBufPtr;
		dwPathLen ^= 0x29AC;
		lpDataBufPtr += 4;

		//·������(ANSI����)
		CHAR *lpPathPtr = (CHAR *)lpDataBufPtr;
		AUTOIT_EA05_DeCrypt((BYTE *)lpPathPtr, dwPathLen, dwPathLen + 0xF25E);
		lpDataBufPtr += dwPathLen;

		//COPY�ͷ�·��
		lpDecHandle->decInfos.bPathUnicode = FALSE;
		ZeroMemory(lpDecHandle->decInfos.szReleasePath, sizeof(lpDecHandle->decInfos.szReleasePath));
		CopyMemory(lpDecHandle->decInfos.szReleasePath, lpPathPtr, dwPathLen);

		//��ѹ����Ŀ�껺��
		DWORD dwOffset = 0;
		BYTE *lpOutPtr = NULL;
		DWORD dwOutSize = 0;
		bRet = AUTOIT_EA05_FileDecompress(lpDataBufPtr, &dwOffset, &lpOutPtr, &dwOutSize, dwPwdSeed);
		if (!bRet)
		{
			break;
		}

		if (NULL != lpDecHandle->decInfos.lpResultBuf)
		{
			delete[] lpDecHandle->decInfos.lpResultBuf;
		}
		lpDecHandle->decInfos.lpResultBuf = lpOutPtr;
		lpDecHandle->decInfos.dwResultSize = dwOutSize;

		//��ѹ���ƫ�ƶ�λ
		lpDataBufPtr += dwOffset;

		//�жϽű���ʶ
		CHAR szUnicodeFlag[MAX_PATH + 4] = ">AUTOIT UNICODE SCRIPT<";
		CHAR szAnsiFlag[MAX_PATH + 4] = ">AUTOIT SCRIPT<";
		BOOL bUnicode = (0 == memcmp(lpFlagPtrScript, szUnicodeFlag, dwScriptFlagLen));
		BOOL bAnsi = (0 == memcmp(lpFlagPtrScript, szAnsiFlag, dwScriptFlagLen));
		lpDecHandle->decInfos.bScript = (bUnicode || bAnsi);
		if (lpDecHandle->decInfos.bScript)
		{
			lpDecHandle->decInfos.bScriptUnicode = bUnicode;

			//ȡ�������汾��Ϣ(�ӽ��ܺ�Ľű��л�ȡ) [�ַ���"; <AUT2EXE VERSION: "]
			if (lpDecHandle->decInfos.bScriptUnicode)
			{
				WCHAR *lpScript = (WCHAR *)lpDecHandle->decInfos.lpResultBuf + 1 + lstrlenW(L"; <AUT2EXE VERSION: ");
				int nLen = 0;
				do 
				{
					if (lpScript[nLen] == '>')
					{
						break;
					}

					nLen++;
				} while (nLen < 32);

				CHAR szAutoitVer[64] = {0};
				CopyMemory(szAutoitVer, lpScript, nLen*sizeof(WCHAR));

				for (int i=0; i<nLen; i++)
				{
					lpDecHandle->decInfos.szAutoitVer[i] = szAutoitVer[2*i];
				}
			}
			else
			{
				CHAR *lpScript = (CHAR *)lpDecHandle->decInfos.lpResultBuf + lstrlenA("; <AUT2EXE VERSION: ");
				int nLen = 0;
				do 
				{
					if (lpScript[nLen] == '>')
					{
						break;
					}

					nLen++;
				} while (nLen < 32);
				CopyMemory(lpDecHandle->decInfos.szAutoitVer, lpScript, nLen);
			}
		}

		//ִ�лص�
		if (NULL != lpfnDecCallback)
		{
			BOOL bContinue = lpfnDecCallback(&lpDecHandle->decInfos, dwContent);
			if (!bContinue)
			{
				break;
			}
		}

		//�ж�λ���Ƿ��ѵ���ĩβ,ѹ���汾"AU3!EA05",ĩβҲ�������ʶ,����У��
		if ((lpDataBufPtr + 8) >= (lpDecHandle->lpFileBuf + lpDecHandle->dwFileSize))
		{
			break;
		}
	} while (TRUE);

	return bRet;
}

//�ļ���ѹ��
BOOL AUTOIT_EA05_FileDecompress(BYTE *lpInputBuf, DWORD *lpdwInputOffset, BYTE **lpOutputBuf, DWORD *lpdwOutputSize, DWORD dwSeed)
{
	//���뻺��
	BYTE *lpDataBuffer = lpInputBuf;

	//ѹ����ʶ
	BOOL bCompressFlag = lpDataBuffer[0];
	lpDataBuffer += 1;

	//ѹ�����С
	DWORD dwCompressSize = *(DWORD *)lpDataBuffer;
	dwCompressSize ^= 0x45AA;
	lpDataBuffer += 4;

	//ѹ��ǰ��С
	DWORD dwUncompressSize = *(DWORD *)lpDataBuffer;
	dwUncompressSize ^= 0x45AA;
	lpDataBuffer += 4;

	//��ѹУ���
	DWORD dwChkSum = *(DWORD *)lpDataBuffer;
	dwChkSum ^= 0xC3D2;
	lpDataBuffer += 4;

	//����16�ֽ���������
	lpDataBuffer += 16;

	//��������,���ܺ������ѹ��������"EA05"��ʼ
	BYTE *lpCompressData = lpDataBuffer;
	AUTOIT_EA05_DeCrypt(lpCompressData, dwCompressSize, dwSeed + 0x22AF);
	lpDataBuffer += dwCompressSize;

	//��������У���
	DWORD dwChkValue = AUTOIT_EA05_CalcCheckSum(lpCompressData, dwCompressSize);
	if (dwChkValue != dwChkSum)
	{
		return FALSE;
	}

	//�����Ϣ
	BYTE *lpResultPtr = NULL;
	DWORD dwResultSize = 0;

	//��ѹ������
	if (!bCompressFlag)
	{
		//δ����ѹ���Ľű�����
		dwResultSize = dwCompressSize;
		lpResultPtr = new BYTE[dwResultSize + 2];
		ZeroMemory(lpResultPtr, dwResultSize + 2);
		CopyMemory(lpResultPtr, lpCompressData, dwResultSize);
	}
	else
	{
		//�Ƚ�"EA05",����������˳�
		if (0x35304145 != *(DWORD *)lpCompressData)
		{
			return FALSE;
		}

		//�����ڴ���Ϊ���
		dwResultSize = dwUncompressSize;
		lpResultPtr = new BYTE[dwResultSize + 2];
		ZeroMemory(lpResultPtr, dwResultSize + 2);

		EAOLD_Decompress(lpCompressData, dwCompressSize, lpResultPtr, dwResultSize);
	}

	//���
	*lpdwInputOffset = lpDataBuffer - lpInputBuf;
	*lpOutputBuf = lpResultPtr;
	*lpdwOutputSize = dwResultSize;

	return TRUE;
}