// PlotCenterDlg.h : header file
#include "PlotListCtrl.h"
#include "Instruct.h"
#include "DPoint.h"
#include "Line.h"
//

#if !defined(AFX_PLOTCENTERDLG_H__30E37CFF_98D6_4D60_BAB2_6AED5A960B2A__INCLUDED_)
#define AFX_PLOTCENTERDLG_H__30E37CFF_98D6_4D60_BAB2_6AED5A960B2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define NONE_TYPE			0
#define DIGIT_TYPE		1
#define STRING_TYPE		2
#define PE_TYPE				3
#define ESC_TYPE			4
#define DT_TYPE				5

#define	PROCESS_MNEMONIC	0//命令
#define PROCESS_PARAMETER	1//参数
#define PROCESS_FIND			2//结束符

//线形
#define LINE_TYPE_SOLID				0
#define LINE_TYPE_DASH				1		/* -------  */
#define LINE_TYPE_DOT				2		/* .......  */
#define LINE_TYPE_DASHDOT			3		/* _._._._  */
//图元类型
#define PRIMITIVE_ARC				1  //弧线：圆心X，Y，当前点X1，Y1，圆弧度数，正为逆时针
#define PRIMITIVE_ARC3				2  //三点弧线：起点X，Y，中点X1，Y1，终点x2,y2
#define PRIMITIVE_LINE				3  //线段：点1 X1，Y1，点2 X2，Y2,...
#define PRIMITIVE_CIRCLE			4  //圆心x0,y0, 半径 r
#define PRIMITIVE_POINT				5  //点x0,y0
#define PRIMITIVE_LINE3				6  //线段：点1 X1，Y1，点2 X2，Y2,点3 X3，Y3,
#define PRIMITIVE_SYMBOL			7  //符号：符号ch 点1 X1，Y1，点2 X2，Y2,...
#define PRIMITIVE_RECT				8  //对角：点1 X1，Y1，点2 X2，Y2,
#define PRIMITIVE_PG				9  //换页
#define PRIMITIVE_LT				10 //线型
#define PRIMITIVE_SP				11 //选笔
#define PRIMITIVE_LO				12 //标志原点
#define PRIMITIVE_SI				13 //绝对字符尺寸
#define PRIMITIVE_DI				14 //绝对方向
#define PRIMITIVE_LB				15 //标志
#define PRIMITIVE_DASH				16 //虚线
#define PRIMITIVE_DR				17 //相对方向
#define PRIMITIVE_CUTLINE			18//切割线//20150729
#define PRIMITIVE_DR				17 //相对方向位图
#define PRIMITIVE_FN				18 //字体名称位图


#define WM_ENDTRANSMITFILE		WM_USER+100  //文件发送完毕
#define WM_TRANSMITPERCENT		WM_USER+101  //已发送的百分比
#define SECTION_WIDTH 1200		//1200点分页

typedef CArray<double,double> CDoubleArray;
typedef CArray<int,int> CIntArray;
#define MAaxPenNum	1
#define PAI					3.1415926535897932384626
/////////////////////////////////////////////////////////////////////////////
// CPlotCenterDlg dialog
class CChinese_char {
public:
  WORD m_wCharCode;
  WORD m_wBytes;
  DWORD m_dwPosition;

  CChinese_char() {m_wCharCode=0;m_wBytes=0;m_dwPosition=0;};
  CChinese_char(CChinese_char &a) {m_wCharCode=a.m_wCharCode;m_wBytes=a.m_wBytes;m_dwPosition=a.m_dwPosition;};
  CChinese_char(WORD wCharCode,WORD wBytes,DWORD dwPosition) {m_wCharCode=wCharCode;m_wBytes=wBytes;m_dwPosition=dwPosition;};
  ~CChinese_char() {};
  void operator=(CChinese_char &a) {m_wCharCode=a.m_wCharCode;m_wBytes=a.m_wBytes;m_dwPosition=a.m_dwPosition;};
};

class CEnglish_char {
public:
  WORD m_wCharCode;
  WORD m_wBytes;
  WORD m_wWidth;
  DWORD m_dwPosition;

