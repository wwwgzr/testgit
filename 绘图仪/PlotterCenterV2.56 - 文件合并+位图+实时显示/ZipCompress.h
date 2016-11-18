//ZipCompress.h
#if !defined(ZIPCOMPRESS_H)
#define ZIPCOMPRESS_H

#define BUF_SIZE 2048

//对给定的内存进行压缩
//[输入] pUnCompressData 需要压缩的数据
//[输入]  iUnCompressCount内存大小
//[输入][输出]  pCompressData 压缩后的数据,内存在函数外申请
//[输入][输出]  iCompressCount 输出: 压缩后的大小,实际使用的大小; 输入: 为pCompressData在外面申请的空间大小,申请空间的大小: iUnCompressCount + 100 , 100为zip压缩头文件大小
void CompressMemory(char *pUnCompressData, int iUnCompressCount,  char *&pCompressData, int &iCompressCount);


//对给定的内存进行解压
//输入参数:
//  pCompressData 压缩后的数据
//  iCompressCount 压缩后的大小
//输出参数：
//  pUnCompressData 需要压缩的数据,内存在函数中申请,需要在函数外删除
//  iUnCompressCount 内存大小
void UnCompressMemory(char *pCompressData, int iCompressCount, char *&pUnCompressData, int &iUnCompressCount);


//对给定的文件进行压缩
//输入参数：
//  strFilePath 需要压缩的文件路径(全路径，包含文件名，后缀)
//  strFileName 压缩的文件名称(文件名，文件后缀)
//  strName 压缩的文件名
//输出参数：
//  strZipSaveName 保存的压缩文件(全路径)
void CompressFile(CString strFilePath, CString strFileName, CString strZipSaveName);


//对给定的文件进行解压
//输入参数:
//  strZipFilePath ZIP文件路径(包含文件名和后缀)
//输出参数:
// strSaveName 保存的文件路径(只需要路径，没有文件名)
void UnCompressFile(CString strZipFilePath,  CString strSavePath);

#endif