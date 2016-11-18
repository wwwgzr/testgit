// UnzipFile_Memory.cpp: implementation of the CUnzipFile_Memory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UnzipFile_Memory.h"

unz_s_Memory::unz_s_Memory()
{
	pfile_in_zip_read = NULL;
  m_pfilebuf = NULL;
}

void unz_s_Memory::alloc_pfile_in_zip_read()
{
	if (!pfile_in_zip_read)
		pfile_in_zip_read = new file_in_zip_read_info;
}

void unz_s_Memory::free_pfile_in_zip_read()
{
	if (pfile_in_zip_read)
	{
		delete pfile_in_zip_read;
		pfile_in_zip_read = NULL;
	}
}

unz_s_Memory::~unz_s_Memory()
{
	free_pfile_in_zip_read();
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnzipFile_Memory::CUnzipFile_Memory()
{
	m_bZipFile = false;
	m_pBufferData = NULL;
  m_lDataCount = 0;
  m_lDataOffset = 0;
}


CUnzipFile_Memory::CUnzipFile_Memory(void* pBuf, LONG lCount)
{
	m_bZipFile = false;
  m_pBufferData = NULL;
  m_lDataCount = 0;
  m_lDataOffset = 0;
	Open(pBuf,lCount);
}

//   Close a ZipFile
// 关闭压缩文件
void CUnzipFile_Memory::Close()
{
	if (IsClosed())
		return;

    if (uf.pfile_in_zip_read)
        CloseCurrentFile();
	
	//uf.file.Close();
}

//   Close the file in zip opened with OpenCurrentFile
// 关闭对压缩文件的单独文件的读入
void CUnzipFile_Memory::CloseCurrentFile()
{

	if (uf.pfile_in_zip_read == NULL)
		return; // already closed - do not throw an exception

	if (uf.pfile_in_zip_read->rest_read_uncompressed == 0)
	{
		if (uf.pfile_in_zip_read->crc32 != uf.pfile_in_zip_read->crc32_wait)
			ThrowError(UNZ_CRCERROR);
	}

	
	if (uf.pfile_in_zip_read->stream_initialised)
		inflateEnd(&uf.pfile_in_zip_read->stream);

	uf.free_pfile_in_zip_read();

}

/*
  Set the current file of the zipfile to the first file.
  得到压缩文件的第一个单独文件
*/
void CUnzipFile_Memory::GoToFirstFile()
{
	uf.pos_in_central_dir = uf.offset_central_dir;
	uf.num_file = 0;
	unzlocal_GetCurrentFileInfoInternal(uf.cur_file_info,
										uf.cur_file_info_internal,
										NULL,0,NULL,0,NULL,0);
	uf.current_file_ok = 1;
}


/*
  Try locate the file szFileName in the zipfile.

  return value :
  true if the file is found. It becomes the current file.
  false if the file is not found
*/
bool CUnzipFile_Memory::LocateFile(CString szFileName, bool bCaseSensitive)
{

    if (szFileName.GetLength() >= UNZ_MAXFILENAMEINZIP)
        ThrowError(UNZ_PARAMERROR);

	if (!uf.current_file_ok)
		return false;

	uLong num_fileSaved = uf.num_file;
	uLong pos_in_central_dirSaved = uf.pos_in_central_dir;

	GoToFirstFile();

	bool res = true;
	while (res)
	{
		char szCurrentFileName[UNZ_MAXFILENAMEINZIP+1];
		GetCurrentFileInfo(NULL, szCurrentFileName, sizeof(szCurrentFileName)-1,
								NULL,0,NULL,0);
		if (StringFileNameCompare(szCurrentFileName, szFileName, bCaseSensitive)==0)
			return true;

		res = GoToNextFile();
	}

	uf.num_file = num_fileSaved ;
	uf.pos_in_central_dir = pos_in_central_dirSaved ;

	return false;
}



/*
  Set the current file of the zipfile to the next file.
  return true if there is no problem
  return false if the actual file was the latest.
  如果压缩文件中存在多个单独的文件，该函数得到下一个单独的文件
*/
bool CUnzipFile_Memory::GoToNextFile()
{
	if (!uf.current_file_ok)
		return false;

	if (uf.num_file + 1 == uf.gi.number_entry)
		return false;

	uf.pos_in_central_dir += SIZECENTRALDIRITEM + uf.cur_file_info.size_filename +
			uf.cur_file_info.size_file_extra + uf.cur_file_info.size_file_comment ;

	uf.num_file++;
	unzlocal_GetCurrentFileInfoInternal(uf.cur_file_info,
										uf.cur_file_info_internal,
										NULL,0,NULL,0,NULL,0);
	uf.current_file_ok = 1;
	return true;
}

/*
  Read the local header of the current zipfile
  Check the coherency of the local header and info in the end of central
        directory about this file
  store in *piSizeVar the size of extra info in local header
        (filename and size of extra field data)
   读取分文件头信息(0x04034b50)
*/
void CUnzipFile_Memory::unzlocal_CheckCurrentFileCoherencyHeader (uInt & iSizeVar,
													uLong & offset_local_extrafield,
													uLong & size_local_extrafield)
{

	iSizeVar = 0;
	offset_local_extrafield = 0;
	size_local_extrafield = 0;

	//uf.file.Seek(uf.cur_file_info_internal.offset_curfile + uf.byte_before_the_zipfile, CFile::begin);
  Seek(uf.cur_file_info_internal.offset_curfile + uf.byte_before_the_zipfile);

  uLong uMagic;
	unzlocal_getLong(uMagic);
	if (uMagic!=0x04034b50)
		ThrowError(UNZ_BADZIPFILE);

	uLong uData;
	unzlocal_getShort(uData);

// 	if (uData != uf.cur_file_info.version)
// 		ThrowError(UNZ_BADZIPFILE);

	uLong uFlags;
	unzlocal_getShort(uFlags);

	unzlocal_getShort(uData);
	if (uData != uf.cur_file_info.compression_method)
		ThrowError(UNZ_BADZIPFILE);

    if ((uf.cur_file_info.compression_method != 0) &&
                         (uf.cur_file_info.compression_method != Z_DEFLATED))
        ThrowError(UNZ_BADZIPFILE);

	unzlocal_getLong(uData); /* date/time */

	unzlocal_getLong(uData); /* crc */
	if ((uData != uf.cur_file_info.crc) &&
		                      ((uFlags & 8)==0))
		ThrowError(UNZ_BADZIPFILE);

	unzlocal_getLong(uData); /* size compr */

	if ((uData != uf.cur_file_info.compressed_size) &&
							  ((uFlags & 8) == 0))
		ThrowError(UNZ_BADZIPFILE);

	unzlocal_getLong(uData); /* size uncompr */

	if ((uData!=uf.cur_file_info.uncompressed_size) && 
							  ((uFlags & 8)==0))
		ThrowError(UNZ_BADZIPFILE);

	uLong size_filename;
	unzlocal_getShort(size_filename);

	if ((size_filename != uf.cur_file_info.size_filename))
		ThrowError(UNZ_BADZIPFILE);

	iSizeVar = (uInt)size_filename;

	uLong size_extra_field;
	unzlocal_getShort(size_extra_field);

	offset_local_extrafield = uf.cur_file_info_internal.offset_curfile +
									SIZEZIPLOCALHEADER + size_filename;
	size_local_extrafield = (uInt)size_extra_field;

	iSizeVar += (uInt)size_extra_field;

}


//   Open for reading data the current file in the zipfile.
// 先关闭当前的单独文件， 得到文件的头信息(0x4034b50),申请读入过程需要的空间
void CUnzipFile_Memory::OpenCurrentFile()
{

	ASSERT(uf.current_file_ok);

	if (!uf.current_file_ok)
		ThrowError(UNZ_PARAMERROR);

    if (uf.pfile_in_zip_read)
        CloseCurrentFile();

	uInt iSizeVar;
	uLong offset_local_extrafield;  /* offset of the local extra field */
	uLong  size_local_extrafield;    /* size of the local extra field */

	unzlocal_CheckCurrentFileCoherencyHeader(iSizeVar,
				offset_local_extrafield, size_local_extrafield);

	uf.alloc_pfile_in_zip_read();


	uf.pfile_in_zip_read->offset_local_extrafield = offset_local_extrafield;
	uf.pfile_in_zip_read->size_local_extrafield = size_local_extrafield;

	
	if ((uf.cur_file_info.compression_method!=0) &&
        (uf.cur_file_info.compression_method!=Z_DEFLATED))
		ThrowError(UNZ_BADZIPFILE);


	uf.pfile_in_zip_read->crc32_wait = uf.cur_file_info.crc;
	uf.pfile_in_zip_read->compression_method =
            uf.cur_file_info.compression_method;
	
	uf.pfile_in_zip_read->byte_before_the_zipfile=uf.byte_before_the_zipfile;


	if (uf.cur_file_info.compression_method != 0)
	{
	  uf.pfile_in_zip_read->stream.zalloc = (alloc_func)myalloc;
	  uf.pfile_in_zip_read->stream.zfree = (free_func)myfree;
	  uf.pfile_in_zip_read->stream.opaque = m_bDetectZlibMemoryLeaks ? &m_list : 0;
      
	  int err = inflateInit2(&uf.pfile_in_zip_read->stream, -MAX_WBITS);

	  if (err == Z_OK)
	    uf.pfile_in_zip_read->stream_initialised=1;
	  else 
		  CheckForError(err);
        /* windowBits is passed < 0 to tell that there is no zlib header.
         * Note that in this case inflate *requires* an extra "dummy" byte
         * after the compressed stream in order to complete decompression and
         * return Z_STREAM_END. 
         * In unzip, i don't wait absolutely Z_STREAM_END because I known the 
         * size of both compressed and uncompressed data
         */
	}
	uf.pfile_in_zip_read->rest_read_compressed = 
            uf.cur_file_info.compressed_size ;
	uf.pfile_in_zip_read->rest_read_uncompressed = 
            uf.cur_file_info.uncompressed_size ;

	
	uf.pfile_in_zip_read->pos_in_zipfile = 
            uf.cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER + 
			  iSizeVar;
	
	uf.pfile_in_zip_read->stream.avail_in = (uInt)0;

}
/*
  Write info about the ZipFile in the *pglobal_info structure.
  No preparation of the structure is needed
*/
void CUnzipFile_Memory::GetGlobalInfo(unz_global_info &global_info)
{
	global_info = uf.gi;
}


//主要读取中心目录结构中的内容
void CUnzipFile_Memory::GetCurrentFileInfo ( unz_file_info* file_info,
                                         LPSTR szFileName,
										 uLong fileNameBufferSize,
										 void *extraField,
										 uLong extraFieldBufferSize,
										 LPSTR szComment,
										 uLong commentBufferSize)
{

	
	unz_file_info temp;
	if (!file_info)
		file_info = &temp;

	unz_file_info_internal temp1;
	unzlocal_GetCurrentFileInfoInternal(*file_info, temp1, szFileName, fileNameBufferSize,
		extraField, extraFieldBufferSize, szComment, commentBufferSize);
}



/*
  Read bytes from the current file.
  buf contain buffer where data must be copied
  len the size of buf.

  return the number of byte copied if somes bytes are copied
  return 0 if the end of file was reached
*/
int CUnzipFile_Memory::ReadCurrentFile(void* buf, UINT len)
{
	if (!uf.pfile_in_zip_read)
		ThrowError(UNZ_PARAMERROR);

	if ((len == 0) || !buf)
		return 0;

	uf.pfile_in_zip_read->stream.next_out = (Bytef*)buf;
	uf.pfile_in_zip_read->stream.avail_out = (uInt)len;
	
	if (len > uf.pfile_in_zip_read->rest_read_uncompressed)
		uf.pfile_in_zip_read->stream.avail_out = 
		  (uInt)uf.pfile_in_zip_read->rest_read_uncompressed;


	uInt iRead = 0;

	while (uf.pfile_in_zip_read->stream.avail_out>0)
	{
		if ((uf.pfile_in_zip_read->stream.avail_in==0) &&
            (uf.pfile_in_zip_read->rest_read_compressed>0))
		{
			uInt uReadThis = UNZ_BUFSIZE;
			if (uf.pfile_in_zip_read->rest_read_compressed<uReadThis)
				uReadThis = (uInt)uf.pfile_in_zip_read->rest_read_compressed;
			if (uReadThis == 0)
				return 0;


			//uf.file.Seek(uf.pfile_in_zip_read->pos_in_zipfile + uf.pfile_in_zip_read->byte_before_the_zipfile, CFile::begin);
      Seek(uf.pfile_in_zip_read->pos_in_zipfile + uf.pfile_in_zip_read->byte_before_the_zipfile);
      
      //uf.file.Read(uf.pfile_in_zip_read->read_buffer, uReadThis);
      Read(uf.pfile_in_zip_read->read_buffer, uReadThis);

			uf.pfile_in_zip_read->pos_in_zipfile += uReadThis;

			uf.pfile_in_zip_read->rest_read_compressed-=uReadThis;
			
			uf.pfile_in_zip_read->stream.next_in = 
                (Bytef*)uf.pfile_in_zip_read->read_buffer;
			uf.pfile_in_zip_read->stream.avail_in = (uInt)uReadThis;
		}

		if (uf.pfile_in_zip_read->compression_method==0)
		{
			uInt uDoCopy;
			if (uf.pfile_in_zip_read->stream.avail_out < 
                            uf.pfile_in_zip_read->stream.avail_in)
				uDoCopy = uf.pfile_in_zip_read->stream.avail_out ;
			else
				uDoCopy = uf.pfile_in_zip_read->stream.avail_in ;

			memcpy(uf.pfile_in_zip_read->stream.next_out, 
				uf.pfile_in_zip_read->stream.next_in, uDoCopy);
					
			uf.pfile_in_zip_read->crc32 = crc32(uf.pfile_in_zip_read->crc32,
								uf.pfile_in_zip_read->stream.next_out,
								uDoCopy);

			uf.pfile_in_zip_read->rest_read_uncompressed-=uDoCopy;
			uf.pfile_in_zip_read->stream.avail_in -= uDoCopy;
			uf.pfile_in_zip_read->stream.avail_out -= uDoCopy;
			uf.pfile_in_zip_read->stream.next_out += uDoCopy;
			uf.pfile_in_zip_read->stream.next_in += uDoCopy;
            uf.pfile_in_zip_read->stream.total_out += uDoCopy;
			iRead += uDoCopy;
		}
		else
		{
			uLong uTotalOutBefore = uf.pfile_in_zip_read->stream.total_out;
			const Bytef *bufBefore = uf.pfile_in_zip_read->stream.next_out;
			int flush=Z_SYNC_FLUSH;

			/*
			if ((uf.pfile_in_zip_read->rest_read_uncompressed ==
			         uf.pfile_in_zip_read->stream.avail_out) &&
				(uf.pfile_in_zip_read->rest_read_compressed == 0))
				flush = Z_FINISH;
			*/
			int err = inflate(&uf.pfile_in_zip_read->stream,flush);

			uLong uTotalOutAfter = uf.pfile_in_zip_read->stream.total_out;
			uLong uOutThis = uTotalOutAfter-uTotalOutBefore;
			
			uf.pfile_in_zip_read->crc32 = 
                crc32(uf.pfile_in_zip_read->crc32,bufBefore,
                        (uInt)(uOutThis));

			uf.pfile_in_zip_read->rest_read_uncompressed -=
                uOutThis;

			iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);
            
			if (err==Z_STREAM_END)
				return iRead;

			CheckForError(err);
		}
	}

	return iRead;
}

