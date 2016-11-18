#include "stdafx.h"
#include "PlotBitmap.h"

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
Bitmap* Create1BPPBitmap(BOOL bTransparent,COLORREF crBkColor,int iWidth,int iHeight,int iStride,DWORD nPixFormat,BYTE *pBmpData)
{
  Bitmap *pBitmapGDI,*pResultBitmap;
  CBitmap BitmapCDC, *pOldBitmapCDC;
  BYTE *pData, *pDataCDC, *pMiddleData;
  int iScanLines;
  HDC hDC;
  CDC memDC, dc;
  CBrush brush,* pBrushOld;
  CPen pen, *pPenOld;
  BITMAPINFOHEADER bi;  //记录图片的小大，颜色深度等
  DWORD dwLineBytes, dwLength, dwSignLineBytes,dwMiddleLineBytes;
  CPoint ptLU, ptLD, ptRU, ptRD;
  BitmapData bitmapData;
  Rect rect;

  pResultBitmap = NULL;

  if ((iWidth == 0) || (iHeight == 0) || (pBmpData == NULL))
  {
    return pResultBitmap;
  }
  
  //将图片画到CDC上
  hDC = ::GetDC(NULL);
  dc.Attach(hDC);
  memDC.CreateCompatibleDC(&dc);
  //生成内存位图,CreateCompatibleBitmap使用系统内核的分页内存获取资源
  if (BitmapCDC.CreateCompatibleBitmap(&dc, iWidth, iHeight) == 0)
  {
#if _DEBUG
    DWORD dError = GetLastError();  //dError == 8 内存不够
#endif
  }

  pOldBitmapCDC = memDC.SelectObject(&BitmapCDC);
  memDC.SetROP2(R2_COPYPEN);
  brush.CreateSolidBrush(RGB(255,255,255));  //背景色，白色
  brush.UnrealizeObject();
  pBrushOld = memDC.SelectObject(&brush);
  pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
  pPenOld = memDC.SelectObject(&pen);
  memDC.Rectangle(0, 0, iWidth, iHeight);  //将背景设了白色
  memDC.SelectObject(pPenOld);
  pen.DeleteObject();
  memDC.SelectObject(pBrushOld);
  brush.DeleteObject();

  ptLU = CPoint(0,0);
  ptLD = ptLU;
  ptLD.y = ptLD.y + iHeight;
  ptRU = ptLU;
  ptRU.x = ptRU.x + iWidth;
  ptRD = ptRU;
  ptRD.y = ptRD.y + iHeight;
  pBitmapGDI = new Bitmap(iWidth,iHeight,iStride,nPixFormat, pBmpData);  //按原始图生成位图

  DrawBitmap(&memDC, pBitmapGDI, bTransparent, crBkColor, ptLU, ptLD, ptRU, ptRD);  //将图画在给定区域

  //取出CDC上数据
  pDataCDC = NULL;
  bi.biSize = sizeof(BITMAPINFOHEADER);
  bi.biWidth =  iWidth;
  bi.biHeight =  iHeight;
  bi.biPlanes = 1;
  bi.biBitCount = 24;
  bi.biCompression = BI_RGB;  //没压缩
  bi.biSizeImage = 0;
  bi.biXPelsPerMeter = 0;
  bi.biYPelsPerMeter = 0;
  bi.biClrUsed = 0;
  bi.biClrImportant = 0;

  dwLineBytes = (iWidth * 24 + 31) / 32 * 4;
  dwLength = dwLineBytes * iHeight;
  pResultBitmap = NULL;
  pDataCDC = NULL;
  pDataCDC = new BYTE[dwLength];
  iScanLines = GetDIBits(memDC.GetSafeHdc(), (HBITMAP)BitmapCDC.GetSafeHandle(), 0, (DWORD)iHeight, (LPVOID)pDataCDC, (LPBITMAPINFO)&bi, (DWORD)DIB_RGB_COLORS);
  if (iScanLines != 0)
  {
    //转灰度图
    dwMiddleLineBytes = (iWidth * 8 + 31) / 32 * 4;
    dwLength = dwMiddleLineBytes * iHeight;
    pMiddleData = new BYTE[dwLength + 1];
    memset(pMiddleData,255, dwLength);
    ConversionRGBToGray(pDataCDC, dwLineBytes, pMiddleData, dwMiddleLineBytes, iHeight, 24);

    //转黑白图
    dwSignLineBytes = iWidth/8 + (iWidth / 8) % 2;
    if (dwSignLineBytes * 8 < (DWORD)iWidth)
    {
      dwSignLineBytes = (iWidth + 31) / 32 * 4;
    }
    dwLength = dwSignLineBytes * iHeight;
    pData = new BYTE[dwLength];
    memset(pData,0xFF, dwLength);  //白色
    ConversionGrayTo2(pMiddleData, dwMiddleLineBytes, dwSignLineBytes,iHeight,pData);

    delete pMiddleData;

	
#ifdef _DEBUG
    //WritreBMP("C:\\123.bmp",iWidth, iHeight, dwSignLineBytes, PixelFormat1bppIndexed, pData);
#endif
    //保存黑白位图
    bitmapData.Width = iWidth;
    bitmapData.Height = iHeight;
    bitmapData.Stride = dwSignLineBytes;
    bitmapData.PixelFormat = PixelFormat1bppIndexed;
    bitmapData.Scan0 = (VOID*)pData;
    bitmapData.Reserved = NULL;

    pResultBitmap = new Bitmap(iWidth,iHeight,PixelFormat1bppIndexed);
    rect.X = 0;
    rect.Y = 0;
    rect.Width = iWidth;
    rect.Height = iHeight;
    pResultBitmap->LockBits(rect,ImageLockModeWrite|ImageLockModeUserInputBuf,PixelFormat1bppIndexed, &bitmapData);
    //pResultBitmap->LockBits(&rect,ImageLockModeWrite|ImageLockModeUserInputBuf,PixelFormat1bppIndexed, &bitmapData);
    pResultBitmap->UnlockBits(&bitmapData);
    delete pData;
  }
  else
  {
#if _DEBUG
    DWORD dError = GetLastError();
#endif
  }

  delete pBitmapGDI;
  delete pDataCDC;

  memDC.SelectObject(pOldBitmapCDC);
  ::ReleaseDC(NULL,hDC);

  return pResultBitmap;
}

