// PlotCenterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "PlotCenterDlg.h"
#include "PictureDlg.h"
#include "NetworkSetup.h"
#include "SetupPicture.h"
#include "SetupPlotter.h"
#include "FolderDialog.h"
#include "FileFixedPrint.h"
#include "SelectDialog.h"
#include "ViewHistory.h"
#include "VerifyCopies.h"
#include "math.h"
#include "ch375dll.h"
#include "Instruct.h"
#include "PlotBitmap.h"
#include "ZipCompress.h"
#include "stdio.h"
#include "FileDlg.h"
#include "PlotBitmap.h"



#include <direct.h>//20150316rwt copyfiles to history directory


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ±íÊ¾¿É¸Ä±ä´óÐ¡µÄÍ¼±êID
#ifndef OBM_SIZE
#define OBM_SIZE 32766
#endif

#define TIME_OUT_TIME 2
#define ID_TIMER_1  1  //¶¨Ê±Æ÷  ID_TIMER 20150714
#define T_TIMES_1  500  //¶¨Ê±,unit:ms   T_TIMES  20150714
#define ID_TIMER_2  2  //¶¨Ê±Æ÷  ID_TIMER  20150714
#define T_TIMES_2  2000  //¶¨Ê±,unit:ms   T_TIMES  20150714
BOOL bIsEncrypt;

#define MMPixelX		11.811023622
#define USB_PID_VID_0  0x55374348	//ÎªÄÜÔÚWIN7 64Î»ÏÂÊ¹ÓÃUSBÇý¶¯Ç©Ãû£¬±ØÐëÊ¹ÓÃÄ¬ÈÏ³§ÉÌID //
#define USB_PID_VID_1  0x4D495250	//±£³Ö»æÍ¼ÖÐÐÄ¼æÈÝÐÔ 20111121

extern double g_dScaleX;
extern double g_dScaleY;
extern int g_nVersion;
extern int g_nLineWidth;
extern long g_lMaxPlotLength;//×î´óÍ¼ÐÎ³¤¶Èg_lMaxPlotWidth
extern long g_lMaxPlotWidth;
extern long g_lHavePlotBitmapNum;//20150716-rwt
extern BOOL g_bisSendBMDataOver;//20150716-rwt
extern BOOL g_bisSendingPlotData;//20150720-rwt


int g_iDeviceIndex=0; //USBÉè±¸ºÅ20110311
CString m_fileContent1;
CString m_fileContent2;
extern int CreateList(CPlotItem *pPlotItem,int nCommunicateMode,SOCKET sock);
extern void RemoveGLBuffer1(void);
int g_nNewVersion;//Çø±ðÐÂ¾É°æ±¾£¬ÓÃÓÚÏòÏÂ¼æÈÝ20100413
UINT BmpGenerationThreadProc(LPVOID pParam);
UINT SendDataThreadProc(LPVOID pParam);
UINT UsbSendDataThreadProc(LPVOID pParam);
//UINT PlotTProgressThreadProc(LPVOID pParam);
UINT  CPlotCenterDlg:: PlotProgressThreadProc(void);

//UINT PlotTProgressThreadProc(LPVOID pParam);

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	int m_nLanguage;
// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// class CPlotItem
CPlotItem::CPlotItem()
{
	m_strPlotName.Empty();
	m_nState=1;
	m_nCopyNum=0;
	m_lFixedPointLength=0;
	m_lPictureLength=0;
	m_lPictureWidth=0;
	m_lSendLength=0;
	m_lPaperWidth=0;
	m_iPlotpercent =0;
	m_dGapRight=0;
	m_lMarkSpace=long(200*MMPixelX);
	m_nRotate=0;
	m_lMaxPlotLength=0;
	m_lMaxPlotWidth=0;
	m_lMinPlotLength=0;
	m_lMinPlotWidth=0;
	m_ctimeMtime=0;
}

CPlotItem::CPlotItem(CPlotItem &a)
{
  (*this) = a;
}

CPlotItem::CPlotItem(CString strPlotName,int nCopyNum,long lFixedPointLength,long lPictureLength,long lPictureWidth,long lPaperWidth,long lMaxPlotLength,long lMaxPlotWidth,long lMinPlotLength,long lMinPlotWidth)
{
  m_strPlotName=strPlotName;
  m_nState=1;
	m_nCopyNum=nCopyNum;
	m_lFixedPointLength=lFixedPointLength;
	m_lPictureLength=lPictureLength;
	m_lPictureWidth=lPictureWidth;
	m_lSendLength=0;
	m_iPlotpercent =0;
	m_lPaperWidth=lPaperWidth;
	m_dGapRight=theApp.m_dGapRight;
	m_lMarkSpace=long(theApp.m_dMarkSpace*MMPixelX);
	m_nRotate=theApp.m_nRotate;
	m_lMaxPlotLength=lMaxPlotLength;
	m_lMaxPlotWidth=lMaxPlotWidth;
	m_lMinPlotLength=lMinPlotLength;
	m_lMinPlotWidth=lMinPlotWidth;
//yhz--start
	CFile tempFile;
	CFileStatus rStatus;
	tempFile.GetStatus(strPlotName,rStatus);
	m_ctimeMtime=rStatus.m_mtime;
//yhz--end
}

CPlotItem::~CPlotItem()
{
}

void CPlotItem::operator=(CPlotItem& a)
{
  m_strPlotName=a.m_strPlotName;
  m_nState=a.m_nState;
	m_nCopyNum=a.m_nCopyNum;
	m_lFixedPointLength=a.m_lFixedPointLength;
	m_lPictureLength=a.m_lPictureLength;
	m_lPictureWidth=a.m_lPictureWidth;
	m_lSendLength=a.m_lSendLength;
	m_lPaperWidth=a.m_lPaperWidth;
	m_dGapRight=a.m_dGapRight;
	m_lMarkSpace=a.m_lMarkSpace;
	m_nRotate=a.m_nRotate;
	m_lMaxPlotLength=a.m_lMaxPlotLength;
	m_lMaxPlotWidth=a.m_lMaxPlotWidth;
	m_lMinPlotLength=a.m_lMinPlotLength;
	m_lMinPlotWidth=a.m_lMinPlotWidth;
}

/////////////////////////////////////////////////////////////////////////////
// CPlotCenterDlg dialog

CPlotCenterDlg::CPlotCenterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlotCenterDlg::IDD, pParent)
, m_iClientWidth(0)
, m_iClientHeight(0)
, m_iMinWidth(0)
, m_iMinHeight(0)
, m_pControlArray(NULL)
, m_iControlNumber(0)
, m_bShowSizeIcon(TRUE)
{
	//{{AFX_DATA_INIT(CPlotCenterDlg)
	m_strTotalFile = _T("0");
	m_strTotalLength = _T("0");
	m_strConnectState = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPlotCenterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlotCenterDlg)
	DDX_Control(pDX, IDC_PRINTSTATE, m_wndPrintState);
	DDX_Control(pDX, IDC_PLOTITEM, m_wndPlotItem);
	DDX_Text(pDX, IDC_TOTALFILE, m_strTotalFile);
	DDX_Text(pDX, IDC_TOTALLENGTH, m_strTotalLength);
	DDX_Text(pDX, IDC_CONNECTSTATE, m_strConnectState);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPlotCenterDlg, CDialog)
	//{{AFX_MSG_MAP(CPlotCenterDlg)
	ON_WM_PAINT()
	ON_COMMAND(ID_ABOUTBOX, OnAboutBox)
	ON_COMMAND(ID_OPEN, OnOpen)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_COMMAND(ID_SETNET, OnSetNet)
	ON_COMMAND(ID_SETUPPICTURE, OnSetupPicture)
	ON_COMMAND(ID_SETUPPLOTTER, OnSetupPlotter)
	ON_COMMAND(ID_AUTODRAWDIR, OnAutoDrawDir)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_COMMAND(ID_LINK, OnLink)
	ON_COMMAND(ID_SIMPLIFIEDCHINESE, OnSimplifiedChinese)
	ON_COMMAND(ID_ENGLISH, OnEnglish)
	ON_WM_TIMER()
	ON_COMMAND(ID_BREAK, OnBreak)
	ON_NOTIFY(NM_CLICK, IDC_PLOTITEM, OnClickPlotitem)
	ON_COMMAND(ID_PAUSE, OnPause)
	ON_COMMAND(ID_RESTART, OnRestart)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EXIT, OnExit)
	ON_MESSAGE(WM_TRANSMITPERCENT,OnTransmitPercent)
	ON_MESSAGE(WM_ENDTRANSMITFILE,OnEndTransmitFile)
	ON_COMMAND(ID_HISTRECORDS, OnHistRecords)
	ON_COMMAND(ID_PLOTCENTERHELP, OnPlotCenterHelp)
	ON_COMMAND(ID_TEST135, OnTest135)
	ON_COMMAND(ID_TEST160, OnTest160)
	ON_COMMAND(ID_TEST180, OnTest180)
	ON_COMMAND(ID_TEST200, OnTest200)
	ON_COMMAND(ID_TEST220, OnTest220)
	ON_COMMAND(ID_SETUSB, OnSetUsb)
	ON_COMMAND(ID_PARAIN, OnParaIn)
	ON_COMMAND(ID_PARAOUT, OnParaOut)
	ON_NOTIFY(NM_RCLICK, IDC_PLOTITEM, OnRclickPlotitem)
	ON_COMMAND(ID_VERIFYFILECOUNT, OnVerifyFileCount)
	ON_COMMAND(ID_VERIFYFIXEDPOINT, OnVerifyFixedPoint)
	ON_COMMAND(ID_DELETEFILE, OnDeleteFile)
	ON_COMMAND(ID_PAUSEFILE, OnPauseFile)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText )
	ON_BN_CLICKED(IDC_STARTORSTOP, OnStartOrStop)
	ON_COMMAND(ID_RESTARTFILE, OnRestartFile)
	ON_COMMAND(ID_DELETEFILE2, OnDeleteFile2)
	ON_COMMAND(ID_PAUSEFILE2, OnPauseFile2)
	ON_COMMAND(ID_RESTARTFILE2, OnRestartFile2)
	ON_COMMAND(ID_OPENFILE, OnOpenFile)
	ON_COMMAND(ID_PLOTCENTERHELP2, OnPlotcenterHelp2)
	ON_COMMAND(ID_UPFILE, OnUpFile)
	ON_COMMAND(ID_DOWNFILE, OnDownFile)
	ON_COMMAND(ID_BUTTONPREVIEW2, OnPreviewFile) //20110402 ´òÓ¡Ô¤ÀÀ
	ON_COMMAND(IDM_SORT_BY_MTIME, OnSortByMtime)
	ON_COMMAND(IDM_SORT_BY_TITLE, OnSortByTitle)
	ON_COMMAND(IDM_AUTO_SORT, OnAutoSort)
	ON_COMMAND(ID_MM, OnSetIU)
	ON_COMMAND(ID_INCH, OnSetInch)
	ON_COMMAND(ID_BUTTONPREVIEW, OnPreviewFile) //20110402 ´òÓ¡Ô¤ÀÀ
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_PRINTSTATE, OnOutofmemoryPrintstate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotCenterDlg message handlers

BOOL CPlotCenterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// ÉèÖÃ¶Ô»°¿òÎª¿É±ä´óÐ¡µÄ
	ModifyStyle(0,WS_SIZEBOX);

	// ÒÔ¶Ô»°¿òµÄ³õÊ¼´óÐ¡×÷Îª¶Ô»°¿òµÄ¿í¶ÈºÍ¸ß¶ÈµÄ×îÐ¡Öµ
	CRect rectDlg;
	GetWindowRect(rectDlg);
	m_iMinWidth = rectDlg.Width();
	m_iMinHeight = rectDlg.Height();

	// µÃµ½¶Ô»°¿òclientÇøÓòµÄ´óÐ¡ 
	CRect rectClient;
	GetClientRect(&rectClient);
	m_iClientWidth = rectClient.Width();
	m_iClientHeight = rectClient.Height();

	// LoadÍ¼±ê
	m_bmpSizeIcon.LoadOEMBitmap(OBM_SIZE);
	m_bmpSizeIcon.GetBitmap(&m_bitmap);

	// ´´½¨ÏÔÊ¾Í¼±êµÄ¾²Ì¬¿Ø¼þ²¢·ÅÔÚ¶Ô»°¿òÓÒÏÂ½Ç
	m_wndSizeIcon.Create(NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, CRect(0, 0, m_bitmap.bmWidth, m_bitmap.bmHeight), this, 0);
	m_wndSizeIcon.SetBitmap(m_bmpSizeIcon);
	m_wndSizeIcon.MoveWindow(m_iClientWidth - m_bitmap.bmWidth, m_iClientHeight - m_bitmap.bmHeight, m_bitmap.bmWidth, m_bitmap.bmHeight);

	// ÏÔÊ¾Í¼±ê
	m_wndSizeIcon.ShowWindow(m_bShowSizeIcon);

	// ¿Ø¼þÐÅÏ¢Êý×é
	static DLGCTLINFO dcMenuGroup[] = 
	{
		{IDC_TFILE, MOVEY, 100},
		{IDC_TLENGTH, MOVEXY, 100},
		{IDC_TOTALFILE, MOVEY, 100},
		{IDC_TOTALLENGTH, MOVEXY, 100},
		{IDC_STATIC1, MOVEY, 100},
		{IDC_PRINTSTATE, ELASTICX, 100},
		{IDC_PRINTSTATE, MOVEY, 100},
		{IDC_CONNECTSTATE, MOVEXY, 100},
		{IDC_PLOTITEM, ELASTICXY, 100},
	};

	// ÉèÖÃ¿Ø¼þÐÅÏ¢
	SetControlProperty(dcMenuGroup, sizeof(dcMenuGroup)/sizeof(DLGCTLINFO));

	// TODO: Add extra initialization here
	this->m_nLanguage = theApp.m_nLanguage;
	this->m_nCommunicateMode = theApp.m_nCommunicateMode;

	if(0==theApp.m_bAutoSort) this->m_bAutoSort=FALSE;
	else this->m_bAutoSort=TRUE;

	if(0==theApp.m_bSortByMtime) this->m_bSortByMTime=FALSE;
	else this->m_bSortByMTime=TRUE;

	if(0==theApp.m_bSortByTitle) this->m_bSortByTitle=FALSE;
	else this->m_bSortByTitle=TRUE;

	m_pSendDataThread=NULL;
	m_pBmpGenerationThread=NULL;
	m_bCutFile = true;
	m_bTimerBusy = true;
	m_bOpenFile = false;
	m_bConnected = false;
	m_bNetworkBusy = false;
	m_isFinishPlot = false;
	m_bWaitConnected = true;
	m_nWaitTime = 0;
	m_cPrintStyle = 1;
	m_nLineWidth = 2;
	m_cSprinklerCount = 1;
	m_cSprinklerNo = 0;
	m_dOverlapError = 0.0; //20100413 ×ßÖ½Îó²î³õÊ¼»¯
	m_dMalposedError = 0;
	m_sSprayErrorX12 = 0;
	m_sSprayErrorX23 = 0;
	m_sSprayErrorX34 = 0;
	m_sSprayErrorY12 = 0;
	m_sSprayErrorY23 = 0;
	m_sSprayErrorY34 = 0;
	m_dPlotterXError = 1000.0;
	m_dPlotterYError = 1000.0;
	m_lMaxPlotLength = 0;
	m_lMaxPlotWidth = 0;
	m_lMinPlotLength = 0;
	m_lMinPlotWidth = 0;
	m_lMaxPGLength = 0;
	m_lMaxPGWidth = 0;
	m_lHavePlotBitmapNum = 0;
	m_sPlotType = 1630;
	m_nStartPrint = 0;

	m_wndPrintState.SetRange(0,100);
	m_englistmenu.LoadMenu(IDR_MAINMENU_EN);
	HMENU m_hMenu=::GetMenu(this->m_hWnd); // ±£´æÖÐÎÄ²Ëµ¥
	m_chinesemenu.Attach(m_hMenu);
	if (!m_wndtoolbar.CreateEx( this,TBSTYLE_FLAT ,  WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS,
		CRect(4,4,0,0)) ||	!m_wndtoolbar.LoadToolBar(IDR_TOOLBAR) )
	{
		TRACE0("failed to create toolbar\n");
		return false;
	}
	m_wndtoolbar.ShowWindow(SW_SHOW);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	ChangeLanguage();
	LinkAndReadPara(false);
	UpdateData(false);

//yhzÔö¼ÓÅÅÐòµÄ²Ëµ¥
	if (m_bAutoSort==TRUE)
	{
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_AUTO_SORT,MF_BYCOMMAND|MF_CHECKED);
		if (m_bSortByMTime==m_bSortByTitle)
		{
			m_bSortByMTime=TRUE;
			m_bSortByTitle=FALSE;
		}
		if (m_bSortByMTime==TRUE)
		{
			GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SORT_BY_MTIME,MF_BYCOMMAND|MF_CHECKED);
		}
		if (m_bSortByTitle==TRUE)
		{
			GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SORT_BY_TITLE,MF_BYCOMMAND|MF_CHECKED);
		}
	}
	else
	{
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_AUTO_SORT,MF_BYCOMMAND|MF_UNCHECKED);
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SORT_BY_MTIME,MF_BYCOMMAND|MF_UNCHECKED);
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SORT_BY_TITLE,MF_BYCOMMAND|MF_UNCHECKED);
		m_bAutoSort=FALSE;
		m_bSortByMTime=FALSE;
		m_bSortByTitle=FALSE;
	}