/*
  Read extra field from the current file (opened by OpenCurrentFile)
  This is the local-header version of the extra field (sometimes, there is
    more info in the local-header version than in the central-header)

  if buf==NULL, it return the size of the local extra field that can be read

  if buf!=NULL, len is the size of the buffer, the extra header is copied in
	buf.
  the return value is the number of bytes copied in buf
*/
int CUnzipFile_Memory::GetLocalExtrafield (void* buf, UINT len)
{
	if (!uf.pfile_in_zip_read)
		ThrowError(UNZ_PARAMERROR);


	uLong size_to_read = (uf.pfile_in_zip_read->size_local_extrafield - 
				uf.pfile_in_zip_read->pos_local_extrafield);

	if (!buf)
		return (int)size_to_read;
	
	uInt read_now;

	if (len>size_to_read)
		read_now = (uInt)size_to_read;
	else
		read_now = (uInt)len ;

	if (!read_now)
		return 0;

	
	//uf.file.Seek(uf.pfile_in_zip_read->offset_local_extrafield + uf.pfile_in_zip_read->pos_local_extrafield, CFile::begin);
  Seek(uf.pfile_in_zip_read->offset_local_extrafield + uf.pfile_in_zip_read->pos_local_extrafield);

	//return (int)uf.file.Read(buf, read_now);
  return (int)Read(buf, read_now);
}
/*
  Get the global comment string of the ZipFile, in the szComment buffer.
  uSizeBuf is the size of the szComment buffer.
  return the number of byte copied
*/
int CUnzipFile_Memory::GetGlobalComment (char* szComment, uLong uSizeBuf)
{

	uLong uReadThis = uSizeBuf;
	if (uReadThis > uf.gi.size_comment)
		uReadThis = uf.gi.size_comment;

	//uf.file.Seek(uf.central_pos+22, CFile::begin);
  Seek(uf.central_pos + 22);

	if (uReadThis)
    {
      *szComment = '\0';
	  //uReadThis = uf.file.Read(szComment, (uInt)uReadThis);
      uReadThis = Read(szComment, (uInt)uReadThis);
    }

	if (szComment && (uSizeBuf > uf.gi.size_comment))
		*(szComment+uf.gi.size_comment)='\0';

	return (int)uReadThis;
}

