// PlotCenterDlg.h : header file
#include "PlotListCtrl.h"
#include "Instruct.h"
#include "DPoint.h"
#include "Line.h"
#include "PlotBitmap.h"
#include "GdiplusHeaders.h"
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
#define LINE_TYPE_DOT					2		/* .......  */
#define LINE_TYPE_DASHDOT			3		/* _._._._  */
//ͼԪ����
#define PRIMITIVE_ARC					1  //���ߣ�Բ��X��Y����ǰ��X1��Y1��Բ����������Ϊ��ʱ��
#define PRIMITIVE_ARC3				2  //���㻡�ߣ����X��Y���е�X1��Y1���յ�x2,y2
#define PRIMITIVE_LINE				3  //�߶Σ���1 X1��Y1����2 X2��Y2,...
#define PRIMITIVE_CIRCLE			4  //Բ��x0,y0, �뾶 r
#define PRIMITIVE_POINT				5  //��x0,y0
#define PRIMITIVE_LINE3				6  //�߶Σ���1 X1��Y1����2 X2��Y2,��3 X3��Y3,
#define PRIMITIVE_SYMBOL			7  //���ţ�����ch ��1 X1��Y1����2 X2��Y2,...
#define PRIMITIVE_RECT				8  //�Խǣ���1 X1��Y1����2 X2��Y2,
#define PRIMITIVE_PG					9  //��ҳ
#define PRIMITIVE_LT					10 //����
#define PRIMITIVE_SP					11 //ѡ��
#define PRIMITIVE_LO					12 //��־ԭ��
#define PRIMITIVE_SI					13 //�����ַ��ߴ�
#define PRIMITIVE_DI					14 //���Է���
#define PRIMITIVE_LB					15 //��־
#define PRIMITIVE_DASH				16 //����
#define PRIMITIVE_DR					17 //��Է���
#define PRIMITIVE_FN					18 //��������

#define WM_ENDTRANSMITFILE		WM_USER+100  //�ļ��������
#define WM_TRANSMITPERCENT		WM_USER+101  //�ѷ��͵İٷֱ�
#define SECTION_WIDTH 7200		//1200���ҳ
#define SECTION_WIDTH1 1200		//1200���ҳ

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
class BitList:public CObject
{
	public:
		CPoint  pBtPoint;
		CPoint  bPostLU,bPostLD, bPostRU, bPostRD;
		//int m_wi;
		//int m_hi;
		int m_nMaxPDLength;//������������
		int m_nMaxPDWidth;
		int m_nMinPDLength;//������С�����
		int m_nMinPDWidth;
		BOOL m_bTransparent;
		Bitmap *pBitmap;
};
// 20121204 ����λͼ���ݽṹ
class BitPrimitive:public CObject
{
public:
	int	 m_BitPost[8];
	int  m_BitDispStyle[2];
	int  m_BitGDI[4];
	//BYTE  *m_BitData;
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
	CString m_cFontName; //��������
	unsigned char m_cIsItali; //б��
	int	m_iWeight;	//����
	
	CDoubleArray m_adPara;
};
class CGLText1:public CObject
{
public:
	CString m_strText;   //�ı���
	CString m_cFontName;	// �������� 20121218
	int m_nX;//���ִ�ӡ�������
	int m_nY;
	double m_dTextHeight;//���ָ߶ȺͿ��
	double m_dTextWidth;
	double m_dTextAngle;//���ֽǶ�
	unsigned char m_cIsItali;
	int m_iWeight;
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
	int m_iPlotpercent;				//2050714-rwt ��ӡ���ȣ��ٷֱ�
	long m_lPaperWidth;//ֽ�ſ��
	long m_lMarkSpace;//�Խӷ����
	double m_dGapRight;//�ұ߾�
	int m_nRotate;//ͼ����ת��־=1��ת
	long m_lMaxPlotLength;	//ͼ�γ���
	long m_lMaxPlotWidth;		//ͼ�ο��
	long m_lMinPlotLength;//��Сͼ�γ���(����)
	long m_lMinPlotWidth;		//ͼ����С���
	CTime m_ctimeMtime;

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

class CPlotCenterDlg : public CDialog
{
// Construction
public:
	void SortByTitle();
	void SortByMtime();
	bool m_bAutoSort;
	bool m_bSortByMTime;
	bool m_bSortByTitle;
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
	UINT  PlotProgressThreadProc(void);//20150714-rwt
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
	BOOL m_isFinishPlot;
	int m_nLineWidth;//�߿�
	unsigned char m_cPrintStyle;//��ӡģʽ
	unsigned char m_cSprinklerCount;//��ͷ��
	unsigned char m_cSprinklerNo;//��ʼ��ͷ���
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
	long m_lMaxPlotLength1;//�ϲ��ļ�
	long m_lMaxPlotWidth1;//�ϲ��ļ�
	long m_lMinPlotLength;
	long m_lMinPlotWidth;
	long m_lMaxPGLength;
	long m_lMaxPGWidth;
	short m_sPlotType;
	long m_lHavePlotBitmapNum;//20150714-rwt
	SendDataThreadInfo m_PlotThreadInfo;
	
	CWinThread *m_pPlotProgressThreadProc;
	CWinThread *m_pSendDataThread;
	CWinThread *m_pBmpGenerationThread;
	int m_nStartPrint;

	void SearchNewJob(void);
	void SendErrorBlock(double dScaleX,double dScaleY);
	void StartAJob(void);
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
	int OpenSecretFileNew(CString strFileName);

	CPlotCenterDlg(CWnd* pParent = NULL);	// standard constructor

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
	afx_msg void OnPreview();   //20110402 �ļ�Ԥ��
	virtual void OnCancel();
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
	afx_msg void OnPlotcenterHelp2();
	afx_msg void OnUpFile();
	afx_msg void OnDownFile();
	afx_msg void OnPreviewFile();   //20110402 �ļ�Ԥ��
	afx_msg void OnSortByMtime();
	afx_msg void OnSortByTitle();
	afx_msg void OnAutoSort();
	afx_msg void OnSetIU();
	afx_msg void OnSetInch();
	afx_msg void OnOutofmemoryPrintstate(NMHDR* pNMHDR, LRESULT* pResult);
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

	//20150318rwt add function
public:
	void SavedToHistoryFolder();
	void DeleteHistoryFilesOnTime();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTCENTERDLG_H__30E37CFF_98D6_4D60_BAB2_6AED5A960B2A__INCLUDED_)
