/* unzip.h -- IO for uncompress .zip files using zlib 
   Version 0.15 beta, Mar 19th, 1998,

   Copyright (C) 1998 Gilles Vollant

   This unzip package allow extract file from .ZIP file, compatible with PKZip 2.04g
     WinZip, InfoZip tools and compatible.
   Encryption and multi volume ZipFile (span) are not supported.
   Old compressions used by old PKZip 1.x are not supported

   THIS IS AN ALPHA VERSION. AT THIS STAGE OF DEVELOPPEMENT, SOMES API OR STRUCTURE
   CAN CHANGE IN FUTURE VERSION !!
   I WAIT FEEDBACK at mail info@winimage.com
   Visit also http://www.winimage.com/zLibDll/unzip.htm for evolution

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
      ftp://ftp.pkware.com/probdesc.zip */

// modified by Tadeusz Dracz - 01.2000:
//		- added class' wrapers
//		- several bugs fixed
//		- several enhancements added
//		- MFC support added
//		- memory leaks eliminated when read error occured
//		- automaticaly free used memory on destruction or exception
//		- modern error notification using exceptions
// Copyright (C) 2000 Tadeusz Dracz  
// This notice may not be removed or altered from any source distribution.



// UnzipFile_Memory.h: interface for the CUnzipFile_Memory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNZIPFILE_MEMORY_H__44D45340_D71F_11D3_B7C7_E77339672847__INCLUDED_)
#define AFX_UNZIPFILE_MEMORY_H__44D45340_D71F_11D3_B7C7_E77339672847__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZUBaseFile_Memory.h"
#include "ZipStruct.h"

/* unz_s contain internal information about the zipfile
*/
struct unz_s_Memory
{
	unz_s_Memory();
	~unz_s_Memory();
	char *m_pfilebuf;  // 需要解压的内存
  unz_global_info gi;       /* public global information */
	uLong byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
	uLong num_file;             /* number of the current file in the zipfile 当前解压的文件在整个文件个数中的索引*/
	uLong pos_in_central_dir;   /* pos of the current file in the central dir 在中心目录结构中当前文件的相对位置*/
	uLong current_file_ok;      /* flag about the usability of the current file 当前文件是否处于解压状态*/
	uLong central_pos;          /* position of the beginning of the central dir 中心目录结构开始的相对位置*/

	uLong size_central_dir;     /* size of the central directory 中心目录结构大小 */
	uLong offset_central_dir;   /* offset of start of central directory with
								   respect to the starting disk number  中心目录的开始偏移量，该偏移量相对磁盘编号*/

	unz_file_info cur_file_info; /* public info about the current file in zip 在压缩文件中当前解压文件的公开信息*/
	unz_file_info_internal cur_file_info_internal; /* private info about it 在压缩文件中当前解压文件的隐藏信息*/
    file_in_zip_read_info* pfile_in_zip_read; /* structure about the current
	                                    file if we are decompressing it 当前解压文件解压时的结构 */
public:
	void free_pfile_in_zip_read();
	void alloc_pfile_in_zip_read();
};



class CUnzipFile_Memory : public CZUBaseFile_Memory
{
public:
/*
 fill in attibutes and time fields of the fileStatus with unz_file_info data
 WARNING: if the file is opened the function closes it
*/
	CUnzipFile_Memory();
	void Open(void* pBuf, LONG lCount);
/*
  Give the current position in uncompressed data
*/
	z_off_t tell();
/*
  return true if the end of file was reached, false elsewhere 
*/
	bool eof();
/*
  Get the global comment string of the ZipFile, in the szComment buffer.
  uSizeBuf is the size of the szComment buffer.
  return the number of byte copied
*/
	int GetGlobalComment (char* szComment, uLong uSizeBuf);
/*
  Read extra field from the current file (opened by OpenCurrentFile)
  This is the local-header version of the extra field (sometimes, there is
    more info in the local-header version than in the central-header)

  if buf==NULL, it return the size of the local extra field

  if buf!=NULL, len is the size of the buffer, the extra header is copied in
	buf.
  the return value is the number of bytes copied in buf
*/
	int GetLocalExtrafield (void* buf, UINT len);
/*
  Get Info about the current file
  if file_info structure will contain somes info about
	    the current file
  if szFileName!=NULL, the filemane string will be copied in szFileName
			(fileNameBufferSize is the size of the buffer)
  if extraField!=NULL, the extra field information will be copied in extraField
			(extraFieldBufferSize is the size of the buffer).
			This is the Central-header version of the extra field
  if szComment!=NULL, the comment string of the file will be copied in szComment
			(commentBufferSize is the size of the buffer)
*/
	void GetCurrentFileInfo ( unz_file_info* file_info, LPSTR szFileName = NULL, 
		uLong fileNameBufferSize = 0, void *extraField = NULL, uLong extraFieldBufferSize = 0,
		LPSTR szComment = NULL, uLong commentBufferSize = 0);
/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
*/
	void GetGlobalInfo(unz_global_info & global_info);
	
/***************************************************************************/
/* Unzip package allow you browse the directory of the zipfile */

/*
  Set the current file of the zipfile to the first file.
*/
	void GoToFirstFile();
/*
  Set the current file of the zipfile to the next file.
  return true if there is no problem
  return false if the actual file was the latest.
*/
	bool GoToNextFile();
/*
  Try locate the file szFileName in the zipfile.
  For the iCaseSensitivity signification, see unzStringFileNameCompare

  return value :
  true if the file is found. It becomes the current file.
  false if the file is not found
*/
	bool LocateFile(CString szFileName, bool bCaseSensitive = false);
/*
  Open for reading data the current file in the zipfile.
*/
	void OpenCurrentFile();
/*
  Close the file in zip opened with OpenCurrentFile
  throw badCrc error if all the file was read but the CRC is not good
*/
	void CloseCurrentFile();
/*
  Read bytes from the current file (opened by OpenCurrentFile)
  buf contain buffer where data must be copied
  len the size of buf.

  return the number of byte copied if somes bytes are copied
  return 0 if the end of file was reached
*/
	int ReadCurrentFile(void* buf, UINT len);

  //得到当前文件未压缩大小
  int GetCurUnCompressFileSize();

	void Close();
	int StringFileNameCompare(CString fileName1, CString fileName2, bool caseSensitive = false);
	CUnzipFile_Memory(void* pBuf, LONG lCount);
	virtual ~CUnzipFile_Memory();
protected:
	void unzlocal_GetCurrentFileInfoInternal( unz_file_info & file_info, 
		unz_file_info_internal & file_info_internal, LPSTR szFileName,
		uLong fileNameBufferSize, void *extraField, uLong extraFieldBufferSize,
		LPSTR szComment, uLong commentBufferSize);

	void unzlocal_DosDateToTmuDate(unz_file_info & file_info);
	void unzlocal_CheckCurrentFileCoherencyHeader (uInt & iSizeVar, uLong & offset_local_extrafield, uLong & size_local_extrafield);
	uLong unzlocal_SearchCentralDir();
	void unzlocal_getLong (uLong & pX);
	void unzlocal_getShort (uLong & pX);
	void unzlocal_getByte(int & pi);
	unz_s_Memory uf;

};

#endif // !defined(AFX_UNZIPFILE_MEMORY_H__44D45340_D71F_11D3_B7C7_E77339672847__INCLUDED_)