/*
  Give the current position in uncompressed data
*/
z_off_t CUnzipFile_Memory::tell()
{
	if (!uf.pfile_in_zip_read)
		ThrowError(UNZ_PARAMERROR);

	return (z_off_t)uf.pfile_in_zip_read->stream.total_out;
}


/*
  return true if the end of file was reached, false elsewhere 
*/
bool CUnzipFile_Memory::eof()
{

	if (!uf.pfile_in_zip_read)
		ThrowError(UNZ_PARAMERROR);
	
	return uf.pfile_in_zip_read->rest_read_uncompressed == 0;
}

CUnzipFile_Memory::~CUnzipFile_Memory()
{
// 	Close(); // cannot be here: if an exception is thrown strange things can happen

}

void CUnzipFile_Memory::unzlocal_getByte(int & pi)
{
  unsigned char c;
	//uf.file.Read(&c, 1);
  Read(&c, 1);
	pi = (int)c;
    
}

void CUnzipFile_Memory::unzlocal_getShort (uLong & pX)
{
    int i;

    unzlocal_getByte(i);
    uLong x = (uLong)i;
    unzlocal_getByte(i);
    x += ((uLong)i)<<8;
   
	pX = x;
}

void CUnzipFile_Memory::unzlocal_getLong (uLong & pX)
{
	

	uLong x;
	unzlocal_getShort(x);
	uLong y;
	unzlocal_getShort(y);
	x += y << 16;
    pX = x;
}

