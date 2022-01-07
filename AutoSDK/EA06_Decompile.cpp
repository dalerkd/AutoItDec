#include "StdAfx.h"
#include "EA06_Decompile.h"
#include "EANEW_UnCompress.h"

typedef struct _AUTOIT_EA06_DECRYPTHANDLE
{
	UINT c0;
	UINT c1;
	UINT ame[17];
}AUTOIT_EA06_DECRYPTHANDLE, *PAUTOIT_EA06_DECRYPTHANDLE;

double AUTOIT_EA06_Fpusht(AUTOIT_EA06_DECRYPTHANDLE *lpHandle);
void AUTOIT_EA06_Srand(AUTOIT_EA06_DECRYPTHANDLE *lpHandle, UINT seed);
BYTE AUTOIT_EA06_GetNext(AUTOIT_EA06_DECRYPTHANDLE *lpHandle);

double AUTOIT_EA06_Fpusht(AUTOIT_EA06_DECRYPTHANDLE *lpHandle) 
{
	union 
	{
		double as_double;
		struct 
		{
			UINT lo;
			UINT hi;
		}as_uint;
	}ret;

#define ROFL(a, b) (( a << (b % (sizeof(a)<<3) ))  |  (a >> (  (sizeof(a)<<3)  -  (b % (sizeof(a)<<3 )) ) ))

	UINT rolled = ROFL(lpHandle->ame[lpHandle->c0],9) +  ROFL(lpHandle->ame[lpHandle->c1],13);

	lpHandle->ame[lpHandle->c0] = rolled;

	if (!lpHandle->c0--) 
	{
		lpHandle->c0 = 16;
	}

	if (!lpHandle->c1--)
	{
		lpHandle->c1 = 16;
	}

	ret.as_uint.lo = rolled << 0x14;
	ret.as_uint.hi = 0x3ff00000 | (rolled >> 0xc);

	return (ret.as_double - 1.0);
}


void AUTOIT_EA06_Srand(AUTOIT_EA06_DECRYPTHANDLE *lpHandle, UINT seed) 
{
	for (int i=0; i<17; i++) 
	{
		seed *= 0x53A9B4FB;
		seed = 1 - seed;
		lpHandle->ame[i] = seed;
	}

	lpHandle->c0 = 0;
	lpHandle->c1 = 10;

	for (int i=0; i<9; i++)
	{
		AUTOIT_EA06_Fpusht(lpHandle);
	}
}

BYTE AUTOIT_EA06_GetNext(AUTOIT_EA06_DECRYPTHANDLE *l)
{
	BYTE ret = 0;
	AUTOIT_EA06_Fpusht(l);
	double x = AUTOIT_EA06_Fpusht(l) * 256.0;
	if ((int)x < 256) 
	{
		ret = (BYTE)x;
	}
	else
	{
		ret = 0xFF;
	}

	return ret;
}

void AUTOIT_EA06_DeCrypt(BYTE *lpBuffer, UINT nSize, UINT nSeed)
{
	AUTOIT_EA06_DECRYPTHANDLE hDecHandle = {0};
	AUTOIT_EA06_Srand(&hDecHandle, (UINT)nSeed);

	while (nSize--)
	{
		*lpBuffer++ ^= AUTOIT_EA06_GetNext(&hDecHandle);
	}
}

