// ZUBaseFile_Memory.cpp: implementation of the CZUBaseFile_Memory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZUBaseFile_Memory.h"
#include <afxcoll.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZUBaseFile_Memory::CZUBaseFile_Memory()
{
	m_bZipFile = true;
	m_bDetectZlibMemoryLeaks = true;
	//m_pFile = NULL;
  m_pBufferData = NULL;
  m_lDataOffset = 0;
  m_lDataCount = 0;
}

CZUBaseFile_Memory::~CZUBaseFile_Memory()
{
	if (m_list.GetCount())
	{
// some memory hasn't been freed due to an error in zlib, so free it now
		POSITION pos = m_list.GetHeadPosition();
		while (pos)
			delete[] m_list.GetNext(pos);
		
	}
}

bool CZUBaseFile_Memory::IsClosed()
{
	//return  !m_pFile || (m_pFile->m_hFile == CFile::hFileNull);
  return  !m_pBufferData;
}

void* CZUBaseFile_Memory::myalloc(void* opaque, UINT items, UINT size)
{
	void* p = new char[size * items];
	if (opaque)
	{
		CPtrList* list  = (CPtrList*) opaque;
		list->AddTail(p);
	}
	return p;
}

void CZUBaseFile_Memory::myfree(void* opaque, void* address)
{
	if (opaque)
	{
		CPtrList* list  = (CPtrList*) opaque;
		POSITION pos = list->Find(address);
		if (pos)
			list->RemoveAt(pos);
	}
	delete[] address;
}

void CZUBaseFile_Memory::ThrowError(int err)
{
	//CString fileName = IsClosed() ? "" : m_pFile->GetFilePath();
  CString fileName = "file";
	AfxThrowZipException(err, fileName, m_bZipFile);
}

void CZUBaseFile_Memory::CheckForError(int err)
{
// 	ASSERT (err != Z_NEED_DICT);
// 	ASSERT (err != Z_STREAM_END);

	if ((err == Z_OK) || (err == Z_NEED_DICT))
		return;

	ThrowError(err);

}

//�����ڴ濪ʼ����λ��
void CZUBaseFile_Memory::Seek(LONG lOff)
{
  m_lDataOffset = lOff;
}

//���ڴ�����
UINT CZUBaseFile_Memory::Read(void* lpBuf, UINT nCount)
{
  UINT uResult;
  char *pData;

  uResult = 0;
  pData = &m_pBufferData[m_lDataOffset];
  if ((m_lDataOffset + (LONG)nCount + 1) > m_lDataCount)
  {  //��������ݳ�����Χ
    uResult = m_lDataCount - m_lDataOffset - 1;
  }
  else
  {
    uResult = nCount;
  }
  if (uResult > 0)
  {
    memcpy(lpBuf, pData, uResult);
    m_lDataOffset = m_lDataOffset + uResult;
  }
  else
  {
    uResult = 0;
  }
  return uResult;
}

//������д���ڴ�
void CZUBaseFile_Memory::Write(const void* lpBuf, UINT nCount)
{
  UINT uResult;
  CHAR *pData;
  
  uResult = 0;
  pData = &m_pBufferData[m_lDataOffset];
  if ((m_lDataOffset + (LONG)nCount + 1) > m_lDataCount)
  {  //д������ݳ�����Χ
    uResult = m_lDataCount - m_lDataOffset - 1;
  }
  else
  {
    uResult = nCount;
  }

  if (uResult > 0)
  {
    memcpy(pData,lpBuf, uResult);
    m_lDataOffset = m_lDataOffset + uResult;
  }
}

//ƫ����ָ���ڴ�β��
void CZUBaseFile_Memory::SeekToEnd()
{
  m_lDataOffset = m_lDataCount - 1;
}

//�õ���ǰƫ��λ�õ�λ��
DWORD CZUBaseFile_Memory::GetPosition()
{
  return m_lDataOffset;
}

LONG CZUBaseFile_Memory::WriteCloseMemory()
{
  m_pBufferData[m_lDataOffset] = '\0';
  return (m_lDataOffset + 1);
}