//    Compare two filename (fileName1,fileName2).
int CUnzipFile_Memory::StringFileNameCompare(CString fileName1, CString fileName2, bool caseSensitive)
{
	return caseSensitive ? fileName1.Collate(fileName2) : 	fileName1.CollateNoCase(fileName2);
}

/*
  Locate the Central directory of a zipfile (at the end, just before
    the global comment)
    找到压缩文件中中心目录记录结束符(0x06054b50)的位置
*/
uLong CUnzipFile_Memory::unzlocal_SearchCentralDir()
{
	
	uLong uMaxBack=0xffff; /* maximum size of global comment */

	uLong uSizeFile = m_lDataCount;//uf.file.GetLength();

	if (uMaxBack > uSizeFile)
		uMaxBack = uSizeFile;

	char* buf = new char[BUFREADCOMMENT + 4];

	uLong uBackRead = 4;
	uLong uPosFound = 0;

	try
	{
		while ((uBackRead < uMaxBack) && !uPosFound)
		{
			uLong uReadSize,uReadPos ;

			if (uBackRead + BUFREADCOMMENT > uMaxBack) 
				uBackRead = uMaxBack;
			else
				uBackRead += BUFREADCOMMENT;

			uReadPos = uSizeFile - uBackRead ;
			
			uReadSize = ((BUFREADCOMMENT + 4) < (uSizeFile - uReadPos)) ? 
						 (BUFREADCOMMENT + 4) : (uSizeFile - uReadPos);

			//uf.file.Seek(uReadPos, CFile::begin);
      Seek(uReadPos);

			//uf.file.Read(buf, uReadSize);
      Read(buf, uReadSize);

			for (int i= (int)uReadSize - 3; (i--) > 0 ;)
				if (((*(buf+i)) == 0x50) && ((*(buf+i+1))==0x4b) && 
					((*(buf+i+2))==0x05) && ((*(buf+i+3))==0x06))
				{
					uPosFound = uReadPos + i;
					break;
				}

		}
	}
	catch (CException*)
	{
		delete[] buf;
		throw;
	}
	delete[] buf;

	if (!uPosFound)
		ThrowError(UNZ_BADZIPFILE);

	return uPosFound;
	
}