//yhzÔö¼ÓÅÅÐòµÄ²Ëµ¥

	SetTimer(ID_TIMER_1,T_TIMES_1,NULL);
	SetTimer(ID_TIMER_2,T_TIMES_2,NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPlotCenterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CPlotCenterDlg::OnAboutBox() 
{
	// TODO: Add your command handler code here
	BOOL bWaitConnected;
	bWaitConnected = m_bWaitConnected;
	m_bWaitConnected = false;
	CAboutDlg dlgAbout;

  dlgAbout.DoModal();
	m_bWaitConnected =bWaitConnected;
}


void CPlotCenterDlg::OnOpen() 
{
	display=0;
	displayx1=0;
	displayx2=0;
	hbwj=0;
	pg=0;
	pgnum=0;
	pgg=0;
	m_lMaxPlotLength1=0;
	CString m_fileContent;
	m_fileContent.Empty();
	m_fileContent1.Empty();
	m_fileContent2.Empty();
	hbwjname.Empty();
	// TODO: Add your command handler code here
	char Custer[80]="HP-GL(*.plt)\0*.plt\0All file(*.*)\0*.*\0\0";
	BOOL bWaitConnected;

	bWaitConnected = m_bWaitConnected;
	m_bWaitConnected = false;
	//CFileDlg dlg(TRUE);



//´ò¿ª¶à¸öÎÄ¼þ
	CFileDlg dlg(TRUE, 
	NULL, 
	NULL, 
    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |OFN_ALLOWMULTISELECT|OFN_ENABLESIZING,
    _T("HP-GL(*.plt)\0*.plt\0All file(*.*)\0*.*\0\0"),
    NULL);
    //×î¶à¿ÉÒÔ´ò¿ª20¸öÎÄ¼þ
    dlg.m_ofn.nMaxFile = 20 * MAX_PATH; 
	dlg.m_ofn.lpstrFile = new TCHAR[dlg.m_ofn.nMaxFile];
    dlg.m_ofn.lpstrFilter=Custer;
	if (theApp.m_nLanguage == 1)
	{
	dlg.m_ofn.lpstrTitle="Open";
    }
    ZeroMemory(dlg.m_ofn.lpstrFile, sizeof(TCHAR) * dlg.m_ofn.nMaxFile);
	//ÏÔÊ¾ÎÄ¼þ¶Ô»°¿ò£¬»ñµÃÎÄ¼þÃû¼¯ºÏ
    int retval;
    if(retval==IDCANCEL)
    return;

//´ò¿ª¶à¸öÎÄ¼þ



	//dlg.m_ofn.lpstrFilter=Custer;
	if(dlg.DoModal()==IDOK && (hbwj==0||hbwj==1))
	{

//´ò¿ª¶à¸öÎÄ¼þ	//¸ù¾ÝÀ©Õ¹Ãû¶ÁÈ¡Ïà¹ØÎÄ¼þ
    POSITION pos_file;
    pos_file = dlg.GetStartPosition();
    CArray<CString, CString> ary_filename;
    while(pos_file != NULL)
    ary_filename.Add(dlg.GetNextPathName(pos_file));
      for(int i=0; i<ary_filename.GetSize(); i++)
	  {
      CString strFileName;
      strFileName = ary_filename.GetAt(i);

	    m_bOpenFile=true;

		int nReturnValue=OpenFile(strFileName);
		if(nReturnValue==0)
		{
			CPictureDlg dlgPicture;
			long lPaperWidth = (long)(theApp.m_dPaperWidth * 40);
			if (m_lMaxPlotWidth > lPaperWidth)
			{
				dlgPicture.m_lFileLength = m_lMaxPlotLength;
				dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
				dlgPicture.m_lFileMinLength = m_lMinPlotLength;
				dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
				dlgPicture.m_lTotalX = (m_lMaxPlotLength + 10 * 40) * long((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth) - 10 * 40;
				dlgPicture.m_lTotalY = lPaperWidth;
			}
			else
			{
				dlgPicture.m_lFileLength = m_lMaxPlotLength;
				dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
				dlgPicture.m_lFileMinLength = m_lMinPlotLength;
				dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
				dlgPicture.m_lTotalX = m_lMaxPlotLength;
				dlgPicture.m_lTotalY = m_lMaxPlotWidth;
			}
			dlgPicture.m_strPictureName = strFileName;
			dlgPicture.m_busyPrint = m_bNetworkBusy;
			if(dlgPicture.DoModal()==IDOK)
			{

				CString strCaption,strCaption1;
			   TCHAR szDir[_MAX_DIR];
				TCHAR szDrive[_MAX_DRIVE];
				TCHAR szFullPath[_MAX_PATH];
				::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
				_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
				 strCaption1.Format(_T("%s%sPlot\\"),szDrive,szDir);
				_splitpath(strFileName,szDrive,szDir,NULL,NULL);
				 strCaption.Format(_T("%s%s"),szDrive,szDir);


				if ((strCaption.CompareNoCase(strCaption1) == 0) ||
					(strCaption.CompareNoCase(theApp.m_strDataDir) == 0))
				{
					CString s1;
					if (m_nLanguage == 0)
						s1.LoadString(IDS_STRING295);
					else
						s1.LoadString(IDS_STRING295_EN);
					AfxMessageBox(s1);
				}
				else
				{
					int nCopyNum = dlgPicture.m_nCopyNum;;
					long lFixedPointLength = dlgPicture.m_lFixedPointLength;

					if (lFixedPointLength>=ftoi(m_lMaxPlotLength*0.025))
					{
						CString s1;
						if (m_nLanguage == 0)
							s1.LoadString(IDS_STRING299);
						else
							s1.LoadString(IDS_STRING299_EN);
						AfxMessageBox(s1);
					}
					else if ((m_lMaxPlotLength!=0)&&(m_lMaxPlotWidth!=0))
					{
						CPlotItem *pPlotItem;			
						pPlotItem=new CPlotItem(strFileName,nCopyNum,lFixedPointLength,ftoi(dlgPicture.m_lTotalX*0.025),ftoi(dlgPicture.m_lTotalY*0.025),lPaperWidth,m_lMaxPlotLength,m_lMaxPlotWidth,m_lMinPlotLength,m_lMinPlotWidth);
						m_listPlotItem.AddTail(pPlotItem);
						FillListCtrl();
					}
				}
			}
		}
		else if(nReturnValue==1)
		{//ÎÄ¼þ´ò²»¿ª
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING106);
			else
				s1.LoadString(IDS_STRING106_EN);
			AfxMessageBox(s1);
		}
	}
	}
//´ò¿ª¶à¸öÎÄ¼þ
	CString str_HB;
	if (hbwj == 2)
	{
		str_HB = "PG;";
	}
	else if (hbwj == 3)
	{
		str_HB = "PG;PPG;";
		hbwj = 2;
	}

	if(hbwj==2)//ºÏ²¢ÎÄ¼þ
	{

//´ò¿ª¶à¸öÎÄ¼þ	//¸ù¾ÝÀ©Õ¹Ãû¶ÁÈ¡Ïà¹ØÎÄ¼þ
    POSITION pos_file;
    pos_file = dlg.GetStartPosition();
    CArray<CString, CString> ary_filename;
    while(pos_file != NULL)
    ary_filename.Add(dlg.GetNextPathName(pos_file));


	 for(int i1=0; i1<ary_filename.GetSize(); i1++)
	 {
      CString strFileName;
	  strFileName.Empty();
      strFileName = ary_filename.GetAt(i1);

	  int iPos = strFileName.ReverseFind('\\');
	  if (hbwjname=="")
	  {
      hbwjname = hbwjname + strFileName.Right(strFileName.GetLength() - iPos - 1);	
	  }
	  else
	  {
      hbwjname = hbwjname + " & " + strFileName.Right(strFileName.GetLength() - iPos - 1);	
	  }


	 char *	pBuf = new	char[64*1024 + 1];
	 if (pBuf == NULL)
		return;
	 TRY
	 {
		CFile file(strFileName, CFile::modeRead | CFile::typeBinary);
		UINT	n = file.GetLength();
		UINT	i2, j;
		i2 = 0;
		j = (n - i2) < 64 * 1024 ? (n - i2) : 64 * 1024;
		file.SeekToBegin();

		while(i2 < n && j >0)
		{
			file.Read(pBuf, j);
			pBuf[j] = '\0';
			m_fileContent =m_fileContent + pBuf;
			i2 += j;
			j = (n - i2) < 64 * 1024 ? (n - i2) : 64 * 1024;
		};
        int pgfind = 0;
		int filelen = 0;
		pgfind = m_fileContent.Find("PG;");
        filelen = m_fileContent.GetLength();
		if(pgfind==-1)
		{	     
		 //m_fileContent1 = m_fileContent1 + m_fileContent + "PG;PPG;";
		 m_fileContent1 = m_fileContent1 + m_fileContent + str_HB;
		 m_fileContent.Empty();
		}
		else
		{
			if( pgfind == filelen-3 )
			{
				//m_fileContent1 = m_fileContent1 + m_fileContent + "PG;PPG;";
				m_fileContent1 = m_fileContent1 + m_fileContent + str_HB;
			}
			else
			{
	            //m_fileContent2 = m_fileContent2 + m_fileContent + "PG;PPG;";
				m_fileContent2 = m_fileContent2 + m_fileContent + str_HB;
			}
                m_fileContent.Empty();
		}

		file.Close();					// ¹Ø±ÕÎÄ¼þ
	 }
	 CATCH( CFileException, e )
	 {
		#ifdef _DEBUG
			afxDump << "File could not be opened " << e->m_cause << "\n";
		#endif
		AfxMessageBox ("File could not be opened ");
        strFileName.Empty();
		m_fileContent.Empty();
		delete	[]pBuf;
		return;
	 }
	 END_CATCH
	 delete	[]pBuf;

	 }
        m_fileContent = m_fileContent2 + m_fileContent1;

        hbwjname="C:\\"+hbwjname;
        int n=m_fileContent.GetLength();
		CFile file(hbwjname, CFile::modeCreate | CFile::modeWrite);
		file.Write(m_fileContent, n);
		file.Close();


		//½«¾ÉÎÄ¼þÉ¾³ý£¬»ñµÃ¾ÉÎÄ¼þÃü
		CString oldHBname;
		oldHBname.Empty();
		char *	pBuf1 = new	char[64*1024 + 1];
	    if (pBuf1 == NULL)
		return;
	    TRY
		{
		CFile file2("C:\\HBName.txt", CFile::modeRead | CFile::typeBinary);
		UINT	n1 = file2.GetLength();
		UINT	i3, j1;
		i3 = 0;
		j1 = n1;
		file2.SeekToBegin();

		while(i3 < n1 && j1 >0)
		{
			file2.Read(pBuf1, j1);
			pBuf1[j1] = '\0';
			oldHBname += pBuf1;
			i3 += j1;
			j1 = n1;
		};
		file2.Close();					// ¹Ø±ÕÎÄ¼þ
		}
	    CATCH( CFileException, e )
		{
		CFile file3("C:\\HBName.txt", CFile::modeCreate | CFile::modeWrite);
		file3.Write("", 0);
		file3.Close();
		}
    	END_CATCH
	    delete	[]pBuf1;


  
		char oldfilename[100];
        strncpy(oldfilename,(LPCTSTR)oldHBname,sizeof(oldfilename));
		char newfilename[100];
		strncpy(newfilename,(LPCTSTR)hbwjname,sizeof(newfilename));
        //int bbb=rename(oldfilename,newfilename);//ÎªÁãµÄ»°ÖØÃüÃû³É¹¦
		if(oldHBname!=hbwjname)
		{
		int aaa=remove(oldfilename);//ÎªÁãµÄ»°É¾³ýÎÄ¼þ³É¹¦
		}
		

        int n1=hbwjname.GetLength();
		CFile file1("C:\\HBName.txt", CFile::modeCreate | CFile::modeWrite);
		file1.Write(hbwjname, n1);
		file1.Close();




	    m_bOpenFile=true;
		int nReturnValue=OpenFile(hbwjname);
		if(nReturnValue==0)
		{
			CPictureDlg dlgPicture;
			long lPaperWidth = (long)(theApp.m_dPaperWidth * 40);
			if (m_lMaxPlotWidth > lPaperWidth)
			{
				dlgPicture.m_lFileLength = m_lMaxPlotLength;
				dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
				dlgPicture.m_lFileMinLength = m_lMinPlotLength;
				dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
				dlgPicture.m_lTotalX = (m_lMaxPlotLength + 10 * 40) * long((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth) - 10 * 40;
				dlgPicture.m_lTotalY = lPaperWidth;
			}
			else
			{
				dlgPicture.m_lFileLength = m_lMaxPlotLength;
				dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
				dlgPicture.m_lFileMinLength = m_lMinPlotLength;
				dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
				dlgPicture.m_lTotalX = m_lMaxPlotLength;
				dlgPicture.m_lTotalY = m_lMaxPlotWidth;
			}
			dlgPicture.m_strPictureName = hbwjname;
			dlgPicture.m_busyPrint = m_bNetworkBusy;
			if(dlgPicture.DoModal()==IDOK)
			{
				pg=0;//ºÏ²¢ÎÄ¼þ
				pgnum=0;//ºÏ²¢ÎÄ¼þ
				pgg=0;//ºÏ²¢ÎÄ¼þ
				CString strCaption,strCaption1;
			  TCHAR szDir[_MAX_DIR];
				TCHAR szDrive[_MAX_DRIVE];
				TCHAR szFullPath[_MAX_PATH];
				::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
				_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
				strCaption1.Format(_T("%s%sPlot\\"),szDrive,szDir);
				_splitpath(hbwjname,szDrive,szDir,NULL,NULL);
				strCaption.Format(_T("%s%s"),szDrive,szDir);
				if ((strCaption.CompareNoCase(strCaption1) == 0) ||
					(strCaption.CompareNoCase(theApp.m_strDataDir) == 0))
				{
					CString s1;
					if (m_nLanguage == 0)
						s1.LoadString(IDS_STRING295);
					else
						s1.LoadString(IDS_STRING295_EN);
					AfxMessageBox(s1);
				}
				else
				{
					int nCopyNum = dlgPicture.m_nCopyNum;;
					long lFixedPointLength = dlgPicture.m_lFixedPointLength;

					if (lFixedPointLength>=ftoi(m_lMaxPlotLength*0.025))
					{
						CString s1;
						if (m_nLanguage == 0)
							s1.LoadString(IDS_STRING299);
						else
							s1.LoadString(IDS_STRING299_EN);
						AfxMessageBox(s1);
					}
					else if ((m_lMaxPlotLength!=0)&&(m_lMaxPlotWidth!=0))
					{
						CPlotItem *pPlotItem;			
						pPlotItem=new CPlotItem(hbwjname,nCopyNum,lFixedPointLength,ftoi(dlgPicture.m_lTotalX*0.025),ftoi(dlgPicture.m_lTotalY*0.025),lPaperWidth,m_lMaxPlotLength,m_lMaxPlotWidth,m_lMinPlotLength,m_lMinPlotWidth);
						m_listPlotItem.AddTail(pPlotItem);
						FillListCtrl();
					}
				}
			}
		}
		else if(nReturnValue==1)
		{//ÎÄ¼þ´ò²»¿ª
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING106);
			else
				s1.LoadString(IDS_STRING106_EN);
			AfxMessageBox(s1);
		}
	}
		//else
		//{//·ÇHP-GLÎÄ¼þ!
			//CString s1;
			//if (m_nLanguage == 0)
				//s1.LoadString(IDS_STRING108);
			//else
				//s1.LoadString(IDS_STRING108_EN);
			//AfxMessageBox(s1);
			//return;
		//}
		RemoveGLBuffer();
		m_bOpenFile=false;

	m_bWaitConnected =bWaitConnected;
}
/*
#define BIGFILE	5000000L
//²ð·ÖÎÄ¼þ´óÐ¡5MÒ»¸ö
int DetachFile(CString strFileName)
{
	CFile file;
	CFile NewFile;
	BYTE  *pBuf;
	CString tmpName;
	char head[] = "SP1;LT;";
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return 1;
	int nLength=file.GetLength();
	int detachNum = nLength / BIGFILE + 1;
	int realByteNum;
	int FilePos;
	char ch1=0, ch2=0;
	int j=0;

	for(int i=0; i<detachNum; i++)
	{
		j = 0;
		tmpName.Format("_%d", i); 
		if(!NewFile.Open(strFileName+tmpName,CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
			return 1;
		
		FilePos = file.GetPosition();
		file.Seek(BIGFILE+j, CFile::current);
		do
		{
			file.Read(&ch1, 1);
			j++;
			if(ch1 == 'P')
			{
				file.Read(&ch1, 1);
				j++;
				if(ch1 == 'U')
				{
					break;
				}
			}
		}while(1);
		file.Seek(-(BIGFILE+j), CFile::current);
		FilePos = file.GetPosition();
		NewFile.Write(head, 7);		
		pBuf = new BYTE[BIGFILE+j];
		realByteNum = file.Read(pBuf,BIGFILE+j-2);	
		NewFile.Write(pBuf, realByteNum);
		delete []pBuf;
		NewFile.Close();
	}
	return 0;
}
*/
//bDisplayFlag=trueÏÔÊ¾´íÎó;=false²»ÏÔÊ¾´íÎó
//·µ»Ø0Õý³£´ò¿ªÎÄ¼þ;=1ÎÄ¼þ²»´æÔÚ;=2ÎÄ¼þ¸ñÊ½´í

int CPlotCenterDlg::OpenFile(CString strFileName)
{
		CFile file;
	//20100406ÐÂÔöCFile::shareExclusiveÊôÐÔ£¬±ÜÃâÆäËûCADÎÄ¼þÐ´×Ô¶¯»æÍ¼Ä¿Â¼Ê±Ð´Èë²»³É¹¦»ò´òÓ¡È±Ò³
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return 1;

	int nLength=file.GetLength();
	int nReturnValue;

	if (nLength >= 22)
	{
		char cFileBuf[23];
		file.Read(&cFileBuf[0],22);
		file.Close();
		if ((cFileBuf[0] == 0x66) && (cFileBuf[1] == 0x53) &&
			(strncmp(&cFileBuf[3],"Richpeace plot file",19) == 0))
			//nReturnValue = OpenSecretFile(strFileName);
			nReturnValue = OpenSecretFileNew(strFileName);
		else
			nReturnValue = OpenDissecretFile(strFileName);
	}
	else
	{
		file.Close();
		nReturnValue = OpenDissecretFile(strFileName);
	}
	return nReturnValue;
}

int CPlotCenterDlg::OpenDissecretFile(CString strFileName)
{
	CFile file;
	//20100406ÐÂÔöCFile::shareExclusiveÊôÐÔ£¬±ÜÃâÆäËûCADÎÄ¼þÐ´×Ô¶¯»æÍ¼Ä¿Â¼Ê±Ð´Èë²»³É¹¦»ò´òÓ¡È±Ò³
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
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

	InitHPGL();
	InitTextChangeLine();
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
	nProcess=PROCESS_MNEMONIC;
	nMnemonicCount=0;
	m=j=0;
	bIsEncrypt=FALSE;
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
			if ((i +1000 - k) < nLength)
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
			{//Ñ°ÕÒÏÂÒ»¸ö';'
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
			else if (nProcess == PROCESS_MNEMONIC)//Öú¼Ç·û
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
			{//²ÎÊý
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
					case NONE_TYPE:
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
					case DIGIT_TYPE:
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
						if(ch==m_cTerminator)
						{
							//add by yhz,Èç¹ûÊÇÁ¬ÐøµÄLBÖ¸Áî£¬Ôò½«ÆäºÏ²¢
							if (('L' == cFileBuf[k]) && ('B' == cFileBuf[k+1]))
							{
								ch=cFileBuf[k+2];
								k+=2;
							}
							//add by yhz,Èç¹ûÊÇÁ¬ÐøµÄLBÖ¸Áî£¬Ôò½«ÆäºÏ²¢
							else
							{
								if(m_cTerminatorMode==0)
								{
									CString str="1";
									str.SetAt(0,ch);
									m_strText+=str;
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
							m_strText+=str;
						}
						break;
					case DT_TYPE:
						ch = cFileBuf[k++];
						if(m_cTerminatorMode==0x81)
						{
							if(ch==',')
								m_cTerminatorMode=0x82;
							else
							{
								m_cTerminatorMode=1;
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
								k--;
							}
						}
						else if(m_cTerminatorMode==0x82)
						{
							if(ch==0 || ch==1)
								m_cTerminatorMode=ch;
							else
							{
								m_cTerminatorMode=1;
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
								k--;
							}
						}
						else
						{
							if(ch==';')
							{
								m_cTerminator=theApp.m_cTerminator;
								m_cTerminatorMode=1;
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
							}
							else if(ch!=0 && ch!=5 && ch!=27)
							{
								m_cTerminator=ch;
								m_cTerminatorMode=0x81;
							}
							else
							{
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
							}
						}
						break;
					default:
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
	m_lMaxPlotLength=m_lMaxPGLength-m_lMinPlotLength;
	if (m_lMaxPlotWidth < m_lMaxPGWidth)
		m_lMaxPlotWidth = m_lMaxPGWidth;
	m_lMaxPlotWidth -= m_lMinPlotWidth;
	if (theApp.m_nRotate == 1)
	{
		long lMaxPlotLength=m_lMaxPlotLength;
		m_lMaxPlotLength=m_lMaxPlotWidth;
		m_lMaxPlotWidth=lMaxPlotLength;
	}
	return 0;
}

#define DEBUG_OUTFILE   1
#if DEBUG_OUTFILE
#endif


int BitMapData(CString strFileName)
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
	CString strWorkPath1("c:\\bimap.dat");
#else	
	TCHAR szFullPath[_MAX_PATH];
  	TCHAR szDir[_MAX_DIR];
  	TCHAR szDrive[_MAX_DRIVE];
 	CString strWorkPath1;
	
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
	strWorkPath1 += "MVPIce.chv";
#endif
    CFile hFile;

	if(!hFile.Open(strWorkPath1,CFile::modeCreate | CFile::modeWrite))
	{
		return 0;
	}
	//SetFileAttributes(strWorkPath1, FILE_ATTRIBUTE_HIDDEN ); //Òþ²Ø

	// Attach a CFile object to the handle we have.
      
	CFile file;
	//20100406ÐÂÔöCFile::shareExclusiveÊôÐÔ£¬±ÜÃâÆäËûCADÎÄ¼þÐ´×Ô¶¯»æÍ¼Ä¿Â¼Ê±Ð´Èë²»³É¹¦»ò´òÓ¡È±Ò³
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


int CPlotCenterDlg::OpenSecretFileNew(CString strFileName)
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
#if _DEBUG
	unsigned char changBuf[2048];
	int cbLong=0;
#endif
	bDecodingFlg = BitMapData(strFileName);	
	if(!bDecodingFlg)
		return FALSE;
#if _DEBUG
	CString strWorkPath1("c:\\bimap.dat");
#else
	TCHAR szFullPath[_MAX_PATH];
  	TCHAR szDir[_MAX_DIR];
  	TCHAR szDrive[_MAX_DRIVE];
 	CString strWorkPath1;
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
	strWorkPath1 += "MVPIce.chv";
#endif	
	CFile file;
	//20100406ÐÂÔöCFile::shareExclusiveÊôÐÔ£¬±ÜÃâÆäËûCADÎÄ¼þÐ´×Ô¶¯»æÍ¼Ä¿Â¼Ê±Ð´Èë²»³É¹¦»ò´òÓ¡È±Ò³
	if(!file.Open(strWorkPath1,CFile::modeRead | CFile::shareExclusive))
		return 1;

#if _DEBUG
	CString strWorkPath2 = "c:\\bitdataCh.dat";
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
	bIsEncrypt = TRUE;
	nLength=file.GetLength();
	j=k=m=n=i=0;
	
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
			if (nProcess == PROCESS_MNEMONIC)//Öú¼Ç·û
			{
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
#if _DEBUG
					changBuf[cbLong++] = Instruct.m_cName[0];
					changBuf[cbLong++] = Instruct.m_cName[1];
					changBuf[cbLong++] = ch;
					changBuf[cbLong++] = ';';
#endif
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;

					}
					break;
				case DEFCMD_LT://0xe6:
					if ((j - k) < 1)
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
#if _DEBUG
					changBuf[cbLong++] = Instruct.m_cName[0];
					changBuf[cbLong++] = Instruct.m_cName[1];
					changBuf[cbLong++] = ch;
					changBuf[cbLong++] = ';';
#endif
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					}
					break;
					
				case DEFCMD_DI://0x5b: ×ÖÌå½Ç¶È
					if ((j - k) < 4)
					{
						m = k;
						k = j;
					}
					else
					{
					Instruct.m_cName[0]='D';
					Instruct.m_cName[1]='I';
					cPin = (unsigned char *)&nParameter;
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
					Instruct.m_adPara.Add(nParameter);
#if _DEBUG
					changBuf[cbLong++] = Instruct.m_cName[0];
					changBuf[cbLong++] = Instruct.m_cName[1];
					changBuf[cbLong++] = *(cPin+3);
					changBuf[cbLong++] = *(cPin+2);
					changBuf[cbLong++] = *(cPin+1);
					changBuf[cbLong++] = *cPin;
					changBuf[cbLong++] = ';';
#endif
					// ÊÇ·ñÊÇÐ±Ìå
					Instruct.m_adPara.Add(cFileBuf[k++]);
					// ×ÖÌå°õÊý
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

				case DEFCMD_SI://0x5d: ×ÖÌå¿í¡Á¸ß
					if ((j - k) < 16)
					{
						m = k;
						k = j;
					}
					else
					{
					Instruct.m_cName[0]='S';
					Instruct.m_cName[1]='I';
#if _DEBUG
					changBuf[cbLong++] = Instruct.m_cName[0];
					changBuf[cbLong++] = Instruct.m_cName[1];			
#endif
					cPin = (unsigned char *)&dParm;
					for(dloop=0; dloop<2; dloop++)
					{
						for(dCount=7; dCount>=0; dCount--)
						{
							*(cPin+dCount) = cFileBuf[k++];
#if _DEBUG
							changBuf[cbLong++] = *(cPin+dCount);
#endif
						}
#if _DEBUG
						changBuf[cbLong++] = ',';
#endif
						Instruct.m_adPara.Add(dParm);
						cPin = (unsigned char *)&dParm;
					}
#if _DEBUG
					changBuf[cbLong++] = ';';
#endif
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					}
					break;

				case DEFCMD_FN://0x4b: ×ÖÌåÃû
					if ((j - k) < 4)
					{
						m = k;
						k = j;
					}
					else
					{
					Instruct.m_cName[0]='F';
					Instruct.m_cName[1]='N';
#if _DEBUG
					changBuf[cbLong++] = Instruct.m_cName[0];
					changBuf[cbLong++] = Instruct.m_cName[1];			
#endif
					cPin = (unsigned char *)&dloop;
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
#if _DEBUG
					changBuf[cbLong++] = *(cPin+3);
					changBuf[cbLong++] = *(cPin+2);
					changBuf[cbLong++] = *(cPin+1);
					changBuf[cbLong++] = *cPin;
					changBuf[cbLong++] = ';';
#endif
					if ((j - k) < dloop)
					{
						k -= 5;
						m = k;
						k = j;
						break;
					}
					else
					{
					pStr=Instruct.m_cFontName.GetBuffer(dloop);
					for(dCount=0; dCount<dloop; dCount++)
					{
						*pStr++ = cFileBuf[k++];
#if _DEBUG
						changBuf[cbLong++] = *(pStr-1);
#endif
					}
#if _DEBUG
					changBuf[cbLong++] = ';';
#endif
					Instruct.m_cString.ReleaseBuffer();  //¸üÐÂ¶ÔÏóÄÚ²¿Êý¾Ý
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					}
					}
					break;

				case DEFCMD_LB://0x4d:
					if ((j - k) < 12)
					{
						m = k;
						k = j;
					}
					else
					{
					Instruct.m_cName[0]='L';
					Instruct.m_cName[1]='B';
#if _DEBUG
					changBuf[cbLong++] = Instruct.m_cName[0];
					changBuf[cbLong++] = Instruct.m_cName[1];			
#endif					
					//×Ö·û´®¸öÊý
					cPin = (unsigned char *)&dloop;
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
#if _DEBUG
					changBuf[cbLong++] = *(cPin+3);
					changBuf[cbLong++] = *(cPin+2);
					changBuf[cbLong++] = *(cPin+1);
					changBuf[cbLong++] = *cPin;
					changBuf[cbLong++] = ',';
#endif
					////////////////////////////////////////
					//×Ö·û´®×óÉÏ½ÇX×ø±ê
					cPin = (unsigned char *)&nParameter;
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
					Instruct.m_adPara.Add(nParameter);
#if _DEBUG
					changBuf[cbLong++] = *(cPin+3);
					changBuf[cbLong++] = *(cPin+2);
					changBuf[cbLong++] = *(cPin+1);
					changBuf[cbLong++] = *cPin;
					changBuf[cbLong++] = ',';
#endif
					/////////////////////////////////////////////
					//×Ö·û´®×óÉÏ½ÇY×ø±ê
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
					Instruct.m_adPara.Add(nParameter);
#if _DEBUG
					changBuf[cbLong++] = *(cPin+3);
					changBuf[cbLong++] = *(cPin+2);
					changBuf[cbLong++] = *(cPin+1);
					changBuf[cbLong++] = *cPin;
					changBuf[cbLong++] = ',';
#endif
					///////////////////////////////////////////
					//×Ö·û´®
					if ((j - k) < dloop)
					{
						k -= 13;
						m = k;
						k = j;
					}
					else
					{
					pStr=Instruct.m_cString.GetBuffer(dloop);
					for(dCount=0; dCount<dloop; dCount++)
					{
						*pStr++ = cFileBuf[k++];
#if _DEBUG
						changBuf[cbLong++] = *(pStr-1);
#endif
					}
#if _DEBUG
					changBuf[cbLong++] = ';';
#endif
					Instruct.m_cString.ReleaseBuffer();  //¸üÐÂ¶ÔÏóÄÚ²¿Êý¾Ý
					m_strText=Instruct.m_cString;
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					Instruct.m_cString.Empty();
					m_strText.Empty();
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
#if _DEBUG
					changBuf[cbLong++] = 'B';
					changBuf[cbLong++] = 'T';	
					changBuf[cbLong++] = ':';
#endif	
					pBitPtr = new BitPrimitive;	
					theApp.m_listBitDataPrimitive.AddTail(pBitPtr);		
					//Î»Í¼µÄ×óÉÏ½ÇÔÚÕû¸öÍ¼ÖÐµÄXÎ»ÖÃ 2004
					//Î»Í¼µÄ×óÉÏ½ÇÔÚÕû¸öÍ¼ÖÐµÄYÎ»ÖÃ	6004			
					//Î»Í¼µÄ×óÏÂ½ÇÔÚÕû¸öÍ¼ÖÐµÄXÎ»ÖÃ	2004
					//Î»Í¼µÄ×óÏÂ½ÇÔÚÕû¸öÍ¼ÖÐµÄYÎ»ÖÃ	2004
					//Î»Í¼µÄÓÒÉÏ½ÇÔÚÕû¸öÍ¼ÖÐµÄXÎ»ÖÃ 6004
					//Î»Í¼µÄÓÒÉÏ½ÇÔÚÕû¸öÍ¼ÖÐµÄYÎ»ÖÃ
					//Î»Í¼µÄÓÒÏÂ½ÇÔÚÕû¸öÍ¼ÖÐµÄXÎ»ÖÃ
					//Î»Í¼µÄÓÒÏÂ½ÇÔÚÕû¸öÍ¼ÖÐµÄYÎ»ÖÃ
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
					
					//Îª1Ê±ÔòÎªÍ¸Ã÷ÏÔÊ¾; Îª0ÔòÎª·ÇÍ¸Ã÷ÏÔÊ¾
					//±³¾°ÑÕÉ«(ÔÚÍ¸Ã÷ÏÔÊ¾Ê±±³¾°ÑÕÉ«µÄµã½«²»±»ÏÔÊ¾)
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
					
					//GDI+ÖÐBitmapDataµÄWidth,µ¥Î»£ºÏñËØ
					//GDI+ÖÐBitmapDataµÄHeight,µ¥Î»£ºÏñËØ
					//GDI+ÖÐBitmapDataµÄStride(Ã¿ÐÐµÄ×Ö½ÚÊý)
					//GDI+ÖÐBitmapDataµÄPixelFormat
					for(dCount=0; dCount<4; dCount++)
					{
						cPin = (unsigned char *)&nParameter;
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						pBitPtr->m_BitGDI[dCount] = nParameter;
					}

					// new 20140416 Ê¹ÓÃÓïÑÔ¸ñÊ½V101
					cPin = (unsigned char *)&nParameter;
					*(cPin+3) = cFileBuf[k++];
					*(cPin+2) = cFileBuf[k++];
					*(cPin+1) = cFileBuf[k++];
					*cPin = cFileBuf[k++];
					dloop = nParameter;
					//******************** 

					//Î»Í¼Êý¾Ý (abs(Height*Stride)¸ö×Ö½Ú) ÓïÑÔ¸ñÊ½V100
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
					hFile.Write(changBuf, cbLong);
					hFile.Write(pBmpData1, dloop);
					hFile.Write(";", 1);
					cbLong=0;
#endif	
				
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					
					CPoint  cPostLU,cPostLD, cPostRU, cPostRD, ptLUNEW;
					cPostLU.x = ftoi(pBitPtr->m_BitPost[0]*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
					cPostLU.y = ftoi(pBitPtr->m_BitPost[1]*m_dRatioY)+m_dpOrigin.y;
					cPostLD.x = ftoi(pBitPtr->m_BitPost[2]*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
					cPostLD.y = ftoi(pBitPtr->m_BitPost[3]*m_dRatioY)+m_dpOrigin.y;
					cPostRU.x = ftoi(pBitPtr->m_BitPost[4]*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
					cPostRU.y = ftoi(pBitPtr->m_BitPost[5]*m_dRatioY)+m_dpOrigin.y;
					cPostRD.x = ftoi(pBitPtr->m_BitPost[6]*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
					cPostRD.y = ftoi(pBitPtr->m_BitPost[7]*m_dRatioY)+m_dpOrigin.y;
					pBitList = new BitList;
					theApp.m_listBitListPrimitive.AddTail(pBitList);
				
					pBitList->m_bTransparent = pBitPtr->m_BitDispStyle[0];
					//pBitList->pBitmap = Create1BPPBitmap(cPostLU,cPostLD,cPostRU,cPostRD,pBitPtr->m_BitDispStyle[0],pBitPtr->m_BitDispStyle[1],
					//pBitPtr->m_BitGDI[0],pBitPtr->m_BitGDI[1],pBitPtr->m_BitGDI[2],pBitPtr->m_BitGDI[3], pBmpData1, pBitList->pBtPoint,iWidth, iHeight);
					//pBitList->m_wi=iWidth;
					//pBitList->m_hi=iHeight;
					char *pUnCompressData;
					int iUnCompressCount;
					UnCompressMemory((char *)pBmpData1, dloop, pUnCompressData, iUnCompressCount);
					pBitList->pBitmap = Create1BPPBitmap(pBitPtr->m_BitDispStyle[0],pBitPtr->m_BitDispStyle[1],pBitPtr->m_BitGDI[0],pBitPtr->m_BitGDI[1],
														 pBitPtr->m_BitGDI[2],pBitPtr->m_BitGDI[3], (BYTE *)pUnCompressData);
					pBitList->bPostLU = cPostLU;	pBitList->bPostLD = cPostLD;
					pBitList->bPostRU = cPostRU;	pBitList->bPostRD = cPostRD;
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
#if _DEBUG
						changBuf[cbLong++] = Instruct.m_cName[0];
						changBuf[cbLong++] = Instruct.m_cName[1];			
#endif	
						cPin = (unsigned char *)&nParameter;
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
#if _DEBUG
						changBuf[cbLong++] = *(cPin+3);
						changBuf[cbLong++] = *(cPin+2);
						changBuf[cbLong++] = *(cPin+1);
						changBuf[cbLong++] = *cPin;
						changBuf[cbLong++] = ',';
#endif
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
#if _DEBUG
						changBuf[cbLong++] = *(cPin+3);
						changBuf[cbLong++] = *(cPin+2);
						changBuf[cbLong++] = *(cPin+1);
						changBuf[cbLong++] = *cPin;
						changBuf[cbLong++] = ',';
#endif
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
#if _DEBUG
						changBuf[cbLong++] = *(cPin+3);
						changBuf[cbLong++] = *(cPin+2);
						changBuf[cbLong++] = *(cPin+1);
						changBuf[cbLong++] = *cPin;
						changBuf[cbLong++] = ',';
#endif
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						nProcess=PROCESS_MNEMONIC;
#if _DEBUG
						changBuf[cbLong++] = *(cPin+3);
						changBuf[cbLong++] = *(cPin+2);
						changBuf[cbLong++] = *(cPin+1);
						changBuf[cbLong++] = *cPin;
						changBuf[cbLong++] = ',';
#endif
					}
					break;
				default:
					break;
				}
			}
		}
#if _DEBUG
		hFile.Write(changBuf, cbLong);	
		cbLong=0;
#endif
		if(bError)
			break;
	}
#if _DEBUG	
	hFile.Close();
#endif

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
	file.Close();
#ifndef _DEBUG
	if(!file.Open(strWorkPath1,CFile::modeCreate | CFile::modeWrite))
	{
		return 0;
	}
	file.Close();
#endif
	if(bError)
		return 2;
	m_lMaxPlotLength=m_lMaxPGLength-m_lMinPlotLength;
	if (m_lMaxPlotWidth < m_lMaxPGWidth)
		m_lMaxPlotWidth = m_lMaxPGWidth;
	m_lMaxPlotWidth -= m_lMinPlotWidth;
	if (theApp.m_nRotate == 1)
	{
		long lMaxPlotLength=m_lMaxPlotLength;
		m_lMaxPlotLength=m_lMaxPlotWidth;
		m_lMaxPlotWidth=lMaxPlotLength;
	}
	return 0;
}



//bDisplayFlag=trueÏÔÊ¾´íÎó;=false²»ÏÔÊ¾´íÎó
//·µ»Ø0Õý³£´ò¿ªÎÄ¼þ;=1ÎÄ¼þ²»´æÔÚ;=2ÎÄ¼þ¸ñÊ½´í
int CPlotCenterDlg::OpenSecretFile(CString strFileName)
{
	CFile file;
	//20100406ÐÂÔöCFile::shareExclusiveÊôÐÔ£¬±ÜÃâÆäËûCADÎÄ¼þÐ´×Ô¶¯»æÍ¼Ä¿Â¼Ê±Ð´Èë²»³É¹¦»ò´òÓ¡È±Ò³
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
	
	InitHPGL();
	InitTextChangeLine();
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
	nProcess=PROCESS_MNEMONIC;
	m=j=0;
	nRemainChar = 0;
	//ÅÐ¶ÏÎÄ¼þÊÇ·ñÎª¼ÓÃÜÎÄ¼þ
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
			if (nProcess == PROCESS_MNEMONIC)//Öú¼Ç·û
			{
				ch = cFileBuf[k++];
				switch (ch)
				{
				case 0x8a:
				case 0x97:
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
	m_lMaxPlotLength=m_lMaxPGLength-m_lMinPlotLength;
	if (m_lMaxPlotWidth < m_lMaxPGWidth)
		m_lMaxPlotWidth = m_lMaxPGWidth;
	m_lMaxPlotWidth -= m_lMinPlotWidth;
	if (theApp.m_nRotate == 1)
	{
		long lMaxPlotLength=m_lMaxPlotLength;
		m_lMaxPlotLength=m_lMaxPlotWidth;
		m_lMaxPlotWidth=lMaxPlotLength;
	}
	return 0;
}

void CPlotCenterDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// ¶Ô»°¿ò¿í¶ÈºÍ¸ß¶ÈµÄÔöÁ¿ 
	int iIncrementX = cx - m_iClientWidth;
	int iIncrementY = cy - m_iClientHeight;

	// ×îÐ¡»¯Ê±ÔöÁ¿Îª0
	if (nType == SIZE_MINIMIZED)
	{
		iIncrementX = iIncrementY = 0;
	}

	for (int i = 0; i < m_iControlNumber; i++)
	{
		CWnd *pWndCtrl = NULL;

		int iId = m_pControlArray[i].iId;
		int iFlag = m_pControlArray[i].iFlag;
		int iPercent = m_pControlArray[i].iPercent;
		
		// ÎÞÐ§Öµ
		if ((iPercent < 0) || (iPercent > 100))
			continue;
		
		// µÃµ½¿Ø¼þÖ¸Õë
		pWndCtrl = GetDlgItem(iId);
		if ((NULL != pWndCtrl) && IsWindow(pWndCtrl->GetSafeHwnd()))
		{
			CRect rectCtrl;
			pWndCtrl->GetWindowRect(rectCtrl);

			ScreenToClient(rectCtrl);

			int iLeft = rectCtrl.left;
			int iTop = rectCtrl.top;
			int iWidth = rectCtrl.Width();
			int iHeight = rectCtrl.Height();
			
			switch (iFlag)
			{
				case MOVEX: // X·½ÏòÒÆ¶¯
					iLeft += (iIncrementX * iPercent / 100);
					break;
				case MOVEY: // Y·½ÏòÒÆ¶¯
					iTop += (iIncrementY * iPercent / 100);
					break;
				case MOVEXY: // X·½ÏòºÍY·½ÏòÍ¬Ê±ÒÆ¶¯
					iLeft += (iIncrementX * iPercent / 100);
					iTop += (iIncrementY * iPercent / 100);
					break;
				case ELASTICX: // X·½Ïò¸Ä±ä´óÐ¡
					iWidth += (iIncrementX * iPercent / 100);
					break;
				case ELASTICY: // Y·½Ïò¸Ä±ä´óÐ¡
					iHeight += (iIncrementY * iPercent / 100);
					break;
				case ELASTICXY: // X·½ÏòºÍY·½ÏòÍ¬Ê±¸Ä±ä´óÐ¡
					iWidth += (iIncrementX * iPercent / 100);
					iHeight += (iIncrementY * iPercent / 100);
					break;
				default:
					break;
			}

			// °Ñ¿Ø¼þÒÆ¶¯µ½ÐÂÎ»ÖÃ
			pWndCtrl->MoveWindow(iLeft, iTop, iWidth, iHeight);
		}
	}

	// °ÑÍ¼±êÒÆ¶¯µ½¶Ô»°¿òÓÒÏÂ½Ç
	if (IsWindow(m_wndSizeIcon.GetSafeHwnd()))
		m_wndSizeIcon.MoveWindow(cx - m_bitmap.bmWidth, cy - m_bitmap.bmHeight, m_bitmap.bmWidth, m_bitmap.bmHeight);

	// ¼ÇÂ¼¶Ô»°¿òclientÇøÓòµÄ´óÐ¡
	if (nType != SIZE_MINIMIZED)
	{
		m_iClientWidth = cx;
		m_iClientHeight = cy;
	}
}

void CPlotCenterDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	CDialog::OnSizing(nSide, lpRect);

	// ¶Ô»°¿ò²»ÄÜÐ¡ÓÚ³õÊ¼´óÐ¡

	int iWidth = lpRect->right - lpRect->left;
	int iHeight = lpRect->bottom - lpRect->top;

	if (iWidth <= m_iMinWidth)
		lpRect->right = lpRect->left + m_iMinWidth;

	if(iHeight <= m_iMinHeight)
		lpRect->bottom = lpRect->top + m_iMinHeight;
}

BOOL CPlotCenterDlg::SetControlProperty(PDLGCTLINFO lp, int nElements)
{
	// ÉèÖÃ¿Ø¼þÊý×éÐÅÏ¢

	if (NULL == lp)
		return FALSE;

	if (nElements <= 0)
		return FALSE;

	m_pControlArray = lp;
	m_iControlNumber = nElements;
	
	return TRUE;
}

void CPlotCenterDlg::ShowSizeIcon(BOOL bShow /*=NULL*/)
{
	m_bShowSizeIcon = bShow;
}

void CPlotCenterDlg::OnSetNet() 
{
	// TODO: Add your command handler code here
	if ((m_nCommunicateMode != 1) && (m_bConnected == false))
	{
		m_nCommunicateMode = 1;
		theApp.m_nCommunicateMode = m_nCommunicateMode;
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_SETUSB,MF_UNCHECKED);
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_SETNET,MF_CHECKED);
	}
	CNetworkSetup NetworkSetupDialog;
	NetworkSetupDialog.m_bConnected = m_bConnected;
	NetworkSetupDialog.DoModal();
}

void CPlotCenterDlg::OnSetupPicture() 
{
	// TODO: Add your command handler code here
	LinkAndReadPara(false);

	CSetupPicture SetupPictureDlg;
	SetupPictureDlg.m_nLineWidth = m_nLineWidth;
	SetupPictureDlg.m_bConnected = m_bConnected;
	SetupPictureDlg.m_bNetworkBusy = m_bNetworkBusy;
	if (SetupPictureDlg.DoModal() == IDOK)
	{
		if (theApp.m_nMeasureUnit == 0)
		{
			theApp.m_dGapDown=SetupPictureDlg.m_dGapDown;
			theApp.m_dGapRight=SetupPictureDlg.m_dGapRight;
			theApp.m_dMarkSpace=SetupPictureDlg.m_dMark;
			theApp.m_dPaperWidth=SetupPictureDlg.m_dPaperWidth;
		}
		else if(theApp.m_nMeasureUnit == 1)
		{
			
			theApp.m_dGapDown=SetupPictureDlg.m_dGapDown*25.4;
			theApp.m_dGapRight=SetupPictureDlg.m_dGapRight*25.4;
			theApp.m_dMarkSpace=SetupPictureDlg.m_dMark*25.4;
			theApp.m_dPaperWidth=SetupPictureDlg.m_dPaperWidth*25.4;
		}
		if (SetupPictureDlg.m_bRotate)
			theApp.m_nRotate = 1;
		else
			theApp.m_nRotate = 0;
		if (SetupPictureDlg.m_bFont)
			theApp.m_nFont = 1;
		else
			theApp.m_nFont = 0;
		if (m_nLineWidth == SetupPictureDlg.m_nLineWidth)
		{
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING293);
			else
				s1.LoadString(IDS_STRING293_EN);
			AfxMessageBox(s1);
			return;
		}
		if ((!SetupPictureDlg.m_bConnected) || SetupPictureDlg.m_bNetworkBusy)
			return;

		if (!m_bConnected)
			LinkAndReadPara(true);
		if ((!m_bConnected) || m_bNetworkBusy)
			return;

		if (m_nVersion == 0)
		{
			char SendBuf[20];
			unsigned long mLength;

			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=3;
			SendBuf[10]=0;
			SendBuf[11]=(char)SetupPictureDlg.m_nLineWidth;
			if (m_nCommunicateMode == 0)
			{
				mLength = 12;
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				send(m_socket,SendBuf,12,0);
				recv(m_socket,SendBuf,20,0);
			}
			if (SendBuf[7] == 9)
			{
				m_nLineWidth = SetupPictureDlg.m_nLineWidth;
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING293);
				else
					s1.LoadString(IDS_STRING293_EN);
				AfxMessageBox(s1);
				return;
			}
		}
		else if (m_nVersion == 1)
		{
			unsigned char *cPin;
			unsigned long mLength;
			short sSprayError;
			double dPlotterError;
			int i;
			char SendBuf[100];

			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=81;
			SendBuf[10]=0;
			SendBuf[11]=m_cPrintStyle;
			SendBuf[12]=0;
			SendBuf[13]=m_cSprinklerCount;
			SendBuf[14]=0;
			SendBuf[15]=m_cSprinklerNo;
			SendBuf[16]=0;
			SendBuf[17]=(char)SetupPictureDlg.m_nLineWidth;
			sSprayError = m_sSprayErrorX12;
			cPin=(unsigned char *)&sSprayError;
			SendBuf[18]=*(cPin+1);
			SendBuf[19]=*cPin;
			sSprayError = m_sSprayErrorX23;
			SendBuf[20]=*(cPin+1);
			SendBuf[21]=*cPin;
			sSprayError = m_sSprayErrorX34;
			SendBuf[22]=*(cPin+1);
			SendBuf[23]=*cPin;
			sSprayError=m_sSprayErrorY12;
			SendBuf[24]=*(cPin+1);
			SendBuf[25]=*cPin;
			sSprayError=m_sSprayErrorY23;
			SendBuf[26]=*(cPin+1);
			SendBuf[27]=*cPin;
			sSprayError=m_sSprayErrorY34;
			SendBuf[28]=*(cPin+1);
			SendBuf[29]=*cPin;
			//20100413 ÉèÖÃ×ßÖ½Îó²î
			//¼æÈÝÐÔÊÇ·ñ·¢ËÍ×ßÖ½Îó²î20100413
			if(g_nNewVersion == 1)//#1 20100413
				dPlotterError=m_dOverlapError;
			else
				dPlotterError = m_dMalposedError;
			cPin = (unsigned char *)&dPlotterError;
			for(i=30;i<38;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}

			if(g_nNewVersion == 1)//#2 20100413
				dPlotterError = m_dMalposedError;
			else
				dPlotterError = m_dPlotterXError;			
			cPin = (unsigned char *)&dPlotterError;
			for(;i<46;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}

			if(g_nNewVersion == 1)//#3 20100413
				dPlotterError = m_dPlotterXError;
			else
				dPlotterError = m_dPlotterYError;
			cPin = (unsigned char *)&dPlotterError;
			for(;i<54;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			
			if(g_nNewVersion == 1)//#4 20100413
			{
				dPlotterError = m_dPlotterYError;
				cPin = (unsigned char *)&dPlotterError;
				for(;i<62;i++)
				{
					SendBuf[i]=*cPin;
					cPin++;
				}	
			}
			
			if (m_nCommunicateMode == 0)
			{
				if(g_nNewVersion == 1)//#5 20100413
					mLength = 62;
				else
					mLength = 54;
					
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				if(g_nNewVersion == 1)//#6 20100413
					send(m_socket,SendBuf,62,0);
				else
					send(m_socket,SendBuf,54,0);					
				
				recv(m_socket,SendBuf,30,0);
			}
			if (SendBuf[7] == 9)
			{
				m_nLineWidth = SetupPictureDlg.m_nLineWidth;
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING293);
				else
					s1.LoadString(IDS_STRING293_EN);
				AfxMessageBox(s1);
				return;
			}
		}
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING250);
		else
			s1.LoadString(IDS_STRING250_EN);
		AfxMessageBox(s1);
	}
}

