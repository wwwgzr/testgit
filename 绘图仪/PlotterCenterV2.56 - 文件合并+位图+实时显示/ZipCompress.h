//ZipCompress.h
#if !defined(ZIPCOMPRESS_H)
#define ZIPCOMPRESS_H

#define BUF_SIZE 2048

//�Ը������ڴ����ѹ��
//[����] pUnCompressData ��Ҫѹ��������
//[����]  iUnCompressCount�ڴ��С
//[����][���]  pCompressData ѹ���������,�ڴ��ں���������
//[����][���]  iCompressCount ���: ѹ����Ĵ�С,ʵ��ʹ�õĴ�С; ����: ΪpCompressData����������Ŀռ��С,����ռ�Ĵ�С: iUnCompressCount + 100 , 100Ϊzipѹ��ͷ�ļ���С
void CompressMemory(char *pUnCompressData, int iUnCompressCount,  char *&pCompressData, int &iCompressCount);


//�Ը������ڴ���н�ѹ
//�������:
//  pCompressData ѹ���������
//  iCompressCount ѹ����Ĵ�С
//���������
//  pUnCompressData ��Ҫѹ��������,�ڴ��ں���������,��Ҫ�ں�����ɾ��
//  iUnCompressCount �ڴ��С
void UnCompressMemory(char *pCompressData, int iCompressCount, char *&pUnCompressData, int &iUnCompressCount);


//�Ը������ļ�����ѹ��
//���������
//  strFilePath ��Ҫѹ�����ļ�·��(ȫ·���������ļ�������׺)
//  strFileName ѹ�����ļ�����(�ļ������ļ���׺)
//  strName ѹ�����ļ���
//���������
//  strZipSaveName �����ѹ���ļ�(ȫ·��)
void CompressFile(CString strFilePath, CString strFileName, CString strZipSaveName);


//�Ը������ļ����н�ѹ
//�������:
//  strZipFilePath ZIP�ļ�·��(�����ļ����ͺ�׺)
//�������:
// strSaveName ������ļ�·��(ֻ��Ҫ·����û���ļ���)
void UnCompressFile(CString strZipFilePath,  CString strSavePath);

#endif