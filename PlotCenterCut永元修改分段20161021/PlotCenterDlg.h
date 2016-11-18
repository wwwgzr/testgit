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

#define	PROCESS_MNEMONIC	0//����
#define PROCESS_PARAMETER	1//����
#define PROCESS_FIND			2//������

//����
#define LINE_TYPE_SOLID				0
#define LINE_TYPE_DASH				1		/* -------  */
#define LINE_TYPE_DOT				2		/* .......  */
#define LINE_TYPE_DASHDOT			3		/* _._._._  */
//ͼԪ����
#define PRIMITIVE_ARC				1  //���ߣ�Բ��X��Y����ǰ��X1��Y1��Բ����������Ϊ��ʱ��
#define PRIMITIVE_ARC3				2  //���㻡�ߣ����X��Y���е�X1��Y1���յ�x2,y2
#define PRIMITIVE_LINE				3  //�߶Σ���1 X1��Y1����2 X2��Y2,...
#define PRIMITIVE_CIRCLE			4  //Բ��x0,y0, �뾶 r
#define PRIMITIVE_POINT				5  //��x0,y0
#define PRIMITIVE_LINE3				6  //�߶Σ���1 X1��Y1����2 X2��Y2,��3 X3��Y3,
#define PRIMITIVE_SYMBOL			7  //���ţ�����ch ��1 X1��Y1����2 X2��Y2,...
#define PRIMITIVE_RECT				8  //�Խǣ���1 X1��Y1����2 X2��Y2,
#define PRIMITIVE_PG				9  //��ҳ
#define PRIMITIVE_LT				10 //����
#define PRIMITIVE_SP				11 //ѡ��
#define PRIMITIVE_LO				12 //��־ԭ��
#define PRIMITIVE_SI				13 //�����ַ��ߴ�
#define PRIMITIVE_DI				14 //���Է���
#define PRIMITIVE_LB				15 //��־
#define PRIMITIVE_DASH				16 //����
#define PRIMITIVE_DR				17 //��Է���
#define PRIMITIVE_CUTLINE			18//�и���//20150729
#define PRIMITIVE_DR				17 //��Է���λͼ
#define PRIMITIVE_FN				18 //��������λͼ


#define WM_ENDTRANSMITFILE		WM_USER+100  //�ļ��������
#define WM_TRANSMITPERCENT		WM_USER+101  //�ѷ��͵İٷֱ�
#define SECTION_WIDTH 1200		//1200���ҳ

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
	int m_nType;//����
	CIntArray m_anData;//����
	int m_nMaxPDLength;//������������
	int m_nMaxPDWidth;
	int m_nMinPDLength;//������С�����
	int m_nMinPDWidth;
};

class CGLText:public CObject
{
public:
	int m_nType;
	CString m_strText;   //�ı���
	CDoubleArray m_adPara;
	int	m_iWeight;	//����λͼ
	unsigned char m_cIsItali; //б��λͼ
	CString m_cFontName; //��������λͼ
};
class CGLText1:public CObject
{
public:
	CString m_strText;   //�ı���
	int m_nX;//���ִ�ӡ�������
	int m_nY;
	double m_dTextHeight;//���ָ߶ȺͿ��
	double m_dTextWidth;
	double m_dTextAngle;//���ֽǶ�
	int m_nMaxPDLength;//�ַ�����������
	int m_nMaxPDWidth;
	int m_nMinPDLength;//�ַ�����С�����
	int m_nMinPDWidth;
};

class CPlotItem : public CObject  {
public:
	CPlotItem();
	CPlotItem(CPlotItem &a);
	CPlotItem(CString strPlotName,int nCopyNum,long lFixedPointLength,long lPictureLength,long lPictureWidth,long lPaperWidth,long lMaxPlotLength,long lMaxPlotWidth,long m_lFileMinLength,long lMinPlotWidth);
	~CPlotItem();

public:
	CString m_strPlotName;  //��ͼ��
	int m_nState;						//״̬,=0 ����æ,=1 �ȴ�,=2 ��ͣ���ͣ�3�ļ�ɾ��,4�ļ�ɾ������λͼ�����߳��д���
													//5���ڴ�ӡ�ļ�����ͣ,6���ڴ�ӡ�ı���ͣ�ļ��������ݷ����̴߳���
	int m_nCopyNum;					//��ӡ����
	long m_lFixedPointLength;//��ӡ��ʼ����,��λ:mm
	long m_lPictureLength;	//ͼ�γ���
	long m_lPictureWidth;		//ͼ�ο��
	long m_lSendLength;			//�ѷ��ͳ���
	long m_lPaperWidth;//ֽ�ſ��
	long m_lMarkSpace;//�Խӷ����
	double m_dGapRight;//�ұ߾�
	int m_nRotate;//ͼ����ת��־=1��ת
	long m_lMaxPlotLength;	//ͼ�γ���
	long m_lMaxPlotWidth;		//ͼ�ο��
	long m_lMinPlotLength;//��Сͼ�γ���(����)
	long m_lMinPlotWidth;		//ͼ����С���

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
	void PrintTrackPaperError(double dTrackPaperError);//20100413������ֽ����ָ���
	void OpenHistoryFile(CString strFileName);