void CPlotCenterDlg::OnSetupPlotter() 
{
	// TODO: Add your command handler code here
	LinkAndReadPara(false);

	CSetupPlotter SetupPlotterDlg;
	SetupPlotterDlg.m_pParent=this;
	SetupPlotterDlg.m_cPrintStyle = m_cPrintStyle;
	SetupPlotterDlg.m_cSprinklerCount = m_cSprinklerCount;
	SetupPlotterDlg.m_cSprinklerNo = m_cSprinklerNo;
	SetupPlotterDlg.m_sSprayErrorX12 = m_sSprayErrorX12;
	SetupPlotterDlg.m_sSprayErrorX23 = m_sSprayErrorX23;
	SetupPlotterDlg.m_sSprayErrorX34 = m_sSprayErrorX34;
	SetupPlotterDlg.m_sSprayErrorY12 = m_sSprayErrorY12;
	SetupPlotterDlg.m_sSprayErrorY23 = m_sSprayErrorY23;
	SetupPlotterDlg.m_sSprayErrorY34 = m_sSprayErrorY34;
	SetupPlotterDlg.m_dMalposedError = m_dMalposedError;
		SetupPlotterDlg.m_dOverlapError = m_dOverlapError;//20100413 ×ßÖ½Îó²î²ÎÊýÖµ´«µÝ
		SetupPlotterDlg.m_dPlotterXError = m_dPlotterXError;
		SetupPlotterDlg.m_dPlotterYError = m_dPlotterYError;
	/*20150724
	if (m_nLanguage == 0)
	{
		SetupPlotterDlg.m_dMalposedError = m_dMalposedError;
		SetupPlotterDlg.m_dOverlapError = m_dOverlapError;//20100413 ×ßÖ½Îó²î²ÎÊýÖµ´«µÝ
		SetupPlotterDlg.m_dPlotterXError = m_dPlotterXError;
		SetupPlotterDlg.m_dPlotterYError = m_dPlotterYError;
	}
	else
	{
		
		SetupPlotterDlg.m_dMalposedError = m_dMalposedError/2.54;
		SetupPlotterDlg.m_dOverlapError = m_dOverlapError/2.54;//20100413 ×ßÖ½Îó²î²ÎÊýÖµ´«µÝ
		SetupPlotterDlg.m_dPlotterXError = m_dPlotterXError/2.54;
		SetupPlotterDlg.m_dPlotterYError = m_dPlotterYError/2.54;
	}
	*/
	SetupPlotterDlg.m_bConnected = m_bConnected;
	SetupPlotterDlg.m_bNetworkBusy = m_bNetworkBusy;
	if (SetupPlotterDlg.DoModal() == IDOK)
	{
		if ((!SetupPlotterDlg.m_bConnected) || SetupPlotterDlg.m_bNetworkBusy)
			return;

		if (!m_bConnected)
			LinkAndReadPara(true);
		if ((!m_bConnected) || m_bNetworkBusy)
			return;

		unsigned char *cPin;
		unsigned long mLength;
		short sSprayError;
		double dPlotterError;
		int i;
		char SendBuf[100];
		if (m_nVersion == 0)
		{
			//ÉèÖÃ´òÓ¡Ä£Ê½
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=12;
			SendBuf[10]=0;
			SendBuf[11]=SetupPlotterDlg.m_cPrintStyle;
			if (m_nCommunicateMode == 0)
			{
				mLength = 12;
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				send(m_socket,SendBuf,12,0);
				recv(m_socket,SendBuf,20,0);
			}
			if (SendBuf[7] != 9)
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING252);
				else
					s1.LoadString(IDS_STRING252_EN);
				AfxMessageBox(s1);
				return;
			}
			m_cPrintStyle = SetupPlotterDlg.m_cPrintStyle;

			//ÉèÖÃÅçÍ·Ñ¡Ôñ
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=13;
			SendBuf[10]=0;
			SendBuf[11]=SetupPlotterDlg.m_cSprinklerCount;
			SendBuf[12]=0;
			SendBuf[13]=SetupPlotterDlg.m_cSprinklerNo;
			if (m_nCommunicateMode == 0)
			{
				mLength = 14;
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				send(m_socket,SendBuf,14,0);
				recv(m_socket,SendBuf,30,0);
			}
			if (SendBuf[7] != 9)
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING254);
				else
					s1.LoadString(IDS_STRING254_EN);
				AfxMessageBox(s1);
				return;
			}
			m_cSprinklerCount = SetupPlotterDlg.m_cSprinklerCount;
			m_cSprinklerNo = SetupPlotterDlg.m_cSprinklerNo;

			//ÉèÖÃÅçÍ·¼äÎó²î
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=11;
			sSprayError = SetupPlotterDlg.m_sSprayErrorX12;
			cPin=(unsigned char *)&sSprayError;
			SendBuf[10]=*(cPin+1);
			SendBuf[11]=*cPin;
			sSprayError = SetupPlotterDlg.m_sSprayErrorX23;
			SendBuf[12]=*(cPin+1);
			SendBuf[13]=*cPin;
			sSprayError = SetupPlotterDlg.m_sSprayErrorX34;
			SendBuf[14]=*(cPin+1);
			SendBuf[15]=*cPin;
			sSprayError=SetupPlotterDlg.m_sSprayErrorY12;
			SendBuf[16]=*(cPin+1);
			SendBuf[17]=*cPin;
			sSprayError=SetupPlotterDlg.m_sSprayErrorY23;
			SendBuf[18]=*(cPin+1);
			SendBuf[19]=*cPin;
			sSprayError=SetupPlotterDlg.m_sSprayErrorY34;
			SendBuf[20]=*(cPin+1);
			SendBuf[21]=*cPin;
			if (m_nCommunicateMode == 0)
			{
				mLength = 22;
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				send(m_socket,SendBuf,22,0);
				recv(m_socket,SendBuf,30,0);
			}
			if (SendBuf[7] != 9)
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING256);
				else
					s1.LoadString(IDS_STRING256_EN);
				AfxMessageBox(s1);
				return;
			}
			m_sSprayErrorX12 = SetupPlotterDlg.m_sSprayErrorX12;
			m_sSprayErrorX23 = SetupPlotterDlg.m_sSprayErrorX23;
			m_sSprayErrorX34 = SetupPlotterDlg.m_sSprayErrorX34;
			m_sSprayErrorY12 = SetupPlotterDlg.m_sSprayErrorY12;
			m_sSprayErrorY23 = SetupPlotterDlg.m_sSprayErrorY23;
			m_sSprayErrorY34 = SetupPlotterDlg.m_sSprayErrorY34;
			
			//ÉèÖÃË«Ïò´òÓ¡Îó²î
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=10;
			dPlotterError = 0;
			cPin = (unsigned char *)&dPlotterError;
			for(i=10;i<18;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			dPlotterError = SetupPlotterDlg.m_dMalposedError;
			cPin = (unsigned char *)&dPlotterError;
			for(;i<26;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			if (m_nCommunicateMode == 0)
			{
				mLength = 26;
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				send(m_socket,SendBuf,26,0);
				recv(m_socket,SendBuf,30,0);
			}
			if (SendBuf[7] != 9)
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING258);
				else
					s1.LoadString(IDS_STRING258_EN);
				AfxMessageBox(s1);
				return;
			}
			//20150724
			if (theApp.m_nMeasureUnit == 0)
				m_dMalposedError = SetupPlotterDlg.m_dMalposedError;
			else if(theApp.m_nMeasureUnit == 1)
				m_dMalposedError = SetupPlotterDlg.m_dMalposedError*25.4;
			
			//ÉèÖÃÍ¼ÐÎÎó²î
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=6;
			
			dPlotterError = SetupPlotterDlg.m_dPlotterXError;
			cPin = (unsigned char *)&dPlotterError;
			for(i=10;i<18;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			dPlotterError = SetupPlotterDlg.m_dPlotterYError;
			cPin = (unsigned char *)&dPlotterError;
			for(;i<26;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			if (m_nCommunicateMode == 0)
			{
				mLength = 26;
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				send(m_socket,SendBuf,26,0);
				recv(m_socket,SendBuf,30,0);
			}
			if (SendBuf[7] != 9)
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING260);
				else
					s1.LoadString(IDS_STRING260_EN);
				AfxMessageBox(s1);
				return;
			}
			m_dPlotterXError = SetupPlotterDlg.m_dPlotterXError;
			m_dPlotterYError = SetupPlotterDlg.m_dPlotterYError;
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING293);
			else
				s1.LoadString(IDS_STRING293_EN);
			AfxMessageBox(s1);
		}
		else if (m_nVersion == 1)
		{
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=81;
			SendBuf[10]=0;
			SendBuf[11]=SetupPlotterDlg.m_cPrintStyle;
			SendBuf[12]=0;
			SendBuf[13]=SetupPlotterDlg.m_cSprinklerCount;
			SendBuf[14]=0;
			SendBuf[15]=SetupPlotterDlg.m_cSprinklerNo;
			SendBuf[16]=0;
			SendBuf[17]=(char)m_nLineWidth;
			sSprayError = SetupPlotterDlg.m_sSprayErrorX12;
			cPin=(unsigned char *)&sSprayError;
			SendBuf[18]=*(cPin+1);
			SendBuf[19]=*cPin;
			sSprayError = SetupPlotterDlg.m_sSprayErrorX23;
			SendBuf[20]=*(cPin+1);
			SendBuf[21]=*cPin;
			sSprayError = SetupPlotterDlg.m_sSprayErrorX34;
			SendBuf[22]=*(cPin+1);
			SendBuf[23]=*cPin;
			sSprayError=SetupPlotterDlg.m_sSprayErrorY12;
			SendBuf[24]=*(cPin+1);
			SendBuf[25]=*cPin;
			sSprayError=SetupPlotterDlg.m_sSprayErrorY23;
			SendBuf[26]=*(cPin+1);
			SendBuf[27]=*cPin;
			sSprayError=SetupPlotterDlg.m_sSprayErrorY34;
			SendBuf[28]=*(cPin+1);
			SendBuf[29]=*cPin;
			//×ßÖ½Îó²îÊý¾Ý×¼±¸20100413
			//¼æÈÝÐÔÊÇ·ñ·¢ËÍ×ßÖ½Îó²î20100413 1:Ö÷°å³ÌÐò>= 3.24
			if(g_nNewVersion == 1)//#1 20100413
				dPlotterError = SetupPlotterDlg.m_dOverlapError;
			else
				dPlotterError = SetupPlotterDlg.m_dMalposedError;
			cPin = (unsigned char *)&dPlotterError;
			for(i=30;i<38;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}

			if(g_nNewVersion == 1)//#2 20100413
				dPlotterError = SetupPlotterDlg.m_dMalposedError;
			else
				dPlotterError = SetupPlotterDlg.m_dPlotterXError;			
			cPin = (unsigned char *)&dPlotterError;
			for(;i<46;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}

			if(g_nNewVersion == 1)//#3 20100413
				dPlotterError = SetupPlotterDlg.m_dPlotterXError;
			else
				dPlotterError = SetupPlotterDlg.m_dPlotterYError;
			cPin = (unsigned char *)&dPlotterError;
			for(;i<54;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			
			if(g_nNewVersion == 1)//#4 20100413
			{
				dPlotterError = SetupPlotterDlg.m_dPlotterYError;
				cPin = (unsigned char *)&dPlotterError;
				for(;i<62;i++)
				{
					SendBuf[i]=*cPin;
					cPin++;
				}	
			}
			
			if (m_nCommunicateMode == 0)
			{
				if(g_nNewVersion == 1)//#5 20100413
					mLength = 62;
				else
					mLength = 54;
					
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				if(g_nNewVersion == 1)//#6 20100413
					send(m_socket,SendBuf,62,0);
				else
					send(m_socket,SendBuf,54,0);					
				
				recv(m_socket,SendBuf,30,0);
			}
			
			if (SendBuf[7] == 9)
			{
				m_cPrintStyle = SetupPlotterDlg.m_cPrintStyle;
				m_cSprinklerCount = SetupPlotterDlg.m_cSprinklerCount;
				m_cSprinklerNo = SetupPlotterDlg.m_cSprinklerNo;
				m_sSprayErrorX12 = SetupPlotterDlg.m_sSprayErrorX12;
				m_sSprayErrorX23 = SetupPlotterDlg.m_sSprayErrorX23;
				m_sSprayErrorX34 = SetupPlotterDlg.m_sSprayErrorX34;
				m_sSprayErrorY12 = SetupPlotterDlg.m_sSprayErrorY12;
				m_sSprayErrorY23 = SetupPlotterDlg.m_sSprayErrorY23;
				m_sSprayErrorY34 = SetupPlotterDlg.m_sSprayErrorY34;
				
				m_dOverlapError = SetupPlotterDlg.m_dOverlapError;//20100413×ßÖ½Îó²î
				m_dMalposedError = SetupPlotterDlg.m_dMalposedError;
				m_dPlotterXError = SetupPlotterDlg.m_dPlotterXError;
				m_dPlotterYError = SetupPlotterDlg.m_dPlotterYError;
				/*20150724
				if (m_nLanguage == 0)
				{
					m_dOverlapError = SetupPlotterDlg.m_dOverlapError;//20100413×ßÖ½Îó²î
					m_dMalposedError = SetupPlotterDlg.m_dMalposedError;
					m_dPlotterXError = SetupPlotterDlg.m_dPlotterXError;
					m_dPlotterYError = SetupPlotterDlg.m_dPlotterYError;
				}
				else
				{
					m_dOverlapError = SetupPlotterDlg.m_dOverlapError*2.54;//20100413×ßÖ½Îó²î
					m_dMalposedError = SetupPlotterDlg.m_dMalposedError*2.54;
					m_dPlotterXError = SetupPlotterDlg.m_dPlotterXError*2.54;
					m_dPlotterYError = SetupPlotterDlg.m_dPlotterYError*2.54;
				}
				*/
				
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING293);
				else
					s1.LoadString(IDS_STRING293_EN);
				AfxMessageBox(s1);
			}
			else
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING250);
				else
					s1.LoadString(IDS_STRING250_EN);
				::AfxMessageBox(s1);
			}
		}
	}
}

void CPlotCenterDlg::OnAutoDrawDir() 
{
	// TODO: Add your command handler code here
	BOOL bWaitConnected;
	bWaitConnected = m_bWaitConnected;
	m_bWaitConnected = false;

	CFolderDialog FolderDialog((LPCTSTR)theApp.m_strDataDir,BIF_RETURNONLYFSDIRS);  

  if (FolderDialog.DoModal()==IDOK) 
  {
    theApp.m_strDataDir=FolderDialog.GetPathName();
		theApp.m_strDataDir+="\\";
    UpdateData(false);      
  }
	m_bWaitConnected =bWaitConnected;
}

void CPlotCenterDlg::ProcessInstruct(CInstruct *pInstruct)
{
		int nSize;
	int i;
	CString str;
	CGLPrimitive *p;
	CGLText *pText;
	//BitPrimitive *pBitPtr;
	CPoint dp,dp1;
	POSITION pos=NULL;
	double d1;
	double d2;
	
	if(strcmp(pInstruct->m_cName,"PD")==0)//Âä±Ê
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			dp=m_dpCurrent;
			if(m_nPen==0)
			{			
				if(m_bAbsolute)
				{
				if(hbwj!=2)
					{
					dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
					dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y;
					}
					else if(hbwj==2 && (pg==1 || pg==0) && pgg!=2)
					{
					dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;//ºÏ²¢ÎÄ¼þ
					dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y;//ºÏ²¢ÎÄ¼þ
					}
                    else if(hbwj==2 && pgg==2 && pg==2)
					{
					dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x;//ºÏ²¢ÎÄ¼þ
					dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y+m_lMaxPlotWidth;//ºÏ²¢ÎÄ¼þ
					}
					else if(hbwj==2 && pg==1 && pgg==2)
					{
						if(pgnum!=0)
						{
					     dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength1*pgnum;//ºÏ²¢ÎÄ¼þ
						 dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y + m_lMaxPlotWidth1;//ºÏ²¢ÎÄ¼þ
						}
						else
						{
                         dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;//ºÏ²¢ÎÄ¼þ
					     dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y + m_lMaxPlotWidth;//ºÏ²¢ÎÄ¼þ
						}
					}
				}
				else
				{
					for(int i=0;i<nSize;)
					{
						dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX);
						dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY);
					}
				}
				m_dpCurrent=dp;
			}
			else
			{
				p=new CGLPrimitive;
				theApp.m_listPrimitive.AddTail(p);
				p->m_nType=PRIMITIVE_LINE;
				p->m_anData.Add(dp.x);
				p->m_anData.Add(dp.y);
				if (dp.x>m_lMaxPGLength)
					m_lMaxPGLength=dp.x;
				else if (dp.x<m_lMinPlotLength)
					m_lMinPlotLength=dp.x;
				if (dp.y>m_lMaxPGWidth)
					m_lMaxPGWidth=dp.y;
				else if (dp.y<m_lMinPlotWidth)
					m_lMinPlotWidth=dp.y;
							
				if(m_bAbsolute)
				{
					for(int i=0;i<nSize;)
					{
						if(hbwj!=2)
					{
					    dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
						dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY)+m_dpOrigin.y;
					}
					else if(hbwj==2 && (pg==1 || pg==0) && pgg!=2)
					{
					    dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;//ºÏ²¢ÎÄ¼þ
						dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY)+m_dpOrigin.y;//ºÏ²¢ÎÄ¼þ
					}
                    else if(hbwj==2 && pgg==2 && pg==2)
					{	
					     dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX)+m_dpOrigin.x;//ºÏ²¢ÎÄ¼þ
					     dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY)+m_dpOrigin.y+m_lMaxPlotWidth;//ºÏ²¢ÎÄ¼þ	
					}
					else if(hbwj==2 && pg==1 && pgg==2)
					{
						if(pgnum!=0)
						{
					    dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength1*pgnum;//ºÏ²¢ÎÄ¼þ
						dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY)+m_dpOrigin.y+m_lMaxPlotWidth1;//ºÏ²¢ÎÄ¼þ
						}
						else
						{
                        dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;//ºÏ²¢ÎÄ¼þ
						dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY)+m_dpOrigin.y+m_lMaxPlotWidth;//ºÏ²¢ÎÄ¼þ
						}
					}

						p->m_anData.Add(dp.x);
						p->m_anData.Add(dp.y);
						if (dp.x>m_lMaxPGLength)
							m_lMaxPGLength=dp.x;
						else if (dp.x<m_lMinPlotLength)
							m_lMinPlotLength=dp.x;
						if (dp.y>m_lMaxPGWidth)
							m_lMaxPGWidth=dp.y;
						else if (dp.y<m_lMinPlotWidth)
							m_lMinPlotWidth=dp.y;
					}
				}
				else
				{
					for(int i=0;i<nSize;)
					{
						dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX);
						dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY);
						p->m_anData.Add(dp.x);
						p->m_anData.Add(dp.y);
						if (dp.x>m_lMaxPGLength)
							m_lMaxPGLength=dp.x;
						else if (dp.x<m_lMinPlotLength)
							m_lMinPlotLength=dp.x;
						if (dp.y>m_lMaxPGWidth)
							m_lMaxPGWidth=dp.y;
						else if (dp.y<m_lMinPlotWidth)
							m_lMinPlotWidth=dp.y;
					}
				}
				m_dpCurrent=dp;
			}
		}
		m_bPenDown=TRUE;
	}
	else if(strcmp(pInstruct->m_cName,"PU")==0)//Ìá±Ê
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			if (m_bAbsolute)
			{
				if(hbwj!=2)
					{
					    dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
				        dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y;
					}
					else if(hbwj==2 && (pg==1 || pg==0) && pgg!=2)
					{
					    dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;//ºÏ²¢ÎÄ¼þ
				        dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y;//ºÏ²¢ÎÄ¼þ
					}
                    else if(hbwj==2 && pgg==2 && pg==2)
					{
					     dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x;//ºÏ²¢ÎÄ¼þ
					     dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y+m_lMaxPlotWidth;//ºÏ²¢ÎÄ¼þ
					}
					else if(hbwj==2 && pg==1 && pgg==2)
					{
						 if(pgnum!=0)
						 {
					     dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength1*pgnum;//ºÏ²¢ÎÄ¼þ
				         dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y + m_lMaxPlotWidth1;//ºÏ²¢ÎÄ¼þ
						 }
						 else
						 {
                         dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;//ºÏ²¢ÎÄ¼þ
						 dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y + m_lMaxPlotWidth;//ºÏ²¢ÎÄ¼þ
						 }
					}
			}
			else
			{
				dp=m_dpCurrent;
				for(int i=0;i<nSize;)
				{
					dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX);
					dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY);
				}
			}
			m_dpCurrent=dp;
		}
		m_bPenDown=FALSE;
	}
	else if(strcmp(pInstruct->m_cName,"PR")==0)//Ïà¶Ô»æÍ¼
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			dp=m_dpCurrent;
			if((m_bPenDown)&&(m_nPen!=0))
			{
				p=new CGLPrimitive;
				theApp.m_listPrimitive.AddTail(p);
				p->m_nType=PRIMITIVE_LINE;
				p->m_anData.Add(dp.x);
				p->m_anData.Add(dp.y);
				if (dp.x>m_lMaxPGLength)
					m_lMaxPGLength=dp.x;
				else if (dp.x<m_lMinPlotLength)
					m_lMinPlotLength=dp.x;
				if (dp.y>m_lMaxPGWidth)
					m_lMaxPGWidth=dp.y;
				else if (dp.y<m_lMinPlotWidth)
					m_lMinPlotWidth=dp.y;
				for(int i=0;i<nSize;)
				{
					dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX);
					dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY);
					p->m_anData.Add(dp.x);
					p->m_anData.Add(dp.y);
					if (dp.x>m_lMaxPGLength)
						m_lMaxPGLength=dp.x;
					else if (dp.x<m_lMinPlotLength)
						m_lMinPlotLength=dp.x;
					if (dp.y>m_lMaxPGWidth)
						m_lMaxPGWidth=dp.y;
					else if (dp.y<m_lMinPlotWidth)
						m_lMinPlotWidth=dp.y;
				}
				m_dpCurrent=dp;
			}
			else
			{
				for(int i=0;i<nSize;)
				{
					dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX);
					dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY);
				}
				m_dpCurrent=dp;
			}
		}
		m_bAbsolute=FALSE;
	}
	else if(strcmp(pInstruct->m_cName,"PA")==0)//¾ø¶Ô»æÍ¼
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			if((m_bPenDown)&&(m_nPen!=0))
			{
				dp=m_dpCurrent;
				p=new CGLPrimitive;
				theApp.m_listPrimitive.AddTail(p);
				p->m_nType=PRIMITIVE_LINE;
				p->m_anData.Add(dp.x);
				p->m_anData.Add(dp.y);
				if (dp.x>m_lMaxPGLength)
					m_lMaxPGLength=dp.x;
				else if (dp.x<m_lMinPlotLength)
					m_lMinPlotLength=dp.x;
				if (dp.y>m_lMaxPGWidth)
					m_lMaxPGWidth=dp.y;
				else if (dp.y<m_lMinPlotWidth)
					m_lMinPlotWidth=dp.y;
				for(int i=0;i<nSize;)
				{
					dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
					dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY)+m_dpOrigin.y;
					p->m_anData.Add(dp.x);
					p->m_anData.Add(dp.y);
					if (dp.x>m_lMaxPGLength)
						m_lMaxPGLength=dp.x;
					else if (dp.x<m_lMinPlotLength)
						m_lMinPlotLength=dp.x;
					if (dp.y>m_lMaxPGWidth)
						m_lMaxPGWidth=dp.y;
					else if (dp.y<m_lMinPlotWidth)
						m_lMinPlotWidth=dp.y;
				}
				m_dpCurrent=dp;
			}
			else
			{
				m_dpCurrent.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
				m_dpCurrent.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y;
			}
		}
		m_bAbsolute=TRUE;
	}
	else if((strcmp(pInstruct->m_cName,"PG")==0)||(strcmp(pInstruct->m_cName,"FR")==0))//ÍÆ½øÒ³Ãæ
	{
		pg=1;
		pgnum=pgnum+1;
		m_lMaxPlotLength=m_lMaxPGLength;
		if (m_lMaxPlotWidth < m_lMaxPGWidth)
			m_lMaxPlotWidth = m_lMaxPGWidth+100;
		if(pgg!=2)
           m_lMaxPlotWidth1 = m_lMaxPlotWidth;
		if(pgg != 2 && pgnum ==1)
           m_lMaxPlotLength1 = m_lMaxPlotLength;  
	       hbwjplotwidth = m_lMaxPlotWidth1;
	}

	else if((strcmp(pInstruct->m_cName,"PP")==0)||(strcmp(pInstruct->m_cName,"FR")==0) || (strcmp(pInstruct->m_cName,"!G")==0))//ÍÆ½øÒ³Ãæ//ºÏ²¢ÎÄ¼þ
	{
		pg=2;
		pgg=2;
		pgnum=0;
		m_lMaxPlotLength=m_lMaxPGLength;
		if (m_lMaxPlotWidth < m_lMaxPGWidth)
		hbwjplotwidth = m_lMaxPlotWidth;
	}


	else if(strcmp(pInstruct->m_cName,"SI")==0)//×Ö·û´óÐ¡
	{
		nSize=pInstruct->m_adPara.GetSize();
		if (theApp.m_nFont==0)
		{
			pText=new CGLText;
			theApp.m_listTextPrimitive.AddTail(pText);
			pText->m_nType=PRIMITIVE_SI;
			for(i=0;i<nSize;i++)
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(i));
		}
		if(nSize>=2)
		{
			m_dTextHeight=pInstruct->m_adPara.GetAt(1);
			m_dTextWidth=pInstruct->m_adPara.GetAt(0);
		}
		else 
		{
			m_dTextHeight=0.375;
			m_dTextWidth=0.285;
		}
	}

	else if(strcmp(pInstruct->m_cName,"LT")==0)//ÏßÐÍ
	{
		nSize=pInstruct->m_adPara.GetSize();
		p=new CGLPrimitive;
		theApp.m_listPrimitive.AddTail(p);
		p->m_nType=PRIMITIVE_LT;
		for(i=0;i<nSize;i++)
			p->m_anData.Add(ftoi(pInstruct->m_adPara.GetAt(i)));
	}
	else if(strcmp(pInstruct->m_cName,"SP")==0)//Ñ¡±Ê
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>MAaxPenNum)
		{
			m_nPen=ftoi(pInstruct->m_adPara.GetAt(0));
			m_nPen=((m_nPen-1)%MAaxPenNum-1)+1;
		}
		else if(nSize>0)
			m_nPen=ftoi(pInstruct->m_adPara.GetAt(0));
		else
			m_nPen=0;
//		if (m_nPen==0)
//			m_nPen++;
	}
	else if(strcmp(pInstruct->m_cName,"PE")==0)//PE
	{
		
	}
	else if(strcmp(pInstruct->m_cName,"LO")==0)//×Ö·ûÏà¶ÔÔ­µãÎ»ÖÃ
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=1)
		{
			int n=ftoi(pInstruct->m_adPara.GetAt(0));
			if(n<1 || n>19 || n==10)
				n=1;
			m_nTextPosition=n;
		}
		else m_nTextPosition=1;
	}
	else if(strcmp(pInstruct->m_cName,"FN")==0)//×ÖÌå 20121204
	{
		pText=new CGLText;
		theApp.m_listTextPrimitive.AddTail(pText);
		pText->m_nType=PRIMITIVE_FN;
		pText->m_cFontName = pInstruct->m_cFontName;
	}
	else if(strcmp(pInstruct->m_cName,"BT")==0)//Î»Í¼20121204
	{
	//	pBitPtr=new BitPrimitive;	
	//	theApp.m_listBitDataPrimitive.AddTail(pBitPtr);
	//	pBitPtr->m_BitPost[0]=pInstruct->m_BitPost[0];
	//	pBitPtr->m_BitPost[1]=pInstruct->m_BitPost[1];
	//	pBitPtr->m_BitPost[2]=pInstruct->m_BitPost[2];
	//	pBitPtr->m_BitPost[3]=pInstruct->m_BitPost[3];
	//	pBitPtr->m_BitDispStyle[0]= pInstruct->m_BitDispStyle[0];
	//	pBitPtr->m_BitDispStyle[1]= pInstruct->m_BitDispStyle[2];
	//	pBitPtr->m_BitGDI[0]= pInstruct->m_BitGDI[0];
	//	pBitPtr->m_BitGDI[1]= pInstruct->m_BitGDI[1];
	//	pBitPtr->m_BitGDI[2]= pInstruct->m_BitGDI[2];
	//	pBitPtr->m_BitGDI[3]= pInstruct->m_BitGDI[3];
	//	pBitPtr->m_BitDATA= pInstruct->m_BitData;		
	}
	else if(strcmp(pInstruct->m_cName,"SI")==0)//×Ö·û´óÐ¡
	{
		nSize=pInstruct->m_adPara.GetSize();
		if (theApp.m_nFont==0)
		{
			pText=new CGLText;
			theApp.m_listTextPrimitive.AddTail(pText);
			pText->m_nType=PRIMITIVE_SI;
			for(i=0;i<nSize;i++)
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(i));
		}
		if(nSize>=2)
		{
			m_dTextHeight=pInstruct->m_adPara.GetAt(1);
			m_dTextWidth=pInstruct->m_adPara.GetAt(0);
		}
		else 
		{
			m_dTextHeight=0.375;
			m_dTextWidth=0.285;
		}
	}
	else if(strcmp(pInstruct->m_cName,"DI")==0)//×Ö·û·½Ïò
	{
		nSize=pInstruct->m_adPara.GetSize();
		// 20121218 ¼ÓÃÜÎÄ¼þ
		if(bIsEncrypt)
		{
			m_dTextAngle=pInstruct->m_adPara.GetAt(0)*0.1;
			if (theApp.m_nFont==0)
			{
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_DI;
				//½Ç¶È
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(0)*0.1);
				pText->m_cIsItali = (unsigned char)pInstruct->m_adPara.GetAt(1);
				pText->m_iWeight = (int)pInstruct->m_adPara.GetAt(2);
			}
		}
		else{
		if(nSize>=2)
		{
			d1=pInstruct->m_adPara.GetAt(0);
			d2=pInstruct->m_adPara.GetAt(1);

			if(d1>1 || d1<-1.0)
				d1=cos(pInstruct->m_adPara.GetAt(0));
			if(d2>1 || d2<-1.0)
				d2=sin(pInstruct->m_adPara.GetAt(1));
			
			if(d1<0.10 && d1>-0.10)
				d1=0;
			if(d2<0.10 && d2>-0.10)
				d2=0;

			if(d1==0 && d2==1)
				m_dTextAngle=90;
			else if(d1==1 && d2==0)
				m_dTextAngle=0;
			else if(d1==0 && d2==-1)
				m_dTextAngle=270;
			else if(d1==-1 && d2==0)
				m_dTextAngle=-180;
			else
				m_dTextAngle=atan2(d2,d1)*180/3.14159265359;
			double tmp = (m_dTextAngle - (int)m_dTextAngle);
			if(abs((int)(tmp*10)) >= 5)
			{
				m_dTextAngle = m_dTextAngle < 0 ? m_dTextAngle += -1 : m_dTextAngle+=1;
			}
			m_dTextAngle = (int)m_dTextAngle;
		}
		else
			m_dTextAngle=0;
		if (theApp.m_nFont==0)
		{
			pText=new CGLText;
			theApp.m_listTextPrimitive.AddTail(pText);
			pText->m_nType=PRIMITIVE_DI;
			for(i=0;i<nSize;i++)
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(i));//*0.1
		}
		}
	}
	else if(strcmp(pInstruct->m_cName,"DR")==0)//×Ö·û·½Ïò
	{
		nSize=pInstruct->m_adPara.GetSize();
		if((nSize==0)||((nSize==2)&&((pInstruct->m_adPara.GetAt(0)==0)||(pInstruct->m_adPara.GetAt(1)==0))))
		{
			if(nSize==0)
				m_dTextAngle=0;
			else if(nSize==2)
			{
				d1=pInstruct->m_adPara.GetAt(0);
				d2=pInstruct->m_adPara.GetAt(1);
				if((d1==0)&&(d2!=0))
					m_dTextAngle=90;
				else if((d1!=0)&&(d2==0))
					m_dTextAngle=0;
			}
			if (theApp.m_nFont==0)
			{
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_DR;
				for(i=0;i<nSize;i++)
					pText->m_adPara.Add(pInstruct->m_adPara.GetAt(i));
			}
		}
		else if((nSize==2)&&(m_bIPValid))
		{
			d1=pInstruct->m_adPara.GetAt(0);
			d2=pInstruct->m_adPara.GetAt(1);
			if((d1==0)&&(d2!=0))
				m_dTextAngle=90;
			else if((d1!=0)&&(d2==0))
				m_dTextAngle=0;
			else if((d1!=0)&&(d2!=0)&&(fabs(d1)<=100)&&(fabs(d2)<=100))
			{
				d1=d1*(m_dpP2.x-m_dpP1.x);
				d2=d2*(m_dpP2.y-m_dpP1.y);
				m_dTextAngle=atan2(d1,d2);
			}
			if (theApp.m_nFont==0)
			{
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_DR;
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(0));
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(1));
				pText->m_adPara.Add(m_dpP2.x-m_dpP1.x);
				pText->m_adPara.Add(m_dpP2.y-m_dpP1.y);
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"LB")==0)//×Ö·û
	{	
		nSize=m_strText.GetLength();
		int j=0;
		unsigned char str1[257];
		CPoint dp0;
		///////20121218
		if(bIsEncrypt)
		{
			dp.x=ftoi(pInstruct->m_adPara.GetAt(0)*m_dRatioX);
			dp.y=ftoi(pInstruct->m_adPara.GetAt(1)*m_dRatioY);
			m_dpCurrent=dp;
		}
		else
			dp=m_dpCurrent;
		/////////////
		while(nSize>0)
		{
			if(nSize>256)
				nSize=256;
			for(i=0;i<nSize;i++)
			{
				if(m_strText[j]==0x0d)
				{
					str1[i]='\0';
					if(i!=0)
					{
						pText=new CGLText;
						pText->m_nType=PRIMITIVE_LB;
						pText->m_adPara.Add(dp.x);
						pText->m_adPara.Add(dp.y);
						pText->m_strText=str1;
						GLTextMinXMaxX(pText);
						if (theApp.m_nFont==0)
							theApp.m_listTextPrimitive.AddTail(pText);
						else
						{
							dp0.x=ftoi(pText->m_adPara.GetAt(0));
							dp0.y=ftoi(pText->m_adPara.GetAt(1));
							TextChangeLine(dp0,(LPCTSTR)str1,i);
							pText->m_adPara.RemoveAll();
							delete pText;
						}
					}
					j++;
					dp.x=m_dpCurrent.x;
					break;
				}
				else if(m_strText[j]==0x0a)
				{
					str1[i]='\0';
					if(i!=0)
					{
						pText=new CGLText;
						pText->m_nType=PRIMITIVE_LB;
						pText->m_adPara.Add(dp.x);
						pText->m_adPara.Add(dp.y);
						pText->m_strText=str1;
						GLTextMinXMaxX(pText);
						if (theApp.m_nFont==0)
							theApp.m_listTextPrimitive.AddTail(pText);
						else
						{
							dp0.x=ftoi(pText->m_adPara.GetAt(0));
							dp0.y=ftoi(pText->m_adPara.GetAt(1));
							TextChangeLine(dp0,(LPCTSTR)str1,i);
							pText->m_adPara.RemoveAll();
							delete pText;
						}
					}
					j++;
					dp.y+=ftoi(m_dTextHeight*400);
					break;
				}
				else
					str1[i]=m_strText[j];
				j++;
			}
			if((i==nSize)&&(i!=0))
			{
				str1[i]='\0';
				pText=new CGLText;
				pText->m_nType=PRIMITIVE_LB;
				pText->m_adPara.Add(dp.x);
				pText->m_adPara.Add(dp.y);
				pText->m_strText=str1;
				GLTextMinXMaxX(pText);
				if (theApp.m_nFont==0)
					theApp.m_listTextPrimitive.AddTail(pText);
				else
				{
					dp0.x=ftoi(pText->m_adPara.GetAt(0));
					dp0.y=ftoi(pText->m_adPara.GetAt(1));
					TextChangeLine(dp0,(LPCTSTR)str1,i);
					pText->m_adPara.RemoveAll();
					delete pText;
				}
			}
			else
				i++;
			nSize-=i;
		}
	}
	else if(strcmp(pInstruct->m_cName,"DF")==0)//È±Ê¡
	{
		m_nLineType=LINE_TYPE_SOLID;
		m_bAbsolute=TRUE;
		m_cTerminator=theApp.m_cTerminator;//±êÖ¾ÖÕ½á·û
		m_cTerminatorMode=1;
		m_bSCValid=false;
		m_dpOrigin=CPoint(0,0);
		m_dRatioX=1;
		m_dRatioY=1;
		m_dTextHeight=0.375;//cm
		m_dTextWidth=0.285;//cm
		m_dTextAngle=0;
		m_nTextPosition=1;
		if(theApp.m_listPrimitive.GetCount()!=0)
		{
			p=new CGLPrimitive;
			theApp.m_listPrimitive.AddTail(p);
			p->m_nType=PRIMITIVE_LT;
			if (theApp.m_nFont==0)
			{
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_SI;
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_DI;
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"IN")==0)//³õÊ¼»¯
	{
		m_dpCurrent=CPoint(0,0);
		m_bPenDown=FALSE;
		m_nRotate=0;
		m_dpP1=CPoint(m_rtHardLimitation.left,m_rtHardLimitation.top);
		m_dpP2=CPoint(m_rtHardLimitation.right,m_rtHardLimitation.bottom);
		m_dpOrigin=CPoint(0,0);
		m_dRatioX=1;
		m_dRatioY=1;
		m_nLineType=LINE_TYPE_SOLID;
		m_bAbsolute=TRUE;
		m_cTerminator=theApp.m_cTerminator;//±êÖ¾ÖÕ½á·û
		m_cTerminatorMode=1;
		m_dTextHeight=0.375;//cm
		m_dTextWidth=0.285;//cm
		m_dTextAngle=0;
		m_nTextPosition=1;
		m_bIPValid=m_bPSValid;
		m_bSCValid=false;
		if(theApp.m_listPrimitive.GetCount()!=0)
		{
			p=new CGLPrimitive;
			theApp.m_listPrimitive.AddTail(p);
			p->m_nType=PRIMITIVE_LT;
			if (theApp.m_nFont==0)
			{
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_SI;
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_DI;
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"PS")==0)//»æÍ¼³ß´ç
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			if(theApp.m_listPrimitive.GetCount()==0)
			{
				dp.x=ftoi(pInstruct->m_adPara.GetAt(0));
				dp.y=ftoi(pInstruct->m_adPara.GetAt(1));
				if((dp.x>0)&&(dp.y>0))
				{
					m_rtHardLimitation.left=0;
					m_rtHardLimitation.top=0;
					m_rtHardLimitation.right=dp.x;
					m_rtHardLimitation.bottom=dp.y;
					m_dpP1.x=0;
					m_dpP1.y=0;
					m_dpP2=dp;
					m_bPSValid=true;
					m_bIPValid=true;
				}
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"IP")==0)//Ëõ·Åµã
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=4)
		{
			m_dpP1=CPoint(ftoi(pInstruct->m_adPara.GetAt(0)),ftoi(pInstruct->m_adPara.GetAt(1)));
			m_dpP2=CPoint(ftoi(pInstruct->m_adPara.GetAt(2)),ftoi(pInstruct->m_adPara.GetAt(3)));
			m_bIPValid=true;
		}
		else if(nSize>=2)
		{
			int xd,yd;
			xd=m_dpP2.x-m_dpP1.x;
			yd=m_dpP2.y-m_dpP1.y;
			m_dpP1=CPoint(ftoi(pInstruct->m_adPara.GetAt(0)),ftoi(pInstruct->m_adPara.GetAt(1)));
			m_dpP2=m_dpP1+CPoint(xd,yd);
			if(m_bIPValid || m_bPSValid)
				m_bIPValid=true;
		}
		else
		{
			m_dpP1=CPoint(m_rtHardLimitation.left,m_rtHardLimitation.top);
			m_dpP2=CPoint(m_rtHardLimitation.right,m_rtHardLimitation.bottom);
			m_bIPValid=m_bPSValid;
		}
		SetRatio();
	}
	else if(strcmp(pInstruct->m_cName,"IR")==0)//Ïà¶ÔËõ·Åµã
	{
		int xd,yd;
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=4)
		{
			xd=m_rtHardLimitation.Width();
			yd=m_rtHardLimitation.Height();
			m_dpP1=CPoint(ftoi(xd*pInstruct->m_adPara.GetAt(0)),ftoi(yd*pInstruct->m_adPara.GetAt(1)));
			m_dpP2=CPoint(ftoi(xd*pInstruct->m_adPara.GetAt(2)),ftoi(yd*pInstruct->m_adPara.GetAt(3)));
		}
		else if(nSize>=2)
		{
			xd=m_dpP2.x-m_dpP1.x;
			yd=m_dpP2.y-m_dpP1.y;
			dp=CPoint(xd,yd);
			xd=m_rtHardLimitation.Width();
			yd=m_rtHardLimitation.Height();
			m_dpP1=CPoint(ftoi(xd*pInstruct->m_adPara.GetAt(0)),ftoi(yd*pInstruct->m_adPara.GetAt(1)));
			m_dpP2=m_dpP1+dp;
		}
		else
		{
			m_dpP1=CPoint(m_rtHardLimitation.left,m_rtHardLimitation.top);
			m_dpP2=CPoint(m_rtHardLimitation.right,m_rtHardLimitation.bottom);
		}
		m_bIPValid=m_bPSValid;
		SetRatio();
	}
	else if(strcmp(pInstruct->m_cName,"SC")==0)//Ëõ·Å
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
					m_struSC.dXMin=pInstruct->m_adPara.GetAt(0);
					m_struSC.dXMax=pInstruct->m_adPara.GetAt(1);
					m_struSC.dYMin=pInstruct->m_adPara.GetAt(2);
					m_struSC.dYMax=pInstruct->m_adPara.GetAt(3);
					m_struSC.nType=n;
					if(n==1)
					{
						if(nSize>5)
						{
							m_struSC.dLeft=pInstruct->m_adPara.GetAt(5);
							m_struSC.dBottom=pInstruct->m_adPara.GetAt(6);
						}
						else
						{
							m_struSC.dLeft=50;
							m_struSC.dBottom=50;
						}
					}
					m_bSCValid=true;
					SetRatio();
				}
			}
			else
			{
				if(pInstruct->m_adPara.GetAt(1)!=0 && pInstruct->m_adPara.GetAt(3)!=0)
				{
					m_struSC.dXMin=pInstruct->m_adPara.GetAt(0);
					m_struSC.dXMax=pInstruct->m_adPara.GetAt(1);
					m_struSC.dYMin=pInstruct->m_adPara.GetAt(2);
					m_struSC.dYMax=pInstruct->m_adPara.GetAt(3);
					m_struSC.nType=n;
					m_bSCValid=true;
					SetRatio();
				}
			}
		}
	}
}

