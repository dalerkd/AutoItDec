#include "StdAfx.h"
#include "EA04_Decompile.h"
#include "EANEW_UnCompress.h"

typedef struct _AUTOIT_EA04_DECRYPTHANDLE
{
	UINT *next;
	UINT items;
	UINT mt[624];
}AUTOIT_EA04_DECRYPTHANDLE, *PAUTOIT_EA04_DECRYPTHANDLE;

BYTE AUTOIT_EA04_GetNext(AUTOIT_EA04_DECRYPTHANDLE *lpHandle) 
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

void AUTOIT_EA04_DeCrypt(BYTE *lpBuffer, UINT nSize, UINT nSeed)
{
	AUTOIT_EA04_DECRYPTHANDLE hDecHandle = {0};
	UINT *mt = hDecHandle.mt;

	*mt = nSeed;
	for(int i=1; i<624; i++)
	{
		mt[i] = i + 0x6c078965*((mt[i-1]>>30) ^ mt[i-1]);
	}

	hDecHandle.items = 1;

	while (nSize--)
	{
		*lpBuffer++ ^= AUTOIT_EA04_GetNext(&hDecHandle);
	}
}

BOOL AUTOIT_EA04_IsEA04(AUTOIT3_DECOMPILER *lpDecHandle)
{
	//��֧�ֽ��������ݵ����
	if (lpDecHandle->decInfos.bA3XFormat)
	{
		return FALSE;
	}

	//���ܺ������ƫ����Ϣ
	typedef struct _EA04_OFFSETINFO
	{
		DWORD dwEndOffset;		//�ű����ݽ���ƫ��,���û�и����ļ�������λ��ĩβ12�ֽڴ�
		DWORD dwBeginOffset;	//�ű�������ʼƫ��(λ��PE֮��)
		DWORD dwChkSum;			//�ű�����У���(����������)
	}EA04_OFFSETINFO, *PEA04_OFFSETINFO;
	EA04_OFFSETINFO *lpOffsetObj = (EA04_OFFSETINFO *)(lpDecHandle->lpFileBuf + lpDecHandle->dwFileSize - 12);

	//�ű����ݽ���ƫ��
	DWORD dwEndOffset = lpOffsetObj->dwEndOffset ^ 0xAAAAAAAA;
	if (dwEndOffset > lpDecHandle->dwFileSize - 12)
	{
		return FALSE;
	}

	//AUTOIT����ƫ��λ��(λ��PE֮��)
	DWORD dwBeginOffset = lpOffsetObj->dwBeginOffset ^ 0xAAAAAAAA;
	if (dwBeginOffset != lpDecHandle->dwPESize)
	{
		return FALSE;
	}

	//����У���
	DWORD dwChkSum = lpOffsetObj->dwChkSum ^ 0xAAAAAAAA;
	
	//����У���
	DWORD dwChkValue = AUTOIT_EA04_CalcCheckSum(lpDecHandle->lpFileBuf, dwEndOffset);
	
	//�Ƚ�У���
	if (dwChkValue != dwChkSum)
	{
		return FALSE;
	}

	//�����ڽ�������ʶ��ʱEA04��PE���ᳬ��300K(�ر�ע��!)
	BYTE *lpBufPtr = lpDecHandle->lpFileBuf + dwBeginOffset;

	//ȡ���ݴ�С
	DWORD dwTmpSize = *(DWORD *)lpBufPtr;
	dwTmpSize ^= 0xADAC;
	lpBufPtr += 4;

	//�������ݲ���
	lpBufPtr += dwTmpSize;

	//ȡѹ���汾��ʶ,����Ϊ4,����Ӧ�˳�
	BYTE nCompressVer = lpBufPtr[0];
	if (4 != nCompressVer)
	{
		return FALSE;
	}

	return TRUE;
}

//�����ļ�У���
DWORD AUTOIT_EA04_CalcCheckSum(BYTE *lpBuffer, DWORD dwSize)
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

