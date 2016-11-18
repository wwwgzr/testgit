//ZipStruct.h
#if !defined(ZIPSTRUCT_H)
#define ZIPSTRUCT_H

#include "zlib.h"
#include "ZipException.h"

//解压用到的结构
#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)


#ifndef UNZ_BUFSIZE
#define UNZ_BUFSIZE (16384)
#endif

#ifndef UNZ_MAXFILENAMEINZIP
#define UNZ_MAXFILENAMEINZIP (256)
#endif

#define BUFREADCOMMENT (0x400)


#define UNZ_PARAMERROR                  (-102)
#define UNZ_BADZIPFILE                  (-103)
#define UNZ_CRCERROR                    (-105)


/* unz_global_info structure contain global data about the ZIPfile
   These data comes from the end of central dir */
struct unz_global_info
{
	uLong number_entry;         /* total number of entries in
				       the central dir on this disk */
	uLong size_comment;         /* size of the global comment of the zipfile */
};

/* unz_file_info contain information about a file in the zipfile */
struct unz_file_info
{
    uLong version;              /* version made by                 2 bytes */
    uLong version_needed;       /* version needed to extract       2 bytes */
    uLong flag;                 /* general purpose bit flag        2 bytes */
    uLong compression_method;   /* compression method              2 bytes */
    uLong dosDate;              /* last mod file date in Dos fmt   4 bytes */
    uLong crc;                  /* crc-32                          4 bytes */
    uLong compressed_size;      /* compressed size                 4 bytes */ 
    uLong uncompressed_size;    /* uncompressed size               4 bytes */ 
    uLong size_filename;        /* filename length                 2 bytes */
    uLong size_file_extra;      /* extra field length              2 bytes */
    uLong size_file_comment;    /* file comment length             2 bytes */

    uLong disk_num_start;       /* disk number start               2 bytes */
    uLong internal_fa;          /* internal file attributes        2 bytes */
    uLong external_fa;          /* external file attributes        4 bytes */

    CTime tmu_date;
};


/* unz_file_info_interntal contain internal info about a file in zipfile*/
struct unz_file_info_internal
{
    uLong offset_curfile;/* relative offset of local header 4 bytes */
};


/* file_in_zip_read_info_s contain internal information about a file in zipfile,
    when reading and decompress it */
struct file_in_zip_read_info
{
	file_in_zip_read_info();
	~file_in_zip_read_info();
	char  *read_buffer;         /* internal buffer for compressed data */
	z_stream stream;            /* zLib stream structure for inflate */

	uLong pos_in_zipfile;       /* position in byte on the zipfile, for fseek*/
	uLong stream_initialised;   /* flag set if stream structure is initialised*/

	uLong offset_local_extrafield;/* offset of the local extra field */
	uInt  size_local_extrafield;/* size of the local extra field */
	uLong pos_local_extrafield;   /* position in the local extra field in read*/

	uLong crc32;                /* crc32 of all data uncompressed */
	uLong crc32_wait;           /* crc32 we must obtain after decompress all */
	uLong rest_read_compressed; /* number of byte to be decompressed */
	uLong rest_read_uncompressed;/*number of byte to be obtained after decomp*/
// 	CFile* file;                 /* io structore of the zipfile */
	uLong compression_method;   /* compression method (0==store) */
	uLong byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
};

//压缩用到的结构
#ifndef VERSIONMADEBY
# define VERSIONMADEBY   (0x0) /* platform depedent */
#endif

#define FLAG_LOCALHEADER_OFFSET (0x06)
#define CRC_LOCALHEADER_OFFSET  (0x0e)

#define SIZECENTRALHEADER (0x2e) /* 46 */



#ifndef Z_BUFSIZE
#define Z_BUFSIZE (16384)
#endif

#ifndef Z_MAXFILENAMEINZIP
#define Z_MAXFILENAMEINZIP (256)
#endif

#ifndef DEF_MEM_LEVEL
#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
#endif

#define SIZEDATA_INDATABLOCK (4096-(4*4))

#define LOCALHEADERMAGIC    (0x04034b50)
#define CENTRALHEADERMAGIC  (0x02014b50)
#define ENDHEADERMAGIC      (0x06054b50)


#define SIZECENTRALHEADER (0x2e) /* 46 */

// it doesn't compile anyway
// const char z_copyright[] =
//    " unzip & zip 0.15 Copyright 1998 Gilles Vollant & MFC/C++ revision Copyright 2000 Tadeusz Dracz";

#define ZIP_PARAMERROR                  (-102)

struct zip_fileinfo
{
	CTime		tmz_date;       /* date in understandable format           */
    uLong		dosDate;       /* if dos_date == 0, tmu_date is used      */
/*    uLong       flag;        */   /* general purpose bit flag        2 bytes */

    uLong       internal_fa;    /* internal file attributes        2 bytes */
    uLong       external_fa;    /* external file attributes        4 bytes */
public:
	uLong get_dos_date();
	zip_fileinfo();
};

struct curfile_info
{
	curfile_info();
	~curfile_info();
  z_stream stream;            /* zLib stream structure for inflate */
    int  stream_initialised;    /* 1 is stream is initialised */
    UINT pos_in_buffered_data;  /* last written byte in buffered_data */

    DWORD pos_local_header;     /* offset of the local header of the file 
                                     currenty writing */
    char* central_header;       /* central header data for the current file */
    DWORD size_centralheader;   /* size of the central header for cur file */
    uLong flag;                 /* flag of the file currently writing */

    int  method;                /* compression method of file currenty wr.*/
    Byte* buffered_data;/* buffer contain compressed data to be writ*/
    uLong dosDate;
    uLong crc32;
public:
	void free_central_header();
	void alloc_central_header();
};

class linkedlist_datablock_internal
{
public:
  linkedlist_datablock_internal();
  ~linkedlist_datablock_internal();
  class linkedlist_datablock_internal* next_datablock;
  uLong  avail_in_this_block;
  uLong  filled_in_this_block;
  uLong  unused; /* for future use and alignement */
  unsigned char* data;
	
};


class CZipFile_Memory;
class linkedlist_data
{
public:
	void add_data_in_datablock(char* buf, uLong len);
	linkedlist_data();
	~linkedlist_data();
	int write_datablock(CFile & f);
  int write_datablock(CZipFile_Memory *pZipFile_Memory);
    linkedlist_datablock_internal* first_block;
    linkedlist_datablock_internal* last_block;
};

#endif