int CPlotCenterDlg::GetParameterType(char cName[])
{
	int nType=-1;

	if(strcmp(cName,"DF")==0) nType=NONE_TYPE;//È±Ê¡
	else if(strcmp(cName,"IN")==0) nType=DIGIT_TYPE;//³õÊ¼»¯
	else if(strcmp(cName,"IP")==0) nType=DIGIT_TYPE;//Ëõ·Åµã
	else if(strcmp(cName,"IR")==0) nType=DIGIT_TYPE;//Ïà¶ÔËõ·Åµã
	else if(strcmp(cName,"SC")==0) nType=DIGIT_TYPE;//Ëõ·Å
	else if(strcmp(cName,"PG")==0) nType=DIGIT_TYPE;//ÍÆ½øÒ³Ãæ
	else if(strcmp(cName,"FR")==0) nType=DIGIT_TYPE;//ÍÆ½øÒ³Ãæ
	else if(strcmp(cName,"PA")==0) nType=DIGIT_TYPE;//¾ø¶Ô»æÍ¼
	else if(strcmp(cName,"PD")==0) nType=DIGIT_TYPE;//Âä±Ê
	else if(strcmp(cName,"PU")==0) nType=DIGIT_TYPE;//Ìá±Ê
	else if(strcmp(cName,"PE")==0) nType=PE_TYPE;//ÕÛÏß±àÂë
	else if(strcmp(cName,"SP")==0) nType=DIGIT_TYPE;//Ñ¡±Ê
	else if(strcmp(cName,"SS")==0) nType=NONE_TYPE;//Ñ¡Ôñ±ê×¼×ÖÌå
	else if(strcmp(cName,"DI")==0) nType=DIGIT_TYPE;//¾ø¶Ô·½Ïò------------
	else if(strcmp(cName,"DR")==0) nType=DIGIT_TYPE;//Ïà¶Ô·½Ïò
	else if(strcmp(cName,"SI")==0) nType=DIGIT_TYPE;//¾ø¶Ô×Ö·û³ß´ç
	else if(strcmp(cName,"LO")==0) nType=DIGIT_TYPE;//±êÖ¾Ô­µã172
	else if(strcmp(cName,"DT")==0) nType=DT_TYPE;//¶¨Òå±êÖ¾ÖÕ½á·û
	else if(strcmp(cName,"LB")==0)
	{ 
		nType=STRING_TYPE;//±êÖ¾171
		m_strText.Empty();
	}
	else if(strcmp(cName,"LT")==0) nType=DIGIT_TYPE;//ÏßÐÍ119
	if(nType==-1)
	{
		int k=0;
		nType=DIGIT_TYPE;
	}

	return nType;
}

BOOL CPlotCenterDlg::GetPeData(int &nPeValue, unsigned char ch, int nMode, int &nMultiplier)
{
	if(nMode==32)
	{
		if(ch>=63 && ch<=94)
		{
			int n=ch-63;
			nPeValue+=n*nMultiplier;
			nMultiplier*=32;
			return FALSE;
		}
		if(ch>=95 && ch<=126)
		{
			int n=ch-95;
			nPeValue+=n*nMultiplier;
			nMultiplier=1;
			return TRUE;
		}
	}
	else //nMode=64
	{
		if(ch>=63 && ch<=126)
		{
			int n=ch-63;
			nPeValue+=n*nMultiplier;
			nMultiplier*=64;
			return FALSE;
		}
		if(ch>=191 && ch<=254)
		{
			int n=ch-191;
			nPeValue+=n*nMultiplier;
			nMultiplier=1;
			return TRUE;
		}
	}
	return FALSE;
}

double CPlotCenterDlg::ConvertPEValue(int nPeValue, int nFraction)
{
	int n=ftoi(nPeValue);
	int nSign;
	if(n%2==1) nSign=-1;
	else nSign=1;
	n=n/2;
	double d=n;
	for(int i=0;i<nFraction;i++)
		d=d/2;
	return d;
}

BOOL CPlotCenterDlg::IsAlpha(char ch)
{
	if(ch>='a' && ch<='z') return TRUE;
	if(ch>='A' && ch<='Z') return TRUE;
	return FALSE;
}

BOOL CPlotCenterDlg::IsDigit(char ch)
{
	if(ch>='0' && ch<='9') return TRUE;
	if(ch=='.') return TRUE;
	if(ch=='-' ||ch=='+') return TRUE;
	return FALSE;
}

int ftoi(double f)
{
	int k;
	if(f>5e+75 ||f<-5e+75) return 1;                                                     
	if(f>=0)
	{
		f+=0.5;
	  k=(int)f;
	}
	else
	{
		f-=0.5;
		k=(int)f;
		if(k==f) k++;// ftoi(0.5)=1,ftoi(-0.5)=0
	}
	return k;
}

void CPlotCenterDlg::RemoveGLBuffer()
{
	POSITION pos=theApp.m_listPrimitive.GetHeadPosition();
	while(pos)
	{
		CGLPrimitive *p=(CGLPrimitive*)theApp.m_listPrimitive.GetNext(pos);
		p->m_anData.RemoveAll();
		delete p;
	}
	theApp.m_listPrimitive.RemoveAll();

	pos=theApp.m_listTextPrimitive.GetHeadPosition();
	while(pos)
	{
		CGLText *pText=(CGLText *)theApp.m_listTextPrimitive.GetNext(pos);
		pText->m_adPara.RemoveAll();
		delete pText;
	}
	theApp.m_listTextPrimitive.RemoveAll();

	int i,listCount = 0;
	listCount = theApp.m_listBitDataPrimitive.GetCount();
	pos = theApp.m_listBitDataPrimitive.GetHeadPosition();
	for(i=0; i<listCount && pos!=NULL; i++)
	{
		BitPrimitive *pBitPrimitive = (BitPrimitive*)theApp.m_listBitDataPrimitive.GetNext(pos);
		if(pBitPrimitive != NULL)
		{
			delete pBitPrimitive;
			theApp.m_listBitDataPrimitive.RemoveTail();
		}
	}
	theApp.m_listBitDataPrimitive.RemoveAll();
/*
	if (theApp.m_listBitDataPrimitive.GetCount() > 0)
	{
		pos = theApp.m_listBitDataPrimitive.GetHeadPosition();
		while(pos)
		{
			BitPrimitive *pBitPrimitive = (BitPrimitive*)theApp.m_listBitDataPrimitive.GetNext(pos);
			if ((pBitPrimitive != NULL) && (pos != NULL))
			{
				delete pBitPrimitive;
				theApp.m_listBitDataPrimitive.RemoveTail();
			}
			else
			{
				//BitPrimitive *pBitPrimitive = (BitPrimitive*)theApp.m_listBitDataPrimitive.GetHead();
				//delete pBitPrimitive;
				theApp.m_listBitDataPrimitive.RemoveAll();
			}
		}
		//theApp.m_listBitDataPrimitive.RemoveTail();
	}
*/
	listCount = theApp.m_listBitListPrimitive.GetCount();
	pos = theApp.m_listBitListPrimitive.GetHeadPosition();
	for(i=0; i<listCount && pos!=NULL; i++)
	{
		BitList *pBitList = (BitList*)theApp.m_listBitListPrimitive.GetNext(pos);
		if(pBitList != NULL)
		{
			delete pBitList;
			theApp.m_listBitListPrimitive.RemoveTail();
		}
	}
	theApp.m_listBitListPrimitive.RemoveAll();
	/*
	if (theApp.m_listBitListPrimitive.GetCount() > 0)
	{
		pos = theApp.m_listBitListPrimitive.GetHeadPosition();
		while(pos)
		{
			BitList *pBitList= (BitList*)theApp.m_listBitListPrimitive.GetNext(pos);
			if ((pBitList != NULL) && (pos != NULL))
			{
				delete pBitList;
				theApp.m_listBitListPrimitive.RemoveTail();
			}
			else 
				theApp.m_listBitListPrimitive.RemoveAll();
		}
		//theApp.m_listBitListPrimitive.RemoveTail();
	}
	*/
}

BOOL CPlotCenterDlg::ThreePointOnLine(CDPoint dp0, CDPoint dp1, CDPoint dp2)
{
	if(dp0==dp1) return TRUE;
	if(dp0==dp2) return TRUE;
	if(dp1==dp2) return TRUE;
	double dx1=dp1.x-dp0.x;
	double dy1=dp1.y-dp0.y;
	double dx2=dp2.x-dp1.x;
	double dy2=dp2.y-dp1.y;
	if(dy1==0 && dy2==0) return TRUE;
	else if(dy1==0 || dy2==0) return FALSE;
	if(dx1*dy2==dx2*dy1) return TRUE;
	return FALSE;
}

double CPlotCenterDlg::GetDistance(CDPoint dp0, CDPoint dp1)
{
	double dx=dp1.x-dp0.x;
	double dy=dp1.y-dp0.y;
	return sqrt(dx*dx+dy*dy);
}

double CPlotCenterDlg::GetAngle(CDPoint dp0, CDPoint dp1)
{
	double d=atan2(dp1.y-dp0.y,dp1.x-dp0.x);
	return d;
}

void CPlotCenterDlg::SetRatio()
{
	double d1,d2;
	if(m_dpP2.x==m_dpP1.x) m_dpP2.x+=1;
	if(m_dpP2.y==m_dpP1.y) m_dpP2.y+=1;
	if (!m_bIPValid || !m_bSCValid)
		return;
	
	switch(m_struSC.nType)
	{
		case 0://Òì²½
			m_dRatioX=(m_dpP2.x-m_dpP1.x)/(m_struSC.dXMax-m_struSC.dXMin);
			m_dRatioY=(m_dpP2.y-m_dpP1.y)/(m_struSC.dYMax-m_struSC.dYMin);
			m_dpOrigin.x=ftoi(m_dpP1.x-m_struSC.dXMin*m_dRatioX);
			m_dpOrigin.y=ftoi(m_dpP1.y-m_struSC.dYMin*m_dRatioY);
			break;
		case 1://Í¬²½
			d1=(m_dpP2.x-m_dpP1.x)/(m_struSC.dXMax-m_struSC.dXMin);
			d2=(m_dpP2.y-m_dpP1.y)/(m_struSC.dYMax-m_struSC.dYMin);
			if(d1>d2) //x>y
			{
				m_dRatioY=d2;
				m_dRatioX=d2;
				m_dpOrigin.y=m_dpP1.y;
				m_dpOrigin.x=ftoi(((m_dpP2.x-m_dpP1.x)-(m_struSC.dXMax-m_struSC.dXMin)*d2)*m_struSC.dLeft/100.0);
			}
			else //x<y
			{
				m_dRatioX=d1;
				m_dRatioY=d1;
				m_dpOrigin.x=m_dpP1.x;
				m_dpOrigin.y=ftoi(((m_dpP2.y-m_dpP1.y)-(m_struSC.dYMax-m_struSC.dYMin)*d1)*m_struSC.dBottom/100.0);
			}
			break;
		case 2://µãÒò×Ó
			m_dRatioX=m_struSC.dXMax;
			m_dRatioY=m_struSC.dYMax;
			m_dpOrigin.x=ftoi(m_dpP1.x-m_struSC.dXMin*m_dRatioX);
			m_dpOrigin.y=ftoi(m_dpP1.y-m_struSC.dYMin*m_dRatioY);
			break;
	}
}

void CPlotCenterDlg::GLTextMinXMaxX(CGLText *pText)
{
	int nLen;
	double PI=3.14159265359;
	double dTextAngle=m_dTextAngle/180.0*PI;
	double dLength;

	nLen=pText->m_strText.GetLength();
	if(nLen==0)
		return;

	CDPoint dp0,dp;
	dp0.x=pText->m_adPara.GetAt(0);
	dp0.y=pText->m_adPara.GetAt(1);
			
	switch(m_nTextPosition)
	{
		case 1:
			dp0.x=dp0.x-(m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(m_dTextHeight*400)*cos(dTextAngle);
			//dp0.x=dp0.x;
			//dp0.y=dp0.y;
			break;
		case 2:
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			break;
		case 4:
			dp0.x=dp0.x-(m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 5:
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 6:
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 7:
			dp0.x=dp0.x-(m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 8:
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 9:
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 11:
			dTextAngle=atan2(0.25*m_dTextHeight*400,0.25*m_dTextWidth*400)+m_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*m_dTextHeight*400)*(0.25*m_dTextHeight*400)+(0.25*m_dTextWidth*400)*(0.25*m_dTextWidth*400));
			dp0.x=dp0.x+dLength*cos(dTextAngle);
			dp0.y=dp0.y+dLength*sin(dTextAngle);
			dTextAngle=m_dTextAngle/180.0*PI;
			dp0.x=dp0.x-(m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(m_dTextHeight*400)*cos(dTextAngle);
			break;
		case 12:
			dp0.x=dp0.x+(0.25*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y+(0.25*m_dTextWidth*400)*sin(dTextAngle);
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			break;
		case 13:
			dTextAngle=atan2(0.25*m_dTextHeight*400,0.25*m_dTextWidth*400)-m_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*m_dTextHeight*400)*(0.25*m_dTextHeight*400)+(0.25*m_dTextWidth*400)*(0.25*m_dTextWidth*400));
			dp0.x=dp0.x+dLength*cos(dTextAngle);
			dp0.y=dp0.y-dLength*sin(dTextAngle);
			dTextAngle=m_dTextAngle/180.0*PI;
			break;
		case 14:
			dp0.x=dp0.x-(1.25*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(1.25*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 15:
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 16:
			dp0.x=dp0.x+(0.25*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y-(0.25*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 17:
			dTextAngle=atan2(0.25*m_dTextHeight*400,0.25*m_dTextWidth*400)-m_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*m_dTextHeight*400)*(0.25*m_dTextHeight*400)+(0.25*m_dTextWidth*400)*(0.25*m_dTextWidth*400));
			dp0.x=dp0.x-dLength*cos(dTextAngle);
			dp0.y=dp0.y+dLength*sin(dTextAngle);
			dTextAngle=m_dTextAngle/180.0*PI;
			dp0.x=dp0.x-(m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 18:
			dp0.x=dp0.x-(0.25*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.25*m_dTextWidth*400)*sin(dTextAngle);
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 19:
			dTextAngle=atan2(0.25*m_dTextHeight*400,0.25*m_dTextWidth*400)+m_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*m_dTextHeight*400)*(0.25*m_dTextHeight*400)+(0.25*m_dTextWidth*400)*(0.25*m_dTextWidth*400));
			dp0.x=dp0.x-dLength*cos(dTextAngle);
			dp0.y=dp0.y-dLength*sin(dTextAngle);
			dTextAngle=m_dTextAngle/180.0*PI;
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
	}
	pText->m_adPara.RemoveAll();
	pText->m_adPara.Add(dp0.x);
	pText->m_adPara.Add(dp0.y);
	if(dp0.x>m_lMaxPGLength)
		m_lMaxPGLength=ftoi(dp0.x);
	else if (dp0.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp0.x);
	if(dp0.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp0.y);
	else if(dp0.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp0.y);
	dp.x=dp0.x+(nLen*m_dTextWidth*400)*cos(dTextAngle);
	dp.y=dp0.y+(nLen*m_dTextWidth*400)*sin(dTextAngle);
	if(dp.x>m_lMaxPGLength)
		m_lMaxPGLength=ftoi(dp.x);
	else if (dp.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp.x);
	if(dp.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp.y);
	else if(dp.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp.y);
	dp.x=dp.x+(m_dTextHeight*400)*sin(dTextAngle);
	dp.y=dp.y-(m_dTextHeight*400)*cos(dTextAngle);
	if(dp.x>m_lMaxPGLength)
		m_lMaxPGLength=ftoi(dp.x);
	else if (dp.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp.x);
	if(dp.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp.y);
	else if(dp.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp.y);
	dp.x=dp0.x+(m_dTextHeight*400)*sin(dTextAngle);
	dp.y=dp0.y-(m_dTextHeight*400)*cos(dTextAngle);
	if(dp.x>m_lMaxPGLength)
		m_lMaxPGLength=ftoi(dp.x);
	else if (dp.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp.x);
	if(dp.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp.y);
	else if(dp.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp.y);
}

void CPlotCenterDlg::FillListCtrl(int nSelect)
{
	LV_ITEM lvitem;
	int nItem;
	CString strItem1;
	POSITION pos1;
	CPlotItem *pPlotItem;
	TCHAR szFName[_MAX_FNAME];
	int nTotalFile=0;
	long lTotalLength=0;

	m_wndPlotItem.DeleteAllItems();

	if(m_bAutoSort==TRUE) //add by yhz Ôö¼Ó×Ô¶¯ÅÅÐò¹¦ÄÜ
	{
		if (m_bSortByMTime)
		{
			SortByMtime();
		}
		else
		{
			SortByTitle();
		}
	}

  pos1=m_listPlotItem.GetHeadPosition();
  nItem=0;
  while (pos1 != NULL)
  {
    pPlotItem=(CPlotItem*)m_listPlotItem.GetNext(pos1);
		nTotalFile += pPlotItem->m_nCopyNum;
		lTotalLength = lTotalLength + (pPlotItem->m_lPictureLength - pPlotItem->m_lFixedPointLength) * pPlotItem->m_nCopyNum;
		lvitem.mask=LVIF_TEXT | (nItem == 0? LVIF_IMAGE : 0);
    lvitem.iItem=nItem;
    lvitem.iSubItem=0;
    strItem1=pPlotItem->m_strPlotName;

	savefilename=strItem1;//ÍË³öºó±£´æÎÄ¼þ

    _splitpath(strItem1,NULL,NULL,szFName,NULL);
		strItem1.Format(_T("%s"),szFName);
		strItem1=strItem1 + _T(".plt");
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
    m_wndPlotItem.InsertItem(&lvitem);

   		 lvitem.iSubItem=1;
		//20150722
		if (theApp.m_nMeasureUnit == 0)
			strItem1.Format("%ldmm",pPlotItem->m_lPictureLength);
		else if(theApp.m_nMeasureUnit == 1)
			strItem1.Format("%ld inch",(int)(double (pPlotItem->m_lPictureLength)/25.4));
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		m_wndPlotItem.SetItem(&lvitem);

   		 lvitem.iSubItem=2;
		//20150722
		if (theApp.m_nMeasureUnit == 0)
			strItem1.Format("%ldmm",pPlotItem->m_lPictureWidth);
		else if(theApp.m_nMeasureUnit == 1)
			strItem1.Format("%ld inch",(int)(double (pPlotItem->m_lPictureWidth)/25.4));
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		m_wndPlotItem.SetItem(&lvitem);

		lvitem.iSubItem=3;
		//20150722
		if (theApp.m_nMeasureUnit == 0)
			strItem1.Format("%ldmm",pPlotItem->m_lFixedPointLength);
		else if(theApp.m_nMeasureUnit == 1)
			strItem1.Format("%ld inch",(int)(double (pPlotItem->m_lFixedPointLength)/25.4));
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		m_wndPlotItem.SetItem(&lvitem);

		lvitem.iSubItem=4;
		//20150722
		if (theApp.m_nMeasureUnit == 0)
			strItem1.Format("%ldmm",pPlotItem->m_lSendLength);
		else if(theApp.m_nMeasureUnit == 1)
			strItem1.Format("%ld inch",(int)(double (pPlotItem->m_lSendLength)/25.4));
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		m_wndPlotItem.SetItem(&lvitem);

		lvitem.iSubItem=5;
		strItem1.Format("%ld%%",pPlotItem->m_iPlotpercent);
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		m_wndPlotItem.SetItem(&lvitem);
		
		lvitem.iSubItem=6;
		strItem1.Format("%d",pPlotItem->m_nCopyNum);
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		m_wndPlotItem.SetItem(&lvitem);

		lvitem.iSubItem=7;
		if (m_nLanguage == 0)
		{
			switch (pPlotItem->m_nState)
			{
				case 0:
				case 3:
				case 4:
					strItem1.LoadString(IDS_PDSTRANSMIT); //´òÓ¡ÖÐ
					break;
				case 1:
					strItem1.LoadString(IDS_PDSWAIT); //µÈ´ý
					break;
				case 2:
				case 5:
				case 6:
					  strItem1.LoadString(IDS_PDSPAUSE); //ÔÝÍ£
						break;
			}
		}
		else
		{
			switch (pPlotItem->m_nState)
			{
				case 0:
				case 3:
				case 4:
					strItem1.LoadString(IDS_PDSTRANSMIT_EN);
					break;
				case 1:
					strItem1.LoadString(IDS_PDSWAIT_EN);
					break;
				case 2:
				case 5:
				case 6:
				  strItem1.LoadString(IDS_PDSPAUSE_EN);
					break;
			}
		}

		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		if(nSelect==nItem) 
		{
			lvitem.stateMask=LVIS_SELECTED;
 			lvitem.state=LVIS_SELECTED;
		}
    m_wndPlotItem.SetItem(&lvitem);

//yhz20120117------start
	lvitem.iSubItem=8;
	strItem1.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
	(pPlotItem->m_ctimeMtime).GetYear(),
	(pPlotItem->m_ctimeMtime).GetMonth(),
	(pPlotItem->m_ctimeMtime).GetDay(),
	(pPlotItem->m_ctimeMtime).GetHour(),
	(pPlotItem->m_ctimeMtime).GetMinute(),
	(pPlotItem->m_ctimeMtime).GetSecond());

		lvitem.pszText=strItem1.GetBuffer(0);
	lvitem.iImage=0;
	m_wndPlotItem.SetItem(&lvitem);
//yhz20120117------end

    nItem++;
  }
  if(nSelect>=0)
  {
		m_wndPlotItem.SetItemState(nSelect,LVIS_SELECTED,LVIS_SELECTED);
  }
  m_wndPlotItem.SetFocus();
	m_strTotalFile.Format("%d",nTotalFile);
	if (theApp.m_nMeasureUnit == 0)
	{
		m_strTotalLength.Format("%ldmm",lTotalLength);
	}
	else if(theApp.m_nMeasureUnit == 1)
	{
		m_strTotalLength.Format("%ld inch",(int)(lTotalLength/25.4));
	}

	UpdateData(false);
}

void CPlotCenterDlg::InitHPGL()
{
	m_bPSValid=false;
	m_bIPValid=false;
	m_bSCValid=false;
	m_rtHardLimitation=CRect(0,0,40000,40000);
	m_dpOrigin=CPoint(0,0);
	m_dpP1=CPoint(0,0);
	m_dpP2=CPoint(40000,40000);
	m_dRatioX=1;//µ±Ç°µÄËõ·Å±ÈÂÊ
	m_dRatioY=1;//µ±Ç°µÄËõ·Å±ÈÂÊ
	m_nRotate=0;
	m_dpCurrent=CPoint(0,0);
	m_bPenDown=FALSE;
	m_nLineType=LINE_TYPE_SOLID;
	m_bAbsolute=TRUE;
	m_nPen=1;
	m_cTerminator=theApp.m_cTerminator;//±êÖ¾ÖÕ½á·û
	m_cTerminatorMode=1;
	m_dTextHeight=0.375;//cm
	m_dTextWidth=0.285;//cm
	m_dTextAngle=0;
	m_nTextPosition=1;
	m_lMaxPlotLength=0;
	m_lMaxPlotWidth=0;
	m_lMinPlotLength=0;
	m_lMinPlotWidth=0;
	m_lMaxPGLength=0;
	m_lMaxPGWidth=0;
	RemoveGLBuffer();

	//m_dFixedPointValue = 0;
}

void CPlotCenterDlg::OnDown() 
{
	// TODO: Add your control notification handler code here
	CPlotItem *pPlotItem;
	POSITION pos1,pos2;
	int nSelCount;

	nSelCount=m_wndPlotItem.GetSelectedCount();
	if (nSelCount==1) 
	{
		pos1=m_wndPlotItem.GetFirstSelectedItemPosition();
		if(pos1) 
		{
			int nSelItem1=m_wndPlotItem.GetNextSelectedItem(pos1);
			pos2=m_listPlotItem.FindIndex(nSelItem1);
			if(pos2)
			{
				pPlotItem=(CPlotItem*)m_listPlotItem.GetAt(pos2);
				if(pPlotItem->m_nState!=0)
				{
					pos1=pos2;
					m_listPlotItem.GetNext(pos1);
					if(!pos1) return;
					m_listPlotItem.RemoveAt(pos2);
					m_listPlotItem.InsertAfter(pos1,pPlotItem);
					FillListCtrl(nSelItem1+1);
				}
				//this->ViewTransmitPercent();
			}
		}
	}
}

void CPlotCenterDlg::OnUp()
{
	// TODO: Add your control notification handler code here
	CPlotItem *pPlotItem;
	POSITION pos1,pos2;
	int nSelCount;

	nSelCount=m_wndPlotItem.GetSelectedCount();
	if (nSelCount==1) 
	{
		pos1=m_wndPlotItem.GetFirstSelectedItemPosition();
		if(pos1) 
		{
			int nSelItem1=m_wndPlotItem.GetNextSelectedItem(pos1);
			pos2=m_listPlotItem.FindIndex(nSelItem1);
			if(pos2)
			{
				pPlotItem=(CPlotItem*)m_listPlotItem.GetAt(pos2);
				pos1=pos2;
				m_listPlotItem.GetPrev(pos1);
				if(pos1)
				{
					CPlotItem *p=(CPlotItem*)m_listPlotItem.GetAt(pos1);
					if(p->m_nState==0)
						return;
					m_listPlotItem.RemoveAt(pos2);
					m_listPlotItem.InsertBefore(pos1,pPlotItem);
					FillListCtrl(nSelItem1-1);
					//this->ViewTransmitPercent();
				}
			}
		}
	}
}

void CPlotCenterDlg::OnDelete() 
{
	// TODO: Add your command handler code here
	int nSelCount,nSelItem1;
  CString strCaption,strFileName;
  POSITION pos1,pos2,pos3;
  CPlotItem *pPlotItem;
  TCHAR szDir[_MAX_DIR];
  TCHAR szDrive[_MAX_DRIVE];
  TCHAR szFullPath[_MAX_PATH];

  nSelCount = m_wndPlotItem.GetSelectedCount();
  if (nSelCount > 0)
  {
		CSelectDialog SelectDlg;
		if (m_nLanguage == 0)
		{
			SelectDlg.m_strPrompting.LoadString(IDS_STRING279);
			SelectDlg.m_strOK.LoadString(IDS_STRING277);
			SelectDlg.m_strNO.LoadString(IDS_STRING275);
			SelectDlg.m_strCancel.LoadString(IDS_STRING182);
		}
		else
		{
			SelectDlg.m_strPrompting.LoadString(IDS_STRING279_EN);
			SelectDlg.m_strOK.LoadString(IDS_STRING277_EN);
			SelectDlg.m_strNO.LoadString(IDS_STRING275_EN);
			SelectDlg.m_strCancel.LoadString(IDS_STRING182_EN);
		}
    if (SelectDlg.DoModal() == IDYES)
    {
			CObList listDelete;
			CArray<int,int> array;
			pos1 = m_wndPlotItem.GetFirstSelectedItemPosition();
			::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
			_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
			strFileName.Format(_T("%s%sPlot\\"),szDrive,szDir);
			pos3 = NULL;
			while (pos1 != NULL)
			{
				nSelItem1 = m_wndPlotItem.GetNextSelectedItem(pos1);
				pos2 = m_listPlotItem.FindIndex(nSelItem1);
				pPlotItem = (CPlotItem*)m_listPlotItem.GetAt(pos2);
				if ((pPlotItem->m_nState == 3) || (pPlotItem->m_nState == 4))
				{
					pos3 = pos2;
					continue;
				}
				else if ((pPlotItem->m_nState == 0) || (pPlotItem->m_nState == 5) || (pPlotItem->m_nState == 6))
				{
					pPlotItem->m_nState = 3;
					pos3 = pos2;
				}
				else
				{
					array.Add(nSelItem1);
					listDelete.AddTail(pPlotItem);
					_splitpath(pPlotItem->m_strPlotName,szDrive,szDir,NULL,NULL);
					strCaption.Format(_T("%s%s"),szDrive,szDir);
					if ((strCaption.CompareNoCase(strFileName) == 0) ||
						(strCaption.CompareNoCase(theApp.m_strDataDir) == 0))
						DeleteFile(pPlotItem->m_strPlotName.GetBuffer(0));
       				 }
      		}
			pos1 = listDelete.GetHeadPosition();
			while (pos1)
			{
				pPlotItem = (CPlotItem*)listDelete.GetNext(pos1);
				pos2 = m_listPlotItem.Find(pPlotItem);
				if (pos2 != NULL)
				{
					m_listPlotItem.RemoveAt(pos2);
					delete pPlotItem;
				}
			}
			listDelete.RemoveAll();
			for (int i=array.GetSize()-1;i>=0;i--)
			{
				nSelItem1 = array.GetAt(i);
				m_wndPlotItem.DeleteItem(nSelItem1);
			}
			if (pos3 != NULL)
			{
				m_listPlotItem.GetNext(pos3);
				if (pos3 != NULL)
				{
					pPlotItem = (CPlotItem*)m_listPlotItem.GetNext(pos3);
					if (pPlotItem->m_nState == 1)
						pPlotItem->m_nState = 2;
				}
			}
			FillListCtrl();
			if (m_listPlotItem.GetCount() == 0)
			{
				m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,false);
				m_nStartPrint = 0;
			}
		}
	}
}


void CPlotCenterDlg::OnBreak() 
{
	// TODO: Add your command handler code here
	if (m_bConnected)
	{  //ÎÄ¼þËÍÍê,¹Ø±ÕÍ¨ÐÅ¿Ú
		if (m_bNetworkBusy)
		{
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING118);
			else
				s1.LoadString(IDS_STRING118_EN);
			AfxMessageBox(s1);
		}
		else
		{
			if (m_nCommunicateMode == 0)
				CH375CloseDevice(g_iDeviceIndex);
			else
				closesocket(m_socket);
			m_bConnected = false;
			if (m_nLanguage == 0)
				m_strConnectState.LoadString(IDS_STRING119);
			else
				m_strConnectState.LoadString(IDS_STRING119_EN);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_LINK,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_BREAK,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST135,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST160,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAIN,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAOUT,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETUSB,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETNET,MF_BYCOMMAND | MF_ENABLED);
			//m_bConnectedErr=false;
			UpdateData(false);
		}
	}
}