  CEnglish_char() {m_wCharCode=0;m_wBytes=0;m_wWidth=0;m_dwPosition=0;};
  CEnglish_char(CEnglish_char &a) {m_wCharCode=a.m_wCharCode;m_wBytes=a.m_wBytes;m_wWidth=a.m_wWidth;m_dwPosition=a.m_dwPosition;};
  CEnglish_char(WORD wCharCode,WORD wBytes,WORD wWidth,DWORD dwPosition) {m_wCharCode=wCharCode;m_wBytes=wBytes;m_wWidth=wWidth;m_dwPosition=dwPosition;};
  ~CEnglish_char() {};
  void operator=(CEnglish_char &a) {m_wCharCode=a.m_wCharCode;m_wBytes=a.m_wBytes;m_wWidth=a.m_wWidth;m_dwPosition=a.m_dwPosition;};
};

class CGLPrimitive:public CObject
{
public:
	int m_nType;
	CIntArray m_anData;
};

class CGLPrimitive1:public CObject
{
public:
	int m_nType;//类型
	CIntArray m_anData;//数据
	int m_nMaxPDLength;//线条最大坐标点
	int m_nMaxPDWidth;
	int m_nMinPDLength;//线条最小坐标点
	int m_nMinPDWidth;
};

class CGLText:public CObject
{
public:
	int m_nType;
	CString m_strText;   //文本串
	CDoubleArray m_adPara;
	int	m_iWeight;	//磅数位图
	unsigned char m_cIsItali; //斜体位图
	CString m_cFontName; //字体名称位图
};
class CGLText1:public CObject
{
public:
	CString m_strText;   //文本串
	int m_nX;//文字打印起点坐标
	int m_nY;
	double m_dTextHeight;//文字高度和宽度
	double m_dTextWidth;
	double m_dTextAngle;//文字角度
	int m_nMaxPDLength;//字符串最大坐标点
	int m_nMaxPDWidth;
	int m_nMinPDLength;//字符串最小坐标点
	int m_nMinPDWidth;
};

class CPlotItem : public CObject  {
public:
	CPlotItem();
	CPlotItem(CPlotItem &a);
	CPlotItem(CString strPlotName,int nCopyNum,long lFixedPointLength,long lPictureLength,long lPictureWidth,long lPaperWidth,long lMaxPlotLength,long lMaxPlotWidth,long m_lFileMinLength,long lMinPlotWidth);
	~CPlotItem();

public:
	CString m_strPlotName;  //绘图名
	int m_nState;						//状态,=0 正在忙,=1 等待,=2 暂停发送，3文件删除,4文件删除已在位图生成线程中处理
													//5正在打印文件被暂停,6正在打印的被暂停文件已在数据发送线程处理
	int m_nCopyNum;					//打印份数
	long m_lFixedPointLength;//打印起始长度,单位:mm
	long m_lPictureLength;	//图形长度
	long m_lPictureWidth;		//图形宽度
	long m_lSendLength;			//已发送长度
	long m_lPaperWidth;//纸张宽度
	long m_lMarkSpace;//对接符间距
	double m_dGapRight;//右边距
	int m_nRotate;//图形旋转标志=1旋转
	long m_lMaxPlotLength;	//图形长度
	long m_lMaxPlotWidth;		//图形宽度
	long m_lMinPlotLength;//最小图形长度(坐标)
	long m_lMinPlotWidth;		//图形最小宽度

	void operator=(CPlotItem& a);
};

struct stru_SC
{
	double dXMin,dXMax,dYMin,dYMax;
	int nType;
	double dLeft,dBottom;
};
class SendDataThreadInfo  {
public:
	CWnd *m_pParent;
	SOCKET sock;
	CPlotItem *m_pPlotItem;
};
int ftoi(double f);
void WriteLogTest(CString content);