//将RGB色转换成黑白色
//输入参数：
//  byteRed,byteGreen,byteBlue 需要转换的颜色
//返回值：
//  iGrayIndex 灰度索引
//返回值：
// =0 黑色；=1 白色
int  ConversionRGB(BYTE byteRed, BYTE byteGreen, BYTE byteBlue, int &iGrayIndex)
{
  int iColorResult, iIndex;

  iColorResult = 1;

  //iIndex = (int)( ( 77 * (int)byteRed + 151 * (int)byteGreen + 28 * (int)byteBlue) >> 8 ); // 24位转8位核心算法
  iIndex = (int)(((byteBlue)*117 + (byteGreen)*601 + (byteRed)*306) >> 10);
  iGrayIndex = (BYTE)iIndex;
  if (iIndex <= 128)
  {
    iColorResult = 0;
  }
  return iColorResult;
}

//显示图片
//输入参数:
//  pBitmap 图片指针
//  bTransparent  透明显示
//  ptLU,ptLD,ptRU,ptRD  位图左上角、左下角、右上角、右下角的位置
void DrawBitmap(CDC *pDC, Bitmap *pBitmap,BOOL bTransparent, COLORREF crBkColor,CPoint ptLU,CPoint ptLD,CPoint ptRU,CPoint ptRD)
{
  Graphics graphics(pDC->m_hDC);
  ImageAttributes imageAtt;
  ColorMap cMap;
  Point destPoints[3];
  
  graphics.SetPageUnit(UnitPixel);

  destPoints[0]=Point(ptLU.x, ptLU.y);
  destPoints[1]=Point(ptRU.x, ptRU.y);
  destPoints[2]=Point(ptLD.x, ptLD.y);
  
  cMap.oldColor = Color(255,GetRValue(crBkColor),GetGValue(crBkColor),GetBValue(crBkColor));
  if (bTransparent)
  {  
	cMap.newColor = Color(0,GetRValue(crBkColor),GetGValue(crBkColor),GetBValue(crBkColor));
  }
  else
  {
	cMap.newColor = Color(255,GetRValue(crBkColor),GetGValue(crBkColor),GetBValue(crBkColor));
  }

  imageAtt.SetRemapTable(1, &cMap,ColorAdjustTypeDefault);
  graphics.DrawImage(pBitmap, destPoints,3, 0, 0, pBitmap->GetWidth(), pBitmap->GetHeight(), UnitPixel,&imageAtt);

  graphics.ReleaseHDC(pDC->m_hDC);
}