void CPlotCenterDlg::OnLink() 
{
	// TODO: Add your command handler code here
	LinkAndReadPara(true);
}

void CPlotCenterDlg::LinkAndReadPara(BOOL bDisplayFlag)
{
	if (m_nCommunicateMode == 0)
	{
		ConnectUSB(bDisplayFlag);
		return;
	}

	int retval;
	
	if (m_bConnected)
	{
		ReadParaFromPlotter();
		return;
	}
	m_socket=socket(AF_INET,SOCK_STREAM,0);//292
	if(INVALID_SOCKET != m_socket)
	{	
		SOCKADDR_IN addrSock;
		addrSock.sin_family=AF_INET;
		addrSock.sin_port=htons(1024);
		addrSock.sin_addr.S_un.S_addr=htonl(theApp.m_dwRemoteIP);

		struct timeval timeout;
		fd_set set;
		unsigned long ul = 1;
	
		retval = ioctlsocket(m_socket,FIONBIO,&ul); //ÉèÖÃÎª·Ç×èÈûÄ£Ê½
		if(retval != SOCKET_ERROR)
		{
			connect(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR));
			timeout.tv_sec = TIME_OUT_TIME;
			timeout.tv_usec = 0;
			FD_ZERO(&set);
			FD_SET(m_socket, &set);
			if(select(NULL,NULL,&set,NULL,&timeout) <= 0)
				closesocket(m_socket);
			else
			{
				ul = 0;
				retval = ioctlsocket(m_socket,FIONBIO,&ul); //ÉèÖÃÎª×èÈûÄ£Ê½
				if(retval == SOCKET_ERROR)
					closesocket(m_socket);
				else
				{
					m_bConnected = true;
					ReadParaFromPlotter();
					m_bWaitConnected = false;
					if (m_nLanguage == 0)
						m_strConnectState.LoadString(IDS_STRING120);
					else
						m_strConnectState.LoadString(IDS_STRING120_EN);
					GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_BREAK,MF_BYCOMMAND | MF_ENABLED);
					GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_LINK,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
					GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAIN,MF_BYCOMMAND | MF_ENABLED);
					GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAOUT,MF_BYCOMMAND | MF_ENABLED);
					GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST135,MF_BYCOMMAND | MF_ENABLED);
					GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST160,MF_BYCOMMAND | MF_ENABLED);
					if (m_sPlotType>=2230)
					{
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_ENABLED);
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_ENABLED);
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_ENABLED);
					}
					else if (m_sPlotType>=2030)
					{
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_ENABLED);
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_ENABLED);
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
					}
					else if (m_sPlotType>=1830)
					{
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_ENABLED);
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
					}
					else
					{
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
						GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
					}
					GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETNET,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
					GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETUSB,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
					UpdateData(false);
					return;
				}
			}
		}
	}
	if (bDisplayFlag)
	{
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING115);
		else
			s1.LoadString(IDS_STRING115_EN);
		AfxMessageBox(s1);
	}
}

void CPlotCenterDlg::OnSimplifiedChinese() 
{
	// TODO: Add your command handler code here
	if (m_nLanguage != 0)
	{
		m_nLanguage = 0;
		theApp.m_nLanguage = m_nLanguage;
		ChangeLanguage();
	}
}

void CPlotCenterDlg::OnEnglish() 
{
	// TODO: Add your command handler code here
	if (m_nLanguage != 1)
	{
		m_nLanguage = 1;
		theApp.m_nLanguage = m_nLanguage;
		ChangeLanguage();
	}
}

void CPlotCenterDlg::ChangeLanguage()
{
	CRect rect1;
	CString strItem1;
	CWnd* pWnd;
	int i;
	
	m_wndPlotItem.GetWindowRect(&rect1);
	for (i=0;i<8;i++) //yhz20120117------7to8

		m_wndPlotItem.DeleteColumn(0);
	if (m_nLanguage == 0)
	{
		strItem1.LoadString(IDS_PLOTCENTERNAME);
		AfxGetMainWnd()->SetWindowText(strItem1);
		SetMenu(&m_chinesemenu);

		GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_SIMPLIFIEDCHINESE,MF_CHECKED);
		strItem1.LoadString(IDS_COLUMN0);
		m_wndPlotItem.InsertColumn(0,strItem1,LVCFMT_LEFT,rect1.Width()*4/28,0);
		strItem1.LoadString(IDS_COLUMN1);
		m_wndPlotItem.InsertColumn(1,strItem1,LVCFMT_LEFT,rect1.Width()*3/28,0);
		strItem1.LoadString(IDS_COLUMN2);
		m_wndPlotItem.InsertColumn(2,strItem1,LVCFMT_LEFT,rect1.Width()*3/28,0);
		strItem1.LoadString(IDS_COLUMN3);
		m_wndPlotItem.InsertColumn(3,strItem1,LVCFMT_LEFT,rect1.Width()*4/28,0);
		strItem1.LoadString(IDS_COLUMN4);
		m_wndPlotItem.InsertColumn(4,strItem1,LVCFMT_LEFT,rect1.Width()*4/28,0);
		//20150713-rwt
		strItem1.LoadString(IDS_COLUMN8);
		m_wndPlotItem.InsertColumn(5,strItem1,LVCFMT_LEFT,rect1.Width()*3/28,0);

		
		strItem1.LoadString(IDS_COLUMN5);
		m_wndPlotItem.InsertColumn(6,strItem1,LVCFMT_LEFT,rect1.Width()*2/28,0);
		strItem1.LoadString(IDS_COLUMN6);
		m_wndPlotItem.InsertColumn(7,strItem1,LVCFMT_LEFT,rect1.Width()*2/28,1);
//yhz20120117------start
		strItem1.LoadString(IDS_COLUMN7); 
		m_wndPlotItem.InsertColumn(8,strItem1,LVCFMT_LEFT,rect1.Width()*9/28,1);
//yhz20120117------end
		if (m_bConnected)
			m_strConnectState.LoadString(IDS_STRING120);
		else
			m_strConnectState.LoadString(IDS_STRING119);
		pWnd = GetDlgItem(IDC_TFILE);
		strItem1.LoadString(IDS_STRING121);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_TLENGTH);
		strItem1.LoadString(IDS_STRING122);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING246);
		pWnd->SetWindowText(strItem1);
	}
	else
	{
		strItem1.LoadString(IDS_PLOTCENTERNAME_EN);
		AfxGetMainWnd()->SetWindowText(strItem1);
		SetMenu(&m_englistmenu);
		GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_ENGLISH,MF_CHECKED);
		
		strItem1.LoadString(IDS_COLUMN0_EN);
		m_wndPlotItem.InsertColumn(0,strItem1,LVCFMT_LEFT,rect1.Width()*5/28,0);
		strItem1.LoadString(IDS_COLUMN1_EN);
		m_wndPlotItem.InsertColumn(1,strItem1,LVCFMT_LEFT,rect1.Width()*3/28,0);
		strItem1.LoadString(IDS_COLUMN2_EN);
		m_wndPlotItem.InsertColumn(2,strItem1,LVCFMT_LEFT,rect1.Width()*3/28,0);
		strItem1.LoadString(IDS_COLUMN3_EN);
		m_wndPlotItem.InsertColumn(3,strItem1,LVCFMT_LEFT,rect1.Width()*3/28,0);
		strItem1.LoadString(IDS_COLUMN4_EN);
		m_wndPlotItem.InsertColumn(4,strItem1,LVCFMT_LEFT,rect1.Width()*3/28,0);
		//20150713-rwt
		strItem1.LoadString(IDS_COLUMN8_EN);
		m_wndPlotItem.InsertColumn(5,strItem1,LVCFMT_LEFT,rect1.Width()*3/28,0);
		
		strItem1.LoadString(IDS_COLUMN5_EN);
		m_wndPlotItem.InsertColumn(6,strItem1,LVCFMT_LEFT,rect1.Width()*2/28,0);
		strItem1.LoadString(IDS_COLUMN6_EN);
		m_wndPlotItem.InsertColumn(7,strItem1,LVCFMT_LEFT,rect1.Width()*3/28,1);
//yhz20120117------start
		strItem1.LoadString(IDS_COLUMN7_EN); 
		m_wndPlotItem.InsertColumn(8,strItem1,LVCFMT_LEFT,rect1.Width()*6/28,1);
//yhz20120117------end

		if (m_bConnected)
			m_strConnectState.LoadString(IDS_STRING120_EN);
		else
			m_strConnectState.LoadString(IDS_STRING119_EN);
		pWnd = GetDlgItem(IDC_TFILE);
		strItem1.LoadString(IDS_STRING121_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_TLENGTH);
		strItem1.LoadString(IDS_STRING122_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING246_EN);
		pWnd->SetWindowText(strItem1);
	}

	/*20150728
	if (theApp.m_nMeasureUnit == 0)
	{
	}
	else (theApp.m_nMeasureUnit == 1)
	{
	}
	*/
	
	m_wndPlotItem.SetExtendedStyle(m_wndPlotItem.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	if (m_nStartPrint == 1)
		m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,true);
	else
		m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,false);
	if (m_nCommunicateMode == 0)
	{
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_SETUSB,MF_CHECKED);
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_SETNET,MF_UNCHECKED);
	}
	else
	{
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_SETUSB,MF_UNCHECKED);
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_SETNET,MF_CHECKED);
	}
	FillListCtrl();
	if (m_bConnected)
	{
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_BREAK,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_LINK,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAIN,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAOUT,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST135,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST160,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETUSB,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETNET,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		if (m_sPlotType>=2230)
		{
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_ENABLED);
		}
		else if (m_sPlotType>=2030)
		{
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else if (m_sPlotType>=1830)
		{
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else
		{
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
	}
	else
	{
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_LINK,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_BREAK,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST135,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST160,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAIN,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAOUT,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETUSB,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETNET,MF_BYCOMMAND | MF_ENABLED);
	}
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_PAUSE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	m_wndtoolbar.GetToolBarCtrl().EnableButton(ID_PAUSEFILE2,false);
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_RESTART,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	m_wndtoolbar.GetToolBarCtrl().EnableButton(ID_RESTARTFILE2,false);
	GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_DELETE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	m_wndtoolbar.GetToolBarCtrl().EnableButton(ID_DELETEFILE2,false);
	UpdateData(false);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	this->m_nLanguage = theApp.m_nLanguage;
	
	CWnd* pWnd;
	CString strItem1;
	if (m_nLanguage == 0)
	{
		strItem1.LoadString(IDS_STRING184);
		SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING186);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC2);
		strItem1.LoadString(IDS_STRING188);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING180);
		pWnd->SetWindowText(strItem1);
	}
	else
	{
		strItem1.LoadString(IDS_STRING184_EN);
		SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING186_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC2);
		strItem1.LoadString(IDS_STRING188_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING180_EN);
		pWnd->SetWindowText(strItem1);
	}
	
	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlotCenterDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1)
	{
		if (m_bTimerBusy)
		{
			m_bTimerBusy=false;
			if (m_bWaitConnected)
			{
				if (m_nWaitTime > 20)
				{
					LinkAndReadPara(false);
					m_nWaitTime = 0;
				}
				else
					m_nWaitTime++;
			}

			if (!m_bOpenFile)
			{
				SearchNewJob();
			}
			StartAJob();

			//DistributeJob();
			m_bTimerBusy=true;
		}
	}
	if (nIDEvent == 2)
	{
		if (g_bisSendingPlotData == true)
		{
			long lTotalPage = (g_lMaxPlotLength + SECTION_WIDTH1 - 1) / SECTION_WIDTH1;
			int time =(int) (g_lMaxPlotWidth * lTotalPage / 0.17 / m_cSprinklerCount/1000)/*ÅçÍ·Êý*/;
			//int time =10;
			 CPlotItem *pPlotItem;
			POSITION pos1;
			pos1=m_listPlotItem.GetHeadPosition();
			if (pos1 == NULL)
			{
				return;
			}
			pPlotItem=(CPlotItem*)m_listPlotItem.GetNext(pos1);
		
			//m_pBmpGenerationThread=AfxBeginThread(BmpGenerationThreadProc,&m_PlotThreadInfo);
			//m_pSendDataThread=AfxBeginThread(UsbSendDataThreadProc,&m_PlotThreadInfo);
		
			//int i = 0;
			//while (1)
			{
				
				if (g_lHavePlotBitmapNum < time)
				{
					g_lHavePlotBitmapNum++;
					pPlotItem->m_iPlotpercent =(int)( 100 * g_lHavePlotBitmapNum/time);
					displayspeed=(int)( 100 * g_lHavePlotBitmapNum/time);
					m_wndPrintState.SetPos((int)(pPlotItem->m_iPlotpercent));
					FillListCtrl();
					//g_lHavePlotBitmapNum++;
				}
				else if (g_lHavePlotBitmapNum == time)
				{
					g_lHavePlotBitmapNum++;
					m_isFinishPlot = true;
					FillListCtrl();
					//g_lHavePlotBitmapNum++;
				}
				else
				{
					if (g_bisSendBMDataOver == true)
					{
					//	POSITION pos=m_listPlotItem.GetHeadPosition();
					//	m_listPlotItem.GetNext(pos);
					//	m_listPlotItem.RemoveHead();
						g_lHavePlotBitmapNum = 0;
						FillListCtrl();
						g_bisSendBMDataOver = false;
						g_bisSendingPlotData = false;
					//	CWnd *pParent;
					//	RemoveGLBuffer1();//20150720-rwt
					//	pParent->SendMessage(WM_TRANSMITPERCENT,0,0);
					//	FillListCtrl();
					}
				}
			}
			//g_bisSendingPlotData = false;
		}
			
	}
	CDialog::OnTimer(nIDEvent);
}




void CPlotCenterDlg::OnClickPlotitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPlotItem *pPlotItem;
	POSITION pos1;
	BOOL bDelete=FALSE;
	BOOL bPause=FALSE;
	BOOL bRestart=FALSE;
	
	if (m_wndPlotItem.GetItemCount() == m_listPlotItem.GetCount())
	{
		int nSelCount;
		nSelCount = m_wndPlotItem.GetSelectedCount();
		if (nSelCount > 0) 
		{
			pos1 = m_wndPlotItem.GetFirstSelectedItemPosition();
			if(pos1) 
			{
				int nSelItem1;
				
				bDelete = true;
				nSelItem1 = m_wndPlotItem.GetNextSelectedItem(pos1);
				pos1 = m_listPlotItem.FindIndex(nSelItem1);
				while (nSelCount > 0)
				{
					if (bPause && bRestart)
						break;
					pPlotItem = (CPlotItem*)m_listPlotItem.GetNext(pos1);
					if ((!bPause) && ((pPlotItem->m_nState != 2) && (pPlotItem->m_nState != 5) && (pPlotItem->m_nState != 6)))
						bPause = true;
					else if ((!bRestart) && ((pPlotItem->m_nState == 2) || (pPlotItem->m_nState == 5) || (pPlotItem->m_nState == 6)))
						bRestart = true;
					nSelCount--;
				}
			}
		}
		if (!bDelete)
		{
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_PAUSE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			m_wndtoolbar.GetToolBarCtrl().EnableButton(ID_PAUSEFILE2,false);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_RESTART,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			m_wndtoolbar.GetToolBarCtrl().EnableButton(ID_RESTARTFILE2,false);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_DELETE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			m_wndtoolbar.GetToolBarCtrl().EnableButton(ID_DELETEFILE2,false);
		}
		else
		{
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_DELETE,MF_BYCOMMAND | MF_ENABLED);
			m_wndtoolbar.GetToolBarCtrl().EnableButton(ID_DELETEFILE2,true);
			if (bPause)
			{
				GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_PAUSE,MF_BYCOMMAND | MF_ENABLED);
				m_wndtoolbar.GetToolBarCtrl().EnableButton(ID_PAUSEFILE2,true);
			}
			else
			{
				GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_PAUSE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				m_wndtoolbar.GetToolBarCtrl().EnableButton(ID_PAUSEFILE2,false);
			}
			if (bRestart)
			{
				GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_RESTART,MF_BYCOMMAND | MF_ENABLED);
				m_wndtoolbar.GetToolBarCtrl().EnableButton(ID_RESTARTFILE2,true);
			}
			else
			{
				GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_RESTART,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				m_wndtoolbar.GetToolBarCtrl().EnableButton(ID_RESTARTFILE2,false);
			}
		}
	}
	
	*pResult = 0;
}

void CPlotCenterDlg::OnPause() 
{
	// TODO: Add your command handler code here
	int nSelCount,nSelItem1;
  POSITION pos1,pos2;
  CPlotItem *pPlotItem;

	nSelCount = m_wndPlotItem.GetSelectedCount();
	if (nSelCount > 0)
	{
		pos1 = m_wndPlotItem.GetFirstSelectedItemPosition();
		while (pos1 != NULL)
		{
			nSelItem1 = m_wndPlotItem.GetNextSelectedItem(pos1);
			pos2 = m_listPlotItem.FindIndex(nSelItem1);
			pPlotItem = (CPlotItem*)m_listPlotItem.GetAt(pos2);
			if ((pPlotItem->m_nState == 3) || (pPlotItem->m_nState == 4) || (pPlotItem->m_nState == 5) || (pPlotItem->m_nState == 6))
				continue;
			else if (pPlotItem->m_nState == 0)
				pPlotItem->m_nState = 5;
			else
				pPlotItem->m_nState = 2;
		}
		FillListCtrl();
	}
}

void CPlotCenterDlg::OnRestart() 
{
	// TODO: Add your command handler code here
	int nSelCount,nSelItem1;
  POSITION pos1,pos2;
  CPlotItem *pPlotItem;

	nSelCount = m_wndPlotItem.GetSelectedCount();
	if (nSelCount > 0)
	{
		pos1 = m_wndPlotItem.GetFirstSelectedItemPosition();
		while (pos1 != NULL)
		{
			nSelItem1 = m_wndPlotItem.GetNextSelectedItem(pos1);
			pos2 = m_listPlotItem.FindIndex(nSelItem1);
			pPlotItem = (CPlotItem*)m_listPlotItem.GetAt(pos2);
			if (pPlotItem->m_nState == 2)
				pPlotItem->m_nState = 1;
			else if ((pPlotItem->m_nState == 5) || (pPlotItem->m_nState == 6))
				pPlotItem->m_nState = 0;
			else
				continue;
		}
		FillListCtrl();
	}
}
//¶ÁÈ¡Ö÷°å±£´æµÄ»æÍ¼ÒÇ²ÎÊý 20100413
int CPlotCenterDlg::ReadParaFromPlotter()
{
	if (!m_bConnected)
		return 1;

	struct
	{
		unsigned char cChinaEn;
		unsigned char cSprinklerCount;
		unsigned char cSprinklerNo;
		unsigned char cPrintStyle;
		unsigned char cLineWidth;
		unsigned char cSprayErrorX12;
		unsigned char cSprayErrorX23;
		unsigned char cSprayErrorX34;
		unsigned short MyIP[2];
		unsigned short SubnetMask[2];
		unsigned short GatewayIP[2];
		unsigned short sPlotType;
		unsigned short sMaxYMM;
		int iOriginalPos;
		int	iSprayErrorY12;
		int	iSprayErrorY23;
		int	iSprayErrorY34;
		double dOverlapError;//20100413 PrintPara²ÎÊý´«µÝ
		double dMalposedError;
		double dPlotterXError;
		double dPlotterYError;
	}PrintPara;
	unsigned short *sPin,*sDest;
	unsigned short sdata1,sdata2;
	char SendBuf[100];
	int i,j;
	unsigned long mTotal,mLength;
	unsigned char *mPin,*mDest;
	
	strcpy(SendBuf,"Msg_Read");
	mLength = 8;
	if (m_nCommunicateMode == 0)
	{//USBÄ£Ê½
		if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
		{
			mLength = 64;
			if (CH375ReadData(g_iDeviceIndex,SendBuf,&mLength))  // ½ÓÊÕ³É¹¦
			{
				if (mLength == 0)
					return 2;
				if ((unsigned long)SendBuf[0] > (mLength - 1))
				{
					if (CH375ReadData(g_iDeviceIndex,&SendBuf[mLength],&mTotal))  // ½ÓÊÕ³É¹¦
					{
						if ((unsigned long)SendBuf[0] != (mTotal + mLength - 1))
						{						
							//return 2;//Õë¶Ôrelease°æ±¾³ö´í£¬×¢ÊÍµôµÄ
						}
					}
					else
						return 2;
				}
				j = sizeof(PrintPara) + 8;
				if (SendBuf[0] > j)
				{
					if (strncmp(&SendBuf[1],"Msg_ParaNew Version3.3",22) == 0)
					{//Çø±ðÐÂ¾É°æ±¾£¬ÓÃÓÚÏòÏÂ¼æÈÝ20100413 »ñÈ¡µ±Ç°°æ±¾ÐÅÏ¢USB
						m_nVersion = 1;
						g_nNewVersion = 1 ;
						mPin = (unsigned char *)SendBuf + 22 + 1;
					}
					else if (strncmp(&SendBuf[1],"Msg_ParaNew Version",19) == 0)
					{//Çø±ðÐÂ¾É°æ±¾£¬ÓÃÓÚÏòÏÂ¼æÈÝ20100413 »ñÈ¡µ±Ç°°æ±¾ÐÅÏ¢USB
						m_nVersion = 1;
						g_nNewVersion = 0 ;
						mPin = (unsigned char *)SendBuf + 22 + 1;
					}
					else
						return 2;
				}
				else if (SendBuf[0] == j)
				{
					m_nVersion = 0;
					g_nNewVersion = 0 ;//20100413
					mPin = (unsigned char *)SendBuf + 8 + 1;
				}
				else
					return 2;
				mDest = (unsigned char *)&PrintPara;
				j -= 8;
				for (i=0;i<j;i++)
				{
					*mDest = *mPin;
					mDest++;
					mPin++;
				}
			}
			else
				return 2;
		}
		else
			return 2;
	}
	else
	{//ÍøÂçÄ£Ê½
		send(m_socket,SendBuf,mLength,0);
		recv(m_socket,SendBuf,100,0);
		if (strncmp(SendBuf,"Msg_ParaNew Version3.3",22) == 0)
		{//Çø±ðÐÂ¾É°æ±¾£¬ÓÃÓÚÏòÏÂ¼æÈÝ20100413 »ñÈ¡µ±Ç°°æ±¾ÐÅÏ¢ÍøÂç
			m_nVersion = 1;
			g_nNewVersion = 1 ;//20100413
			sPin = (unsigned short *)SendBuf + 11;
		
		}
		else if (strncmp(SendBuf,"Msg_ParaNew Version",19) == 0)
		{//Çø±ðÐÂ¾É°æ±¾£¬ÓÃÓÚÏòÏÂ¼æÈÝ20100413 »ñÈ¡µ±Ç°°æ±¾ÐÅÏ¢ÍøÂç
			m_nVersion = 1;
			g_nNewVersion = 0 ;//20100413
			sPin = (unsigned short *)SendBuf + 11;
		}
		else
		{
			m_nVersion = 0;
			g_nNewVersion = 0 ;//20100413
			sPin = (unsigned short *)SendBuf + 4;
		}
		sDest = (unsigned short *)&PrintPara;
		j = (sizeof(PrintPara) + 1) / 2;
		for (i=0;i<j;i++)
		{
			sdata1 = *sPin;
			sdata2 = sdata1 << 8;
			sdata1 >>= 8;
			sdata1 |= sdata2;
			*sDest = sdata1;
			sDest++;
			sPin++;
		}
	}
	if ((PrintPara.cPrintStyle !=0) && (PrintPara.cPrintStyle != 1))
		return 2;
	if ((PrintPara.cSprinklerCount < 1) || (PrintPara.cSprinklerCount > 4))
		return 2;
	switch (PrintPara.cSprinklerCount)
	{
		case 1:
			if ((PrintPara.cSprinklerNo < 0) || (PrintPara.cSprinklerNo > 3))
				return 2;
			break;
		case 2:
			if ((PrintPara.cSprinklerNo < 0) || (PrintPara.cSprinklerNo > 2))
				return 2;
			break;
		case 3:
			if ((PrintPara.cSprinklerNo != 0) && (PrintPara.cSprinklerNo != 1))
				return 2;
			break;
		case 4:
			if (PrintPara.cSprinklerNo != 0)
				return 2;
			break;
	}
	//20100413 ÅÐ¶ÏÖ÷°å´«ËÍµÄ ×ßÖ½Îó²îÉèÖÃÖµµÄ·¶Î§
	if ((PrintPara.dOverlapError < -20.0) || (PrintPara.dOverlapError > 20.0))
		return 2;
	if ((PrintPara.dMalposedError < -6.0) || (PrintPara.dMalposedError > 6.0))
		return 2;
	if ((PrintPara.dPlotterXError < 900.0) || (PrintPara.dPlotterXError > 1100.0))
		return 2;
	if ((PrintPara.dPlotterYError < 900.0) || (PrintPara.dPlotterYError > 1100.0))
		return 2;
	if ((PrintPara.cLineWidth < 1) || (PrintPara.cLineWidth > 9))
		return 2;
	if ((PrintPara.cSprayErrorX12 < 0) || (PrintPara.cSprayErrorX12 > 150))
		return 2;
	if ((PrintPara.cSprayErrorX23 < 0) || (PrintPara.cSprayErrorX23 > 150))
		return 2;
	if ((PrintPara.cSprayErrorX34 < 0) || (PrintPara.cSprayErrorX34 > 150))
		return 2;
	if ((PrintPara.iSprayErrorY12 < -20) || (PrintPara.iSprayErrorY12 > 20))
		return 2;
	if ((PrintPara.iSprayErrorY23 < -20) || (PrintPara.iSprayErrorY23 > 20))
		return 2;
	if ((PrintPara.iSprayErrorY34 < -20) || (PrintPara.iSprayErrorY34 > 20))
		return 2;

	
	m_cPrintStyle = PrintPara.cPrintStyle;
	m_cSprinklerCount = PrintPara.cSprinklerCount;
	m_cSprinklerNo = PrintPara.cSprinklerNo;
	m_dOverlapError = PrintPara.dOverlapError;//×ßÖ½Îó²î20100413
	m_dMalposedError = PrintPara.dMalposedError;
	m_dPlotterXError = PrintPara.dPlotterXError;
	m_dPlotterYError = PrintPara.dPlotterYError;
	m_nLineWidth = PrintPara.cLineWidth;
	m_sSprayErrorX12 = PrintPara.cSprayErrorX12;
	m_sSprayErrorX23 = PrintPara.cSprayErrorX23;
	m_sSprayErrorX34 = PrintPara.cSprayErrorX34;
	m_sSprayErrorY12 = PrintPara.iSprayErrorY12;
	m_sSprayErrorY23 = PrintPara.iSprayErrorY23;
	m_sSprayErrorY34 = PrintPara.iSprayErrorY34;
	m_sPlotType = PrintPara.sPlotType;
	return 0;
}

void CPlotCenterDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	CPlotItem *pPlotItem;
  POSITION pos;

	if (m_pSendDataThread != NULL)
  {
    if (TerminateThread(m_pSendDataThread->m_hThread,0))
		{
      delete m_pSendDataThread;
    }
  }
	if (m_pBmpGenerationThread != NULL)
  {
    if (TerminateThread(m_pBmpGenerationThread->m_hThread,0))
		{
      delete m_pBmpGenerationThread;
    }
  }
	pos = m_listPlotItem.GetHeadPosition();
	while (pos != NULL)
	{
		pPlotItem=(CPlotItem*)m_listPlotItem.GetNext(pos);
		delete pPlotItem;
	}
	m_listPlotItem.RemoveAll();
	RemoveGLBuffer();
	RemoveGLBuffer1();
  if (m_bConnected)
  {
		if (m_nCommunicateMode == 0)
			CH375CloseDevice(g_iDeviceIndex);
		else
			closesocket(m_socket);
		m_bConnected=false;
		m_bNetworkBusy=false;
		//m_bConnectedErr=false;
		GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETUSB,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETNET,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAIN,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAOUT,MF_BYCOMMAND | MF_ENABLED);
  }
  ::RemoveProp(GetSafeHwnd(),AfxGetApp()->m_pszExeName);
//  ::CloseHandle(m_hMapObject);
}

void CPlotCenterDlg::OnExit() 
{
	// TODO: Add your command handler code here
	OnCancel();
}

void CPlotCenterDlg::OnCancel()
{
	// TODO: Add extra cleanup here
	CString strText,strCaption,strFileName;
  CPlotItem *pPlotItem;
  TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	FILE *pFile;
	SYSTEMTIME st;

  if (m_listPlotItem.GetCount() > 0)
  {
		pPlotItem=(CPlotItem*)m_listPlotItem.GetHead();
		if (pPlotItem->m_nState==0)
		{
			::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
			_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
			strFileName.Format(_T("%s%shistory.ini"),szDrive,szDir);
			pFile=fopen((LPCTSTR)strFileName,"a");
			GetLocalTime(&st);
			fprintf(pFile,"%d:%d:%d|",st.wHour,st.wMinute,st.wSecond);
			fprintf(pFile,"%.2f%%|\n",(double)(pPlotItem->m_lSendLength) / (double)(pPlotItem->m_lPictureLength - pPlotItem->m_lFixedPointLength) * 100.0);
			fclose(pFile);
		}
		CSelectDialog SelectDlg;
		if (m_nLanguage == 0)
		{
			SelectDlg.m_strPrompting.LoadString(IDS_STRING263);
			SelectDlg.m_strOK.LoadString(IDS_STRING277);
			SelectDlg.m_strNO.LoadString(IDS_STRING275);
			SelectDlg.m_strCancel.LoadString(IDS_STRING182);
		}
		else
		{
			SelectDlg.m_strPrompting.LoadString(IDS_STRING263_EN);
			SelectDlg.m_strOK.LoadString(IDS_STRING277_EN);
			SelectDlg.m_strNO.LoadString(IDS_STRING275_EN);
			SelectDlg.m_strCancel.LoadString(IDS_STRING182_EN);
		}
    switch (SelectDlg.DoModal())
		{
		case IDYES:
			if ((pPlotItem->m_nState == 0) && (pPlotItem->m_lSendLength > 0))
			{
				theApp.m_strPrintFile = pPlotItem->m_strPlotName;
				theApp.m_lFixedPointLength = pPlotItem->m_lFixedPointLength + pPlotItem->m_lSendLength;
				theApp.m_lPaperWidth = pPlotItem->m_lPaperWidth;
			}
			else
			{
				theApp.m_strPrintFile = " ";
				theApp.m_lFixedPointLength = 0;
				theApp.m_lPaperWidth = 2600;
			}
			CDialog::OnCancel();
			break;
		case IDNO:
			::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
			_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
			strFileName.Format(_T("%s%sPlot\\"),szDrive,szDir);
			_splitpath(pPlotItem->m_strPlotName,szDrive,szDir,NULL,NULL);
			strCaption.Format(_T("%s%s"),szDrive,szDir);
			if ((strCaption.CompareNoCase(strFileName) == 0) ||
				(strCaption.CompareNoCase(theApp.m_strDataDir) == 0))
			{
				theApp.m_strPrintFile = " ";
				theApp.m_lFixedPointLength = 0;
				theApp.m_lPaperWidth = 2600;
				DeleteFile(pPlotItem->m_strPlotName.GetBuffer(0));
			}
			else if ((pPlotItem->m_nState == 0) && (pPlotItem->m_lSendLength > 0))
			{
				theApp.m_strPrintFile = pPlotItem->m_strPlotName;
				theApp.m_lFixedPointLength = pPlotItem->m_lFixedPointLength + pPlotItem->m_lSendLength;
				theApp.m_lPaperWidth = pPlotItem->m_lPaperWidth;
			}
			else
			{
				theApp.m_strPrintFile = " ";
				theApp.m_lFixedPointLength = 0;
				theApp.m_lPaperWidth = 2600;
			}
			m_listPlotItem.RemoveHead();
			delete pPlotItem;
			while (m_listPlotItem.GetCount() > 0)
			{
				pPlotItem = (CPlotItem*)m_listPlotItem.GetHead();
				_splitpath(pPlotItem->m_strPlotName,szDrive,szDir,NULL,NULL);
				strCaption.Format(_T("%s%s"),szDrive,szDir);
				if (strCaption.CompareNoCase(strFileName) == 0)
					DeleteFile(pPlotItem->m_strPlotName.GetBuffer(0));
				else if (strCaption.CompareNoCase(theApp.m_strDataDir) == 0)
					DeleteFile(pPlotItem->m_strPlotName.GetBuffer(0));
				m_listPlotItem.RemoveHead();
				delete pPlotItem;
			}
			CDialog::OnCancel();
			break;
		default:
			break;
		}
  }
	else
	{
		theApp.m_strPrintFile = " ";
		theApp.m_lFixedPointLength = 0;
		theApp.m_lPaperWidth = 2600;
		CDialog::OnCancel();
	}
}

extern bool   g_bStateBrak;