class CPlotCenterDlg : public CDialog
{
// Construction
public:
	void SortByTitle();	//20140530
	void ConnectUSB(BOOL bDisplayFlag);
	void PrintTestGraph(CString strPathName);
	void WriteTextData(BOOL bFirst,long x,long y);
	int IntOfTwoLine(double a1,double b1,double c1,double a2,double b2,double c2,double& x,double& y);
	double angle_2(double startx,double starty,double endx,double endy);
	CPoint GetNextCoodinate(WORD wDirection,WORD wLength,CPoint ptPoint);
	BOOL CircleCR(double x1,double y1,double x2,double y2,double x3,double y3,double &cx,double &cy,double &cr);
	void Arc(int nLeftRect,int nTopRect,int nRightRect,int nBottomRect,int nXStartArc,int nYStartArc,int nXEndArc,int nYEndArc);
	int IntRound(double x);
	void LineTo(long x, long y);
	void MoveTo(long x, long y);
	CPoint CPToLP(CPoint ptCP,int nHeight,CPoint ptPointLU,double dSin,double dCos,double dScale);
	void InitTextChangeLine(void);
	void PlotChar(CPoint ptPointLU,BYTE *pbyData,WORD wBytes,int nLeft,int nDown,int nFontScale);
	void TextChangeLine(CPoint dp,LPCTSTR lpszString, int nCount);
	void SprayError(short *sSprayError);
	void PrintErrorY(double dGraphWidth,double dDockError);
	void PrintTrackPaperError(double dTrackPaperError);//20100413测试走纸误差发送指令函数
	void OpenHistoryFile(CString strFileName);

	int m_nLineType;
	int m_nPen;//笔号
	int m_nPenPE;//PE笔号
	CPoint m_dpOrigin;//原点坐标
	CPoint m_dpCurrent;//当前坐标
	unsigned char m_cTerminator;//标志终结符
	unsigned char m_cTerminatorMode;//模式
	CString m_strText;//字符
	CPoint m_dpP1,m_dpP2;//缩放点
	BOOL m_bPSValid;//是否收到PS指令
	BOOL m_bIPValid;//IP指令是否有效
	BOOL m_bSCValid;//SC指令是否有效
	CRect m_rtHardLimitation;//硬剪切限
	double m_dRatioX,m_dRatioY;//当前的缩放比率
	BOOL m_bAbsolute;//绝对坐标
	double m_dTextAngle;//字符方向
	double m_dTextHeight;//字符高度
	double m_dTextWidth;//字符宽度
	int m_nTextPosition;//字符相对原点的位置
	BOOL m_bPenDown;//笔是否落下
	int m_nRotate;//旋转角度
	struct stru_SC m_struSC;//SC参数
	CObList m_listPlotItem;  //绘图项
	CMenu m_englistmenu;
	CMenu m_chinesemenu;
	CToolBar m_wndtoolbar;
	
	int m_nVersion;
	int m_nLanguage;
	int m_nCommunicateMode;
	int m_nWaitTime;//记录等待时间
	BOOL m_bWaitConnected;//是否需要检测网络并建立连接
	BOOL m_bCutFile;//打开绘图中心时判断是否继续打印
	BOOL m_bTimerBusy;//是否已经退出定时器中断
	BOOL m_bOpenFile;//是否正在打开文件
	BOOL m_bConnected;//=true 已建立了连接，=false 没有建立连接
	BOOL m_bNetworkBusy;//=true 正在打印
	int m_nLineWidth;//线宽
	unsigned char m_cPrintStyle;//打印模式
	unsigned char m_cSprinklerCount;//喷头数
	unsigned char m_cSprinklerNo;//起始喷头编号
	unsigned char m_cPenNo;//笔号 20101206 笔号刀号
	unsigned char m_cCutNo;//刀号
	//unsigned char m_cPrintCutSelect; //20101206 打印切割选择
	double m_dOverlapError;//走纸误差20100413
	double m_dMalposedError;//双向打印误差
	double m_dTrackPaperError;//走纸误差20100413
	short	m_sSprayErrorX12;//喷头间重叠误差
	short	m_sSprayErrorX23;
	short	m_sSprayErrorX34;
	short	m_sSprayErrorY12;//喷头间错位误差
	short	m_sSprayErrorY23;
	short	m_sSprayErrorY34;
	double m_dPlotterXError;
	double m_dPlotterYError;
	long m_lMaxPlotLength;
	long m_lMaxPlotWidth;
	long m_lMinPlotLength;
	long m_lMinPlotWidth;
	long m_lMaxPGLength;
	long m_lMaxPGWidth;
	short m_sPlotType;
	SendDataThreadInfo m_PlotThreadInfo;
	CWinThread *m_pSendDataThread;
	CWinThread *m_pBmpGenerationThread;
	int m_nStartPrint;