//灰色转黑白色
//输入参数:
// pSrcDataCDC 原图数据
// dw256Bytes  原始图片每行的字节数
// dwSignBytes 新图片每行的字节数
// iHeight 图高度
//输出参数：
//  pDesData 转换颜色之后的图数据
//说明:
// 采用Bayer matrix (4x4 ordered dither)方式处理
//void ConversionGrayTo2(BYTE *pSrcDataCDC, DWORD dw256Bytes,DWORD dwSignBytes,int iHeight, BYTE *pDesData)
//{
//  BYTE *lpPtr,*pBmatrix,byteData;
//  int iIndexX, iIndexY, iIndex, iPointCount, iGray, iSize, iOrder;
//  int iDither,iTemp, iScale, iLevel;
//
//
//  iOrder = 4;
//  if (iOrder > 4) iOrder = 4;
//  iSize = (1 << (2 * iOrder));
//  pBmatrix = NULL;
//  pBmatrix = new BYTE[iSize * sizeof(BYTE)];
//  for(iIndex = 0; iIndex < iSize; iIndex++)
//  {
//    iTemp = iOrder;
//    iIndexX = iIndex / iTemp;
//    iIndexY = iIndex % iTemp;
//    iDither = 0;
//    while (iTemp-- > 0)
//    {
//      iDither = (((iDither<<1)|((iIndexX & 1) ^ (iIndexY & 1))) << 1) | (iIndexY & 1);
//      iIndexX >>= 1;
//      iIndexY >>= 1;
//    }
//    pBmatrix[iIndex] = (BYTE)(iDither);
//  }
//
//  iScale = max(0,(8 - 2 * iOrder));
//  iIndex = 0;
//  byteData = 0xFF;  //白色
//  for (iIndexY = 0;iIndexY < iHeight;iIndexY++)
//  {
//    iPointCount = 0;
//    lpPtr = pSrcDataCDC + iIndexY * dw256Bytes;
//    for (iIndexX = 0;(DWORD)iIndexX < dw256Bytes;iIndexX++)
//    {
//      iGray = (int)(*lpPtr++);
//      iLevel = iGray >> iScale;
//      if(iLevel <= pBmatrix[ (iIndexX % iOrder) + iOrder * (iIndexY % iOrder)])
//      {
//        switch(iIndex)
//        {
//        case 7:
//          byteData = byteData & 0xFE;
//          break;
//        case 6:
//          byteData = byteData & 0xFD;
//          break;
//        case 5:
//          byteData = byteData & 0xFB;
//          break;
//        case 4:
//          byteData = byteData & 0xF7;
//          break;
//        case 3:
//          byteData = byteData & 0xEF;
//          break;
//        case 2:
//          byteData = byteData & 0xDF;
//          break;
//        case 1:
//          byteData = byteData & 0xBF;
//          break;
//        case 0:
//          byteData = byteData & 0x7F;
//          break;
//        }
//      }
//      iIndex++;
//      if (iIndex == 8)
//      {
//        pDesData[iPointCount + (iHeight - iIndexY - 1) * dwSignBytes] = byteData;  //0黑色，1 白色
//        iIndex = 0;
//        iPointCount++;
//        byteData = 0xFF;  //白色
//      }
//    }
//    if (iIndex > 0)
//    {
//      pDesData[iPointCount + (iHeight - iIndexY - 1) * dwSignBytes] = byteData;  //0黑色，1 白色
//      iIndex = 0;
//      byteData = 0xFF;  //白色
//    }
//  }
//  delete pBmatrix;
//}

