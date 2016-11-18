/********************************************************************
创建时间:  2013/06/09
文 件 名:  ZipStruct.cpp
创建作者:  徐 琼
文件描述：

文件修改：
修改时间        修改作者        修改内容
*********************************************************************/
#include "stdafx.h"
#include "ZipStruct.h"
#include "ZipFile_Memory.h"

//解压

file_in_zip_read_info::file_in_zip_read_info()
{
  read_buffer = new char[UNZ_BUFSIZE];
  stream_initialised  = 0;
  pos_local_extrafield = 0;
  crc32 = 0;
  stream.total_out = 0;
}


file_in_zip_read_info::~file_in_zip_read_info()
{
  delete[] read_buffer;
}



//压缩

zip_fileinfo::zip_fileinfo()
{
	tmz_date = CTime::GetCurrentTime();
	dosDate = internal_fa = external_fa = 0;
}

uLong zip_fileinfo::get_dos_date()
{
	if (dosDate)
		return dosDate;

    uLong year = (uLong)tmz_date.GetYear();
    if (year>1980)
        year-=1980;
    else if (year>80)
        year-=80;
    return
      (uLong) ((tmz_date.GetDay() + 32 * tmz_date.GetMonth() + 512 * year) << 16) |
        (tmz_date.GetSecond() / 2 + 32 * tmz_date.GetMinute() + 2048 * (uLong)tmz_date.GetHour());

}

void curfile_info::alloc_central_header()
{
	ASSERT(!central_header);
	if (size_centralheader)
		central_header = new char[size_centralheader];
}

void curfile_info::free_central_header()
{
	if (central_header)
	{
		delete[] central_header;
		central_header = NULL;
	}
}


curfile_info::curfile_info()
{
	buffered_data = new Byte [Z_BUFSIZE];
	central_header = NULL;
}

curfile_info::~curfile_info()
{
	free_central_header();
	delete[] buffered_data;	
}

linkedlist_datablock_internal::linkedlist_datablock_internal()
{
	next_datablock = NULL;
	avail_in_this_block = SIZEDATA_INDATABLOCK;
	filled_in_this_block = 0;
	data = new unsigned char [SIZEDATA_INDATABLOCK];
}

linkedlist_datablock_internal::~linkedlist_datablock_internal()
{
	delete[] data;
}


linkedlist_data::linkedlist_data()
{
	first_block = last_block = NULL;
}

linkedlist_data::~linkedlist_data()
{
	linkedlist_datablock_internal* b = first_block;
	while (b)
	{
		linkedlist_datablock_internal* a = b->next_datablock;
		delete b;
		b = a;
	}
}


void linkedlist_data::add_data_in_datablock(char* buf, uLong len)
{
  if (!last_block)
    first_block = last_block = new linkedlist_datablock_internal;
  
  linkedlist_datablock_internal* ldi = last_block;
  
  while (len>0)
  {
    uInt copy_this;
    unsigned char* to_copy;
    
    if (ldi->avail_in_this_block == 0)
    {
      ldi->next_datablock = new linkedlist_datablock_internal;
      ldi = ldi->next_datablock ;
      last_block = ldi;
    }
    
    if (ldi->avail_in_this_block < len)
      copy_this = (uInt)ldi->avail_in_this_block;
    else
      copy_this = (uInt)len;
    
    to_copy = &(ldi->data[ldi->filled_in_this_block]);
    
    memcpy(to_copy, buf, copy_this);
    
    ldi->filled_in_this_block += copy_this;
    ldi->avail_in_this_block -= copy_this;
    buf += copy_this ;
    len -= copy_this;
  }
}

int linkedlist_data::write_datablock(CFile & f)
{
    linkedlist_datablock_internal* ldi;
    ldi = first_block;
	int size = 0;
    while (ldi)
    {
        if (ldi->filled_in_this_block > 0)
			f.Write(ldi->data, (uInt)ldi->filled_in_this_block);

		size += ldi->filled_in_this_block;
        ldi = ldi->next_datablock;
    }
	return size;
}

int linkedlist_data::write_datablock(CZipFile_Memory *pZipFile_Memory)
{
  linkedlist_datablock_internal* ldi;
  ldi = first_block;
  int size = 0;
  while (ldi)
  {
    if (ldi->filled_in_this_block > 0)
      pZipFile_Memory->Write(ldi->data, (uInt)ldi->filled_in_this_block);
    
    size += ldi->filled_in_this_block;
    ldi = ldi->next_datablock;
  }
  return size;
}