//�жϺ���
BOOL AUTOIT_EA06_IsEA06(AUTOIT3_DECOMPILER *lpDecHandle)
{
	BOOL bResult = FALSE;

	//16�ֽ�GUID
	static const BYTE cbAutoGUID[] = 
	{
		0xA3, 0x48, 0x4B, 0xBE, 0x98, 0x6C, 0x4A, 0xA9, 
		0x99, 0x4C, 0x53, 0x0A, 0x86, 0xD6, 0x48, 0x7D
	};

	//����ָ��
	BYTE *lpDataPtr = lpDecHandle->lpDataPtr;


LoopFind:

	//ѹ���汾�Ƚ�
	do 
	{
		//GUID��ʼ��ʶ�Ƚ�
		int nRet = memcmp(lpDataPtr, cbAutoGUID, 16);
		if (0 != nRet)
		{
			break;
		}
		lpDataPtr += 16;

		//ѹ���汾"AU3!EA06"
		static const BYTE cbAutoVersion[] = 
		{
			0x41, 0x55, 0x33, 0x21, 0x45, 0x41, 0x30, 0x36
		};

		//AUCN���޸İ�
		static const BYTE cbAutoVersionMod[] = 
		{
			0x21, 0x2A, 0x55, 0x23, 0x41, 0x55, 0x43, 0x4E
		};

		nRet = memcmp(lpDataPtr, cbAutoVersion, sizeof(cbAutoVersion));
		if (0 != nRet)
		{
			nRet = memcmp(lpDataPtr, cbAutoVersionMod, sizeof(cbAutoVersionMod));
			if (0 != nRet)
			{
				break;
			}
		}

		lpDataPtr += sizeof(cbAutoVersion);

		//���滹��16�ֽ���������
		lpDataPtr += 16;

		bResult = TRUE;
	} while (FALSE);

	//û�ҵ���ʶ��ʱ����Ҫ��������
	if (!bResult)
	{
		BYTE *pTmp = lpDecHandle->lpFileBuf;
		for (DWORD i=0; i<lpDecHandle->dwFileSize; i++)
		{
			int nCmp = memcmp(cbAutoGUID, pTmp, sizeof(cbAutoGUID));
			if (0 == nCmp)
			{
				lpDataPtr = pTmp;

				lpDecHandle->lpDataPtr = pTmp;
				lpDecHandle->dwDataSize = lpDecHandle->lpFileBuf + lpDecHandle->dwFileSize - lpDecHandle->lpDataPtr;
				lpDecHandle->bPacked = TRUE;

				goto LoopFind;
			}

			pTmp++;
		}
	}

	return bResult;
}

//�����ļ�У���
DWORD AUTOIT_EA06_CalcCheckSum(BYTE *lpBuffer, DWORD dwSize)
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

//�����봦��ʼ
typedef struct _AUTOIT_DECOMPILE_STRUCT
{
	BYTE *lpInputPtr;	//���뻺��
	UINT nInputSize;	//���뻺���С
	UINT nInputPos;		//���뻺����ʼƫ��

	BYTE *lpOutputPtr;	//�������
	UINT nOutputSize;	//��������С
	UINT nOutputPos;	//���������ʼƫ��

	UINT nLineCount;	//��������������
	UINT nError;		//������
}AUTOIT_EA06_DECOMPILE_STRUCT, *PAUTOIT_DECOMPILE_STRUCT;

BOOL EA06_ConvertOPCode2UnicodeScript(AUTOIT_EA06_DECOMPILE_STRUCT *lpDecObj);


