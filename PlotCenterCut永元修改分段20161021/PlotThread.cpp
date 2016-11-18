// PlotThread.cpp : implementation file
#include "stdafx.h"
#include "PlotCenter.h"
#include "PlotCenterDlg.h"
#include "Instruct.h"
#include <math.h>
#include "ch375dll.h"
#include <stdio.h>


#define BUFFER_SIZE 100000
#define HpglPixelX		0.29527559//X方向1个hpgl单位对应的像素点数
#define HpglPixelY		0.198412732//Y方向1个hpgl单位对应的像素点数
#define MMPixelX		11.811023622//X方向1MM对应的像素点数
#define MMPixelY		7.936508//Y方向1MM对应的像素点数
#define PixelHpglY	5//1个像素点对应的hpgl单位数
#define PLTDATAPATH 	"c:\\pltData.plt"
#define CUTDATAPATH		"c:\\CutData.plt"
#define NCUTDATAPATH	"c:\\NCutData.plt"
BOOL g_bUserDefine=FALSE;//自定义命令

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
//BOOL g_bPrintEnd=false;			//  图形文件打印结束

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
long g_lMaxPlotLength;//最大图形长度
long g_lMaxPlotWidth;//最大图形宽度
long g_lMaxPGLength;//段内（PG）最大图形长度
long g_lMaxPGWidth;//段内（PG）最大图形宽度
long g_lInitMaxPlotLength;//图形旋转时最大图形长度
long g_lInitMaxPlotWidth;//图形旋转时最大图形宽度
long g_lMinPlotWidth;//最小图形宽度
int g_nRotate;//设置中的旋转
int g_nFont;//字体
//int g_iNewFileSplitNum = 0; //20140520 文件拆分个数

long g_lCurrMaxWidthLength;//当前图形最大宽度20100907
extern unsigned char g_cPrintCutSelect;//20101206 打印切割选择
CArray <CChinese_char,CChinese_char> g_aChinese_char;
CArray <CEnglish_char,CEnglish_char> g_aEnglish_char;
int g_nInternalLeading_E,g_nHeight_E,g_nDescent_E; //m_nHeight_E是windows中字体的Ascent-InternalLeading
int g_nHeight_C,g_nWidth_C;//字高，字宽
CPoint g_ptCurrentPosition;   //当前位置(坐标)
CPoint g_ptPrevPosition;      //前一位置(坐标)
CGLPrimitive1 *g_pText;//文本

unsigned char g_cPenNo = 1; // 20101206 笔号刀号 20150729 默认值改为1
unsigned char g_cCutNo;

double  gl_dPaperLength;	//20140709  纸张长度，用于分页切割
CStringArray m_NeedCutDataFiles;//gzr20161029存储需将切割数据复制打印数据中