	int m_nLineType;
	int m_nPen;//�ʺ�
	int m_nPenPE;//PE�ʺ�
	CPoint m_dpOrigin;//ԭ������
	CPoint m_dpCurrent;//��ǰ����
	unsigned char m_cTerminator;//��־�ս��
	unsigned char m_cTerminatorMode;//ģʽ
	CString m_strText;//�ַ�
	CPoint m_dpP1,m_dpP2;//���ŵ�
	BOOL m_bPSValid;//�Ƿ��յ�PSָ��
	BOOL m_bIPValid;//IPָ���Ƿ���Ч
	BOOL m_bSCValid;//SCָ���Ƿ���Ч
	CRect m_rtHardLimitation;//Ӳ������
	double m_dRatioX,m_dRatioY;//��ǰ�����ű���
	BOOL m_bAbsolute;//��������
	double m_dTextAngle;//�ַ�����
	double m_dTextHeight;//�ַ��߶�
	double m_dTextWidth;//�ַ����
	int m_nTextPosition;//�ַ����ԭ���λ��
	BOOL m_bPenDown;//���Ƿ�����
	int m_nRotate;//��ת�Ƕ�
	struct stru_SC m_struSC;//SC����
	CObList m_listPlotItem;  //��ͼ��
	CMenu m_englistmenu;
	CMenu m_chinesemenu;
	CToolBar m_wndtoolbar;
	
	int m_nVersion;
	int m_nLanguage;
	int m_nCommunicateMode;
	int m_nWaitTime;//��¼�ȴ�ʱ��
	BOOL m_bWaitConnected;//�Ƿ���Ҫ������粢��������
	BOOL m_bCutFile;//�򿪻�ͼ����ʱ�ж��Ƿ������ӡ
	BOOL m_bTimerBusy;//�Ƿ��Ѿ��˳���ʱ���ж�
	BOOL m_bOpenFile;//�Ƿ����ڴ��ļ�
	BOOL m_bConnected;//=true �ѽ��������ӣ�=false û�н�������
	BOOL m_bNetworkBusy;//=true ���ڴ�ӡ
	int m_nLineWidth;//�߿�
	unsigned char m_cPrintStyle;//��ӡģʽ
	unsigned char m_cSprinklerCount;//��ͷ��
	unsigned char m_cSprinklerNo;//��ʼ��ͷ���
	unsigned char m_cPenNo;//�ʺ� 20101206 �ʺŵ���
	unsigned char m_cCutNo;//����
	//unsigned char m_cPrintCutSelect; //20101206 ��ӡ�и�ѡ��
	double m_dOverlapError;//��ֽ���20100413
	double m_dMalposedError;//˫���ӡ���
	double m_dTrackPaperError;//��ֽ���20100413
	short	m_sSprayErrorX12;//��ͷ���ص����
	short	m_sSprayErrorX23;
	short	m_sSprayErrorX34;
	short	m_sSprayErrorY12;//��ͷ���λ���
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

	CString	m_filePath;//20160122�и�ָ��ʶ��
	void	InitPara();//20160122�и�ָ��ʶ��
	CString			m_fileContent;//20160122�и�ָ��ʶ��
	CStringArray	m_hpglArray;//20160122�и�ָ��ʶ��
	//CPlotContext    m_plotContext;//20160122�и�ָ��ʶ��

