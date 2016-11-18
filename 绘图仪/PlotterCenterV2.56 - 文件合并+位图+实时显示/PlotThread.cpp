// PlotThread.cpp : implementation file
#include "stdafx.h"
#include "PlotCenter.h"
#include "PlotCenterDlg.h"
#include "Instruct.h"
#include <math.h>
#include "ch375dll.h"
#include "PlotBitmap.h"
#include "ZipCompress.h"

#define BUFFER_SIZE 100000
#define HpglPixelX		0.29527559 //X方向1个hpgl单位对应的像素点数
#define HpglPixelY		0.198412732//Y方向1个hpgl单位对应的像素点数
#define MMPixelX		11.811023622//X方向1MM对应的像素点数
#define MMPixelY		7.936508//Y方向1MM对应的像素点数
#define PixelHpglY	5//1个像素点对应的hpgl单位数

BOOL bEncrypt;
BOOL g_bUserDefine=FALSE;//自定义命令
CString g_cFontName;	//字体名
unsigned char g_cIsItali;
int g_iWeight;

long g_lMarkSpace;//对接符间距
long g_lFixedPointLength;//文件起始打印长度
long g_lLinetypeStep=400;//虚线步长
double g_dMoveLength;//虚线长度
int g_iSectionCount;		//已打完位图数（一个图形被划分的位图数)
long g_lSectionPosY;//已打完页数
double g_dScaleX;//X方向放大比例
double g_dScaleY;//Y方向放大比例
char g_cWriteFile=0;		//当前要写入数据的文件号
char g_cSendFile=0;			//当前要发送的文件号
BYTE *g_pBits1;//点阵存放内存指针
BYTE *g_pBits2;//点阵存放内存指针
unsigned long g_lBits1Count;//内存块1数据个数
unsigned long g_lBits2Count;//内存块2数据个数
BOOL g_bFile1OK=false;			//文件1准备好标志,true准备好
BOOL g_bFile2OK=false;			//文件2准备好标志,true准备好

int g_nSplitNum;//页数
int g_nVersion;//版本号
int g_nLineWidth;//线宽
int g_nLineType;//线型
int g_nPen;//笔号
int g_nPenPE;//PE笔号
CPoint g_dpOrigin;//原点坐标
CPoint g_dpCurrent;//当前坐标
unsigned char g_cTerminator;//标志终结符
unsigned char g_cTerminatorMode;//模式
CString g_strText;//字符
CPoint g_dpP1,g_dpP2;//缩放点
BOOL g_bPSValid;//是否收到PS指令
BOOL g_bIPValid;//IP指令是否有效
BOOL g_bSCValid;//SC指令是否有效
CRect g_rtHardLimitation;//硬剪切限
double g_dRatioX,g_dRatioY;//当前的缩放比率
BOOL g_bAbsolute;//绝对坐标
double g_dTextAngle;//字符角度
double g_dTextHeight;//字符高度
double g_dTextWidth;//字符宽度
int g_nTextPosition;//字符相对原点的位置
BOOL g_bPenDown;//笔是否落下
int g_nPlotRotate;//文件中的旋转角度
struct stru_SC g_struSC;//SC参数
CObList g_listPrimitive;//图元表
CObList g_listTextPrimitive;//文字链表
CObList	g_listBitDataPrimitive; // 20121204 位图链表 留着以后用
CObList	g_listBitListPrimitive; // 20121204 位图链表
long g_lMaxPlotLength;//最大图形长度
long g_lMaxPlotWidth;//最大图形宽度
long g_lMaxPGLength;//段内（PG）最大图形长度
long g_lMaxPGWidth;//段内（PG）最大图形宽度
long g_lInitMaxPlotLength;//图形旋转时最大图形长度
long g_lInitMaxPlotWidth;//图形旋转时最大图形宽度
long g_lMinPlotWidth;//最小图形宽度
long g_lMaxPlotLength1;//合并文件
long g_lMaxPlotWidth1;//合并文件
int g_nRotate;//设置中的旋转
int g_nFont;//字体
long g_lHavePlotBitmapNum;//20150716-rwt
BOOL g_bisSendBMDataOver;//20150717-rwt
BOOL g_bisSendingPlotData;//20150717-rwt

CArray <CChinese_char,CChinese_char> g_aChinese_char;
CArray <CEnglish_char,CEnglish_char> g_aEnglish_char;
int g_nInternalLeading_E,g_nHeight_E,g_nDescent_E; //m_nHeight_E是windows中字体的Ascent-InternalLeading
int g_nHeight_C,g_nWidth_C;//字高，字宽
CPoint g_ptCurrentPosition;   //当前位置(坐标)
CPoint g_ptPrevPosition;      //前一位置(坐标)
CGLPrimitive1 *g_pText;//文本
CArray<POSITION,POSITION> ptArray;
bool   g_bStateBrak = false;
extern int g_iDeviceIndex; //USB设备号20110311

void WriteSolidLine(CInstruct *pInstruct);
void WriteDashLine(CInstruct *pInstruct);
CDPoint GetRotatePoint(CDPoint dp, CDPoint dpCenter);
void GLTextMinXMaxX(CGLText1 *pText);
void SetRatio(void);
void RemoveGLBuffer1(void);
void InitHPGL(void);
int DissecretFileList(CString strFileName);
int SecretFileList(CString strFileName);
int CreateList(CPlotItem *pPlotItem,int nCommunicateMode,SOCKET sock);
long IntRound(double x);
BOOL IsAlpha(char ch);
BOOL IsDigit(char ch);
void ClearInstruct(CInstruct *pInstruct);
void ProcessInstruct(CInstruct *pInstruct);
void DrawBMP(CDC *pDC,int nSplitNum,long lMarkSpace);
int GetParameterType(char cName[]);
void TextChangeLine(CGLText1 *pText);
void PlotChar(CPoint ptPointLU, BYTE *pbyData, WORD wBytes, int nLeft, int nDown, int nFontScale);
CPoint CPToLP(CPoint ptCP, int nHeight, CPoint ptPointLU, double dSin, double dCos, double dScale);
void MoveTo(long x, long y);
void LineTo(long x, long y);
void Arc(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXStartArc, int nYStartArc, int nXEndArc, int nYEndArc);
BOOL CircleCR(double x1, double y1, double x2, double y2, double x3, double y3, double &cx, double &cy, double &cr);
CPoint GetNextCoodinate(WORD wDirection, WORD wLength, CPoint ptPoint);
double angle_2(double startx, double starty, double endx, double endy);
void WriteTextData(BOOL bFirst,long x,long y);
int IntOfTwoLine(double a1, double b1, double c1, double a2, double b2, double c2, double &x, double &y);
void InitTextChangeLine(void);
void HeapSort();
//四舍五入取整
//入口:
//  x
//返回值:
//  四舍五入后的整数
long IntRound(double x)
{
	double d;
	long lResult;
	
	if (x<0)
		d = x - 0.5;
	else
		d = x + 0.5;
	lResult = (long)d;
	return lResult;
}

BOOL IsAlpha(char ch)
{
	if(ch>='a' && ch<='z') return TRUE;
	if(ch>='A' && ch<='Z') return TRUE;
	return FALSE;
}

BOOL IsDigit(char ch)
{
	if(ch>='0' && ch<='9') return TRUE;
	if(ch=='.') return TRUE;
	if(ch=='-' ||ch=='+') return TRUE;
	return FALSE;
}

void ClearInstruct(CInstruct *pInstruct)
{
//	pInstruct->m_anType.RemoveAll();
	pInstruct->m_adPara.RemoveAll();
	pInstruct->m_cName[0]=0;
}


void LineEquationCoefficient(double x1,double y1,double x2,double y2,double& a,double& b,double& c)
{
  a=(double)y2 - (double)y1;
  b=(double)x1 - (double)x2;
  c=-a*(double)x1 - b*(double)y1;
}

//求直线方程ax+by+c=0的系数
//输入参数:
//  ptPoint1,ptPoint2 直线上两点的坐标
//输出参数:
//  a,b,c 直线方程ax+by+c=0的系数
void LineEquationCoefficient(CPoint ptPoint1,CPoint ptPoint2,double& a,double& b,double& c)
{
  LineEquationCoefficient(ptPoint1.x,ptPoint1.y,ptPoint2.x,ptPoint2.y,a,b,c);
}


#define DBL_MIN 2.2250738585072014e-308
//点ptPoint到直线ax+by+c=0的距离
double DistanceOfPL(CPoint ptPoint,double a,double b,double c)
{
  double dAB,dL;
  dL=0;
  dAB=a*a + b*b;
  if (dAB > DBL_MIN)  {
    dL=fabs(a*(double)ptPoint.x + b*(double)ptPoint.y + c)/sqrt(dAB);
  }

  return dL;
}

//点ptPoint到直线ptPointL1_ptPointL2的距离
double DistanceOfPL(CPoint ptPoint,CPoint ptPointL1,CPoint ptPointL2)
{
  double a,b,c;

  LineEquationCoefficient(ptPointL1,ptPointL2,a,b,c);
  return DistanceOfPL(ptPoint,a,b,c);
}

//输入参数:
//  iFontH,iFontW  统一按分辨率为1/40mm得到的字体高和宽
//  dAngle 字体的角度，单位：度
//输出参数:
//  iHeight,iWidth 新的字体高和宽
void GetFontHW(int iFontH,int iFontW,double dAngle,int &iHeight,int &iWidth, BOOL DispFlag)
{
  CPoint ptLU,ptRU,ptLD,ptRD;
  double dCos,dSin,pi,dR,dH,dW;
  int nLineType = -g_nLineWidth / 2 - g_nLineWidth % 2 + 1;

  pi=3.1415926;
  dCos=cos(dAngle / 180.0 * pi);
  dSin=sin(dAngle / 180.0 * pi);

  //求新的高
  dR=4000.0;  //100mm换算成1/10mm为单位的长: 100 * 40;
  ptLU=CPoint(0,0);
  ptRU=CPoint(IntRound(dR * dCos),IntRound(dR * dSin));
  ptLD=CPoint(IntRound((double)iFontH * dSin),-IntRound((double)iFontH * dCos));

  //转换成设备
  if(DispFlag){
  ptLU.x = long(ptLU.x/40 * MMPixelX);	ptLU.y = long(ptLU.y/40*MMPixelY+nLineType);
  ptRU.x = long(ptRU.x/40 * MMPixelX);	ptRU.y = long(ptRU.y/40*MMPixelY+nLineType);
  ptLD.x = long(ptLD.x/40 * MMPixelX);	ptLD.y = long(ptLD.y/40*MMPixelY+nLineType);
  	}
  dH=DistanceOfPL(ptLD,ptLU,ptRU);
  iHeight=IntRound(dH);  //新的高

  //求新的宽
  dR=4000.0;  //100mm换算成1/10mm为单位的长: 100 * 40;
  ptLU=CPoint(0,0);
  ptRU=CPoint(IntRound((double)iFontW * dCos),IntRound((double)iFontW * dSin));
  ptLD=CPoint(IntRound(dR * dSin),-IntRound(dR * dCos));

  //转换成设备
  if(DispFlag){
  ptLU.x = long(ptLU.x/40 * MMPixelX);	ptLU.y = long(ptLU.y/40*MMPixelY+nLineType);
  ptRU.x = long(ptRU.x/40 * MMPixelX);	ptRU.y = long(ptRU.y/40*MMPixelY+nLineType);
  ptLD.x = long(ptLD.x/40 * MMPixelX);	ptLD.y = long(ptLD.y/40*MMPixelY+nLineType);
  	}
  dW=DistanceOfPL(ptRU,ptLU,ptLD);
  iWidth=IntRound(dW);  //新的高
}



void ProcessInstruct(CInstruct *pInstruct)
{
	int nSize;
	int i;
	CString str;
	CGLText1 *pText,*pText1;
	CPoint dp,dp1;
	POSITION pos=NULL;
	POSITION pos1;
	
	if(strcmp(pInstruct->m_cName,"PD")==0)//落笔
	{
		g_bPenDown=TRUE;
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			if(g_nPen==0)
			{
				nSize--;
				if(g_bAbsolute)
				{
					/*未改前的
					dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;
					dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y-g_lMinPlotWidth;
					*/

					if(hbwj!=2)
					{
					dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;
					dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y-g_lMinPlotWidth;
					}
					else if(hbwj==2 && (pg==1 || pg==0) && pgg!=2)
					{
					dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;//合并文件
					dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y-g_lMinPlotWidth;//合并文件
					}
                    else if(hbwj==2 && pgg==2 && pg==2)
					{
					dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x;//合并文件
					dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y+g_lMaxPlotWidth-g_lMinPlotWidth;//合并文件
					}
					else if(hbwj==2 && pg==1 && pgg==2)
					{
						if(pgnum!=0)
						{
					     dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength1*pgnum;//合并文件
						 dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y + g_lMaxPlotWidth1;//合并文件
						}
						else
						{
                         dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;//合并文件
					     dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y + g_lMaxPlotWidth;//合并文件
						}
					}
				}
				else
				{
					dp=g_dpCurrent;
					for(int i=0;i<nSize;)
					{
						dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioX);
						dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioY);
					}
				}
				g_dpCurrent=dp;
			}
			else if (g_nLineType != LINE_TYPE_SOLID)
				WriteDashLine(pInstruct);
			else
				WriteSolidLine(pInstruct);
		}
	}
	else if(strcmp(pInstruct->m_cName,"PU")==0)//提笔
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			if (g_bAbsolute)
			{
					/*未改前的
				dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;
				dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y-g_lMinPlotWidth;
				*/
				if(hbwj!=2)
					{
					    dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;
				        dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y-g_lMinPlotWidth;
					}
					else if(hbwj==2 && (pg==1 || pg==0) && pgg!=2)
					{
					    dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;//合并文件
				        dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y-g_lMinPlotWidth;//合并文件
					}
                    else if(hbwj==2 && pgg==2 && pg==2)
					{
					     dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x;//合并文件
					     dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y+g_lMaxPlotWidth-g_lMinPlotWidth;//合并文件
					}
					else if(hbwj==2 && pg==1 && pgg==2)
					{
						 if(pgnum!=0)
						 {
					     dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength1*pgnum;//合并文件
				         dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y + g_lMaxPlotWidth1-g_lMinPlotWidth;//合并文件
						 }
						 else
						 {
                         dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;//合并文件
						 dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y + g_lMaxPlotWidth-g_lMinPlotWidth;//合并文件
						 }
					}
			}
			else
			{
				dp=g_dpCurrent;
				for(int i=0;i<nSize;)
				{
					dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioX);
					dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioY);
				}
			}
			g_dpCurrent=dp;
			g_dMoveLength=0.0;
		}
		g_bPenDown=FALSE;
	}
	else if(strcmp(pInstruct->m_cName,"PR")==0)//相对绘图
	{
		g_bAbsolute=FALSE;
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			if((g_bPenDown)&&(g_nPen!=0))
			{
				if (g_nLineType != LINE_TYPE_SOLID)
					WriteDashLine(pInstruct);
				else
					WriteSolidLine(pInstruct);
			}
			else
			{
				dp=g_dpCurrent;
				for(int i=0;i<nSize;)
				{
					dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioX);
					dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioY);
				}
				g_dpCurrent=dp;
				g_dMoveLength=0.0;
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"PA")==0)//绝对绘图
	{
		g_bAbsolute=TRUE;
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			if((g_bPenDown)&&(g_nPen!=0))
			{
				if (g_nLineType != LINE_TYPE_SOLID)
					WriteDashLine(pInstruct);
				else
					WriteSolidLine(pInstruct);
			}
			else
			{
				g_dpCurrent.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;
				g_dpCurrent.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y-g_lMinPlotWidth;
				g_dMoveLength=0.0;
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"LT")==0)//线型
	{
		int n,nMode;
		double d1;
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize<=0)
			g_nLineType=LINE_TYPE_SOLID;
		else
		{
			n=ftoi(pInstruct->m_adPara.GetAt(0));
			n=abs(n);
			if(n<=8)
			{
				if(nSize>1)
				{
					d1=pInstruct->m_adPara.GetAt(1);
					if(d1>0)
					{
						if(nSize>2)
							nMode=ftoi(pInstruct->m_adPara.GetAt(2));
						else
							nMode=0;
						if((nMode==0)||(nMode==1))
						{
							if(n==0){}
							else if(n<=1) g_nLineType=LINE_TYPE_DOT;
							else if(n<=3) g_nLineType=LINE_TYPE_DASH;
							else					g_nLineType=LINE_TYPE_DASHDOT;
							if(nMode==1)
								g_lLinetypeStep=ftoi(d1*40);
							else
							{
								if(g_bIPValid)
								{
									d1=d1*sqrt((g_dpP2.x-g_dpP1.x)*(g_dpP2.x-g_dpP1.x)+(g_dpP2.y-g_dpP1.y)*(g_dpP2.y-g_dpP1.y))/100.0;
									g_lLinetypeStep=ftoi(d1);
								}
								else
									g_lLinetypeStep = 400;
							}
						}
					}
				}
				else
				{
					if(n==0){}
					else if(n<=1) g_nLineType=LINE_TYPE_DOT;
					else if(n<=3) g_nLineType=LINE_TYPE_DASH;
					else					g_nLineType=LINE_TYPE_DASHDOT;
				}
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"SP")==0)//选笔
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>MAaxPenNum)
		{
			g_nPen=ftoi(pInstruct->m_adPara.GetAt(0));
			g_nPen=((g_nPen-1)%MAaxPenNum-1)+1;
		}
		else if(nSize>0)
			g_nPen=ftoi(pInstruct->m_adPara.GetAt(0));
		else
			g_nPen=0;
	}
	else if(strcmp(pInstruct->m_cName,"PE")==0)//PE
	{
		
	}
	else if(strcmp(pInstruct->m_cName,"LO")==0)//字符相对原点位置
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=1)
		{
			int n=ftoi(pInstruct->m_adPara.GetAt(0));
			if(n<1 || n>19 || n==10)
				n=1;
			g_nTextPosition=n;
		}
		else g_nTextPosition=1;
	}
	else if(strcmp(pInstruct->m_cName,"FN")==0)//字体 20121204
	{
		g_cFontName.Empty();
		if(pInstruct->m_cFontName.IsEmpty())
			g_cFontName = pInstruct->m_cFontName;
	}
	else if(strcmp(pInstruct->m_cName,"SI")==0)//字符大小
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			g_dTextHeight=pInstruct->m_adPara.GetAt(1);
			g_dTextWidth=pInstruct->m_adPara.GetAt(0);
		}
		else 
		{
			g_dTextHeight=0.375;
			g_dTextWidth=0.285;
		}
	}
	else if(strcmp(pInstruct->m_cName,"DI")==0)//字符方向
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(bEncrypt)
		{
			g_dTextAngle=pInstruct->m_adPara.GetAt(0)*0.1;
			if((g_nFont==0)&&(g_nRotate==1))
				g_dTextAngle+=90;

			if (theApp.m_nFont==0)
			{
				g_cIsItali = pInstruct->m_cIsItali;
				g_iWeight = (int)pInstruct->m_adPara.GetAt(1);
			}
		}
		else
		{
			if(nSize>=2)
			{//20130123 解决处理格博输出文件文字坐标不正确的问题
				double d1=pInstruct->m_adPara.GetAt(0);
				double d2=pInstruct->m_adPara.GetAt(1);

				if(d1>1 || d1<-1.0)
					d1=cos(pInstruct->m_adPara.GetAt(0));
				if(d2>1 || d2<-1.0)
					d2=sin(pInstruct->m_adPara.GetAt(1));
			
				if(d1<0.10 && d1>-0.10)
					d1=0;
				if(d2<0.10 && d2>-0.10)
					d2=0;

				if(d1==0 && d2==1)
					g_dTextAngle=90;
				else if(d1==1 && d2==0)
					g_dTextAngle=0;
				else if(d1==0 && d2==-1)
					g_dTextAngle=270;
				else if(d1==-1 && d2==0)
					g_dTextAngle=-180;
				else
					g_dTextAngle=atan2(d2,d1)*180/3.14159265359;
				double tmp = (g_dTextAngle - (int)g_dTextAngle);
				if(abs((int)(tmp*10)) >= 5)
				{
					g_dTextAngle = g_dTextAngle < 0 ? g_dTextAngle += -1 : g_dTextAngle+=1;
				}
				g_dTextAngle = (int)g_dTextAngle;
			}
			else
				g_dTextAngle=0;
		
			if((g_nFont==0)&&(g_nRotate==1))
				g_dTextAngle+=90;
		}
	}
	else if(strcmp(pInstruct->m_cName,"DR")==0)//相对字符方向
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			double d1=pInstruct->m_adPara.GetAt(0);
			double d2=pInstruct->m_adPara.GetAt(1);
			if((d1==0)&&(d2!=0))
				g_dTextAngle=90;
			else if((d1!=0)&&(d2==0))
				g_dTextAngle=0;
			else if((d1!=0)&&(d2!=0)&&(fabs(d1)<=100)&&(fabs(d2)<=100)&&g_bIPValid)
			{
				d1=d1*(g_dpP2.x-g_dpP1.x);
				d2=d1*(g_dpP2.y-g_dpP1.y);
				g_dTextAngle=atan2(d1,d2);
			}
			else
			{
				if((g_nFont==0)&&(g_nRotate==1))
					g_dTextAngle-=90;
			}
		}
		else
			g_dTextAngle=0;
		if((g_nFont==0)&&(g_nRotate==1))
			g_dTextAngle+=90;
	}
	else if(strcmp(pInstruct->m_cName,"LB")==0)//字符
	{
		nSize=g_strText.GetLength();
		int j=0;
		unsigned char str1[257];
		if(bEncrypt)
		{
			if(nSize>0)
			{
				dp.x=ftoi(pInstruct->m_adPara.GetAt(0)*g_dRatioX);
				dp.y=ftoi(pInstruct->m_adPara.GetAt(1)*g_dRatioY);
				g_dpCurrent=dp;
			}
		}
		else
			dp=g_dpCurrent;
		
		while(nSize>0)
		{
			if(nSize>256)
				nSize=256;
			pText=new CGLText1;
			pText->m_nX=ftoi(dp.x);
			pText->m_nY=ftoi(dp.y);
			pText->m_dTextAngle=g_dTextAngle;
			pText->m_dTextHeight=g_dTextHeight;
			pText->m_dTextWidth=g_dTextWidth;
			pText->m_cIsItali = g_cIsItali;
			pText->m_iWeight = g_iWeight;
			pText->m_cFontName = g_cFontName;
			for(i=0;i<nSize;i++)
			{
				if(g_strText[j]==0x0d)
				{
					j++;
					dp.x=g_dpCurrent.x;
					break;
				}
				else if(g_strText[j]==0x0a)
				{
					j++;
					dp.y+=ftoi(g_dTextHeight*400);
					break;
				}
				else
					str1[i]=g_strText[j];
				j++;
			}
			if(i!=0)
			{
				str1[i]='\0';
				pText->m_strText=str1;
				GLTextMinXMaxX(pText);
			}
			else
			{
				nSize-=(i+1);
				delete pText;
				continue;
			}
			nSize-=(i+1);
			if(g_nFont==0)
			{
				pos=g_listTextPrimitive.GetTailPosition();
				if (pos==NULL)
					g_listTextPrimitive.AddTail(pText);
				else
				{//20100825  暂不处理
					pos1=pos;
					while(pos)
					{
						pText1=(CGLText1*)g_listTextPrimitive.GetPrev(pos);
						if (pText1->m_nMinPDLength>pText->m_nMinPDLength)
							pos1=pos;
						else
							break;
					}
					if (pos1==NULL)
						g_listTextPrimitive.AddHead(pText);
					else
						g_listTextPrimitive.InsertAfter(pos1,pText);
				}
			}
			else
			{
				TextChangeLine(pText);
				delete pText;
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"DF")==0)//缺省
	{
		g_nLineType=LINE_TYPE_SOLID;
		g_bAbsolute=TRUE;
		g_cTerminator=theApp.m_cTerminator;//标志终结符
		g_cTerminatorMode=1;
		g_bSCValid=false;
		g_dpOrigin=CPoint(0,0);
		g_dRatioX=1;
		g_dRatioY=1;
		if((g_nFont==0)&&(g_nRotate==1))
			g_dTextAngle=90;
		else
			g_dTextAngle=0;
		g_dTextHeight=0.375;//cm
		g_dTextWidth=0.285;//cm
		g_nTextPosition=1;
	}
	else if(strcmp(pInstruct->m_cName,"IN")==0)//初始化
	{
		g_dpCurrent=CPoint(0,0);
		g_bPenDown=FALSE;
		g_nPlotRotate=0;
		g_dpP1=CPoint(g_rtHardLimitation.left,g_rtHardLimitation.top);
		g_dpP2=CPoint(g_rtHardLimitation.right,g_rtHardLimitation.bottom);
		g_dpOrigin=CPoint(0,0);
		g_dRatioX=1;
		g_dRatioY=1;
		g_nLineType=LINE_TYPE_SOLID;
		g_bAbsolute=TRUE;
		g_cTerminator=theApp.m_cTerminator;//标志终结符
		g_cTerminatorMode=1;
		if((g_nFont==0)&&(g_nRotate==1))
			g_dTextAngle=90;
		else
			g_dTextAngle=0;
		g_dTextHeight=0.375;//cm
		g_dTextWidth=0.285;//cm
		g_nTextPosition=1;
		g_bIPValid=g_bPSValid;
		g_bSCValid=false;
	}
	else if(strcmp(pInstruct->m_cName,"PS")==0)//绘图尺寸
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			if(g_listPrimitive.GetCount()==0)
			{
				dp.x=ftoi(pInstruct->m_adPara.GetAt(0));
				dp.y=ftoi(pInstruct->m_adPara.GetAt(1));
				if((dp.x>0)&&(dp.y>0))
				{
					g_rtHardLimitation.left=0;
					g_rtHardLimitation.top=0;
					g_rtHardLimitation.right=dp.x;
					g_rtHardLimitation.bottom=dp.y;
					g_dpP1.x=0;
					g_dpP1.y=0;
					g_dpP2=dp;
					g_bPSValid=true;
					g_bIPValid=true;
				}
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"IP")==0)//缩放点
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=4)
		{
			g_dpP1=CPoint(ftoi(pInstruct->m_adPara.GetAt(0)),ftoi(pInstruct->m_adPara.GetAt(1)));
			g_dpP2=CPoint(ftoi(pInstruct->m_adPara.GetAt(2)),ftoi(pInstruct->m_adPara.GetAt(3)));
			g_bIPValid=true;
		}
		else if(nSize>=2)
		{
			int xd,yd;
			xd=g_dpP2.x-g_dpP1.x;
			yd=g_dpP2.y-g_dpP1.y;
			g_dpP1=CPoint(ftoi(pInstruct->m_adPara.GetAt(0)),ftoi(pInstruct->m_adPara.GetAt(1)));
			g_dpP2=g_dpP1+CPoint(xd,yd);
			if(g_bIPValid || g_bPSValid)
				g_bIPValid=true;
		}
		else
		{
			g_dpP1=CPoint(g_rtHardLimitation.left,g_rtHardLimitation.top);
			g_dpP2=CPoint(g_rtHardLimitation.right,g_rtHardLimitation.bottom);
			g_bIPValid=g_bPSValid;
		}
		SetRatio();
	}
	else if(strcmp(pInstruct->m_cName,"IR")==0)//相对缩放点
	{
		int xd,yd;
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=4)
		{
			xd=g_rtHardLimitation.Width();
			yd=g_rtHardLimitation.Height();
			g_dpP1=CPoint(ftoi(xd*pInstruct->m_adPara.GetAt(0)),ftoi(yd*pInstruct->m_adPara.GetAt(1)));
			g_dpP2=CPoint(ftoi(xd*pInstruct->m_adPara.GetAt(2)),ftoi(yd*pInstruct->m_adPara.GetAt(3)));
		}
		else if(nSize>=2)
		{
			xd=g_dpP2.x-g_dpP1.x;
			yd=g_dpP2.y-g_dpP1.y;
			dp=CPoint(xd,yd);
			xd=g_rtHardLimitation.Width();
			yd=g_rtHardLimitation.Height();
			g_dpP1=CPoint(ftoi(xd*pInstruct->m_adPara.GetAt(0)),ftoi(yd*pInstruct->m_adPara.GetAt(1)));
			g_dpP2=g_dpP1+dp;
		}
		else
		{
			g_dpP1=CPoint(g_rtHardLimitation.left,g_rtHardLimitation.top);
			g_dpP2=CPoint(g_rtHardLimitation.right,g_rtHardLimitation.bottom);
		}
		g_bIPValid=g_bPSValid;
		SetRatio();
	}
	else if(strcmp(pInstruct->m_cName,"SC")==0)//缩放
	{
		int n;
		nSize=pInstruct->m_adPara.GetSize();
		if((nSize>=4)&&(nSize!=6))
		{
			if(nSize==4)
				n=0;
			else
				n=ftoi(pInstruct->m_adPara.GetAt(4));
			if(n!=2)
			{
				if(pInstruct->m_adPara.GetAt(0)!=pInstruct->m_adPara.GetAt(1) &&
					pInstruct->m_adPara.GetAt(2)!=pInstruct->m_adPara.GetAt(3) )
				{
					g_struSC.dXMin=pInstruct->m_adPara.GetAt(0);
					g_struSC.dXMax=pInstruct->m_adPara.GetAt(1);
					g_struSC.dYMin=pInstruct->m_adPara.GetAt(2);
					g_struSC.dYMax=pInstruct->m_adPara.GetAt(3);
					g_struSC.nType=n;
					if(n==1)
					{
						if(nSize>5)
						{
							g_struSC.dLeft=pInstruct->m_adPara.GetAt(5);
							g_struSC.dBottom=pInstruct->m_adPara.GetAt(6);
						}
						else
						{
							g_struSC.dLeft=50;
							g_struSC.dBottom=50;
						}
					}
					g_bSCValid=true;
					SetRatio();
				}
			}
			else
			{
				if(pInstruct->m_adPara.GetAt(1)!=0 && pInstruct->m_adPara.GetAt(3)!=0)
				{
					g_struSC.dXMin=pInstruct->m_adPara.GetAt(0);
					g_struSC.dXMax=pInstruct->m_adPara.GetAt(1);
					g_struSC.dYMin=pInstruct->m_adPara.GetAt(2);
					g_struSC.dYMax=pInstruct->m_adPara.GetAt(3);
					g_struSC.nType=n;
					g_bSCValid=true;
					SetRatio();
				}
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"IW")==0)//软剪切限
	{
		
	}
	else if((strcmp(pInstruct->m_cName,"PG")==0)  ||(strcmp(pInstruct->m_cName,"FR")==0))//推进页面
	{
		if(g_nRotate==1)
		{
			g_lMaxPlotLength=g_lMaxPGWidth;
			if (g_lMaxPlotWidth < g_lMaxPGLength)
				g_lMaxPlotWidth = g_lMaxPGLength;
		}
		else
		{
			/*未改前的
			g_lMaxPlotLength=g_lMaxPGLength;
			if (g_lMaxPlotWidth < g_lMaxPGWidth)
				g_lMaxPlotWidth = g_lMaxPGWidth;
			*/

			pg=1;/////12345678
		    pgnum=pgnum+1;
	    	g_lMaxPlotLength=g_lMaxPGLength;
	    	if (g_lMaxPlotWidth < g_lMaxPGWidth)
			g_lMaxPlotWidth = g_lMaxPGWidth+100;
		    if(pgg!=2)
            g_lMaxPlotWidth1 = g_lMaxPlotWidth;
		    if(pgg != 2 && pgnum ==1)
            g_lMaxPlotLength1 = g_lMaxPlotLength; 
		}
	}

	else if((strcmp(pInstruct->m_cName,"PP")==0)||(strcmp(pInstruct->m_cName,"FR")==0) || (strcmp(pInstruct->m_cName,"!G")==0))//推进页面//合并文件
	{
		pg=2;
		pgg=2;
		pgnum=0;
		g_lMaxPlotLength=g_lMaxPGLength;
		if (g_lMaxPlotWidth < g_lMaxPGWidth)
		g_lMaxPlotWidth = g_lMaxPGWidth+100;

	}

/*	else if(strcmp(pInstruct->m_cName,"RP")==0)//重新绘图
	{
	}
	else if(strcmp(pInstruct->m_cName,"RO")==0)//旋转角度
	{
		nSize=pInstruct->m_alPara.GetSize();
		if(nSize>=1)
		{
			d=pInstruct->m_alPara.GetAt(0);
			if(d==0) m_nRotate=0;
			else if(d==90) m_nRotate=90;
			else if(d==180) m_nRotate=180;
			else if(d==270) m_nRotate=270;
		}
		else m_nRotate=0;
	}*/
}

