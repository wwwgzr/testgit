#if !defined(AFX_PICTUREDLG_H__8BC8E9D0_C3DF_40FF_B137_7CD6ADA597F2__INCLUDED_)
#define AFX_PICTUREDLG_H__8BC8E9D0_C3DF_40FF_B137_7CD6ADA597F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PictureDlg.h : header file
//
#include "plotCenterDlg.h"
#include "PlotContext.h"//20160125�и�ָ��ʶ��
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

	// ���ÿؼ���Ϣ
	BOOL SetControlProperty(PDLGCTLINFO lp, int nElements);

	// �Ƿ��ڶԻ������½���ʾ��ʾ�ɸı��С��ͼ��
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
	CPlotContext *	m_pPltContxt;//20160125�и�ָ��ʶ��
	
	bool		ChangePen(CPoint & point, BYTE cuepen);//20160125�и�ָ��ʶ��

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
	int m_iClientWidth;		// �Ի���client����Ŀ��
	int m_iClientHeight;	// �Ի���client����ĸ߶�
	int m_iMinWidth;			// �Ի������С���
	int m_iMinHeight;			// �Ի������С�߶�
	PDLGCTLINFO m_pControlArray; // �ؼ���Ϣ����ָ��
	int m_iControlNumber;	// ���ÿؼ���Ϣ�Ŀؼ�����
	BOOL m_bShowSizeIcon;	// �Ƿ���ʾ��ʾ�ɸı��С��ͼ��
	CStatic m_wndSizeIcon;// ��ͼ��ľ�̬�ؼ�
	// ����ͼ���bitmap
	CBitmap m_bmpSizeIcon;
	BITMAP m_bitmap;

	double	m_multiple;		// ���ŵı���20160121�и�ָ��ʶ��
	double	m_mulConst;		// ���ŵĳ���
	long	m_maxY;			// ͼ�����ݵķ�Χ�����ֵ


	long	m_ofstX;
	long	m_ofstY;		// ��ǰ��ͼԭ���������е�ƫ��

	int  picvar;//ͼ�ηŴ���С����
	POINT		LogPointToScr(CMyPoint * pit);//20160127�и�ָ��ʶ��

	bool	GetDirectionX(int escapement, double & x, double & y);//20160122�и�ָ��ʶ��
	bool	GetOnLinePt(CMyPoint * ptBeg, CMyPoint * ptEnd, long value, long & result, bool valX = true);//20160122�и�ָ��ʶ��
	bool	m_yDirection;//20160127�и�ָ��ʶ��
	int OpenSecretFileNew(CString strFileName);//20160127�и�ָ��ʶ��
	void InitHPGL();//20160127�и�ָ��ʶ��
	BOOL m_bPSValid;//�Ƿ��յ�PSָ��20160127�и�ָ��ʶ��
	BOOL m_bIPValid;//IPָ���Ƿ���Ч20160127�и�ָ��ʶ��
	BOOL m_bSCValid;//SCָ���Ƿ���Ч20160127�и�ָ��ʶ��
	CRect m_rtHardLimitation;//Ӳ������20160127�и�ָ��ʶ��
	CPoint m_dpOrigin;//ԭ������20160127�и�ָ��ʶ��
	CPoint m_dpCurrent;//��ǰ����20160127�и�ָ��ʶ��
	CPoint m_dpP1,m_dpP2;//���ŵ�20160127�и�ָ��ʶ��
	double m_dRatioX,m_dRatioY;//��ǰ�����ű���20160127�и�ָ��ʶ��
	int m_nRotate;//20160127�и�ָ��ʶ��
	BOOL m_bPenDown;//���Ƿ�����20160127�и�ָ��ʶ��
	int m_nLineType;//20160127�и�ָ��ʶ��
	int m_nPen;//�ʺ�//20160127�и�ָ��ʶ��
	BOOL m_bAbsolute;//��������//20160127�и�ָ��ʶ��
	unsigned char m_cTerminator;//20160127�и�ָ��ʶ��
	unsigned char m_cTerminatorMode;//ģʽ//20160127�и�ָ��ʶ��
	double m_dTextHeight;//���ָ߶ȺͿ��//20160127�и�ָ��ʶ��
	double m_dTextWidth;//20160127�и�ָ��ʶ��
	double m_dTextAngle;//���ֽǶ�//20160127�и�ָ��ʶ��
	int m_nTextPosition;//�ַ����ԭ���λ��//20160127�и�ָ��ʶ��
	long m_lMaxPlotLength;	//ͼ�γ���//20160127�и�ָ��ʶ��
	long m_lMaxPlotWidth;		//ͼ�ο��//20160127�и�ָ��ʶ��
	long m_lMinPlotLength;//��Сͼ�γ���(����)//20160127�и�ָ��ʶ��
	long m_lMinPlotWidth;		//ͼ����С���//20160127�и�ָ��ʶ��
	long m_lMaxPGLength;//20160127�и�ָ��ʶ��
	long m_lMaxPGWidth;//20160127�и�ָ��ʶ��
	CString m_strText;   //�ı���20160127�и�ָ��ʶ��
	void RemoveGLBuffer();//20160127�и�ָ��ʶ��
	void InitTextChangeLine(void);//20160127�и�ָ��ʶ��
	CPoint m_ptCurrentPosition;   //��ǰλ��(����)20160127�и�ָ��ʶ��
	int m_nInternalLeading_E,m_nHeight_E,m_nDescent_E; //m_nHeight_E��windows�������Ascent-InternalLeading20160127�и�ָ��ʶ��
	 CArray <CEnglish_char,CEnglish_char> m_aEnglish_char;//20160127�и�ָ��ʶ��
	 int m_nHeight_C,m_nWidth_C;//20160127�и�ָ��ʶ��
	 CArray <CChinese_char,CChinese_char> m_aChinese_char;//20160127�и�ָ��ʶ��
	 void ProcessInstruct(CInstruct *pInstruct);//20160127�и�ָ��ʶ��
	 void GLTextMinXMaxX(CGLText *pText);//20160127�и�ָ��ʶ��
	 void TextChangeLine(CPoint dp,LPCTSTR lpszString, int nCount);//20160127�и�ָ��ʶ��
	 void PlotChar(CPoint ptPointLU,BYTE *pbyData,WORD wBytes,int nLeft,int nDown,int nFontScale);//20160127�и�ָ��ʶ��
	 CPoint CPToLP(CPoint ptCP,int nHeight,CPoint ptPointLU,double dSin,double dCos,double dScale);//20160127�и�ָ��ʶ��
	 int IntRound(double x);//20160127�и�ָ��ʶ��
	 void MoveTo(long x, long y);//20160127�и�ָ��ʶ��
	 CPoint m_ptPrevPosition;      //ǰһλ��(����)20160127�и�ָ��ʶ��
	 void LineTo(long x, long y);//20160127�и�ָ��ʶ��
	 void WriteTextData(BOOL bFirst,long x,long y);//20160127�и�ָ��ʶ��
	 void Arc(int nLeftRect,int nTopRect,int nRightRect,int nBottomRect,int nXStartArc,int nYStartArc,int nXEndArc,int nYEndArc);//20160127�и�ָ��ʶ��
	 double angle_2(double startx,double starty,double endx,double endy);//20160127�и�ָ��ʶ��
	 BOOL CircleCR(double x1,double y1,double x2,double y2,double x3,double y3,double &cx,double &cy,double &cr);//20160127�и�ָ��ʶ��
	 int IntOfTwoLine(double a1,double b1,double c1,double a2,double b2,double c2,double& x,double& y);//20160127�и�ָ��ʶ��
	 CPoint GetNextCoodinate(WORD wDirection,WORD wLength,CPoint ptPoint);//20160127�и�ָ��ʶ��
	 void SetRatio();//20160127�и�ָ��ʶ��
	 struct stru_SC m_struSC;//SC����20160127�и�ָ��ʶ��
	 int OpenDissecretFile(CString strFileName);//20160127�и�ָ��ʶ��
	 BOOL IsAlpha(char ch);//20160127�и�ָ��ʶ��
	 int GetParameterType(char cName[]);//20160127�и�ָ��ʶ��
	 BOOL IsDigit(char ch);//20160127�и�ָ��ʶ��

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICTUREDLG_H__8BC8E9D0_C3DF_40FF_B137_7CD6ADA597F2__INCLUDED_)