//灰色转黑白色
//输入参数:
// pSrcDataCDC 原图数据
// dw256Bytes  原始图片每行的字节数
// dwSignBytes 新图片每行的字节数
// iHeight 图高度
//输出参数：
//  pDesData 转换颜色之后的图数据
//说明:
// 采用Floyd-Steinberg error diffusion方式处理
void ConversionGrayTo2(BYTE *pSrcDataCDC, DWORD dw256Bytes,DWORD dwSignBytes,int iHeight, BYTE *pDesData)
{
  BYTE *lpPtr,byteData, byteLevel;
  int iIndexX, iIndexY, iPointIndex, iPointCount;
  int iLevel,iError,iCoeff, iIndex, iMax;
  BOOL bEnable;

  bEnable = TRUE;
  iCoeff = 1;
  iPointIndex = 0;
  byteData = 0xFF;  //白色

  iMax = dw256Bytes * iHeight - 1;

  for (iIndexY = 0;iIndexY < iHeight;iIndexY++)
  {
    iPointCount = 0;
    lpPtr = pSrcDataCDC + iIndexY * dw256Bytes;
    for (iIndexX = 0;(DWORD)iIndexX < dw256Bytes; iIndexX++)
    {
      bEnable = TRUE;
      if ((iIndexX + 1 +iIndexY * dw256Bytes) > (DWORD)iMax)
      {  //最后一个点不处理，处理就会出错
        bEnable = FALSE;
      }
      byteLevel = (*lpPtr++);
      if (byteLevel > 128)
      {
        iError = (int)byteLevel - 255;
      }
      else
      {
        switch(iPointIndex)
        {
        case 7:
          byteData = byteData & 0xFE;
          break;
        case 6:
          byteData = byteData & 0xFD;
          break;
        case 5:
          byteData = byteData & 0xFB;
          break;
        case 4:
          byteData = byteData & 0xF7;
          break;
        case 3:
          byteData = byteData & 0xEF;
          break;
        case 2:
          byteData = byteData & 0xDF;
          break;
        case 1:
          byteData = byteData & 0xBF;
          break;
        case 0:
          byteData = byteData & 0x7F;
          break;
        }
        iError = (int)byteLevel;
      }
      iPointIndex++;
      if (iPointIndex == 8)
      {
        pDesData[iPointCount + (iHeight - iIndexY - 1) * dwSignBytes] = byteData;  //0黑色，1 白色
        iPointIndex = 0;
        iPointCount++;
        byteData = 0xFF;  //白色
      }
      if (bEnable)
      {
        iLevel = pSrcDataCDC[iIndexX + 1 +iIndexY * dw256Bytes] + (iError * 7)/16;
        byteLevel = (BYTE)min(255,max(0,(int)iLevel));
        pSrcDataCDC[iIndexX + 1+ iIndexY * dw256Bytes] = byteLevel;
        if (iIndexY != (iHeight - 1))
        {  //最后一行不处理
          for(iIndex = -1; iIndex < 2; iIndex++)
          {
            switch(iIndex)
            {
            case -1:
              iCoeff = 3; break;
            case 0:
              iCoeff = 5; break;
            case 1:
              iCoeff = 1; break;
            }
            iLevel =pSrcDataCDC[iIndexX + iIndex + (iIndexY + 1) * dw256Bytes] + (iError * iCoeff)/16;
            byteLevel = (BYTE)min(255,max(0,iLevel));
            pSrcDataCDC[iIndexX + iIndex + (iIndexY + 1) * dw256Bytes] = byteLevel;
          }
        }
      }
    }
    if (iPointIndex > 0)
    {
      pDesData[iPointCount + (iHeight - iIndexY - 1) * dwSignBytes] = byteData;  //0黑色，1 白色
      iPointIndex = 0;
      byteData = 0xFF;  //白色
    }
  }
}

