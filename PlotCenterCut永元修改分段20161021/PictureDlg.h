#if !defined(AFX_PICTUREDLG_H__8BC8E9D0_C3DF_40FF_B137_7CD6ADA597F2__INCLUDED_)
#define AFX_PICTUREDLG_H__8BC8E9D0_C3DF_40FF_B137_7CD6ADA597F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PictureDlg.h : header file
//
#include "plotCenterDlg.h"
#include "PlotContext.h"//20160125切割指令识别
/////////////////////////////////////////////////////////////////////////////
// CPictureDlg dialog
class CPictureDlg : public CDialog
{
// Construction
public:
	CPictureDlg(CWnd* pParent = NULL);   // standard constructor
//	CPlotCenterDlg *m_pParent;

	typedef struct _dlgControlTag
	{
		int iId;
		int iFlag;
		int iPercent;
	} DLGCTLINFO, *PDLGCTLINFO;

	enum
	{
		MOVEX=0,
		MOVEY,
		MOVEXY,
		ELASTICX,
		ELASTICY,
		ELASTICXY
	};

	// 设置控件信息
	BOOL SetControlProperty(PDLGCTLINFO lp, int nElements);

	// 是否在对话框右下角显示表示可改变大小的图标
	void ShowSizeIcon(BOOL bShow = TRUE);

// Dialog Data
	//{{AFX_DATA(CPictureDlg)
	enum { IDD = IDD_PICTURE };
	CString	m_strTotalX;
	CString	m_strTotalY;
	long	m_lFixedPointLength;
	int		m_nCopyNum;
	//}}AFX_DATA
public:
	void DrawPicture(int nLarge);
	void DocToClient(CPoint &pt);
	long m_lFileLength;
	long m_lFileWidth;
	long m_lFileMinLength;
	long m_lFileMinWidth;
	long m_lTotalX;
	long m_lTotalY;
	double m_dProportion;
	CString m_strPictureName;
	BOOL m_bLButtonDown;
	CPoint m_ptMouseDownPos;
	CPoint m_ptMouseUpPos;
	CPoint m_ptStartPos;
	int m_nLanguage;
	CRect m_rtDrawPicture;
	CPlotContext *	m_pPltContxt;//20160125切割指令识别
	
	bool		ChangePen(CPoint & point, BYTE cuepen);//20160125切割指令识别

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPictureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPictureDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDraw();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChangeFixedPoint();
	afx_msg void OnChangeCopyNum();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int m_iClientWidth;		// 对话框client区域的宽度
	int m_iClientHeight;	// 对话框client区域的高度
	int m_iMinWidth;			// 对话框的最小宽度
	int m_iMinHeight;			// 对话框的最小高度
	PDLGCTLINFO m_pControlArray; // 控件信息数组指针
	int m_iControlNumber;	// 设置控件信息的控件个数
	BOOL m_bShowSizeIcon;	// 是否显示表示可改变大小的图标
	CStatic m_wndSizeIcon;// 放图标的静态控件
	// 保存图标的bitmap
	CBitmap m_bmpSizeIcon;
	BITMAP m_bitmap;

	double	m_multiple;		// 缩放的倍数20160121切割指令识别
	double	m_mulConst;		// 缩放的常数
	long	m_maxY;			// 图像数据的范围的最大值


	long	m_ofstX;
	long	m_ofstY;		// 当前视图原点在数据中的偏移

	int  picvar;//图形放大缩小变量
	POINT		LogPointToScr(CMyPoint * pit);//20160127切割指令识别