	bool	LoadAsHpgl(const CString & content);//20160122�и�ָ��ʶ��
	bool	FillShapeArray(const CStringArray & hpglArray);//20160122�и�ָ��ʶ��
	bool	ProcessHpglCmd(CString & hpglCmd, CPlotContext & plotContext);//20160122�и�ָ��ʶ��
	bool	GroupLines();//20160122�и�ָ��ʶ��

	CString	PltFileChangePage(CString filePath);//gzr20161024
	//void	GetMinMaxSize();//20160122�и�ָ��ʶ��
	int		m_maxWordWidth;//20160122�и�ָ��ʶ��
	int		m_maxWordHeight;//20160122�и�ָ��ʶ��
	bool	FetchANumber(const CString & hpglCmd, int & i, long & result);//20160122�и�ָ��ʶ��
	bool	FetchANumber(const CString & hpglCmd, int & i, double & result);//20160122�и�ָ��ʶ��
	int		GetAngleX(double x, double y);//20160122�и�ָ��ʶ��
	void	ClearUpIntPtSet(CIntPointSet & intPtSet);//20160122�и�ָ��ʶ��
	void	FindLineUnicursal(CIntPointSet & intPtSet);//20160122�и�ָ��ʶ��
	void	ConnectLines(CIntPointSet & intPtSet);//20160122�и�ָ��ʶ��
	void	RemovePointLine();//20160122�и�ָ��ʶ��
	void	ConnectUnbentLines();//20160122�и�ָ��ʶ��
	void	SetClosedFlags();//20160122�и�ָ��ʶ��
	void	FilterClosedLines();//20160122�и�ָ��ʶ��
	void	SetCuttedFlags();//20160122�и�ָ��ʶ��
	void	IdentifyTTypeCut();//20160122�и�ָ��ʶ��
	void	SetDrawPens(BYTE pen);//20160122�и�ָ��ʶ��
	void	GetCircumRect(CPolyLine * pPolyLine, CRect * pRect);//20160122�и�ָ��ʶ��
	bool	PointOnPolyLine(CPolyLine * pPolyLine, CMyPoint * pPtBg, CMyPoint * pPtEd);//20160122�и�ָ��ʶ��
	bool	PointOnLine(CMyPoint * pPtLBg, CMyPoint * pPtLEd, CMyPoint * pPit);//20160122�и�ָ��ʶ��
	bool	PointInClosedGraph(CPolyLine *	pPolyLine, CPoint pit);//20160122�и�ָ��ʶ��
	bool	GetOnLinePt(CMyPoint * ptBeg, CMyPoint * ptEnd, long value, long & result, bool valX = true);//20160122�и�ָ��ʶ��
	void	OrderScanValue(CUIntArray & array);//20160122�и�ָ��ʶ��
	bool	CoalitionPolyLine(CPolyLine * pTargetPL, CPolyLine * pSourcePL);//20160122�и�ָ��ʶ��
	bool	Pagination(long length, CString & hpgl);//20160122�и�ָ��ʶ��
	bool	OutputByCom();//20160122�и�ָ��ʶ��
	CPlotCenterDlg	* m_pDoc;//20160122�и�ָ��ʶ��
	bool	SaveHPGLFile(CString filename, CString & hpgl);//20160122�и�ָ��ʶ��
	bool	GetDirectionX(int escapement, double & x, double & y);//20160122�и�ָ��ʶ��
	BYTE		m_curCutPen;//20160122�и�ָ��ʶ��
	public:
	CPlotContext *	GetPlotContext();//20160122�и�ָ��ʶ��
	CPlotContext *	m_pPltContxt;//20160122�и�ָ��ʶ��


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
	afx_msg void OnPreview();   //20140715 �ļ�Ԥ��
	afx_msg void OnPreviewFile();   //20140715  �ļ�Ԥ��

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

	SOCKET m_socket;

	CArray <CChinese_char,CChinese_char> m_aChinese_char;
  CArray <CEnglish_char,CEnglish_char> m_aEnglish_char;
	int m_nInternalLeading_E,m_nHeight_E,m_nDescent_E; //m_nHeight_E��windows�������Ascent-InternalLeading
	int m_nHeight_C,m_nWidth_C;
	CPoint m_ptCurrentPosition;   //��ǰλ��(����)
	CPoint m_ptPrevPosition;      //ǰһλ��(����)
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTCENTERDLG_H__30E37CFF_98D6_4D60_BAB2_6AED5A960B2A__INCLUDED_)