int GetFontWidth(LOGFONT lfFont)
{
  CDC dc;
  HDC hDC;
  TEXTMETRIC tm;
	CFont font0;
  int iWidth;
  CFont *pFontOld;

  hDC=::GetDC(NULL);
  dc.Attach(hDC);

  font0.CreateFontIndirect(&lfFont);
  pFontOld=dc.SelectObject(&font0);

  dc.GetTextMetrics(&tm);
  iWidth=tm.tmAveCharWidth;

  dc.SelectObject(pFontOld);

  dc.Detach();
  ::ReleaseDC(NULL,hDC);

  return iWidth;
}

//角度转换
//输入参数:
//  dAngle 角度，单位：度
//  dDPMM_X  X轴分辨率
//  dDPMM_Y  Y轴分辨率
//返回值:
//  角度,单位：度,在[0,360)之间
double Angle_L2D(double dAngle,double dDPMM_X,double dDPMM_Y)
{
  int startx,starty,endx,endy;
  double pi,q1;
  double x,y;

  pi=3.1415926;

  startx=0;
  starty=0;

  endx=IntRound(100.0 * cos(dAngle / 180.0 * pi) * dDPMM_X);
  endy=IntRound(100.0 * sin(dAngle / 180.0 * pi) * dDPMM_Y);

  q1=0.0;
  if (endx == startx)  {
    if (starty==endy)   return(q1 / pi * 180.0);

    if (starty<endy)  q1=0.5*pi;
    else  q1=1.5*pi;
  }
  else if (endy == starty)  {
    if (startx < endx)  q1=0;
    else q1=pi;
  }
  else  {
    x=(double)endx-(double)startx;
    y=(double)endy-(double)starty;
    q1=atan(y/x);
    if (x < 0)  {  //第二三象限
      q1=pi+q1;
    }
    else  {
      if ((x > 0) && (y < 0))  {  //第四象限
        q1=(double)2.0*pi+q1;
      }
    }
  }

  return (q1 / pi * 180.0);
}


void DrawBMP(CDC* pDC,int nSplitNum,long lMarkSpace)
{
	int i;
	int iSectionValueY1,iSectionValueX;
	int iSectionValueX_1,iSectionValueX_2;
	int iSectionValueY_1,iSectionValueY_2,iSectionValueY_3;
	POSITION pos,pos1;
	CGLPrimitive1 *p;
	CGLText1 *pText;
	CPoint pt;
	int nSize;
	CPen pen;
	CPen *oldPen;
	int nLineType;

	pos=g_listPrimitive.GetHeadPosition();
	iSectionValueY1 = g_iSectionCount * g_lSectionPosY;
	iSectionValueY_1 = iSectionValueY1 + g_lSectionPosY + 5;
	iSectionValueY_2 = iSectionValueY_1 - 10;
	iSectionValueY_3 = iSectionValueY1 - 5;
	iSectionValueX = (nSplitNum + 1) * g_lMaxPlotWidth * 8;
	nSplitNum++;
	iSectionValueX_1 = iSectionValueX + 5;
	iSectionValueX_2 = iSectionValueX_1 - 10;
	//pDC->SetBkColor(RGB(0,0,0));
	pDC->SetBkColor(RGB(255,255,255));
	if (g_nSplitNum > 1)
	{//分页
		//pen.CreatePen(LINE_TYPE_DOT,1,RGB(255,255,255));
		pen.CreatePen(LINE_TYPE_DOT,1,RGB(0,0,0));
		oldPen = pDC->SelectObject(&pen);
		nSize = g_nLineWidth / 2 + 1;
		if ((nSplitNum != g_nSplitNum) && (iSectionValueY_1 >= g_lMaxPlotLength))
		{
			pt.x=g_lMaxPlotLength-ftoi(5*40*g_dScaleX*HpglPixelX)-iSectionValueY1;
			nLineType = -g_nLineWidth / 2 - g_nLineWidth % 2 + 1;
			while (nLineType < nSize)
			{
				pt.y=0;
				pDC->MoveTo(pt.x+nLineType,pt.y);
				pt.y=g_lMaxPlotWidth*8;
				pDC->LineTo(pt.x+nLineType,pt.y);
				nLineType++;
			}
		}
		long lMaxPlotLength = g_lMaxPlotLength - ftoi(30*40*g_dScaleX*HpglPixelX);
		//画分页对接符号
		while (g_lMarkSpace < iSectionValueY_1)
		{
			if (g_lMarkSpace > lMaxPlotLength)
				break;
			nLineType = -g_nLineWidth / 2 - g_nLineWidth % 2 + 1;
			if (nSplitNum == 1)
			{
				while (nLineType < nSize)
				{
					pt.y=0;
					pt.x=g_lMarkSpace+nLineType-iSectionValueY1;
					pDC->MoveTo(pt);
					pt.y=long(10 * MMPixelY)+nLineType;
					pDC->LineTo(pt);
					pt.x+=long(20 *MMPixelX);
					pDC->LineTo(pt);
					pt.y=0;
					pDC->LineTo(pt);
					pt.x-=long(10 * MMPixelX);
					pDC->MoveTo(pt);
					pt.y=long(10 * MMPixelY)+nLineType;
					pt.x=g_lMarkSpace+nLineType-iSectionValueY1;
					pDC->LineTo(pt);
					pt.y=0;
					pt.x+=long(10 *MMPixelX);
					pDC->MoveTo(pt);
					pt.y=long(10 * MMPixelY)+nLineType;
					pt.x+=long(10 *MMPixelX);
					pDC->LineTo(pt);
					nLineType++;
				}
			}
			else if (nSplitNum == g_nSplitNum)
			{
				while (nLineType < nSize)
				{
					pt.x=g_lMarkSpace+nLineType-iSectionValueY1;
					pt.y=g_lMaxPlotWidth * 8 + nLineType;
					pDC->MoveTo(pt);
					pt.y-=long(10 * MMPixelY);
					pDC->LineTo(pt);
					pt.x+=long(20 * MMPixelX);
					pDC->LineTo(pt);
					pt.y=g_lMaxPlotWidth * 8 + nLineType;
					pDC->LineTo(pt);
					pt.x-=long(10 * MMPixelX);
					pDC->MoveTo(pt);
					pt.x=g_lMarkSpace+nLineType-iSectionValueY1;
					pt.y-=long(10 * MMPixelY);
					pDC->LineTo(pt);
					pt.x+=long(10 *MMPixelX);
					pt.y=g_lMaxPlotWidth * 8 + nLineType;
					pDC->MoveTo(pt);
					pt.x+=long(10 *MMPixelX);
					pt.y-=long(10 * MMPixelY);
					pDC->LineTo(pt);
					nLineType++;
				}
			}
			else
			{
				while (nLineType < nSize)
				{
					pt.y=0;
					pt.x=g_lMarkSpace+nLineType-iSectionValueY1;
					pDC->MoveTo(pt);
					pt.y=long(10 * MMPixelY)+nLineType;
					pDC->LineTo(pt);
					pt.x+=long(20 *MMPixelX);
					pDC->LineTo(pt);
					pt.y=0;
					pDC->LineTo(pt);
					pt.x-=long(10 * MMPixelX);
					pDC->MoveTo(pt);
					pt.y=long(10 * MMPixelY)+nLineType;
					pt.x=g_lMarkSpace+nLineType-iSectionValueY1;
					pDC->LineTo(pt);
					pt.y=0;
					pt.x+=long(10 *MMPixelX);
					pDC->MoveTo(pt);
					pt.y=long(10 * MMPixelY)+nLineType;
					pt.x+=long(10 *MMPixelX);
					pDC->LineTo(pt);

					pt.y=g_lMaxPlotWidth * 8 + nLineType;
					pt.x=g_lMarkSpace+nLineType-iSectionValueY1;
					pDC->MoveTo(pt);
					pt.y-=long(10 * MMPixelY);
					pDC->LineTo(pt);
					pt.x+=long(20 *MMPixelX);
					pDC->LineTo(pt);
					pt.y=g_lMaxPlotWidth * 8 + nLineType;
					pDC->LineTo(pt);
					pt.x-=long(10 * MMPixelX);
					pDC->MoveTo(pt);
					pt.y-=long(10 * MMPixelY);
					pt.x=g_lMarkSpace+nLineType-iSectionValueY1;
					pDC->LineTo(pt);
					pt.y=g_lMaxPlotWidth * 8 + nLineType;
					pt.x+=long(10 *MMPixelX);
					pDC->MoveTo(pt);
					pt.y-=long(10 * MMPixelY);
					pt.x+=long(10 *MMPixelX);
					pDC->LineTo(pt);
					nLineType++;
				}
			}
			g_lMarkSpace += lMarkSpace;
		}
		if ((g_lMarkSpace-lMarkSpace+(20 *MMPixelX)) > iSectionValueY_2)
			g_lMarkSpace-=lMarkSpace;
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
	}
	nLineType=LINE_TYPE_SOLID;
	//pen.CreatePen(nLineType,g_nLineWidth,RGB(255,255,255));
	pen.CreatePen(nLineType,g_nLineWidth,RGB(0,0,0));
	oldPen = pDC->SelectObject(&pen);
	while(pos)
	{
		pos1=pos;
		p=(CGLPrimitive1*)g_listPrimitive.GetNext(pos);
		switch(p->m_nType)
		{
		case PRIMITIVE_LINE:
			nSize=p->m_anData.GetSize();
			if(nSize>=2)
			{
				if(p->m_nMinPDLength>iSectionValueY_1)
				{
					break;
					pos=NULL;
				}
				else if(p->m_nMaxPDLength<iSectionValueY_3)
					break;
				else if(p->m_nMinPDWidth>iSectionValueX_1)
					break;
				pt.x=p->m_anData.GetAt(0)-iSectionValueY1;
				pt.y=iSectionValueX-p->m_anData.GetAt(1);
				pDC->MoveTo(pt);
				
				for(i=2;i<nSize;)
				{
					pt.x=p->m_anData.GetAt(i++)-iSectionValueY1;
					pt.y=iSectionValueX-p->m_anData.GetAt(i++);
					pDC->LineTo(pt);
				}
				if (p->m_nMaxPDWidth < iSectionValueX_2)
				{
					if(p->m_nMaxPDLength < iSectionValueY_2)
					{
						p->m_anData.RemoveAll();
						g_listPrimitive.RemoveAt(pos1);
						delete p;
					}
				}
			}
			else
			{
				p->m_anData.RemoveAll();
				g_listPrimitive.RemoveAt(pos1);
				delete p;
			}
			break;
		case PRIMITIVE_DASH:
			nSize=p->m_anData.GetSize();
			if(nSize>=4)
			{
				if(p->m_nMinPDLength>iSectionValueY_1)
				{
					break;
					pos=NULL;
				}
				else if(p->m_nMaxPDLength<iSectionValueY_3)
					break;
				else if(p->m_nMinPDWidth>iSectionValueX_1)
					break;
				nSize-=3;
				for(i=0;i<nSize;)
				{
					pt.x=p->m_anData.GetAt(i++)-iSectionValueY1;
					pt.y=iSectionValueX-p->m_anData.GetAt(i++);
					pDC->MoveTo(pt);
					pt.x=p->m_anData.GetAt(i++)-iSectionValueY1;
					pt.y=iSectionValueX-p->m_anData.GetAt(i++);
					pDC->LineTo(pt);
				}
				if (p->m_nMaxPDWidth < iSectionValueX_2)
				{
					if(p->m_nMaxPDLength < iSectionValueY_2)
					{
						p->m_anData.RemoveAll();
						g_listPrimitive.RemoveAt(pos1);
						delete p;
					}
				}
			}
			else
			{
				p->m_anData.RemoveAll();
				g_listPrimitive.RemoveAt(pos1);
				delete p;
			}
			break;
		}
	}
	pDC->SelectObject(oldPen);
	pen.DeleteObject();

		// 位图显示 
	pos = g_listBitListPrimitive.GetHeadPosition();
	//pos1 = g_listBitDataPrimitive.GetHeadPosition();
	while(pos != NULL)
	{
		BitList *pBitPtr = (BitList*)g_listBitListPrimitive.GetNext(pos);

		//if(pBitPtr->m_nMinPDLength>iSectionValueY_1)
		//	break;
		//else if(pBitPtr->m_nMaxPDLength<iSectionValueY_3)
		//	break;
		//else if(pBitPtr->m_nMinPDWidth>iSectionValueX_1)
		//	break;
		
		CPoint bPostLU,bPostLD,bPostRU,bPostRD;
		bPostLU.x=pBitPtr->bPostLU.x-iSectionValueY1;
		bPostLU.y=iSectionValueX-pBitPtr->bPostLU.y;
		bPostLD.x=pBitPtr->bPostLD.x-iSectionValueY1;
		bPostLD.y=iSectionValueX-pBitPtr->bPostLD.y;
		bPostRU.x=pBitPtr->bPostRU.x-iSectionValueY1;
		bPostRU.y=iSectionValueX-pBitPtr->bPostRU.y;
		bPostRD.x=pBitPtr->bPostRD.x-iSectionValueY1;
		bPostRD.y=iSectionValueX-pBitPtr->bPostRD.y;

		/*
		pDC->MoveTo(bPostLU);
		pDC->LineTo(bPostLD);
		pDC->MoveTo(bPostLD);
		pDC->LineTo(bPostRD);
		pDC->MoveTo(bPostRD);
		pDC->LineTo(bPostRU);
		pDC->MoveTo(bPostRU);
		pDC->ineTo(bPostLU);
        */
		
		//pDC->MoveTo(50, 200);
		//pDC->LineTo(50, 800);
		//DrawBitmap(pDC,pBitPtr->pBitmap,pBitPtr->m_bTransparent , RGB(255,255,255), pBitPtr->bPostLU, pBitPtr->bPostLD, pBitPtr->bPostRU, pBitPtr->bPostRD);
		DrawBitmap(pDC,pBitPtr->pBitmap,TRUE, RGB(255,255,255), bPostLU, bPostLD, bPostRU, bPostRD);
	}
	//pDC->SelectObject(oldPen);
	//pen.DeleteObject();

	if (g_nFont==0)
	{
	CFont font;
		CFont *pOldFont;
		LOGFONT lf;
		CFont fontNew;
		BOOL b;
		double dTextAngle;
		CPoint ptPoint, ptPointL1, ptPointL2;
		int fw,fh, opt;
		pDC->SetBkMode(TRANSPARENT);
		//pDC->SetTextColor(RGB(255,255,255));
		pDC->SetTextColor(RGB(0,0,0));
		pos=g_listTextPrimitive.GetHeadPosition();
		while(pos)
		{
			pos1=pos;
			pText=(CGLText1*)g_listTextPrimitive.GetNext(pos);
			if(!bEncrypt)
			{
				if(pText->m_nMinPDLength>iSectionValueY_1)
					break;
				else if(pText->m_nMaxPDLength<iSectionValueY_3)
					continue;
				else if(pText->m_nMinPDWidth>iSectionValueX_1)
					continue;
			}
			if(pText->m_dTextHeight==0)
				continue;
			
			if(!font.CreateStockObject(DEFAULT_GUI_FONT)) 
				VERIFY(font.CreateStockObject(SYSTEM_FONT));
			pOldFont=pDC->SelectObject(&font);
			pOldFont->GetLogFont(&lf);
			
			if(!bEncrypt)
			{
				strcpy(lf.lfFaceName,"HP Roman 8");
				lf.lfHeight=ftoi(pText->m_dTextHeight*3000.0/25.4*g_dScaleX);
				lf.lfWidth = 0;
				lf.lfWeight=FW_THIN;
				//dTextAngle = pText->m_dTextAngle;
				dTextAngle = Angle_L2D((pText->m_dTextAngle),(MMPixelX),(MMPixelY));
			}
			else
			{
				strcpy(lf.lfFaceName,pText->m_cFontName);
				fw = ftoi(pText->m_dTextWidth*10*40);
				fh = ftoi(pText->m_dTextHeight*10*40);
				lf.lfHeight = fh;
				lf.lfWidth = fw;
				opt = 1;

				while(fh > 20000.0)
				{
					fh /= 10;
					opt *= 10;
				}

				if(fw == 0)
				{
					lf.lfHeight = fh;
					lf.lfWidth = 0;
					fw = ftoi(GetFontWidth(lf));
				}
				else
					fw = ftoi(GetFontWidth(lf));
			
				if(opt > 1)
				{
					fw *= opt;
				}

				GetFontHW(fh, fw, pText->m_dTextAngle, fh, fw, TRUE);
				lf.lfHeight = fh;
				lf.lfWidth = fw;

				lf.lfWeight = pText->m_iWeight;
				lf.lfItalic = pText->m_cIsItali;
				dTextAngle = Angle_L2D((pText->m_dTextAngle),(MMPixelX),(MMPixelY));
			}	
				

			lf.lfOrientation=(long)dTextAngle*10;//ftoi(dTextAngle*10);
			lf.lfEscapement=(long)dTextAngle*10;//ftoi(dTextAngle*10);
			b=fontNew.CreateFontIndirect(&lf);
			pDC->SelectObject(&fontNew);

			nSize=pText->m_strText.GetLength();
			if(nSize>256)
				nSize=256;
			pDC->TextOut(pText->m_nX-iSectionValueY1,iSectionValueX-pText->m_nY,pText->m_strText);
			pDC->SelectObject(pOldFont);
			font.DeleteObject();
			fontNew.DeleteObject();
			
			if(!bEncrypt)
			{
				if (pText->m_nMaxPDWidth < iSectionValueX_2)
				{
					if(pText->m_nMaxPDLength < iSectionValueY_2)
					{
						g_listTextPrimitive.RemoveAt(pos1);
						delete pText;
					}
				}
			}
		}
	}
	g_iSectionCount++;
}