//将iDataBit * 8位真彩色转换成256色
//输入参数：
//  pDIBData 真彩数据
//  dwLineBytes 真彩每行字节数
//  p256Data 输入/输出 256色数据
//  iWidth 256色每行字节数
//  iHeight 数据高度
//  iDataBit 数据位
void ConversionRGBToGray(BYTE *pDIBData, DWORD dwLineBytes, BYTE *p256Data, int iWidth, int iHeight, int iDataBit)
{
  int iOffset, iIndexH, iIndexW, iIndex;
  BYTE *lpPtr;
  BYTE byRed, byGreen, byBlue;

  iOffset = 0;
  for (iIndexH = 0; iIndexH < iHeight; iIndexH++)
  {
    lpPtr = pDIBData + iIndexH * dwLineBytes;
    iOffset = iIndexH * iWidth;
    for (iIndexW = 0; (DWORD)iIndexW < dwLineBytes;)
    {
      switch (iDataBit)
      {
      case 24:
        byBlue = (BYTE)(*lpPtr++);
        byGreen = (BYTE)(*lpPtr++);
        byRed = (BYTE)(*lpPtr++);
        iIndexW = iIndexW + 3;
        break;
      case 32:
        byBlue = (BYTE)(*lpPtr++);
        byGreen = (BYTE)(*lpPtr++);
        byRed = (BYTE)(*lpPtr++);
        iIndexW = iIndexW + 4;
        break;
      case 48:
        byBlue = (BYTE)(*lpPtr++);
        *lpPtr++;
        byGreen = (BYTE)(*lpPtr++);
        *lpPtr++;
        byRed = (BYTE)(*lpPtr++);
        *lpPtr++;
        iIndexW = iIndexW + 6;
        break;
      case 64:
        byBlue = (BYTE)(*lpPtr++);
        *lpPtr++;
        byGreen = (BYTE)(*lpPtr++);
        *lpPtr++;
        byRed = (BYTE)(*lpPtr++);
        *lpPtr++;
        iIndexW = iIndexW + 8;
        break;
      }
      ConversionRGB(byRed, byGreen,byBlue, iIndex);
      p256Data[iOffset] = (BYTE)iIndex;
      iOffset++;
    }
  }
}

//根据给定三个点画出一个平行四边形，并且求出平行四边形所在的矩形长宽
//输入参数：
//  ptLU，ptLD，ptRU 三个点
//输出参数：
//  iWidth，iHeight 矩形所在长度和宽度
//返回值：
// 矩形的左上角
CPoint FindParallelogramRect(CPoint ptLU,CPoint ptLD,CPoint ptRU, int &iWidth, int &iHeight)
{
  POSITION pos;
  CPoint ptPoint, ptRD;
  int nMinX,nMaxX,nMinY,nMaxY;
  CRect rect;
  CList<CPoint, CPoint> listPoint;

  ptPoint = ptRU - ptLU;
  ptRD = ptLD + ptPoint;

  listPoint.RemoveAll();
  listPoint.AddTail(ptLU);
  listPoint.AddTail(ptRU);
  listPoint.AddTail(ptLD);
  listPoint.AddTail(ptRD);

  nMaxX = INT_MIN;
  nMinX = INT_MAX;
  nMaxY = INT_MIN;
  nMinY = INT_MAX;
  pos = listPoint.GetHeadPosition();
  while (pos != NULL)
  {
    ptPoint = listPoint.GetNext(pos);
    if (nMaxX < ptPoint.x) nMaxX = ptPoint.x;
    if (nMinX > ptPoint.x) nMinX = ptPoint.x;
    if (nMaxY < ptPoint.y) nMaxY = ptPoint.y;
    if (nMinY > ptPoint.y) nMinY = ptPoint.y;
  }

  if ((nMaxX >= nMinX) && (nMaxY >= nMinY))  rect = CRect(nMinX,nMinY,nMaxX,nMaxY);
  else  rect.SetRectEmpty();

  iWidth = rect.Width();
  iHeight = rect.Height();

  return rect.TopLeft();
}