BOOL AUTOIT_EA06_Decompile(AUTOIT3_DECOMPILER *lpDecHandle, DECOMPILE_CALLBACK lpfnDecCallback, DWORD dwContent)
{
	//����16�ֽ�GUID��8�ֽڰ汾��ʶ,�Լ������16�ֽ���������
	BYTE *lpDataBufPtr = lpDecHandle->lpDataPtr + (16 + 8 + 16);

	//����ѭ���������ݽ��ܴ���,�ű�����λ����ǰ��
	BOOL bRet = FALSE;
	DWORD dwFileCount = 0;
	do
	{
		//�����ļ���ʶ�ַ��� "FILE"
		BYTE *lpFileFlagPtr = lpDataBufPtr;
		AUTOIT_EA06_DeCrypt(lpFileFlagPtr, 4, 0x18EE);
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
		dwScriptFlagLen ^= 0xADBC;
		lpDataBufPtr += 4;

		//���ܽű���ʶ�ַ���
		WCHAR *lpFlagPtrScript = (WCHAR *)lpDataBufPtr;
		AUTOIT_EA06_DeCrypt((BYTE *)lpFlagPtrScript, dwScriptFlagLen*sizeof(WCHAR), dwScriptFlagLen + 0xB33F);
		lpDataBufPtr += (dwScriptFlagLen*sizeof(WCHAR));

		//·�����ݳ���
		DWORD dwPathLen = *(DWORD *)lpDataBufPtr;
		dwPathLen ^= 0xF820;
		lpDataBufPtr += 4;

		//·������(UNICODE����)
		WCHAR *lpPathPtr = (WCHAR *)lpDataBufPtr;
		AUTOIT_EA06_DeCrypt((BYTE *)lpPathPtr, dwPathLen*sizeof(WCHAR), dwPathLen + 0xF479);
		lpDataBufPtr += (dwPathLen*sizeof(WCHAR));

		//COPY�ͷ�·��
		lpDecHandle->decInfos.bPathUnicode = TRUE;
		ZeroMemory(lpDecHandle->decInfos.szReleasePath, sizeof(lpDecHandle->decInfos.szReleasePath));
		CopyMemory(lpDecHandle->decInfos.szReleasePath, lpPathPtr, dwPathLen*sizeof(WCHAR));

		//��ѹ����Ŀ�껺��,����汾û�м���������
		DWORD dwOffset = 0;
		BYTE *lpOutPtr = NULL;
		DWORD dwOutSize = 0;
		bRet = AUTOIT_EA06_FileDecompress(lpDataBufPtr, (DWORD)(lpDecHandle->dwDataSize - (lpDataBufPtr - lpDecHandle->lpDataPtr)), &dwOffset, &lpOutPtr, &dwOutSize, 0);
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
		WCHAR szEA06_FlagString[MAX_PATH + 4] = L">>>AUTOIT SCRIPT<<<";
		lpDecHandle->decInfos.bScript = (0 == memcmp(lpFlagPtrScript, szEA06_FlagString, dwScriptFlagLen*sizeof(WCHAR)));
		if (lpDecHandle->decInfos.bScript)
		{
			//���ϵ�еĽű�ֻ��UNICODE�汾
			lpDecHandle->decInfos.bScriptUnicode = TRUE;

			//�ű����ݻ���Ҫ����һ�λ�ԭ����
			//��������10����ԭ����Ĵ�С,�϶��㹻��
			DWORD dwFinalOutSize = dwOutSize * 10;
			BYTE *lpFinalResult = new BYTE[dwFinalOutSize];
			ZeroMemory(lpFinalResult, dwFinalOutSize);

			//��Ҫ�Լ�дUNICODE��ʶ
			lpFinalResult[0] = 0xFF;
			lpFinalResult[1] = 0xFE;

			AUTOIT_EA06_DECOMPILE_STRUCT decObj = {0};
			decObj.lpInputPtr = lpOutPtr + 4;
			decObj.nInputSize = dwOutSize - 4;
			decObj.nInputPos = 0;		//ǰ4�ֽ������յĴ�������

			decObj.lpOutputPtr = lpFinalResult + 2;
			decObj.nOutputSize = dwFinalOutSize - 2;
			decObj.nOutputPos = 0;		//ǰ2�ֽ���UNICODE��ʶ

			decObj.nLineCount = *(DWORD *)lpOutPtr;
			decObj.nError = 0;

			BOOL bResult = EA06_ConvertOPCode2UnicodeScript(&decObj);
			delete[] lpOutPtr;

			//��Ҫ���¸�ֵ�ű�����Ϣ
			lpDecHandle->decInfos.lpResultBuf = lpFinalResult;
			lpDecHandle->decInfos.dwResultSize = 2 + (decObj.nOutputPos - 1) * sizeof(WCHAR);
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

		//�ж�λ���Ƿ��ѵ���ĩβ,ѹ���汾"AU3!EA06",ĩβҲ�������ʶ,����У��
		if ((lpDataBufPtr + 8) >= (lpDecHandle->lpFileBuf + lpDecHandle->dwFileSize))
		{
			break;
		}
	} while (TRUE);

	return bRet;
}

//�ļ���ѹ��
BOOL AUTOIT_EA06_FileDecompress(BYTE *lpInputBuf, DWORD dwInputSize, DWORD *lpdwInputOffset, BYTE **lpOutputBuf, DWORD *lpdwOutputSize, DWORD dwSeed)
{
	//���뻺��
	BYTE *lpDataBuffer = lpInputBuf;

	//ѹ����ʶ
	BOOL bCompressFlag = lpDataBuffer[0];
	lpDataBuffer += 1;

	//ѹ�����С
	DWORD dwCompressSize = *(DWORD *)lpDataBuffer;
	dwCompressSize ^= 0x87BC;
	lpDataBuffer += 4;

	//���У�鲻̫�ϸ�
	if (lpDataBuffer + dwCompressSize > lpInputBuf + dwInputSize)
	{
		return FALSE;
	}

	//ѹ��ǰ��С
	DWORD dwUncompressSize = *(DWORD *)lpDataBuffer;
	dwUncompressSize ^= 0x87BC;
	lpDataBuffer += 4;

	//��ѹУ���
	DWORD dwChkSum = *(DWORD *)lpDataBuffer;
	dwChkSum ^= 0xA685;
	lpDataBuffer += 4;

	//����16�ֽ���������
	lpDataBuffer += 16;

	//��������,���ܺ������ѹ��������"EA06"��ʼ
	BYTE *lpCompressData = lpDataBuffer;
	AUTOIT_EA06_DeCrypt(lpCompressData, dwCompressSize, dwSeed + 0x2477);
	lpDataBuffer += dwCompressSize;

	//��������У���
	DWORD dwChkValue = AUTOIT_EA06_CalcCheckSum(lpCompressData, dwCompressSize);
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
		lpResultPtr = new BYTE[dwResultSize];
		CopyMemory(lpResultPtr, lpCompressData, dwResultSize);
	}
	else
	{
		//�Ƚ�"EA06",����������˳�
		if (0x36304145 != *(DWORD *)lpCompressData			//EA06
			&& 0x4E435541 != *(DWORD *)lpCompressData)		//AUCN
		{
			return FALSE;
		}

		//�����ڴ���Ϊ���
		dwResultSize = dwUncompressSize;
		lpResultPtr = new BYTE[dwResultSize];
		ZeroMemory(lpResultPtr, dwResultSize);

		EA06_Decompress(lpCompressData, dwCompressSize, lpResultPtr, dwResultSize);
	}

	//���
	*lpdwInputOffset = lpDataBuffer - lpInputBuf;
	*lpOutputBuf = lpResultPtr;
	*lpdwOutputSize = dwResultSize;

	return TRUE;
}

