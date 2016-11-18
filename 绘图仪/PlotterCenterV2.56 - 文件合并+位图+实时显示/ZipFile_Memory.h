
/* zip.h -- IO for compress .zip files using zlib 
   Version 0.15 alpha, Mar 19th, 1998,

   Copyright (C) 1998 Gilles Vollant

   This unzip package allow creates .ZIP file, compatible with PKZip 2.04g
     WinZip, InfoZip tools and compatible.
   Encryption and multi volume ZipFile (span) are not supported.
   Old compressions used by old PKZip 1.x are not supported

  For uncompress .zip file, look at unzip.h

   THIS IS AN ALPHA VERSION. AT THIS STAGE OF DEVELOPPEMENT, SOMES API OR STRUCTURE
   CAN CHANGE IN FUTURE VERSION !!
   I WAIT FEEDBACK at mail info@winimage.com
   Visit also http://www.winimage.com/zLibDll/zip.htm for evolution

   Condition of use and distribution are the same than zlib :

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.


*/

/* for more info about .ZIP format, see 
      ftp://ftp.cdrom.com/pub/infozip/doc/appnote-970311-iz.zip
   PkWare has also a specification at :
      ftp://ftp.pkware.com/probdesc.zip
*/

// modified by Tadeusz Dracz - 01.2000:
//		- added class' wrapers
//		- several bugs fixed
//		- several enhancements added
//		- MFC support added
//		- memory leaks eliminated when write error occured
//		- automaticaly free used memory on destruction or exception
//		- modern error notification using exceptions
// Copyright (C) Tadeusz Dracz  
// This notice may not be removed or altered from any source distribution.

// ZipFile_Memory.h: interface for the CZipFile_Memory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIPFILE_MEMORY_H__7F795F27_D6BD_11D3_B7C7_BBF03FA53147__INCLUDED_)
#define AFX_ZIPFILE_MEMORY_H__7F795F27_D6BD_11D3_B7C7_BBF03FA53147__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZUBaseFile_Memory.h"
#include "ZipStruct.h"

struct zip_internal_Memory
{
  zip_internal_Memory();
  ~zip_internal_Memory();
  char *m_pZipbuf;  //压缩后的数据
  linkedlist_data central_dir;/* datablock with central dir in construction*/
  int  in_opened_file_inzip;  /* 1 if a file in the zip is currently writ.*/
  struct curfile_info ci;            /* info on the file curretly writing */
  
  DWORD begin_pos;            /* position of the beginning of the zipfile */
  DWORD number_entry;
};


class CZipFile_Memory : public CZUBaseFile_Memory
{
public:
	CZipFile_Memory();
	void Open(void* pBuf, LONG lCount);
/*
  Create a zipfile.
	 if the file pathname exist and append is true, the zip will be created at the end
	   of the file. (useful if the file contain a self extractor code)
*/

	CZipFile_Memory(void* pBuf, LONG lCount);
/*
  Open a file in the ZIP for writing.
  filename : the filename in zip (if NULL, '-' without quote will be used
  *zipfi contain supplemental information
  if extrafield_local!=NULL and size_extrafield_local>0, extrafield_local
    contains the extrafield data the the local header
  if extrafield_global!=NULL and size_extrafield_global>0, extrafield_global
    contains the extrafield data the the local header
  if comment != NULL, comment contain the comment string
  method contain the compression method (0 for store, Z_DEFLATED for deflate)
  level contain the level of compression (can be Z_DEFAULT_COMPRESSION)
*/

	void OpenNewFileInZip (	CString filename, 
							zip_fileinfo& zipfi, 
							int level = Z_DEFAULT_COMPRESSION,
							CString comment = "", 
							const void* extrafield_local = NULL, 
							uInt size_extrafield_local = 0, 
							const void* extrafield_global = NULL, 
							uInt size_extrafield_global = 0, 
							int method = Z_DEFLATED);
/*
  Write data in the zipfile_Memory
*/

	void WriteInFileInZip(const void *buf, UINT len);
/*
  Close the current file in the zipfile
*/

	void CloseFileInZip();
/*
  Close the zipfile and the current file in the zipfile if is opened
  返回值：压缩内存的大小
*/
	int Close(CString global_comment = "");
/*
	Fill in zip_fileinfo fields (time an attributes) according to CFile data
*/
	void UpdateZipInfo(zip_fileinfo &zi, CFile &f);
	virtual ~CZipFile_Memory();

protected:
	void ziplocal_putValue_inmemory(Byte dest, uLong x, int nbByte);
	void ziplocal_putValue(uLong x, int nbByte);
	zip_internal_Memory zi;
};

#endif // !defined(AFX_ZIPFILE_MEMORY_H__7F795F27_D6BD_11D3_B7C7_BBF03FA53147__INCLUDED_)