void CPlotCenterDlg::StartAJob()
{
	CPlotItem *pPlotItem;
	int nReturnValue;

	if ((m_bNetworkBusy) || (m_nStartPrint == 1))
		return;//ÈÎÎñÃ»Íê³É£¬²»ÄÜ´òÓ¡ÏÂÒ»ÎÄ¼þ
	if (m_listPlotItem.GetCount() <= 0)
		return;//Ã»ÓÐÎÄ¼þµÈ´ý´òÓ¡
	//20100413ÅÐ¶ÏÊÇ·ñÉèÖÃÑ¡ÔñÎÄ¼þºó×Ô¶¯´òÓ¡  StartAJob
	if(theApp.m_nAutoPrintStyle==0){
 	if ((m_nStartPrint == 0))//&&(theApp.m_nAutoPrintStyle==0))
	{//ÒÑ¾­µã»÷¡°¿ªÊ¼¡±´òÓ¡
		m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,true);//
		m_nStartPrint = 1;
		return;
	}
	}
	else
	{//20100517½â¾öÒòÐÞ¸ÄÉèÖÃ×Ô¶¯´òÓ¡ºó²»ÄÜÁ¬Ðø´òÓ¡µÄÎÊÌâ£»
		if((m_nStartPrint == 0)){
		m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,false);//
		m_nStartPrint = 2;
		}
	}
	//************
	pPlotItem = (CPlotItem*)m_listPlotItem.GetHead();
	if (pPlotItem->m_nState != 1)
		return;
	
	if (!m_bConnected)
	{
		if (!m_bWaitConnected)
		{//½¨Á¢Á¬½Ó
			LinkAndReadPara(false);
			if (!m_bConnected)
			{
				m_nWaitTime = 0;
				m_bWaitConnected = true;
				return;
			}
		}
		else
			return;
	}

	g_dScaleX = 1000.0 / m_dPlotterYError;
	g_dScaleY = 1000.0 / m_dPlotterXError;
	g_nLineWidth = m_nLineWidth;
	g_nVersion = m_nVersion;
	nReturnValue = CreateList(pPlotItem,m_nCommunicateMode,m_socket);
	
	if (nReturnValue == 0)
	{
		TCHAR szFullPath[_MAX_PATH];
		TCHAR szDir[_MAX_DIR];
		TCHAR szDrive[_MAX_DRIVE];
		//TCHAR szFileName[_MAX_FNAME];//20150316rwt Truncate filename
		//TCHAR szExt[_MAX_EXT];
		CString sWorkPath;
		CString sWorkPath1;//20150316rwt makedir history
		CString sWorkPath2;//20150318rwt makedir history
		
		FILE *pFile;
		SYSTEMTIME st;

		//Ð´ÀúÊ·¼ÇÂ¼
		::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
		_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
		sWorkPath.Format(_T("%s%shistory.ini"),szDrive,szDir);
		pFile=fopen((LPCTSTR)sWorkPath,"ab");
		GetLocalTime(&st);
		fprintf(pFile,"%s|",pPlotItem->m_strPlotName);
		fprintf(pFile,"%d-%d-%d ",st.wYear,st.wMonth,st.wDay);
		fprintf(pFile,"%d:%d:%d|",st.wHour,st.wMinute,st.wSecond);
		fclose(pFile);

		/* ×Ô¶¯±£´æµ½¹¤×÷Ä¿Â¼ÏÂµÄhisteryÎÄ¼þ¼ÐÖÐ
		//20150316rwt****start****/
		//SavedToHistoryFolder();
		//20150316rwt****start****/
		/* ×Ô¶¯±£´æµ½¹¤×÷Ä¿Â¼ÏÂµÄhisteryÎÄ¼þ¼ÐÖÐ*///20150316rwt**** end ****/

		pPlotItem->m_nState=0;
		m_PlotThreadInfo.m_pParent=this;
		m_PlotThreadInfo.sock=m_socket;
		m_PlotThreadInfo.m_pPlotItem=pPlotItem;
		m_bNetworkBusy = true;
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAIN,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAOUT,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		FillListCtrl();
		if (m_nCommunicateMode == 0)
		{//USB´òÓ¡
			//PlotProgressThreadProc();
			m_pBmpGenerationThread=AfxBeginThread(BmpGenerationThreadProc,&m_PlotThreadInfo);
			m_pSendDataThread=AfxBeginThread(UsbSendDataThreadProc,&m_PlotThreadInfo);
			//PlotProgressThreadProc();
			/*
			if (g_bStateBrak == true)
		 	{
				CH375CloseDevice(g_iDeviceIndex);	
				m_bConnected=false;
				LinkAndReadPara(false);
				//CString s1;
				//s1.LoadString(IDS_STRING303);
				//AfxMessageBox(s1);
			 }
			*/
		}
		else
		{//ÍøÂç´òÓ¡
			m_pBmpGenerationThread=AfxBeginThread(BmpGenerationThreadProc,&m_PlotThreadInfo);
			m_pSendDataThread=AfxBeginThread(SendDataThreadProc,&m_PlotThreadInfo);
		}
		//20150318rwt 
		SavedToHistoryFolder();
		DeleteHistoryFilesOnTime();
	}
	else
	{
		CString s1;
		if (nReturnValue == 1)
		{//´ò¿ªÒ»¸ö»æÍ¼ÎÄ¼þÊ§°Ü
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING106);
			else
				s1.LoadString(IDS_STRING106_EN);
			AfxMessageBox(s1);
		}
		else
		{
			CString strWorkPath1,strWorkPath2;
			TCHAR szFullPath[_MAX_PATH];
			TCHAR szDir[_MAX_DIR];
			TCHAR szDrive[_MAX_DRIVE];
			if (nReturnValue == 2)
			{//·ÇHP-GLÎÄ¼þ!
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING108);
				else
					s1.LoadString(IDS_STRING108_EN);
				AfxMessageBox(s1);
			}
			else if (nReturnValue == 3)
			{
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING267);
				else
					s1.LoadString(IDS_STRING267_EN);
				AfxMessageBox(s1);
			}
			::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
			_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
			strWorkPath2.Format(_T("%s%sPlot\\"),szDrive,szDir);
			_splitpath(pPlotItem->m_strPlotName,szDrive,szDir,NULL,NULL);
			strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
			if (strWorkPath1.CompareNoCase(strWorkPath2) == 0)
				DeleteFile(pPlotItem->m_strPlotName.GetBuffer(0));
			else if (strWorkPath1.CompareNoCase(theApp.m_strDataDir) == 0)
				DeleteFile(pPlotItem->m_strPlotName.GetBuffer(0));
		}
		m_wndPlotItem.DeleteItem(0);
		m_listPlotItem.RemoveHead();
		delete pPlotItem;
		if (m_listPlotItem.GetCount() == 0)
		{//ÎÄ¼þËÍÍê,¹Ø±ÕÍ¨ÐÅ¿Ú
			if (m_nCommunicateMode == 0)
				CH375CloseDevice(g_iDeviceIndex);
			else
				closesocket(m_socket);
			m_bConnected=false;
			if (m_nLanguage == 0)
				m_strConnectState.LoadString(IDS_STRING119);
			else
				m_strConnectState.LoadString(IDS_STRING119_EN);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_LINK,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_BREAK,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETUSB,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETNET,MF_BYCOMMAND | MF_ENABLED);
		}
		FillListCtrl();
		//m_bConnectedErr=false;
	}
}


void CPlotCenterDlg::SendErrorBlock(double dScaleX, double dScaleY)
{
	unsigned int i,j,icount;
	BYTE *pBits,*pBits1;
	char SendBuf[100];
	unsigned int icountX,icountY;
	unsigned int iCountLengthX,iCountLengthY;
	unsigned int iTemp[16] = {0xc000,0x0001,0x0003,0x0006,0x000c,0x0018,0x0030,0x0060,0x00c0,
					 0x0180,0x0300,0x0600,0x0c00,0x1800,0x3000,0x6000};
	unsigned int iCountByte=0;
	unsigned long mLength;

	iCountLengthX = (unsigned int)(1000.0 * dScaleX / 25.4 * 300.0);
	icountX = (iCountLengthX + 15) / 16;
	iCountLengthY = (unsigned int)(1000.0 * dScaleY * 111.11113 / 14.0);
	icountY = (iCountLengthY + 7) / 8;
	icountY *= 8;
	icount = icountY * icountX * 2;
	pBits = new BYTE[2000000];
	pBits1 = pBits;
	
	iCountLengthY -= 2;
	iCountLengthX = iCountLengthX % 16;

	*pBits1 = 0x04;
	pBits1++;
	*pBits1 = 0xff;
	pBits1++;
	iCountByte += 2;
	for (i=2;i<iCountLengthY;i++)
	{
		*pBits1 = 0x01;
		pBits1++;
		*pBits1 = 0x03;
		pBits1++;
		*pBits1 = 0x01;
		pBits1++;
		*pBits1 = 0x0;
		pBits1++;
	}
	iCountByte += ((iCountLengthY - 2) * 4);
	*pBits1 = 0x04;
	pBits1++;
	*pBits1 = 0xff;
	pBits1++;
	iCountByte += 2;
	i = (icountY - iCountLengthY - 2) * 2;
	while (i)
	{
		if (i > 0xff)
		{
			*pBits1 = 0xff;
			i -= 0xff;
		}
		else
		{
			*pBits1 = (BYTE)i;
			i = 0;
		}
		pBits1++;
		*pBits1 = 0x0;
		pBits1++;
		iCountByte += 2;
	}
	
	icountX -= 3;
	for (j=0;j<icountX;j++)
	{
		*pBits1 = 0x04;
		pBits1++;
		*pBits1 = 0xff;
		pBits1++;
		iCountByte += 2;
		i = (iCountLengthY - 2) * 2;
		while (i)
		{
			if (i > 0xff)
			{
				*pBits1 = 0xff;
				i -= 0xff;
			}
			else
			{
				*pBits1 = (BYTE)i;
				i = 0;
			}
			pBits1++;
			*pBits1 = 0x0;
			pBits1++;
			iCountByte += 2;
		}
		*pBits1 = 0x04;
		pBits1++;
		*pBits1 = 0xff;
		pBits1++;
		iCountByte += 2;
		i = (icountY - iCountLengthY - 2) * 2;
		while (i)
		{
			if (i > 0xff)
			{
				*pBits1 = 0xff;
				i -= 0xff;
			}
			else
			{
				*pBits1 = (BYTE)i;
				i = 0;
			}
			pBits1++;
			*pBits1 = 0x0;
			pBits1++;
			iCountByte += 2;
		}
	}
	
	*pBits1 = 0x04;
	pBits1++;
	*pBits1 = 0xff;
	pBits1++;
	iCountByte += 2;
	if (iCountLengthX == 0)
	{
		for (i=2;i<iCountLengthY;i++)
		{
			*pBits1 = 0x01;
			pBits1++;
			*pBits1 = 0x0;
			pBits1++;
			*pBits1 = 0x01;
			pBits1++;
			*pBits1 = 0xc0;
			pBits1++;
		}
		iCountByte += ((iCountLengthY - 2) * 4);
	}
	else if (iCountLengthX == 1)
	{
		for (i=2;i<iCountLengthY;i++)
		{
			*pBits1 = 0x01;
			pBits1++;
			*pBits1 = 0x0;
			pBits1++;
			*pBits1 = 0x01;
			pBits1++;
			*pBits1 = 0x80;
			pBits1++;
		}
		iCountByte += ((iCountLengthY - 2) * 4);
	}
	else
	{
		i = (iCountLengthY - 2) * 2;
		while (i)
		{
			if (i > 0xff)
			{
				*pBits1 = 0xff;
				i -= 0xff;
			}
			else
			{
				*pBits1 = (BYTE)i;
				i = 0;
			}
			pBits1++;
			*pBits1 = 0x0;
			pBits1++;
			iCountByte += 2;
		}
	}
	*pBits1 = 0x04;
	pBits1++;
	*pBits1 = 0xff;
	pBits1++;
	iCountByte += 2;
	i = (icountY - iCountLengthY - 2) * 2;
	while (i)
	{
		if (i > 0xff)
		{
			*pBits1 = 0xff;
			i -= 0xff;
		}
		else
		{
			*pBits1 = (BYTE)i;
			i = 0;
		}
		pBits1++;
		*pBits1 = 0x0;
		pBits1++;
		iCountByte += 2;
	}
	*pBits1 = 0x04;
	pBits1++;
	*pBits1 = 0xff;
	pBits1++;
	iCountByte += 2;
	for (i=2;i<iCountLengthY;i++)
	{
		*pBits1 = 0x01;
		pBits1++;
		*pBits1 = (BYTE)iTemp[iCountLengthX];
		pBits1++;
		*pBits1 = 0x01;
		pBits1++;
		*pBits1 = (BYTE)(iTemp[iCountLengthX]>>8);
		pBits1++;
	}
	iCountByte += ((iCountLengthY - 2) * 4);
	*pBits1 = 0x04;
	pBits1++;
	*pBits1 = 0xff;
	pBits1++;
	iCountByte += 2;
	i = (icountY - iCountLengthY - 2) * 2;
	while (i)
	{
		if (i > 0xff)
		{
			*pBits1 = 0xff;
			i -= 0xff;
		}
		else
		{
			*pBits1 = (BYTE)i;
			i = 0;
		}
		pBits1++;
		*pBits1 = 0x0;
		pBits1++;
		iCountByte += 2;
	}
	
	icountY /= 8;
	//·¢ËÍÍ¼ÐÎ¿í¶È£¨×Ö½ÚÊý£©
	while (1)
	{
		strcpy(SendBuf,"Msg_Comm");//·¢ËÍ²éÑ¯Ö¡
		SendBuf[8] = 0;
		SendBuf[9] = 1;
		pBits1 = (BYTE *)&icountY;
		SendBuf[10] = *(pBits1+3);
		SendBuf[11] = *(pBits1+2);
		SendBuf[12] = *(pBits1+1);
		SendBuf[13] = *pBits1;
		if (m_nCommunicateMode == 0)
		{
			mLength = 14;
			if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
			{
				mLength = 64;
				CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
			}
		}
		else
		{
			send(m_socket,SendBuf,14,0);
			recv(m_socket,SendBuf,100,0);
		}
		if (memcmp(SendBuf,"Msg_",4) == 0)
		{
			if ((SendBuf[5] == 1) && (SendBuf[7] == 9))
				break;
		}
		Sleep(10);
	}

	unsigned int iBufLen;
		
	//²éÑ¯µÍ²ãÍ¨Ñ¶»º³åÇø³¤¶È
	while (1)
	{
		strcpy(SendBuf,"Buf_Long");//·¢ËÍ²éÑ¯Ö¡
		if (m_nCommunicateMode == 0)
		{
			mLength = 8;
			if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
			{
				mLength = 64;
				CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
			}
		}
		else
		{
			send(m_socket,SendBuf,8,0);
			recv(m_socket,SendBuf,100,0);
		}
		pBits1 = (BYTE *)&iBufLen;
		*(pBits1+3) = SendBuf[4];
		*(pBits1+2) = SendBuf[5];
		*(pBits1+1) = SendBuf[6];
		*pBits1 = SendBuf[7];
		if (iBufLen > icount)
			break;
		else
			Sleep(10);
	}
	strcpy(SendBuf,"Snd_Long");//·¢ËÍ²éÑ¯Ö¡
	pBits1 = (BYTE *)&iCountByte;
	SendBuf[8] = *(pBits1+3);
	SendBuf[9] = *(pBits1+2);
	SendBuf[10] = *(pBits1+1);
	SendBuf[11] = *pBits1;
	if (m_nCommunicateMode == 0)
	{
		mLength = 12;
		if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
		{
			mLength = 64;
			CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
		}
	}
	else
	{
		send(m_socket,SendBuf,12,0);
		recv(m_socket,SendBuf,100,0);
	}
	if(strncmp(SendBuf,"Msg_OkS ",8)!=0)
	{
		delete [] pBits;
		return;
	}
	pBits1 = pBits;
	if (m_nCommunicateMode == 0)
	{
		while (1)
		{
			mLength = (unsigned long)iCountByte;
			if (CH375WriteData(g_iDeviceIndex,(char *)pBits1,&mLength))  // ·¢ËÍ³É¹¦
			{
				if (iCountByte > mLength)
				{
					iCountByte -= mLength;
					pBits1 += mLength;
				}
				else
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
					break;
				}
			}
		}
	}
	else
	{
		send(m_socket,(char *)pBits1,iCountByte,0);
		while (1)
		{
			recv(m_socket,SendBuf,100,0);
			if(strncmp(SendBuf,"Msg_OkPG",8)==0)
				break;
		}
	}
	while (1)
	{
		strcpy(SendBuf,"Msg_Comm");//·¢ËÍÎÄ¼þ´«ËÍ½áÊøÖ¡
		SendBuf[8] = 0;
		SendBuf[9] = 2;
		if (m_nCommunicateMode == 0)
		{
			mLength = 10;
			if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
			{
				mLength = 64;
				CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
			}
		}
		else
		{
			send(m_socket,SendBuf,10,0);
			recv(m_socket,SendBuf,100,0);
		}
		if (memcmp(SendBuf,"Msg_",4) == 0)
		{
			if ((SendBuf[5] == 2) && (SendBuf[7] == 9))
				break;
		}
		Sleep(10);
	}
	delete [] pBits;
}

void CPlotCenterDlg::SearchNewJob()
{
	TCHAR szFullPath[_MAX_PATH];
  TCHAR szDir[_MAX_DIR];
  TCHAR szDrive[_MAX_DRIVE];
  CString strWorkPath1,strWorkPath2,strPathName;
  CFileFind filefind;
  BOOL bMoreFiles;
  CPlotItem *pPlotItem;
  POSITION pos1;
  BOOL bNewFile;
  BOOL bAddNewJob;
	int nReturnValue;
	long lPaperWidth;
	
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strWorkPath2.Format(_T("%s%sPlot\\"),szDrive,szDir);
	if (m_bCutFile)
	{//
		m_bCutFile = false;
		strPathName = theApp.m_strPrintFile;
		if (!strPathName.IsEmpty())
		{
			nReturnValue = OpenFile(strPathName);
			RemoveGLBuffer();
			if ((nReturnValue==0) && (m_lMaxPlotLength!=0) && (m_lMaxPlotWidth!=0))
			{
				CFileFixedPrint dlgFileFixedPrint;

				dlgFileFixedPrint.m_strFrameName = strPathName;
				if (dlgFileFixedPrint.DoModal()==IDOK)
				{
					if (theApp.m_lFixedPointLength >= ftoi(m_lMaxPlotLength*0.025))
					{
						_splitpath(strPathName,szDrive,szDir,NULL,NULL);
						strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
						if (strWorkPath1.CompareNoCase(strWorkPath2) == 0)
							DeleteFile(strPathName.GetBuffer(0));
						else if (strWorkPath1.CompareNoCase(theApp.m_strDataDir) == 0)
							DeleteFile(strPathName.GetBuffer(0));
					}
					else
					{
						if (m_lMaxPlotWidth > theApp.m_lPaperWidth)
						{//20100412 ÐÞÕý×Ô¶¯»æÍ¼Ä¿Â¼ÒòÂó¼Ü¹ý³¤¶øÔì³ÉÔËËãÒç³ö
							m_lMaxPlotLength = m_lMaxPlotLength * ((m_lMaxPlotWidth + theApp.m_lPaperWidth - 1) / theApp.m_lPaperWidth);
							m_lMaxPlotWidth = theApp.m_lPaperWidth;
						}
						pPlotItem=new CPlotItem(strPathName,1,theApp.m_lFixedPointLength,ftoi(m_lMaxPlotLength*0.025),ftoi(m_lMaxPlotWidth*0.025),theApp.m_lPaperWidth,m_lMaxPlotLength,m_lMaxPlotWidth,m_lMinPlotLength,m_lMinPlotWidth);
						m_listPlotItem.AddTail(pPlotItem);
						FillListCtrl();
					}
				}
				else
				{
					_splitpath(strPathName,szDrive,szDir,NULL,NULL);
					strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
					if (strWorkPath1.CompareNoCase(strWorkPath2) == 0)
						DeleteFile(strPathName.GetBuffer(0));
					else if (strWorkPath1.CompareNoCase(theApp.m_strDataDir) == 0)
						DeleteFile(strPathName.GetBuffer(0));
				}
			}
			else if (nReturnValue!=1)
			{
				_splitpath(strPathName,szDrive,szDir,NULL,NULL);
				strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
				if (strWorkPath1.CompareNoCase(strWorkPath2) == 0)
					DeleteFile(strPathName.GetBuffer(0));
				else if (strWorkPath1.CompareNoCase(theApp.m_strDataDir) == 0)
					DeleteFile(strPathName.GetBuffer(0));
			}
		}
		theApp.m_strPrintFile = " ";
		return;
	}

	strWorkPath1=strWorkPath2 + "*.*";
	bAddNewJob=false;
  bMoreFiles=filefind.FindFile(strWorkPath1);
  while (bMoreFiles)
  {
    bMoreFiles=filefind.FindNextFile();
    if ((!filefind.IsDirectory()) && (!filefind.IsDots()))
		{
      strPathName=filefind.GetFilePath();
			bNewFile=true;
			pos1=m_listPlotItem.GetHeadPosition();
			while ((pos1 != NULL) && bNewFile)
			{
				pPlotItem=(CPlotItem*)m_listPlotItem.GetNext(pos1);
				if (strPathName.CompareNoCase(pPlotItem->m_strPlotName) == 0)
					bNewFile=false;
			}
			if (bNewFile)
			{
				nReturnValue = OpenFile(strPathName);
				RemoveGLBuffer();
				if ((nReturnValue == 0) && (m_lMaxPlotLength!=0) && (m_lMaxPlotWidth!=0))
				{
					lPaperWidth = (long)(theApp.m_dPaperWidth * 40);
					if (m_lMaxPlotWidth > lPaperWidth)
					{//20100412 ÐÞÕý×Ô¶¯»æÍ¼Ä¿Â¼ÒòÂó¼Ü¹ý³¤¶øÔì³ÉÔËËãÒç³ö
						m_lMaxPlotLength = m_lMaxPlotLength * ((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth);
						m_lMaxPlotWidth = lPaperWidth;
					}
					pPlotItem=new CPlotItem(strPathName,1,0,ftoi(m_lMaxPlotLength*0.025),ftoi(m_lMaxPlotWidth*0.025),lPaperWidth,m_lMaxPlotLength,m_lMaxPlotWidth,m_lMinPlotLength,m_lMinPlotWidth);
					m_listPlotItem.AddTail(pPlotItem);
					bAddNewJob=true;
				}
				else if (nReturnValue != 1)
					DeleteFile(strPathName.GetBuffer(0));
			}
		}
  }

  if (strWorkPath2.CompareNoCase(theApp.m_strDataDir) != 0)
  {
    strWorkPath1=theApp.m_strDataDir;
    strWorkPath1=strWorkPath1 + "*.*";
    bMoreFiles=filefind.FindFile(strWorkPath1);
    while (bMoreFiles)
		{
      bMoreFiles=filefind.FindNextFile();
      if ((!filefind.IsDirectory()) && (!filefind.IsDots()))
			{
        strPathName=filefind.GetFilePath();
				bNewFile=true;
				pos1=m_listPlotItem.GetHeadPosition();
				while ((pos1 != NULL) && bNewFile)
				{
					pPlotItem=(CPlotItem*)m_listPlotItem.GetNext(pos1);
					if (strPathName.CompareNoCase(pPlotItem->m_strPlotName) == 0)
						bNewFile=false;
				}
        if (bNewFile)
				{
					nReturnValue = OpenFile(strPathName);
					RemoveGLBuffer();
					if ((nReturnValue == 0) && (m_lMaxPlotLength!=0) && (m_lMaxPlotWidth!=0))
					{
						lPaperWidth = (long)(theApp.m_dPaperWidth * 40);
						if (m_lMaxPlotWidth > lPaperWidth)
						{//20100412 ÐÞÕý×Ô¶¯»æÍ¼Ä¿Â¼ÒòÂó¼Ü¹ý³¤¶øÔì³ÉÔËËãÒç³ö
							m_lMaxPlotLength = m_lMaxPlotLength * ((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth);
							m_lMaxPlotWidth = lPaperWidth;
						}
						pPlotItem=new CPlotItem(strPathName,1,0,ftoi(m_lMaxPlotLength*0.025),ftoi(m_lMaxPlotWidth*0.025),lPaperWidth,m_lMaxPlotLength,m_lMaxPlotWidth,m_lMinPlotLength,m_lMinPlotWidth);
						m_listPlotItem.AddTail(pPlotItem);
						bAddNewJob=true;
					}
					else if (nReturnValue != 1)
						DeleteFile(strPathName.GetBuffer(0));
        }
      }
    }
  }
  if (bAddNewJob)
  {
	  FillListCtrl();
     
    //20151216×Ô¶¯»æÍ¼Ä¿Â¼´òÓ¡Ç°ÐÞ¸Ä·ÝÊý
	  times++;
	  if(times==1)
	  {
	int len=m_listPlotItem.GetCount();//»ñÈ¡ÁÐ±íÐÐÊý
	for(int i=0;i<len;i++)
	{
	FillListCtrl(i);

	CPlotItem *pPlotItem;
	POSITION pos1;
		
	if (m_wndPlotItem.GetItemCount() == m_listPlotItem.GetCount())
	{
	int nSelCount;
		//m_wndPlotItem.OnKeyDown;
		nSelCount = m_wndPlotItem.GetSelectedCount();
		if (nSelCount == 1) 
		{
			pos1 = m_wndPlotItem.GetFirstSelectedItemPosition();
			if(pos1) 
			{
				int nSelItem1;
				
				nSelItem1 = m_wndPlotItem.GetNextSelectedItem(pos1);
				pos1 = m_listPlotItem.FindIndex(nSelItem1);
				pPlotItem = (CPlotItem*)m_listPlotItem.GetNext(pos1);
				if (pPlotItem->m_nState == 0)
				{
					CString s1;
					if (m_nLanguage == 0)
						s1.LoadString(IDS_STRING309);
					else
						s1.LoadString(IDS_STRING309_EN);
					AfxMessageBox(s1);
				}
				else
				{
					CVerifyCopies VerifyCopiesDialog;
					VerifyCopiesDialog.m_nDialogFlag = 1;
					VerifyCopiesDialog.m_lWriteValue = pPlotItem->m_nCopyNum;
					VerifyCopiesDialog.m_strFileName = pPlotItem->m_strPlotName;
					if (VerifyCopiesDialog.DoModal() == IDOK)
					{
						if (VerifyCopiesDialog.m_lWriteValue != pPlotItem->m_nCopyNum)
						{
							pPlotItem->m_nCopyNum = VerifyCopiesDialog.m_lWriteValue;
							FillListCtrl();
						}
					}
				}
			}
		}
	}
	}
	  }
      else
	  {
       int length=m_listPlotItem.GetCount();
	   	FillListCtrl(length-1);

    	CPlotItem *pPlotItem;
    	POSITION pos1;
		
	if (m_wndPlotItem.GetItemCount() == m_listPlotItem.GetCount())
	{
	int nSelCount;
		//m_wndPlotItem.OnKeyDown;
		nSelCount = m_wndPlotItem.GetSelectedCount();
		if (nSelCount == 1) 
		{
			pos1 = m_wndPlotItem.GetFirstSelectedItemPosition();
			if(pos1) 
			{
				int nSelItem1;
				
				nSelItem1 = m_wndPlotItem.GetNextSelectedItem(pos1);
				pos1 = m_listPlotItem.FindIndex(nSelItem1);
				pPlotItem = (CPlotItem*)m_listPlotItem.GetNext(pos1);
				if (pPlotItem->m_nState == 0)
				{
					CString s1;
					if (m_nLanguage == 0)
						s1.LoadString(IDS_STRING309);
					else
						s1.LoadString(IDS_STRING309_EN);
					AfxMessageBox(s1);
				}
				else
				{
					CVerifyCopies VerifyCopiesDialog;
					VerifyCopiesDialog.m_nDialogFlag = 1;
					VerifyCopiesDialog.m_lWriteValue = pPlotItem->m_nCopyNum;
					VerifyCopiesDialog.m_strFileName = pPlotItem->m_strPlotName;
					if (VerifyCopiesDialog.DoModal() == IDOK)
					{
						if (VerifyCopiesDialog.m_lWriteValue != pPlotItem->m_nCopyNum)
						{
							pPlotItem->m_nCopyNum = VerifyCopiesDialog.m_lWriteValue;
							FillListCtrl();
						}
					}
				}
			}
		}
	}
	  }



  }

}

LRESULT CPlotCenterDlg::OnTransmitPercent(WPARAM wParam,LPARAM lParam)
{
  double dPercent;
	LV_ITEM lvitem;
  int nItem;
  CString strItem1;
  POSITION pos1;
  CPlotItem *pPlotItem;
	
  if (m_wndPlotItem.GetItemCount() != m_listPlotItem.GetCount())
		return true;

  pos1=m_listPlotItem.GetHeadPosition();
  nItem=0;
  while (pos1 != NULL)
  {
    pPlotItem=(CPlotItem*)m_listPlotItem.GetNext(pos1);
    if (pPlotItem->m_nState == 0)
		{
      lvitem.mask=LVIF_TEXT;
      lvitem.iItem=nItem;
      lvitem.iSubItem=4;
      strItem1.Format("%ld",pPlotItem->m_lSendLength);
		  lvitem.pszText=strItem1.GetBuffer(0);
      lvitem.iImage=0;
      m_wndPlotItem.SetItem(&lvitem);
			//dPercent = (double)(pPlotItem->m_lSendLength) / (double)(pPlotItem->m_lPictureLength - pPlotItem->m_lFixedPointLength) * 100.0;
			dPercent = pPlotItem->m_iPlotpercent;//½ø¶ÈÌõ³¤¶ÈÏÔÊ¾ÒÑ¾­´òÓ¡µÄ½ø¶È20150721-rwt
			m_wndPrintState.SetPos((int)dPercent);
			break;
    }
    nItem++;
  }
  return true;
}

LRESULT CPlotCenterDlg::OnEndTransmitFile(WPARAM wParam,LPARAM lParam)
{//ÎÄ¼þ´òÓ¡Íê
	CString strFileName,strCaption;
	CPlotItem *pPlotItem;
	
	m_pBmpGenerationThread=NULL;//ÖÕÖ¹Ïß³Ì
	m_pSendDataThread=NULL;

	if (m_listPlotItem.GetCount() > 0)
	{//´òÓ¡ÁÐ±íÖÐ»¹ÓÐÎÄ¼þ
		pPlotItem=(CPlotItem*)m_listPlotItem.GetHead();
		//ÅÐ¶ÏÎÄ¼þ×´Ì¬
		if ((pPlotItem->m_nState == 0) || 
			(pPlotItem->m_nState == 3) || 
			(pPlotItem->m_nState == 4))
		{//Õý´òÓ¡»ò±»É¾³ýµÄÎÄ¼þ
			if (pPlotItem->m_nCopyNum > 1)
			{//´òÓ¡·ÝÊýÊÇ·ñÎª¶à·Ý
				TCHAR szFullPath[_MAX_PATH];
				TCHAR szDir[_MAX_DIR];
				TCHAR szDrive[_MAX_DRIVE];
				FILE *pFile;
				SYSTEMTIME st;
			
				//Ð´ÀúÊ·¼ÇÂ¼
				::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
				_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
				strFileName.Format(_T("%s%shistory.ini"),szDrive,szDir);
				pFile=fopen((LPCTSTR)strFileName,"a");
				GetLocalTime(&st);
				fprintf(pFile,"%d:%d:%d|",st.wHour,st.wMinute,st.wSecond);
				fprintf(pFile,"%.2f%%|\n",(double)(pPlotItem->m_lSendLength) / (double)(pPlotItem->m_lPictureLength - pPlotItem->m_lFixedPointLength) * 100.0);
				fclose(pFile);

				pPlotItem->m_nCopyNum = pPlotItem->m_nCopyNum - 1;
				pPlotItem->m_lSendLength = 0;
				theApp.m_strPrintFile = " ";
				theApp.m_lFixedPointLength = 0;
				theApp.m_lPaperWidth = 2600;
				if (pPlotItem->m_nState != 0)
				{
					m_wndPrintState.SetPos(0);
					pPlotItem->m_nState = 2;
				}
				else
					pPlotItem->m_nState = 1;
			}
			else
			{
				TCHAR szFullPath[_MAX_PATH];
				TCHAR szDir[_MAX_DIR];
				TCHAR szDrive[_MAX_DRIVE];
				FILE *pFile;
				SYSTEMTIME st;
			
				::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
				_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
				strFileName.Format(_T("%s%shistory.ini"),szDrive,szDir);
				pFile=fopen((LPCTSTR)strFileName,"a");
				GetLocalTime(&st);
				fprintf(pFile,"%d:%d:%d|",st.wHour,st.wMinute,st.wSecond);
				fprintf(pFile,"%.2f%%|\n",(double)(pPlotItem->m_lSendLength) / (double)(pPlotItem->m_lPictureLength - pPlotItem->m_lFixedPointLength) * 100.0);
				fclose(pFile);

				strFileName.Format(_T("%s%sPlot\\"),szDrive,szDir);
				_splitpath(pPlotItem->m_strPlotName,szDrive,szDir,NULL,NULL);
				strCaption.Format(_T("%s%s"),szDrive,szDir);
				if ((strCaption.CompareNoCase(strFileName) == 0) ||
					(strCaption.CompareNoCase(theApp.m_strDataDir) == 0))
					DeleteFile(pPlotItem->m_strPlotName.GetBuffer(0));
				m_listPlotItem.RemoveHead();
				m_wndPlotItem.DeleteItem(0);
				theApp.m_strPrintFile = " ";
				theApp.m_lFixedPointLength = 0;
				theApp.m_lPaperWidth = 2600;
				m_wndPrintState.SetPos(0);
				if (pPlotItem->m_nState != 0)
				{
					delete pPlotItem;
					if (m_listPlotItem.GetCount() > 0)
					{
						pPlotItem=(CPlotItem*)m_listPlotItem.GetHead();
						if (pPlotItem->m_nState == 1)
							pPlotItem->m_nState = 2;
					}
				}
				else
					delete pPlotItem;
			}
		}
	}
	if (m_bConnected)
	{
		if (m_listPlotItem.GetCount() > 0)
		{
			pPlotItem=(CPlotItem*)m_listPlotItem.GetHead();
			if (pPlotItem->m_nState == 2)
				m_bWaitConnected = false;
		}
		else
		{  //ÎÄ¼þËÍÍê,¹Ø±ÕÍ¨ÐÅ¿Ú
			if (m_nCommunicateMode == 0)
				CH375CloseDevice(g_iDeviceIndex);
			else
				closesocket(m_socket);
			m_bConnected=false;
			if (m_nLanguage == 0)
				m_strConnectState.LoadString(IDS_STRING119);
			else
				m_strConnectState.LoadString(IDS_STRING119_EN);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_LINK,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_BREAK,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETUSB,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETNET,MF_BYCOMMAND | MF_ENABLED);
		}
	}
	if (m_listPlotItem.GetCount() == 0)
	{
		m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,false);
		m_nStartPrint = 0;
	}
	FillListCtrl();
	m_bNetworkBusy = false;
	GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAIN,MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAOUT,MF_BYCOMMAND | MF_ENABLED);
/*	if (m_bAutoRun && (m_listPlotItem.GetCount() == 0))
	{  //ÓÉÓ¦ÓÃ³ÌÐò×Ô¶¯ÔËÐÐÇÒÎÄ¼þËÍÍê
		CDialog::OnOK();
	}*/

	return true;
}

void CPlotCenterDlg::OnHistRecords() 
{
	// TODO: Add your command handler code here
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString strFileName;
	FILE *pFile;
	BOOL bWaitConnected;
	CPlotItem *pPlotItem;

	bWaitConnected = m_bWaitConnected;
	m_bWaitConnected = false;
	CViewHistory ViewHistoryDlg;
	ViewHistoryDlg.m_pParent = this;
	ViewHistoryDlg.DoModal();
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strFileName.Format(_T("%s%shistory.ini"),szDrive,szDir);

	pFile = fopen((LPCTSTR)strFileName,"r");
	if (pFile == NULL)
	{
		pFile = fopen((LPCTSTR)strFileName,"w");
		if (m_listPlotItem.GetCount() > 0)
		{
			pPlotItem = (CPlotItem*)m_listPlotItem.GetHead();
			if (pPlotItem->m_nState == 0)
			{
				fprintf(pFile,"%s|",pPlotItem->m_strPlotName);
				fprintf(pFile,"0000-00-00 ");
				fprintf(pFile,"00:00:00|");
			}
		}
	}
	fclose(pFile);
	m_bWaitConnected = bWaitConnected;
}