int GetParameterType(char cName[])
{
	int nType=-1;

	if		 (strcmp(cName,"DF")==0) nType=NONE_TYPE;//缺省
	else if(strcmp(cName,"IN")==0) nType=DIGIT_TYPE;//初始化
	else if(strcmp(cName,"PS")==0) nType=DIGIT_TYPE;//绘图尺寸
	else if(strcmp(cName,"IP")==0) nType=DIGIT_TYPE;//缩放点
	else if(strcmp(cName,"IR")==0) nType=DIGIT_TYPE;//相对缩放点
	else if(strcmp(cName,"SC")==0) nType=DIGIT_TYPE;//缩放
	else if(strcmp(cName,"PG")==0) nType=DIGIT_TYPE;//推进页面
	else if(strcmp(cName,"FR")==0) nType=DIGIT_TYPE;//推进页面
	else if(strcmp(cName,"AA")==0) nType=DIGIT_TYPE;//绝对画弧
	else if(strcmp(cName,"AR")==0) nType=DIGIT_TYPE;//相对画弧
	else if(strcmp(cName,"CI")==0) nType=DIGIT_TYPE;//圆
	else if(strcmp(cName,"PA")==0) nType=DIGIT_TYPE;//绝对绘图
	else if(strcmp(cName,"PD")==0) nType=DIGIT_TYPE;//落笔
	else if(strcmp(cName,"PR")==0) nType=DIGIT_TYPE;//相对绘图
	else if(strcmp(cName,"PU")==0) nType=DIGIT_TYPE;//提笔
	else if(strcmp(cName,"SP")==0) nType=DIGIT_TYPE;//选笔
	else if(strcmp(cName,"DI")==0) nType=DIGIT_TYPE;//绝对方向------------
	else if(strcmp(cName,"DR")==0) nType=DIGIT_TYPE;//相对方向
	else if(strcmp(cName,"SI")==0) nType=DIGIT_TYPE;//绝对字符尺寸
	else if(strcmp(cName,"LO")==0) nType=DIGIT_TYPE;//标志原点172
	else if(strcmp(cName,"DT")==0) nType=DT_TYPE;//定义标志终结符
	else if(strcmp(cName,"LB")==0)
	{
		g_strText.Empty();
		nType=STRING_TYPE;//标志171
	}
	else if(strcmp(cName,"LT")==0) nType=DIGIT_TYPE;//线型119
	
	if(nType==-1)
	{
		int k=0;
		nType=DIGIT_TYPE;
		//ASSERT(FALSE);
	}		

	return nType;
}

void SetRatio(void)
{
	double d1,d2;
	if(g_dpP2.x==g_dpP1.x) g_dpP2.x+=1;
	if(g_dpP2.y==g_dpP1.y) g_dpP2.y+=1;
	if (!g_bIPValid || !g_bSCValid)
		return;
	
	switch(g_struSC.nType)
	{
		case 0://异步
			g_dRatioX=(g_dpP2.x-g_dpP1.x)/(g_struSC.dXMax-g_struSC.dXMin);
			g_dRatioY=(g_dpP2.y-g_dpP1.y)/(g_struSC.dYMax-g_struSC.dYMin);
			g_dpOrigin.x=ftoi(g_dpP1.x-g_struSC.dXMin*g_dRatioX);
			g_dpOrigin.y=ftoi(g_dpP1.y-g_struSC.dYMin*g_dRatioY);
			break;
		case 1://同步
			d1=(g_dpP2.x-g_dpP1.x)/(g_struSC.dXMax-g_struSC.dXMin);
			d2=(g_dpP2.y-g_dpP1.y)/(g_struSC.dYMax-g_struSC.dYMin);
			if(d1>d2) //x>y
			{
				g_dRatioY=d2;
				g_dRatioX=d2;
				g_dpOrigin.y=g_dpP1.y;
				g_dpOrigin.x=ftoi(((g_dpP2.x-g_dpP1.x)-(g_struSC.dXMax-g_struSC.dXMin)*d2)*g_struSC.dLeft/100.0);
			}
			else //x<y
			{
				g_dRatioX=d1;
				g_dRatioY=d1;
				g_dpOrigin.x=g_dpP1.x;
				g_dpOrigin.y=ftoi(((g_dpP2.y-g_dpP1.y)-(g_struSC.dYMax-g_struSC.dYMin)*d1)*g_struSC.dBottom/100.0);
			}
			break;
		case 2://点因子
			g_dRatioX=g_struSC.dXMax;
			g_dRatioY=g_struSC.dYMax;
			g_dpOrigin.x=ftoi(g_dpP1.x-g_struSC.dXMin*g_dRatioX);
			g_dpOrigin.y=ftoi(g_dpP1.y-g_struSC.dYMin*g_dRatioY);
			break;
	}
}

void RemoveGLBuffer1(void)
{
POSITION pos=g_listPrimitive.GetHeadPosition();
	while(pos)
	{
		CGLPrimitive1 *p=(CGLPrimitive1*)g_listPrimitive.GetNext(pos);
		p->m_anData.RemoveAll();
		delete p;
	}
	g_listPrimitive.RemoveAll();

	pos=g_listTextPrimitive.GetHeadPosition();
	while(pos)
	{
		CGLText1 *pText=(CGLText1 *)g_listTextPrimitive.GetNext(pos);
		delete pText;
	}
	g_listTextPrimitive.RemoveAll();

	int i,listCount = 0;
	listCount = g_listBitDataPrimitive.GetCount();
	pos = g_listBitDataPrimitive.GetHeadPosition();
	for(i=0; i<listCount && pos!=NULL; i++)
	{
		BitPrimitive *pBitPrimitive = (BitPrimitive*)g_listBitDataPrimitive.GetNext(pos);
		if(pBitPrimitive != NULL)
		{
			delete pBitPrimitive;
			g_listBitDataPrimitive.RemoveTail();
		}
	}
	g_listBitDataPrimitive.RemoveAll();

	listCount = g_listBitListPrimitive.GetCount();
	pos = g_listBitListPrimitive.GetHeadPosition();
	for(i=0; i<listCount && pos!=NULL; i++)
	{
		BitList *pBitList = (BitList*)g_listBitListPrimitive.GetNext(pos);
		if(pBitList != NULL)
		{
			delete pBitList;
			g_listBitListPrimitive.RemoveTail();
		}
	}
	g_listBitListPrimitive.RemoveAll();

}

void InitHPGL(void)
{
	g_nFont=theApp.m_nFont;
	g_bFile1OK=false;
	g_bFile2OK=false;
	g_cWriteFile=0;
	g_cSendFile=0;
	g_bPSValid=false;
	g_bIPValid=false;
	g_bSCValid=false;
	g_rtHardLimitation=CRect(0,0,40000,40000);
	g_dpOrigin=CPoint(0,0);
	g_dpP1=CPoint(0,0);
	g_dpP2=CPoint(40000,40000);
	g_dRatioX=1;//当前的缩放比率
	g_dRatioY=1;//当前的缩放比率
	g_nPlotRotate=0;
	g_dpCurrent=CPoint(0,0);
	g_bPenDown=FALSE;
	g_nLineType=LINE_TYPE_SOLID;
	g_bAbsolute=TRUE;
	g_nPen=1;
	g_cTerminator=theApp.m_cTerminator;//标志终结符
	g_cTerminatorMode=1;
	if((g_nFont==0)&&(g_nRotate==1))
			g_dTextAngle=90;
		else
			g_dTextAngle=0;
	g_dTextHeight=0.375;//cm
	g_dTextWidth=0.285;//cm
	g_nTextPosition=1;
	g_lMaxPlotLength=0;

    g_lMaxPlotWidth=0;

	g_lMaxPGLength=0;
	g_lMaxPGWidth=0;
	g_dMoveLength=0;
	RemoveGLBuffer1();

	//m_dFixedPointValue = 0;
}

int BitMapData2(CString strFileName)
{
	int i,j,k,m,n,nRemainChar;
	unsigned char cFileBuf[1025];
	unsigned char cSecretBuf[256];
	char errRe;
	//BYTE *ptrBitData;
	//long lBitDataLen;

	BOOL bError=FALSE;
	m=j=0;
	nRemainChar = 0;
	errRe=0;

#if _DEBUG
	CString strWorkPath1("c:\\bimap1.dat");
#else	
	TCHAR szFullPath[_MAX_PATH];
  	TCHAR szDir[_MAX_DIR];
  	TCHAR szDrive[_MAX_DRIVE];
 	CString strWorkPath1;
	
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
	strWorkPath1 += "HidIc.chv";
#endif
    CFile hFile;
	CFileStatus status;

	if(!hFile.Open(strWorkPath1,CFile::modeCreate | CFile::modeWrite))
		return 0;
	//SetFileAttributes(strWorkPath1, FILE_ATTRIBUTE_HIDDEN ); //隐藏
      // Attach a CFile object to the handle we have.
      
	CFile file;
	//20100406新增CFile::shareExclusive属性，避免其他CAD文件写自动绘图目录时写入不成功或打印缺页
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return 0;

	int nLength=file.GetLength();
	file.Read(&cFileBuf[0],22);
	i = 22;
//	lBitDataLen = 0;
//	ptrBitData = new BYTE[nLength];
	for(;i<nLength;)
	{
		k = 0;
		if (m < j)
		{
			while(k<(j-m))
			{
				cFileBuf[k] = cFileBuf[m+k];
				k++;
			}
			j = 1024 - k;
		}
		else
			j = 1024;
		if (nRemainChar > 0)
		{
			n = 252 - nRemainChar + 2;
			if (j > nRemainChar)
			{
				m = 254;
				j -= nRemainChar;
				i += nRemainChar;
			}
			else
			{
				m = n + j;
				i += j;
				j = 0;
			}
			if ((n < m) && ((nRemainChar & 0x01) != 0))
			{
				nRemainChar -= (m - n);
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
			}
			else
				nRemainChar -= (m - n);
			for (;n<m;)
			{
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
			}	
		}
	
		while (j > 0)
		{
			if (nLength == i)
			{
				break;
			}
			if ((nLength - i) < 256)
			{
				bError = TRUE;
				errRe = 1;
				break;
			}
			file.Read(cSecretBuf,256);
			i += 256;
			cSecretBuf[0] ^= cSecretBuf[254];
			cSecretBuf[1] ^= cSecretBuf[255];
			cSecretBuf[0] ^= 0xac;
			cSecretBuf[1] ^= 0xe3;
			if (j < 252)
			{
				m = j + 2;
				nRemainChar = 252 - j;
				j = 0;
				i -= nRemainChar;
			}
			else
			{
				m = 254;
				j -= 252;
				nRemainChar = 0;
			}
			if ((m & 0x01) != 0)
			{
				m--;
				for (n=2;n<m;)
				{
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
				}
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
			}
			else
			{
				for (n=2;n<m;)
				{
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
				}
			}
		}
		j = k;
		k = 0;
		m = j;
		hFile.Write(cFileBuf, j);
		if (bError)
			break;
		
		//while(k<j)
		//{
		//	ptrBitData[lBitDataLen++] = cFileBuf[k++];
		//}
	}
//////////////////////////////////////////////////////
	//hFile.Write(ptrBitData, lBitDataLen-1);
	file.Close();
    hFile.Close();
	if(errRe == 0)
		return TRUE;
	else
		return FALSE;
}