	bool	GetDirectionX(int escapement, double & x, double & y);//20160122切割指令识别
	bool	GetOnLinePt(CMyPoint * ptBeg, CMyPoint * ptEnd, long value, long & result, bool valX = true);//20160122切割指令识别
	bool	m_yDirection;//20160127切割指令识别
	int OpenSecretFileNew(CString strFileName);//20160127切割指令识别
	void InitHPGL();//20160127切割指令识别
	BOOL m_bPSValid;//是否收到PS指令20160127切割指令识别
	BOOL m_bIPValid;//IP指令是否有效20160127切割指令识别
	BOOL m_bSCValid;//SC指令是否有效20160127切割指令识别
	CRect m_rtHardLimitation;//硬剪切限20160127切割指令识别
	CPoint m_dpOrigin;//原点坐标20160127切割指令识别
	CPoint m_dpCurrent;//当前坐标20160127切割指令识别
	CPoint m_dpP1,m_dpP2;//缩放点20160127切割指令识别
	double m_dRatioX,m_dRatioY;//当前的缩放比率20160127切割指令识别
	int m_nRotate;//20160127切割指令识别
	BOOL m_bPenDown;//笔是否落下20160127切割指令识别
	int m_nLineType;//20160127切割指令识别
	int m_nPen;//笔号//20160127切割指令识别
	BOOL m_bAbsolute;//绝对坐标//20160127切割指令识别
	unsigned char m_cTerminator;//20160127切割指令识别
	unsigned char m_cTerminatorMode;//模式//20160127切割指令识别
	double m_dTextHeight;//文字高度和宽度//20160127切割指令识别
	double m_dTextWidth;//20160127切割指令识别
	double m_dTextAngle;//文字角度//20160127切割指令识别
	int m_nTextPosition;//字符相对原点的位置//20160127切割指令识别
	long m_lMaxPlotLength;	//图形长度//20160127切割指令识别
	long m_lMaxPlotWidth;		//图形宽度//20160127切割指令识别
	long m_lMinPlotLength;//最小图形长度(坐标)//20160127切割指令识别
	long m_lMinPlotWidth;		//图形最小宽度//20160127切割指令识别
	long m_lMaxPGLength;//20160127切割指令识别
	long m_lMaxPGWidth;//20160127切割指令识别
	CString m_strText;   //文本串20160127切割指令识别
	void RemoveGLBuffer();//20160127切割指令识别
	void InitTextChangeLine(void);//20160127切割指令识别
	CPoint m_ptCurrentPosition;   //当前位置(坐标)20160127切割指令识别
	int m_nInternalLeading_E,m_nHeight_E,m_nDescent_E; //m_nHeight_E是windows中字体的Ascent-InternalLeading20160127切割指令识别
	 CArray <CEnglish_char,CEnglish_char> m_aEnglish_char;//20160127切割指令识别
	 int m_nHeight_C,m_nWidth_C;//20160127切割指令识别
	 CArray <CChinese_char,CChinese_char> m_aChinese_char;//20160127切割指令识别
	 void ProcessInstruct(CInstruct *pInstruct);//20160127切割指令识别
	 void GLTextMinXMaxX(CGLText *pText);//20160127切割指令识别
	 void TextChangeLine(CPoint dp,LPCTSTR lpszString, int nCount);//20160127切割指令识别
	 void PlotChar(CPoint ptPointLU,BYTE *pbyData,WORD wBytes,int nLeft,int nDown,int nFontScale);//20160127切割指令识别
	 CPoint CPToLP(CPoint ptCP,int nHeight,CPoint ptPointLU,double dSin,double dCos,double dScale);//20160127切割指令识别
	 int IntRound(double x);//20160127切割指令识别
	 void MoveTo(long x, long y);//20160127切割指令识别
	 CPoint m_ptPrevPosition;      //前一位置(坐标)20160127切割指令识别
	 void LineTo(long x, long y);//20160127切割指令识别
	 void WriteTextData(BOOL bFirst,long x,long y);//20160127切割指令识别
	 void Arc(int nLeftRect,int nTopRect,int nRightRect,int nBottomRect,int nXStartArc,int nYStartArc,int nXEndArc,int nYEndArc);//20160127切割指令识别
	 double angle_2(double startx,double starty,double endx,double endy);//20160127切割指令识别
	 BOOL CircleCR(double x1,double y1,double x2,double y2,double x3,double y3,double &cx,double &cy,double &cr);//20160127切割指令识别
	 int IntOfTwoLine(double a1,double b1,double c1,double a2,double b2,double c2,double& x,double& y);//20160127切割指令识别
	 CPoint GetNextCoodinate(WORD wDirection,WORD wLength,CPoint ptPoint);//20160127切割指令识别
	 void SetRatio();//20160127切割指令识别
	 struct stru_SC m_struSC;//SC参数20160127切割指令识别
	 int OpenDissecretFile(CString strFileName);//20160127切割指令识别
	 BOOL IsAlpha(char ch);//20160127切割指令识别
	 int GetParameterType(char cName[]);//20160127切割指令识别
	 BOOL IsDigit(char ch);//20160127切割指令识别

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICTUREDLG_H__8BC8E9D0_C3DF_40FF_B137_7CD6ADA597F2__INCLUDED_)
