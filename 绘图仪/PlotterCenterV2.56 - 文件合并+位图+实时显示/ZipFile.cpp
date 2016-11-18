// ZipFile.cpp: implementation of the CZipFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZipFile.h"

zip_internal::zip_internal()
{
	in_opened_file_inzip = 0;
	ci.stream_initialised = 0;
	number_entry = 0;
}
zip_internal::~zip_internal()
{

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CZipFile::CZipFile()
{

	m_pFile = &zi.filezip;
}


CZipFile::CZipFile(LPCTSTR pathname, bool append)
{
	m_pFile = &zi.filezip;
	Open(pathname, append);
}

void CZipFile::Open(LPCTSTR pathname, bool append)
{
	if (!IsClosed())
		return;
	CFileException* e = new CFileException;
	if (!zi.filezip.Open(pathname, (append ? CFile::modeNoTruncate : CFile::modeCreate)|CFile::modeWrite | CFile::shareDenyWrite, e))
		throw e;
	e->Delete();

	zi.filezip.SeekToEnd();
	zi.begin_pos = zi.filezip.GetPosition();
	
}


CZipFile::~CZipFile()
{
// 	Close(); // cannot be here: if an exception is thrown strange things can happen
}

/* ===========================================================================
   Outputs a long in LSB order to the given file
   nbByte == 1, 2 or 4 (byte, short or long)
*/

void CZipFile::ziplocal_putValue(uLong x, int nbByte)
{
    unsigned char buf[4];
    int n;
    for (n = 0; n < nbByte; n++) {
        buf[n] = (unsigned char)(x & 0xff);
        x >>= 8;
    }
    zi.filezip.Write(buf, nbByte);
}

void CZipFile::ziplocal_putValue_inmemory(Byte dest, uLong x, int nbByte)
{
    unsigned char* buf=(unsigned char*)zi.ci.central_header + dest;
    int n;
    for (n = 0; n < nbByte; n++) 
	{
        buf[n] = (unsigned char)(x & 0xff);
        x >>= 8;
	}
}


// 
void CZipFile::OpenNewFileInZip (CString filename, 
								zip_fileinfo& zipfi, 
								int level,
								CString comment, 
								const void* extrafield_local, 
								uInt size_extrafield_local, 
								const void* extrafield_global, 
								uInt size_extrafield_global, 
								int method)
{
    if ((method != Z_DEFLATED) || (!level)) 
        ThrowError(ZIP_PARAMERROR);


    if (zi.in_opened_file_inzip == 1)
        CloseFileInZip();

	if (filename.IsEmpty())
		filename = "-";

	zi.ci.dosDate = zipfi.get_dos_date();

    zi.ci.flag = 0;
    if ((level == 8) || (level == 9))
      zi.ci.flag |= 2;
    if (level == 2)
      zi.ci.flag |= 4;
    if (level == 1)
      zi.ci.flag |= 6;

    zi.ci.crc32 = 0;
    zi.ci.method = method;
    zi.ci.stream_initialised = 0;
    zi.ci.pos_in_buffered_data = 0;
    zi.ci.pos_local_header = zi.filezip.GetPosition();
    zi.ci.size_centralheader = SIZECENTRALHEADER + filename.GetLength() + 
                                      size_extrafield_global + comment.GetLength();

    zi.ci.alloc_central_header();

    ziplocal_putValue_inmemory(0, (uLong)CENTRALHEADERMAGIC,4);
    /* version info */
    ziplocal_putValue_inmemory(4,(uLong)VERSIONMADEBY,2);
    ziplocal_putValue_inmemory(6,(uLong)20, 2);
    ziplocal_putValue_inmemory(8,(uLong)zi.ci.flag,2);
    ziplocal_putValue_inmemory(10,(uLong)zi.ci.method,2);
    ziplocal_putValue_inmemory(12,(uLong)zi.ci.dosDate,4);
    ziplocal_putValue_inmemory(16,(uLong)0, 4); /*crc*/
    ziplocal_putValue_inmemory(20,(uLong)0, 4); /*compr size*/
    ziplocal_putValue_inmemory(24,(uLong)0, 4); /*uncompr size*/
    ziplocal_putValue_inmemory(28,(uLong)filename.GetLength(), 2);
    ziplocal_putValue_inmemory(30,(uLong)size_extrafield_global,2);
    ziplocal_putValue_inmemory(32,(uLong)comment.GetLength(), 2);
    ziplocal_putValue_inmemory(34,(uLong)0, 2); /*disk nm start*/

	ziplocal_putValue_inmemory(36,(uLong)zipfi.internal_fa, 2); 
	ziplocal_putValue_inmemory(38,(uLong)zipfi.external_fa, 4);

    ziplocal_putValue_inmemory(42,(uLong)zi.ci.pos_local_header, 4);

	char* pDest = zi.ci.central_header + SIZECENTRALHEADER;
	memcpy(pDest, (LPCTSTR)filename, filename.GetLength());
	pDest += filename.GetLength();
	memcpy(pDest, extrafield_global, size_extrafield_global);
	pDest += size_extrafield_global;
	memcpy(pDest, (LPCTSTR)comment, comment.GetLength());

    /* write the local header */
    ziplocal_putValue((uLong)LOCALHEADERMAGIC, 4);
	ziplocal_putValue((uLong)20, 2);/* version needed to extract */
    ziplocal_putValue((uLong)zi.ci.flag, 2);
	ziplocal_putValue((uLong)zi.ci.method, 2);
	ziplocal_putValue((uLong)zi.ci.dosDate, 4);
	ziplocal_putValue((uLong)0, 4); /* crc 32, unknown */
    ziplocal_putValue((uLong)0, 4); /* compressed size, unknown */
	ziplocal_putValue((uLong)0, 4); /* uncompressed size, unknown */
	ziplocal_putValue((uLong)filename.GetLength(), 2);
	ziplocal_putValue((uLong)size_extrafield_local, 2);

	zi.filezip.Write(filename, filename.GetLength());

	if (size_extrafield_local > 0)
		zi.filezip.Write(extrafield_local, size_extrafield_local);

    zi.ci.stream.avail_in = (uInt)0;
    zi.ci.stream.avail_out = (uInt)Z_BUFSIZE;
    zi.ci.stream.next_out = zi.ci.buffered_data;
    zi.ci.stream.total_in = 0;
    zi.ci.stream.total_out = 0;

    if ((zi.ci.method == Z_DEFLATED))
    {
        zi.ci.stream.zalloc = (alloc_func)myalloc;
        zi.ci.stream.zfree = (free_func)myfree;
        zi.ci.stream.opaque = m_bDetectZlibMemoryLeaks ? &m_list : 0;

        int err = deflateInit2(&zi.ci.stream, level,
               Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);

		CheckForError(err);
        zi.ci.stream_initialised = 1;
    }


        zi.in_opened_file_inzip = 1;

}


void CZipFile::WriteInFileInZip(const void *buf, UINT len)
{
    if (zi.in_opened_file_inzip == 0)
        ThrowError(ZIP_PARAMERROR);

    zi.ci.stream.next_in = (unsigned char*)buf;
    zi.ci.stream.avail_in = len;
    zi.ci.crc32 = crc32(zi.ci.crc32, (unsigned char*)buf, len);

	int err = Z_OK;

    while ((err == Z_OK) && (zi.ci.stream.avail_in > 0))
    {

        if (zi.ci.stream.avail_out == 0)
        {
			zi.filezip.Write(zi.ci.buffered_data, zi.ci.pos_in_buffered_data);
            zi.ci.pos_in_buffered_data = 0;
            zi.ci.stream.avail_out = (uInt)Z_BUFSIZE;
            zi.ci.stream.next_out = zi.ci.buffered_data;
        }

        if (zi.ci.method == Z_DEFLATED)
        {
            uLong uTotalOutBefore = zi.ci.stream.total_out;
            err = deflate(&zi.ci.stream,  Z_NO_FLUSH);
            zi.ci.pos_in_buffered_data += (uInt)(zi.ci.stream.total_out - uTotalOutBefore) ;
        }
        else
        {
            uInt copy_this;
            if (zi.ci.stream.avail_in < zi.ci.stream.avail_out)
                copy_this = zi.ci.stream.avail_in;
            else
                copy_this = zi.ci.stream.avail_out;

			memcpy(zi.ci.stream.next_out, zi.ci.stream.next_in, copy_this);

            zi.ci.stream.avail_in -= copy_this;
            zi.ci.stream.avail_out -= copy_this;
            zi.ci.stream.next_in += copy_this;
            zi.ci.stream.next_out += copy_this;
            zi.ci.stream.total_in += copy_this;
            zi.ci.stream.total_out += copy_this;
            zi.ci.pos_in_buffered_data += copy_this;
        }
    }

	CheckForError(err);

}


void CZipFile::CloseFileInZip()
{
    if (zi.in_opened_file_inzip == 0)    
        return;

	int err = Z_OK;

    zi.ci.stream.avail_in = 0;
    
    if (zi.ci.method == Z_DEFLATED)
        while (err == Z_OK)
		{
			uLong uTotalOutBefore;
			if (zi.ci.stream.avail_out == 0)
			{
				zi.filezip.Write(zi.ci.buffered_data, zi.ci.pos_in_buffered_data);
				zi.ci.pos_in_buffered_data = 0;
				zi.ci.stream.avail_out = (uInt)Z_BUFSIZE;
				zi.ci.stream.next_out = zi.ci.buffered_data;
			}
			uTotalOutBefore = zi.ci.stream.total_out;
			err = deflate(&zi.ci.stream,  Z_FINISH);
			zi.ci.pos_in_buffered_data += (uInt)(zi.ci.stream.total_out - uTotalOutBefore) ;
		}

    if (err == Z_STREAM_END)
        err = Z_OK; /* this is normal */

	CheckForError(err);

	if (zi.ci.pos_in_buffered_data > 0)
		zi.filezip.Write(zi.ci.buffered_data, zi.ci.pos_in_buffered_data);

	if (zi.ci.method == Z_DEFLATED)
	{
		err = deflateEnd(&zi.ci.stream);
		zi.ci.stream_initialised = 0;
	}

	CheckForError(err);

	ziplocal_putValue_inmemory(16, (uLong)zi.ci.crc32, 4); /*crc*/
	ziplocal_putValue_inmemory(20, (uLong)zi.ci.stream.total_out, 4); /*compr size*/
	ziplocal_putValue_inmemory(24, (uLong)zi.ci.stream.total_in, 4); /*uncompr size*/

	zi.central_dir.add_data_in_datablock(zi.ci.central_header, (uLong)zi.ci.size_centralheader);

	zi.ci.free_central_header();

    long cur_pos_inzip = zi.filezip.GetPosition();
	zi.filezip.Seek(zi.ci.pos_local_header + 14, CFile::begin);
	ziplocal_putValue((uLong)zi.ci.crc32, 4); /* crc 32, unknown */
    ziplocal_putValue((uLong)zi.ci.stream.total_out, 4);/* compressed size, unknown */
	ziplocal_putValue((uLong)zi.ci.stream.total_in, 4); /* uncompressed size, unknown */
	zi.filezip.Seek(cur_pos_inzip, CFile::begin);

    zi.number_entry++;
    zi.in_opened_file_inzip = 0;
}

void CZipFile::Close(CString global_comment)
{

	if (IsClosed())
		return;

    if (zi.in_opened_file_inzip == 1)
        CloseFileInZip ();


    uLong centraldir_pos_inzip = zi.filezip.GetPosition();

	uLong size_centraldir = zi.central_dir.write_datablock(zi.filezip);
    

    /* Magic End */
    ziplocal_putValue((uLong)ENDHEADERMAGIC, 4);
	ziplocal_putValue((uLong)0, 2);/* number of this disk */
	ziplocal_putValue((uLong)0, 2);/* number of the disk with the start of the central directory */
    ziplocal_putValue((uLong)zi.number_entry, 2);/* total number of entries in the central dir on this disk */
    ziplocal_putValue((uLong)zi.number_entry,2);/* total number of entries in the central dir */
	ziplocal_putValue((uLong)size_centraldir,4);/* size of the central directory */
	ziplocal_putValue((uLong)centraldir_pos_inzip ,4);/* offset of start of central directory with respect to the starting disk number */
	ziplocal_putValue((uLong)global_comment.GetLength(), 2);/* zipfile comment length */
	if (!global_comment.IsEmpty())
		zi.filezip.Write(global_comment, global_comment.GetLength());

	zi.filezip.Close();

}

void CZipFile::UpdateZipInfo(zip_fileinfo &zi, CFile &f)
{
	CFileStatus fs;
	f.GetStatus(fs);
	zi.tmz_date = fs.m_mtime;
	zi.external_fa = ::GetFileAttributes(f.GetFilePath()); // mfc bug: m_attribute is 1-byte

}