int SecretFileNew(CString strFileName)
{	
	BOOL  bDecodingFlg;
	int nProcess;
	unsigned char cParameterType;
	unsigned char cFileBuf[1025];
	long i,j,k,m,n;//, iWidth, iHeight;
	long nLength, iPackLen;
	BOOL bError=FALSE;
	CInstruct Instruct;
	unsigned char ch,*cPin;
	int nParameter;
	double dParm;
	long dCount, dloop;
	LPTSTR pStr;
	BitPrimitive *pBitPtr;
	BitList	*pBitList;

	bDecodingFlg = BitMapData2(strFileName);	
	if(!bDecodingFlg)
		return FALSE;
#if _DEBUG
	CString strWorkPath1("c:\\bimap1.dat");
#else
	TCHAR szFullPath[_MAX_PATH];
  	TCHAR szDir[_MAX_DIR];
  	TCHAR szDrive[_MAX_DRIVE];
 	CString strWorkPath1;
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
	strWorkPath1 += "HidIc.chv";
#endif	
	CFile file;
	//20100406新增CFile::shareExclusive属性，避免其他CAD文件写自动绘图目录时写入不成功或打印缺页
	if(!file.Open(strWorkPath1,CFile::modeRead | CFile::shareExclusive))
		return 1;

#if _DEBUG
	CString strWorkPath2 = "c:\\bitdataCh1.dat";

	CFile hFile;
	if(!hFile.Open(strWorkPath2,CFile::modeCreate | CFile::modeWrite))
	{
	 return FALSE;
	}
#endif
	InitHPGL();
	InitTextChangeLine();
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
	nProcess=PROCESS_MNEMONIC;

	nLength=file.GetLength();
	j=k=m=n=i=0;

	bEncrypt = TRUE;
	if(nLength>1024)
		iPackLen = 1024;
	else
		iPackLen = nLength;
	while(i<nLength)
	{
		k = 0;
		if (m < j)
		{
			while(k<(j-m))
			{
				cFileBuf[k] = cFileBuf[m+k];
				k++;
			}
			j = iPackLen - k;
		}
		else
			j = iPackLen;

		i += j;
		if(i>nLength)
			j -= (i-nLength);

		file.Read(&cFileBuf[k],j);

		j += k;
		k = 0;
		m = j;
		while (k < j)
		{
			if (nProcess == PROCESS_MNEMONIC)//助记符
			{
				if(k==0x9b)
					k=k;
				ch = cFileBuf[k++];
				switch (ch)
				{
				case DEFCMD_IN://0x8a://IN
				case DEFCMD_PG://0x97://PG
					if (cParameterType == DEFCMD_LPU)//0x5e
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					else if (cParameterType == DEFCMD_LPD)//0x3b
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					if (ch == DEFCMD_IN)//0x8a
					{
						Instruct.m_cName[0]='I';
						Instruct.m_cName[1]='N';
					}
					else
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='G';
					}
					ProcessInstruct(&Instruct);
					break;
				case DEFCMD_SP://0x7c:
				case DEFCMD_LT://0xe6:
				case DEFCMD_PU://0xb5:
				case DEFCMD_PD://0x3d:
					if (cParameterType == DEFCMD_LPU)//0x5e
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					else if (cParameterType == DEFCMD_LPD)//0x3b
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;
				case DEFCMD_LPU://0x5e:
					if (cParameterType == DEFCMD_LPD)//0x3b)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;
				case DEFCMD_LPD://0x3b:
					if (cParameterType == DEFCMD_LPU)//0x5e)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;

				case DEFCMD_DI://0x5b:
					if (cParameterType == DEFCMD_DI)//0x5e)
					{
						Instruct.m_cName[0]='D';
						Instruct.m_cName[1]='I';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;

				case DEFCMD_SI://0x5d:
					if (cParameterType == DEFCMD_SI)//0x5e)
					{
						Instruct.m_cName[0]='S';
						Instruct.m_cName[1]='I';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;

				case DEFCMD_FN://0x4b:
					if (cParameterType == DEFCMD_FN)//0x5e)
					{
						Instruct.m_cName[0]='F';
						Instruct.m_cName[1]='N';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;

				case DEFCMD_LB://0x9a:
					if (cParameterType == DEFCMD_LB)//0x5e)
					{
						Instruct.m_cName[0]='L';
						Instruct.m_cName[1]='B';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;
				
				case DEFCMD_BT://0x9a:
					if (cParameterType == DEFCMD_BT)//0x5e)
					{/*
						Instruct.m_cName[0]='B';
						Instruct.m_cName[1]='T';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();*/
					}
					nProcess=PROCESS_PARAMETER;
					break;
				
				case 0x00:
					if (cParameterType == DEFCMD_LPU)//0x5e)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					else if (cParameterType == DEFCMD_LPD)//0x3b)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					break;
				default:
					bError=TRUE;
					break;
				}
				cParameterType = ch;
			}
			else
			{//
				switch (cParameterType)
				{
				case DEFCMD_SP://0x7c:
					if ((j - k) < 1)
					{
						m = k;
						k = j;
					}
					else
					{
					ch = cFileBuf[k++];
					Instruct.m_cName[0]='S';
					Instruct.m_cName[1]='P';
					Instruct.m_adPara.Add((double)ch);
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					}
					break;
				case DEFCMD_LT://0xe6:
					if ((j - k) < 8)
					{
						m = k;
						k = j;
					}
					else
					{
					ch = cFileBuf[k++];
					Instruct.m_cName[0]='L';
					Instruct.m_cName[1]='T';
					ch ^= 0xce;
					if (ch != 0)
					{
						if (ch == 1)
							ch = 3;
						else if (ch == 2)
							ch = 1;
						else
							ch = 8;
						Instruct.m_adPara.Add((double)ch);
					}
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					}
					break;
					
				case DEFCMD_DI://0x5b:
					if ((j - k) < 4)
					{
						m = k;
						k = j;
					}
					else
					{
					Instruct.m_cName[0]='D';
					Instruct.m_cName[1]='I';
					//角度
					cPin = (unsigned char *)&nParameter;
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
					Instruct.m_adPara.Add(nParameter);
					// 是否是斜体
					Instruct.m_cIsItali = cFileBuf[k++];
					//Instruct.m_adPara.Add(cFileBuf[k++]);
					// 字体磅数
					cPin = (unsigned char *)&nParameter;
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
					Instruct.m_adPara.Add(nParameter);
					
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					}
					break;

				case DEFCMD_SI://0x5d:
					if ((j - k) < 16)
					{
						m = k;
						k = j;
					}
					else
					{
					Instruct.m_cName[0]='S';
					Instruct.m_cName[1]='I';
					cPin = (unsigned char *)&dParm;
					for(dloop=0; dloop<2; dloop++)
					{
						for(dCount=7; dCount>=0; dCount--)
						{
							*(cPin+dCount) = cFileBuf[k++];
						}
						Instruct.m_adPara.Add(dParm);
					}
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					}
					break;

				case DEFCMD_FN://0x4b:
					if ((j - k) < 4)
					{
						m = k;
						k = j;
					}
					else
					{
					Instruct.m_cName[0]='F';
					Instruct.m_cName[1]='N';
					cPin = (unsigned char *)&dloop;
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
					if ((j - k) < dloop)
					{
						m = k;
						k = j;
					}
					else
					{
					pStr=Instruct.m_cFontName.GetBuffer(dloop);
					for(dCount=0; dCount<dloop; dCount++)
					{
						*pStr++ = cFileBuf[k++];
					}
					Instruct.m_cString.ReleaseBuffer();  //更新对象内部数据
					
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					}
					}
					break;

				case DEFCMD_LB://0x4b:
					if ((j - k) < 12)
					{
						m = k;
						k = j;
					}
					else
					{
					Instruct.m_cName[0]='L';
					Instruct.m_cName[1]='B';
					
					//字符串个数
					cPin = (unsigned char *)&dloop;
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];

					//字符串左上角X坐标
					cPin = (unsigned char *)&nParameter;
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
					Instruct.m_adPara.Add(nParameter);
					//字符串左上角Y坐标
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
					Instruct.m_adPara.Add(nParameter);
					//字符串
					if ((j - k) < dloop)
					{
						m = k;
						k = j;
					}
					else
					{
					pStr=Instruct.m_cString.GetBuffer(dloop);
					for(dCount=0; dCount<dloop; dCount++)
					{
						*pStr++ = cFileBuf[k++];
					}
					Instruct.m_cString.ReleaseBuffer();  //更新对象内部数据
					g_strText.Empty();
					g_strText=Instruct.m_cString;
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					Instruct.m_cString.Empty();
					nProcess=PROCESS_MNEMONIC;
					}
					}
					break;	

				case DEFCMD_BT://0x9a:
					if ((j - k) < 56)
					{
						m = k;
						k = j;
					}
					else
					{
					pBitPtr = new BitPrimitive;	
					g_listBitDataPrimitive.AddTail(pBitPtr);		
					//位图的左上角在整个图中的X位置 2004
					//位图的左上角在整个图中的Y位置	6004			
					//位图的左下角在整个图中的X位置	2004
					//位图的左下角在整个图中的Y位置	2004
					//位图的右上角在整个图中的X位置 6004
					//位图的右上角在整个图中的Y位置
					//位图的右下角在整个图中的X位置
					//位图的右下角在整个图中的Y位置
					for(dCount=0; dCount<8; dCount++)
					{
						cPin = (unsigned char *)&nParameter;
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						//Instruct.m_BitPost[dCount] = nParameter;
						pBitPtr->m_BitPost[dCount] = nParameter;
					}
					
					//为1时则为透明显示; 为0则为非透明显示
					//背景颜色(在透明显示时背景颜色的点将不被显示)
					for(dCount=0; dCount<2; dCount++)
					{
						cPin = (unsigned char *)&nParameter;
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						//Instruct.m_BitDispStyle[dCount] = nParameter;
						pBitPtr->m_BitDispStyle[dCount] = nParameter;
					}
					
					//GDI+中BitmapData的Width,单位：像素
					//GDI+中BitmapData的Height,单位：像素
					//GDI+中BitmapData的Stride(每行的字节数)
					//GDI+中BitmapData的PixelFormat
					for(dCount=0; dCount<4; dCount++)
					{
						cPin = (unsigned char *)&nParameter;
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						pBitPtr->m_BitGDI[dCount] = nParameter;
					}

					// new 20140416 使用语言格式V101
					cPin = (unsigned char *)&nParameter;
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
					dloop = nParameter;
					//************************************

					//位图数据 (abs(Height*Stride)个字节)
 					//dloop=abs(pBitPtr->m_BitGDI[1]*pBitPtr->m_BitGDI[2]);
					BYTE *pBmpData1;
					pBmpData1 = new BYTE[dloop];
					for(dCount=0; dCount<dloop; dCount++)
					{
						pBmpData1[dCount] = cFileBuf[k++];
						if((j - k)==0)
						{
							if((i+1024) < nLength)
							{
								j = 1024;
								file.Read(&cFileBuf[0],j);
								i += 1024;
							}
							else{
								j = (nLength-i);
								file.Read(&cFileBuf[0],j);
								i = nLength;
							}
							k = 0;
						}
					}
#if _DEBUG
					hFile.Write(pBmpData1, dloop);
					//hFile.Close();
#endif
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
													
					CPoint  cPostLU,cPostLD, cPostRU, cPostRD, ptLUNEW;
					int tmpPost;
					if (g_nRotate==1)
					{//旋转
						cPostLU.x = g_lInitMaxPlotLength - ftoi(pBitPtr->m_BitPost[0]*g_dRatioY)+g_dpOrigin.y+g_lMinPlotWidth;
						cPostLU.x = ftoi((cPostLU.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2);
						cPostLU.y = ftoi(pBitPtr->m_BitPost[1] * g_dRatioX) + g_dpOrigin.x + g_lMaxPGLength;
						cPostLU.y = ftoi(cPostLU.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
						
						cPostLD.x = g_lInitMaxPlotLength - ftoi(pBitPtr->m_BitPost[2]*g_dRatioY)+g_dpOrigin.y+g_lMinPlotWidth;
						cPostLD.x = ftoi((cPostLD.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2);
						cPostLD.y = ftoi(pBitPtr->m_BitPost[3] * g_dRatioX) + g_dpOrigin.x + g_lMaxPGLength;
						cPostLD.y = ftoi(cPostLD.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
							
						cPostRU.x = g_lInitMaxPlotLength - ftoi(pBitPtr->m_BitPost[4]*g_dRatioY)+g_dpOrigin.y+g_lMinPlotWidth;
						cPostRU.x = ftoi((cPostRU.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2);
						cPostRU.y = ftoi(pBitPtr->m_BitPost[5] * g_dRatioX) + g_dpOrigin.x + g_lMaxPGLength;
						cPostRU.y = ftoi(cPostRU.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
							
						cPostRD.x = g_lInitMaxPlotLength - ftoi(pBitPtr->m_BitPost[6]*g_dRatioY)+g_dpOrigin.y+g_lMinPlotWidth;
						cPostRD.x = ftoi((cPostRD.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2);
						cPostRD.y = ftoi(pBitPtr->m_BitPost[7] * g_dRatioX) + g_dpOrigin.x + g_lMaxPGLength;
						cPostRD.y = ftoi(cPostRD.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2);

						tmpPost = pBitPtr->m_BitGDI[0];
						//pBitPtr->m_BitGDI[0] = (int)(((pBitPtr->m_BitGDI[1]/PixelHpglY)+1)*HpglPixelX);
						//pBitPtr->m_BitGDI[1] = (int)(((tmpPost/PixelHpglY)+1)*HpglPixelY);
					}
					else{
							cPostLU.x = ftoi(pBitPtr->m_BitPost[0]*g_dRatioX)+g_dpOrigin.x + g_lMaxPlotLength;
							cPostLU.x = ftoi((cPostLU.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2);
							cPostLU.y = ftoi(pBitPtr->m_BitPost[1]*g_dRatioY)+g_dpOrigin.y - g_lMinPlotWidth;
							cPostLU.y = ftoi(cPostLU.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
							
							cPostLD.x = ftoi(pBitPtr->m_BitPost[2]*g_dRatioX)+g_dpOrigin.x + g_lMaxPlotLength;
							cPostLD.x = ftoi((cPostLD.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2);
							cPostLD.y = ftoi(pBitPtr->m_BitPost[3]*g_dRatioY)+g_dpOrigin.y - g_lMinPlotWidth;;
							cPostLD.y = ftoi(cPostLD.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
							
							cPostRU.x = ftoi(pBitPtr->m_BitPost[4]*g_dRatioX)+g_dpOrigin.x + g_lMaxPlotLength;
							cPostRU.x = ftoi((cPostRU.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2);
							cPostRU.y = ftoi(pBitPtr->m_BitPost[5]*g_dRatioY)+g_dpOrigin.y - g_lMinPlotWidth;;
							cPostRU.y = ftoi(cPostRU.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
							
							cPostRD.x = ftoi(pBitPtr->m_BitPost[6]*g_dRatioX)+g_dpOrigin.x + g_lMaxPlotLength;
							cPostRD.x = ftoi((cPostRD.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2);
							cPostRD.y = ftoi(pBitPtr->m_BitPost[7]*g_dRatioY)+g_dpOrigin.y - g_lMinPlotWidth;;
							cPostRD.y = ftoi(cPostRD.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2);

							//pBitPtr->m_BitGDI[0] = (int)(((pBitPtr->m_BitGDI[0]/PixelHpglY)+1)*HpglPixelX);
							//pBitPtr->m_BitGDI[1] = (int)(((pBitPtr->m_BitGDI[1]/PixelHpglY)+1)*HpglPixelY);
						}
					
						
					pBitList = new BitList;
					g_listBitListPrimitive.AddTail(pBitList);
				
					pBitList->m_bTransparent = pBitPtr->m_BitDispStyle[0];
					//pBitList->pBitmap = Create1BPPBitmap(cPostLU,cPostLD,cPostRU,cPostRD,pBitPtr->m_BitDispStyle[0],pBitPtr->m_BitDispStyle[1],
					//pBitPtr->m_BitGDI[0],pBitPtr->m_BitGDI[1],pBitPtr->m_BitGDI[2],pBitPtr->m_BitGDI[3], pBmpData1, pBitList->pBtPoint,iWidth, iHeight);
					//pBitList->m_wi=iWidth;
					//pBitList->m_hi=iHeight;
					// new 20140416 绘图语言V101 压缩解压缩
					char *pUnCompressData;
					int iUnCompressCount;
					UnCompressMemory((char *)pBmpData1, dloop, pUnCompressData, iUnCompressCount);
					//*************************
					pBitList->pBitmap = Create1BPPBitmap(pBitPtr->m_BitDispStyle[0],pBitPtr->m_BitDispStyle[1],pBitPtr->m_BitGDI[0],pBitPtr->m_BitGDI[1],
														 pBitPtr->m_BitGDI[2],pBitPtr->m_BitGDI[3], (BYTE *)pUnCompressData);
					pBitList->bPostLU=cPostLU;
					pBitList->bPostLD=cPostLD;
					pBitList->bPostRU=cPostRU;
					pBitList->bPostRD=cPostRD;

					int minX,minY,maxX,maxY;
					minX=cPostLU.x;
					minX>cPostLD.x?(maxX=minX,minX=cPostLD.x):minX>cPostRU.x?(maxX=minX,minX=cPostRU.x):minX>cPostRD.x?(maxX=minX,minX=cPostRD.x):maxX=cPostRD.x;
					minY=cPostLU.y;
					minY>cPostLD.y?(maxY=minY,minY=cPostLD.y):minY>cPostRU.y?(maxY=minY,minY=cPostRU.y):minY>cPostRD.y?(maxY=minY,minY=cPostRD.y):maxY=cPostRD.y;

					pBitList->m_nMinPDLength = minX;
					pBitList->m_nMaxPDLength = maxX;
					pBitList->m_nMinPDWidth = minY;
					pBitList->m_nMaxPDWidth = maxY;
					delete pBmpData1;
					delete pUnCompressData;
					}
					break;
					
				case DEFCMD_PU://0xb5:
				case DEFCMD_PD://0x3d:
					if ((j - k) < 8)
					{
						m = k;
						k = j;
					}
					else
					{
						if (cParameterType == DEFCMD_PU)//0xb5)
						{
							Instruct.m_cName[0]='P';
							Instruct.m_cName[1]='U';
						}
						else
						{
							Instruct.m_cName[0]='P';
							Instruct.m_cName[1]='D';
						}
						cPin = (unsigned char *)&nParameter;
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
						nProcess=PROCESS_MNEMONIC;
					}
					break;
				case DEFCMD_LPU://0x5e:
				case DEFCMD_LPD://0x3b:
					if ((j - k) < 8)
					{
						m = k;
						k = j;
					}
					else
					{
						cPin = (unsigned char *)&nParameter;
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						nProcess=PROCESS_MNEMONIC;
					}
					break;
				default:
					break;
				}
			}
		}
		if(bError)
			break;
	}
	if(!bError)
	{
		if (cParameterType == DEFCMD_LPU)//0x5e)
		{
			Instruct.m_cName[0]='P';
			Instruct.m_cName[1]='U';
			ProcessInstruct(&Instruct);
			Instruct.m_adPara.RemoveAll();
		}
		else if (cParameterType == DEFCMD_LPD)//0x3b)
		{
			Instruct.m_cName[0]='P';
			Instruct.m_cName[1]='D';
			ProcessInstruct(&Instruct);
			Instruct.m_adPara.RemoveAll();
		}
	}
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
#ifdef	_DEGBUG
	hFile.Close();
#endif
	file.Close();

#ifndef _DEGUG
	if(!file.Open(strWorkPath1,CFile::modeCreate | CFile::modeWrite))
	{
	 return FALSE;
	}
	file.Close();
#endif
	if(bError)
		return 2;
	if(g_nRotate==1)
	{
		g_lMaxPlotLength=g_lMaxPGWidth;
		if (g_lMaxPlotWidth < g_lMaxPGLength)
			g_lMaxPlotWidth = g_lMaxPGLength;
	}
	else
	{
		g_lMaxPlotLength=g_lMaxPGLength;
		if (g_lMaxPlotWidth < g_lMaxPGWidth)
			g_lMaxPlotWidth = g_lMaxPGWidth;
	}
	if((g_lMaxPlotLength<=0)||(g_lMaxPlotWidth<=0))
		return 4;
	else
		return 0;
}

//返回0打开文件成功,1打开文件失败,2文件格式错,3图形超宽
int CreateList(CPlotItem *pPlotItem,int nCommunicateMode,SOCKET sock)
{
	CFile file;
	CString strFileName;
	
	strFileName=pPlotItem->m_strPlotName;
	//20100406新增CFile::shareExclusive属性，避免其他CAD文件写自动绘图目录时写入不成功或打印缺页
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return 1;
	g_nRotate=pPlotItem->m_nRotate;
	InitTextChangeLine();
	InitHPGL();
	g_lFixedPointLength=pPlotItem->m_lFixedPointLength*40;
	g_lInitMaxPlotLength=pPlotItem->m_lMaxPlotLength;
	g_lInitMaxPlotWidth=pPlotItem->m_lMaxPlotWidth;
	g_lMaxPlotLength-=pPlotItem->m_lMinPlotLength;
	g_lMinPlotWidth=pPlotItem->m_lMinPlotWidth;

	int nLength=file.GetLength();
	int nReturnValue;

	if (nLength >= 22)
	{
		char cFileBuf[23];
		file.Read(&cFileBuf[0],22);
		file.Close();
		if ((cFileBuf[0] == 0x66) && (cFileBuf[1] == 0x53) &&
			(strncmp(&cFileBuf[3],"Richpeace plot file",19) == 0))
			//nReturnValue = SecretFileList(strFileName);//加密文件
			nReturnValue = SecretFileNew(strFileName);
		else
			nReturnValue = DissecretFileList(strFileName);
	}
	else
	{
		file.Close();
		nReturnValue = DissecretFileList(strFileName);

	}

	int listlen;
	listlen = ptArray.GetSize();
	listlen = g_listPrimitive.GetCount();
	if (nReturnValue != 0)
		return nReturnValue;

	//HeapSort();
	
	long lMaxPlotLength;
	long lMaxWidthLength;
	if(g_nRotate==1)
	{//图形正向旋转90度
		lMaxWidthLength=g_lMaxPlotLength;
		g_lMaxPlotLength=g_lMaxPlotWidth;
		g_lMaxPlotWidth=lMaxWidthLength;
	}
	//20100304 解决叙利亚的MJ喷墨机问题有些文件能打有些文件不能打，由于图形中文字
	//矢量化时超出绘图仪打印宽度设置的范围
	//if ((g_lMaxPlotWidth > pPlotItem->m_lMaxPlotWidth) || (g_lMaxPlotLength > pPlotItem->m_lMaxPlotLength))
		//return 4;

	if (g_lMaxPlotWidth <= pPlotItem->m_lPaperWidth)
	{
		g_nSplitNum = 1;
		lMaxWidthLength = ftoi(g_lMaxPlotWidth * 0.025);
		lMaxPlotLength = ftoi(g_lMaxPlotLength * 0.025);
		if (lMaxPlotLength != pPlotItem->m_lPictureLength)
			pPlotItem->m_lPictureLength = lMaxPlotLength;
		if (lMaxWidthLength != pPlotItem->m_lPictureWidth)
			pPlotItem->m_lPictureWidth = lMaxWidthLength;
		g_lMaxPlotWidth = long((g_lMaxPlotWidth * g_dScaleY * HpglPixelY + g_nLineWidth + 15) / 16);
		g_lMaxPlotLength = ftoi(g_lMaxPlotLength * g_dScaleX * HpglPixelX + g_nLineWidth) - ftoi(pPlotItem->m_lFixedPointLength * g_dScaleX * MMPixelX);
	}
	else
	{//纸张宽度小于文件宽度，图形分页
		lMaxWidthLength = (g_lMaxPlotWidth + pPlotItem->m_lPaperWidth - 1) / pPlotItem->m_lPaperWidth;
		g_nSplitNum = (int)lMaxWidthLength;
		lMaxWidthLength = ftoi(pPlotItem->m_lPaperWidth * 0.025);
		lMaxPlotLength = ftoi((g_lMaxPlotLength * 0.025 + 10) * g_nSplitNum) - 10;
		if (lMaxPlotLength != pPlotItem->m_lPictureLength)
			pPlotItem->m_lPictureLength = lMaxPlotLength;
		if (lMaxWidthLength != pPlotItem->m_lPictureWidth)
			pPlotItem->m_lPictureWidth = lMaxWidthLength;
		g_lMaxPlotWidth = long((pPlotItem->m_lPaperWidth * g_dScaleY * HpglPixelY + 15) / 16);
		g_lMaxPlotLength = ftoi((g_lMaxPlotLength + 10 * 40 - pPlotItem->m_lFixedPointLength)* g_dScaleX * HpglPixelX);
	}
	g_lMaxPlotWidth *= 2;

	//发送图形宽度（字节数）
	char SendBuf[100];
	BYTE *pBits1;
	unsigned long mLength;

	lMaxWidthLength = g_lMaxPlotWidth + long((pPlotItem->m_dGapRight * MMPixelY + 7) / 8);
	while (1)
	{
		strcpy(SendBuf,"Msg_Comm");//发送文件开始命令（文件宽度）
		SendBuf[8] = 0;
		SendBuf[9] = 1;
		pBits1 = (BYTE *)&lMaxWidthLength;
		SendBuf[10] = *(pBits1+3);
		SendBuf[11] = *(pBits1+2);
		SendBuf[12] = *(pBits1+1);
		SendBuf[13] = *pBits1;
		if (nCommunicateMode == 0)
		{
			mLength = 14;
			if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // 发送成功
			{
				mLength = 64;
				CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
			}
		}
		else
		{
			send(sock,SendBuf,14,0);
			recv(sock,SendBuf,100,0);
		}
		if (memcmp(SendBuf,"Msg_",4) == 0)
		{
			if ((SendBuf[5] == 1) && (SendBuf[7] == 3))
				//20150316rwt******start***
				//return 0;
				//20150316rwt******end***
				return 3;//date before;
			else if ((SendBuf[5] == 1) && (SendBuf[7] == 9))
				break;
		}
		Sleep(10);
	}
	return 0;
}

#define LENGTH5M   5000000L
int  FileLenExceed5MFlag = 0;
//非加密文件链表生成
int DissecretFileList(CString strFileName)
{
	CFile file;
	//20100406新增CFile::shareExclusive属性，避免其他CAD文件写自动绘图目录时写入不成功或打印缺页
	if(!file.Open(strFileName,CFile::modeRead |CFile::shareExclusive ))
		return 1;

	int nLength=file.GetLength();
	//20100825 文件大于5M不进行链表遍历查找最小插入点
	if(nLength > LENGTH5M)
		FileLenExceed5MFlag = 1;
	
	BOOL bError=FALSE;
	CInstruct Instruct;
	int nProcess;
	int nMnemonicCount;
	CString strParameter;
	int nParameterType=NONE_TYPE;
	int nEscCount;
	char cFileBuf[1001];
	int i,j,k,m;
	unsigned char ch;
    bEncrypt = FALSE;
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
	nProcess=PROCESS_MNEMONIC;
	nMnemonicCount=0;
	
	m=j=0;
	for(i=0;i<nLength;)
	{
		if (m < j)
		{
			k=0;
			while(k<(j-m))
			{
				cFileBuf[k] = cFileBuf[m+k];
				k++;
			}
			if ((i + 1000 - k) < nLength)
				j = 1000 - k;
			else
				j = nLength - i;
		}
		else
		{
			if ((i + 1000) < nLength)
				j = 1000;
			else
				j = nLength - i;
			k = 0;
		}
		file.Read(&cFileBuf[k],j);
		i += j;
		j += k;
		k = 0;
		m = j;
		while (k < j)
		{
			if (nProcess == PROCESS_FIND)
			{//寻找下一个';'
				while (k < j)
				{
					ch = cFileBuf[k++];
					if(ch==';')
					{
						nProcess=PROCESS_MNEMONIC;
						break;
					}
				}
			}
			else if (nProcess == PROCESS_MNEMONIC)//助记符
			{
				ch = cFileBuf[k++];
				if (nMnemonicCount == 0)
				{
					if(IsAlpha(ch) || (ch =='!'))
						Instruct.m_cName[nMnemonicCount++]=toupper(ch);
					else if(ch==27)
					{
						nProcess=PROCESS_PARAMETER;
						nParameterType=ESC_TYPE;
						nEscCount=4;
					}
				}
				else
				{
					if(IsAlpha(ch) || (ch =='!'))
					{
						Instruct.m_cName[nMnemonicCount++]=toupper(ch);
						nProcess=PROCESS_PARAMETER;
						nParameterType=GetParameterType(Instruct.m_cName);
						strParameter.Empty();
						if(nParameterType==PE_TYPE)
						{
							nProcess=PROCESS_FIND;
							nMnemonicCount=0;
						}
					}
					else
						nMnemonicCount=0;
				}
			}
			else
			{//参数
				switch(nParameterType)
				{
					case ESC_TYPE:
						if ((k+nEscCount) <= j)
						{
							k+=nEscCount;
							nProcess=PROCESS_MNEMONIC;
						}
						else
						{
							nEscCount-=(j-k);
							k=j;
						}
						break;
					case NONE_TYPE://无参数
						ch = cFileBuf[k++];
						if(ch==';')
						{
							ProcessInstruct(&Instruct);
							nProcess=PROCESS_MNEMONIC;
							nMnemonicCount=0;
						}
						else if(IsAlpha(ch))
						{
							ProcessInstruct(&Instruct);
							nProcess=PROCESS_MNEMONIC;
							nMnemonicCount=0;
							Instruct.m_cName[nMnemonicCount++]=toupper(ch);
						}
						else if(IsDigit(ch))
							bError=TRUE;
						break;
					case DIGIT_TYPE://数字
						m=k;
						while (k < j)
						{
							ch = cFileBuf[k++];
							if (!IsDigit(ch))
							{
								if (ch=='.')
									continue;
								if (m != (k-1))
									Instruct.m_adPara.Add(atof(&cFileBuf[m]));
								if (ch==';')
								{
									ProcessInstruct(&Instruct);
									Instruct.m_adPara.RemoveAll();
									nProcess=PROCESS_MNEMONIC;
									nMnemonicCount=0;
									m = j;
									break;
								}
								else if (IsAlpha(ch))
								{
									ProcessInstruct(&Instruct);
									Instruct.m_adPara.RemoveAll();
									nMnemonicCount=0;
									nProcess=PROCESS_MNEMONIC;
									Instruct.m_cName[nMnemonicCount++]=toupper(ch);
									m = j;
									break;
								}
								else if(ch==27)
								{
									ProcessInstruct(&Instruct);
									Instruct.m_adPara.RemoveAll();
									nMnemonicCount=0;
									nProcess=PROCESS_PARAMETER;
									nParameterType=ESC_TYPE;
									nEscCount=4;
									m = j;
									break;
								}
								else
									m=k;
							}
						}
						break;
					case STRING_TYPE:
						ch = cFileBuf[k++];
						if(ch==g_cTerminator)
						{//文字终止符
							//add by yhz,如果是连续的LB指令，则将其合并
							if (('L' == cFileBuf[k]) && ('B' == cFileBuf[k+1]))
							{
								ch=cFileBuf[k+2];
								k+=2;
							}
							//add by yhz,如果是连续的LB指令，则将其合并
							else
							{
								if(g_cTerminatorMode==0)
								{
									CString str="1";
									str.SetAt(0,ch);
									g_strText+=str;
								}
								ProcessInstruct(&Instruct);
								Instruct.m_adPara.RemoveAll();
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
							}
						}
						else
						{
							CString str="1";
							str.SetAt(0,ch);
							g_strText+=str;
						}
						break;
					case DT_TYPE:
						ch = cFileBuf[k++];
						if(g_cTerminatorMode==0x81)
						{
							if(ch==',')
								g_cTerminatorMode=0x82;
							else
							{
								g_cTerminatorMode=1;
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
								k--;
							}
						}
						else if(g_cTerminatorMode==0x82)
						{
							if(ch==0 || ch==1)
								g_cTerminatorMode=ch;
							else
							{
								g_cTerminatorMode=1;
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
								k--;
							}
						}
						else
						{
							if(ch==';')
							{
								g_cTerminator=theApp.m_cTerminator;
								g_cTerminatorMode=1;
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
							}
							else if(ch!=0 && ch!=5 && ch!=27)
							{
								g_cTerminator=ch;
								g_cTerminatorMode=0x81;
							}
							else
							{
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
							}
						}
						break;
				}
			}
			if(bError)
				break;
		}
		if(bError)
			break;
	}
	if(!bError)
		ProcessInstruct(&Instruct);
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
	file.Close();
	FileLenExceed5MFlag = 0;
	if(bError)
		return 2;
	if(g_nRotate==1)
	{//图形正向旋转90度
		g_lMaxPlotLength=g_lMaxPGWidth;
		if (g_lMaxPlotWidth < g_lMaxPGLength)
			g_lMaxPlotWidth = g_lMaxPGLength;
	}
	else
	{
		g_lMaxPlotLength=g_lMaxPGLength;
		if (g_lMaxPlotWidth < g_lMaxPGWidth)
			g_lMaxPlotWidth = g_lMaxPGWidth;
	}
	if((g_lMaxPlotLength<=0)||(g_lMaxPlotWidth<=0))
		return 4;
	else
		return 0;
}



//加密文件链表生成
int SecretFileList(CString strFileName)
{
	CFile file;
	//20100406新增CFile::shareExclusive属性，避免其他CAD文件写自动绘图目录时写入不成功或打印缺页
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return 1;

	int nLength=file.GetLength();
	BOOL bError=FALSE;
	CInstruct Instruct;
	int nProcess;
	unsigned char cParameterType;
	char cFileBuf[1025];
	char cSecretBuf[256];
	int i,j,k,m,n;
	int nRemainChar;
	unsigned char ch,*cPin;
	int nParameter;
	
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
	nProcess=PROCESS_MNEMONIC;
	m=j=0;
	nRemainChar = 0;
	//判断文件是否为加密文件
	file.Read(&cFileBuf[0],22);
	i = 22;
	for(;i<nLength;)
	{
		k = 0;
		if (m < j)
		{
			while(k<(j-m))
			{
				cFileBuf[k] = cFileBuf[m+k];
				k++;
			}
			j = 1024 - k;
		}
		else
			j = 1024;
		if (nRemainChar > 0)
		{
			n = 252 - nRemainChar + 2;
			if (j > nRemainChar)
			{
				m = 254;
				j -= nRemainChar;
				i += nRemainChar;
			}
			else
			{
				m = n + j;
				i += j;
				j = 0;
			}
			if ((n < m) && ((nRemainChar & 0x01) != 0))
			{
				nRemainChar -= (m - n);
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
			}
			else
				nRemainChar -= (m - n);
			for (;n<m;)
			{
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
			}	
		}
		while (j > 0)
		{
			if (nLength == i)
				break;
			if ((nLength - i) < 256)
			{
				bError = TRUE;
				break;
			}
			file.Read(cSecretBuf,256);
			i += 256;
			cSecretBuf[0] ^= cSecretBuf[254];
			cSecretBuf[1] ^= cSecretBuf[255];
			cSecretBuf[0] ^= 0xac;
			cSecretBuf[1] ^= 0xe3;
			if (j < 252)
			{
				m = j + 2;
				nRemainChar = 252 - j;
				j = 0;
				i -= nRemainChar;
			}
			else
			{
				m = 254;
				j -= 252;
				nRemainChar = 0;
			}
			if ((m & 0x01) != 0)
			{
				m--;
				for (n=2;n<m;)
				{
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
				}
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
			}
			else
			{
				for (n=2;n<m;)
				{
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
				}
			}
		}
		j = k;
		k = 0;
		m = j;
		if (bError)
			break;
		while (k < j)
		{
			if (nProcess == PROCESS_MNEMONIC)//助记符
			{
				ch = cFileBuf[k++];
				switch (ch)
				{
				case 0x8a://IN
				case 0x97://PG
					if (cParameterType == 0x5e)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					else if (cParameterType == 0x3b)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					if (ch == 0x8a)
					{
						Instruct.m_cName[0]='I';
						Instruct.m_cName[1]='N';
					}
					else
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='G';
					}
					ProcessInstruct(&Instruct);
					break;
				case 0x7c:
				case 0xe6:
				case 0xb5:
				case 0x3d:
					if (cParameterType == 0x5e)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					else if (cParameterType == 0x3b)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;
				case 0x5e:
					if (cParameterType == 0x3b)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;
				case 0x3b:
					if (cParameterType == 0x5e)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;
				case 0x00:
					if (cParameterType == 0x5e)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					else if (cParameterType == 0x3b)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					break;
				default:
					bError=TRUE;
					break;
				}
				cParameterType = ch;
			}
			else
			{
				switch (cParameterType)
				{
				case 0x7c:
					ch = cFileBuf[k++];
					Instruct.m_cName[0]='S';
					Instruct.m_cName[1]='P';
					Instruct.m_adPara.Add((double)ch);
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					break;
				case 0xe6:
					ch = cFileBuf[k++];
					Instruct.m_cName[0]='L';
					Instruct.m_cName[1]='T';
					ch ^= 0xce;
					if (ch != 0)
					{
						if (ch == 1)
							ch = 3;
						else if (ch == 2)
							ch = 1;
						else
							ch = 8;
						Instruct.m_adPara.Add((double)ch);
					}
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					break;
				case 0xb5:
				case 0x3d:
					if ((j - k) < 8)
					{
						m = k;
						k = j;
					}
					else
					{
						if (cParameterType == 0xb5)
						{
							Instruct.m_cName[0]='P';
							Instruct.m_cName[1]='U';
						}
						else
						{
							Instruct.m_cName[0]='P';
							Instruct.m_cName[1]='D';
						}
						cPin = (unsigned char *)&nParameter;
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
						nProcess=PROCESS_MNEMONIC;
					}
					break;
				case 0x5e:
				case 0x3b:
					if ((j - k) < 8)
					{
						m = k;
						k = j;
					}
					else
					{
						cPin = (unsigned char *)&nParameter;
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						nProcess=PROCESS_MNEMONIC;
					}
					break;
				default:
					break;
				}
			}
		}
		if(bError)
			break;
	}
	if(!bError)
	{
		if (cParameterType == 0x5e)
		{
			Instruct.m_cName[0]='P';
			Instruct.m_cName[1]='U';
			ProcessInstruct(&Instruct);
			Instruct.m_adPara.RemoveAll();
		}
		else if (cParameterType == 0x3b)
		{
			Instruct.m_cName[0]='P';
			Instruct.m_cName[1]='D';
			ProcessInstruct(&Instruct);
			Instruct.m_adPara.RemoveAll();
		}
	}
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
	file.Close();
	if(bError)
		return 2;
	if(g_nRotate==1)
	{
		g_lMaxPlotLength=g_lMaxPGWidth;
		if (g_lMaxPlotWidth < g_lMaxPGLength)
			g_lMaxPlotWidth = g_lMaxPGLength;
	}
	else
	{
		g_lMaxPlotLength=g_lMaxPGLength;
		if (g_lMaxPlotWidth < g_lMaxPGWidth)
			g_lMaxPlotWidth = g_lMaxPGWidth;
	}
	if((g_lMaxPlotLength<=0)||(g_lMaxPlotWidth<=0))
		return 4;
	else
		return 0;
}

//生成位图
UINT BmpGenerationThreadProc(LPVOID pParam)
{
		CPlotItem *pPlotItem;
  
	pPlotItem=((SendDataThreadInfo*)pParam)->m_pPlotItem;

	long iIndex1;
	long lSendPage;//已生成位图个数
	long lTotalPage;//图形位图总数
//	HDC hdc,hdcMem;
//	HBITMAP hBitMap = NULL;
//	CBitmap pBitMap;
//	CBitmap *pOldBitmap1;
//	CDC *pMemDC = NULL;
	BYTE *pBits,*pBits1,*pBits2,*pBits3;//内存块指针
	BYTE *pBuf1,*pBuf2;
	long i,j,m,n;
	unsigned long *lBitsCount;//位图数据个数
	//数据转换目录表
	unsigned char pChangeValue[256] = {0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
									   0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
									   0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
									   0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
									   0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
									   0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
									   0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
									   0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
									   0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
									   0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
									   0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
									   0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
									   0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
									   0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
									   0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
									   0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff};
	CString strFileName;
	unsigned char cCounts,cValue;
	long lGrapRight;
//////////////////////////////////////////////////////
  CBitmap *pOldBitmap, bitmapDraw;
  COLORREF crBkColorOld;
  CBrush brush;
  CBrush* pBrushOld;
  CPen pen;
  CPen *pPenOld;
  CRect rectClient;
  int iWidth, iHeigth;//, buf1Length, buf2Length, cMinCount;
  BYTE *pBuffer, byteData;
  BITMAPINFOHEADER bi;  //记录图片的小大，颜色深度等
  HDC hDC;
  CDC memdc,dc;
  DWORD dwSignLineBytes;
  BITMAPINFO *pBMI;
     // GetClientRect(rectClient);
    hDC = ::GetDC(NULL);
    dc.Attach(hDC);
    memdc.CreateCompatibleDC(&dc);

//////////////////////////////////////////////////////////////
	//计算右边距
	lGrapRight = long((pPlotItem->m_dGapRight * MMPixelY + 7) / 8);
/*
	//pMemDC = GetCDC();
	hdc = CreateIC(TEXT("DISPLAY"),NULL,NULL,NULL);
	hdcMem = CreateCompatibleDC(hdc);
	pMemDC = new CDC;
	pMemDC->Attach(hdcMem);
	*/
	lTotalPage = (g_lMaxPlotLength + SECTION_WIDTH - 1) / SECTION_WIDTH;
	g_lSectionPosY = SECTION_WIDTH;
	//开辟内存块
	g_pBits1 = new BYTE[g_lMaxPlotWidth * g_lSectionPosY * 2 + lGrapRight * g_lSectionPosY];
	g_pBits2 = new BYTE[g_lMaxPlotWidth * g_lSectionPosY * 2 + lGrapRight * g_lSectionPosY];
	pBuf1 = new BYTE[g_lMaxPlotWidth * g_lSectionPosY + lGrapRight * g_lSectionPosY];
	pBuf2 = new BYTE[g_lMaxPlotWidth * g_lSectionPosY + lGrapRight * g_lSectionPosY];
	rectClient.left = 0;
	rectClient.top = 0;
	rectClient.right = g_lSectionPosY;
	rectClient.bottom = g_lMaxPlotWidth*8;
	pBMI = new BITMAPINFO[sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD)];
	bi.biSize = sizeof(BITMAPINFOHEADER);    bi.biWidth =  rectClient.Width();
	bi.biHeight =  -rectClient.Height();	 bi.biPlanes = 1;
	bi.biBitCount = 1;						 bi.biCompression = BI_RGB;  //没压缩
	bi.biSizeImage = 0;						 bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;					 bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	memcpy(pBMI,&bi,sizeof(BITMAPINFOHEADER));
	pBMI->bmiColors[0].rgbBlue = (BYTE)0;
	pBMI->bmiColors[0].rgbGreen = (BYTE)0;
	pBMI->bmiColors[0].rgbRed = (BYTE)0;
	pBMI->bmiColors[0].rgbReserved = 0;
	pBMI->bmiColors[1].rgbBlue = (BYTE)255;
	pBMI->bmiColors[1].rgbGreen = (BYTE)255;
	pBMI->bmiColors[1].rgbRed = (BYTE)255;
	pBMI->bmiColors[1].rgbReserved = 0;
	
	pBuffer = NULL;
	g_lBits1Count = 0;
	g_lBits2Count = 0;
	lGrapRight *= 8;
	
	bitmapDraw.CreateCompatibleBitmap(&dc,rectClient.Width(), rectClient.Height());
	pOldBitmap  = memdc.SelectObject(&bitmapDraw);

	for(int nSplitNum=0;nSplitNum<g_nSplitNum;nSplitNum++)
	{//图形分页数
		if ((nSplitNum != 0) && (nSplitNum == (g_nSplitNum-1)))
			lTotalPage = (g_lMaxPlotLength - ftoi(10 * 40 * g_dScaleX * HpglPixelX) + g_nLineWidth + SECTION_WIDTH - 1) / SECTION_WIDTH;
		g_iSectionCount=0;
		g_lMarkSpace=g_nLineWidth/2;
		lSendPage = 0;
		while(lSendPage < lTotalPage)
		{//图形位图个数
			m = g_lMaxPlotWidth * g_lSectionPosY;
			for (iIndex1=0;iIndex1<m;iIndex1++)//清空缓冲区
				pBuf1[iIndex1]=0;

			//pBitMap.CreateBitmap(g_lSectionPosY,g_lMaxPlotWidth*8,1,1,pBuf1);
			//pOldBitmap1=pMemDC->SelectObject(&pBitMap);
			//创建CDC和内存位图
			
			//将整个位图背景设置为白色
			memdc.SetROP2(R2_COPYPEN);
			crBkColorOld = memdc.SetBkColor(RGB(255,255,255));
			memdc.SetStretchBltMode(COLORONCOLOR);

			brush.CreateSolidBrush(RGB(255,255,255));
			brush.UnrealizeObject();
			pBrushOld = memdc.SelectObject(&brush);
			pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
			pPenOld = memdc.SelectObject(&pen);

			memdc.Rectangle(rectClient);
			memdc.SelectObject(pPenOld);
			pen.DeleteObject();
			memdc.SelectObject(pBrushOld);
			brush.DeleteObject();

			//添加自绘图形 
			DrawBMP(&memdc,nSplitNum,pPlotItem->m_lMarkSpace);

			//pBitMap.GetBitmapBits(m,pBuf1);//获取点阵
			//pMemDC->SelectObject(pOldBitmap1);
			//pBitMap.DeleteObject();
			//取位图数据
		
			iHeigth = rectClient.Height();			iWidth = rectClient.Width();
			dwSignLineBytes = iWidth / 8 + (iWidth / 8) % 2;
			if (dwSignLineBytes * 8 < (DWORD)iWidth)
			{
				dwSignLineBytes = (iWidth + 31) / 32 * 4;
			}
			pBuffer = new BYTE[dwSignLineBytes * iHeigth];
			GetDIBits(memdc.GetSafeHdc(), (HBITMAP)bitmapDraw.GetSafeHandle(), 0, (DWORD)iHeigth, (LPVOID)pBuffer, (LPBITMAPINFO)pBMI, (DWORD)DIB_RGB_COLORS);

#if _DEBUG
			WritreBMP("C:\\345.bmp", rectClient.Width(), rectClient.Height(), dwSignLineBytes, PixelFormat1bppIndexed, pBuffer);
#endif
			dwSignLineBytes = iWidth / 8;
			for (j = 0; j < iHeigth;j++)
			{
				for (i = 0; (DWORD)i < dwSignLineBytes; i++)
				{
					byteData = pBuffer[i + j * dwSignLineBytes];
					byteData = byteData ^ 0xFF;  //异或
					pBuf1[i + j * dwSignLineBytes] = byteData;
				}
			}

			delete pBuffer;
			pBuffer = NULL;

			iIndex1 = g_lMaxPlotWidth * 8;

			if (g_cWriteFile==0)
			{//点阵保存在内存块1
				while (g_bFile1OK)
				{//等待缓冲区空
					if (pPlotItem->m_nState == 3)
						break;
					else
						Sleep(10);
				}
				pBits = g_pBits1;
				lBitsCount = &g_lBits1Count;
			}
			else
			{//点阵保存在内存块2
				while (g_bFile2OK)
				{//等待缓冲区空
					if (pPlotItem->m_nState == 3)
						break;
					else
						Sleep(10);
				}
				pBits = g_pBits2;
				lBitsCount = &g_lBits2Count;
			}
			if (pPlotItem->m_nState == 3)
			{//文件被删除,退出线程
				pPlotItem->m_nState = 4;
				g_bFile1OK = true;
				g_bFile2OK = true;
				delete [] pBuf1;
				delete [] pBuf2;
				//pMemDC->DeleteDC();
			//	delete pMemDC; 
				//pMemDC = NULL;
				
				delete pBMI;
				::ReleaseDC(NULL,hDC);
				return 0;
			}
			pBits1 = pBuf1+m;
			lSendPage++;
			if (lSendPage == lTotalPage)
			{//如果最后一页，位图X方向最后空白数据不用发送
				if ((nSplitNum != 0) && (nSplitNum == (g_nSplitNum-1)))
					m = (g_lMaxPlotLength - ftoi(10 * 40 * g_dScaleX * HpglPixelX) + g_nLineWidth - (lSendPage - 1) * SECTION_WIDTH + 15) / 16;
				else
					m = (g_lMaxPlotLength - (lSendPage - 1) * SECTION_WIDTH + 15) / 16;
			}
			else
				m = g_lSectionPosY / 16;
			n = g_lSectionPosY / 8 + 1;
			pBits1-=(n-1);
			pBits2 = pBits1;
			pBits3 = pBuf2;
			for(j=0;j<m;j++)
			{
				pBits1 = pBits2;
				for(i=0;i<lGrapRight;i++)
				{//图形右边距补0
					*pBits3 = 0;
					pBits3++;
					*pBits3 = 0;
					pBits3++;
				}
				for(i=0;i<iIndex1;i++)
				{//将数据高位和低位对调
					*pBits3 = pChangeValue[*pBits1];
					pBits3++;
					pBits1++;
					*pBits3 = pChangeValue[*pBits1];
					pBits3++;
					pBits1 -= n;
				}
				pBits2 += 2;
			}
			//压缩数据包
			m = m * (iIndex1 + lGrapRight) * 2;
			pBits3 = pBuf2;
			cCounts = 0;
			cValue = *pBits3;
			*lBitsCount = 0;
			for (j=0;j<m;j++)
			{
				if (cValue == *pBits3)
				{
					cCounts++;
					pBits3++;
					if (cCounts == 0xff)
					{
						*pBits = cCounts;
						pBits++;
						*pBits = cValue;
						pBits++;
						(*lBitsCount)++;
						cCounts = 0;
						cValue = *pBits3;
					}
				}
				else
				{
					*pBits = cCounts;
					pBits++;
					*pBits = cValue;
					pBits++;
					(*lBitsCount)++;
					cCounts = 1;
					cValue = *pBits3;
					pBits3++;
				}
			}
			if (cCounts != 0)
			{
				*pBits = cCounts;
				pBits++;
				*pBits = cValue;
				pBits++;
				(*lBitsCount)++;
			}
			(*lBitsCount) *= 2;
			//保存到文件并创建位图结构
			if (g_cWriteFile==0)
			{
				g_bFile1OK=true;
				g_cWriteFile=1;
			}
			else
			{
				g_bFile2OK=true;
				g_cWriteFile=0;
			}
		}
	}

	//走马克间距
	//if (g_nVersion == 0)	//20100224 屏蔽
	{
		n = ftoi(theApp.m_dGapDown * MMPixelX);
		lTotalPage = (n + SECTION_WIDTH - 1) / SECTION_WIDTH;
		lSendPage = 0;
		iIndex1 = g_lMaxPlotWidth * 8;
		
		
		while(lSendPage < lTotalPage)
		{
			if ((lSendPage + 1) == lTotalPage)
				m = (n - lSendPage * SECTION_WIDTH + 15) / 16;
			else
				m = SECTION_WIDTH / 16;
			if (g_cWriteFile==0)
			{
				while (g_bFile1OK)
				{
					if (pPlotItem->m_nState == 3)
						break;
					else
						Sleep(10);
				}
				pBits = g_pBits1;
				lBitsCount = &g_lBits1Count;
			}
			else
			{
				while (g_bFile2OK)
				{
					if (pPlotItem->m_nState == 3)
						break;
					else
						Sleep(10);
				}
				pBits = g_pBits2;
				lBitsCount = &g_lBits2Count;
			}
			if (pPlotItem->m_nState == 3)
			{//文件被删除
				pPlotItem->m_nState = 4;
				g_bFile1OK = true;
				g_bFile2OK = true;
				delete [] pBuf1;
				delete [] pBuf2;
			//	pMemDC->DeleteDC();
			//	delete pMemDC; 
			//	pMemDC = NULL;
				delete pBMI;
				::ReleaseDC(NULL,hDC);
				return 0;
			}
			//压缩数据包
			m = m * (iIndex1 + lGrapRight) * 2;
			pBits3 = pBuf2;
			cCounts = 0;
			cValue = *pBits3;
			*lBitsCount = 0;
			for (j=0;j<m;)
			{
				if((m-j)>=0xff)
				{
					*pBits = 0xff;
					pBits++;
					*pBits = 0;
					pBits++;
					(*lBitsCount)++;
					j+=0xff;
				}
				else
				{
					*pBits = (unsigned char)(m-j);
					pBits++;
					*pBits = 0;
					pBits++;
					(*lBitsCount)++;
					j+=(m-j);
				}
			}
			(*lBitsCount) *= 2;
			//保存到文件并创建位图结构
			if (g_cWriteFile==0)
			{
				g_bFile1OK=true;
				g_cWriteFile=1;
			}
			else
			{
				g_bFile2OK=true;
				g_cWriteFile=0;
			}
			lSendPage++;
		}
	}
	delete [] pBuf1;
	delete [] pBuf2;
	//pMemDC->DeleteDC();
	//delete pMemDC; 
	//pMemDC = NULL;

   
    delete pBMI;
    ::ReleaseDC(NULL,hDC);
	return 0;
}

//发送点阵,两个文件轮流发送(网络)
UINT SendDataThreadProc(LPVOID pParam)
{
	CWnd *pParent;
	SOCKET sock;
	CString strTmpFile;
	CPlotItem *pPlotItem;
	long lSendPage;
	long lTotalPage,lTotalPage1;
	char SendBuf[100];
	BYTE *pBits;
	BYTE *pBits1;
	unsigned long lBufLen;
	unsigned long lFileLen;
	unsigned long lTotalLen;
	long lSendLength;

	pParent=(CWnd*)((SendDataThreadInfo*)pParam)->m_pParent;
	sock=((SendDataThreadInfo*)pParam)->sock;
	pPlotItem=((SendDataThreadInfo*)pParam)->m_pPlotItem;
	lTotalLen = (g_lMaxPlotWidth + long((pPlotItem->m_dGapRight * MMPixelY + 7) / 8)) * SECTION_WIDTH;
	lTotalPage = (g_lMaxPlotLength + SECTION_WIDTH - 1) / SECTION_WIDTH;
	lTotalPage1 = lTotalPage;
	for(int i=0;i<g_nSplitNum;i++)
	{//文件分页数
		if (i == (g_nSplitNum-1))
		{
			if (i != 0)
				lTotalPage = (g_lMaxPlotLength - ftoi(10 * 40 * g_dScaleX * HpglPixelX) + g_nLineWidth + SECTION_WIDTH - 2) / SECTION_WIDTH;
			lTotalPage1 = lTotalPage;
			//20100224 屏蔽
			//if (g_nVersion == 0)//（如果是旧版本,图形间距是加在图形位图中走的)
				lTotalPage = lTotalPage + (ftoi(theApp.m_dGapDown * MMPixelX) + SECTION_WIDTH - 1) / SECTION_WIDTH;
		}
		lSendPage=0;
		while(lSendPage < lTotalPage)
		{//文件位图个数
			if (pPlotItem->m_nState == 5)
			{//文件被暂停
				if (g_nVersion == 1)
				{
					while (1)
					{
						strcpy(SendBuf,"Msg_Comm");//发送文件暂停帧
						SendBuf[8] = 0;
						SendBuf[9] = 7;
						send(sock,SendBuf,10,0);
						recv(sock,SendBuf,100,0);
						if (memcmp(SendBuf,"Msg_",4) == 0)
						{
							if ((SendBuf[5] == 7) && (SendBuf[7] == 9))
							{
								pPlotItem->m_nState = 6;//当前暂停帧已发送
								break;
							}
						}
						Sleep(10);
					}
				}
			}
			if(g_cSendFile==0)
			{
				while(!g_bFile1OK)//等待数据准备好
					Sleep(10);
				pBits = g_pBits1;
				lFileLen = g_lBits1Count;
			}
			else
			{
				while(!g_bFile2OK)
					Sleep(10);
				pBits = g_pBits2;
				lFileLen = g_lBits2Count;
			}
			//查询低层通讯缓冲区长度
			while (1)
			{
				if ((g_nVersion == 1) && (pPlotItem->m_nState != 6))
				{
					strcpy(SendBuf,"Buf_Long_Start");//发送查询帧（新版本）
					send(sock,SendBuf,14,0);
				}
				else
				{
					strcpy(SendBuf,"Buf_Long");//发送查询帧
					send(sock,SendBuf,8,0);
				}
				recv(sock,SendBuf,100,0);
				if (memcmp(SendBuf,"End_",4) == 0)
				{
					pPlotItem->m_nState = 3;
					while (pPlotItem->m_nState != 4)
					{
						if (((lSendPage+1)==lTotalPage) || (((lSendPage+2)==lTotalPage) && g_bFile1OK && g_bFile2OK))
							break;
						Sleep(10);
					}
					delete [] g_pBits1;
					delete [] g_pBits2;
					RemoveGLBuffer1();
					pParent->SendMessage(WM_ENDTRANSMITFILE,0,0);
					return 0;
				}
				pBits1 = (BYTE *)&lBufLen;
				*(pBits1+3) = SendBuf[4];
				*(pBits1+2) = SendBuf[5];
				*(pBits1+1) = SendBuf[6];
				*pBits1 = SendBuf[7];
				if (lBufLen > lTotalLen)
					break;
				else
				{
					if (pPlotItem->m_nState == 4)
						break;
					else
						Sleep(10);
				}
			}
			if (pPlotItem->m_nState == 6)
			{
				Sleep(10);
				continue;
			}
			if (pPlotItem->m_nState == 4)
			{
				while (1)
				{
					strcpy(SendBuf,"Msg_Comm");//发送文件传送结束帧
					SendBuf[8] = 0;
					SendBuf[9] = 4;
					send(sock,SendBuf,10,0);
					recv(sock,SendBuf,100,0);
					if (memcmp(SendBuf,"Msg_",4) == 0)
					{
						if ((SendBuf[5] == 4) && (SendBuf[7] == 9))
							break;
					}
					Sleep(10);
				}
				delete [] g_pBits1;
				delete [] g_pBits2;
				RemoveGLBuffer1();
				pParent->SendMessage(WM_ENDTRANSMITFILE,0,0);
				return 0;
			}
			strcpy(SendBuf,"Snd_Long");//发送数据包大小
			pBits1 = (BYTE *)&lFileLen;
			SendBuf[8] = *(pBits1+3);
			SendBuf[9] = *(pBits1+2);
			SendBuf[10] = *(pBits1+1);
			SendBuf[11] = *pBits1;
			send(sock,SendBuf,12,0);
			recv(sock,SendBuf,100,0);
			if(strncmp(SendBuf,"Msg_OkS ",8)==0)
				send(sock,(char *)pBits,lFileLen,0);
			
			while (1)
			{
				recv(sock,SendBuf,100,0);
				if(strncmp(SendBuf,"Msg_OkPG",8)==0)//已接收完一个数据包
					break;
				else if(strncmp(SendBuf,"Msg_End ",8)==0)//文件已被删除
				{
					pPlotItem->m_nState = 3;
					while (pPlotItem->m_nState != 4)
					{
						if (((lSendPage+1)==lTotalPage) || (((lSendPage+2)==lTotalPage) && g_bFile1OK && g_bFile2OK))
							break;
						Sleep(10);
					}
					delete [] g_pBits1;
					delete [] g_pBits2;
					RemoveGLBuffer1();
					pParent->SendMessage(WM_ENDTRANSMITFILE,0,0);
					return 0;
				}
			}
			if(g_cSendFile==0)
			{
				g_bFile1OK=false;
				g_cSendFile = 0x01;
			}
			else
			{
				g_bFile2OK=false;
				g_cSendFile = 0;
			}
			lSendPage++;
			if (lSendPage == lTotalPage1)
			{
				if (i == (g_nSplitNum-1))
					pPlotItem->m_lSendLength = (pPlotItem->m_lPictureLength - pPlotItem->m_lFixedPointLength * g_nSplitNum);
				else
					pPlotItem->m_lSendLength = long(g_lMaxPlotLength * (i + 1) / g_dScaleX * 25.4 / 300);
			}
			else if (lSendPage < lTotalPage1)
			{
				lSendLength = lSendPage * SECTION_WIDTH;//实时显示
				pPlotItem->m_lSendLength = long((g_lMaxPlotLength * i + lSendLength) / g_dScaleX * 25.4 / 300);
			}
			//g_bisSendingPlotData = true;//20150720-rwt
			pParent->SendMessage(WM_TRANSMITPERCENT,0,0);
			
		}
	}
	while (1)
	{
		strcpy(SendBuf,"Msg_Comm");//发送文件传送结束帧
		SendBuf[8] = 0;
		SendBuf[9] = 2;
		/*if (g_nVersion == 1)
		{
			pBits1 = (unsigned char *)&theApp.m_dGapDown;
			for(int i=10;i<18;i++)
			{
				SendBuf[i]=*pBits1;
				pBits1++;
			}
			send(sock,SendBuf,18,0);
		}
		else*/
			send(sock,SendBuf,10,0);
		recv(sock,SendBuf,100,0);
		if (memcmp(SendBuf,"Msg_",4) == 0)
		{
			if ((SendBuf[5] == 2) && (SendBuf[7] == 9))
				break;
		}
		Sleep(10);
	}
	delete [] g_pBits1;
	delete [] g_pBits2;

	
	//由定时器2来删除当前工作条目
	//RemoveGLBuffer1();		//20150715-rwt
	pParent->SendMessage(WM_ENDTRANSMITFILE,0,0);
	return 0;
}

/*
extern char USBDeviceState;
// 20101118 解决打印传输过程中USB中断传输，通讯连接失败造成的打印停止的问题
bool  CheckUSBState(CString BugMsg, bool  bOnDispState)
{
	int Count=0;
	if (USBDeviceState == 0)
	{
		CH375CloseDevice(g_iDeviceIndex);
		while(USBDeviceState==0)
		{
			Count++;
			if (Count>10000)
			{
				if(bOnDispState)
				AfxMessageBox(BugMsg+"[USB设备连接失败]");
				Count = 0;
				continue;
			}
		}
		
		Count=0;
		while(CH375OpenDevice(g_iDeviceIndex) == INVALID_HANDLE_VALUE)
		{
			Count++;
			if (Count>10000)
			{
				if(bOnDispState)
				AfxMessageBox(BugMsg+"[USB设备打开失败]");
				Count = 0;
				continue;
			}
		}
		
		Count=0;
		while(CH375GetUsbID(g_iDeviceIndex) != 0x4D495250)
		{
			Count++;
			if (Count>10000)
			{
				if(bOnDispState)
				AfxMessageBox(BugMsg+"[无法设别的USB设备]");
				Count = 0;
				continue;
			}			
		}
	}
	return true;
}
*/
//发送点阵,两个文件轮流发送(Usb)
UINT UsbSendDataThreadProc(LPVOID pParam)
{
	CWnd *pParent;
	CString strTmpFile;
	CPlotItem *pPlotItem;
	long lSendPage;
	long lTotalPage,lTotalPage1;
	char SendBuf[100];
	BYTE *pBits;
	BYTE *pBits1;
	unsigned long lBufLen;//绘图仪缓冲区长度
	unsigned long lFileLen;
	unsigned long lTotalLen;//数据包长度
	long lSendLength;
	unsigned long mLength;

	pParent=(CWnd*)((SendDataThreadInfo*)pParam)->m_pParent;
	pPlotItem=((SendDataThreadInfo*)pParam)->m_pPlotItem;
	lTotalLen = (g_lMaxPlotWidth + long((pPlotItem->m_dGapRight * MMPixelY + 7) / 8)) * SECTION_WIDTH;
	lTotalPage = (g_lMaxPlotLength + SECTION_WIDTH - 1) / SECTION_WIDTH;
	lTotalPage1 = lTotalPage;
	//CheckUSBState("Msg_Inset,", false);
	for(int i=0;i<g_nSplitNum;i++)
	{
		if (i == (g_nSplitNum-1))
		{
			if (i != 0)
				lTotalPage = (g_lMaxPlotLength - ftoi(10 * 40 * g_dScaleX * HpglPixelX) + g_nLineWidth + SECTION_WIDTH - 2) / SECTION_WIDTH;
			lTotalPage1 = lTotalPage;
			//20100224 屏蔽
			//if (g_nVersion == 0)
				lTotalPage = lTotalPage + (ftoi(theApp.m_dGapDown * MMPixelX) + SECTION_WIDTH - 1) / SECTION_WIDTH;
		}
		lSendPage=0;
		while(lSendPage < lTotalPage)
		{
			if (pPlotItem->m_nState == 5)
			{
				if (g_nVersion == 1)
				{
					while (1)
					{
						strcpy(SendBuf,"Msg_Comm");//发送文件暂停帧
						SendBuf[8] = 0;
						SendBuf[9] = 7;
						mLength = 10;
						if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // 发送成功
						{
							mLength = 64;
							CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
						}
						if (memcmp(SendBuf,"Msg_",4) == 0)
						{
							if ((SendBuf[5] == 7) && (SendBuf[7] == 9))
							{
								pPlotItem->m_nState = 6;
								break;
							}
						}
						Sleep(10);
					}
				}
			}
			if(g_cSendFile==0)
			{
				while(!g_bFile1OK)
					Sleep(10);
				pBits = g_pBits1;
				lFileLen = g_lBits1Count;
			}
			else
			{
				while(!g_bFile2OK)
					Sleep(10);
				pBits = g_pBits2;
				lFileLen = g_lBits2Count;
			}
			//查询低层通讯缓冲区长度
			while (1)
			{
				if ((g_nVersion == 1) && (pPlotItem->m_nState != 6))
				{
					strcpy(SendBuf,"Buf_Long_Start");//发送查询帧,查询绘图仪缓冲区长度
					mLength = 14;
				}
				else
				{
					strcpy(SendBuf,"Buf_Long");//发送查询帧
					mLength = 8;
				}
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // 发送成功
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
				if (memcmp(SendBuf,"End_",4) == 0)
				{//文件被删除
					pPlotItem->m_nState = 3;
					while (pPlotItem->m_nState != 4)
					{
						if (((lSendPage+1)==lTotalPage) || (((lSendPage+2)==lTotalPage) && g_bFile1OK && g_bFile2OK))
							break;
						Sleep(10);
					}
					delete [] g_pBits1;
					delete [] g_pBits2;
					RemoveGLBuffer1();
					pParent->SendMessage(WM_ENDTRANSMITFILE,0,0);
					return 0;
				}
				pBits1 = (BYTE *)&lBufLen;
				*(pBits1+3) = SendBuf[4];
				*(pBits1+2) = SendBuf[5];
				*(pBits1+1) = SendBuf[6];
				*pBits1 = SendBuf[7];
				if (lBufLen > lTotalLen)
					break;
				else
				{
					if (pPlotItem->m_nState == 4)
						break;
					else
						Sleep(10);
				}
			}
			if (pPlotItem->m_nState == 6)
			{
				Sleep(10);
				continue;
			}
			if (pPlotItem->m_nState == 4)
			{
				while (1)
				{
					strcpy(SendBuf,"Msg_Comm");//发送文件传送结束帧
					SendBuf[8] = 0;
					SendBuf[9] = 4;
					mLength = 10;
					if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // 发送成功
					{
						mLength = 64;
						CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
					}
					if (memcmp(SendBuf,"Msg_",4) == 0)
					{
						if ((SendBuf[5] == 4) && (SendBuf[7] == 9))
							break;
					}
					Sleep(10);
				}
				delete [] g_pBits1;
				delete [] g_pBits2;
				RemoveGLBuffer1();
				pParent->SendMessage(WM_ENDTRANSMITFILE,0,0);
				return 0;
			}
			strcpy(SendBuf,"Snd_Long");//发送查询帧
			pBits1 = (BYTE *)&lFileLen;
			SendBuf[8] = *(pBits1+3);
			SendBuf[9] = *(pBits1+2);
			SendBuf[10] = *(pBits1+1);
			SendBuf[11] = *pBits1;
			mLength = 12;
			if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // 发送成功
			{
				mLength = 64;
				CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
			}
			if(strncmp(SendBuf,"Msg_OkS ",8)==0)
			{
				while (1)
				{
					mLength = lFileLen;
					if (CH375WriteData(g_iDeviceIndex,(char *)pBits,&mLength))  // 发送成功
					{
						if (lFileLen > mLength)
						{
							lFileLen -= mLength;
							pBits += mLength;
						}
						else
							break;
					}
				}
			}
			
			while (1)
			{
				mLength = 64;
				CH375ReadData(0,SendBuf,&mLength);
				if(strncmp(SendBuf,"Msg_OkPG",8)==0)
					break;
				else if(strncmp(SendBuf,"Msg_End ",8)==0)
				{
					pPlotItem->m_nState = 3;
					while (pPlotItem->m_nState != 4)
					{
						if (((lSendPage+1)==lTotalPage) || (((lSendPage+2)==lTotalPage) && g_bFile1OK && g_bFile2OK))
							break;
						Sleep(10);
					}
					delete [] g_pBits1;
					delete [] g_pBits2;
					RemoveGLBuffer1();
					pParent->SendMessage(WM_ENDTRANSMITFILE,0,0);
					return 0;
				}
			}
			if(g_cSendFile==0)
			{
				g_bFile1OK=false;
				g_cSendFile = 0x01;
			}
			else
			{
				g_bFile2OK=false;
				g_cSendFile = 0;
			}
			lSendPage++;
			if (lSendPage == lTotalPage1)
			{
				if (i == (g_nSplitNum-1))
					pPlotItem->m_lSendLength = (pPlotItem->m_lPictureLength - pPlotItem->m_lFixedPointLength * g_nSplitNum);
				else
					pPlotItem->m_lSendLength = long(g_lMaxPlotLength * (i + 1) / g_dScaleX * 25.4 / 300);
			}
			else if (lSendPage < lTotalPage1)
			{
				lSendLength = lSendPage * SECTION_WIDTH;//实时显示
				pPlotItem->m_lSendLength = long((g_lMaxPlotLength * i + lSendLength) / g_dScaleX * 25.4 / 300);
			}
			g_bisSendingPlotData = true;//20150720-rwt
			pParent->SendMessage(WM_TRANSMITPERCENT,0,0);
		}
	}
	while (1)
	{
		strcpy(SendBuf,"Msg_Comm");//发送文件传送结束帧
		SendBuf[8] = 0;
		SendBuf[9] = 2;
		//20100224 
	/*	if (g_nVersion == 1)
		{
			int temp = (int)theApp.m_dGapDown;
//			pBits1 = (unsigned char *)&theApp.m_dGapDown;
			pBits1 = (unsigned char *)&temp;
			for(int i=10;i<14;i++)
			{
				SendBuf[i]=*pBits1;
				pBits1++;
			}
			mLength = 14;
		}
		else*/
			mLength = 10;
		if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // 发送成功
		{
			mLength = 64;
			CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
		}
		if (memcmp(SendBuf,"Msg_",4) == 0)
		{
			if ((SendBuf[5] == 2) && (SendBuf[7] == 9))
				break;
		}
		Sleep(10);
	}
	delete [] g_pBits1;
	delete [] g_pBits2;

	g_bisSendBMDataOver = true;
	while (g_bisSendBMDataOver != false)
		;
	RemoveGLBuffer1();//20150720-rwt
	pParent->SendMessage(WM_ENDTRANSMITFILE,0,0);//20150720-rwt
	
	return 0;
}


void GLTextMinXMaxX(CGLText1 *pText)
{
	int nLen;
	double PI=3.14159265359;
	double dTextAngle=g_dTextAngle/180.0*PI;
	double dLength;
	
	nLen=pText->m_strText.GetLength();
	if(nLen==0)
		return;

	CDPoint dp0,dp1;
	if((g_nFont==0)&&(g_nRotate==1))
	{
		dp0.x=g_lInitMaxPlotLength-pText->m_nY;
		dp0.y=pText->m_nX;
	}
	else
	{
		dp0.x=pText->m_nX;
		dp0.y=pText->m_nY;
	}
	switch(g_nTextPosition)
	{
		case 1:
			dp0.x=dp0.x-(g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(g_dTextHeight*400)*cos(dTextAngle);
			break;
		case 2:
			dp0.x=dp0.x-(0.5*g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*g_dTextHeight*400)*cos(dTextAngle);
			break;
		case 4:
			dp0.x=dp0.x-(g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(g_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
		case 5:
			dp0.x=dp0.x-(0.5*g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*g_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
		case 6:
			dp0.x=dp0.x-(0.5*nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
		case 7:
			dp0.x=dp0.x-(g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(g_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
		case 8:
			dp0.x=dp0.x-(0.5*g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*g_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
		case 9:
			dp0.x=dp0.x-(nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
		case 11:
			dTextAngle=atan2(0.25*g_dTextHeight*400,0.25*g_dTextWidth*400)+g_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*g_dTextHeight*400)*(0.25*g_dTextHeight*400)+(0.25*g_dTextWidth*400)*(0.25*g_dTextWidth*400));
			dp0.x=dp0.x+dLength*cos(dTextAngle);
			dp0.y=dp0.y+dLength*sin(dTextAngle);
			dTextAngle=g_dTextAngle/180.0*PI;
			dp0.x=dp0.x-(g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(g_dTextHeight*400)*cos(dTextAngle);
			break;
		case 12:
			dp0.x=dp0.x+(0.25*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y+(0.25*g_dTextWidth*400)*sin(dTextAngle);
			dp0.x=dp0.x-(0.5*g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*g_dTextHeight*400)*cos(dTextAngle);
			break;
		case 13:
			dTextAngle=atan2(0.25*g_dTextHeight*400,0.25*g_dTextWidth*400)-g_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*g_dTextHeight*400)*(0.25*g_dTextHeight*400)+(0.25*g_dTextWidth*400)*(0.25*g_dTextWidth*400));
			dp0.x=dp0.x+dLength*cos(dTextAngle);
			dp0.y=dp0.y-dLength*sin(dTextAngle);
			dTextAngle=g_dTextAngle/180.0*PI;
			break;
		case 14:
			dp0.x=dp0.x-(1.25*g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(1.25*g_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
		case 15:
			dp0.x=dp0.x-(0.5*g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*g_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
		case 16:
			dp0.x=dp0.x+(0.25*g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y-(0.25*g_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
		case 17:
			dTextAngle=atan2(0.25*g_dTextHeight*400,0.25*g_dTextWidth*400)-g_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*g_dTextHeight*400)*(0.25*g_dTextHeight*400)+(0.25*g_dTextWidth*400)*(0.25*g_dTextWidth*400));
			dp0.x=dp0.x-dLength*cos(dTextAngle);
			dp0.y=dp0.y+dLength*sin(dTextAngle);
			dTextAngle=g_dTextAngle/180.0*PI;
			dp0.x=dp0.x-(g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(g_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
		case 18:
			dp0.x=dp0.x-(0.25*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.25*g_dTextWidth*400)*sin(dTextAngle);
			dp0.x=dp0.x-(0.5*g_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*g_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
		case 19:
			dTextAngle=atan2(0.25*g_dTextHeight*400,0.25*g_dTextWidth*400)+g_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*g_dTextHeight*400)*(0.25*g_dTextHeight*400)+(0.25*g_dTextWidth*400)*(0.25*g_dTextWidth*400));
			dp0.x=dp0.x-dLength*cos(dTextAngle);
			dp0.y=dp0.y-dLength*sin(dTextAngle);
			dTextAngle=g_dTextAngle/180.0*PI;
			dp0.x=dp0.x-(nLen*g_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*g_dTextWidth*400)*sin(dTextAngle);
			break;
	}
	if(g_nFont==0)
	{
		pText->m_nX=ftoi((dp0.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2);
		pText->m_nY=ftoi(dp0.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
		pText->m_nMaxPDLength=ftoi(dp0.x);
		pText->m_nMinPDLength=ftoi(dp0.x);
		dp1.x=dp0.x+(nLen*g_dTextWidth*400)*cos(dTextAngle);
		if(pText->m_nMaxPDLength<ftoi(dp1.x))
			pText->m_nMaxPDLength=ftoi(dp1.x);
		else if(pText->m_nMinPDLength>ftoi(dp1.x))
			pText->m_nMinPDLength=ftoi(dp1.x);
		dp1.x=dp1.x+(g_dTextHeight*400)*sin(dTextAngle);
		if(pText->m_nMaxPDLength<ftoi(dp1.x))
			pText->m_nMaxPDLength=ftoi(dp1.x);
		else if(pText->m_nMinPDLength>ftoi(dp1.x))
			pText->m_nMinPDLength=ftoi(dp1.x);
		dp1.x=dp0.x+(g_dTextHeight*400)*sin(dTextAngle);
		if(pText->m_nMaxPDLength<ftoi(dp1.x))
			pText->m_nMaxPDLength=ftoi(dp1.x);
		else if(pText->m_nMinPDLength>ftoi(dp1.x))
			pText->m_nMinPDLength=ftoi(dp1.x);
		pText->m_nMaxPDWidth=ftoi(dp0.y);
		pText->m_nMinPDWidth=ftoi(dp0.y);
		dp1.y=dp0.y+(nLen*g_dTextWidth*400)*sin(dTextAngle);
		if(pText->m_nMaxPDWidth<ftoi(dp1.y))
			pText->m_nMaxPDWidth=ftoi(dp1.y);
		else if(pText->m_nMinPDWidth>ftoi(dp1.y))
			pText->m_nMinPDWidth=ftoi(dp1.y);
		dp1.y=dp1.y-(g_dTextHeight*400)*cos(dTextAngle);
		if(pText->m_nMaxPDWidth<ftoi(dp1.y))
			pText->m_nMaxPDWidth=ftoi(dp1.y);
		else if(pText->m_nMinPDWidth>ftoi(dp1.y))
			pText->m_nMinPDWidth=ftoi(dp1.y);
		dp1.y=dp0.y-(g_dTextHeight*400)*cos(dTextAngle);
		if(pText->m_nMaxPDWidth<ftoi(dp1.y))
			pText->m_nMaxPDWidth=ftoi(dp1.y);
		else if(pText->m_nMinPDWidth>ftoi(dp1.y))
			pText->m_nMinPDWidth=ftoi(dp1.y);
		if (g_lMaxPGLength < pText->m_nMaxPDLength)
			g_lMaxPGLength = pText->m_nMaxPDLength;
		if (g_lMaxPGWidth < pText->m_nMaxPDWidth)
			g_lMaxPGWidth = pText->m_nMaxPDWidth;
		pText->m_nMaxPDLength = ftoi((pText->m_nMaxPDLength - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2);
		pText->m_nMinPDLength = ftoi((pText->m_nMinPDLength - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2);
		pText->m_nMaxPDWidth = ftoi(pText->m_nMaxPDWidth * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
		pText->m_nMinPDWidth = ftoi(pText->m_nMinPDWidth * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
	}
	else
	{
		pText->m_nX=ftoi(dp0.x);
		pText->m_nY=ftoi(dp0.y);
	}
}

CDPoint GetRotatePoint(CDPoint dp, CDPoint dpCenter)
{
	double x=dp.x-dpCenter.x;
	double y=dp.y-dpCenter.y;
	double xTmp=x*cos(-g_dTextAngle) - y*sin(-g_dTextAngle);
	double yTmp=x*sin(-g_dTextAngle) + y*cos(-g_dTextAngle);
	double cx=xTmp + dpCenter.x;
	double cy=yTmp + dpCenter.y;
	return CDPoint(cx,cy);
}

/*
void HeapAdjust(int i, int nLength)
//本函数功能是：根据数组array构建大根堆
{
	int nChild;
	CGLPrimitive1 *nTemp, *pTmp1, *pTmp2;
	POSITION Pos1;
	
	Pos1 = g_listPrimitive.FindIndex(i);
	nTemp = (CGLPrimitive1*)g_listPrimitive.GetAt(Pos1);

	for (; 2 * i + 1 < nLength; i = nChild)
	{
		// 子结点的位置＝２*(父结点位置）+ 1
		nChild = 2 * i + 1;
		// 得到子结点中较大的结点
		pTmp1 = (CGLPrimitive1*)g_listPrimitive.GetAt(g_listPrimitive.FindIndex(i+1));
		pTmp2= (CGLPrimitive1*)g_listPrimitive.GetAt(g_listPrimitive.FindIndex(i));
		if (nChild != nLength - 1 && pTmp1->m_nMinPDLength > pTmp2->m_nMinPDLength)
			++nChild;
		// 如果较大的子结点大于父结点那么把较大的子结点往上移动,替换它的父结点
		pTmp1 = (CGLPrimitive1*)g_listPrimitive.GetAt(g_listPrimitive.FindIndex(nChild));
		if (nTemp->m_nMinPDLength < pTmp1->m_nMinPDLength)
		{
			nTemp = pTmp1;
		}
		else
		{ // 否则退出循环
			break;
		}
	}
	// 最后把需要调整的元素值放到合适的位置
	pTmp1 = (CGLPrimitive1*)g_listPrimitive.GetAt(g_listPrimitive.FindIndex(nChild));
	pTmp1 = nTemp;
}

void HeapSort()
{
	POSITION pTmp1, pTmp2;
	CGLPrimitive1 *pHead, *pTail;
	int i;
	int length = g_listPrimitive.GetCount();
	// 调整序列的前半部分元素,调整完之后第一个元素是序列的最大的元素
	for (i = length / 2 - 1; i >= 0; --i)
	{
		HeapAdjust(i, length);
	}
	// 从最后一个元素开始对序列进行调整,不断的缩小调整的范围直到第一个元素
	for (i = length - 1; i > 0; --i)
	{
		// 把第一个元素和当前的最后一个元素交换,
		// 保证当前的最后一个位置的元素都是在现在的这个序列之中最大的
		pTmp1 = g_listPrimitive.GetHeadPosition();
		pHead = (CGLPrimitive1*)g_listPrimitive.GetAt(pTmp1);
		pTmp2 = g_listPrimitive.FindIndex(i);
		pTail =(CGLPrimitive1*) g_listPrimitive.GetAt(pTmp2);
		g_listPrimitive.SetAt(pTmp1, pTail);
		g_listPrimitive.SetAt(pTmp2, pHead);
		// 不断缩小调整heap的范围,每一次调整完毕保证第一个元素是当前序列的最大值
		HeapAdjust(0, i);
	}
}
*/

//#define ADDRLEN	100000L
//CArray<POSITION,POSITION> ptArray;
static int ListNodeNum=0;
void InsertListData(CGLPrimitive1 *p)
{
	POSITION HeadPos, TailPos,  CurrPos;//, TmpPos;
	CGLPrimitive1 *pTmp1;//, *pTmp2;
	int  MidNode, MinNode, MaxNode, InsertNodeNum;//, TmpNode, listLength;
	int i=0;//, j;
	
	TailPos = g_listPrimitive.GetTailPosition();
	if(TailPos == NULL)
	{
		g_listPrimitive.AddTail(p);
		if(ptArray.GetSize() != 0)
			ptArray.RemoveAll();
		ListNodeNum = 0;
		ptArray.Add(TailPos);//尾插
		ListNodeNum++;
		return;
	}
	else
	{
		pTmp1 = (CGLPrimitive1*)g_listPrimitive.GetAt(TailPos);
		if (pTmp1->m_nMinPDLength <= p->m_nMinPDLength)	
		{
			g_listPrimitive.InsertAfter(TailPos, p);
			ptArray.Add(TailPos);//尾插
			ListNodeNum++;
			return;
		}
	}

	HeadPos = g_listPrimitive.GetHeadPosition();
	pTmp1= (CGLPrimitive1*)g_listPrimitive.GetAt(HeadPos);//返回当前位置数据之后，位置后移
	if (pTmp1->m_nMinPDLength >= p->m_nMinPDLength)	
	{
		g_listPrimitive.InsertBefore(HeadPos, p);
		ptArray.InsertAt(1, HeadPos);//首插	
		ListNodeNum++;
		return;
	}

	MaxNode = ListNodeNum-1  ;
	MinNode = 0;
	while( (MaxNode-MinNode) > 10)
	{
		MidNode = (MinNode + MaxNode)/2;
		CurrPos = ptArray.GetAt(MidNode);
		pTmp1 = (CGLPrimitive1*)g_listPrimitive.GetAt(CurrPos);
		if (pTmp1->m_nMinPDLength == p->m_nMinPDLength)
		{
			g_listPrimitive.InsertAfter(CurrPos, p);
			ptArray.InsertAt(MidNode, CurrPos);
			ListNodeNum++; 
			return;
		}
		else if(pTmp1->m_nMinPDLength > p->m_nMinPDLength)
			{//判断节点左边
				MaxNode = MidNode - 1;
			}
			else if(pTmp1->m_nMinPDLength < p->m_nMinPDLength)
				{//判断节点右边
					MinNode = MidNode + 1;
				}
	}
	
	TailPos = ptArray.GetAt(MaxNode);
	InsertNodeNum = MaxNode;
	CurrPos = TailPos;
	while (TailPos)
	{
		pTmp1 = (CGLPrimitive1*)g_listPrimitive.GetPrev(TailPos);
		if (pTmp1->m_nMinPDLength > p->m_nMinPDLength)
		{
			CurrPos = TailPos;
			InsertNodeNum -= 1;
		}
		else
			break;
	}
	if(InsertNodeNum < 1) InsertNodeNum = 1;
	g_listPrimitive.InsertAfter(CurrPos,p);
	ptArray.InsertAt(InsertNodeNum, CurrPos);
	ListNodeNum++;
}

void WriteSolidLine(CInstruct *pInstruct)
{
	int nSize;
	CGLPrimitive1 *p,*p1;
	CPoint dp;
	POSITION pos,pos1;

	nSize = pInstruct->m_adPara.GetSize();
	nSize--;
	if (g_nRotate==1)
	{
		dp.x = g_lInitMaxPlotLength - g_dpCurrent.y;
		dp.y = g_dpCurrent.x;
	}
	else
		dp = g_dpCurrent;

	p = new CGLPrimitive1;
	p->m_nType = PRIMITIVE_LINE;
	p->m_nMaxPDLength = dp.x;
	p->m_nMaxPDWidth = dp.y;
	p->m_nMinPDLength = dp.x;
	p->m_nMinPDWidth = dp.y;
	p->m_anData.Add(ftoi((dp.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2));
	p->m_anData.Add(ftoi(dp.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
	if (g_bAbsolute)
	{
		for (int i=0;i<nSize;)
		{
			if (g_nRotate==1)
			{
				dp.x = g_lInitMaxPlotLength - (ftoi(pInstruct->m_adPara.GetAt(i+1) * g_dRatioY) + g_dpOrigin.y - g_lMinPlotWidth);
				dp.y = ftoi(pInstruct->m_adPara.GetAt(i) * g_dRatioX) + g_dpOrigin.x + g_lMaxPlotLength;
				i += 2;
			}
			else
			{
				/*未改前
				dp.x = ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioX) + g_dpOrigin.x + g_lMaxPlotLength;
				dp.y = ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioY) + g_dpOrigin.y - g_lMinPlotWidth;
				*/
					if(hbwj!=2)
					{
					    dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;
						dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioY)+g_dpOrigin.y- g_lMinPlotWidth;
					}
					else if(hbwj==2 && (pg==1 || pg==0) && pgg!=2)
					{
					    dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;//合并文件
						dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioY)+g_dpOrigin.y- g_lMinPlotWidth;//合并文件
					}
                    else if(hbwj==2 && pgg==2 && pg==2)
					{	
					     dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioX)+g_dpOrigin.x;//合并文件
					     dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioY)+g_dpOrigin.y+g_lMaxPlotWidth- g_lMinPlotWidth;//合并文件	
					}
					else if(hbwj==2 && pg==1 && pgg==2)
					{
						if(pgnum!=0)
						{
					    dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength1*pgnum;//合并文件
						dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioY)+g_dpOrigin.y+g_lMaxPlotWidth1- g_lMinPlotWidth;//合并文件
						}
						else
						{
                        dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;//合并文件
						dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*g_dRatioY)+g_dpOrigin.y+g_lMaxPlotWidth- g_lMinPlotWidth;//合并文件
						}
					}
			}
			p->m_anData.Add(ftoi((dp.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2));
			p->m_anData.Add(ftoi(dp.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
			if (dp.x > p->m_nMaxPDLength)
				p->m_nMaxPDLength = dp.x;
			else if (dp.x < p->m_nMinPDLength)
				p->m_nMinPDLength = dp.x;
			if (dp.y > p->m_nMaxPDWidth)
				p->m_nMaxPDWidth = dp.y;
			else if (dp.y < p->m_nMinPDWidth)
				p->m_nMinPDWidth = dp.y;
		}
	}
	else
	{
		for (int i=0;i<nSize;)
		{
			if (g_nRotate==1)
			{
				dp.x = dp.x + ftoi(pInstruct->m_adPara.GetAt(i+1) * g_dRatioY);
				dp.y = dp.y - ftoi(pInstruct->m_adPara.GetAt(i) * g_dRatioX);
				i += 2;
			}
			else
			{
				dp.x = dp.x + ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioX);
				dp.y = dp.y + ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioY);
			}
			p->m_anData.Add(ftoi((dp.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
			p->m_anData.Add(ftoi(dp.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
			if (dp.x > p->m_nMaxPDLength)
				p->m_nMaxPDLength = dp.x;
			else if (dp.x < p->m_nMinPDLength)
				p->m_nMinPDLength = dp.x;
			if (dp.y > p->m_nMaxPDWidth)
				p->m_nMaxPDWidth = dp.y;
			else if (dp.y < p->m_nMinPDWidth)
				p->m_nMinPDWidth = dp.y;
		}
	}
	if (g_lMaxPGLength < p->m_nMaxPDLength)
		g_lMaxPGLength = p->m_nMaxPDLength;
	if (g_lMaxPGWidth < p->m_nMaxPDWidth)
		g_lMaxPGWidth = p->m_nMaxPDWidth;
	p->m_nMaxPDLength = ftoi((p->m_nMaxPDLength - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2);
	p->m_nMinPDLength = ftoi((p->m_nMinPDLength - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2);
	p->m_nMaxPDWidth = ftoi(p->m_nMaxPDWidth * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
	p->m_nMinPDWidth = ftoi(p->m_nMinPDWidth * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
	if (g_nRotate == 1)
	{
		g_dpCurrent.x = dp.y;
		g_dpCurrent.y = g_lInitMaxPlotLength - dp.x;
	}
	else
		g_dpCurrent = dp;

	if(FileLenExceed5MFlag == 0)
	{
		pos = g_listPrimitive.GetTailPosition();
		if (pos == NULL)
			g_listPrimitive.AddTail(p);
		else
		{
			pos1 = pos;
			while (pos)
			{
				p1 = (CGLPrimitive1*)g_listPrimitive.GetPrev(pos);
				if (p1->m_nMinPDLength > p->m_nMinPDLength)
					pos1 = pos;
				else
				break;
			}
			g_listPrimitive.InsertAfter(pos1,p);
		}
	}
	else
		InsertListData(p);
	g_dMoveLength = 0.0;
}

//将虚线全部分割成短实线
void WriteDashLine(CInstruct *pInstruct)
{
	int i,nSize;
	CGLPrimitive1 *p,*p1;
	CPoint dp,dp1,dp2,dp3;
	POSITION pos,pos1;
	long lIndexS;
	double dx,dy,dL1;
	double dLength,dMoveLengthS,dMoveLengthE;
	double dStep1,dStep2,dStep3,dStep4;
	unsigned char cEnd;

	nSize = pInstruct->m_adPara.GetSize();
	nSize--;
	if (g_nRotate==1)
	{
		dp.x = g_lInitMaxPlotLength - g_dpCurrent.y;
		dp.y = g_dpCurrent.x;
	}
	else
		dp = g_dpCurrent;
	p = new CGLPrimitive1;
	p->m_nType = PRIMITIVE_DASH;
	p->m_nMaxPDLength = dp.x;
	p->m_nMaxPDWidth = dp.y;
	p->m_nMinPDLength = dp.x;
	p->m_nMinPDWidth = dp.y;

	if ((g_nLineType == PS_DASH) || (g_nLineType == PS_DOT))
	{
		if (g_nLineType == PS_DASH)
		{   //虚线
			dStep1 = (double)g_lLinetypeStep * 0.5;
			dStep2 = (double)g_lLinetypeStep * 0.5;
		}
		else
		{   //点线
			dStep1 = (double)g_lLinetypeStep * 0.2;
			dStep2 = (double)g_lLinetypeStep * 0.8;
		}
		for (i=0;i<nSize;)
		{
			if (g_bAbsolute)
			{
				if (g_nRotate==1)
				{
					dp1.x = g_lInitMaxPlotLength - (ftoi(pInstruct->m_adPara.GetAt(i+1) * g_dRatioY) + g_dpOrigin.y - g_lMinPlotWidth);
					dp1.y = ftoi(pInstruct->m_adPara.GetAt(i) * g_dRatioX) + g_dpOrigin.x + g_lMaxPlotLength;
					i += 2;
				}
				else
				{
					dp1.x = ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioX) + g_dpOrigin.x + g_lMaxPlotLength;
					dp1.y = ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioY) + g_dpOrigin.y - g_lMinPlotWidth;
				}
			}
			else
			{
				if (g_nRotate==1)
				{
					dp1.x = dp.x + ftoi(pInstruct->m_adPara.GetAt(i+1) * g_dRatioY);
					dp1.y = dp.y - ftoi(pInstruct->m_adPara.GetAt(i) * g_dRatioX);
					i += 2;
				}
				else
				{
					dp1.x = dp.x + ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioX);
					dp1.y = dp.y + ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioY);
				}
			}
			if (dp == dp1)
				continue;
			if (dp1.x > p->m_nMaxPDLength)
				p->m_nMaxPDLength = dp1.x;
			else if (dp1.x < p->m_nMinPDLength)
				p->m_nMinPDLength = dp1.x;
			if (dp1.y > p->m_nMaxPDWidth)
				p->m_nMaxPDWidth = dp1.y;
			else if (dp1.y < p->m_nMinPDWidth)
				p->m_nMinPDWidth = dp1.y;
			dx = dp1.x - dp.x;
			dy = dp1.y - dp.y;
			dLength = sqrt(dx * dx + dy * dy);
			lIndexS = (long)(g_dMoveLength / (double)g_lLinetypeStep);
			dp2=dp;
			dMoveLengthS = g_lLinetypeStep * lIndexS;
			dMoveLengthE = g_dMoveLength + dLength;
			dL1 = dMoveLengthS + dStep1;
			if (dL1 <= g_dMoveLength)
			{
				dL1 = dL1 + dStep2;
				if (dL1 < dMoveLengthE)
				{
					dp2.x = ftoi(dx / dLength * (dL1 - g_dMoveLength)) + dp.x;
					dp2.y = ftoi(dy / dLength * (dL1 - g_dMoveLength)) + dp.y;
					lIndexS++;
					dMoveLengthS += g_lLinetypeStep;
				}
				else
				{
					g_dMoveLength = g_dMoveLength + dLength;
					dp = dp1;
					continue;
				}
			}
			while (dMoveLengthS < dMoveLengthE)
			{
				dL1 = dMoveLengthS + dStep1;
				if (dL1 >= dMoveLengthE)
				{
					p->m_anData.Add(ftoi((dp2.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
					p->m_anData.Add(ftoi(dp2.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
					p->m_anData.Add(ftoi((dp1.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
					p->m_anData.Add(ftoi(dp1.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
				}
				else
				{
					dp3.x = ftoi(dx / dLength * (dL1 - g_dMoveLength)) + dp.x;
					dp3.y = ftoi(dy / dLength * (dL1 - g_dMoveLength)) + dp.y;
					p->m_anData.Add(ftoi((dp2.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
					p->m_anData.Add(ftoi(dp2.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
					p->m_anData.Add(ftoi((dp3.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
					p->m_anData.Add(ftoi(dp3.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
					dL1 = dL1 + dStep2;
					if (dL1 < dMoveLengthE)
					{
						dp2.x = ftoi(dx / dLength * (dL1 - g_dMoveLength)) + dp.x;
						dp2.y = ftoi(dy / dLength * (dL1 - g_dMoveLength)) + dp.y;
					}
				}
				lIndexS++;
				dMoveLengthS += g_lLinetypeStep;
			}
			g_dMoveLength = g_dMoveLength + dLength;
			dp = dp1;
		}
	}
	else
	{//点划线
		dStep1 = (double)g_lLinetypeStep * 0.55;
		dStep2 = (double)g_lLinetypeStep * 0.15;
		dStep3 = (double)g_lLinetypeStep * 0.15;
		dStep4 = (double)g_lLinetypeStep * 0.15;
		for (i=0;i<nSize;)
		{
			if (g_bAbsolute)
			{
				if (g_nRotate==1)
				{
					dp1.x = g_lInitMaxPlotLength - (ftoi(pInstruct->m_adPara.GetAt(i+1) * g_dRatioY) + g_dpOrigin.y - g_lMinPlotWidth);
					dp1.y = ftoi(pInstruct->m_adPara.GetAt(i) * g_dRatioX) + g_dpOrigin.x + g_lMaxPlotLength;
					i += 2;
				}
				else
				{
					dp1.x = ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioX) + g_dpOrigin.x + g_lMaxPlotLength;
					dp1.y = ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioY) + g_dpOrigin.y - g_lMinPlotWidth;
				}
			}
			else
			{
				if (g_nRotate==1)
				{
					dp1.x = dp.x + ftoi(pInstruct->m_adPara.GetAt(i+1) * g_dRatioY);
					dp1.y = dp.y - ftoi(pInstruct->m_adPara.GetAt(i) * g_dRatioX);
					i += 2;
				}
				else
				{
					dp1.x = dp.x + ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioX);
					dp1.y = dp.y + ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioY);
				}
			}
			if (dp == dp1)
				continue;
			if (dp1.x > p->m_nMaxPDLength)
				p->m_nMaxPDLength = dp1.x;
			else if (dp1.x < p->m_nMinPDLength)
				p->m_nMinPDLength = dp1.x;
			if (dp1.y > p->m_nMaxPDWidth)
				p->m_nMaxPDWidth = dp1.y;
			else if (dp1.y < p->m_nMinPDWidth)
				p->m_nMinPDWidth = dp1.y;
			dx = dp1.x - dp.x;
			dy = dp1.y - dp.y;
			dLength = sqrt(dx * dx + dy * dy);
			lIndexS = ftoi(g_dMoveLength / (double)g_lLinetypeStep);
			if (((double)lIndexS * (double)g_lLinetypeStep) < g_dMoveLength)
				lIndexS++;
			lIndexS--;
			dp2=dp;
			dMoveLengthS = g_lLinetypeStep * lIndexS;
			dMoveLengthE = g_dMoveLength + dLength;
			while (dMoveLengthS < dMoveLengthE)
			{
				cEnd = 0;
				dL1 = dMoveLengthS + dStep1;
				if (dL1 > g_dMoveLength)
				{
					if (dL1 >= dMoveLengthE)
					{
						p->m_anData.Add(ftoi((dp2.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
						p->m_anData.Add(ftoi(dp2.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
						p->m_anData.Add(ftoi((dp1.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
						p->m_anData.Add(ftoi(dp1.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
						cEnd = 1;
					}
					else
					{
						dp3.x = ftoi(dx / dLength * (dL1 - g_dMoveLength)) + dp.x;
						dp3.y = ftoi(dy / dLength * (dL1 - g_dMoveLength)) + dp.y;
						p->m_anData.Add(ftoi((dp2.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
						p->m_anData.Add(ftoi(dp2.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
						p->m_anData.Add(ftoi((dp3.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
						p->m_anData.Add(ftoi(dp3.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
					}
				}
				if (cEnd == 0)
				{
					dL1 = dL1 + dStep2;
					if (dL1 > g_dMoveLength)
					{
						if (dL1 >= dMoveLengthE)
							cEnd = 1;
						else
						{
							dp2.x = ftoi(dx / dLength * (dL1 - g_dMoveLength)) + dp.x;
							dp2.y = ftoi(dy / dLength * (dL1 - g_dMoveLength)) + dp.y;
						}
					}
				}
				if (cEnd == 0)
				{
					dL1 = dL1 + dStep3;
					if (dL1 > g_dMoveLength)
					{
						if (dL1 >= dMoveLengthE)
						{
							p->m_anData.Add(ftoi((dp2.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
							p->m_anData.Add(ftoi(dp2.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
							p->m_anData.Add(ftoi((dp1.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
							p->m_anData.Add(ftoi(dp1.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
							cEnd = 1;
						}
						else
						{
							dp3.x = ftoi(dx / dLength * (dL1 - g_dMoveLength)) + dp.x;
							dp3.y = ftoi(dy / dLength * (dL1 - g_dMoveLength)) + dp.y;
							p->m_anData.Add(ftoi((dp2.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
							p->m_anData.Add(ftoi(dp2.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
							p->m_anData.Add(ftoi((dp3.x - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2));
							p->m_anData.Add(ftoi(dp3.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
						}
					}
				}
				if (cEnd == 0)
				{
					dL1 = dL1 + dStep4;
					if (dL1 > g_dMoveLength)
					{
						if (dL1 < dMoveLengthE)
						{
							dp2.x = ftoi(dx / dLength * (dL1 - g_dMoveLength)) + dp.x;
							dp2.y = ftoi(dy / dLength * (dL1 - g_dMoveLength)) + dp.y;
						}
					}
				}
				lIndexS++;
				dMoveLengthS = (double)g_lLinetypeStep * (double)lIndexS;
			}
			//实际走了才加，<0.5时没走不加，以免重复累加
			g_dMoveLength = g_dMoveLength + dLength;
			dp = dp1;
		}
	}
	if (g_lMaxPGLength < p->m_nMaxPDLength)
		g_lMaxPGLength = p->m_nMaxPDLength;
	if (g_lMaxPGWidth < p->m_nMaxPDWidth)
		g_lMaxPGWidth = p->m_nMaxPDWidth;
	p->m_nMaxPDLength = ftoi((p->m_nMaxPDLength - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2);
	p->m_nMinPDLength = ftoi((p->m_nMinPDLength - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2);
	p->m_nMaxPDWidth = ftoi(p->m_nMaxPDWidth * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
	p->m_nMinPDWidth = ftoi(p->m_nMinPDWidth * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
	if (g_nRotate == 1)
	{
		g_dpCurrent.x = dp.y;
		g_dpCurrent.y = g_lInitMaxPlotLength - dp.x;
	}
	else
		g_dpCurrent = dp;
	
	if(FileLenExceed5MFlag == 0)
	{
		pos = g_listPrimitive.GetTailPosition();
		if (pos == NULL)
			g_listPrimitive.AddTail(p);
		else
		{
			pos1 = pos;
			while (pos)
			{
				p1 = (CGLPrimitive1*)g_listPrimitive.GetPrev(pos);
				if (p1->m_nMinPDLength > p->m_nMinPDLength)
					pos1 = pos;
				else
					break;
			}
			g_listPrimitive.InsertAfter(pos1,p);
		}
	}
	else
		InsertListData(p);
}

//将字符转换成线条
void TextChangeLine(CGLText1 *pText)
{
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString sWorkPath,strEnglish,strChinese;
	CFile fileEnglish,fileChinese;
	BYTE *pbyData;
	int nLeft;
	BYTE byByte1,byByte2;
	WORD wCharCode;
	int nIndex1,nIndex2,nIndex3,nIndex4;
	BOOL bEnglishChar;
	CPoint dp;
	LPCTSTR lpszString;
	int nCount;
	CGLPrimitive1 *p1;
	POSITION pos,pos1;

	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	sWorkPath.Format(_T("%s%s"),szDrive,szDir);
	strEnglish=sWorkPath;
	strEnglish=strEnglish + _T("english.fnt");
	strChinese=sWorkPath;
	strChinese=strChinese + _T("hz.fnt");

	fileEnglish.Open((LPCTSTR)strEnglish,CFile::modeRead | CFile::typeBinary);
	fileChinese.Open((LPCTSTR)strChinese,CFile::modeRead | CFile::typeBinary);

	dp.x=pText->m_nX;
	dp.y=pText->m_nY;
	lpszString=pText->m_strText;
	nCount=pText->m_strText.GetLength();

	nLeft=0;
	nIndex1=0;
	while (nIndex1 < nCount)
	{
		pbyData=NULL;
		g_pText=NULL;
		byByte1=(BYTE)lpszString[nIndex1];
		if ((byByte1 >= 128) && ((nIndex1+1) < nCount))
		{
			bEnglishChar=false;
			nIndex1++;
			byByte2=(BYTE)lpszString[nIndex1];
			wCharCode=(((WORD)byByte1) << 8) | (WORD)byByte2;
			nIndex2=0;
			nIndex3=g_aChinese_char.GetSize() - 1;
			if (g_aChinese_char[nIndex2].m_wCharCode == wCharCode)
				nIndex4=nIndex2;
      else if (g_aChinese_char[nIndex3].m_wCharCode == wCharCode)
				nIndex4=nIndex3;
      else
				nIndex4=(nIndex2 + nIndex3) / 2;
      while (((nIndex3 - nIndex2) > 1) && (g_aChinese_char[nIndex4].m_wCharCode != wCharCode))
			{
        if (g_aChinese_char[nIndex4].m_wCharCode > wCharCode)
				{
          nIndex3=nIndex4;
          if (nIndex2 > nIndex3)
						break;
          nIndex4=(nIndex2 + nIndex3) / 2;
        }
        else
				{
          nIndex2=nIndex4;
          if (nIndex2 > nIndex3)
						break;
          nIndex4=(nIndex2 + nIndex3) / 2;
        }
      }
      if ((g_aChinese_char[nIndex4].m_wCharCode == wCharCode) &&
          (g_aChinese_char[nIndex4].m_wBytes > 0))
			{
        pbyData=new BYTE[g_aChinese_char[nIndex4].m_wBytes + 1];
        fileChinese.Seek(g_aChinese_char[nIndex4].m_dwPosition,CFile::begin);
        fileChinese.Read(pbyData,g_aChinese_char[nIndex4].m_wBytes);
      }
    }
    else
		{
      bEnglishChar=true;
      wCharCode=(WORD)byByte1;
      nIndex2=0;
      nIndex3=g_aEnglish_char.GetSize() - 1;
      if (g_aEnglish_char[nIndex2].m_wCharCode == wCharCode)
				nIndex4=nIndex2;
      else if (g_aEnglish_char[nIndex3].m_wCharCode == wCharCode)
				nIndex4=nIndex3;
      else
				nIndex4=(nIndex2 + nIndex3) / 2;
      while (((nIndex3 - nIndex2) > 1) && (g_aEnglish_char[nIndex4].m_wCharCode != wCharCode))
			{
        if (g_aEnglish_char[nIndex4].m_wCharCode > wCharCode)
				{
          nIndex3=nIndex4;
          if (nIndex2 > nIndex3)
						break;
          nIndex4=(nIndex2 + nIndex3) / 2;
        }
        else
				{
          nIndex2=nIndex4;
          if (nIndex2 > nIndex3)
						break;
          nIndex4=(nIndex2 + nIndex3) / 2;
        }
      }
      if ((g_aEnglish_char[nIndex4].m_wCharCode == wCharCode) &&
          (g_aEnglish_char[nIndex4].m_wBytes > 0))
			{
        pbyData=new BYTE[g_aEnglish_char[nIndex4].m_wBytes + 1];

        fileEnglish.Seek(g_aEnglish_char[nIndex4].m_dwPosition,CFile::begin);
        fileEnglish.Read(pbyData,g_aEnglish_char[nIndex4].m_wBytes);
      }
    }
    if (pbyData != NULL)
		{
      if (bEnglishChar)
				PlotChar(dp,pbyData,g_aEnglish_char[nIndex4].m_wBytes,nLeft,0,g_nHeight_C);
     else
				PlotChar(dp,pbyData,g_aChinese_char[nIndex4].m_wBytes,nLeft,0,g_nInternalLeading_E + g_nHeight_E);
      delete pbyData;
      if (bEnglishChar)
				nLeft=nLeft + g_aEnglish_char[nIndex4].m_wWidth * g_nHeight_C;
      else
				nLeft=nLeft + g_nWidth_C * (g_nInternalLeading_E + g_nHeight_E);
    }
    else
		{
      if (bEnglishChar)
				nLeft=nLeft + g_aEnglish_char[0].m_wWidth * g_nHeight_C;
      else
				nLeft=nLeft + g_nWidth_C * (g_nInternalLeading_E + g_nHeight_E);
    }
    nIndex1++;
		if (g_pText!=NULL)
		{
			if (g_lMaxPGLength < g_pText->m_nMaxPDLength)
				g_lMaxPGLength = g_pText->m_nMaxPDLength;
			if (g_lMaxPGWidth < g_pText->m_nMaxPDWidth)
				g_lMaxPGWidth = g_pText->m_nMaxPDWidth;
			g_pText->m_nMaxPDLength = ftoi((g_pText->m_nMaxPDLength - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2);
			g_pText->m_nMinPDLength = ftoi((g_pText->m_nMinPDLength - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2);
			g_pText->m_nMaxPDWidth = ftoi(g_pText->m_nMaxPDWidth * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
			g_pText->m_nMinPDWidth = ftoi(g_pText->m_nMinPDWidth * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
			pos = g_listPrimitive.GetTailPosition();
			if (pos == NULL)
				g_listPrimitive.AddTail(g_pText);
			else
			{
				pos1 = pos;
				while (pos)
				{
					p1 = (CGLPrimitive1*)g_listPrimitive.GetPrev(pos);
					if (p1->m_nMinPDLength > g_pText->m_nMinPDLength)
						pos1 = pos;
					else
						break;
				}
				g_listPrimitive.InsertAfter(pos1,g_pText);
			}
			g_dMoveLength = 0.0;
			g_pText=NULL;
		}
  }
  fileEnglish.Close();
  fileChinese.Close();
}

//输入参数:
//  ptPointLU  显示字符串的左上角坐标(即TextOut(...)的x,y值)
//  pbyData 字符的描述数据
//  wBytes 字符的描述数据个数
//  nLeft,nDown 字符的左下角坐标(即以英汉字库的公共单位及坐标水平绘制字符串时,字符的的左下角坐标)
//  nFontScale 字库的长度比例
void PlotChar(CPoint ptPointLU, BYTE *pbyData, WORD wBytes, int nLeft, int nDown, int nFontScale)
{
  BYTE byByteX,byByteY;
  CPoint ptCurrentPos,ptCenter,ptPointS,ptPointE;
  CPoint ptPoint1;
  WORD wIndex1;
  WORD wDirection,wLength;
  BOOL bPenUp,bVerTextCommand,bClockwise;
  WORD wRadius,wArcS,wSpan;
  int nRadius;
  double dAngleS,dAngleE,dAngleOffsetS,dAngleOffsetE;
  double dx1,dy1,dx2,dy2,dxc,dyc,dr,dD,dH,dBulge;
  double dCos,dSin;
  double dLToDScale;
  int nHeight;
  double PI=3.14159265359;

  if (wBytes == 0)
		return;

  nHeight=(g_nInternalLeading_E + g_nHeight_E) * g_nHeight_C;
  dLToDScale=(double)nHeight / 	(g_dTextHeight*400);//实际毫米数

 //by YHZ 使用矢量字体时文字方向错误
//  dCos=cos(g_dTextAngle);
//  dSin=sin(g_dTextAngle);
  dCos=cos(g_dTextAngle/180.0*PI);
  dSin=sin(g_dTextAngle/180.0*PI);

  ptCurrentPos=CPoint(nLeft,nDown);
  ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
  MoveTo(ptPoint1.x,ptPoint1.y);
  bVerTextCommand=false;
  bPenUp=false;
  wIndex1=0;
  while (wIndex1 < wBytes)
  {
    switch (pbyData[wIndex1])
		{
    case 0x0:
      wIndex1++;
      break;
    case 0x1:
      wIndex1++;
      bPenUp=false;
      break;
    case 0x2:
      wIndex1++;
      bPenUp=true;
      break;
    case 0x3:
      wIndex1++;
      wIndex1++;
      bVerTextCommand=false;
      break;
    case 0x4:
      wIndex1++;
      wIndex1++;
      bVerTextCommand=false;
      break;
    case 0x5:
      break;
    case 0x6:
      break;
    case 0x7:
      break;
    case 0x8:
      if (bVerTextCommand)
			{
        wIndex1=wIndex1+3;
      }
      else
			{
        wIndex1++;
        ptCurrentPos.x=ptCurrentPos.x + (char)pbyData[wIndex1] * nFontScale;
        wIndex1++;
        ptCurrentPos.y=ptCurrentPos.y + (char)pbyData[wIndex1] * nFontScale;

        ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        if (bPenUp)
					MoveTo(ptPoint1.x,ptPoint1.y);
        else
					LineTo(ptPoint1.x,ptPoint1.y);

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    case 0x9:
      if (bVerTextCommand)
			{
        wIndex1++;
        byByteX=pbyData[wIndex1];
        wIndex1++;
        byByteY=pbyData[wIndex1];
        while ((byByteX != 0) || (byByteY != 0))
				{
          wIndex1++;
          byByteX=pbyData[wIndex1];
          wIndex1++;
          byByteY=pbyData[wIndex1];
        }
        wIndex1++;
      }
      else
			{
        wIndex1++;
        byByteX=pbyData[wIndex1];
        wIndex1++;
        byByteY=pbyData[wIndex1];
        while ((byByteX != 0) || (byByteY != 0))
				{
          ptCurrentPos.x=ptCurrentPos.x + (char)byByteX * nFontScale;
          ptCurrentPos.y=ptCurrentPos.y + (char)byByteY * nFontScale;

          ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
          if (bPenUp)
						MoveTo(ptPoint1.x,ptPoint1.y);
          else
						LineTo(ptPoint1.x,ptPoint1.y);

          wIndex1++;
          byByteX=pbyData[wIndex1];
          wIndex1++;
          byByteY=pbyData[wIndex1];
        }

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    case 0xA:
      if (bVerTextCommand)
			{
        wIndex1=wIndex1+3;
      }
      else
			{
        wIndex1++;
        wRadius=(WORD)pbyData[wIndex1];
        wIndex1++;
        if (((char)pbyData[wIndex1]) < 0)
					bClockwise=true;
        else
					bClockwise=false;
        wArcS=(WORD)pbyData[wIndex1];
        wArcS=wArcS & 0x70;
        wArcS=wArcS >> 4;
        wSpan=(WORD)pbyData[wIndex1];
        wSpan=wSpan & 0x7;

        //计算圆心
        ptCenter=ptCurrentPos;
        ptCenter.x=ptCenter.x - IntRound((double)wRadius * (double)nFontScale * cos((double)wArcS * 45.0 / 180.0 * 3.14159265359));
        ptCenter.y=ptCenter.y - IntRound((double)wRadius * (double)nFontScale * sin((double)wArcS * 45.0 / 180.0 * 3.14159265359));

        dAngleS=(double)wArcS * 45.0 / 180.0 * 3.14159265359;
        if (bClockwise)
					dAngleE=dAngleS - (double)wSpan * 45.0 / 180.0 * 3.14159265359;
        else
					dAngleE=dAngleS + (double)wSpan * 45.0 / 180.0 * 3.14159265359;

        if (bClockwise)
				{
          ptPointS.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleE));
          ptPointS.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleE));
          ptPointE.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleS));
          ptPointE.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleS));
          ptCurrentPos=ptPointS;
        }
        else
				{
          ptPointS.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleS));
          ptPointS.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleS));
          ptPointE.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleE));
          ptPointE.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleE));
          ptCurrentPos=ptPointE;
        }

        ptCenter=CPToLP(ptCenter,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointS=CPToLP(ptPointS,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointE=CPToLP(ptPointE,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        nRadius=IntRound((double)wRadius * (double)nFontScale / dLToDScale);
        Arc(ptCenter.x - nRadius,ptCenter.y - nRadius,
            ptCenter.x + nRadius,ptCenter.y + nRadius,
            ptPointS.x,ptPointS.y,ptPointE.x,ptPointE.y);

        ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        MoveTo(ptPoint1.x,ptPoint1.y);

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    case 0xB:
      if (bVerTextCommand)
			{
        wIndex1=wIndex1+6;
      }
      else
			{
        wIndex1++;
        dAngleOffsetS=(double)((WORD)pbyData[wIndex1]);
        wIndex1++;
        dAngleOffsetE=(double)((WORD)pbyData[wIndex1]);
        wIndex1++;
        wRadius=(WORD)pbyData[wIndex1];
        wRadius=wRadius << 8;
        wIndex1++;
        wRadius=wRadius | (WORD)pbyData[wIndex1];
        wIndex1++;
        if (((char)pbyData[wIndex1]) < 0)
					bClockwise=true;
        else
					bClockwise=false;
        wArcS=(WORD)pbyData[wIndex1];
        wArcS=wArcS & 0x70;
        wArcS=wArcS >> 4;
        wSpan=(WORD)pbyData[wIndex1];
        wSpan=wSpan & 0x7;

        //计算圆心
        ptCenter=ptCurrentPos;
        ptCenter.x=ptCenter.x - IntRound((double)wRadius * (double)nFontScale * cos((double)wArcS * 45.0 / 180.0 * 3.14159265359));
        ptCenter.y=ptCenter.y - IntRound((double)wRadius * (double)nFontScale * sin((double)wArcS * 45.0 / 180.0 * 3.14159265359));

        dAngleS=(double)wArcS * 45.0;
        dAngleS=dAngleOffsetS * 45.0 / 256.0 + dAngleS;
        if (bClockwise)
					dAngleE=-(double)wSpan * 45.0;
        else
					dAngleE=(double)wSpan * 45.0;
        dAngleE=dAngleOffsetE * 45.0 / 256.0 + dAngleE;
        dAngleE=dAngleOffsetE * 45.0 / 256.0 + dAngleE;
        dAngleS=dAngleS / 180.0 * 3.14159265359;
        dAngleE=dAngleE / 180.0 * 3.14159265359;

        if (bClockwise)
				{
          ptPointS.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleE));
          ptPointS.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleE));
          ptPointE.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleS));
          ptPointE.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleS));
          ptCurrentPos=ptPointS;
        }
        else
				{
          ptPointS.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleS));
          ptPointS.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleS));
          ptPointE.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleE));
          ptPointE.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleE));
          ptCurrentPos=ptPointE;
        }

        ptCenter=CPToLP(ptCenter,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointS=CPToLP(ptPointS,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointE=CPToLP(ptPointE,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        nRadius=IntRound((double)wRadius * (double)nFontScale / dLToDScale);

        Arc(ptCenter.x - nRadius,ptCenter.y - nRadius,
            ptCenter.x + nRadius,ptCenter.y + nRadius,
            ptPointS.x,ptPointS.y,ptPointE.x,ptPointE.y);

        ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        MoveTo(ptPoint1.x,ptPoint1.y);

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    case 0xC:
      if (bVerTextCommand)
			{
        wIndex1=wIndex1+4;
      }
      else
			{
        wIndex1++;
        dx1=(double)((char)pbyData[wIndex1]) * (double)nFontScale;
        wIndex1++;
        dy1=(double)((char)pbyData[wIndex1]) * (double)nFontScale;
        wIndex1++;
        dBulge=(double)((char)pbyData[wIndex1]);
        if (((char)pbyData[wIndex1]) < 0)
					bClockwise=true;
        else
					bClockwise=false;

        dD=sqrt(dx1*dx1 + dy1*dy1);
        dH=fabs(dBulge) * dD / 127.0 / 2.0;

        if (bClockwise)
				{
          dx2=-dy1;
          dy2=dx1;
        }
        else
				{
          dx2=dy1;
          dy2=-dx1;
        }
        dx2=dx2 / dD * dH + dx1 / 2.0;
        dy2=dy2 / dD * dH + dy1 / 2.0;

        CircleCR(0.0,0.0,dx2,dy2,dx1,dy1,dxc,dyc,dr);
        ptCenter.x=ptCurrentPos.x + IntRound(dxc);
        ptCenter.y=ptCurrentPos.y + IntRound(dyc);

        if (bClockwise)
				{
          ptPointS.x=ptCurrentPos.x + IntRound(dx1);
          ptPointS.y=ptCurrentPos.y + IntRound(dy1);
          ptPointE=ptCurrentPos;
          ptCurrentPos=ptPointS;
        }
        else
				{
          ptPointS=ptCurrentPos;
          ptPointE.x=ptCurrentPos.x + IntRound(dx1);
          ptPointE.y=ptCurrentPos.y + IntRound(dy1);
          ptCurrentPos=ptPointE;
        }

        ptCenter=CPToLP(ptCenter,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointS=CPToLP(ptPointS,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointE=CPToLP(ptPointE,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        nRadius=IntRound(dr / dLToDScale);

        Arc(ptCenter.x - nRadius,ptCenter.y - nRadius,
            ptCenter.x + nRadius,ptCenter.y + nRadius,
            ptPointS.x,ptPointS.y,ptPointE.x,ptPointE.y);

        ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        MoveTo(ptPoint1.x,ptPoint1.y);

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    case 0xD:
      break;
    case 0xE:
      bVerTextCommand=true;
      wIndex1++;
      break;
    default:
      if (bVerTextCommand)
			{
        wIndex1++;
      }
      else
			{
        wDirection=(WORD)(pbyData[wIndex1] % 16);
        wLength=(WORD)(pbyData[wIndex1] / 16) * (WORD)nFontScale;
        ptCurrentPos=GetNextCoodinate(wDirection,wLength,ptCurrentPos);

        ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        if (bPenUp)
					MoveTo(ptPoint1.x,ptPoint1.y);
        else
					LineTo(ptPoint1.x,ptPoint1.y);

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    }
  }
}

CPoint CPToLP(CPoint ptCP, int nHeight, CPoint ptPointLU, double dSin, double dCos, double dScale)
{
	double dx,dy;
  CPoint ptPoint1;

  dx=(double)ptCP.x;
  dy=(double)ptCP.y-(double)nHeight;
  ptPoint1.x=ptPointLU.x + IntRound((dx*dCos - dy*dSin) / dScale);
  ptPoint1.y=ptPointLU.y + IntRound((dx*dSin + dy*dCos) / dScale);

  return ptPoint1;
}

void MoveTo(long x, long y)
{
	g_ptCurrentPosition.x=x;
	g_ptCurrentPosition.y=y;

  g_ptPrevPosition=g_ptCurrentPosition;
  g_ptPrevPosition.x=g_ptPrevPosition.x +1;  //为在LineTo中优化用
}

void LineTo(long x, long y)
{
  CPoint ptCurrentPoint;
	CPoint ptPoint1;
	CPoint ptPoint2;
	CRect rect1;
  
	ptCurrentPoint.x=x;
	ptCurrentPoint.y=y;

	ptPoint1=g_ptCurrentPosition;
	ptPoint2=ptCurrentPoint;
	if ((g_ptCurrentPosition == g_ptPrevPosition) &&
			(0 < g_ptCurrentPosition.x) && (0 < g_ptCurrentPosition.y))
	{
	}
	else
	{
		WriteTextData(true,ptPoint1.x,ptPoint1.y);
	}
	WriteTextData(false,ptPoint2.x,ptPoint2.y);
  g_ptPrevPosition = ptCurrentPoint;
  g_ptCurrentPosition = ptCurrentPoint;
}

void Arc(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXStartArc, int nYStartArc, int nXEndArc, int nYEndArc)
{
  double dXC,dYC,dRadius;
  double dx,dy;
  double dAngleS,dAngleE,dAngle,dStep;
  double dCos,dSin,PI=3.14159265359;
  int nCount;

  dXC=((double)nLeftRect + (double)nRightRect) / 2.0;
  dYC=((double)nTopRect + (double)nBottomRect) / 2.0;
  dRadius=((double)nRightRect - (double)nLeftRect) / 2.0;

  dAngleS=angle_2(dXC,dYC,(double)nXStartArc,(double)nYStartArc);
  dAngleE=angle_2(dXC,dYC,(double)nXEndArc,(double)nYEndArc);
  while (dAngleE >= dAngleS) dAngleE=dAngleE - 2.0 * PI;

  if ((nXStartArc == nXEndArc) && (nYStartArc == nYEndArc))  {
    dAngleS=0.0;
    dAngleE=-2.0 * PI;
  }

  dSin=sin(dAngleS);
  dCos=cos(dAngleS);
  dx=dRadius * dCos + dXC;
  dy=dRadius * dSin + dYC;
  MoveTo((int)dx,(int)dy);

  nCount=120;
  dStep=PI / 180.0 * (360.0 / (double)nCount);  //3度
  dAngle=dAngleS;
  while (dAngle > dAngleE)  {
    dAngle=dAngle - dStep;
    if (dAngle < dAngleE)  dAngle=dAngleE;
    dSin=sin(dAngle);
    dCos=cos(dAngle);
    dx=dRadius * dCos + dXC;
    dy=dRadius * dSin + dYC;

    LineTo((int)dx,(int)dy);
  }
}

BOOL CircleCR(double x1, double y1, double x2, double y2, double x3, double y3, double &cx, double &cy, double &cr)
{
  double a1,b1,c1,a2,b2,c2;
  BOOL bOk;

  if (((x1==x2) && (y1==y2)) ||
      ((x1==x3) && (y1==y3)) ||
      ((x2==x3) && (y2==y3)))  {  //三点中有两点相同
    return false;
  }

  a1=x1 - x2;
  b1=y1 - y2;
  c1=-a1 * (x1 + x2) / 2.0 - b1 * (y1 + y2) / 2.0;
  a2=x3 - x2;
  b2=y3 - y2;
  c2=-a2 * (x2 + x3) / 2.0 - b2 * (y2 + y3) / 2.0;
  bOk=IntOfTwoLine(a1,b1,c1,a2,b2,c2,cx,cy);

  if (bOk)  {
    cr=(x1 - cx) * (x1 - cx) + (y1 - cy) * (y1 - cy);
    cr=sqrt(cr);
  }

  return bOk;
}

CPoint GetNextCoodinate(WORD wDirection, WORD wLength, CPoint ptPoint)
{
  CPoint ptPoint1;

  switch (wDirection)  {
  case 0:
    ptPoint1.x = ptPoint.x + (int)wLength;
    ptPoint1.y = ptPoint.y;
    break;
  case 1:
    ptPoint1.x = ptPoint.x + (int)wLength;
    ptPoint1.y = ptPoint.y + (int)wLength/2;
    break;
  case 2:
    ptPoint1.x = ptPoint.x + (int)wLength;
    ptPoint1.y = ptPoint.y + (int)wLength;
    break;
  case 3:
    ptPoint1.x = ptPoint.x + (int)wLength/2;
    ptPoint1.y = ptPoint.y + (int)wLength;
    break;
  case 4:
    ptPoint1.x = ptPoint.x;
    ptPoint1.y = ptPoint.y + (int)wLength;
    break;
  case 5:
    ptPoint1.x = ptPoint.x - (int)wLength/2;
    ptPoint1.y = ptPoint.y + (int)wLength;
    break;
  case 6:
    ptPoint1.x = ptPoint.x - (int)wLength;
    ptPoint1.y = ptPoint.y + (int)wLength;
    break;
  case 7:
    ptPoint1.x = ptPoint.x - (int)wLength;
    ptPoint1.y = ptPoint.y + (int)wLength/2;
    break;
  case 8:
    ptPoint1.x = ptPoint.x - (int)wLength;
    ptPoint1.y = ptPoint.y;
    break;
  case 9:
    ptPoint1.x = ptPoint.x - (int)wLength;
    ptPoint1.y = ptPoint.y - (int)wLength/2;
    break;
  case 10:
    ptPoint1.x = ptPoint.x - (int)wLength;
    ptPoint1.y = ptPoint.y - (int)wLength;
    break;
  case 11:
    ptPoint1.x = ptPoint.x - (int)wLength/2;
    ptPoint1.y = ptPoint.y - (int)wLength;
    break;
  case 12:
    ptPoint1.x = ptPoint.x;
    ptPoint1.y = ptPoint.y - (int)wLength;
    break;
  case 13:
    ptPoint1.x = ptPoint.x + (int)wLength/2;
    ptPoint1.y = ptPoint.y - (int)wLength;
    break;
  case 14:
    ptPoint1.x = ptPoint.x + (int)wLength;
    ptPoint1.y = ptPoint.y - (int)wLength;
    break;
  case 15:
    ptPoint1.x = ptPoint.x + (int)wLength;
    ptPoint1.y = ptPoint.y - (int)wLength/2;
    break;
  }

  return ptPoint1;
}

double angle_2(double startx, double starty, double endx, double endy)
{
  double pi,q1;
  double x,y;

  pi=3.14159265359;

  q1=0.0;
  x=endx - startx;
  if (x==0)  {
    if (starty==endy)  return(q1);
    if (starty<endy)  q1=0.5*pi;
    else  q1=1.5*pi;
  }
  else  {
    y=endy-starty;
    q1=atan(y/x);
    if (x < 0)  {  //第二三象限
      q1=pi+q1;
    }
    else  {
      if ((x > 0) && (y < 0))  {  //第四象限
        q1=(double)2.0*pi+q1;
      }
    }
  }
  return q1;
}

void WriteTextData(BOOL bFirst,long x,long y)
{
	CPoint dp;
	CGLPrimitive1 *p1;
	POSITION pos,pos1;

	if (g_nRotate==1)
	{
		dp.x = g_lInitMaxPlotLength - y;
		dp.y = x;
	}
	else
	{
		dp.x = x;
		dp.y = y;
	}
	if(bFirst)
	{
		if (g_pText!=NULL)
		{
			if (g_lMaxPGLength < g_pText->m_nMaxPDLength)
				g_lMaxPGLength = g_pText->m_nMaxPDLength;
			if (g_lMaxPGWidth < g_pText->m_nMaxPDWidth)
				g_lMaxPGWidth = g_pText->m_nMaxPDWidth;
			g_pText->m_nMaxPDLength = ftoi((g_pText->m_nMaxPDLength - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2);
			g_pText->m_nMinPDLength = ftoi((g_pText->m_nMinPDLength - g_lFixedPointLength) * g_dScaleX  * HpglPixelX + g_nLineWidth / 2);
			g_pText->m_nMaxPDWidth = ftoi(g_pText->m_nMaxPDWidth * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
			g_pText->m_nMinPDWidth = ftoi(g_pText->m_nMinPDWidth * g_dScaleY * HpglPixelY + g_nLineWidth / 2);
			pos = g_listPrimitive.GetTailPosition();
			if (pos == NULL)
				g_listPrimitive.AddTail(g_pText);
			else
			{
				pos1 = pos;
				while (pos)
				{
					p1 = (CGLPrimitive1*)g_listPrimitive.GetPrev(pos);
					if (p1->m_nMinPDLength > g_pText->m_nMinPDLength)
						pos1 = pos;
					else
						break;
				}
				g_listPrimitive.InsertAfter(pos1,g_pText);
			}
			g_dMoveLength = 0.0;
			g_pText=NULL;
		}
		g_pText = new CGLPrimitive1;
		g_pText->m_nType = PRIMITIVE_LINE;
		g_pText->m_nMaxPDLength = dp.x;
		g_pText->m_nMaxPDWidth = dp.y;
		g_pText->m_nMinPDLength = dp.x;
		g_pText->m_nMinPDWidth = dp.y;
		g_pText->m_anData.Add(ftoi((dp.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2));
		g_pText->m_anData.Add(ftoi(dp.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
	}
	else
	{
		g_pText->m_anData.Add(ftoi((dp.x - g_lFixedPointLength) * g_dScaleX * HpglPixelX + g_nLineWidth / 2));
		g_pText->m_anData.Add(ftoi(dp.y * g_dScaleY * HpglPixelY + g_nLineWidth / 2));
		if (dp.x > g_pText->m_nMaxPDLength)
			g_pText->m_nMaxPDLength = dp.x;
		else if (dp.x < g_pText->m_nMinPDLength)
			g_pText->m_nMinPDLength = dp.x;
		if (dp.y > g_pText->m_nMaxPDWidth)
			g_pText->m_nMaxPDWidth = dp.y;
		else if (dp.y < g_pText->m_nMinPDWidth)
			g_pText->m_nMinPDWidth = dp.y;
	}
}

//求两直线的交点
//入口参数
//  a1,b1,c1 直线a1*x+b1*y+c1=0
//  a2,b2,c2 直线a2*x+b2*y+c2=0
//出口参数
//  (x,y) 交点坐标
//返回值
//  =0,没有交点
//  =1,有交点(x,y)
//  =2,两直线重合
int IntOfTwoLine(double a1, double b1, double c1, double a2, double b2, double c2, double &x, double &y)
{
  double p,q;
  int nResult;

  p=a1*b2-a2*b1;
  q=c2*b1-c1*b2;
  if ((fabs(p)+1.0) <= 1.0)  {  //两直线段平行
    if ((fabs(q)+1.0) <= 1.0)  {  //两直线重合
      nResult=2;
    }
    else  {  //两直线段平行但不重合
      nResult=0;
    }
  }
  else  {  //两直线段有交点
    x=q/p;
    q=a2*c1-a1*c2;
    y=q/p;
    nResult=1;
  }
  return nResult;
}

void InitTextChangeLine(void)
{
	CString strPL;
  TCHAR szFullPath[_MAX_PATH];
  TCHAR szDir[_MAX_DIR];
  TCHAR szDrive[_MAX_DRIVE];
  CString sWorkPath,strCharFileName;
  CFile fFile1;
  int nCountChar;
  CChinese_char Chinese_char1;
  CEnglish_char English_char1;
  BYTE byte[10];
  int nCharCount_C,nCharCount_E;
  CString strNetworkPath,string1;
  CFileFind filefind;

	g_ptCurrentPosition=NULL;
	g_pText=NULL;

  ::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
  _splitpath(szFullPath,szDrive,szDir,NULL,NULL);
  sWorkPath.Format(_T("%s%s"),szDrive,szDir);

  strCharFileName=sWorkPath;
  strCharFileName=strCharFileName + _T("english.fnt");
  fFile1.Open((LPCTSTR)strCharFileName,CFile::modeRead | CFile::typeBinary);
  fFile1.SeekToBegin();
  fFile1.Read(byte,10);
  g_nInternalLeading_E=(int)((WORD)byte[0] | ((WORD)byte[1] << 8));
  g_nHeight_E=(int)((WORD)byte[2] | ((WORD)byte[3] << 8));
  g_nDescent_E=(int)((WORD)byte[4] | ((WORD)byte[5] << 8));
  nCharCount_E=(int)((DWORD)byte[6] | ((DWORD)byte[7] << 8) | ((DWORD)byte[9] << 16) | ((DWORD)byte[9] << 24));
  g_aEnglish_char.RemoveAll();
  g_aEnglish_char.SetSize(nCharCount_E);
  fFile1.Seek(10,CFile::begin);
  nCountChar=nCharCount_E;
  while (nCountChar > 0)
  {
    fFile1.Read(byte,10);

    English_char1.m_wCharCode=(WORD)byte[0] | ((WORD)byte[1] << 8);
    English_char1.m_wBytes=(WORD)byte[2] | ((WORD)byte[3] << 8);
    English_char1.m_wWidth=(WORD)byte[4] | ((WORD)byte[5] << 8);
    English_char1.m_dwPosition=(DWORD)byte[6] | ((DWORD)byte[7] << 8) | ((DWORD)byte[9] << 16) | ((DWORD)byte[9] << 24);

    g_aEnglish_char.SetAt(nCharCount_E-nCountChar,English_char1);

    nCountChar--;
  }
  fFile1.Close();

  strCharFileName=sWorkPath;
  strCharFileName=strCharFileName + _T("hz.fnt");
  fFile1.Open((LPCTSTR)strCharFileName,CFile::modeRead | CFile::typeBinary);
  fFile1.SeekToBegin();
  fFile1.Read(byte,8);
  g_nWidth_C=(int)((WORD)byte[0] | ((WORD)byte[1] << 8));
  g_nHeight_C=(int)((WORD)byte[2] | ((WORD)byte[3] << 8));
  nCharCount_C=(int)((DWORD)byte[4] | ((DWORD)byte[5] << 8) | ((DWORD)byte[6] << 16) | ((DWORD)byte[7] << 24));

  g_aChinese_char.RemoveAll();
  g_aChinese_char.SetSize(nCharCount_C);
  fFile1.Seek(8,CFile::begin);
  nCountChar=nCharCount_C;
  while (nCountChar > 0)
  {
    fFile1.Read(byte,8);

    Chinese_char1.m_wCharCode=(WORD)byte[0] | ((WORD)byte[1] << 8);
    Chinese_char1.m_wBytes=(WORD)byte[2] | ((WORD)byte[3] << 8);
    Chinese_char1.m_dwPosition=(DWORD)byte[4] | ((DWORD)byte[5] << 8) | ((DWORD)byte[6] << 16) | ((DWORD)byte[7] << 24);

    g_aChinese_char.SetAt(nCharCount_C-nCountChar,Chinese_char1);

    nCountChar--;
  }
  fFile1.Close();
}

void GetBitmapData(HDC hDC, HGDIOBJ bmpHandle,int iWidth, int iHeight, BYTE *pData, long &lLength)
{
	BITMAPINFOHEADER bi;  //记录图片的小大，颜色深度等
	DWORD dwSignLineBytes;
	
	dwSignLineBytes = iWidth/8 + (iWidth / 8) % 2;
	if (dwSignLineBytes * 8 < (DWORD)iWidth)
	{
		dwSignLineBytes = (iWidth + 31) / 32 * 4;
	}
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth =  iWidth;
	bi.biHeight =  iHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 1;
	bi.biCompression = BI_RGB;  //没压缩
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	
	lLength = dwSignLineBytes * iHeight;
	pData = NULL;
	pData = new BYTE[lLength];
	GetDIBits(hDC, (HBITMAP)bmpHandle, 0, (DWORD)iHeight, (LPVOID)pData, (LPBITMAPINFO)&bi, (DWORD)DIB_RGB_COLORS);
}

