#pragma once

#include "Decompile.h"

//��������
typedef struct _AUTOIT3_DECOMPILER
{
	AUTOIT3_DECINFORMATIONS decInfos;	//��������Ϣ

	//����Ϊ�ڲ�ʹ��
	BOOL bSucess;						//������ɹ���ʶ

	BYTE *lpFileBuf;					//�ļ�����[���ڴ�����]
	DWORD dwFileSize;					//�ļ������С
	BOOL bPacked;						//�Ƿ񱻼ӿ�

	DWORD dwPESize;						//PE��С
	BYTE *lpDataPtr;					//��������ָ��
	DWORD dwDataSize;					//�������ݴ�С
	LONG nProcPtr;						//�����뺯��ָ��
}AUTOIT3_DECOMPILER, *PAUTOIT3_DECOMPILER;

/*
//AUTOIT3�����ļ��ṹ
typedef struct _AUTOIT3_COMPILE_OBJECT
{
	BYTE guidAutoIT[16];	//�ű���ʼ��GUID��ʶ,����ΪA3484BBE986C4AA9994C530A86D6487D
	//�ű�������������ͨ�����������������

	DWORD dwMajorVer;		//AUTO�����汾��, "AU3!"  0x21335541
	DWORD dwCompVer;		//AUTO��ѹ������汾��, "EA05" "EA06"

	//16�ֽ�У������,EA05���ڼ����������(EA06ʵ����δ�õ�,�����Ϊ��������)
	BYTE cbChkSum[16];
	
	//���濪ʼ������ݽṹ�ǿ��ظ���(���ȱ仯)
	DWORD dwTypeFlag;		//�������ͱ�ʶ,���ܺ�Ϊ"FILE"
	DWORD dwFileTypeStrLen;	//�ļ������ַ�������
	WCHAR szTypeString[dwFileTypeStrLen];	//�ļ������ַ���
	//Ŀǰ������L">>>AUTOIT NO CMDEXECUTE<<<" �� L">>>AUTOIT SCRIPT<<<"
	//���ھɰ汾���Ի������������ַ�����ʶ

	DWORD dwFilePathStrLen;	//�ļ�·���ַ�������
	WCHAR szPathString[dwFilePathStrLen];	//�ļ�·���ַ���(�Է�����ű�û��ʵ���ô�)

	BYTE bCompressFlag;		//ѹ����ʶ
	DWORD dwDataSize;		//�������ݵĴ�С
	{
		//�ű�ʱ��ʹ������ṹ,�����ֱ��������24�ֽ�
		DWORD dwUncompSize;		//���ݽ�ѹ����Ĵ�С
		DWORD dwChkSum;			//У���
		BYTE cbUnuseData[16];	//Ϊ�ű�����ʱ�Ǵ�δʹ�õ�������,ֱ������
	}
	BYTE cbData[dwDataSize];	//����������,EA05ֻҪ���ܳɹ��������,EA06��Ҫ�ٽ��нű���ԭ
}AUTOIT3_COMPILE_OBJECT, *PAUTOIT3_COMPILE_OBJECT;
*/