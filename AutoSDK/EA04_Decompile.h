#pragma once

#include "include.h"

/*֧�ֵİ汾:
3.0.102.0
3.1.0.15
3.1.1.0
3.1.1.74~3.1.1.133(�汾������)
*/

//�жϺ���
BOOL AUTOIT_EA04_IsEA04(AUTOIT3_DECOMPILER *lpDecHandle);

//����У���
DWORD AUTOIT_EA04_CalcCheckSum(BYTE *lpBuffer, DWORD dwSize);

//���ܺ���
void AUTOIT_EA04_DeCrypt(BYTE *lpBuffer, UINT nSize, UINT nSeed);

//�����뺯��
BOOL AUTOIT_EA04_Decompile(AUTOIT3_DECOMPILER *lpDecHandle, DECOMPILE_CALLBACK lpfnDecCallback, DWORD dwContent);

//�ļ���ѹ��
BOOL AUTOIT_EA04_FileDecompress(BYTE *lpInputBuf, DWORD *lpdwInputOffset, BYTE **lpOutputBuf, DWORD *lpdwOutputSize, DWORD dwSeed);