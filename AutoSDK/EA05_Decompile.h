#pragma once

#include "include.h"

/*֧�ֵİ汾��Χ:
3.2.0.0
3.2.0.1
3.2.1.0 
3.2.2.0
3.2.4.0
3.2.4.1
3.2.4.2
3.2.4.3
3.2.4.4
3.2.4.5
3.2.4.6
3.2.4.7
3.2.4.8
3.2.4.9
*/

//�жϺ���
BOOL AUTOIT_EA05_IsEA05(AUTOIT3_DECOMPILER *lpDecHandle);

//����У���
DWORD AUTOIT_EA05_CalcCheckSum(BYTE *lpBuffer, DWORD dwSize);

//���ܺ���
void AUTOIT_EA05_DeCrypt(BYTE *lpBuffer, UINT nSize, UINT nSeed);

//�����뺯��
BOOL AUTOIT_EA05_Decompile(AUTOIT3_DECOMPILER *lpDecHandle, DECOMPILE_CALLBACK lpfnDecCallback, DWORD dwContent);

//�ļ���ѹ��
BOOL AUTOIT_EA05_FileDecompress(BYTE *lpInputBuf, DWORD *lpdwInputOffset, BYTE **lpOutputBuf, DWORD *lpdwOutputSize, DWORD dwSeed);