void WriteSolidLine(CInstruct *pInstruct);
void WriteDashLine(CInstruct *pInstruct);
CDPoint GetRotatePoint(CDPoint dp, CDPoint dpCenter);
void GLTextMinXMaxX(CGLText1 *pText);
void SetRatio(void);
void RemoveGLBuffer1(void);
void InitHPGL(void);
int DissecretFileList(CPlotItem *pPlotItem,int nCommunicateMode, SOCKET sock);
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
UINT SendCutDataFile(int nCommunicateMode, CString strFileName, SOCKET sock);//20100907
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
					dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;
					dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y-g_lMinPlotWidth;
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
				dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*g_dRatioX)+g_dpOrigin.x+g_lMaxPlotLength;
				dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*g_dRatioY)+g_dpOrigin.y-g_lMinPlotWidth;
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
		if(nSize>=2)
		{
			//20100304
			double d1=pInstruct->m_adPara.GetAt(0);
			double d2=pInstruct->m_adPara.GetAt(1);
			if((d1==0)&&(d2==-1))
				g_dTextAngle=-90;
			else if((d1==0)&&(d2==1))
				g_dTextAngle=90;
			else if((d1==1)&&(d2==0))
				g_dTextAngle=0;
			else if((d1==-1)&&(d2==0))
				g_dTextAngle=180;
			else if((d1!=0)&&(d2!=0))
				g_dTextAngle=atan2(d1,d2);
			/*
			double d1=pInstruct->m_adPara.GetAt(0);
			double d2=pInstruct->m_adPara.GetAt(1);
			if((d1==0)&&(d2!=0))
				g_dTextAngle=90;
			else if((d1!=0)&&(d2==0))
				g_dTextAngle=0;
			else if((d1!=0)&&(d2!=0))
				g_dTextAngle=atan2(d1,d2);
			else
			{
				if((g_nFont==0)&&(g_nRotate==1))
					g_dTextAngle-=90;
			}
			*/
		}
		else
			g_dTextAngle=0;
		if((g_nFont==0)&&(g_nRotate==1))
			g_dTextAngle+=90;
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
				{
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
/*	else if(strcmp(pInstruct->m_cName,"CI")==0)//圆
	{
		nSize=pInstruct->m_alPara.GetSize();
		if(nSize>=1)
		{
			CGLPrimitive *p=new CGLPrimitive;
			m_listPrimitive.AddTail(p);
			SetGLPrimitiveBasePara(p);
			p->m_nType=PRIMITIVE_CIRCLE;
			p->m_adData.Add(m_dpCurrent.x);
			p->m_adData.Add(m_dpCurrent.y);
			d=pInstruct->m_alPara.GetAt(0);
			p->m_adData.Add(pInstruct->m_alPara.GetAt(0));
		}
		//终点不变
	}
	else if(strcmp(pInstruct->m_cName,"EA")==0)//绝对矩形
	{
		nSize=pInstruct->m_alPara.GetSize();
		if(nSize>=2)
		{
			CGLPrimitive *p=new CGLPrimitive;
			m_listPrimitive.AddTail(p);
			SetGLPrimitiveBasePara(p);
			p->m_nType=PRIMITIVE_RECT;
			p->m_adData.Add(m_dpCurrent.x);
			p->m_adData.Add(m_dpCurrent.y);
			p->m_adData.Add(pInstruct->m_alPara.GetAt(0));
			p->m_adData.Add(pInstruct->m_alPara.GetAt(1));
		}
		//终点不变
	}
	else if(strcmp(pInstruct->m_cName,"ER")==0)//相对矩形
	{
		nSize=pInstruct->m_alPara.GetSize();
		if(nSize>=2)
		{
			CGLPrimitive *p=new CGLPrimitive;
			m_listPrimitive.AddTail(p);
			SetGLPrimitiveBasePara(p);
			p->m_nType=PRIMITIVE_RECT;
			p->m_adData.Add(m_dpCurrent.x);
			p->m_adData.Add(m_dpCurrent.y);
			p->m_adData.Add(pInstruct->m_alPara.GetAt(0)+m_dpCurrent.x);
			p->m_adData.Add(pInstruct->m_alPara.GetAt(1)+m_dpCurrent.y);
		}
		//终点不变
	}
	else if(strcmp(pInstruct->m_cName,"AT")==0)//3点画弧
	{
		nSize=pInstruct->m_alPara.GetSize();
		if(nSize>=4)
		{
			CDPoint dp0,dp1;
			dp0.x=pInstruct->m_alPara.GetAt(0);
			dp0.y=pInstruct->m_alPara.GetAt(1);
			dp1.x=pInstruct->m_alPara.GetAt(2);
			dp1.y=pInstruct->m_alPara.GetAt(3);
			if(m_bPenDown)
			{
				if(ThreePointOnLine(m_dpCurrent,dp0,dp1)==FALSE)
				{
					CGLPrimitive *p=new CGLPrimitive;
					m_listPrimitive.AddTail(p);
					SetGLPrimitiveBasePara(p);
					p->m_nType=PRIMITIVE_ARC3;
					p->m_adData.Add(m_dpCurrent.x);
					p->m_adData.Add(m_dpCurrent.y);
					p->m_adData.Add(dp0.x);
					p->m_adData.Add(dp0.y);
					p->m_adData.Add(dp1.x);
					p->m_adData.Add(dp1.y);
				}
				else//三点共线
				{
					if(m_dpCurrent==dp0 && dp0==dp1)
					{
						CGLPrimitive *p=new CGLPrimitive;
						m_listPrimitive.AddTail(p);
						SetGLPrimitiveBasePara(p);
						p->m_nType=PRIMITIVE_POINT;
						p->m_adData.Add(m_dpCurrent.x);
						p->m_adData.Add(m_dpCurrent.y);
					}
					else if(dp0==m_dpCurrent || dp0==dp1)
					{
						CGLPrimitive *p=new CGLPrimitive;
						m_listPrimitive.AddTail(p);
						SetGLPrimitiveBasePara(p);
						p->m_nType=PRIMITIVE_LINE;
						p->m_adData.Add(m_dpCurrent.x);
						p->m_adData.Add(m_dpCurrent.y);
						p->m_adData.Add(dp1.x);
						p->m_adData.Add(dp1.y);
					}
					else if(m_dpCurrent==dp1)
					{
						CGLPrimitive *p=new CGLPrimitive;
						m_listPrimitive.AddTail(p);
						SetGLPrimitiveBasePara(p);
						p->m_nType=PRIMITIVE_CIRCLE;
						p->m_adData.Add(m_dpCurrent.x+dp1.x);
						p->m_adData.Add(m_dpCurrent.y+dp1.y);
						p->m_adData.Add(GetDistance(m_dpCurrent,dp1)/2);
					}
					else //三点各异
					{
						CGLPrimitive *p=new CGLPrimitive;
						m_listPrimitive.AddTail(p);
						SetGLPrimitiveBasePara(p);
						p->m_nType=PRIMITIVE_LINE;
						p->m_adData.Add(m_dpCurrent.x);
						p->m_adData.Add(m_dpCurrent.y);
						p->m_adData.Add(dp0.x);
						p->m_adData.Add(dp0.y);
						p->m_adData.Add(dp1.x);
						p->m_adData.Add(dp1.y);
					}
				}
			}
			//计算终点
			m_dpCurrent=dp1;
		}
	}
	else if(strcmp(pInstruct->m_cName,"RT")==0)//相对3点画弧
	{
		nSize=pInstruct->m_alPara.GetSize();
		if(nSize>=4)
		{
			CDPoint dp0,dp1;
			dp0.x=pInstruct->m_alPara.GetAt(0);
			dp0.y=pInstruct->m_alPara.GetAt(1);
			dp1.x=pInstruct->m_alPara.GetAt(2);
			dp1.y=pInstruct->m_alPara.GetAt(3);
			dp0+=m_dpCurrent;
			dp1+=dp0;
			if(m_bPenDown)
			{
				if(ThreePointOnLine(m_dpCurrent,dp0,dp1)==FALSE)
				{
					CGLPrimitive *p=new CGLPrimitive;
					m_listPrimitive.AddTail(p);
					SetGLPrimitiveBasePara(p);
					p->m_nType=PRIMITIVE_ARC3;
					p->m_adData.Add(m_dpCurrent.x);
					p->m_adData.Add(m_dpCurrent.y);
					p->m_adData.Add(dp0.x);
					p->m_adData.Add(dp0.y);
					p->m_adData.Add(dp1.x);
					p->m_adData.Add(dp1.y);
				}
				else//三点共线
				{
					if(m_dpCurrent==dp0 && dp0==dp1)
					{
						CGLPrimitive *p=new CGLPrimitive;
						m_listPrimitive.AddTail(p);
						SetGLPrimitiveBasePara(p);
						p->m_nType=PRIMITIVE_POINT;
						p->m_adData.Add(m_dpCurrent.x);
						p->m_adData.Add(m_dpCurrent.y);
					}
					else if(dp0==m_dpCurrent || dp0==dp1)
					{
						CGLPrimitive *p=new CGLPrimitive;
						m_listPrimitive.AddTail(p);
						SetGLPrimitiveBasePara(p);
						p->m_nType=PRIMITIVE_LINE;
						p->m_adData.Add(m_dpCurrent.x);
						p->m_adData.Add(m_dpCurrent.y);
						p->m_adData.Add(dp1.x);
						p->m_adData.Add(dp1.y);
					}
					else if(m_dpCurrent==dp1)
					{
						CGLPrimitive *p=new CGLPrimitive;
						m_listPrimitive.AddTail(p);
						SetGLPrimitiveBasePara(p);
						p->m_nType=PRIMITIVE_CIRCLE;
						p->m_adData.Add(m_dpCurrent.x+dp1.x);
						p->m_adData.Add(m_dpCurrent.y+dp1.y);
						p->m_adData.Add(GetDistance(m_dpCurrent,dp1)/2);
					}
					else //三点各异
					{
						CGLPrimitive *p=new CGLPrimitive;
						m_listPrimitive.AddTail(p);
						SetGLPrimitiveBasePara(p);
						p->m_nType=PRIMITIVE_LINE;
						p->m_adData.Add(m_dpCurrent.x);
						p->m_adData.Add(m_dpCurrent.y);
						p->m_adData.Add(dp0.x);
						p->m_adData.Add(dp0.y);
						p->m_adData.Add(dp1.x);
						p->m_adData.Add(dp1.y);
					}
				}
			}
			//计算终点
			m_dpCurrent=dp1;
		}
	}
	else if(strcmp(pInstruct->m_cName,"AR")==0)//相对画弧
	{
		nSize=pInstruct->m_alPara.GetSize();
		if(nSize>=3)
		{
			CDPoint dp0;
			dp0.x=pInstruct->m_alPara.GetAt(0);
			dp0.y=pInstruct->m_alPara.GetAt(1);
			dp0+=m_dpCurrent;
			if(m_bPenDown)
			{
				CGLPrimitive *p=new CGLPrimitive;
				m_listPrimitive.AddTail(p);
				SetGLPrimitiveBasePara(p);
				p->m_nType=PRIMITIVE_ARC;
				p->m_adData.Add(dp0.x);
				p->m_adData.Add(dp0.y);
				p->m_adData.Add(m_dpCurrent.x);
				p->m_adData.Add(m_dpCurrent.y);
				p->m_adData.Add(pInstruct->m_alPara.GetAt(2));
			}
			//计算终点
			double dAngle=GetAngle(dp0,m_dpCurrent);
			dAngle+=pInstruct->m_alPara.GetAt(3)*PAI/180;
			double r=GetDistance(dp0,m_dpCurrent);
			m_dpCurrent.y=r*sin(dAngle)+dp0.x;
			m_dpCurrent.x=r*cos(dAngle)+dp0.y;
		}
	}
	else if(strcmp(pInstruct->m_cName,"AA")==0)//绝对画弧
	{
		nSize=pInstruct->m_alPara.GetSize();
		if(nSize>=3)
		{
			CDPoint dp0;
			dp0.x=pInstruct->m_alPara.GetAt(0);
			dp0.y=pInstruct->m_alPara.GetAt(1);
			if(m_bPenDown)
			{
				CGLPrimitive *p=new CGLPrimitive;
				m_listPrimitive.AddTail(p);
				SetGLPrimitiveBasePara(p);
				p->m_nType=PRIMITIVE_ARC;
				p->m_adData.Add(dp0.x);
				p->m_adData.Add(dp0.y);
				p->m_adData.Add(m_dpCurrent.x);
				p->m_adData.Add(m_dpCurrent.y);
				p->m_adData.Add(pInstruct->m_alPara.GetAt(2));
			}
			//计算终点
			double dAngle=GetAngle(dp0,m_dpCurrent);
			dAngle+=pInstruct->m_alPara.GetAt(3)*PAI/180;
			double r=GetDistance(dp0,m_dpCurrent);
			m_dpCurrent.y=r*sin(dAngle)+dp0.x;
			m_dpCurrent.x=r*cos(dAngle)+dp0.y;
		}
	}*/
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
			g_lMaxPlotLength=g_lMaxPGLength;
			if (g_lMaxPlotWidth < g_lMaxPGWidth)
				g_lMaxPlotWidth = g_lMaxPGWidth;
		}
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
	pDC->SetBkColor(RGB(0,0,0));
	if (g_nSplitNum > 1)
	{//分页
		pen.CreatePen(LINE_TYPE_DOT,1,RGB(255,255,255));
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
	pen.CreatePen(nLineType,g_nLineWidth,RGB(255,255,255));
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

	if (g_nFont==0)
	{
		CFont font;
		CFont *pOldFont;
		LOGFONT lf;
		CFont fontNew;
		BOOL b;
		
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255,255,255));
		pos=g_listTextPrimitive.GetHeadPosition();
		while(pos)
		{
			pos1=pos;
			pText=(CGLText1*)g_listTextPrimitive.GetNext(pos);
			if(pText->m_nMinPDLength>iSectionValueY_1)
				break;
			else if(pText->m_nMaxPDLength<iSectionValueY_3)
				continue;
			else if(pText->m_nMinPDWidth>iSectionValueX_1)
				continue;
			if(pText->m_dTextHeight==0)
				continue;
			
			if(!font.CreateStockObject(DEFAULT_GUI_FONT)) 
				VERIFY(font.CreateStockObject(SYSTEM_FONT));
			pOldFont=pDC->SelectObject(&font);
			pOldFont->GetLogFont(&lf);
			//gzr20160907 start
			lf = theApp.m_config.m_lfDrawText;
			//lf.lfCharSet = 222;		
			//strcpy(lf.lfFaceName,"HP Roman 8");
			//gzr20160907 end

			lf.lfHeight=ftoi(pText->m_dTextHeight*3000.0/25.4*g_dScaleX * 0.68);
			if(abs(lf.lfHeight)<1)
				lf.lfHeight=1;
			
  			//lf.lfWidth=ftoi(pText->m_dTextWidth*3000.0/25.4*g_dScaleX);
  			//if(abs(lf.lfWidth)<1)
  			//	lf.lfWidth=1;
			lf.lfWidth=lf.lfHeight * 0.55 * g_dScaleY / g_dScaleX;
			//lf.lfWidth=lf.lfHeight * MMPixelX/MMPixelY;
			//lf.lfHeight = lf.lfHeight * MMPixelY;
			//lf.lfWidth = lf.lfWidth * MMPixelX;
			/*
			lf.lfWidth=ftoi(pText->m_dTextWidth*3000.0/25.4*g_dScaleY);
			if(abs(lf.lfWidth)<1)
				lf.lfWidth=1;
			*/

			//gzr20160907 start
			//lf.lfWeight=FW_THIN;
			//gzr20160907 end

			lf.lfOrientation=ftoi(pText->m_dTextAngle*10);
			lf.lfEscapement=ftoi(pText->m_dTextAngle*10);
			b=fontNew.CreateFontIndirect(&lf);
			pDC->SelectObject(&fontNew);
				
			nSize=pText->m_strText.GetLength();
			if(nSize>256)
				nSize=256;
			pDC->TextOut(pText->m_nX-iSectionValueY1,iSectionValueX-pText->m_nY,pText->m_strText);
			pDC->SelectObject(pOldFont);
			font.DeleteObject();
			fontNew.DeleteObject();
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


//取得文件名和扩展名
CString GetOldFileName(CString FileName, CString &ExtendName)
{
	int Where;
	Where=FileName.ReverseFind('\\');
	if(Where==-1)
	{
		Where=FileName.ReverseFind('/');
	}
	CString FileTitle=FileName.Right(FileName.GetLength()-1-Where);
	int Which=FileTitle.ReverseFind('.');
	ExtendName=FileTitle.Right(FileTitle.GetLength()-Which-1);
	if (Which!=-1)
	{
		FileTitle=FileTitle.Left(Which);
	}
	return FileTitle;	
}

//取得文件路径
CString GetOldFilePath(CString FileName)
{
	int Where;
	Where=FileName.ReverseFind('\\');
	if(Where==-1)
	{
		Where=FileName.ReverseFind('/');
	}
	CString FileTitlePath=FileName.Left(Where);
	
	return FileTitlePath;	
}

/*
//解析文件PG指令，生成新的文件装入自动绘图目录
CString NewFileReentrant(CPlotItem *pPlotItem)
{
	CFile file;
	CFile fileNew;
	CString strFileName, sNewFileName, sExtName;
	long cNum=0;
	long nLength, iReadCout;
	char cHavePG;
	
	strFileName=pPlotItem->m_strPlotName;
	//20100406新增CFile::shareExclusive属性，避免其他CAD文件写自动绘图目录时写入不成功或打印缺页
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return strFileName;	

	nLength=file.GetLength();
	sNewFileName = GetOldFileName(strFileName, sExtName);
	if(sNewFileName[sNewFileName.GetLength()-1] == '_')
	{
		file.Close();
		return strFileName;
	}
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString strWorkPath2, sFirstName;
	char cReadChr[2];

	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);

	cNum += 1;
	strWorkPath2.Format(_T("%s%sPlot\\%s%d_.%s"),szDrive,szDir,sNewFileName,cNum,sExtName);
	iReadCout = 0;
	sFirstName = strWorkPath2;
	cHavePG = 0;

	while(iReadCout < nLength)
	{
		if( !fileNew.Open(strWorkPath2 , CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
			return sFirstName;

		while(iReadCout < nLength)
		{
			file.Read(&cReadChr[0], 1); iReadCout++;
			if(cReadChr[0] == 'P')
			{
				file.Read(&cReadChr[1], 1);iReadCout++;
				if(cReadChr[1] == 'G')
				{
					file.Read(&cReadChr[0], 1);iReadCout++;
					while(cReadChr[0] != ';')
					{	file.Read(&cReadChr[0], 1);iReadCout++;}
					cHavePG = 1;
					//fileNew.Close();
					break;
				}
				else
				{
					fileNew.Write(&cReadChr[0], 1);
					fileNew.Write(&cReadChr[1], 1);
				}
			}
			else
			{
				fileNew.Write(&cReadChr[0], 1);	
			}
		}
		fileNew.Close();
		if(cHavePG==1)
		{
			cNum += 1;
			strWorkPath2.Format(_T("%s%sPlot\\%s%d_.%s"),szDrive,szDir,sNewFileName,cNum,sExtName);

		}
	}
	file.Close();

	if(cHavePG==0)
	{
		DeleteFile(sFirstName);
		g_iNewFileSplitNum = 0;
		return strFileName;
	}
	else
	{
		strWorkPath2.Format(_T("%s%sPlot\\%s%d_.%s"),szDrive,szDir,sNewFileName,2,sExtName);
		if( !fileNew.Open(strWorkPath2 , CFile::modeRead))
		{
			DeleteFile(sFirstName);
			g_iNewFileSplitNum = 0;	
			return strFileName;
		}
		else
		{
			fileNew.Close();
			strWorkPath2.Format(_T("%s%sPlot\\%s.%s"),szDrive,szDir,sNewFileName,sExtName);
			if(strcmp(strFileName, strWorkPath2) == 0)
				DeleteFile(strFileName);
			g_iNewFileSplitNum = cNum;	
		}
	}

	//g_iNewFileSplitNum = cNum;		//20140530
	
	//DeleteFile(strFileName);
	return sFirstName;
}
*/
/*
// 1:存在两个以上PG  0：没有PG
int QuickScanFilePG(CPlotItem *pPlotItem)
{
	CFile file;
	CString strFileName;
	long nLength, iReadCout;
	int iHavePG;
	char cReadChr[2];
	
	strFileName=pPlotItem->m_strPlotName;
	iHavePG=0;
	if(file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
	{
		nLength=file.GetLength();
		iReadCout = 0;
		while(iReadCout < nLength)
		{
			file.Read(&cReadChr[0], 1); 
			iReadCout++;
			if(cReadChr[0] == 'P')
			{
				file.Read(&cReadChr[1], 1);
				iReadCout++;
				if(cReadChr[1] == 'G')
				{
					iHavePG++;
				}
			}
			if(iHavePG > 1)
				break;
		}
		file.Close();
		if(iHavePG > 1)
			return 1;
	}
	
	return 0;
}
*/
/*
//将长文件按纸张长度插入ＰＧ指令
CString ScanFileSection(CPlotItem *pPlotItem)
{
	CFile file;
	CFile ptrCF[25];
	CString strFileName, sNewFileName, sExtName, strFirstFileName;
	long cNum=0;
	long nFileToalLength, iFCur;
	char cGlobalCMD[100], cGCount;
	char *bufSource;
	long lWrStarPost, lWrEndPost;

	int  iToalFileNum;
	char cGetXPost[20], cigp;
	char cNewBuff[3096];
	long lGetXlen;
	long lToalSecHPGLLen;
	char cOdd;
	int iMinX, iMinY, iMaxX, iMaxY;
	char cMia[4][10], cmix, cmiy;

	//读源文件，将文件数据读到内存
	strFileName=pPlotItem->m_strPlotName;
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return strFileName;	
	
	nFileToalLength=file.GetLength();
	bufSource = (char *)malloc(nFileToalLength);
	file.Read(bufSource, nFileToalLength);
	file.Close();
	
	//计算拆分文件数
	iToalFileNum = pPlotItem->m_lPictureLength / (long)gl_dPaperLength;
	if((pPlotItem->m_lPictureLength % (long)gl_dPaperLength) != 0)
		iToalFileNum += 1;

	//取得文件名
	sNewFileName = GetOldFileName(strFileName, sExtName);

	//建立文件
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString strWorkPath2, sFirstName;
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);

	strWorkPath2.Format(_T("%s%sPlot\\%s%d_.%s"),szDrive,szDir,sNewFileName,0,sExtName);
	strFirstFileName = strWorkPath2;

	while(cNum<iToalFileNum)
	{
		strWorkPath2.Format(_T("%s%sPlot\\%s%d_.%s"),szDrive,szDir,sNewFileName,cNum,sExtName);
	
		if(!ptrCF[cNum].Open(strWorkPath2 , CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
		{
			AfxMessageBox("文件创建失败！");
			return strFirstFileName;
		}
		
		cNum += 1;
	}
	
	//************************
	lToalSecHPGLLen = (long)(gl_dPaperLength * 40.0);
	iFCur = 0;
	cGCount = 0;
	lWrStarPost = 0;
	lWrEndPost = 0;
	iMinX = 0;
	iMinY = 0;
	iMaxX = 0;
	iMaxY = 0;
	cmix = 0;
	cmiy = 0;

	while(iFCur < nFileToalLength)
	{
		if(isalpha(bufSource[iFCur]))
		{
			switch(toupper(bufSource[iFCur]))
			{
				case 'I':
					if(bufSource[iFCur+1] == 'N') 
					{
						cGCount = 0;
						while(bufSource[iFCur] != ';')
						{
							cGlobalCMD[cGCount++] = bufSource[iFCur++];
						}
						cGlobalCMD[cGCount++] = bufSource[iFCur++];
						
						cNum = 0;
						while(cNum<iToalFileNum)
							ptrCF[cNum++].Write(cGlobalCMD, cGCount);	
						cNum = 0;
						continue;
					}
					else if(bufSource[iFCur+1] == 'P')
					{
						cmix = 0;
						cmiy = 0;
						cGCount = 0;
						cGlobalCMD[cGCount++] = bufSource[iFCur++];
						cGlobalCMD[cGCount++] = bufSource[iFCur++];
						while(bufSource[iFCur] != ';')
						{
							if(IsDigit(bufSource[iFCur]))
							{
								cMia[cmix][cmiy++] = bufSource[iFCur];
								cGlobalCMD[cGCount++] = bufSource[iFCur];
							}
							else
							{
								cGlobalCMD[cGCount++] = bufSource[iFCur];
								cMia[cmix][cmiy] = '\0';
								cmix++;
								cmiy=0;
							}
							iFCur++;
						}
						cGlobalCMD[cGCount++] = bufSource[iFCur++];

						iMaxX = atoi(&cMia[2][0]);
						iMaxY = atoi(&cMia[3][0]);
						lToalSecHPGLLen = (long)((double)lToalSecHPGLLen / ((double)iMaxX/1000.0));

						cNum = 0;
						while(cNum<iToalFileNum)
							ptrCF[cNum++].Write(cGlobalCMD, cGCount);	
						cNum = 0;
						continue;
					}
				break;
				
				case 'S':
					if(bufSource[iFCur+1] == 'P' || bufSource[iFCur+1] == 'C' || bufSource[iFCur+1] == 'I')
					{
						cGCount = 0;
						while(bufSource[iFCur] != ';')
						{
							cGlobalCMD[cGCount++] = bufSource[iFCur++];
						}
						cGlobalCMD[cGCount++] = bufSource[iFCur++];
						
						cNum = 0;
						while(cNum<iToalFileNum)
							ptrCF[cNum++].Write(cGlobalCMD, cGCount);	
						cNum = 0;
						continue;	
					}
				break;

				case 'L':
					if(bufSource[iFCur+1] == 'B')
					{
						lWrStarPost = iFCur;
						lWrEndPost = iFCur;
						while(bufSource[iFCur] != ';')
						{
							lWrEndPost++;
							iFCur++;
							if((bufSource[iFCur] == 0x0a) || (bufSource[iFCur] == 0x03))
								break;
						}
						lWrEndPost++;
						ptrCF[cNum].Write(&bufSource[lWrStarPost], lWrEndPost - lWrStarPost);
						iFCur++;
					}
					else
					{
						cGCount = 0;
						while(bufSource[iFCur] != ';')
						{
							cGlobalCMD[cGCount++] = bufSource[iFCur++];
						}
						cGlobalCMD[cGCount++] = bufSource[iFCur++];
							
						cNum = 0;
						while(cNum<iToalFileNum)
							ptrCF[cNum++].Write(cGlobalCMD, cGCount);	
						cNum = 0;
						continue;	
					}
				break;
				
				case 'P':
					if(bufSource[iFCur+1] == 'U' || bufSource[iFCur+1] == 'D' || bufSource[iFCur+1] == 'A')
					{
						lWrStarPost = iFCur;
						iFCur+=2;
						cigp = 0;

						if(bufSource[iFCur+1] == ',')
							iFCur++;
						while(bufSource[iFCur] != ';')
						{
							if(IsDigit(bufSource[iFCur]))
							{
								cGetXPost[cigp++] = bufSource[iFCur];
							}
							iFCur++;
							if(bufSource[iFCur] == ',')
							{
								cGetXPost[cigp] = '\0';
								break;
							}
						}

						if(cigp == 0)
						{//PU;
							iFCur = lWrStarPost;
							lWrStarPost = iFCur;
							lWrEndPost = iFCur;
							while(bufSource[iFCur++] != ';')
								lWrEndPost++;
							if(bufSource[iFCur] == 0x0d)
							{
								lWrEndPost++;
								if(bufSource[iFCur+1] == 0x0a)
								{
									lWrEndPost++;
									iFCur += 2;
								}
							}
							lWrEndPost++;
							ptrCF[cNum].Write(&bufSource[lWrStarPost], lWrEndPost - lWrStarPost);
							continue;
						}	
						else
							lGetXlen = atoi(cGetXPost);

						if(lGetXlen > lToalSecHPGLLen)
						{
							cNum =  lGetXlen/lToalSecHPGLLen;
						}
						else
							cNum = 0;
						
						if(cNum != 0)
						{
							iFCur = lWrStarPost;
							cOdd = 1;
							lWrStarPost = 0;
							cNewBuff[lWrStarPost++] = bufSource[iFCur++]; 
							cNewBuff[lWrStarPost++] = bufSource[iFCur++];
							while(bufSource[iFCur] != ';')
							{
								if((cOdd%2) != 0)
								{
									cigp = 0;
									while(bufSource[iFCur] != ',')
									{
										if(IsDigit(bufSource[iFCur]))
										{
											cGetXPost[cigp++] = bufSource[iFCur];
										}
										cGetXPost[cigp] = '\0';
										iFCur++;
									}
									
									lGetXlen = atoi(cGetXPost);
									lGetXlen -= (lToalSecHPGLLen*cNum);
									itoa(lGetXlen, cGetXPost, 10);
									cigp = 0;
									while(cGetXPost[cigp++] != '\0');
									strncpy(&cNewBuff[lWrStarPost], &cGetXPost[0],cigp);
									lWrStarPost += (cigp-1);
									cNewBuff[lWrStarPost++] = ','; 
								}
								else
								{
									while(bufSource[iFCur] != ',' && bufSource[iFCur] != ';')
									{
										cNewBuff[lWrStarPost++] = bufSource[iFCur++]; 
									}
									cNewBuff[lWrStarPost++] = bufSource[iFCur]; 
									if(bufSource[iFCur] == ';')
										break;
								}
								iFCur++;
								cOdd++;
							}
							ptrCF[cNum].Write(cNewBuff, lWrStarPost);
						}
						else
						{
							iFCur = lWrStarPost;
							lWrStarPost = iFCur;
							lWrEndPost = iFCur;
							while(bufSource[iFCur++] != ';')
								lWrEndPost++;
							lWrEndPost++;
							ptrCF[cNum].Write(&bufSource[lWrStarPost], lWrEndPost - lWrStarPost);
						}			
					}
				break;
				
				default:
					lWrStarPost = iFCur;
					lWrEndPost = iFCur;
					while(bufSource[iFCur] != ';')
					{
						lWrEndPost++;
						iFCur++;
						if((bufSource[iFCur] == 0x0a) || (bufSource[iFCur] == 0x03))
							break;
					}
					lWrEndPost++;
					ptrCF[cNum].Write(&bufSource[lWrStarPost], lWrEndPost - lWrStarPost);
					iFCur++;
				break;
			}//SWITCH
		}//IF
		else
			iFCur++;
	}//WHILE

	//关闭文件
	cNum = 0;
	while(cNum<iToalFileNum)
		ptrCF[cNum++].Close();

	return strFirstFileName;
}
*/
//返回0打开文件成功,1打开文件失败,2文件格式错,3图形超宽
int CreateList(CPlotItem *pPlotItem,int nCommunicateMode,SOCKET sock)
{
	//************分页工作
	int nReturnValue;
//	CString strFileName;

/*	if((QuickScanFilePG(pPlotItem) == 0) && (pPlotItem->m_lPictureLength > gl_dPaperLength))
	{//没有PG且文件长度大于1.5m的格博文件
		strFileName = ScanFileSection(pPlotItem);
		pPlotItem->m_strPlotName = strFileName;
	}
	else
	{
		//20140530
		if(g_iNewFileSplitNum == 0)
		{
			strFileName = NewFileReentrant(pPlotItem);
			pPlotItem->m_strPlotName = strFileName;
		}
		else
		{
			strFileName=pPlotItem->m_strPlotName;
			g_iNewFileSplitNum--;
		}
	}*/
	nReturnValue = DissecretFileList(pPlotItem, nCommunicateMode, sock);
	//***************************
	if (nReturnValue != 0)
		return nReturnValue;

	long lMaxPlotLength;
	long lMaxWidthLength;

	if(g_nRotate==1)
	{//图形正向旋转90度
		lMaxWidthLength=g_lMaxPlotLength;
		g_lMaxPlotLength=g_lMaxPlotWidth;
		g_lMaxPlotWidth=lMaxWidthLength;
	}

	if (g_lMaxPlotWidth <= pPlotItem->m_lPaperWidth)
	{
		g_nSplitNum = 1;
		//lMaxWidthLength = ftoi(g_lMaxPlotWidth * 0.025);
		lMaxWidthLength = ftoi(pPlotItem->m_lMaxPlotWidth * 0.025); //20140813
		lMaxPlotLength = ftoi(g_lMaxPlotLength * 0.025);
		if (lMaxPlotLength != pPlotItem->m_lPictureLength)
			pPlotItem->m_lPictureLength = lMaxPlotLength;
		if (lMaxWidthLength != pPlotItem->m_lPictureWidth)
			pPlotItem->m_lPictureWidth = lMaxWidthLength;
		//g_lMaxPlotWidth = long((g_lMaxPlotWidth * g_dScaleY * HpglPixelY + g_nLineWidth + 15) / 16); 
		g_lMaxPlotWidth = long((pPlotItem->m_lMaxPlotWidth * g_dScaleY * HpglPixelY + g_nLineWidth + 15 ) / 16) ;//20140813
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
	lMaxWidthLength = g_lMaxPlotWidth + long((pPlotItem->m_dGapRight * MMPixelY + 7) / 8);

	//发送图形宽度（字节数）
	char SendBuf[100];
	BYTE *pBits1;
	unsigned long mLength;
	//gzr20161109
	//return 0;
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
/*		
		//pBits1 = (BYTE *)&lMaxWidthLength;
		pBits1 = (BYTE *)&PictureWidth;
		g_lCurrMaxWidthLength = lMaxWidthLength;//20100907
		SendBuf[10] = *(pBits1+3);
		SendBuf[11] = *(pBits1+2);
		SendBuf[12] = *(pBits1+1);
		SendBuf[13] = *pBits1;
*/

		pBits1 = (BYTE *)&lMaxPlotLength;
		SendBuf[14] = *(pBits1+3);
		SendBuf[15] = *(pBits1+2);
		SendBuf[16] = *(pBits1+1);
		SendBuf[17] = *pBits1;
		/*
		pBits1 = (BYTE *)&pPlotItem->m_lPictureLength;
		SendBuf[19] = *(pBits1+3);
		SendBuf[20] = *(pBits1+2);
		SendBuf[21] = *(pBits1+1);
		SendBuf[22] = *pBits1;	
		*/
		if (nCommunicateMode == 0)
		{
			mLength = 18;
			if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
			{
				mLength = 64;
				CH375ReadData(0,SendBuf,&mLength);
			}
		}
		else
		{
			send(sock,SendBuf,18,0);
			recv(sock,SendBuf,100,0);
		}
		if (memcmp(SendBuf,"Msg_",4) == 0)
		{
			if ((SendBuf[5] == 1) && (SendBuf[7] == 3))
				return 3;
			else if ((SendBuf[5] == 1) && (SendBuf[7] == 9))
				break;
		}
		Sleep(10);
	}
	//SendCutDataFile(0);//20100907
	return 0;
}

//文件数据解析，喷墨数据和切割数据分离
//
void  FileParse(CString strFileName)//20100907
{
	CFile fileSource, filePlt, fileCut;
	char *bufSource;
	char *bufPlt, *bufCut;
	char cOldCompStr[30],cNewCompStr[30], cOldPCmd[5], cNewPCmd[5];
//	char cOldXPostion[10], cOldYPostion[10], cNewXPostion[10], cNewYPostion[10];
//	CArray<unsigned char, unsigned char> bufPlt, bufCut;
	long iS, iP, inC, iK, iQ, iResult, iOffset, it;
	bool  isCutData = false; // false:plt   true:cut
	bool  bHaveCutCmd = false, bHavePenCmd = false, bIsCmpStr = false, bIsPDCmd = false;
	char  cOldLT,cNewLT;
	
	cOldLT = 0;
	cNewLT = 0;
	iK = 0;
	iQ = 0;
	iResult = 0;
	iOffset = 0;
	//g_cTerminator=theApp.m_cTerminator;//标志终结符20150212rwt
	
	if(!fileSource.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return ;
#if _DEBUG
	CString strWorkPath1(PLTDATAPATH);
	CString strWorkPath2(CUTDATAPATH);
#else
	TCHAR szFullPath[_MAX_PATH];
  	TCHAR szDir[_MAX_DIR];
  	TCHAR szDrive[_MAX_DRIVE];
 	CString strWorkPath1, strWorkPath2;
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
	strWorkPath1 += "Hidpt.chv";
	strWorkPath2.Format(_T("%s%s"),szDrive,szDir);
	strWorkPath2 += "Cutpt.chv";
#endif	

	long nLength=fileSource.GetLength();

	if (nLength != 0)
	{
		if( !filePlt.Open(strWorkPath1 , CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
			return ;
		if( !fileCut.Open(strWorkPath2 , CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
			return ;		
		
		bufSource = (char *)malloc(nLength);
		bufCut = (char *)malloc(nLength);
		bufPlt = (char *)malloc(nLength);
		fileSource.Read(bufSource, nLength);
		
		//20140507
		if(bufSource[0] != 'I' && bufSource[1] != 'N')
		{
			strcpy(bufCut, "IN;");
			strcpy(bufPlt, "IN;");
			iP = 3;
			inC = 3;
		}
		else
		{
			
			iP = 0;
			inC = 0;
		}

		iS = 0;
		for(long k=0; k<nLength; k++)
		{			
			if(isalpha(bufSource[k]))
			switch(toupper(bufSource[k]))
			{
				case 'C':
					if(toupper(bufSource[k+1]) == 'T')
					{
						while(bufSource[k] != ';')
						{
							bufCut[inC++] = bufSource[k++];
							//20140507
							if(bufSource[k+1] == 0x0a)
							{	
								bufSource[++k] = ';';
								break;
							}
							//**********
						}
						bufCut[inC++] = bufSource[k];
						continue;
					}
				break;
				
				case 'D':
					if(toupper(bufSource[k+1]) == 'F')
					{
						while(bufSource[k] != ';')
						{
							bufPlt[iP++] = bufSource[k++];
							//20140507
							if(bufSource[k+1] == 0x0a)
							{	
								bufSource[++k] = ';';
								break;
							}
							//**********
						}
						bufPlt[iP++] = bufSource[k];
						continue;
					}
				break;
				
				case 'I':
					if(toupper(bufSource[k+1]) == 'N')
					{
						while(bufSource[k] != ';')
						{
							bufPlt[iP++] = bufSource[k];
							bufCut[inC++] = bufSource[k];
							k++;
							//20140507
							if(bufSource[k] == 0x0a)
							{	
								bufSource[k] = ';';
								break;
							}
							//**********
						}							
						bufPlt[iP++] = bufSource[k];
						bufCut[inC++] = bufSource[k];
						continue;
					}// 20140605解决格博问题
					else if(toupper(bufSource[k+1]) == 'P')
					{
						while(bufSource[k++] != ';');
						/*{//解决
							bufPlt[iP++] = bufSource[k++];
						}
						bufPlt[iP++] = bufSource[k++];*/

						//if(bufSource[k] == 0x0d)
						//	k++;
						//if(bufSource[k] == 0x0a)
						//	k++;
					}
				break;
					
				case 'S':
					if(toupper(bufSource[k+1]) == 'P')
					{
						if((bufSource[k+2]-0x30) == 0)
						{
							iK = k;
							bufPlt[iP++] = bufSource[iK++];
							bufPlt[iP++] = bufSource[iK++];
							bufPlt[iP++] = bufSource[iK++];
							//20140507
							if(bufSource[iK] == 0x0a)
							{	
								bufPlt[iP++] = ';';
							}
							else
								bufPlt[iP++] = bufSource[iK];
							//************

							bufCut[inC++] = bufSource[k++];
							bufCut[inC++] = bufSource[k++];
							bufCut[inC++] = bufSource[k++];
							//20140507 Assyst CAD 输出
							if(bufSource[k] == 0x0a)
							{	
								bufSource[k] = ';';
							}
							//*********
							bufCut[inC++] = bufSource[k];
							continue;
						}
						else if((bufSource[k+2]-0x30) == g_cPenNo)//&&((bufSource[k+2]-0x30) != g_cCutNo))	
						{//喷墨数据
								bufPlt[iP++] = bufSource[k++];
								bufPlt[iP++] = bufSource[k++];
								bufPlt[iP++] = bufSource[k++];
								//20140507
								if(bufSource[k] == 0x0a)
									bufSource[k] = ';';
								//******
								bufPlt[iP++] = bufSource[k];
								isCutData = false;
						}
						else 
						{//切割数据
							if(bHaveCutCmd == false)
							{
								bufCut[inC++] = bufSource[k++];
								bufCut[inC++] = bufSource[k++];
								bufCut[inC++] = bufSource[k++];
								//20140507
								if(bufSource[k] == 0x0a)
									bufSource[k] = ';';
								//******
								bufCut[inC++] = bufSource[k];
								bHaveCutCmd = true;
								bIsPDCmd = false;
							}
							else
							{
								if((bufSource[k+2]) != ';')
									k+=3;
								else
									k+=2;
							}
							isCutData = true;
						}
						
						if(bIsPDCmd)
						{
							if(isCutData)
								bufCut[(--inC)++] = ',';	
							bIsPDCmd = false;
						}
						continue;
					}
					else if(toupper(bufSource[k+1]) == 'C')
					{
						while(bufSource[k++] != ';');
					}
					break;

					case 'L':
					if(toupper(bufSource[k+1]) == 'T')
					{
						if(isCutData)
						{
							if(bufSource[k+2] != ';')
							{
								cNewLT = bufSource[k+2];
							}
							else
							{
								cNewLT = 10;
							}
							
							if(cOldLT != cNewLT)
							{
								cOldLT = cNewLT;	
								//如果在新的LT之前是,号，则应修改成;
								if(bufCut[inC-1] == ',')
									bufCut[inC-1] = ';';

								bufCut[inC++] = bufSource[k++];
								bufCut[inC++] = bufSource[k++];
								if(bufSource[k] > '0' && bufSource[k] < '9')
								{
									bufCut[inC++] = bufSource[k++];
									bufCut[inC++] = bufSource[k];
								}
								else
									bufCut[inC++] = bufSource[k];
								continue;
							}
						}
						else{
								bufPlt[iP++] = bufSource[k++];
								bufPlt[iP++] = bufSource[k++];
								if(bufSource[k] > '0' && bufSource[k] < '9')
								{
									bufPlt[iP++] = bufSource[k++];
									bufPlt[iP++] = bufSource[k];
								}
								else
									bufPlt[iP++] = bufSource[k];
								continue;
							}
					
						if(bufSource[k+2] != ';')
							k+=3;
						else
							k+=2;
						if(bIsPDCmd)
						{
							if(isCutData)
								bufCut[(--inC)++] = ',';	
							bIsPDCmd = false;
						}
						continue;
					}
					//20150211rwt LB未处理
					else if ((toupper(bufSource[k+1]) == 'B'))
					{
						while (bufSource[k] != ';')
						{
							bufPlt[iP++] = bufSource[k++];
						}
						bufPlt[iP++] = bufSource[k];
						continue;
					}
					else if ((toupper(bufSource[k+1]) == 'O'))
					{
						while (bufSource[k] != ';')
						{
							bufPlt[iP++] = bufSource[k++];
						}
						bufPlt[iP++] = bufSource[k];
						continue;
					}
					
					break;

					case 'P':
						if((toupper(bufSource[k+1]) == 'U')||(toupper(bufSource[k+1]) == 'D')||(toupper(bufSource[k+1]) == 'A'))
						{			
							if(!isCutData)
							{
								while(bufSource[k] != ';')
								{
									//土耳其Investronica软件输出20140703
									//?

									bufPlt[iP++] = bufSource[k++];
									//20140507
									if(bufSource[k] == 0x0a)
									{
										bufSource[k] = ';';
										break;
									}
									//******
								}
								bufPlt[iP++] = bufSource[k];
								continue;
							}
							//20140507
							if(toupper(bufSource[k+2]) == ';' || toupper(bufSource[k+2]) == 0x0a)
							{
								if(bufSource[k+3]=='P' && bufSource[k+4]=='A')
								{
									// 20141023 解决至尊宝坊软件输出PLT文件解析问题
									if((toupper(bufSource[k+1]) == 'U') && (bufSource[k+2] == ';'))
									{
										bufSource[k+4]='U';
										k += 3;
									}
									/* 20141023 注释
									bufSource[k++]=';';
									bufSource[k++]=';';
									bufSource[k++]=';';
									
									iQ=k;
									while(1)
									{
										if(bufSource[k] == 'P' && bufSource[k+1] == 'A')
										{
											bufSource[k++] = ';';
											bufSource[k++] = 'P';
											bufSource[k++] = 'D';
										}
										else if(bufSource[k] == 'P' && bufSource[k+1] == 'U')
											break;
										
										if(k++>nLength)
											break;
									}
									k=iQ+1;
									*/
								}
								else
								{
									k += 2;
									continue;
								}
							}
							
							bIsPDCmd = true;	
							iQ = k;
							cNewPCmd[0] = bufSource[iQ++];
							cNewPCmd[1] = bufSource[iQ];
							cNewPCmd[2] = '\0';
							iResult = strcmp(cNewPCmd, cOldPCmd);
							if(iResult == 0)
								iOffset = 2;

										if(iOffset != 0)//绘图命令错HHY
									    {
			 								bufCut[inC-1] = ',';
										}
							else
							{
								// 20120723
								if(isCutData)
								{
									it = inC-1;
									if (bufCut[it] == ',')
										bufCut[it] = ';';
								}
								
								strcpy(cOldPCmd, cNewPCmd);
								iOffset = 0;
							}
							
							iK = 0;
							iQ = k+2;
							if(bufSource[iQ] == ',')
								iQ++;

							for(int itc=0; itc < 2; itc++)
							{
								while((bufSource[iQ] != ',') && (bufSource[iQ] != ';'))
								{//土耳其Investronica软件输出20140703
									if((bufSource[iQ] == 'P')||(bufSource[iQ+1] == 'D'))
										break;
									if(bufSource[iQ] == '-')
										iQ++;
									//*******************
									cNewCompStr[iK++] = bufSource[iQ++];
									//20140507
									if(bufSource[iQ] == 0x0a)
									{
										break;
									}
									//******
								}
								iQ++;
							}
							
							cNewCompStr[iK] = '\0';
							if(bIsCmpStr == false)
							{
								strcpy(cOldCompStr, cNewCompStr);
								bIsCmpStr = true;
									
								if(isCutData)
								{
									while(bufSource[k] != ';')
									{//土耳其Investronica软件输出20140703
										if((bufSource[k-1] == 'U')&&(bufSource[k] == ','))
											k++;
										if(cNewPCmd[0] == 'P' && cNewPCmd[1] == 'U')
										if(bufSource[k]=='P' && bufSource[k+1]=='D')
										{	
											k--;
											bufCut[inC++] = ';';
											break;
										}
										if(bufSource[k] == '-')
											k++;
									//*************************

										bufCut[inC++] = bufSource[k++];
										//20140507
										if(bufSource[k] == 0x0a)
										{
											break;
										}
										//******
									}
								}
							
							}
							else
							{
								iResult = strcmp(cOldCompStr, cNewCompStr);
								if((iResult != 0) || (iOffset == 0))
								{ // 是新的数据
									strcpy(cOldCompStr, cNewCompStr);
									if(iOffset != 0)
										k += iOffset;										
							
									if(isCutData)
									{
										if(iOffset == 0)
										{
											bufCut[inC-1] = ';';
										}
										
										iS = inC;
										while(bufSource[k] != ';')
										{//土耳其Investronica软件输出20140703
											if((bufSource[k-1] == 'U')&&(bufSource[k] == ','))
												k++;
											if(cNewPCmd[0] == 'P' && cNewPCmd[1] == 'U')

											if(bufSource[k]=='P' && bufSource[k+1]=='D')
											{	
												k--;
												bufCut[inC++] = ';';
												break;
											}
											if(bufSource[k] == '-')
												k++;
											//*************************

											bufCut[inC++] = bufSource[k++];
											//20140507
											if(bufSource[k] == 0x0a)
											{
												//bufSource[k] = ';';
												break;
											}
											//******
										}
										
										// 判断下一个命令是否同前一个命令一致
										if(bufSource[k+1] == 0x0a)
										{//OPTITEX GARMENT CAD输出
											iQ = k+2; 
										}
										else if((bufSource[k+1] == 0x0d)&&(bufSource[k+2] == 0x0a))
										{//Gemini CAD
											iQ = k+3; 
										}
										else
											iQ = k+1;

										if(bufSource[iQ] == ';')
											iQ++;
										k=iQ-1;
										cNewPCmd[0] = bufSource[iQ++];
										cNewPCmd[1] = bufSource[iQ];
										cNewPCmd[2] = '\0';
										iResult = strcmp(cNewPCmd, cOldPCmd);
										if(iResult == 0)
											iOffset = 2;	
										else
											iOffset = 0;

										if(iOffset != 0)
									    {
			 								bufCut[inC++] = ',';
											
											continue;
										}
									}
									
								}
								else
								{	// 数据一致
									k += (iK+3);
									continue;
								}
									
							}
							
						}
						else if(toupper(bufSource[k+1]) == 'A')
						{
							while(bufSource[k] != ';')
							{
								bufPlt[iP++] = bufSource[k++];
								//20140507
								if(bufSource[k] == 0x0a)
								{
									bufSource[k] = ';';
									break;
								}
								//******
							}
							bufPlt[iP++] = bufSource[k];
							continue;
						}
						else if(toupper(bufSource[k+1]) == 'G')
						{
							while(bufSource[k] != ';')
							{
								bufCut[inC++] = bufSource[k++];
							}
							bufCut[inC++] = bufSource[k];
							continue;
						}
						else
						{
							
						}
					break;

					/*
					//201502060902 rwt 自动裁纸优化ShutDownOverCutOffPaper
					case 'A':
						if((toupper(bufSource[k+1]) == 'U')&&(toupper(bufSource[k+3]) == 'O')
							&&(toupper(bufSource[k+5]) == 'U')&&(toupper(bufSource[k+8]) == 'F'))
						{
							while(bufSource[k] != ';')
							{
								bufCut[inC++] = bufSource[k++];
							}
							bufCut[inC++] = bufSource[k];
							continue;
						}
						else
						{
							while(bufSource[k] != ';')
							{
								bufPlt[iP++] = bufSource[k++];
								//20140507
								if(bufSource[k] == 0x0a)
								{
									bufSource[k] = ';';
									break;
								}
								//******
							}
							bufPlt[iP++] = bufSource[k];
						}
						break;
						*/
						
					default:
						while(bufSource[k] != ';')
						{
							bufPlt[iP++] = bufSource[k++];
							//20140507
							if(bufSource[k] == 0x0a)
							{
								bufSource[k] = ';';
								break;
							}
							//******
						}
						bufPlt[iP++] = bufSource[k];
						continue;
					break;
				}
				
				if(isCutData)
				{
					if((bufSource[k] == 0x0a)&&iOffset!=0)
						continue;
					else if((bufSource[k] == 0x0d)&&(bufSource[k+1] == 0x0a))
						k++;
					else if (bufSource[k] == 0x0a)
					{
						if(bufCut[inC-1] != ';')
							bufCut[inC++] = ';';
					}
					else
					{
						if(bufCut[inC-1] != ';')
							bufCut[inC++] = bufSource[k];
					}
				}
				else
				{
					bufPlt[iP++] = bufSource[k];
				}
				
			}
			
			if(inC!=0)
			{
				bufCut[inC++] = 'P';
				bufCut[inC++] = 'U';
				bufCut[inC++] = '0';
				bufCut[inC++] = ',';
				bufCut[inC++] = '0';
				bufCut[inC++] = ';';
				fileCut.Write(&bufCut[0], inC);
				inC = 0;
			}
			
			if(iP!=0)
			{
				filePlt.Write(&bufPlt[0], iP);
				iP = 0;
			}
			
		fileSource.Close();
		filePlt.Close();
		fileCut.Close();
		free(bufSource);
		free(bufCut);
		free(bufPlt);
		
	}
	else
	{
		fileSource.Close();
	}
}

int AnalysisPltFile(CString strFileName);

//gzr20161029
CString ReadFileContent(CString strFileName)
{
	CString fileContent;
	char *	pBuf = new	char[64*1024 + 1];
	if (pBuf == NULL)
		return "";
	TRY
	{
		CFile file(strFileName, CFile::modeRead | CFile::typeBinary);
		UINT	n = file.GetLength();
		UINT	i, j;
		i = 0;
		j = (n - i) < 64 * 1024 ? (n - i) : 64 * 1024;
		file.SeekToBegin();
		
		while(i < n && j >0)
		{
			file.Read(pBuf, j);
			pBuf[j] = '\0';
			fileContent += pBuf;
			i += j;
			j = (n - i) < 64 * 1024 ? (n - i) : 64 * 1024;
		};
		file.Close();					// 关闭文件
	}
	CATCH( CFileException, e )
	{
#ifdef _DEBUG
		afxDump << "File could not be opened " << e->m_cause << "\n";
#endif
		//		AfxMessageBox ("File could not be opened ");
		
		//InitPara();
		delete	[]pBuf;
		return "";
	}
	END_CATCH
		delete	[]pBuf;
	return fileContent;
}
//gzr20161027
CString ReplacePen(CString strFileName,CString strSaveName)
{
	CString fileContent = ReadFileContent(strFileName);
	
	long n = fileContent.GetLength();
	if (n == 0)
		return "";
	fileContent.Replace("PG;PU0,0;","");

	//fileContent.Delete(fileContent.Find("PG;PU0,0;"),9);
	fileContent.Replace("SP2;","SP1;");
	fileContent.Replace("SP3;","SP1;");
	fileContent.Replace("SP4;","SP1;");
	fileContent.Replace("SP5;","SP1;");
	fileContent.Replace("SP6;","SP1;");
	fileContent.Replace("SP7;","SP1;");
	fileContent.Replace("SP8;","SP1;");

	CFile filePlt;
	if( !filePlt.Open(strSaveName , CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
		return "";
	n = fileContent.GetLength();
	filePlt.Write(fileContent.GetBuffer(n),n);
	filePlt.Close();
	return "";
}

CString CombinCutFile(CString strFileName,CString strSaveName)
{
	CString fileContent = ReadFileContent(strFileName);
	CString fileContent1 = ReadFileContent(strSaveName);
	
	long n = fileContent.GetLength();
	if (n == 0)
		return "";
	fileContent.Replace("IN;","PU0,0;");
	//fileContent.Replace("PG;","PU;");
	fileContent.Replace("SP2;","");
	fileContent.Replace("SP3;","");
	fileContent.Replace("SP4;","");
	fileContent.Replace("SP5;","");
	fileContent.Replace("SP6;","");
	fileContent.Replace("SP7;","");
	fileContent.Replace("SP8;","");
	//fileContent1.Replace(".;PU;SP1;",".");
	CFile filePlt;
	if( !filePlt.Open(strSaveName , CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
		return "";
	fileContent1 += fileContent;
	n = fileContent1.GetLength();
	
	filePlt.Write(fileContent1.GetBuffer(n),n);
	filePlt.Close();
	return "";
}
//非加密文件链表生成
int DissecretFileList(CPlotItem *pPlotItem, int nCommunicateMode, SOCKET sock)
{
	int reValue;
		// 打印
	if ( g_cPrintCutSelect == 1 )//20160113只打印文件不分段
	{
		//20130926 打印图形乱
		g_nRotate=pPlotItem->m_nRotate;
		InitTextChangeLine();
		InitHPGL();
		g_lFixedPointLength=pPlotItem->m_lFixedPointLength*40;
		g_lInitMaxPlotLength=pPlotItem->m_lMaxPlotLength;
		g_lInitMaxPlotWidth=pPlotItem->m_lMaxPlotWidth;
		g_lMaxPlotLength-=pPlotItem->m_lMinPlotLength;

		g_lMinPlotWidth=pPlotItem->m_lMinPlotWidth;
	#if _DEBUG
		CString strWorkPath1(PLTDATAPATH);
	#else
		TCHAR szFullPath[_MAX_PATH];
	  	TCHAR szDir[_MAX_DIR];
	  	TCHAR szDrive[_MAX_DRIVE];
	 	CString strWorkPath1;
		::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
		_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
		strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
		strWorkPath1 += "Hidpt.chv";
	#endif		
		reValue = AnalysisPltFile( pPlotItem->m_strPlotName );
		if(reValue != 0)
			return reValue;
	}
	else
	{
		FileParse(pPlotItem->m_strPlotName);
		g_cPrintCutSelect &= 0x03;
		// 20101214  打印切割选择
		if ((g_cPrintCutSelect&0x02) == 0x02)
		{	
			//20130926 打印图形乱
			g_nRotate=pPlotItem->m_nRotate;
			InitTextChangeLine();
			InitHPGL();
			g_lFixedPointLength=pPlotItem->m_lFixedPointLength*40;
			g_lInitMaxPlotLength=pPlotItem->m_lMaxPlotLength;
			g_lInitMaxPlotWidth=pPlotItem->m_lMaxPlotWidth;
			g_lMaxPlotLength-=pPlotItem->m_lMinPlotLength;
			g_lMinPlotWidth=pPlotItem->m_lMinPlotWidth;
		#if _DEBUG
			CString strWorkPath2(CUTDATAPATH);
		#else
			TCHAR szFullPath[_MAX_PATH];
	  		TCHAR szDir[_MAX_DIR];
	  		TCHAR szDrive[_MAX_DRIVE];
	 		CString strWorkPath2;
			::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
			_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
			strWorkPath2.Format(_T("%s%s"),szDrive,szDir);
			strWorkPath2 += "Cutpt.chv";
		#endif	
			reValue = AnalysisPltFile( strWorkPath2 ); //20120822 调整
			if(reValue != 0)
			{
				return reValue;
			}

			SendCutDataFile(nCommunicateMode, strWorkPath2, sock);
			RemoveGLBuffer1();
			//如果当前只选择了切割，那么返回2
			if(g_cPrintCutSelect == 0x02)
			   return 2;
		}

			// 打印
		if ( (g_cPrintCutSelect&0x01) == 0x01 )
		{
			//20130926 打印图形乱
			g_nRotate=pPlotItem->m_nRotate;
			InitTextChangeLine();
			InitHPGL();
			g_lFixedPointLength=pPlotItem->m_lFixedPointLength*40;
			g_lInitMaxPlotLength=pPlotItem->m_lMaxPlotLength;
			g_lInitMaxPlotWidth=pPlotItem->m_lMaxPlotWidth;
			g_lMaxPlotLength-=pPlotItem->m_lMinPlotLength;
			g_lMinPlotWidth=pPlotItem->m_lMinPlotWidth;
			//if((g_cPrintCutSelect&0x02) == 0x02)
			////gzr20161028test
			int needCutSize = m_NeedCutDataFiles.GetSize();
			BOOL isNeedCut = FALSE;
			for (int i = 0; i < needCutSize; i++)
			{
				CString path = m_NeedCutDataFiles.GetAt(i);
				if (path.Find(pPlotItem->m_strPlotName) != -1)
				{
					isNeedCut = TRUE;
					break;
				}
			}
			if (isNeedCut && (g_cPrintCutSelect&0x02) == 0x02)
			{
			#if _DEBUG
				CString strWorkPath1(PLTDATAPATH);
				CString strWorkPath2(CUTDATAPATH);
			#else
				TCHAR szFullPath[_MAX_PATH];
				TCHAR szDir[_MAX_DIR];
				TCHAR szDrive[_MAX_DRIVE];
				CString strWorkPath1;
				CString strWorkPath2;
				::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
				_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
				strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
				strWorkPath1 += "Hidpt.chv";
				strWorkPath2.Format(_T("%s%s"),szDrive,szDir);
				strWorkPath2 += "Cutpt.chv";
			#endif
				reValue = AnalysisPltFile( strWorkPath1 );
				if (reValue == 4)
				{
					ReplacePen(strWorkPath2,strWorkPath1);
					reValue = AnalysisPltFile( strWorkPath1 );
				}
				else if (reValue == 0)
				{
					g_nRotate=pPlotItem->m_nRotate;
					InitTextChangeLine();
					InitHPGL();
					g_lFixedPointLength=pPlotItem->m_lFixedPointLength*40;
					g_lInitMaxPlotLength=pPlotItem->m_lMaxPlotLength;
					g_lInitMaxPlotWidth=pPlotItem->m_lMaxPlotWidth;
					g_lMaxPlotLength-=pPlotItem->m_lMinPlotLength;
					g_lMinPlotWidth=pPlotItem->m_lMinPlotWidth;
					CombinCutFile(strWorkPath2,strWorkPath1);
					//ReplacePen(strWorkPath2,strWorkPath1);
					reValue = AnalysisPltFile( strWorkPath1 );
					return reValue;
				}

				if(reValue != 0)
				{
					return reValue;
				}
			}
			else
			{
			#if _DEBUG
				CString strWorkPath1(PLTDATAPATH);
				CString strWorkPath2(CUTDATAPATH);
			#else
				TCHAR szFullPath[_MAX_PATH];
				TCHAR szDir[_MAX_DIR];
				TCHAR szDrive[_MAX_DRIVE];
				CString strWorkPath1;
				CString strWorkPath2;
				::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
				_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
				strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
				strWorkPath1 += "Hidpt.chv";
				strWorkPath2.Format(_T("%s%s"),szDrive,szDir);
				strWorkPath2 += "Cutpt.chv";
			#endif
				reValue = AnalysisPltFile( strWorkPath1 );
				if (reValue == 4 && (g_cPrintCutSelect&0x02) == 0x02)
				{
					ReplacePen(strWorkPath1,strWorkPath2);
					reValue = AnalysisPltFile( strWorkPath1 );
				}

				if(reValue != 0)
				{
					return reValue;
				}
			}
		}
	}
	return reValue;
}
//20120822
int AnalysisPltFile(CString strFileName)
{
	CFile file;	
	//20100406新增CFile::shareExclusive属性，避免其他CAD文件写自动绘图目录时写入不成功或打印缺页
	if(!file.Open(strFileName,CFile::modeRead |CFile::shareExclusive ))
		return 1;

	int nLength=file.GetLength();
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
					if(IsAlpha(ch))
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
					if(IsAlpha(ch))
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

void WritePageFile(int iPageNum, BYTE *pBmpData, long lflen)
{
	CFile file;
	char strFileName[200];
	sprintf(strFileName, "c:\\A%d.data", iPageNum);
	if(!file.Open(strFileName,CFile::modeCreate | CFile::modeWrite))
	{
		return;
	}
	file.Write(pBmpData, lflen);
	file.Close();
}

//生成位图
UINT BmpGenerationThreadProc(LPVOID pParam)
{
	CPlotItem *pPlotItem;
  
	pPlotItem=((SendDataThreadInfo*)pParam)->m_pPlotItem;

	long iIndex1;
	long lSendPage;//已生成位图个数
	long lTotalPage;//图形位图总数
	HDC hdc,hdcMem;
	HBITMAP hBitMap = NULL;
	CBitmap pBitMap;
	CBitmap *pOldBitmap1;
	CDC *pMemDC = NULL;
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
	//计算右边距
	long lGrapRight = long((pPlotItem->m_dGapRight * MMPixelY + 7) / 8);

	hdc = CreateIC(TEXT("DISPLAY"),NULL,NULL,NULL);
	hdcMem = CreateCompatibleDC(hdc);
	pMemDC = new CDC;
	pMemDC->Attach(hdcMem);
	
	lTotalPage = (g_lMaxPlotLength + SECTION_WIDTH - 1) / SECTION_WIDTH;
	g_lSectionPosY = SECTION_WIDTH;
	//开辟内存块
	g_pBits1 = new BYTE[g_lMaxPlotWidth * g_lSectionPosY * 2 + lGrapRight * g_lSectionPosY];
	g_pBits2 = new BYTE[g_lMaxPlotWidth * g_lSectionPosY * 2 + lGrapRight * g_lSectionPosY];
	pBuf1 = new BYTE[g_lMaxPlotWidth * g_lSectionPosY + lGrapRight * g_lSectionPosY];
	pBuf2 = new BYTE[g_lMaxPlotWidth * g_lSectionPosY + lGrapRight * g_lSectionPosY];
	g_lBits1Count = 0;
	g_lBits2Count = 0;
	lGrapRight *= 8;

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
			//for (iIndex1=0;iIndex1<m;iIndex1++)//清空缓冲区
			//	pBuf2[iIndex1]=0;
			
			pBitMap.CreateBitmap(g_lSectionPosY,g_lMaxPlotWidth*8,1,1,pBuf1);
			pOldBitmap1=pMemDC->SelectObject(&pBitMap);
			//添加自绘图形 
			DrawBMP(pMemDC,nSplitNum,pPlotItem->m_lMarkSpace);
			
			pBitMap.GetBitmapBits(m,pBuf1);//获取点阵
			pMemDC->SelectObject(pOldBitmap1);
			pBitMap.DeleteObject();
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
				pMemDC->DeleteDC();
				delete pMemDC; 
				pMemDC = NULL;
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
				pMemDC->DeleteDC();
				delete pMemDC; 
				pMemDC = NULL;
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
	pMemDC->DeleteDC();
	delete pMemDC; 
	pMemDC = NULL;
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
				lSendLength = lSendPage * SECTION_WIDTH;
				pPlotItem->m_lSendLength = long((g_lMaxPlotLength * i + lSendLength) / g_dScaleX * 25.4 / 300);
			}
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
	RemoveGLBuffer1();
	pParent->SendMessage(WM_ENDTRANSMITFILE,0,0);
	return 0;
}

UINT SendCommand(int nCommunicateMode, char Comm[], long param, int ord,  SOCKET sock)
{
	char SendBuf[100];
	BYTE *pBits1;
//	SOCKET sock;
	unsigned long mLength;
	long lMaxWidthLength, lMaxPlotLength;
	// 20120822 新增切割文件发送前发送麦架长宽
	lMaxWidthLength = ftoi(g_lMaxPlotWidth * 0.025);
	lMaxPlotLength = ftoi(g_lMaxPlotLength * 0.025);

	int maxTimes = 0;
	while (1)
	{//发送文件开始命令
		strcpy(SendBuf,"Msg_Comm");//发送文件开始命令（文件宽度）
		SendBuf[8] = Comm[0];
		SendBuf[9] = Comm[1];
		pBits1 = (BYTE *)&param;
		SendBuf[10] = *(pBits1+3);
		SendBuf[11] = *(pBits1+2);
		SendBuf[12] = *(pBits1+1);
		SendBuf[13] = *pBits1;

		pBits1 = (BYTE *)&lMaxWidthLength;
		SendBuf[14] = *(pBits1+3);
		SendBuf[15] = *(pBits1+2);
		SendBuf[16] = *(pBits1+1);
		SendBuf[17] = *pBits1;

		pBits1 = (BYTE *)&lMaxPlotLength;
		SendBuf[18] = *(pBits1+3);
		SendBuf[19] = *(pBits1+2);
		SendBuf[20] = *(pBits1+1);
		SendBuf[21] = *pBits1;
		
		mLength = 22;
		if (nCommunicateMode == 0)
		{
// 			CString logtt,temp;
// 			logtt = "Write:";
// 			for (int i = 0;i < 22;i++)
// 			{
// 				temp.Format(",%x",SendBuf[i]);
// 				logtt += temp;
// 			}
// 			WriteLogTest(logtt);
			if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
			{
				mLength = 64;
				CH375ReadData(0,SendBuf,&mLength);
// 				logtt = "Read:";
// 				for (int i = 0;i < 22;i++)
// 				{
// 					temp.Format(",%x",SendBuf[i]);
// 					logtt += temp;
// 				}
				//WriteLogTest(logtt);
			}
		}
		else
		{
			send(sock,SendBuf,mLength,0);
			recv(sock,SendBuf,100,0);
		}
		if (memcmp(SendBuf,"Msg_",4) == 0)
		{
			if ((SendBuf[5] == ord) && (SendBuf[7] == 3))
			{
				//WriteLogTest("return3");
				return 3;
			}
			else if ((SendBuf[5] == ord) && (SendBuf[7] == 9))
				break;
		}
		Sleep(10);
		
		//gzr20161026
 		if (maxTimes > 10)
		{
			//WriteLogTest("Timeout");
 			return 3;
 		}
 		maxTimes++;
	}
	//WriteLogTest("return1");
	return 1;
}

void RestUsbPort()
{
	bool  restOk = false;
	int count=0;

		CH375CloseDevice(0);
		Sleep(20);
		while(CH375OpenDevice(0) == INVALID_HANDLE_VALUE){
			Sleep(20);
			CH375CloseDevice(0);
			count++;
			if(count > 50)
			{
				AfxMessageBox("打开USB设备失败!");
			}
		}
		/*
		count=0;
		while(CH375GetUsbID(0) != 0x4D495250)
		{
			Sleep(20);
			count++;
			if(count > 50)
			{
				AfxMessageBox("未知的USB设备!");
			}				
		}
		*/
}


UINT NetSendData(CString strFileName, SOCKET sock)
{
	char  SendBuf[100];//, command[2];
	//char cmdState;
	//char cReturn;

	CFile file;
	BYTE *pBits, *pBits1;
	BYTE  *pTestVerify;
	unsigned long  fileLength, lBufLen;
	unsigned long  mLength;
	unsigned long i, cumula=0;
	int iRetStatus;

	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return 1;
	fileLength = file.GetLength();
	pBits = new BYTE[fileLength];
	pTestVerify = pBits;
	file.Read(pBits, fileLength);
	
	i=0;
	while(i++ < fileLength)
		cumula +=  *pTestVerify++;

	i=0;
    
	//查询低层通讯缓冲区长度
	while (1)
	{
		strcpy(SendBuf,"Buf_Long_Start");//发送查询帧,查询绘图仪缓冲区长度
		mLength = 14;

		send(sock,SendBuf,mLength,0);
		recv(sock,SendBuf,100,0);

		pBits1 = (BYTE *)&lBufLen;
		*(pBits1+3) = SendBuf[4];
		*(pBits1+2) = SendBuf[5];
		*(pBits1+1) = SendBuf[6];
		*pBits1 = SendBuf[7];
		if (lBufLen > fileLength)
			break;
		else
		{
			Sleep(10);
		}
	}
	
	do
	{
		strcpy(SendBuf,"Snd_Long");
		pBits1 = (BYTE *)&fileLength;
		SendBuf[8] = *(pBits1+3);
		SendBuf[9] = *(pBits1+2);
		SendBuf[10] = *(pBits1+1);
		SendBuf[11] = *pBits1;
			
		send(sock,SendBuf,12,0);
		recv(sock,SendBuf,100,0);
	}while(strncmp(SendBuf,"Msg_OkS ",8)!=0);

	iRetStatus = send(sock,(char *)pBits,fileLength,0);
	while (1)
	{
		iRetStatus = recv(sock,SendBuf,100,0);
		if(strncmp(SendBuf,"Msg_OkPG",8)==0)//已接收完一个数据包
			break;
		else if(strncmp(SendBuf,"Msg_End ",8)==0)//文件已被删除	
			break;
	}

	while (1)
	{
		strcpy(SendBuf,"Msg_Comm");//发送文件传送结束帧
		SendBuf[8] = 0;
		SendBuf[9] = 2;

		mLength = 10;
		
		send(sock,SendBuf,mLength,0);
		recv(sock,SendBuf,100,0);
		
		if (memcmp(SendBuf,"Msg_",4) == 0)
		{
			if ((SendBuf[5] == 2) && (SendBuf[7] == 9))
				break;
		}
		Sleep(10);
	}	
	file.Close();
	return 0;
}

UINT USBSendData(CString strFileName)
{
	char SendBuf[100];//command[2], 
//	char cmdState;
	//char cReturn;

	CFile file;
	BYTE *pBits, *pBits1;
	//BYTE  *pTestVerify;
	unsigned long  fileLength, lBufLen;
	unsigned long  mLength;
	//unsigned long i, cumula=0, retuValue=0;

	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return 1;
	fileLength = file.GetLength();
	pBits = new BYTE[fileLength];
	//pTestVerify = pBits;
	file.Read(pBits, fileLength);
	
	//cReturn = 0;
	//i=0;
	//while(i++ < fileLength)
		//cumula +=  *pTestVerify++;	
    
	//查询低层通讯缓冲区长度
	while (1)
	{
		strcpy(SendBuf,"Buf_Long_Start");//发送查询帧,查询绘图仪缓冲区长度
		mLength = 14;

		if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
		{
			mLength = 64;
			CH375ReadData(0,SendBuf,&mLength);
		}
		
		pBits1 = (BYTE *)&lBufLen;
		*(pBits1+3) = SendBuf[4];
		*(pBits1+2) = SendBuf[5];
		*(pBits1+1) = SendBuf[6];
		*pBits1 = SendBuf[7];
		if (lBufLen > fileLength)
			break;
		else
		{
			Sleep(10);
		}
	}
		
	//发送数据
	strcpy(SendBuf,"Snd_Long");
	pBits1 = (BYTE *)&fileLength;
	SendBuf[8] = *(pBits1+3);
	SendBuf[9] = *(pBits1+2);
	SendBuf[10] = *(pBits1+1);
	SendBuf[11] = *pBits1;
	mLength = 12;
	if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
	{
		mLength = 64;
		CH375ReadData(0,SendBuf,&mLength);
	}
	if(strncmp(SendBuf,"Msg_OkS ",8)==0)
	{
		while (1)
		{
			mLength = fileLength;
			if (CH375WriteData(0,(char *)pBits,&mLength))  // 发送成功
			{
				if (fileLength > mLength)
				{
					fileLength -= mLength;
					pBits += mLength;
				}
				else
					break;
			}
		}
	}

	//文件是否发送完毕
	while (1)
	{//文件是否发送完毕
		mLength = 64;
		CH375ReadData(0,SendBuf,&mLength);
		if(strncmp(SendBuf,"Msg_End ",8)==0)
		{
			file.Close();
			break;
		}
	}

	while (1)
	{
		strcpy(SendBuf,"Msg_Comm");//发送文件传送结束帧
		SendBuf[8] = 0;
		SendBuf[9] = 2;

		mLength = 10;
		if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
		{
			mLength = 64;
			CH375ReadData(0,SendBuf,&mLength);
		}
		if (memcmp(SendBuf,"Msg_",4) == 0)
		{
			if ((SendBuf[5] == 2) && (SendBuf[7] == 9))
			{
				//retuValue |= ((SendBuf[8]<<24)&0xff000000);
				//retuValue |= ((SendBuf[9]<<16)&0x00ff0000);
				//retuValue |= ((SendBuf[10]<<8)&0x0000ff00);
				//retuValue |= (SendBuf[11]&0x000000ff);
				//if(retuValue != cumula)
				//	cReturn = 1;
				break;
			}
		}
		Sleep(10);
	}
//	RestUsbPort();
	return 0;
}

UINT SendCutDataFile(int nCommunicateMode, CString strFileName,  SOCKET sock)
{
	char command[2];//, SendBuf[100];
	CFile file;
	unsigned long  fileLength;

	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return 1;
	fileLength = file.GetLength();
	file.Close();

	command[0] = 0;
	command[1] = 3;
	SendCommand(nCommunicateMode, command, fileLength, 3, sock);	

	if(nCommunicateMode == 0)
		USBSendData(strFileName);
	else
		NetSendData(strFileName, sock);
	
	return 0;
}


/*
UINT SendCutDataFile(int nCommunicateMode, CString strFileName)//20100907
{
	char command[2], SendBuf[100];
	CFile file;
	BYTE *pBits, *pBits1;
	unsigned long  fileLength, lBufLen;
	unsigned long  mLength;

	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return 1;
	fileLength = file.GetLength();
	pBits = new BYTE[fileLength];
	file.Read(pBits, fileLength);

	command[0] = 0;
	command[1] = 3;
	SendCommand(nCommunicateMode, command, fileLength, 3);	

	//查询低层通讯缓冲区长度
	while (1)
	{
		strcpy(SendBuf,"Buf_Long_Start");//发送查询帧,查询绘图仪缓冲区长度
		mLength = 14;

		if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
		{
			mLength = 64;
			CH375ReadData(0,SendBuf,&mLength);
		}
		
		pBits1 = (BYTE *)&lBufLen;
		*(pBits1+3) = SendBuf[4];
		*(pBits1+2) = SendBuf[5];
		*(pBits1+1) = SendBuf[6];
		*pBits1 = SendBuf[7];
		if (lBufLen > fileLength)
			break;
		else
		{
			Sleep(10);
		}
	}
		
	//发送数据
	strcpy(SendBuf,"Snd_Long");
	pBits1 = (BYTE *)&fileLength;
	SendBuf[8] = *(pBits1+3);
	SendBuf[9] = *(pBits1+2);
	SendBuf[10] = *(pBits1+1);
	SendBuf[11] = *pBits1;
	mLength = 12;
	if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
	{
		mLength = 64;
		CH375ReadData(0,SendBuf,&mLength);
	}
	if(strncmp(SendBuf,"Msg_OkS ",8)==0)
	{
		while (1)
		{
			mLength = fileLength;
			if (CH375WriteData(0,(char *)pBits,&mLength))  // 发送成功
			{
				if (fileLength > mLength)
				{
					fileLength -= mLength;
					pBits += mLength;
				}
				else
					break;
			}
		}
	}

	//文件是否发送完毕
	while (1)
	{//文件是否发送完毕
		mLength = 64;
		CH375ReadData(0,SendBuf,&mLength);
		if(strncmp(SendBuf,"Msg_End ",8)==0)
		{
			file.Close();
			break;
		}
	}

	while (1)
	{
		strcpy(SendBuf,"Msg_Comm");//发送文件传送结束帧
		SendBuf[8] = 0;
		SendBuf[9] = 2;

		mLength = 10;
		if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
		{
			mLength = 64;
			CH375ReadData(0,SendBuf,&mLength);
		}
		if (memcmp(SendBuf,"Msg_",4) == 0)
		{
			if ((SendBuf[5] == 2) && (SendBuf[7] == 9))
				break;
		}
		Sleep(10);
	}
	RestUsbPort();
	return 0;
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
	long DelayWait = 0;		//20140814
	//int iSendFailCount=0; //发送失败计数器
	
	pParent=(CWnd*)((SendDataThreadInfo*)pParam)->m_pParent;
	pPlotItem=((SendDataThreadInfo*)pParam)->m_pPlotItem;
	lTotalLen = (g_lMaxPlotWidth + long((pPlotItem->m_dGapRight * MMPixelY + 7) / 8)) * SECTION_WIDTH;
	lTotalPage = (g_lMaxPlotLength + SECTION_WIDTH - 1) / SECTION_WIDTH;
	lTotalPage1 = lTotalPage;
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
			{//正在打印文件被暂停
				if (g_nVersion == 1)
				{
					while (1)
					{
						strcpy(SendBuf,"Msg_Comm");//发送文件暂停帧
						SendBuf[8] = 0;
						SendBuf[9] = 7;
						mLength = 10;
						if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
						{
							mLength = 64;
							CH375ReadData(0,SendBuf,&mLength);
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
				
				if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
			//******查询低层通讯缓冲区长度END******
			
			if (pPlotItem->m_nState == 6)
			{// 正在打印的暂停文件已在数据发送线程处理
				Sleep(10);
				continue;
			}
			if (pPlotItem->m_nState == 4)
			{//文件删除已在位图生成线程中处理
				while (1)
				{
					strcpy(SendBuf,"Msg_Comm");//发送文件传送结束帧
					SendBuf[8] = 0;
					SendBuf[9] = 4;
					mLength = 10;
					if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
					{
						mLength = 64;
						CH375ReadData(0,SendBuf,&mLength);
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
			if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
			{
				mLength = 64;
				CH375ReadData(0,SendBuf,&mLength);
			}
			if(strncmp(SendBuf,"Msg_OkS ",8)==0)
			{
				
				while (1)
				{
					mLength = lFileLen;
					
					if (CH375WriteData(0,(char *)pBits,&mLength))  // 发送成功
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
		
			DelayWait = 0;   //20140814
			while (1)
			{//文件是否发送完毕
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
					//g_bPrintEnd = true;
					return 0;
				}
				else
				{	//20140814 				
					DelayWait++;
					if(DelayWait > 100)
					{
						AfxMessageBox("Delay > 100 Count!!");
						break;
					}
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
				lSendLength = lSendPage * SECTION_WIDTH;
				pPlotItem->m_lSendLength = long((g_lMaxPlotLength * i + lSendLength) / g_dScaleX * 25.4 / 300);
			}
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
		if (CH375WriteData(0,SendBuf,&mLength))  // 发送成功
		{
			mLength = 64;
			CH375ReadData(0,SendBuf,&mLength);
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
	RemoveGLBuffer1();
	pParent->SendMessage(WM_ENDTRANSMITFILE,0,0);
//	g_bPrintEnd = true;
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
				dp.x = ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioX) + g_dpOrigin.x + g_lMaxPlotLength;
				dp.y = ftoi(pInstruct->m_adPara.GetAt(i++) * g_dRatioY) + g_dpOrigin.y - g_lMinPlotWidth;
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

  if (wBytes == 0)
		return;

  nHeight=(g_nInternalLeading_E + g_nHeight_E) * g_nHeight_C;
  dLToDScale=(double)nHeight / 	(g_dTextHeight*400);//实际毫米数

  dCos=cos(g_dTextAngle);
  dSin=sin(g_dTextAngle);

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

//判断文件是否需要将切割数据转成打印数据
int JudgeFileNeedCutData(CString strFilePathC, int nCommunicateMode, SOCKET sock)
{
	int reValue;
	FileParse(strFilePathC);	
	g_cPrintCutSelect &= 0x03;
	// 20101214  打印切割选择
	// 打印
	if ( (g_cPrintCutSelect&0x01) == 0x01 && (g_cPrintCutSelect&0x02) == 0x02)
	{
		// g_nRotate=pPlotItem->m_nRotate;
		InitTextChangeLine();
		InitHPGL();
// 		g_lFixedPointLength=pPlotItem->m_lFixedPointLength*40;
// 		g_lInitMaxPlotLength=pPlotItem->m_lMaxPlotLength;
// 		g_lInitMaxPlotWidth=pPlotItem->m_lMaxPlotWidth;
// 		g_lMaxPlotLength-=pPlotItem->m_lMinPlotLength;
// 		g_lMinPlotWidth=pPlotItem->m_lMinPlotWidth;
#if _DEBUG
		CString strWorkPath1(PLTDATAPATH);
#else
		TCHAR szFullPath[_MAX_PATH];
		TCHAR szDir[_MAX_DIR];
		TCHAR szDrive[_MAX_DRIVE];
		CString strWorkPath1;
		::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
		_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
		strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
		strWorkPath1 += "Hidpt.chv";
#endif		
		reValue = AnalysisPltFile( strWorkPath1 );
	}
	return reValue;
}