	void SearchNewJob(void);
	void SendErrorBlock(double dScaleX,double dScaleY);
	void StartAJob(void);
	void CheckWorkStatus(void);
	void ChangeLanguage(void);
	void LinkAndReadPara(BOOL bDisplayFlag);
	int ReadParaFromPlotter(void);
	void InitHPGL();
	void FillListCtrl(int nSelect=-1);
	void GLTextMinXMaxX(CGLText *pText);
	void SetRatio();
	double GetAngle(CDPoint dp0,CDPoint dp1);
	double GetDistance(CDPoint dp0,CDPoint dp1);
	BOOL ThreePointOnLine(CDPoint dp0,CDPoint dp1,CDPoint dp2);
	BOOL IsDigit(char ch);
	void RemoveGLBuffer();
	BOOL IsAlpha(char ch);
	double ConvertPEValue(int nPeValue, int nFraction);
	BOOL GetPeData(int &nPeValue, unsigned char ch,int nMode, int &nMultiplier);
	int GetParameterType(char cName[]);
	void ProcessInstruct(CInstruct *pInstruct);
	int OpenFile(CString strFileName);
	int OpenDissecretFile(CString strFileName);
	int OpenSecretFile(CString strFileName);
	CString ScanFileSection(CString strFileName,  long lPictureLength);  //20140715
	int QuickScanFilePG(CString strFileName);	//20140715
	CString CPlotCenterDlg::NewFileReentrant(CString strFileName); //20140722
	CString EtFileSection(CString strFileName);//20150514rwt

	CPlotCenterDlg(CWnd* pParent = NULL);	// standard constructor

	CString	m_filePath;//20160122切割指令识别
	void	InitPara();//20160122切割指令识别
	CString			m_fileContent;//20160122切割指令识别
	CStringArray	m_hpglArray;//20160122切割指令识别
	//CPlotContext    m_plotContext;//20160122切割指令识别

	bool	LoadAsHpgl(const CString & content);//20160122切割指令识别
	bool	FillShapeArray(const CStringArray & hpglArray);//20160122切割指令识别
	bool	ProcessHpglCmd(CString & hpglCmd, CPlotContext & plotContext);//20160122切割指令识别
	bool	GroupLines();//20160122切割指令识别

