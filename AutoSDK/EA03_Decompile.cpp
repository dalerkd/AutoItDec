#include "StdAfx.h"
#include "EA03_Decompile.h"
#include "EA03_UnCompress.h"

typedef struct _AUTOIT_EA03_DECRYPTHANDLE
{
	UINT *next;
	UINT items;
	UINT mt[624];
}AUTOIT_EA03_DECRYPTHANDLE, *PAUTOIT_EA03_DECRYPTHANDLE;

BYTE AUTOIT_EA03_GetNext(AUTOIT_EA03_DECRYPTHANDLE *lpHandle) 
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

void AUTOIT_EA03_DeCrypt(BYTE *lpBuffer, UINT nSize, UINT nSeed)
{
	AUTOIT_EA03_DECRYPTHANDLE hDecHandle = {0};
	UINT *mt = hDecHandle.mt;

	*mt = nSeed;
	for(int i=1; i<624; i++)
	{
		mt[i] = i + 0x6c078965*((mt[i-1]>>30) ^ mt[i-1]);
	}

	hDecHandle.items = 1;

	while (nSize--)
	{
		*lpBuffer++ ^= AUTOIT_EA03_GetNext(&hDecHandle);
	}
}

//�жϺ���
BOOL AUTOIT_EA03_IsEA03(AUTOIT3_DECOMPILER *lpDecHandle)
{
	//��֧�ֽ��������ݵ����
	if (lpDecHandle->decInfos.bA3XFormat)
	{
		return FALSE;
	}

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

	//EA03�İ汾��16�ֽ�֮����3
	if (3 != lpDataPtr[0])
	{
		return FALSE;
	}

	//���ݴ�С
	typedef struct _EA03_OFFSETINFO
	{
		DWORD dwBeginOffset;	//�ű�������ʼƫ��(λ��PE֮��)
		DWORD dwChkSum;			//�ļ�У���
	}EA03_OFFSETINFO, *PEA03_OFFSETINFO;

	//�ű�������ʼƫ��
	EA03_OFFSETINFO *lpOffsetObj = (EA03_OFFSETINFO *)(lpDecHandle->lpDataPtr + lpDecHandle->dwDataSize - 8);
	if (lpOffsetObj->dwBeginOffset != lpDecHandle->dwPESize)
	{
		return FALSE;
	}

	//PE���ֵ�����У���
	DWORD dwCheckValue = lpOffsetObj->dwChkSum ^ 0xAAAAAAAA;

	//�����ļ�У���
	DWORD dwCRCValue = AUTOIT_EA03_CalcCheckSum(lpDecHandle->lpFileBuf, lpDecHandle->dwFileSize-4);

	//�Ա�CRCУ���
	if (dwCheckValue != dwCRCValue)
	{
		return FALSE;
	}

	return TRUE;
}

DWORD AUTOIT_EA03_CalcCheckSum(BYTE *lpBuffer, DWORD dwSize)
{
	//����CRC��
	DWORD dwCRCTable[256] = {0};
	int nIndex = 0;
	do
	{
		int nTableValue = nIndex << 24;
		int nIndexSub = 8;
		do
		{
			if (nTableValue & 0x80000000)
			{
				nTableValue = 2 * nTableValue ^ 0x4C11DB7;
			}
			else
			{
				nTableValue *= 2;
			}

			nIndexSub--;
		}
		while (nIndexSub);

		dwCRCTable[nIndex++] = nTableValue;
	}
	while (nIndex < 256);

	//����CRCУ���
	DWORD dwCRCValue = 0xFFFFFFFF;
	for (DWORD i=0; i<dwSize; i++)
	{
		dwCRCValue = (dwCRCValue << 8) ^ dwCRCTable[lpBuffer[i] ^ (dwCRCValue >> 24)];
	}

	return dwCRCValue;
}

