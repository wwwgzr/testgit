//ZipCompress.cpp
/********************************************************************
创建时间:  2013/06/09
文 件 名:  ZipCompress.cpp
创建作者:  徐 琼
文件描述：
           使用zip压缩/解压
文件修改：
修改时间        修改作者        修改内容
*********************************************************************/

#include "stdafx.h"
#include "ZipCompress.h"
#include "ZipFile_Memory.h"
#include "UnzipFile_Memory.h"
#include "ZipFile.h"
#include "UnzipFile.h"

//对给定的内存进行压缩
//[输入] pUnCompressData 需要压缩的数据
//[输入]  iUnCompressCount内存大小
//[输入][输出]  pCompressData 压缩后的数据,内存在函数外申请
//[输入][输出]  iCompressCount 输出: 压缩后的大小,实际使用的大小; 输入: 为pCompressData在外面申请的空间大小
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


//对给定的内存进行解压
//输入参数:
//  pCompressData 压缩后的数据
//  iCompressCount 压缩后的大小
//输出参数：
//  pUnCompressData 需要压缩的数据,内存在函数中申请,需要在函数外删除
//  iUnCompressCount 内存大小
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

//对给定的文件进行压缩
//输入参数：
//  strFilePath 需要压缩的文件路径(全路径，包含文件名，后缀)
//  strFileName 压缩的文件名称(文件名，文件后缀)
//  strName 压缩的文件名
//输出参数：
//  strSaveName 保存的压缩文件(全路径)
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


//对给定的文件进行解压
//输入参数:
//  strZipFilePath ZIP文件路径(包含文件名和后缀)
//输出参数:
// strSaveName 保存的文件路径(只需要路径，没有文件名)
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