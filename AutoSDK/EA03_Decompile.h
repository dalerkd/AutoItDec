#pragma once

#include "include.h"

/*֧�ֵİ汾:
3.0.100.0
3.0.101.0
*/

//�жϺ���
BOOL AUTOIT_EA03_IsEA03(AUTOIT3_DECOMPILER *lpDecHandle);

//����У���
DWORD AUTOIT_EA03_CalcCheckSum(BYTE *lpBuffer, DWORD dwSize);

//���ܺ���
void AUTOIT_EA03_DeCrypt(BYTE *lpBuffer, UINT nSize, UINT nSeed);

//�����뺯��
BOOL AUTOIT_EA03_Decompile(AUTOIT3_DECOMPILER *lpDecHandle, DECOMPILE_CALLBACK lpfnDecCallback, DWORD dwContent);

//�ļ���ѹ��
BOOL AUTOIT_EA03_FileDecompress(BYTE *lpInputBuf, DWORD *lpdwInputOffset, BYTE **lpOutputBuf, DWORD *lpdwOutputSize, DWORD dwSeed);