void CPlotCenterDlg::OpenHistoryFile(CString strFileName)
{
	m_bOpenFile = true;
	int nReturnValue = OpenFile(strFileName);
	if((nReturnValue == 0) && (m_lMaxPlotLength != 0) && (m_lMaxPlotWidth != 0))
	{
		CPlotItem *pPlotItem;
		long lPaperWidth = (long)(theApp.m_dPaperWidth * 40);
		if (m_lMaxPlotWidth > lPaperWidth)
		{
			m_lMaxPlotLength = m_lMaxPlotLength * (m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth;
			m_lMaxPlotWidth = lPaperWidth;
		}
		pPlotItem = new CPlotItem(strFileName,1,0,ftoi(m_lMaxPlotLength*0.025),ftoi(m_lMaxPlotWidth*0.025),lPaperWidth,m_lMaxPlotLength,m_lMaxPlotWidth,m_lMinPlotLength,m_lMinPlotWidth);
		m_listPlotItem.AddTail(pPlotItem);
		FillListCtrl();
	}
	else if(nReturnValue==1)
	{//ÎÄ¼þ´ò²»¿ª
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING106);
		else
			s1.LoadString(IDS_STRING106_EN);
		AfxMessageBox(s1);
	}
	else
	{//·ÇHP-GLÎÄ¼þ!
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING108);
		else
			s1.LoadString(IDS_STRING108_EN);
		AfxMessageBox(s1);
	}
	RemoveGLBuffer();
	m_bOpenFile=false;
}

void CPlotCenterDlg::PrintErrorY(double dGraphWidth, double dDockError)
{
	if (!m_bConnected)
		LinkAndReadPara(true);
	if ((!m_bConnected) || m_bNetworkBusy)
		return;

	char SendBuf[30];
	unsigned char *cPin;
	int i;
	unsigned long mLength;

	strcpy(SendBuf,"Msg_Comm");
	SendBuf[8]=0;
	SendBuf[9]=17;
	cPin = (unsigned char *)&dGraphWidth;
	for(i=10;i<18;i++)
	{
		SendBuf[i]=*cPin;
		cPin++;
	}
	cPin = (unsigned char *)&dDockError;
	for(;i<26;i++)
	{
		SendBuf[i]=*cPin;
		cPin++;
	}
	if (m_nCommunicateMode == 0)
	{
		mLength = 26;
		if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
		{
			mLength = 64;
			CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
		}
	}
	else
	{
		send(m_socket,SendBuf,26,0);
		recv(m_socket,SendBuf,30,0);
	}
	if (SendBuf[7] == 1)
	{
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING267);
		else
			s1.LoadString(IDS_STRING267_EN);
		AfxMessageBox(s1);
	}
}

//20100413·¢ËÍ²âÊÔ×ßÖ½Îó²îÃüÁî
void CPlotCenterDlg::PrintTrackPaperError(double dTrackPaperError)
{
	if (!m_bConnected)
		LinkAndReadPara(true);
	if ((!m_bConnected) || m_bNetworkBusy)
		return;

	char SendBuf[30];
	unsigned char *cPin;
	int i;
	unsigned long mLength;

	strcpy(SendBuf,"Msg_Comm");
	SendBuf[8]=0;
	SendBuf[9]=14;
	cPin = (unsigned char *)&dTrackPaperError;
	for(i=10;i<18;i++)
	{
		SendBuf[i]=*cPin;
		cPin++;
	}

	if (m_nCommunicateMode == 0)
	{
		mLength = 18;
		if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
		{
			mLength = 64;
			CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
		}
	}
	else
	{
		send(m_socket,SendBuf,18,0);
		recv(m_socket,SendBuf,30,0);
	}
	if (SendBuf[7] == 1)
	{
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING267);
		else
			s1.LoadString(IDS_STRING267_EN);
		AfxMessageBox(s1);
	}
}


void CPlotCenterDlg::SprayError(short *sSprayError)
{
	if (!m_bConnected)
		LinkAndReadPara(true);
	if ((!m_bConnected) || m_bNetworkBusy)
		return;

	char SendBuf[25];
	unsigned char *cPin;
	strcpy(SendBuf,"Msg_Comm");
	SendBuf[8]=0;
	SendBuf[9]=15;
	cPin=(unsigned char *)sSprayError;
	unsigned char i;
	unsigned long mLength;

	for(i=10;i<22;)
	{
		SendBuf[i]=*(cPin+1);
		SendBuf[i+1]=*cPin;
		cPin += 2;
		i += 2;
	}
	if (m_nCommunicateMode == 0)
	{
		mLength = 22;
		if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
		{
			mLength = 64;
			CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
		}
	}
	else
	{
		send(m_socket,SendBuf,22,0);
		recv(m_socket,SendBuf,20,0);
	}
	if (SendBuf[7] == 1)
	{
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING267);
		else
			s1.LoadString(IDS_STRING267_EN);
		AfxMessageBox(s1);
	}
}

void CPlotCenterDlg::OnPlotCenterHelp() 
{
	// TODO: Add your command handler code here
	if (m_nLanguage == 0)
		WinExec("hh.exe   HelpCN.chm",SW_SHOW);
	else
		WinExec("hh.exe   HelpEN.chm",SW_SHOW);
}

//½«×Ö·û×ª»»³ÉÏßÌõ
void CPlotCenterDlg::TextChangeLine(CPoint dp, LPCTSTR lpszString, int nCount)
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

	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	sWorkPath.Format(_T("%s%s"),szDrive,szDir);
	strEnglish=sWorkPath;
	strEnglish=strEnglish + _T("english.fnt");
	strChinese=sWorkPath;
	strChinese=strChinese + _T("hz.fnt");

	fileEnglish.Open((LPCTSTR)strEnglish,CFile::modeRead | CFile::typeBinary);
	fileChinese.Open((LPCTSTR)strChinese,CFile::modeRead | CFile::typeBinary);

	nLeft=0;
	nIndex1=0;
	while (nIndex1 < nCount)
	{
		pbyData=NULL;
		byByte1=(BYTE)lpszString[nIndex1];
		if ((byByte1 >= 128) && ((nIndex1+1) < nCount))
		{
			bEnglishChar=false;
			nIndex1++;
			byByte2=(BYTE)lpszString[nIndex1];
			wCharCode=(((WORD)byByte1) << 8) | (WORD)byByte2;
			nIndex2=0;
			nIndex3=m_aChinese_char.GetSize() - 1;
			if (m_aChinese_char[nIndex2].m_wCharCode == wCharCode)
				nIndex4=nIndex2;
      else if (m_aChinese_char[nIndex3].m_wCharCode == wCharCode)
				nIndex4=nIndex3;
      else
				nIndex4=(nIndex2 + nIndex3) / 2;
      while (((nIndex3 - nIndex2) > 1) && (m_aChinese_char[nIndex4].m_wCharCode != wCharCode))
			{
        if (m_aChinese_char[nIndex4].m_wCharCode > wCharCode)
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
      if ((m_aChinese_char[nIndex4].m_wCharCode == wCharCode) &&
          (m_aChinese_char[nIndex4].m_wBytes > 0))
			{
        pbyData=new BYTE[m_aChinese_char[nIndex4].m_wBytes + 1];
        fileChinese.Seek(m_aChinese_char[nIndex4].m_dwPosition,CFile::begin);
        fileChinese.Read(pbyData,m_aChinese_char[nIndex4].m_wBytes);
      }
    }
    else
		{
      bEnglishChar=true;
      wCharCode=(WORD)byByte1;
      nIndex2=0;
      nIndex3=m_aEnglish_char.GetSize() - 1;
      if (m_aEnglish_char[nIndex2].m_wCharCode == wCharCode)
				nIndex4=nIndex2;
      else if (m_aEnglish_char[nIndex3].m_wCharCode == wCharCode)
				nIndex4=nIndex3;
      else
				nIndex4=(nIndex2 + nIndex3) / 2;
      while (((nIndex3 - nIndex2) > 1) && (m_aEnglish_char[nIndex4].m_wCharCode != wCharCode))
			{
        if (m_aEnglish_char[nIndex4].m_wCharCode > wCharCode)
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
      if ((m_aEnglish_char[nIndex4].m_wCharCode == wCharCode) &&
          (m_aEnglish_char[nIndex4].m_wBytes > 0))
			{
        pbyData=new BYTE[m_aEnglish_char[nIndex4].m_wBytes + 1];

        fileEnglish.Seek(m_aEnglish_char[nIndex4].m_dwPosition,CFile::begin);
        fileEnglish.Read(pbyData,m_aEnglish_char[nIndex4].m_wBytes);
      }
    }
    if (pbyData != NULL)
		{
      if (bEnglishChar)
				PlotChar(dp,pbyData,m_aEnglish_char[nIndex4].m_wBytes,nLeft,0,m_nHeight_C);
     else
				PlotChar(dp,pbyData,m_aChinese_char[nIndex4].m_wBytes,nLeft,0,m_nInternalLeading_E + m_nHeight_E);
      delete pbyData;
      if (bEnglishChar)
				nLeft=nLeft + m_aEnglish_char[nIndex4].m_wWidth * m_nHeight_C;
      else
				nLeft=nLeft + m_nWidth_C * (m_nInternalLeading_E + m_nHeight_E);
    }
    else
		{
      if (bEnglishChar)
				nLeft=nLeft + m_aEnglish_char[0].m_wWidth * m_nHeight_C;
      else
				nLeft=nLeft + m_nWidth_C * (m_nInternalLeading_E + m_nHeight_E);
    }
    nIndex1++;
  }
  fileEnglish.Close();
  fileChinese.Close();
}

//ÊäÈë²ÎÊý:
//  ptPointLU  ÏÔÊ¾×Ö·û´®µÄ×óÉÏ½Ç×ø±ê(¼´TextOut(...)µÄx,yÖµ)
//  pbyData ×Ö·ûµÄÃèÊöÊý¾Ý
//  wBytes ×Ö·ûµÄÃèÊöÊý¾Ý¸öÊý
//  nLeft,nDown ×Ö·ûµÄ×óÏÂ½Ç×ø±ê(¼´ÒÔÓ¢ºº×Ö¿âµÄ¹«¹²µ¥Î»¼°×ø±êË®Æ½»æÖÆ×Ö·û´®Ê±,×Ö·ûµÄµÄ×óÏÂ½Ç×ø±ê)
//  nFontScale ×Ö¿âµÄ³¤¶È±ÈÀý
void CPlotCenterDlg::PlotChar(CPoint ptPointLU, BYTE *pbyData, WORD wBytes, int nLeft, int nDown, int nFontScale)
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
	double PI=3.14159265359;
  int nHeight;

  if (wBytes == 0)
		return;

  nHeight=(m_nInternalLeading_E + m_nHeight_E) * m_nHeight_C;
  dLToDScale=(double)nHeight / 	(m_dTextHeight*400);//Êµ¼ÊºÁÃ×Êý

  dCos=cos(m_dTextAngle/180.0*PI);
  dSin=sin(m_dTextAngle/180.0*PI);

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

        //¼ÆËãÔ²ÐÄ
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

        //¼ÆËãÔ²ÐÄ
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

void CPlotCenterDlg::InitTextChangeLine(void)
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

	m_ptCurrentPosition=NULL;

  ::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
  _splitpath(szFullPath,szDrive,szDir,NULL,NULL);
  sWorkPath.Format(_T("%s%s"),szDrive,szDir);

  strCharFileName=sWorkPath;
  strCharFileName=strCharFileName + _T("english.fnt");
  if (!fFile1.Open((LPCTSTR)strCharFileName,CFile::modeRead | CFile::typeBinary))
  {
	  AfxMessageBox("can not find english.fnt");
  }
  fFile1.SeekToBegin();
  fFile1.Read(byte,10);
  m_nInternalLeading_E=(int)((WORD)byte[0] | ((WORD)byte[1] << 8));
  m_nHeight_E=(int)((WORD)byte[2] | ((WORD)byte[3] << 8));
  m_nDescent_E=(int)((WORD)byte[4] | ((WORD)byte[5] << 8));
  nCharCount_E=(int)((DWORD)byte[6] | ((DWORD)byte[7] << 8) | ((DWORD)byte[9] << 16) | ((DWORD)byte[9] << 24));
  m_aEnglish_char.RemoveAll();
  m_aEnglish_char.SetSize(nCharCount_E);
  fFile1.Seek(10,CFile::begin);
  nCountChar=nCharCount_E;
  while (nCountChar > 0)
  {
    fFile1.Read(byte,10);

    English_char1.m_wCharCode=(WORD)byte[0] | ((WORD)byte[1] << 8);
    English_char1.m_wBytes=(WORD)byte[2] | ((WORD)byte[3] << 8);
    English_char1.m_wWidth=(WORD)byte[4] | ((WORD)byte[5] << 8);
    English_char1.m_dwPosition=(DWORD)byte[6] | ((DWORD)byte[7] << 8) | ((DWORD)byte[9] << 16) | ((DWORD)byte[9] << 24);

    m_aEnglish_char.SetAt(nCharCount_E-nCountChar,English_char1);

    nCountChar--;
  }
  fFile1.Close();

  strCharFileName=sWorkPath;
  strCharFileName=strCharFileName + _T("hz.fnt");
  fFile1.Open((LPCTSTR)strCharFileName,CFile::modeRead | CFile::typeBinary);
  fFile1.SeekToBegin();
  fFile1.Read(byte,8);
  m_nWidth_C=(int)((WORD)byte[0] | ((WORD)byte[1] << 8));
  m_nHeight_C=(int)((WORD)byte[2] | ((WORD)byte[3] << 8));
  nCharCount_C=(int)((DWORD)byte[4] | ((DWORD)byte[5] << 8) | ((DWORD)byte[6] << 16) | ((DWORD)byte[7] << 24));

  m_aChinese_char.RemoveAll();
  m_aChinese_char.SetSize(nCharCount_C);
  fFile1.Seek(8,CFile::begin);
  nCountChar=nCharCount_C;
  while (nCountChar > 0)
  {
    fFile1.Read(byte,8);

    Chinese_char1.m_wCharCode=(WORD)byte[0] | ((WORD)byte[1] << 8);
    Chinese_char1.m_wBytes=(WORD)byte[2] | ((WORD)byte[3] << 8);
    Chinese_char1.m_dwPosition=(DWORD)byte[4] | ((DWORD)byte[5] << 8) | ((DWORD)byte[6] << 16) | ((DWORD)byte[7] << 24);

    m_aChinese_char.SetAt(nCharCount_C-nCountChar,Chinese_char1);

    nCountChar--;
  }
  fFile1.Close();
}

CPoint CPlotCenterDlg::CPToLP(CPoint ptCP, int nHeight, CPoint ptPointLU, double dSin, double dCos, double dScale)
{
	double dx,dy;
  CPoint ptPoint1;

  dx=(double)ptCP.x;
  dy=(double)ptCP.y-(double)nHeight;
  ptPoint1.x=ptPointLU.x + IntRound((dx*dCos - dy*dSin) / dScale);
  ptPoint1.y=ptPointLU.y + IntRound((dx*dSin + dy*dCos) / dScale);

  return ptPoint1;
}

void CPlotCenterDlg::MoveTo(long x, long y)
{
	m_ptCurrentPosition.x=x;
	m_ptCurrentPosition.y=y;

  m_ptPrevPosition=m_ptCurrentPosition;
  m_ptPrevPosition.x=m_ptPrevPosition.x +1;  //ÎªÔÚLineToÖÐÓÅ»¯ÓÃ
}

void CPlotCenterDlg::LineTo(long x, long y)
{
  CPoint ptCurrentPoint;
	CPoint ptPoint1;
	CPoint ptPoint2;
	CRect rect1;
  
	ptCurrentPoint.x=x;
	ptCurrentPoint.y=y;
//	rect1=CRect(0,0,m_sizePageSize.cx,m_sizePageSize.cy);

	ptPoint1=m_ptCurrentPosition;
	ptPoint2=ptCurrentPoint;
	if ((m_ptCurrentPosition == m_ptPrevPosition) &&
			(0 < m_ptCurrentPosition.x) && (0 < m_ptCurrentPosition.y))
	{
	}
	else
	{
		WriteTextData(true,ptPoint1.x,ptPoint1.y);
	}
	WriteTextData(false,ptPoint2.x,ptPoint2.y);
  m_ptPrevPosition = ptCurrentPoint;
  m_ptCurrentPosition = ptCurrentPoint;
}

int CPlotCenterDlg::IntRound(double x)
{
	double d;
  int nResult;

  if (x<0)
    d=x-0.5;
  else
    d=x+0.5;

  if ((d > 0) && (d > (double)INT_MAX))  {
    nResult=INT_MAX;
  }
  else  {
    if ((d < 0) && (d < (double)INT_MIN))  {
      nResult=INT_MIN;
    }
    else  {
      nResult=(int)d;
    }
  }

  return nResult;
}

void CPlotCenterDlg::Arc(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXStartArc, int nYStartArc, int nXEndArc, int nYEndArc)
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
  dStep=PI / 180.0 * (360.0 / (double)nCount);  //3¶È
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

BOOL CPlotCenterDlg::CircleCR(double x1, double y1, double x2, double y2, double x3, double y3, double &cx, double &cy, double &cr)
{
  double a1,b1,c1,a2,b2,c2;
  BOOL bOk;

  if (((x1==x2) && (y1==y2)) ||
      ((x1==x3) && (y1==y3)) ||
      ((x2==x3) && (y2==y3)))  {  //ÈýµãÖÐÓÐÁ½µãÏàÍ¬
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

CPoint CPlotCenterDlg::GetNextCoodinate(WORD wDirection, WORD wLength, CPoint ptPoint)
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


double CPlotCenterDlg::angle_2(double startx, double starty, double endx, double endy)
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
    if (x < 0)  {  //µÚ¶þÈýÏóÏÞ
      q1=pi+q1;
    }
    else  {
      if ((x > 0) && (y < 0))  {  //µÚËÄÏóÏÞ
        q1=(double)2.0*pi+q1;
      }
    }
  }
  return q1;
}

//ÇóÁ½Ö±ÏßµÄ½»µã
//Èë¿Ú²ÎÊý
//  a1,b1,c1 Ö±Ïßa1*x+b1*y+c1=0
//  a2,b2,c2 Ö±Ïßa2*x+b2*y+c2=0
//³ö¿Ú²ÎÊý
//  (x,y) ½»µã×ø±ê
//·µ»ØÖµ
//  =0,Ã»ÓÐ½»µã
//  =1,ÓÐ½»µã(x,y)
//  =2,Á½Ö±ÏßÖØºÏ
int CPlotCenterDlg::IntOfTwoLine(double a1, double b1, double c1, double a2, double b2, double c2, double &x, double &y)
{
  double p,q;
  int nResult;

  p=a1*b2-a2*b1;
  q=c2*b1-c1*b2;
  if ((fabs(p)+1.0) <= 1.0)  {  //Á½Ö±Ïß¶ÎÆ½ÐÐ
    if ((fabs(q)+1.0) <= 1.0)  {  //Á½Ö±ÏßÖØºÏ
      nResult=2;
    }
    else  {  //Á½Ö±Ïß¶ÎÆ½ÐÐµ«²»ÖØºÏ
      nResult=0;
    }
  }
  else  {  //Á½Ö±Ïß¶ÎÓÐ½»µã
    x=q/p;
    q=a2*c1-a1*c2;
    y=q/p;
    nResult=1;
  }
  return nResult;
}

void CPlotCenterDlg::WriteTextData(BOOL bFirst,long x,long y)
{
	CGLPrimitive *p;

	if(bFirst)
	{
		p=new CGLPrimitive;
		theApp.m_listPrimitive.AddTail(p);
		p->m_nType=PRIMITIVE_LINE;
		p->m_anData.Add(x);
		p->m_anData.Add(y);
	}
	else
	{
		p=(CGLPrimitive*)theApp.m_listPrimitive.GetTail();
		p->m_anData.Add(x);
		p->m_anData.Add(y);
	}
}

void CPlotCenterDlg::OnTest135() 
{
	// TODO: Add your command handler code here
	CString strPathName;

	strPathName =_T("test135.plt");
	PrintTestGraph(strPathName);
}

void CPlotCenterDlg::OnTest160() 
{
	// TODO: Add your command handler code here
	CString strPathName;

	strPathName =_T("test160.plt");
	PrintTestGraph(strPathName);
}

void CPlotCenterDlg::OnTest180() 
{
	// TODO: Add your command handler code here
	CString strPathName;

	strPathName =_T("test180.plt");
	PrintTestGraph(strPathName);
}

void CPlotCenterDlg::OnTest200() 
{
	// TODO: Add your command handler code here
	CString strPathName;

	strPathName =_T("test200.plt");
	PrintTestGraph(strPathName);
}

void CPlotCenterDlg::OnTest220() 
{
	// TODO: Add your command handler code here
	CString strPathName;

	strPathName =_T("test220.plt");
	PrintTestGraph(strPathName);
}

void CPlotCenterDlg::PrintTestGraph(CString strPathName)
{
	TCHAR szFullPath[_MAX_PATH];
  TCHAR szDir[_MAX_DIR];
  TCHAR szDrive[_MAX_DRIVE];
  CString strWorkPath1;
  CPlotItem *pPlotItem;
 	int nReturnValue;
	long lPaperWidth;
	
	if (!m_bConnected)
		LinkAndReadPara(true);

	if (!m_bConnected)
		return;

	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strWorkPath1.Format(_T("%s%s"),szDrive,szDir);
	strWorkPath1 += strPathName;

	m_bOpenFile=true;
	nReturnValue = OpenFile(strWorkPath1);
	RemoveGLBuffer();
	if ((nReturnValue == 0) && (m_lMaxPlotLength!=0) && (m_lMaxPlotWidth!=0))
	{
		lPaperWidth = (long)(theApp.m_dPaperWidth * 40);
		if (m_lMaxPlotWidth > lPaperWidth)
		{
			m_lMaxPlotLength = m_lMaxPlotLength * (m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth;
			m_lMaxPlotWidth = lPaperWidth;
		}
		pPlotItem=new CPlotItem(strWorkPath1,1,0,ftoi(m_lMaxPlotLength*0.025),ftoi(m_lMaxPlotWidth*0.025),lPaperWidth,m_lMaxPlotLength,m_lMaxPlotWidth,m_lMinPlotLength,m_lMinPlotWidth);
		m_listPlotItem.AddTail(pPlotItem);
		FillListCtrl();
	}
	else if(nReturnValue==1)
	{//ÎÄ¼þ´ò²»¿ª
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING106);
		else
			s1.LoadString(IDS_STRING106_EN);
		AfxMessageBox(s1);
	}
	else
	{//·ÇHP-GLÎÄ¼þ!
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING108);
		else
			s1.LoadString(IDS_STRING108_EN);
		AfxMessageBox(s1);
	}
	m_bOpenFile=false;
}

void CPlotCenterDlg::OnSetUsb() 
{
	// TODO: Add your command handler code here
	if ((m_nCommunicateMode != 0) && (m_bConnected == false))
	{
		m_nCommunicateMode = 0;
		theApp.m_nCommunicateMode = m_nCommunicateMode;
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_SETUSB,MF_CHECKED);
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_SETNET,MF_UNCHECKED);
	}
}

// 20101118
char  USBDeviceState;
VOID CALLBACK NotifyRoutine (  // Éè±¸ÊÂ¼þÍ¨Öª»Øµ÷³ÌÐò
	  ULONG	iEventStatus )  // Éè±¸ÊÂ¼þºÍµ±Ç°×´Ì¬: 0=Éè±¸°Î³öÊÂ¼þ, 3=Éè±¸²åÈëÊÂ¼þ
{
	//ÔÚÖÐ¶Ï³ÌÐòÖÐ¶ÔÏµÍ³×ÊÔ´²»ÄÜ²Ù×÷Ì«¶à,ËùÒÔ½«²å°ÎÊÂ¼þ´¦ÀíÍ¨¹ýPostMessage()·Åµ½IDC_INT_DEVARRIVALÖÐ´¦Àí
	if(iEventStatus==CH375_DEVICE_ARRIVAL)  
		USBDeviceState = 3;	//½«Éè±¸²åÈëÏûÏ¢·¢ËÍµ½´°Ìå½øÐÐ´¦Àí			
	else if(iEventStatus==CH375_DEVICE_REMOVE)	
		USBDeviceState = 0;	//½«Éè±¸°Î³öÏûÏ¢·¢ËÍµ½´°Ìå½øÐÐ´¦Àí		
	return;
}

void CPlotCenterDlg::ConnectUSB(BOOL bDisplayFlag)
{
	if (m_bConnected)
	{
		ReadParaFromPlotter();
		return;
	}
	
	if(!CH375SetDeviceNotify(0,NULL,NotifyRoutine)) //ÉèÖÃÉè±¸²å°Î¼àÊÓ// 20101118
		AfxMessageBox("ÉèÖÃ¼àÊÓCH372/CH375Éè±¸²å°ÎÊ§°Ü");
	
	CString s1;
	
	bool bDeviceOpen = false;
	char dispbuf[3];
	while(g_iDeviceIndex < 10)
	{
		if (CH375OpenDevice(g_iDeviceIndex) != INVALID_HANDLE_VALUE)
		{
			bDeviceOpen = true;
			break;
		}
		g_iDeviceIndex++;
	}
	CH375SetExclusive(g_iDeviceIndex, 1);// ¶ÀÕ¼Ê¹ÓÃµ±Ç°USBÉè±¸
	_itoa(g_iDeviceIndex, dispbuf, 10);

	//if (CH375OpenDevice(0) == INVALID_HANDLE_VALUE)
	if (!bDeviceOpen)
	{
		m_bConnected = FALSE;
		if (bDisplayFlag)
		{
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING301);
			else
				s1.LoadString(IDS_STRING301_EN);
			AfxMessageBox(s1);
		}
	}
	else
	{
		unsigned long lUsb_Pid_Vid;
		lUsb_Pid_Vid = CH375GetUsbID(g_iDeviceIndex);
		if ((lUsb_Pid_Vid != USB_PID_VID_0) && ( lUsb_Pid_Vid != USB_PID_VID_1 ))
		{
			CH375CloseDevice(g_iDeviceIndex);
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING303);
			else
				s1.LoadString(IDS_STRING303_EN);
			AfxMessageBox(s1);
			return;
		}
		CH375SetTimeout( g_iDeviceIndex, 3000, 3000 ); // 20101118
		USBDeviceState = 3;
		m_bConnected = TRUE;
		ReadParaFromPlotter();
		m_bWaitConnected = false;
		//if (m_nLanguage == 0)
		//	m_strConnectState.LoadString(IDS_STRING120);
		//else
		//	m_strConnectState.LoadString(IDS_STRING120_EN);
		char cbuf[50] = "USB";
		strcat(cbuf,dispbuf);
		m_strConnectState=cbuf;
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_BREAK,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_LINK,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAIN,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAOUT,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST135,MF_BYCOMMAND | MF_ENABLED);
		GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST160,MF_BYCOMMAND | MF_ENABLED);
		if (m_sPlotType>=2230)
		{
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_ENABLED);
		}
		else if (m_sPlotType>=2030)
		{
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else if (m_sPlotType>=1830)
		{
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_ENABLED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else
		{
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST180,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST200,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			GetMenu()->GetSubMenu(0)->EnableMenuItem(ID_TEST220,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETNET,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(3)->EnableMenuItem(ID_SETUSB,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		UpdateData(false);
	}
}
//20100413²ÎÊýµ¼Èë
void CPlotCenterDlg::OnParaIn() 
{
	// TODO: Add your command handler code here
	if (!m_bConnected)
	{
		LinkAndReadPara(true);
		if (!m_bConnected)
			return;
	}
	if (m_bNetworkBusy)
		return;

	unsigned char *cPin;
	unsigned long mLength;
	short sSprayError;
	double dPlotterError;
	int i;
	char SendBuf[100];

	int nLineWidth;
	unsigned char cPrintStyle;
	unsigned char cSprinklerCount;
	unsigned char cSprinklerNo;
	double dOverlapError; //20100413 ²ÎÊý¶ÁÈ¡ ×ßÖ½Îó²î
	double dMalposedError;
	double dPlotterXError;
	double dPlotterYError;

	short sSprayErrorY12;
	short sSprayErrorY23;
	short sSprayErrorY34;
	short sSprayErrorX12;
	short sSprayErrorX23;
	short sSprayErrorX34;

	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString strFileName;
	CStdioFile file;
	char RecBuf[50];

	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strFileName.Format(_T("%s%sPlotPara.ini"),szDrive,szDir);

	while (1)
	{
		if (!file.Open(strFileName,CFile::modeRead | CFile::typeText))
			break;
		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"LineWidth=",10) != 0)
			break;
		nLineWidth = (int)atoi(&RecBuf[10]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"PrintStyle=",11) != 0)
			break;
		cPrintStyle = (unsigned char)atoi(&RecBuf[11]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"SprayCount=",11) != 0)
			break;
		cSprinklerCount = (unsigned char)atoi(&RecBuf[11]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"SprayNo=",8) != 0)
			break;
		cSprinklerNo = (unsigned char)atoi(&RecBuf[8]);

		//20100413¶ÁÈ¡²ÎÊýÎÄ¼þÖÐµÄ--×ßÖ½Îó²îÖµ
		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"OverlapError=",13) != 0)
			break;
		dOverlapError = atof(&RecBuf[13]);
		//***************

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"ShiftError=",11) != 0)
			break;
		dMalposedError = atof(&RecBuf[11]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"ShiftError1/2=",14) != 0)
			break;
		sSprayErrorY12 = (short)atoi(&RecBuf[14]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"ShiftError2/3=",14) != 0)
			break;
		sSprayErrorY23 = (short)atoi(&RecBuf[14]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"ShiftError3/4=",14) != 0)
			break;
		sSprayErrorY34 = (short)atoi(&RecBuf[14]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"OverlapError1/2=",16) != 0)
			break;
		sSprayErrorX12 = (short)atoi(&RecBuf[16]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"OverlapError2/3=",16) != 0)
			break;
		sSprayErrorX23 = (short)atoi(&RecBuf[16]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"OverlapError3/4=",16) != 0)
			break;
		sSprayErrorX34 = (short)atoi(&RecBuf[16]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"PlotterXError=",14) != 0)
			break;
		dPlotterXError = atof(&RecBuf[14]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"PlotterYError=",14) != 0)
			break;
		dPlotterYError = atof(&RecBuf[14]);


		if (m_nVersion == 0)
		{
			//ÉèÖÃ´òÓ¡Ä£Ê½
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=12;
			SendBuf[10]=0;
			SendBuf[11]=cPrintStyle;
			if (m_nCommunicateMode == 0)
			{
				mLength = 12;
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				send(m_socket,SendBuf,12,0);
				recv(m_socket,SendBuf,20,0);
			}
			if (SendBuf[7] != 9)
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING252);
				else
					s1.LoadString(IDS_STRING252_EN);
				AfxMessageBox(s1);
				return;
			}
			m_cPrintStyle = cPrintStyle;

			//ÉèÖÃÅçÍ·Ñ¡Ôñ
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=13;
			SendBuf[10]=0;
			SendBuf[11]=cSprinklerCount;
			SendBuf[12]=0;
			SendBuf[13]=cSprinklerNo;
			if (m_nCommunicateMode == 0)
			{
				mLength = 14;
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				send(m_socket,SendBuf,14,0);
				recv(m_socket,SendBuf,30,0);
			}
			if (SendBuf[7] != 9)
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING254);
				else
					s1.LoadString(IDS_STRING254_EN);
				AfxMessageBox(s1);
				return;
			}
			m_cSprinklerCount = cSprinklerCount;
			m_cSprinklerNo = cSprinklerNo;

			//ÉèÖÃÅçÍ·¼äÎó²î
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=11;
			sSprayError = sSprayErrorX12;
			cPin=(unsigned char *)&sSprayError;
			SendBuf[10]=*(cPin+1);
			SendBuf[11]=*cPin;
			sSprayError = sSprayErrorX23;
			SendBuf[12]=*(cPin+1);
			SendBuf[13]=*cPin;
			sSprayError = sSprayErrorX34;
			SendBuf[14]=*(cPin+1);
			SendBuf[15]=*cPin;
			sSprayError=sSprayErrorY12;
			SendBuf[16]=*(cPin+1);
			SendBuf[17]=*cPin;
			sSprayError=sSprayErrorY23;
			SendBuf[18]=*(cPin+1);
			SendBuf[19]=*cPin;
			sSprayError=sSprayErrorY34;
			SendBuf[20]=*(cPin+1);
			SendBuf[21]=*cPin;
			if (m_nCommunicateMode == 0)
			{
				mLength = 22;
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				send(m_socket,SendBuf,22,0);
				recv(m_socket,SendBuf,30,0);
			}
			if (SendBuf[7] != 9)
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING256);
				else
					s1.LoadString(IDS_STRING256_EN);
				AfxMessageBox(s1);
				return;
			}
			m_sSprayErrorX12 = sSprayErrorX12;
			m_sSprayErrorX23 = sSprayErrorX23;
			m_sSprayErrorX34 = sSprayErrorX34;
			m_sSprayErrorY12 = sSprayErrorY12;
			m_sSprayErrorY23 = sSprayErrorY23;
			m_sSprayErrorY34 = sSprayErrorY34;
			
			//ÉèÖÃË«Ïò´òÓ¡Îó²î
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=10;
			dPlotterError = 0;
			cPin = (unsigned char *)&dPlotterError;
			for(i=10;i<18;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			dPlotterError = dMalposedError;
			cPin = (unsigned char *)&dPlotterError;
			for(;i<26;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			if (m_nCommunicateMode == 0)
			{
				mLength = 26;
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				send(m_socket,SendBuf,26,0);
				recv(m_socket,SendBuf,30,0);
			}
			if (SendBuf[7] != 9)
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING258);
				else
					s1.LoadString(IDS_STRING258_EN);
				AfxMessageBox(s1);
				return;
			}
			m_dMalposedError = dMalposedError;
				
			//ÉèÖÃÍ¼ÐÎÎó²î
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=6;
			dPlotterError = dPlotterXError;
			cPin = (unsigned char *)&dPlotterError;
			for(i=10;i<18;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			dPlotterError = dPlotterYError;
			cPin = (unsigned char *)&dPlotterError;
			for(;i<26;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			if (m_nCommunicateMode == 0)
			{
				mLength = 26;
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				send(m_socket,SendBuf,26,0);
				recv(m_socket,SendBuf,30,0);
			}
			if (SendBuf[7] != 9)
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING260);
				else
					s1.LoadString(IDS_STRING260_EN);
				AfxMessageBox(s1);
				return;
			}
			m_dPlotterXError = dPlotterXError;
			m_dPlotterYError = dPlotterYError;
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING293);
			else
				s1.LoadString(IDS_STRING293_EN);
			AfxMessageBox(s1);
		}
		else if (m_nVersion == 1)
		{
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=81;
			SendBuf[10]=0;
			SendBuf[11]=cPrintStyle;
			SendBuf[12]=0;
			SendBuf[13]=cSprinklerCount;
			SendBuf[14]=0;
			SendBuf[15]=cSprinklerNo;
			SendBuf[16]=0;
			SendBuf[17]=(char)nLineWidth;
			sSprayError = sSprayErrorX12;
			cPin=(unsigned char *)&sSprayError;
			SendBuf[18]=*(cPin+1);
			SendBuf[19]=*cPin;
			sSprayError = sSprayErrorX23;
			SendBuf[20]=*(cPin+1);
			SendBuf[21]=*cPin;
			sSprayError = sSprayErrorX34;
			SendBuf[22]=*(cPin+1);
			SendBuf[23]=*cPin;
			sSprayError=sSprayErrorY12;
			SendBuf[24]=*(cPin+1);
			SendBuf[25]=*cPin;
			sSprayError=sSprayErrorY23;
			SendBuf[26]=*(cPin+1);
			SendBuf[27]=*cPin;
			sSprayError=sSprayErrorY34;
			SendBuf[28]=*(cPin+1);
			SendBuf[29]=*cPin;
			//20100413 ÉèÖÃ×ßÖ½Îó²î
			//¼æÈÝÐÔÊÇ·ñ·¢ËÍ×ßÖ½Îó²î20100413
			if(g_nNewVersion == 1)//#1 20100413
				dPlotterError=dOverlapError;
			else
				dPlotterError = dMalposedError;
			cPin = (unsigned char *)&dPlotterError;
			for(i=30;i<38;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}

			if(g_nNewVersion == 1)//#2 20100413
				dPlotterError = dMalposedError;
			else
				dPlotterError = dPlotterXError;			
			cPin = (unsigned char *)&dPlotterError;
			for(;i<46;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}

			if(g_nNewVersion == 1)//#3 20100413
				dPlotterError = dPlotterXError;
			else
				dPlotterError = dPlotterYError;
			cPin = (unsigned char *)&dPlotterError;
			for(;i<54;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			
			if(g_nNewVersion == 1)//#4 20100413
			{
				dPlotterError = dPlotterYError;
				cPin = (unsigned char *)&dPlotterError;
				for(;i<62;i++)
				{
					SendBuf[i]=*cPin;
					cPin++;
				}	
			}
			
			if (m_nCommunicateMode == 0)
			{
				if(g_nNewVersion == 1)//#5 20100413
					mLength = 62;
				else
					mLength = 54;
					
				if (CH375WriteData(g_iDeviceIndex,SendBuf,&mLength))  // ·¢ËÍ³É¹¦
				{
					mLength = 64;
					CH375ReadData(g_iDeviceIndex,SendBuf,&mLength);
				}
			}
			else
			{
				if(g_nNewVersion == 1)//#6 20100413
					send(m_socket,SendBuf,62,0);
				else
					send(m_socket,SendBuf,54,0);					
				
				recv(m_socket,SendBuf,30,0);
			}
			if (SendBuf[7] == 9)
			{//²ÎÊýÉèÖÃ³É¹¦
				m_cPrintStyle = cPrintStyle;
				m_cSprinklerCount = cSprinklerCount;
				m_cSprinklerNo = cSprinklerNo;
				m_nLineWidth = nLineWidth;
				m_dOverlapError = dOverlapError;//20100413 ²ÎÊýÉèÖÃ³É¹¦±£´æ
				m_dMalposedError = dMalposedError;
				m_sSprayErrorX12 = sSprayErrorX12;
				m_sSprayErrorX23 = sSprayErrorX23;
				m_sSprayErrorX34 = sSprayErrorX34;
				m_sSprayErrorY12 = sSprayErrorY12;
				m_sSprayErrorY23 = sSprayErrorY23;
				m_sSprayErrorY34 = sSprayErrorY34;
				m_dPlotterXError = dPlotterXError;
				m_dPlotterYError = dPlotterYError;
				
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING293);
				else
					s1.LoadString(IDS_STRING293_EN);
				AfxMessageBox(s1);
			}
			else
			{//²ÎÊýÉèÖÃÊ§°Ü
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING250);
				else
					s1.LoadString(IDS_STRING250_EN);
				::AfxMessageBox(s1);
			}
		}
		return;
	}

	CString s1;
	if (m_nLanguage == 0)
		s1.LoadString(IDS_STRING305);
	else
		s1.LoadString(IDS_STRING305_EN);
	::AfxMessageBox(s1);
}
//²ÎÊýµ¼³ö20100413
void CPlotCenterDlg::OnParaOut() 
{
	// TODO: Add your command handler code here
	if (!m_bConnected)
	{
		LinkAndReadPara(true);
		if (!m_bConnected)
			return;
	}
	if (m_bNetworkBusy)
		return;

	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString strFileName;
	FILE *pFile;
	
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strFileName.Format(_T("%s%sPlotPara.ini"),szDrive,szDir);

	pFile = fopen((LPCTSTR)strFileName,"w");
	fprintf(pFile,"LineWidth=%d\n",m_nLineWidth);
	fprintf(pFile,"PrintStyle=%d\n",m_cPrintStyle);
	fprintf(pFile,"SprayCount=%d\n",m_cSprinklerCount);
	fprintf(pFile,"SprayNo=%d\n",m_cSprinklerNo);
	fprintf(pFile,"OverlapError=%f\n",m_dOverlapError);//20100413 ×ßÖ½Îó²î²ÎÊý±¸·Ý
	fprintf(pFile,"ShiftError=%f\n",m_dMalposedError);
	fprintf(pFile,"ShiftError1/2=%d\n",m_sSprayErrorY12);
	fprintf(pFile,"ShiftError2/3=%d\n",m_sSprayErrorY23);
	fprintf(pFile,"ShiftError3/4=%d\n",m_sSprayErrorY34);
	fprintf(pFile,"OverlapError1/2=%d\n",m_sSprayErrorX12);
	fprintf(pFile,"OverlapError2/3=%d\n",m_sSprayErrorX23);
	fprintf(pFile,"OverlapError3/4=%d\n",m_sSprayErrorX34);
	fprintf(pFile,"PlotterXError=%f\n",m_dPlotterXError);
	fprintf(pFile,"PlotterYError=%f\n",m_dPlotterYError);
	
	fclose(pFile);

	CString s1;
	if (m_nLanguage == 0)
		s1.LoadString(IDS_STRING307);
	else
		s1.LoadString(IDS_STRING307_EN);
	::AfxMessageBox(s1);
}

