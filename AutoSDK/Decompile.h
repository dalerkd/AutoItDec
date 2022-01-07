#pragma once

#include <windows.h>

//��������
typedef struct _AUTOIT3_DECINFORMATIONS
{
	//�����ǹ���������
	CHAR szAutoitVer[64];			//[OUT]AUTOIT�汾��Ϣ(��ֻ�ܴӽű��л�ȡ)
	CHAR szPassword[MAX_PATH];		//[OUT]�����ַ���
	BOOL bA3XFormat;				//[OUT]�Ƿ�������Ľű�����
	BOOL bX64;						//[OUT]��PEͷʱ�Ƿ���64λPEͷ

	//������ÿ���ļ�����������
	DWORD dwFileIndex;				//[OUT]�ļ����,��0��ʼ
	BOOL bScript;					//[OUT]�Ƿ�ű�����,ÿ���ļ���ֻ��һ���ű�
	BOOL bScriptUnicode;			//[OUT]Ϊ�ű�����ʱ,�ű��Ƿ�ΪUNICODE����,UNICODE����ʱǰ���ֽ���UNICODE�ı���ʶ
	BYTE *lpResultBuf;				//[OUT]������������[���ڴ�����]
	DWORD dwResultSize;				//[OUT]����������С

	BOOL bPathUnicode;				//[OUT]�ͷ�·���Ƿ�UNICODE����
	BYTE szReleasePath[MAX_PATH*sizeof(WCHAR)];	//[OUT]��ǰ�ļ��ͷ�·��
}AUTOIT3_DECINFORMATIONS, *PAUTOIT3_DECINFORMATIONS;

//������ص�����,����ص�����FALSE���ٽ��к����ص���
typedef BOOL (*DECOMPILE_CALLBACK)(AUTOIT3_DECINFORMATIONS *lpDecInfos, DWORD dwContent);

//�����뵼���ӿ�
HANDLE __stdcall AUTOIT_Init(BYTE *lpFileBuffer, DWORD dwFileSize);
BOOL __stdcall AUTOIT_DoDecompile(HANDLE hDecHandle, DECOMPILE_CALLBACK lpfnDecCallback, DWORD dwContent);
void __stdcall AUTOIT_UnInit(HANDLE hDecHandle);