BOOL AUTOIT_EA04_Decompile(AUTOIT3_DECOMPILER *lpDecHandle, DECOMPILE_CALLBACK lpfnDecCallback, DWORD dwContent)
{
	//û����Ҫ����������
	BYTE *lpDataBufPtr = lpDecHandle->lpDataPtr;

	//ȡ�������ݴ�С
	DWORD dwInvalidDataSize = *(DWORD *)lpDataBufPtr;
	dwInvalidDataSize ^= 0xADAC;
	lpDataBufPtr += 4;

	//�����������ݲ���
	lpDataBufPtr += dwInvalidDataSize;

	//����ѹ���汾��ʶ
	lpDataBufPtr += 1;

	//���������ַ���
	DWORD dwPwdStrLen = *(DWORD *)lpDataBufPtr;
	dwPwdStrLen ^= 0xFAC1;
	lpDataBufPtr += 4;

	//AUTOIT�������ַ���,�������ܻ��õ�
	DWORD dwPwdSeed = 0;
	if (0 != dwPwdStrLen)
	{
		CHAR szPassword[MAX_PATH] = {0};
		CopyMemory(szPassword, lpDataBufPtr, dwPwdStrLen);
		AUTOIT_EA04_DeCrypt((BYTE *)szPassword, dwPwdStrLen, dwPwdStrLen + 0xC3D2);

		for (DWORD i=0; i<dwPwdStrLen; i++)
		{
			dwPwdSeed += szPassword[i];
		}

		lpDataBufPtr += dwPwdStrLen;

		CopyMemory(lpDecHandle->decInfos.szPassword, szPassword, dwPwdStrLen);
	}

	//����ѭ���������ݽ��ܴ���,�ű�����λ����ǰ��
	BOOL bRet = FALSE;
	DWORD dwFileCount = 0;
	do 
	{
		//�����ļ���ʶ�ַ���"FILE"
		BYTE *lpFileFlagPtr = lpDataBufPtr;
		AUTOIT_EA04_DeCrypt(lpFileFlagPtr, 4, 0x16FA);
		lpDataBufPtr += 4;

		CHAR *lpFileFlag = "FILE";
		if (0 != memcmp(lpFileFlag, lpFileFlagPtr, 4))
		{
			break;
		}

		//�ļ����
		lpDecHandle->decInfos.dwFileIndex = dwFileCount;
		dwFileCount++;

		//�ű�����
		DWORD dwScriptFlagLen = *(DWORD *)lpDataBufPtr;
		dwScriptFlagLen ^= 0x29BC;
		lpDataBufPtr += 4;

		//���ܱ�ʶ�ַ���
		BYTE *lpFlagPtrScript = lpDataBufPtr;
		AUTOIT_EA04_DeCrypt(lpFlagPtrScript, dwScriptFlagLen, dwScriptFlagLen + 0xA25E);
		lpDataBufPtr += dwScriptFlagLen;

		//·�����ݲ��ֳ���
		DWORD dwPathLen = *(DWORD *)lpDataBufPtr;
		dwPathLen ^= 0x29AC;
		lpDataBufPtr += 4;

		//·������(ANSI����)
		CHAR *lpPathPtr = (CHAR *)lpDataBufPtr;
		AUTOIT_EA04_DeCrypt((BYTE *)lpPathPtr, dwPathLen, dwPathLen + 0xF25E);
		lpDataBufPtr += dwPathLen;

		//COPY�ͷ�·��
		lpDecHandle->decInfos.bPathUnicode = FALSE;
		ZeroMemory(lpDecHandle->decInfos.szReleasePath, sizeof(lpDecHandle->decInfos.szReleasePath));
		CopyMemory(lpDecHandle->decInfos.szReleasePath, lpPathPtr, dwPathLen);

		//��ѹ����Ŀ�껺��
		DWORD dwOffset = 0;
		BYTE *lpOutPtr = NULL;
		DWORD dwOutSize = 0;
		bRet = AUTOIT_EA04_FileDecompress(lpDataBufPtr, &dwOffset, &lpOutPtr, &dwOutSize, dwPwdSeed);
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
		CHAR szScriptFlag[MAX_PATH + 4] = ">AUTOIT SCRIPT<";
		lpDecHandle->decInfos.bScript = (0 == memcmp(lpFlagPtrScript, szScriptFlag, dwScriptFlagLen));
		if (lpDecHandle->decInfos.bScript)
		{
			//���ϵ�еĽű�ֻ��ANSI�汾
			lpDecHandle->decInfos.bScriptUnicode = FALSE;

			//ȡ�������汾��Ϣ(�ӽ��ܺ�Ľű��л�ȡ) [�ַ���"; <AUT2EXE VERSION: "]
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

		//ִ�лص�
		if (NULL != lpfnDecCallback)
		{
			BOOL bContinue = lpfnDecCallback(&lpDecHandle->decInfos, dwContent);
			if (!bContinue)
			{
				break;
			}
		}

		//�ж�λ���Ƿ��ѵ���ĩβ
		if ((lpDataBufPtr + 12) >= (lpDecHandle->lpFileBuf + lpDecHandle->dwFileSize))
		{
			break;
		}
	} while (TRUE);

	return bRet;
}

//�ļ���ѹ��
BOOL AUTOIT_EA04_FileDecompress(BYTE *lpInputBuf, DWORD *lpdwInputOffset, BYTE **lpOutputBuf, DWORD *lpdwOutputSize, DWORD dwSeed)
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

	//����16�ֽ���������
	lpDataBuffer += 16;

	//��������,���ܺ������ѹ��������"EA04"��ʼ
	BYTE *lpCompressData = lpDataBuffer;
	AUTOIT_EA04_DeCrypt(lpCompressData, dwCompressSize, dwSeed + 0x22AF);
	lpDataBuffer += dwCompressSize;

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
		//�Ƚ�"EA04",����������˳�
		if (0x34304145 != *(DWORD *)lpCompressData)
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