void WritreBMP(CString strFileName, int iWidth,int iHeight,int iStride,DWORD nPixFormat,BYTE *pBmpData)
{
  BITMAPINFO *pBMI;
  BITMAPFILEHEADER bfh;
  BITMAPINFOHEADER bi;
  CFile file;
  int iBitCount, iIndexX, iIndexY, iLineByte;
  BYTE *pDIBData, *lpPtr, byData;
  RGBQUAD tRGBQuad[2];

  //_T("C:\\345.bmp")
  if(!file.Open(strFileName,CFile::modeCreate | CFile::modeWrite))
  {
    return;
  }
  iBitCount = 24;
  switch (nPixFormat)
  {
  case PixelFormat24bppRGB:
    iBitCount = 24;
    break;
  case PixelFormat32bppRGB:
    iBitCount = 32;
    break;
  case PixelFormat32bppARGB:
    iBitCount = 32;
    break;
  case PixelFormat32bppPARGB:
    iBitCount = 32;
    break;
  case PixelFormat48bppRGB:
    iBitCount = 48;
    break;
  case PixelFormat64bppARGB:
    iBitCount = 64;
    break;
  case PixelFormat64bppPARGB:
    iBitCount = 64;
    break;
  case PixelFormat1bppIndexed:
    iBitCount = 1;
    break;
  }
  bi.biSize = sizeof(BITMAPINFOHEADER);
  bi.biWidth =  iWidth;
  bi.biHeight =  iHeight;
  bi.biPlanes = 1;
  bi.biBitCount = iBitCount;
  bi.biCompression = 0;
  bi.biSizeImage = 0;
  bi.biXPelsPerMeter = 0;
  bi.biYPelsPerMeter = 0;
  bi.biClrUsed = 0;
  bi.biClrImportant = 0;

  iLineByte = iWidth * bi.biBitCount / 8 + (iWidth * bi.biBitCount / 8) % 2;
  if (iLineByte * 8 < iWidth)
  {
	  iLineByte = (iWidth * bi.biBitCount + 31) / 32 * 4;
  }

  bfh.bfType = 0x4d42;   //   "BM "
  bfh.bfOffBits = sizeof(bfh) + sizeof(BITMAPINFOHEADER);
  bfh.bfSize = bfh.bfOffBits + iHeight * iLineByte;
  bfh.bfReserved1 = 0;
  bfh.bfReserved2 = 0;

  pBMI = (BITMAPINFO*)new char[sizeof(BITMAPINFO)];
  memcpy(pBMI,&bi,sizeof(BITMAPINFOHEADER));
  
  pDIBData = new BYTE[bfh.bfSize - bfh.bfOffBits];
  for (iIndexY = 0 ; iIndexY < iHeight; iIndexY++)
  {
    lpPtr = pBmpData + iIndexY * iStride;
    for (iIndexX = 0; iIndexX < iLineByte; iIndexX++)
    {
      byData = (BYTE)(*lpPtr++);
      pDIBData[iIndexX + iLineByte * (iHeight - iIndexY - 1)] = byData;
    }
  }
  
  file.Write(&bfh,sizeof(bfh));
  file.Write(pBMI,sizeof(BITMAPINFOHEADER));
  if (iBitCount == 1)
  {
    //对调色板进行赋值

    tRGBQuad[0].rgbBlue = (BYTE)0;
    tRGBQuad[0].rgbGreen = (BYTE)0;
    tRGBQuad[0].rgbRed = (BYTE)0;
    tRGBQuad[0].rgbReserved = 0;

    tRGBQuad[1].rgbBlue = (BYTE)255;
    tRGBQuad[1].rgbGreen = (BYTE)255;
    tRGBQuad[1].rgbRed = (BYTE)255;
    tRGBQuad[1].rgbReserved = 0;

    file.Write(tRGBQuad, sizeof(RGBQUAD) * 2); //写调色板
  }
  
  file.Write(pDIBData,iHeight * iLineByte);
  file.Close();

  delete pDIBData;
  delete pBMI;
}

