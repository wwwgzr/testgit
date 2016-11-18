//PlotBitmap.h
#ifndef _PLOTBITMAP_H
#define _PLOTBITMAP_H

#include <afxtempl.h>


//������ɫ��λͼ
//�������:
//  bTransparent  ͸����ʾ
//  crBkColor  ������ɫ(��͸����ʾʱλͼ����ɫΪcrBkColor�ĵ㽫������ʾ��ֽ����)
//  iWidth λͼ�Ŀ�,��λ:����
//  iHeight λͼ�ĸ�,��λ:����
//  iStride λͼÿ�е��ֽ���
//  nPixFormat λͼ��ĸ�ʽ(��GDI+��BitmapData��PixelFormat�Ķ���)
//  pBmpData λͼ����(�������ݵ�˳���Ǵ�����,���ϵ���)
//����ֵ:
//  NULL ʧ��
//  ��NULL �����ɵ�λͼָ��
Bitmap* Create1BPPBitmap(BOOL bTransparent,COLORREF crBkColor,int iWidth,int iHeight,int iStride,DWORD nPixFormat,BYTE *pBmpData);

//��RGBɫת���ɺڰ�ɫ
//���������
//  byteRed,byteGreen,byteBlue ��Ҫת������ɫ
//����ֵ��
//  iGrayIndex �Ҷ�����
//����ֵ��
// =0 ��ɫ��=1 ��ɫ
int  ConversionRGB(BYTE byteRed, BYTE byteGreen, BYTE byteBlue, int &iGrayIndex);

//��ʾͼƬ
//�������:
//  pBitmap ͼƬָ��
//  bTransparent  ͸����ʾ
//  crBkColor  ������ɫ(��͸����ʾʱλͼ����ɫΪcrBkColor�ĵ㽫������ʾ)
//  ptLU,ptLD,ptRU,ptRD  λͼ���Ͻǡ����½ǡ����Ͻǡ����½ǵ�λ��
void DrawBitmap(CDC *pDC,Bitmap *pBitmap,BOOL bTransparent,COLORREF crBkColor, CPoint ptLU,CPoint ptLD,CPoint ptRU,CPoint ptRD);

//��ɫת�ڰ�ɫ
//�������:
// pSrcDataCDC ԭͼ����
// dwOldLineBytes  ԭʼͼƬÿ�е��ֽ���
// dwSignBytes ��ͼƬÿ�е��ֽ���
// iHeight ͼ�߶�
//���������
//  pDesData ת����ɫ֮���ͼ����
void ConversionGrayTo2(BYTE *pSrcDataCDC, DWORD dw256Bytes,DWORD dwSignBytes,int iHeight, BYTE *pDesData);

//��iDataBit * 8λ���ɫת����256ɫ
//���������
//  pDIBData �������
//  dwLineBytes ���ÿ���ֽ���
//  p256Data ����/��� 256ɫ����
//  iWidth 256ɫÿ���ֽ���
//  iHeight ���ݸ߶�
//  iDataBit ����λ
void ConversionRGBToGray(BYTE *pDIBData, DWORD dwLineBytes, BYTE *p256Data, int iWidth, int iHeight, int iDataBit);

//���ݸ��������㻭��һ��ƽ���ı��Σ��������ƽ���ı������ڵľ��γ���
//���������
//  ptLU��ptLD��ptRU ������
//���������
//  iWidth��iHeight �������ڳ��ȺͿ��
//����ֵ��
// ���ε����Ͻ�
CPoint FindParallelogramRect(CPoint ptLU,CPoint ptLD,CPoint ptRU, int &iWidth, int &iHeight);


//����ʹ��
void WritreBMP(CString strFileName, int iWidth,int iHeight,int iStride,DWORD nPixFormat,BYTE *pBmpData);

#endif