//�����ʽΪUNICODE
BOOL EA06_ConvertOPCode2UnicodeScript(AUTOIT_EA06_DECOMPILE_STRUCT *lpDecObj)
{
	//��������ôС�Ľű�,�ȹ��˵�!
	if (lpDecObj->nInputSize < 8)
	{
		return FALSE;
	}

	//����Ϊ0
	if (0 == lpDecObj->nLineCount)
	{
		return FALSE;
	}

	//ѭ������
	DWORD dwProcessedLine = 0;
	WCHAR *lpOutText = (WCHAR *)(lpDecObj->lpOutputPtr);
	do
	{
		//ȡ������
		BYTE nOpCode = lpDecObj->lpInputPtr[lpDecObj->nInputPos++];
		switch (nOpCode) 
		{
		case 0x05: /* <INT> */
			{
				//��ֹ�����쳣,��֤4�ֽڵ����ݳ�������Ч������
				if (lpDecObj->nInputPos >= lpDecObj->nInputSize-4) 
				{
					lpDecObj->nError = 1;
					break;
				}

				//ȡ��ֵ
				DWORD dwNumber = *(DWORD *)(&lpDecObj->lpInputPtr[lpDecObj->nInputPos]);

				//�����ֵǰ������������������ո�
				if (lpDecObj->nOutputPos > 4)
				{
					if ((' ' == lpOutText[lpDecObj->nOutputPos-1])
						&& ('-' == lpOutText[lpDecObj->nOutputPos-2])
						&& (' ' == lpOutText[lpDecObj->nOutputPos-3]))
					{
						if (('=' == lpOutText[lpDecObj->nOutputPos-4])
							|| (',' == lpOutText[lpDecObj->nOutputPos-4]))
						{
							lpDecObj->nOutputPos--;
						}						
					}
				}
				
				//ͳһ��ʮ������
				WCHAR szIntNumber[32] = {0};
				int nLen = wsprintfW(szIntNumber, L"0x%08X ", dwNumber);
				
				//COPY���������
				lstrcpyW(&lpOutText[lpDecObj->nOutputPos], szIntNumber);
				lpDecObj->nOutputPos += nLen;
				lpDecObj->nInputPos += 4;

				break;
			}

		case 0x10: /* <INT64> */
			{
				//��ֹ�����쳣,��֤8�ֽڵ����ݳ�������Ч������
				if (lpDecObj->nInputPos >= lpDecObj->nInputSize-8)
				{
					lpDecObj->nError = 1;
					break;
				}

				//ͳһ��ʮ������(��Ҫ�����ν��д���)
				DWORD dwLow = *(DWORD*)(&lpDecObj->lpInputPtr[lpDecObj->nInputPos]);
				DWORD dwHigh = *(DWORD*)(&lpDecObj->lpInputPtr[lpDecObj->nInputPos+4]);

				UINT64 val = dwHigh;
				val = (val << 32) + dwLow;

				WCHAR szINT64Number[32] = {0};
				int nLen = wsprintfW(szINT64Number, L"%I64d", val);

				//COPY���������
				lstrcpyW(&lpOutText[lpDecObj->nOutputPos], szINT64Number);
				lpDecObj->nOutputPos += nLen;
				lpDecObj->nInputPos += 8;
				
				break;
			}

		case 0x20: /* <DOUBLE> */
			{
				//��ֹ�����쳣,��֤8�ֽڵ����ݳ�������Ч������
				if (lpDecObj->nInputPos >= lpDecObj->nInputSize-8) 
				{
					lpDecObj->nError = 1;
					break;
				}

				//���淽ʽ
				WCHAR szDoubleNumber[64] = {0};
				double val = *(double *)&lpDecObj->lpInputPtr[lpDecObj->nInputPos];
				int nLen = _snwprintf(szDoubleNumber, 64, L"%g ", val);

				//COPY���������
				lstrcpyW(&lpOutText[lpDecObj->nOutputPos], szDoubleNumber);
				lpDecObj->nOutputPos += nLen;
				lpDecObj->nInputPos += 8;

				break;
			}

			//�﷨�ṹ����
		case 0x30: /* KEYWORD */	//�ؼ���
		case 0x31: /* COMMAND */	//����
		case 0x32: /* MACRO */		//��
		case 0x33: /* VAR */		//����
		case 0x34: /* FUNC */		//����
		case 0x35: /* OBJECT */		//����
		case 0x36: /* STRING */		//�ַ���
		case 0x37: /* DIRECTIVE */	//Ԥ��������
			{
				//��ֹ�����쳣,��֤4�ֽڵ��ַ���������Ч������
				if (lpDecObj->nInputPos >= lpDecObj->nInputSize-4) 
				{
					lpDecObj->nError = 1;
					break;
				}

				//ȡ4�ֽڵ��ַ���
				UINT nNumberOfChars = *(DWORD *)(&lpDecObj->lpInputPtr[lpDecObj->nInputPos]);
				UINT nNumberOfBytes = nNumberOfChars*sizeof(WCHAR);
				lpDecObj->nInputPos += 4;

				//У����Ч��Χ
				if ((lpDecObj->nInputSize < nNumberOfBytes) 
					|| (lpDecObj->nInputPos + nNumberOfBytes >= lpDecObj->nInputSize))
				{
					lpDecObj->nError = 1;
					break;
				}

				//���ǰ׺
				const static BYTE EA06_prefixes[] = {'\0', '\0', '@', '$', '\0', '.', '"', '\0'};
				if (0 != EA06_prefixes[nOpCode-0x30])
				{
					lpOutText[lpDecObj->nOutputPos++] = EA06_prefixes[nOpCode-0x30];
				}				

				//�ַ�����
				if (nNumberOfChars) 
				{
					for (UINT i=0; i<nNumberOfBytes; i+=2) 
					{
						lpDecObj->lpInputPtr[lpDecObj->nInputPos+i] ^= (BYTE)nNumberOfChars;
						lpDecObj->lpInputPtr[lpDecObj->nInputPos+i+1] ^= (BYTE)(nNumberOfChars>>8);
					}
					CopyMemory(&lpOutText[lpDecObj->nOutputPos], &lpDecObj->lpInputPtr[lpDecObj->nInputPos], nNumberOfBytes);

					//���λ�ú���
					lpDecObj->nOutputPos += nNumberOfChars;
					lpDecObj->nInputPos += nNumberOfBytes;
				}

				//��Ӻ�׺
				const static BYTE EA06_postfixes[] = {' ', '\0', ' ', ' ', '\0', ' ', '"', ' '};
				if (0 != EA06_postfixes[nOpCode-0x30])
				{
					lpOutText[lpDecObj->nOutputPos++] = EA06_postfixes[nOpCode-0x30];
				}
			}
			break;

			//����������
		case 0x40: /* , */
		case 0x41: /* = */
		case 0x42: /* > */
		case 0x43: /* < */
		case 0x44: /* <> */
		case 0x45: /* >= */
		case 0x46: /* <= */
		case 0x47: /* ( */
		case 0x48: /* ) */
		case 0x49: /* + */
		case 0x4A: /* - */
		case 0x4B: /* / */
		case 0x4C: /* * */
		case 0x4D: /* & */
		case 0x4E: /* [ */
		case 0x4F: /* ] */
		case 0x50: /* == */
		case 0x51: /* ^ */
		case 0x52: /* += */
		case 0x53: /* -= */
		case 0x54: /* /= */
		case 0x55: /* *= */
		case 0x56: /* &= */
			{					
				//����������
				const static WCHAR *EA06_opers[] = 
				{
					L",",	L"=",	L">",	L"<",	L"<>",	L">=",	L"<=",	L"(", 
					L")",	L"+",	L"-",	L"/",	L"*",	L"&",	L"[",	L"]",
					L"==",	L"^",	L"+=",	L"-=",	L"/=",	L"*=",	L"&="
				};

				const static BYTE EA06_opPostfixes[] = 
				{
					' ',	' ',	' ',	' ',	' ',	' ',	' ',	'\0',
					' ',	' ',	' ',	' ',	' ',	' ',	'\0',	'\0',
					' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
				};

				//���������ż�����ǰ�Ŀո�
				if (0x48 == nOpCode || 0x40 == nOpCode)
				{					
					if (lpOutText[lpDecObj->nOutputPos-1] == ' ')
					{
						lpDecObj->nOutputPos--;
					}
				}

				//���ֲ�����������Ҫ��ӿո�
				lpDecObj->nOutputPos += _snwprintf(&lpOutText[lpDecObj->nOutputPos], 4, L"%s", EA06_opers[nOpCode-0x40]);
				if (0 != EA06_opPostfixes[nOpCode-0x40])
				{
					lpOutText[lpDecObj->nOutputPos++] = EA06_opPostfixes[nOpCode-0x40];
				}

				break;
			}

		case 0x7F:	//�н���
			{
				dwProcessedLine++;
				lpDecObj->nLineCount--;
				if (0 != lpDecObj->nLineCount)
				{
					//������ĩ����Ŀո�
					if (lpOutText[lpDecObj->nOutputPos-1] == ' ')
					{
						lpDecObj->nOutputPos--;
					}

					//��ӻس�����
					lpOutText[lpDecObj->nOutputPos++] = '\r';
					lpOutText[lpDecObj->nOutputPos++] = '\n';
				}
				else
				{
					//�����ļ�ĩβ����Ŀո�
					if (lpOutText[lpDecObj->nOutputPos-1] == ' ')
					{
						lpOutText[lpDecObj->nOutputPos-1] = 0;
					}
				}

				break;
			}

		default:
			//������δ֪��OP������
			lpDecObj->nError = 1;
		}
	} while (!lpDecObj->nError && lpDecObj->nLineCount && lpDecObj->nInputPos < lpDecObj->nInputSize);

	return (0 == lpDecObj->nError);
}