/*
   Translate date/time from Dos format to tm_unz (readable more easilty)
*/
void CUnzipFile_Memory::unzlocal_DosDateToTmuDate(unz_file_info &file_info)
{
	CTime t(HIWORD(file_info.dosDate), LOWORD(file_info.dosDate));
	file_info.tmu_date = t;
}


/*
  Get Info about the current file in the zipfile, with internal only info
  得到中心目录结构(0x02014b50)的数据，文件名，文件注释
*/
void CUnzipFile_Memory::unzlocal_GetCurrentFileInfoInternal( unz_file_info & file_info,
                                         unz_file_info_internal & file_info_internal,
                                         LPSTR szFileName,
										 uLong fileNameBufferSize,
										 void *extraField,
										 uLong extraFieldBufferSize,
										 LPSTR szComment,
										 uLong commentBufferSize)
{

	
	//uf.file.Seek(uf.pos_in_central_dir + uf.byte_before_the_zipfile, CFile::begin);
	Seek(uf.pos_in_central_dir + uf.byte_before_the_zipfile);

	uLong uMagic;

	/* we check the magic */
	unzlocal_getLong(uMagic);
	if (uMagic != 0x02014b50)
		ThrowError(UNZ_BADZIPFILE);

	unzlocal_getShort(file_info.version);
	unzlocal_getShort(file_info.version_needed);
	unzlocal_getShort(file_info.flag);
	unzlocal_getShort(file_info.compression_method);
	unzlocal_getLong(file_info.dosDate);

    unzlocal_DosDateToTmuDate(file_info);

	unzlocal_getLong(file_info.crc);
	unzlocal_getLong(file_info.compressed_size);
	unzlocal_getLong(file_info.uncompressed_size);
	unzlocal_getShort(file_info.size_filename);
	unzlocal_getShort(file_info.size_file_extra);
	unzlocal_getShort(file_info.size_file_comment);
	unzlocal_getShort(file_info.disk_num_start);
	unzlocal_getShort(file_info.internal_fa);
	unzlocal_getLong(file_info.external_fa);
	unzlocal_getLong(file_info_internal.offset_curfile);

	uLong lSeek = file_info.size_filename;

	if (szFileName)
	{
		uLong uSizeRead ;
		if (file_info.size_filename < fileNameBufferSize)
		{
			*(szFileName + file_info.size_filename) = '\0';
			uSizeRead = file_info.size_filename;
		}
		else
			uSizeRead = fileNameBufferSize;

		if ((file_info.size_filename>0) && (fileNameBufferSize>0))
			//uf.file.Read(szFileName, uSizeRead);
      Read(szFileName, uSizeRead);

		lSeek -= uSizeRead;
	}

	
	if (extraField)
	{
		uLong uSizeRead ;
		if (file_info.size_file_extra < extraFieldBufferSize)
			uSizeRead = file_info.size_file_extra;
		else
			uSizeRead = extraFieldBufferSize;

		if (lSeek != 0)
		{
			//uf.file.Seek(lSeek, CFile::begin);
      Seek(lSeek);

			lSeek=0;
		}
		
		if ((file_info.size_file_extra>0) && (extraFieldBufferSize>0))
    {
			//uf.file.Read(extraField, uSizeRead);
      Read(extraField, uSizeRead);
    }

		lSeek += file_info.size_file_extra - uSizeRead;
	}
	else
		lSeek+=file_info.size_file_extra; 

	
	if (szComment)
	{
		uLong uSizeRead ;

		if (file_info.size_file_comment<commentBufferSize)
		{
			*(szComment+file_info.size_file_comment)='\0';
			uSizeRead = file_info.size_file_comment;
		}
		else
			uSizeRead = commentBufferSize;

		if (lSeek != 0)
		{
			//uf.file.Seek(lSeek, CFile::begin);
      Seek(lSeek);

			lSeek=0;
		}

		if ((file_info.size_file_comment>0) && (commentBufferSize>0))
    {
			//uf.file.Read(szComment, uSizeRead);
      Read(szComment, uSizeRead);
    }
			
		lSeek+=file_info.size_file_comment - uSizeRead;
	}
	else
		lSeek+=file_info.size_file_comment;

}

