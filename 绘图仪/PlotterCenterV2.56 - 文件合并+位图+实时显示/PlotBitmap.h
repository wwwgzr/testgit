//PlotBitmap.h
#ifndef _PLOTBITMAP_H
#define _PLOTBITMAP_H

#include <afxtempl.h>


//创建单色的位图
//输入参数:
//  bTransparent  透明显示
//  crBkColor  背景颜色(在透明显示时位图中颜色为crBkColor的点将不被显示在纸样上)
//  iWidth 位图的宽,单位:像素
//  iHeight 位图的高,单位:像素
//  iStride 位图每行的字节数
//  nPixFormat 位图点的格式(见GDI+中BitmapData类PixelFormat的定义)
//  pBmpData 位图数据(保存数据的顺序是从左到右,从上到下)
//返回值:
//  NULL 失败
//  非NULL 新生成的位图指针
Bitmap* Create1BPPBitmap(BOOL bTransparent,COLORREF crBkColor,int iWidth,int iHeight,int iStride,DWORD nPixFormat,BYTE *pBmpData);

//将RGB色转换成黑白色
//输入参数：
//  byteRed,byteGreen,byteBlue 需要转换的颜色
//返回值：
//  iGrayIndex 灰度索引
//返回值：
// =0 黑色；=1 白色
int  ConversionRGB(BYTE byteRed, BYTE byteGreen, BYTE byteBlue, int &iGrayIndex);

//显示图片
//输入参数:
//  pBitmap 图片指针
//  bTransparent  透明显示
//  crBkColor  背景颜色(在透明显示时位图中颜色为crBkColor的点将不被显示)
//  ptLU,ptLD,ptRU,ptRD  位图左上角、左下角、右上角、右下角的位置
void DrawBitmap(CDC *pDC,Bitmap *pBitmap,BOOL bTransparent,COLORREF crBkColor, CPoint ptLU,CPoint ptLD,CPoint ptRU,CPoint ptRD);

//灰色转黑白色
//输入参数:
// pSrcDataCDC 原图数据
// dwOldLineBytes  原始图片每行的字节数
// dwSignBytes 新图片每行的字节数
// iHeight 图高度
//输出参数：
//  pDesData 转换颜色之后的图数据
void ConversionGrayTo2(BYTE *pSrcDataCDC, DWORD dw256Bytes,DWORD dwSignBytes,int iHeight, BYTE *pDesData);

//将iDataBit * 8位真彩色转换成256色
//输入参数：
//  pDIBData 真彩数据
//  dwLineBytes 真彩每行字节数
//  p256Data 输入/输出 256色数据
//  iWidth 256色每行字节数
//  iHeight 数据高度
//  iDataBit 数据位
void ConversionRGBToGray(BYTE *pDIBData, DWORD dwLineBytes, BYTE *p256Data, int iWidth, int iHeight, int iDataBit);

//根据给定三个点画出一个平行四边形，并且求出平行四边形所在的矩形长宽
//输入参数：
//  ptLU，ptLD，ptRU 三个点
//输出参数：
//  iWidth，iHeight 矩形所在长度和宽度
//返回值：
// 矩形的左上角
CPoint FindParallelogramRect(CPoint ptLU,CPoint ptLD,CPoint ptRU, int &iWidth, int &iHeight);


//测试使用
void WritreBMP(CString strFileName, int iWidth,int iHeight,int iStride,DWORD nPixFormat,BYTE *pBmpData);

#endif