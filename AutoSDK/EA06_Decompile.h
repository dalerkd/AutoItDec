#pragma once

#include "include.h"

/*֧�ֵİ汾��Χ:	
3.2.6.0
3.2.8.0
3.2.8.1
3.2.10.0
3.2.12.0
3.2.12.1
3.3.0.0
3.3.2.0
3.3.4.0
3.3.6.0
3.3.6.1
3.3.7.2
3.3.7.15
*/

//�жϺ���
BOOL AUTOIT_EA06_IsEA06(AUTOIT3_DECOMPILER *lpDecHandle);

//����У���
DWORD AUTOIT_EA06_CalcCheckSum(BYTE *lpBuffer, DWORD dwSize);

//���ܺ���
void AUTOIT_EA06_DeCrypt(BYTE *lpBuffer, UINT nSize, UINT nSeed);

//�����뺯��
BOOL AUTOIT_EA06_Decompile(AUTOIT3_DECOMPILER *lpDecHandle, DECOMPILE_CALLBACK lpfnDecCallback, DWORD dwContent);

//�ļ���ѹ��
BOOL AUTOIT_EA06_FileDecompress(BYTE *lpInputBuf, DWORD dwInputSize, DWORD *lpdwInputOffset, BYTE **lpOutputBuf, DWORD *lpdwOutputSize, DWORD dwSeed);