BOOL AUTOIT_EA03_Decompile(AUTOIT3_DECOMPILER *lpDecHandle, DECOMPILE_CALLBACK lpfnDecCallback, DWORD dwContent)
{
	//����16�ֽ�GUID��1�ֽڰ汾��ʶ
	BYTE *lpDataBufPtr = lpDecHandle->lpDataPtr + (16 + 1);

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
		AUTOIT_EA03_DeCrypt((BYTE *)szPassword, dwPwdStrLen, dwPwdStrLen + 0xC3D2);

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
		AUTOIT_EA03_DeCrypt(lpFileFlagPtr, 4, 0x16FA);
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
		AUTOIT_EA03_DeCrypt(lpFlagPtrScript, dwScriptFlagLen, dwScriptFlagLen + 0xA25E);
		lpDataBufPtr += dwScriptFlagLen;

		//·�����ݳ���
		DWORD dwPathLen = *(DWORD *)lpDataBufPtr;
		dwPathLen ^= 0x29AC;
		lpDataBufPtr += 4;

		//·������
		CHAR *lpPathPtr = (CHAR *)lpDataBufPtr;
		AUTOIT_EA03_DeCrypt((BYTE *)lpPathPtr, dwPathLen, dwPathLen + 0xF25E);
		lpDataBufPtr += dwPathLen;

		//COPY�ͷ�·��
		lpDecHandle->decInfos.bPathUnicode = FALSE;
		ZeroMemory(lpDecHandle->decInfos.szReleasePath, sizeof(lpDecHandle->decInfos.szReleasePath));
		CopyMemory(lpDecHandle->decInfos.szReleasePath, lpPathPtr, dwPathLen);

		//��ѹ����Ŀ�껺��
		DWORD dwOffset = 0;
		BYTE *lpOutPtr = NULL;
		DWORD dwOutSize = 0;
		bRet = AUTOIT_EA03_FileDecompress(lpDataBufPtr, &dwOffset, &lpOutPtr, &dwOutSize, dwPwdSeed);
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
		CHAR szAutoitScriptFlag[MAX_PATH + 4] = ">AUTOIT SCRIPT<";
		lpDecHandle->decInfos.bScript = (0 == memcmp(lpFlagPtrScript, szAutoitScriptFlag, dwScriptFlagLen));
		if (lpDecHandle->decInfos.bScript)
		{
			//���ϵ�еĽű�ֻ��ANSI�汾
			lpDecHandle->decInfos.bScriptUnicode = FALSE;

			//ȡ�������汾��Ϣ(�ӽ��ܺ�Ľű��л�ȡ) [�ַ���"; <COMPILER: v3.0.100.0>"]
			CHAR *lpScript = (CHAR *)lpDecHandle->decInfos.lpResultBuf + lstrlenA("; <COMPILER: ");
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

		//�ж��Ƿ�ɰ汾autohotkey�ű�(�°汾��ֱ�Ӱ��������ݷŵ���Դ����)
		CHAR szAutoHotkeyScriptFlag[MAX_PATH + 4] = ">AUTOHOTKEY SCRIPT<";
		if (!lpDecHandle->decInfos.bScript)
		{
			lpDecHandle->decInfos.bScript = (0 == memcmp(lpFlagPtrScript, szAutoHotkeyScriptFlag, dwScriptFlagLen));
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
		if ((lpDataBufPtr + 8) >= (lpDecHandle->lpFileBuf + lpDecHandle->dwFileSize))
		{
			break;
		}
	} while (TRUE);

	return bRet;
}

//�ļ���ѹ��
BOOL AUTOIT_EA03_FileDecompress(BYTE *lpInputBuf, DWORD *lpdwInputOffset, BYTE **lpOutputBuf, DWORD *lpdwOutputSize, DWORD dwSeed)
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

	//��������,���ܺ������ѹ��������"JB01"��ʼ
	BYTE *lpCompressData = lpDataBuffer;
	AUTOIT_EA03_DeCrypt(lpCompressData, dwCompressSize, dwSeed + 0x22AF);
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
		//�Ƚ�"JB01",����������˳�
		if (0x3130424A != *(DWORD *)lpCompressData)
		{
			return FALSE;
		}

		//�����ڴ���Ϊ���
		dwResultSize = dwUncompressSize;
		lpResultPtr = new BYTE[dwResultSize + 2];
		ZeroMemory(lpResultPtr, dwResultSize + 2);

		//EA03��ѹ��
		EA03_Decompress obj_EA03;
		obj_EA03.SetDefaults();
		obj_EA03.SetInputBuffer(lpCompressData);
		obj_EA03.SetOutputBuffer(lpResultPtr);
		obj_EA03.SetAlgID("JB01");
		obj_EA03.Decompress();
	}

	//���
	*lpdwInputOffset = lpDataBuffer - lpInputBuf;
	*lpOutputBuf = lpResultPtr;
	*lpdwOutputSize = dwResultSize;

	return TRUE;
}