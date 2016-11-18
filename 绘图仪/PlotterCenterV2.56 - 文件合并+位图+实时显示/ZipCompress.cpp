//ZipCompress.cpp
/********************************************************************
����ʱ��:  2013/06/09
�� �� ��:  ZipCompress.cpp
��������:  �� ��
�ļ�������
           ʹ��zipѹ��/��ѹ
�ļ��޸ģ�
�޸�ʱ��        �޸�����        �޸�����
*********************************************************************/

#include "stdafx.h"
#include "ZipCompress.h"
#include "ZipFile_Memory.h"
#include "UnzipFile_Memory.h"
#include "ZipFile.h"
#include "UnzipFile.h"

//�Ը������ڴ����ѹ��
//[����] pUnCompressData ��Ҫѹ��������
//[����]  iUnCompressCount�ڴ��С
//[����][���]  pCompressData ѹ���������,�ڴ��ں���������
//[����][���]  iCompressCount ���: ѹ����Ĵ�С,ʵ��ʹ�õĴ�С; ����: ΪpCompressData����������Ŀռ��С
void CompressMemory(char *pUnCompressData, int iUnCompressCount,  char *&pCompressData, int &iCompressCount)
{
  CZipFile_Memory uf(pCompressData, iCompressCount);
  zip_fileinfo zipfi;
  CString strName;

  strName.Empty();
  uf.OpenNewFileInZip(strName, zipfi);
  uf.WriteInFileInZip(pUnCompressData, iUnCompressCount);
  uf.CloseFileInZip();
  iCompressCount = uf.Close();
}


//�Ը������ڴ���н�ѹ
//�������:
//  pCompressData ѹ���������
//  iCompressCount ѹ����Ĵ�С
//���������
//  pUnCompressData ��Ҫѹ��������,�ڴ��ں���������,��Ҫ�ں�����ɾ��
//  iUnCompressCount �ڴ��С
void UnCompressMemory(char *pCompressData, int iCompressCount, char *&pUnCompressData, int &iUnCompressCount)
{
  char buf[BUF_SIZE],*pData;
  int size_read, iIndex;
  
  CUnzipFile_Memory uf(pCompressData, iCompressCount);
  
  iUnCompressCount = 0;
  pUnCompressData = NULL;
  
  uf.OpenCurrentFile();
  iUnCompressCount = uf.GetCurUnCompressFileSize();
  //pUnCompressData = (char*)malloc(iUnCompressCount);
  pUnCompressData = new char[iUnCompressCount];
  iIndex = 0;
  
  do
  {
    size_read = uf.ReadCurrentFile(buf, BUF_SIZE);
    if (size_read > 0)
    {
      pData = &pUnCompressData[iIndex];
      memcpy(pData, buf, size_read);
      iIndex = iIndex + size_read;
    }
  }
  while (size_read == BUF_SIZE);
  
  uf.Close();
}

//�Ը������ļ�����ѹ��
//���������
//  strFilePath ��Ҫѹ�����ļ�·��(ȫ·���������ļ�������׺)
//  strFileName ѹ�����ļ�����(�ļ������ļ���׺)
//  strName ѹ�����ļ���
//���������
//  strSaveName �����ѹ���ļ�(ȫ·��)
void CompressFile(CString strFilePath, CString strFileName, CString strSaveName)
{
  CZipFile uf(strSaveName, FALSE);
  zip_fileinfo zipfi;
  char buf[BUF_SIZE];

  uf.OpenNewFileInZip(strFileName, zipfi);

  CFile f( strFilePath, CFile::modeRead);
  int size_read;
  do
  {
    size_read = f.Read(buf, BUF_SIZE);
    if (size_read > 0)
    {
      uf.WriteInFileInZip(buf, size_read);
    }
  }
  while (size_read == BUF_SIZE);

  uf.CloseFileInZip();
  uf.Close();
}


//�Ը������ļ����н�ѹ
//�������:
//  strZipFilePath ZIP�ļ�·��(�����ļ����ͺ�׺)
//�������:
// strSaveName ������ļ�·��(ֻ��Ҫ·����û���ļ���)
void UnCompressFile(CString strZipFilePath,  CString strSavePath)
{
  CUnzipFile uf(strZipFilePath);
  do
  {
    unz_file_info ui;
    
    // 	getting the information about the file 
    // (date, attributes, the size of the filename)
    uf.GetCurrentFileInfo(&ui);
    
    int iNameSize = ui.size_filename + 1;
    char* pName = new char [iNameSize];
    
    // get the name of the file
    uf.GetCurrentFileInfo(NULL, pName, iNameSize);
    
    //		TCHAR szDir[_MAX_DIR];
    //		TCHAR szDrive[_MAX_DRIVE];
    //		_tsplitpath(OpenZipFileDlg.GetPathName(), szDrive, szDir,NULL, NULL);
    //		CString szPath = CString(szDrive) + szDir;
    CFile f( strSavePath + pName, CFile::modeWrite | CFile::modeCreate);
    
    delete[] pName;
    uf.OpenCurrentFile();
    char buf[BUF_SIZE];
    
    int size_read;
    do
    {
      size_read = uf.ReadCurrentFile(buf, BUF_SIZE);
      if (size_read > 0)
        f.Write(buf, size_read);
    }
    while (size_read == BUF_SIZE);
    
    // set the original date stamp and attributes to the unpacked file
    // (this function closes the file "f" which is needed to apply
    // the attributes)
    uf.UpdateFileStatus(f, ui);
  }while(uf.GoToNextFile());
  // cannot be called by the destructor 
  uf.Close();	
}