// ZUBaseFile_Memory.h: interface for the CZUBaseFile_Memory class.
//
// Copyright (C) 2000 Tadeusz Dracz
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZUBASEFILE_MEMORY_H__AEA985C0_D813_11D3_B7C7_8E2FB86D3347__INCLUDED_)
#define AFX_ZUBASEFILE_MEMORY_H__AEA985C0_D813_11D3_B7C7_8E2FB86D3347__INCLUDED_

#include "zlib.h"
#include "ZipException.h"
class CPtrList;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// base class for CUnzipFile_Memory and CZipFile_Memory
class CZUBaseFile_Memory  
{
public:
	CZUBaseFile_Memory();
	virtual ~CZUBaseFile_Memory();
	// specify whether to control memory allocation and freeing by zlib library
	// strongly suggested to set it to true (default)
	bool m_bDetectZlibMemoryLeaks;

  //设置内存开始读入位置
  void Seek(LONG lOff);

  //读内存内容
  UINT Read(void* lpBuf, UINT nCount);

  //将内容写入内存
  void Write(const void* lpBuf, UINT nCount);

  //偏移量指向内存尾部
  void SeekToEnd();

  //得到当前偏移位置的位置
  DWORD GetPosition();

  //在内存最后位置写入结束符
  //返回值：
  //  内存大小
  LONG WriteCloseMemory();

protected:
	CPtrList m_list;
	void CheckForError(int err);
	void ThrowError(int err);
	static void myfree(void* opaque, void* address);
	static void* myalloc(void* opaque, UINT items, UINT size);
	bool IsClosed();
	//CFile* m_pFile;
	bool m_bZipFile;
  char *m_pBufferData;
  LONG m_lDataOffset;
  LONG m_lDataCount;
};

#endif // !defined(AFX_ZUBASEFILE_MEMORY_H__AEA985C0_D813_11D3_B7C7_8E2FB86D3347__INCLUDED_)