void CPlotCenterDlg::OnRclickPlotitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if (m_wndPlotItem.GetItemCount() == m_listPlotItem.GetCount())
	{
		int nSelCount;
		nSelCount = m_wndPlotItem.GetSelectedCount();

		CPoint oPoint;
		GetCursorPos(&oPoint); 

		CMenu menu,*pSubMenu;
		if (m_nLanguage == 0)
			menu.LoadMenu(IDR_SUBMENU);
		else
			menu.LoadMenu(IDR_SUBMENU_EN);
		pSubMenu=menu.GetSubMenu(0); 
		if (nSelCount > 1)
		{
			pSubMenu->EnableMenuItem(ID_BUTTONPREVIEW2, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_VERIFYFILECOUNT,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_VERIFYFIXEDPOINT,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else if (nSelCount <= 0)
		{
			pSubMenu->EnableMenuItem(ID_BUTTONPREVIEW2, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_VERIFYFILECOUNT,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_VERIFYFIXEDPOINT,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_DELETEFILE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_PAUSEFILE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_RESTARTFILE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}

		CPlotItem *pPlotItem;
		POSITION pos1;
		BOOL bDelete=FALSE;
		BOOL bPause=FALSE;
		BOOL bRestart=FALSE;
	
		if (nSelCount > 0) 
		{
			pos1 = m_wndPlotItem.GetFirstSelectedItemPosition();
			if(pos1) 
			{
				int nSelItem1;
				
				bDelete = true;
				nSelItem1 = m_wndPlotItem.GetNextSelectedItem(pos1);
				pos1 = m_listPlotItem.FindIndex(nSelItem1);
				while (nSelCount > 0)
				{
					if (bPause && bRestart)
						break;
					pPlotItem = (CPlotItem*)m_listPlotItem.GetNext(pos1);
					if ((!bPause) && ((pPlotItem->m_nState != 2) && (pPlotItem->m_nState != 5) && (pPlotItem->m_nState != 6)))
						bPause = true;
					else if ((!bRestart) && ((pPlotItem->m_nState == 2) || (pPlotItem->m_nState == 5) || (pPlotItem->m_nState == 6)))
						bRestart = true;
					nSelCount--;
				}
			}
		}
		if (!bDelete)
		{
			pSubMenu->EnableMenuItem(ID_DELETEFILE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_PAUSEFILE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_RESTARTFILE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else
		{
			pSubMenu->EnableMenuItem(ID_DELETEFILE,MF_BYCOMMAND | MF_ENABLED);
			if (bPause)
				pSubMenu->EnableMenuItem(ID_PAUSEFILE,MF_BYCOMMAND | MF_ENABLED);
			else
				pSubMenu->EnableMenuItem(ID_PAUSEFILE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			if (bRestart)
				pSubMenu->EnableMenuItem(ID_RESTARTFILE,MF_BYCOMMAND | MF_ENABLED);
			else
				pSubMenu->EnableMenuItem(ID_RESTARTFILE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,oPoint.x,oPoint.y,this); 
	}

	*pResult = 0;
}

void CPlotCenterDlg::OnVerifyFileCount() 
{
	// TODO: Add your command handler code here
	CPlotItem *pPlotItem;
	POSITION pos1;
		
	if (m_wndPlotItem.GetItemCount() == m_listPlotItem.GetCount())
	{
		int nSelCount;
		nSelCount = m_wndPlotItem.GetSelectedCount();
		if (nSelCount == 1) 
		{
			pos1 = m_wndPlotItem.GetFirstSelectedItemPosition();
			if(pos1) 
			{
				int nSelItem1;
				
				nSelItem1 = m_wndPlotItem.GetNextSelectedItem(pos1);
				pos1 = m_listPlotItem.FindIndex(nSelItem1);
				pPlotItem = (CPlotItem*)m_listPlotItem.GetNext(pos1);
				if (pPlotItem->m_nState == 0)
				{
					CString s1;
					if (m_nLanguage == 0)
						s1.LoadString(IDS_STRING309);
					else
						s1.LoadString(IDS_STRING309_EN);
					AfxMessageBox(s1);
				}
				else
				{
					CVerifyCopies VerifyCopiesDialog;
					VerifyCopiesDialog.m_nDialogFlag = 1;
					VerifyCopiesDialog.m_lWriteValue = pPlotItem->m_nCopyNum;
					VerifyCopiesDialog.m_strFileName = pPlotItem->m_strPlotName;
					if (VerifyCopiesDialog.DoModal() == IDOK)
					{
						if (VerifyCopiesDialog.m_lWriteValue != pPlotItem->m_nCopyNum)
						{
							pPlotItem->m_nCopyNum = VerifyCopiesDialog.m_lWriteValue;
							FillListCtrl();
						}
					}
				}
			}
		}
	}
}

//20110402 ´òÓ¡ÎÄ¼þÔ¤ÀÀ
void CPlotCenterDlg::OnPreview()
{
	// TODO: Add your command handler code here
	CPlotItem *pPlotItem;
	POSITION pos1;
	display=0;
	displayx1=0;
	displayx2=0;
		
	if (m_wndPlotItem.GetItemCount() == m_listPlotItem.GetCount())
	{
		int nSelCount;
		nSelCount = m_wndPlotItem.GetSelectedCount();
		if (nSelCount == 1) 
		{
			pos1 = m_wndPlotItem.GetFirstSelectedItemPosition();
			if(pos1) 
			{
				int nSelItem1;
				
				nSelItem1 = m_wndPlotItem.GetNextSelectedItem(pos1);
				pos1 = m_listPlotItem.FindIndex(nSelItem1);
				pPlotItem = (CPlotItem*)m_listPlotItem.GetNext(pos1);
				if (pPlotItem->m_nState == 0 || pPlotItem->m_nState == 3 || pPlotItem->m_nState == 4)//0¡¢3¡¢4´òÓ¡ÖÐ£¬1µÈ´ý£¬2¡¢5¡¢6ÔÝÍ£
				{
					/*
					CString s1;
					if (m_nLanguage == 0)
						s1.LoadString(IDS_STRING309);
					else
						s1.LoadString(IDS_STRING309_EN);
					AfxMessageBox(s1);
					*/
					display=1;
					int nReturnValue=OpenFile(pPlotItem->m_strPlotName);
					if(nReturnValue==0)
					{
						CPictureDlg dlgPicture;
						long lPaperWidth = (long)(theApp.m_dPaperWidth * 40);
						if (m_lMaxPlotWidth > lPaperWidth)
						{
							dlgPicture.m_lFileLength = m_lMaxPlotLength;
							dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
							dlgPicture.m_lFileMinLength = m_lMinPlotLength;
							dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
							dlgPicture.m_lTotalX = (m_lMaxPlotLength + 10 * 40) * long((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth) - 10 * 40;
							dlgPicture.m_lTotalY = lPaperWidth;
						}
						else
						{
							dlgPicture.m_lFileLength = m_lMaxPlotLength;
							dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
							dlgPicture.m_lFileMinLength = m_lMinPlotLength;
							dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
							dlgPicture.m_lTotalX = m_lMaxPlotLength;
							dlgPicture.m_lTotalY = m_lMaxPlotWidth;
						}
						dlgPicture.m_strPictureName = pPlotItem->m_strPlotName;
						dlgPicture.m_lFixedPointLength = pPlotItem->m_lFixedPointLength;
						dlgPicture.m_nCopyNum = pPlotItem->m_nCopyNum;
						dlgPicture.m_busyPrint = m_bNetworkBusy;
						dlgPicture.DoModal();
					}
				}


				else if (pPlotItem->m_nState == 2 || pPlotItem->m_nState == 5 || pPlotItem->m_nState == 6)//0¡¢3¡¢4´òÓ¡ÖÐ£¬1µÈ´ý£¬2¡¢5¡¢6ÔÝÍ£
				{

					display=2;
					int nReturnValue=OpenFile(pPlotItem->m_strPlotName);
					if(nReturnValue==0)
					{
						CPictureDlg dlgPicture;
						long lPaperWidth = (long)(theApp.m_dPaperWidth * 40);
						if (m_lMaxPlotWidth > lPaperWidth)
						{
							dlgPicture.m_lFileLength = m_lMaxPlotLength;
							dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
							dlgPicture.m_lFileMinLength = m_lMinPlotLength;
							dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
							dlgPicture.m_lTotalX = (m_lMaxPlotLength + 10 * 40) * long((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth) - 10 * 40;
							dlgPicture.m_lTotalY = lPaperWidth;
						}
						else
						{
							dlgPicture.m_lFileLength = m_lMaxPlotLength;
							dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
							dlgPicture.m_lFileMinLength = m_lMinPlotLength;
							dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
							dlgPicture.m_lTotalX = m_lMaxPlotLength;
							dlgPicture.m_lTotalY = m_lMaxPlotWidth;
						}
						dlgPicture.m_strPictureName = pPlotItem->m_strPlotName;
						dlgPicture.m_lFixedPointLength = pPlotItem->m_lFixedPointLength;
						dlgPicture.m_nCopyNum = pPlotItem->m_nCopyNum;
						dlgPicture.m_busyPrint = m_bNetworkBusy;
						dlgPicture.DoModal();
					}
				}


				else if(pPlotItem->m_nState == 1)//µÈ´ý
				{
					display=3;
					int nReturnValue=OpenFile(pPlotItem->m_strPlotName);
					if(nReturnValue==0)
					{
						CPictureDlg dlgPicture;
						long lPaperWidth = (long)(theApp.m_dPaperWidth * 40);
						if (m_lMaxPlotWidth > lPaperWidth)
						{
							dlgPicture.m_lFileLength = m_lMaxPlotLength;
							dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
							dlgPicture.m_lFileMinLength = m_lMinPlotLength;
							dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
							dlgPicture.m_lTotalX = (m_lMaxPlotLength + 10 * 40) * long((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth) - 10 * 40;
							dlgPicture.m_lTotalY = lPaperWidth;
						}
						else
						{
							dlgPicture.m_lFileLength = m_lMaxPlotLength;
							dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
							dlgPicture.m_lFileMinLength = m_lMinPlotLength;
							dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
							dlgPicture.m_lTotalX = m_lMaxPlotLength;
							dlgPicture.m_lTotalY = m_lMaxPlotWidth;
						}
						dlgPicture.m_strPictureName = pPlotItem->m_strPlotName;
						dlgPicture.m_lFixedPointLength = pPlotItem->m_lFixedPointLength;
						dlgPicture.m_nCopyNum = pPlotItem->m_nCopyNum;
						dlgPicture.m_busyPrint = m_bNetworkBusy;
						if(dlgPicture.DoModal()==IDOK)
						{
							if (dlgPicture.m_lFixedPointLength != pPlotItem->m_lFixedPointLength)
							{
								if ( dlgPicture.m_lFixedPointLength > (dlgPicture.m_lTotalX*0.025) )
								{
									CString s1;
									if (m_nLanguage == 0)
										s1.LoadString(IDS_STRING299);
									else
										s1.LoadString(IDS_STRING299_EN);
									AfxMessageBox(s1);
									return;
								}
								pPlotItem->m_lFixedPointLength = dlgPicture.m_lFixedPointLength;			
							}
							
							if (dlgPicture.m_nCopyNum != pPlotItem->m_nCopyNum)
							{
								if ( (dlgPicture.m_nCopyNum > 255) || (dlgPicture.m_nCopyNum == 0) )
								{
									CString s1;
									if (m_nLanguage == 0)
										s1.LoadString(IDS_STRING311);
									else
										s1.LoadString(IDS_STRING311_EN);
									AfxMessageBox(s1);
									return;
								}
								pPlotItem->m_nCopyNum = dlgPicture.m_nCopyNum;
							}
							FillListCtrl();
						}
					}
				}
			}
     	}
	}
}

void CPlotCenterDlg::OnVerifyFixedPoint() 
{
	// TODO: Add your command handler code here
	CPlotItem *pPlotItem;
	POSITION pos1;
		
	if (m_wndPlotItem.GetItemCount() == m_listPlotItem.GetCount())
	{
		int nSelCount;
		nSelCount = m_wndPlotItem.GetSelectedCount();
		if (nSelCount == 1) 
		{
			pos1 = m_wndPlotItem.GetFirstSelectedItemPosition();
			if(pos1) 
			{
				int nSelItem1;
				
				nSelItem1 = m_wndPlotItem.GetNextSelectedItem(pos1);
				pos1 = m_listPlotItem.FindIndex(nSelItem1);
				pPlotItem = (CPlotItem*)m_listPlotItem.GetNext(pos1);
				if (pPlotItem->m_nState == 0)
				{
					CString s1;
					if (m_nLanguage == 0)
						s1.LoadString(IDS_STRING309);
					else
						s1.LoadString(IDS_STRING309_EN);
					AfxMessageBox(s1);
				}
				else
				{
					CVerifyCopies VerifyCopiesDialog;
					VerifyCopiesDialog.m_nDialogFlag = 2;
					VerifyCopiesDialog.m_lWriteValue = pPlotItem->m_lFixedPointLength;
					VerifyCopiesDialog.m_strFileName = pPlotItem->m_strPlotName;
					VerifyCopiesDialog.m_lPictureLength = pPlotItem->m_lPictureLength;
					if (VerifyCopiesDialog.DoModal() == IDOK)
					{
						if (VerifyCopiesDialog.m_lWriteValue != pPlotItem->m_lFixedPointLength)
						{
							pPlotItem->m_lFixedPointLength = VerifyCopiesDialog.m_lWriteValue;
							FillListCtrl();
						}
					}
				}
			}
		}
	}
}

void CPlotCenterDlg::OnDeleteFile() 
{
	// TODO: Add your command handler code here
	OnDelete();
}

void CPlotCenterDlg::OnPauseFile() 
{
	// TODO: Add your command handler code here
	OnPause();
}

//¹¤¾ßÀ¸ÌáÊ¾
BOOL CPlotCenterDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	
	// UNICODEÏûÏ¢
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	//TCHAR szFullText[512];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;
	
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFromÎª¹¤¾ßÌõµÄHWND 
		nID = ::GetDlgCtrlID((HWND)nID);
	}
	
	if (nID != 0) //²»Îª·Ö¸ô·û
	{
		if (m_nLanguage == 1)
			nID++;
		strTipText.LoadString(nID);
		strTipText = strTipText.Mid(strTipText.Find('\n',0)+1);
		
#ifndef _UNICODE
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else
		{
			_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#else
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			_wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
		}
		else
		{
			lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#endif
		
		*pResult = 0;
		
		// Ê¹¹¤¾ßÌõÌáÊ¾´°¿ÚÔÚ×îÉÏÃæ
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,SWP_NOACTIVATE|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER); 
		return TRUE;
	}
	return TRUE;
}


void CPlotCenterDlg::OnStartOrStop() 
{
	// TODO: Add your control notification handler code here
	if (m_nStartPrint == 1)
	{
		m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,false);
		m_nStartPrint = 2;
	}
}

void CPlotCenterDlg::OnRestartFile() 
{
	// TODO: Add your command handler code here
	OnRestart();
}

void CPlotCenterDlg::OnDeleteFile2() 
{
	// TODO: Add your command handler code here
	OnDelete();
}

void CPlotCenterDlg::OnPauseFile2() 
{
	// TODO: Add your command handler code here
	OnPause();
}

void CPlotCenterDlg::OnRestartFile2() 
{
	// TODO: Add your command handler code here
	OnRestart();
}

void CPlotCenterDlg::OnOpenFile() 
{
	// TODO: Add your command handler code here
	OnOpen();
}

void CPlotCenterDlg::OnPlotcenterHelp2() 
{
	// TODO: Add your command handler code here
	OnPlotCenterHelp();
}

void CPlotCenterDlg::OnUpFile() 
{
	// TODO: Add your command handler code here
	OnUp();
}

void CPlotCenterDlg::OnDownFile() 
{
	// TODO: Add your command handler code here
	OnDown();
}

void CPlotCenterDlg::OnPreviewFile() 
{
	// TODO: Add your command handler code here

	OnPreview();
}

void CPlotCenterDlg::OnSortByMtime()
{
	if (m_bAutoSort==TRUE)
	{
		m_bSortByMTime=TRUE;
		m_bSortByTitle=FALSE;
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SORT_BY_MTIME,MF_BYCOMMAND|MF_CHECKED);
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SORT_BY_TITLE,MF_BYCOMMAND|MF_UNCHECKED);
	}
	else
	{
		SortByMtime();
	}

	if(FALSE == m_bAutoSort) theApp.m_bAutoSort=0;
	else theApp.m_bAutoSort=1;
	
	if(FALSE == m_bSortByMTime) theApp.m_bSortByMtime=0;
	else theApp.m_bSortByMtime=1;
	
	if(FALSE == m_bSortByTitle) theApp.m_bSortByTitle=0;
	else theApp.m_bSortByTitle=1;

	FillListCtrl();
}

void CPlotCenterDlg::OnSortByTitle()
{
	if (m_bAutoSort==TRUE)
	{
		m_bSortByMTime=FALSE;
		m_bSortByTitle=TRUE;
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SORT_BY_TITLE,MF_BYCOMMAND|MF_CHECKED);
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SORT_BY_MTIME,MF_BYCOMMAND|MF_UNCHECKED);
	}
	else
	{
		SortByTitle();
	}

	if(FALSE == m_bAutoSort) theApp.m_bAutoSort=0;
	else theApp.m_bAutoSort=1;
	
	if(FALSE == m_bSortByMTime) theApp.m_bSortByMtime=0;
	else theApp.m_bSortByMtime=1;
	
	if(FALSE == m_bSortByTitle) theApp.m_bSortByTitle=0;
	else theApp.m_bSortByTitle=1;

	FillListCtrl();
}

void CPlotCenterDlg::OnAutoSort() 
{
	if (m_bAutoSort==FALSE)
	{
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_AUTO_SORT,MF_BYCOMMAND|MF_CHECKED);
		m_bAutoSort=TRUE;
		if (m_bSortByMTime==m_bSortByTitle)
		{
			GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SORT_BY_MTIME,MF_BYCOMMAND|MF_CHECKED);
			m_bSortByMTime=TRUE;
			m_bSortByTitle=FALSE;
		}
	}
	else
	{
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_AUTO_SORT,MF_BYCOMMAND|MF_UNCHECKED);
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SORT_BY_MTIME,MF_BYCOMMAND|MF_UNCHECKED);
		GetMenu()->GetSubMenu(2)->CheckMenuItem(IDM_SORT_BY_TITLE,MF_BYCOMMAND|MF_UNCHECKED);
		m_bAutoSort=FALSE;
		m_bSortByMTime=FALSE;
		m_bSortByTitle=FALSE;
	}

	if(m_bAutoSort==TRUE)
	{
		if (m_bSortByMTime)
		{
			SortByMtime();
		}
		else
		{
			SortByTitle();
		}
	}

	if(FALSE == m_bAutoSort) theApp.m_bAutoSort=0;
	else theApp.m_bAutoSort=1;
	
	if(FALSE == m_bSortByMTime) theApp.m_bSortByMtime=0;
	else theApp.m_bSortByMtime=1;
	
	if(FALSE == m_bSortByTitle) theApp.m_bSortByTitle=0;
	else theApp.m_bSortByTitle=1;

	FillListCtrl();
}

void CPlotCenterDlg::SortByMtime()
{
	CString tempbuff;
	CPlotItem *pPlotItem1;
	CPlotItem *pPlotItem2;
	POSITION pos1,pos2;

	int nItemCount=0;
	int i,j;
	int iSortStart=0;

	nItemCount = m_listPlotItem.GetCount();
	if (nItemCount < 2 )
		return;

	pos1=m_listPlotItem.FindIndex(0);
	pPlotItem1 = (CPlotItem*)m_listPlotItem.GetAt(pos1);

	if( 1 == pPlotItem1->m_nState ) //×´Ì¬ÎªµÈ´ý
		iSortStart=0;
	else
		iSortStart=1;

	if (nItemCount < (2+iSortStart) )
		return;

	for(i=iSortStart;i<nItemCount-1;i++) //ÓÃÃ°ÅÝÅÅÐòÁ´±í m_listPlotItem
		for(j=iSortStart;j<nItemCount-1;j++)
		{
			pos1=m_listPlotItem.FindIndex(j);
			pos2=m_listPlotItem.FindIndex(j+1);
			
			pPlotItem1 = (CPlotItem*)m_listPlotItem.GetAt(pos1);
			pPlotItem2 = (CPlotItem*)m_listPlotItem.GetAt(pos2);

			if (pPlotItem1->m_ctimeMtime > pPlotItem2->m_ctimeMtime)
			{
				m_listPlotItem.InsertAfter(pos2,pPlotItem1);
				m_listPlotItem.InsertAfter(pos2,pPlotItem2);
				m_listPlotItem.RemoveAt(pos2);
				m_listPlotItem.RemoveAt(pos1);
			}
		}
}

void CPlotCenterDlg::SortByTitle()
{
	CString tempbuff;
	CPlotItem *pPlotItem1;
	CPlotItem *pPlotItem2;
	
	POSITION pos1,pos2;
	int nItemCount=0;
	int i,j;
	int iSortStart=0;
	
	nItemCount = m_listPlotItem.GetCount();
	if (nItemCount < 2 )
		return;

	pos1=m_listPlotItem.FindIndex(0);
	pPlotItem1 = (CPlotItem*)m_listPlotItem.GetAt(pos1);
	
	if( 1 == pPlotItem1->m_nState ) //×´Ì¬ÎªµÈ´ý
		iSortStart=0;
	else
		iSortStart=1;

	if (nItemCount < (2+iSortStart) )
		return;
	
	for(i=iSortStart;i<nItemCount-1;i++) //ÓÃÃ°ÅÝÅÅÐòÁ´±í m_listPlotItem
		for(j=iSortStart;j<nItemCount-1;j++)
		{
			pos1=m_listPlotItem.FindIndex(j);
			pos2=m_listPlotItem.FindIndex(j+1);
			
			pPlotItem1 = (CPlotItem*)m_listPlotItem.GetAt(pos1);
			pPlotItem2 = (CPlotItem*)m_listPlotItem.GetAt(pos2);
			
			if( strcmp(pPlotItem1->m_strPlotName,pPlotItem2->m_strPlotName) > 0 )
			{
				m_listPlotItem.InsertAfter(pos2,pPlotItem1);
				m_listPlotItem.InsertAfter(pos2,pPlotItem2);
				m_listPlotItem.RemoveAt(pos2);
				m_listPlotItem.RemoveAt(pos1);
			}
		}
}


void CPlotCenterDlg::SavedToHistoryFolder()
{
	
	/* ×Ô¶¯±£´æµ½¹¤×÷Ä¿Â¼ÏÂµÄhisteryÎÄ¼þ¼ÐÖÐ*/
	//20150316rwt****start****//
	SYSTEMTIME st;
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szFileName[_MAX_FNAME];//20150316rwt Truncate filename
	TCHAR szExt[_MAX_EXT];
	CString sWorkPath1;//20150316rwt makedir history
	CString sWorkPath2;//20150318rwt makedir history


	CPlotItem *pPlotItem;
	GetLocalTime(&st);
	pPlotItem = (CPlotItem*)m_listPlotItem.GetHead();
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	sWorkPath1.Format(_T("%s%s888"), szDrive, szDir);/*folder_name:888 && historyfiles*/
	CFileFind finder;
	BOOL bWorking = finder.FindFile(sWorkPath1);
	if ((bWorking != 0)/*||(finder.IsDirectory() == false)*/)
	{
		/* assertion 20150316rwt
		if (finder.IsDirectory() == false)
		{
			AfxMessageBox("Cant Find directory Error'historyfiles'\n");
		}
		*/
	}
	else
	{
		if (_mkdir(sWorkPath1) != 0)
		{
			AfxMessageBox("Creating directory Error'historyfiles'\n");
		}
	
	}
	
	int i_strlength= pPlotItem->m_strPlotName.GetLength() + 1;
	TCHAR *c_strFile = new TCHAR[i_strlength+10];
	strncpy(c_strFile, pPlotItem->m_strPlotName, i_strlength);
	_splitpath(c_strFile,szDrive,szDir,szFileName,szExt);
	sWorkPath2.Format(_T("%s\\%d%d%d"), sWorkPath1, st.wYear, st.wMonth, st.wDay); 
	bWorking = finder.FindFile(sWorkPath2);
	if ((bWorking != 0))
	{
		/* assertion 20150316rwt
		if (finder.IsDirectory() == false)
		{
			AfxMessageBox("Cant Find directory Error'historyfiles'\n");
		}
		*/
	}
	else
	{
		if (_mkdir(sWorkPath2) != 0)
		{
			AfxMessageBox("Creating directory Error'historyfiles'\n");
		}
	
	}
	sWorkPath1.Format(_T("%s\\%s"), sWorkPath2, szFileName);
	sWorkPath2.Format(_T("%s%s"), sWorkPath1, szExt);
	if (CopyFile(pPlotItem->m_strPlotName, sWorkPath2,false) == 0)
	{
		//AfxMessageBox("Copy files Error\n");
		
		/*20150316rwt  printf error*/
		///*
		LPVOID lpMsgBuf; 
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			GetLastError(), 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf, 
			0, 
			NULL 
			); 
		MessageBox(  (LPCTSTR)lpMsgBuf); 
		LocalFree( lpMsgBuf );
		//*/
		/*20150316rwt  printf error*/
	}
	/* ×Ô¶¯±£´æµ½¹¤×÷Ä¿Â¼ÏÂµÄhisteryÎÄ¼þ¼ÐÖÐ*///20150316rwt**** end ****¡
}

void CPlotCenterDlg::DeleteHistoryFilesOnTime()
{
	CFileFind filefindDir;
	CFileFind filefind;
	CTime file_createtime;  //±£´æÎÄ¼þ´´½¨Ê±¼ä
	CTime m_FCrTime;
	CTime m_Seltime;	
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];

	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	m_Seltime = CTime::GetCurrentTime();
	m_FCrTime = m_Seltime - CTimeSpan(60/*m_Days*/,0,0,0);//É¾³ý´ËÊ±¼äÇ°µÄÎÄ¼þ
	CString SzDir; //ÎÄ¼þÄ¿Â¼
	SzDir.Format(_T("%s%s"), szDrive, szDir);
	CString delfiledirpath;
	CString delfilepath;
    UpdateData(FALSE);
	if (SzDir.Right(1)!="\\")
		SzDir+="\\";

	SzDir += "historyfiles\\";
	SzDir += "*.*";
	BOOL bRes=filefindDir.FindFile(SzDir);
	while(bRes)
	{
		bRes=filefindDir.FindNextFile();
		if(filefindDir.IsDirectory()&&!filefindDir.IsDots())
		{	
			file_createtime = 0;
			filefindDir.GetCreationTime(file_createtime); //»ñÈ¡ÎÄ¼þ´´½¨Ê±¼ä
			if (file_createtime < m_FCrTime) //ÓÃCTimeÀàÖØÔØµÄ±È½Ï·½·¨
			{
				delfiledirpath = "";
				delfiledirpath = filefindDir.GetFilePath();	
				SzDir = delfiledirpath;
				if (delfiledirpath.Right(1)!="\\")
				{
					delfiledirpath+="\\";
				}
				delfilepath = delfiledirpath + "*.*";

				BOOL bResHistoryDate=filefind.FindFile(delfilepath);
				while (bResHistoryDate)
				{
					bResHistoryDate=filefind.FindNextFile();
					if (!filefind.IsDots())
					{
						delfilepath = "";
						delfilepath = filefind.GetFilePath();
						DeleteFile(delfilepath);
					}

				}
				//MessageBox(delfilepath.GetBuffer(delfilepath.GetLength()+1));//²âÊÔÊ±Ê¹ÓÃ
				//delfiledirpath -= "*.*";
				//DeleteFile(SzDir);
				_rmdir(SzDir);
				
			}
        }
	}
	filefind.Close();	
}

UINT  CPlotCenterDlg:: PlotProgressThreadProc(void)
{
	
	long lTotalPage = (g_lMaxPlotLength + SECTION_WIDTH - 1) / SECTION_WIDTH;
	int time =(int) (g_lMaxPlotWidth * lTotalPage / 0.5 / 2)/*ÅçÍ·Êý*/;
         CPlotItem *pPlotItem;
	POSITION pos1;
	pos1=m_listPlotItem.GetHeadPosition();
	pPlotItem=(CPlotItem*)m_listPlotItem.GetNext(pos1);

	
	//m_pBmpGenerationThread=AfxBeginThread(BmpGenerationThreadProc,&m_PlotThreadInfo);
	//m_pSendDataThread=AfxBeginThread(UsbSendDataThreadProc,&m_PlotThreadInfo);

	int i = 0;
	while (1)
	{
		
		if (i<= time)
		{
			i++;
			pPlotItem->m_iPlotpercent =(int)( i/time * 100);
			FillListCtrl();
		}
		else
		{
			
			break;
		}
		Sleep(1000);
	}
	
	return 0;
}

//20150728
void CPlotCenterDlg::OnSetIU() 
{
	// TODO: Add your command handler code here
	//if ((m_nCommunicateMode != 0) && (m_bConnected == false))
	{
		
		theApp.m_nMeasureUnit = 0;
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_MM,MF_CHECKED);
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_INCH,MF_UNCHECKED);
		ChangeLanguage();
	}
}

void CPlotCenterDlg::OnSetInch() 
{
	// TODO: Add your command handler code here
	//if ((m_nCommunicateMode != 0) && (m_bConnected == false))
	{
		//m_nCommunicateMode = 0;
		theApp.m_nMeasureUnit = 1;
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_MM,MF_UNCHECKED);
		GetMenu()->GetSubMenu(3)->CheckMenuItem(ID_INCH,MF_CHECKED);
		ChangeLanguage();
	}
}


void CPlotCenterDlg::OnOutofmemoryPrintstate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