//打开压缩文件，读取中心目录记录结束符(0x06054b50),获取全局变量的信息
void CUnzipFile_Memory::Open(void* pBuf, LONG lCount)
{
	if (!IsClosed())
		return;

  uf.m_pfilebuf = (char*)pBuf;
  m_pBufferData = (char*)pBuf;
  m_lDataCount = lCount;
  m_lDataOffset = 0;

	uLong central_pos = unzlocal_SearchCentralDir();

	//uf.file.Seek(central_pos, CFile::begin);
  Seek(central_pos);

	/* the signature, already checked */
	uLong uL;
	unzlocal_getLong(uL);

	uLong number_disk;          /* number of the current dist, used for 
								   spaning ZIP, unsupported, always 0*/
	unzlocal_getShort(number_disk);

	uLong number_disk_with_CD;  /* number the the disk with central dir, used
								   for spaning ZIP, unsupported, always 0*/
	unzlocal_getShort(number_disk_with_CD);

	/* number of the disk with the start of the central directory */
	unzlocal_getShort(uf.gi.number_entry);


	uLong number_entry_CD;      /* total number of entries in
	                               the central dir 
	                               (same than number_entry on nospan) */
	unzlocal_getShort(number_entry_CD);

	
	if ( (number_entry_CD != uf.gi.number_entry) ||
		(number_disk_with_CD != 0) ||
		(number_disk != 0))
			ThrowError(UNZ_BADZIPFILE);

	/* size of the central directory */
	unzlocal_getLong(uf.size_central_dir);

	/* offset of start of central directory with respect to the 
	      starting disk number */
	unzlocal_getLong(uf.offset_central_dir);

	/* zipfile comment length */
	unzlocal_getShort(uf.gi.size_comment);

	if ( central_pos < uf.offset_central_dir + uf.size_central_dir)
		ThrowError(UNZ_BADZIPFILE);


	uf.byte_before_the_zipfile = central_pos -
		(uf.offset_central_dir + uf.size_central_dir);

	uf.central_pos = central_pos;
	
	GoToFirstFile();	

}

//得到当前文件未压缩大小
int CUnzipFile_Memory::GetCurUnCompressFileSize()
{
  return uf.cur_file_info.uncompressed_size;
}