	CString	PltFileChangePage(CString filePath);//gzr20161024
	//void	GetMinMaxSize();//20160122切割指令识别
	int		m_maxWordWidth;//20160122切割指令识别
	int		m_maxWordHeight;//20160122切割指令识别
	bool	FetchANumber(const CString & hpglCmd, int & i, long & result);//20160122切割指令识别
	bool	FetchANumber(const CString & hpglCmd, int & i, double & result);//20160122切割指令识别
	int		GetAngleX(double x, double y);//20160122切割指令识别
	void	ClearUpIntPtSet(CIntPointSet & intPtSet);//20160122切割指令识别
	void	FindLineUnicursal(CIntPointSet & intPtSet);//20160122切割指令识别
	void	ConnectLines(CIntPointSet & intPtSet);//20160122切割指令识别
	void	RemovePointLine();//20160122切割指令识别
	void	ConnectUnbentLines();//20160122切割指令识别
	void	SetClosedFlags();//20160122切割指令识别
	void	FilterClosedLines();//20160122切割指令识别
	void	SetCuttedFlags();//20160122切割指令识别
	void	IdentifyTTypeCut();//20160122切割指令识别
	void	SetDrawPens(BYTE pen);//20160122切割指令识别
	void	GetCircumRect(CPolyLine * pPolyLine, CRect * pRect);//20160122切割指令识别
	bool	PointOnPolyLine(CPolyLine * pPolyLine, CMyPoint * pPtBg, CMyPoint * pPtEd);//20160122切割指令识别
	bool	PointOnLine(CMyPoint * pPtLBg, CMyPoint * pPtLEd, CMyPoint * pPit);//20160122切割指令识别
	bool	PointInClosedGraph(CPolyLine *	pPolyLine, CPoint pit);//20160122切割指令识别
	bool	GetOnLinePt(CMyPoint * ptBeg, CMyPoint * ptEnd, long value, long & result, bool valX = true);//20160122切割指令识别
	void	OrderScanValue(CUIntArray & array);//20160122切割指令识别
	bool	CoalitionPolyLine(CPolyLine * pTargetPL, CPolyLine * pSourcePL);//20160122切割指令识别
	bool	Pagination(long length, CString & hpgl);//20160122切割指令识别
	bool	OutputByCom();//20160122切割指令识别
	CPlotCenterDlg	* m_pDoc;//20160122切割指令识别
	bool	SaveHPGLFile(CString filename, CString & hpgl);//20160122切割指令识别
	bool	GetDirectionX(int escapement, double & x, double & y);//20160122切割指令识别
	BYTE		m_curCutPen;//20160122切割指令识别
	public:
	CPlotContext *	GetPlotContext();//20160122切割指令识别
	CPlotContext *	m_pPltContxt;//20160122切割指令识别


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
	//{{AFX_DATA(CPlotCenterDlg)
	enum { IDD = IDD_PLOTCENTER_DIALOG };
	CProgressCtrl	m_wndPrintState;
	CPlotListCtrl	m_wndPlotItem;
	CString	m_strTotalFile;
	CString	m_strTotalLength;
	CString	m_strConnectState;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotCenterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPlotCenterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnAboutBox();
	afx_msg void OnOpen();
	afx_msg void OnOpen1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnSetNet();
	afx_msg void OnSetupPicture();
	afx_msg void OnSetupPlotter();
	afx_msg void OnAutoDrawDir();
	afx_msg void OnDown();
	afx_msg void OnUp();
	afx_msg void OnDelete();
	afx_msg void OnLink();
	afx_msg void OnSimplifiedChinese();
	afx_msg void OnEnglish();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBreak();
	afx_msg void OnClickPlotitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPause();
	afx_msg void OnRestart();
	afx_msg void OnDestroy();
	afx_msg void OnExit();
	virtual void OnCancel();
	afx_msg void OnPreview();   //20140715 文件预览
	afx_msg void OnPreviewFile();   //20140715  文件预览

	afx_msg LRESULT OnTransmitPercent(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnEndTransmitFile(WPARAM wParam,LPARAM lParam);
	afx_msg void OnHistRecords();
	afx_msg void OnPlotCenterHelp();
	afx_msg void OnTest135();
	afx_msg void OnTest160();
	afx_msg void OnTest180();
	afx_msg void OnTest200();
	afx_msg void OnTest220();
	afx_msg void OnSetUsb();
	afx_msg void OnParaIn();
	afx_msg void OnParaOut();
	afx_msg void OnRclickPlotitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVerifyFileCount();
	afx_msg void OnVerifyFixedPoint();
	afx_msg void OnDeleteFile();
	afx_msg void OnPauseFile();
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStartOrStop();
	afx_msg void OnRestartFile();
	afx_msg void OnDeleteFile2();
	afx_msg void OnPauseFile2();
	afx_msg void OnRestartFile2();
	afx_msg void OnOpenFile();
	afx_msg void OnOpenFile1();
	afx_msg void OnPlotcenterHelp2();
	afx_msg void OnUpFile();
	afx_msg void OnDownFile();
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

	SOCKET m_socket;

	CArray <CChinese_char,CChinese_char> m_aChinese_char;
  CArray <CEnglish_char,CEnglish_char> m_aEnglish_char;
	int m_nInternalLeading_E,m_nHeight_E,m_nDescent_E; //m_nHeight_E是windows中字体的Ascent-InternalLeading
	int m_nHeight_C,m_nWidth_C;
	CPoint m_ptCurrentPosition;   //当前位置(坐标)
	CPoint m_ptPrevPosition;      //前一位置(坐标)
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTCENTERDLG_H__30E37CFF_98D6_4D60_BAB2_6AED5A960B2A__INCLUDED_)
