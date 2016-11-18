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

#include "Sensapi.h"
#pragma comment(lib, "Sensapi.lib")
#include <IO.H>
#include <DIRECT.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ��ʾ�ɸı��С��ͼ��ID
#ifndef OBM_SIZE
#define OBM_SIZE 32766
#endif

#define TIME_OUT_TIME 2
#define ID_TIMER  1  //��ʱ��
#define T_TIMES   1000  //��ʱ,unit:ms //20150515-rwt
#define MMPixelX		11.811023622
//#define USB_PID_VID 0x4D495250
#define USB_PID_VID_0  0x55374348	//Ϊ����WIN7 64λ��ʹ��USB����ǩ��������ʹ��Ĭ�ϳ���ID //
#define USB_PID_VID_1  0x4D495250	//���ֻ�ͼ���ļ����� 20111121
BOOL bIsEncrypt;

extern CStringArray m_NeedCutDataFiles;//gzr20161029�洢�轫�и����ݸ��ƴ�ӡ������
extern double gl_dPaperLength;
extern double g_dScaleX;
extern double g_dScaleY;
extern int g_nVersion;
extern int g_nLineWidth;
extern unsigned char g_cPenNo; // 20101206 �ʺŵ���
extern unsigned char g_cCutNo;
extern unsigned char g_cTerminator;//��־�ս��20150212rwt

extern int CreateList(CPlotItem *pPlotItem,int nCommunicateMode,SOCKET sock);
extern int JudgeFileNeedCutData(CString strFilePathC,int nCommunicateMode,SOCKET sock);//gzr20161029
extern void RemoveGLBuffer1(void);
int g_nNewVersion;//�����¾ɰ汾���������¼���20100413

int breakLinkTimes;//�Ͽ�״̬��timerִ�д��� gzr20160906

UINT BmpGenerationThreadProc(LPVOID pParam);
UINT SendDataThreadProc(LPVOID pParam);
UINT UsbSendDataThreadProc(LPVOID pParam);
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
	m_dGapRight=0;
	m_lMarkSpace=long(200*MMPixelX);
	m_nRotate=0;
	m_lMaxPlotLength=0;
	m_lMaxPlotWidth=0;
	m_lMinPlotLength=0;
	m_lMinPlotWidth=0;
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
	m_lPaperWidth=lPaperWidth;
	m_dGapRight=theApp.m_dGapRight;
	m_lMarkSpace=long(theApp.m_dMarkSpace*MMPixelX);
	m_nRotate=theApp.m_nRotate;
	m_lMaxPlotLength=lMaxPlotLength;
	m_lMaxPlotWidth=lMaxPlotWidth;
	m_lMinPlotLength=lMinPlotLength;
	m_lMinPlotWidth=lMinPlotWidth;
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

/////////////////////////////////////////////////////////////////////////////////////
//gzr20161029 ɾ���и�ת��ӡ��Ϣ
void DeleteCutInfo(CString strFilePath)
{
	int needCutSize = m_NeedCutDataFiles.GetSize();
	for (int i = 0; i < needCutSize; )
	{
		if (m_NeedCutDataFiles[i].Find(strFilePath))
		{
			m_NeedCutDataFiles[i].Replace(strFilePath + "|","");
			if (m_NeedCutDataFiles[i].GetLength() == 0)
			{
				m_NeedCutDataFiles.RemoveAt(i);
				continue;
			}
		}
		i++;
	}
}
//gzr20161024 д��־����
void WriteLogTest(CString content)
{
	FILE *fp = fopen("C:\\LogTest.txt", "a+");
	if (fp==0) 
	{ 
		printf("can't open file\n"); 
		return;
	}
	CTime t=CTime::GetCurrentTime();
	CString strC;
	
	strC.Format("\n%d-%d-%d %d:%d:%d :%s",t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond(),content);
	
	fseek(fp, 0, SEEK_END);
	char sz_add[1024];
	strcpy(sz_add, strC.GetBuffer(strC.GetLength()));
	fwrite(sz_add, strlen(sz_add), 1, fp);
	fclose(fp);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

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
	ON_COMMAND(ID_OPEN1, OnOpen1)
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
	ON_COMMAND(ID_OPENFILE1, OnOpenFile1)
	ON_COMMAND(ID_PLOTCENTERHELP2, OnPlotcenterHelp2)
	ON_COMMAND(ID_UPFILE, OnUpFile)
	ON_COMMAND(ID_DOWNFILE, OnDownFile)
	ON_COMMAND(ID_BUTTONPREVIEW2, OnPreviewFile) //20110402 ��ӡԤ��
	ON_COMMAND(ID_BUTTONPREVIEW, OnPreviewFile) //20110402 ��ӡԤ��
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
	
	// ���öԻ���Ϊ�ɱ��С��
	ModifyStyle(0,WS_SIZEBOX);
	
	// �ԶԻ���ĳ�ʼ��С��Ϊ�Ի���Ŀ�Ⱥ͸߶ȵ���Сֵ
	CRect rectDlg;
	GetWindowRect(rectDlg);
	m_iMinWidth = rectDlg.Width();
	m_iMinHeight = rectDlg.Height();

	// �õ��Ի���client����Ĵ�С 
	CRect rectClient;
	GetClientRect(&rectClient);
	m_iClientWidth = rectClient.Width();
	m_iClientHeight = rectClient.Height();

	// Loadͼ��
	m_bmpSizeIcon.LoadOEMBitmap(OBM_SIZE);
	m_bmpSizeIcon.GetBitmap(&m_bitmap);

	// ������ʾͼ��ľ�̬�ؼ������ڶԻ������½�
	m_wndSizeIcon.Create(NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, CRect(0, 0, m_bitmap.bmWidth, m_bitmap.bmHeight), this, 0);
	m_wndSizeIcon.SetBitmap(m_bmpSizeIcon);
	m_wndSizeIcon.MoveWindow(m_iClientWidth - m_bitmap.bmWidth, m_iClientHeight - m_bitmap.bmHeight, m_bitmap.bmWidth, m_bitmap.bmHeight);

	// ��ʾͼ��
	m_wndSizeIcon.ShowWindow(m_bShowSizeIcon);

	// �ؼ���Ϣ����
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

	// ���ÿؼ���Ϣ
	SetControlProperty(dcMenuGroup, sizeof(dcMenuGroup)/sizeof(DLGCTLINFO));

	// TODO: Add extra initialization here
	this->m_nLanguage = theApp.m_nLanguage;
	this->m_nCommunicateMode = theApp.m_nCommunicateMode;
	m_pSendDataThread=NULL;
	m_pBmpGenerationThread=NULL;
	m_bCutFile = true;
	m_bTimerBusy = true;
	m_bOpenFile = false;
	m_bConnected = false;
	m_bNetworkBusy = false;
	m_bWaitConnected = true;
	m_nWaitTime = 0;
	m_cPrintStyle = 1;
	m_nLineWidth = 2;
	m_cSprinklerCount = 1;
	m_cSprinklerNo = 0;
	m_dOverlapError = 0.0; //20100413 ��ֽ����ʼ��
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
	m_sPlotType = 1630;
	m_nStartPrint = 0;
	m_cPenNo = 1;	//20101206 �뵶��
	m_cCutNo = 4;

	m_wndPrintState.SetRange(0,100);
	m_englistmenu.LoadMenu(IDR_MAINMENU_EN);
	HMENU m_hMenu=::GetMenu(this->m_hWnd); // �������Ĳ˵�
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

	SetTimer(ID_TIMER,T_TIMES,NULL);

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


//20110402 ��ӡ�ļ�Ԥ��
void CPlotCenterDlg::OnPreview()
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
					int nReturnValue=OpenFile(pPlotItem->m_strPlotName);
					if(nReturnValue==0)
					{
						CPictureDlg dlgPicture;
						long lPaperWidth = (long)(theApp.m_dPaperWidth * 40);
						if (m_lMaxPlotWidth > lPaperWidth)
						{
							dlgPicture.m_lFileLength = m_lMaxPlotLength;
							dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
							//dlgPicture.m_lFileMinLength = m_lMinPlotLength;
							//dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
							if(m_lMinPlotLength < 0)
								m_lMinPlotLength = 0;
							dlgPicture.m_lFileMinLength = m_lMinPlotLength;
							
							if(m_lMinPlotWidth < 0)
								m_lMinPlotWidth = 0;
							dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;

							dlgPicture.m_lTotalX = (m_lMaxPlotLength + 10 * 40) * long((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth) - 10 * 40;
							dlgPicture.m_lTotalY = lPaperWidth;
						}
						else
						{
							dlgPicture.m_lFileLength = m_lMaxPlotLength;
							dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
							//dlgPicture.m_lFileMinLength = m_lMinPlotLength;
							//dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
							if(m_lMinPlotLength < 0)
								m_lMinPlotLength = 0;
							dlgPicture.m_lFileMinLength = m_lMinPlotLength;
							
							if(m_lMinPlotWidth < 0)
								m_lMinPlotWidth = 0;
							dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;

							dlgPicture.m_lTotalX = m_lMaxPlotLength;
							dlgPicture.m_lTotalY = m_lMaxPlotWidth;
						}
						dlgPicture.m_strPictureName = pPlotItem->m_strPlotName;
						dlgPicture.m_lFixedPointLength = pPlotItem->m_lFixedPointLength;
						dlgPicture.m_nCopyNum = pPlotItem->m_nCopyNum;
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

void CPlotCenterDlg::OnPreviewFile() 
{
	// TODO: Add your command handler code here
	OnPreview();
}


/*
						if(cNum != 0)
						{
							char cisbig = 0;
							iFCur = lWrStarPost;
							cNewNum = cNum;
							lWrStarPost = 0;
							while(bufSource[iFCur] != 0x0a)
							{
								cOdd = 1;
								//lWrStarPost = 0;
								if(!IsDigit(bufSource[iFCur]))
								{
									if((bufSource[iFCur] == 'L') && (bufSource[iFCur+1] == 'B'))
									{
										while(bufSource[iFCur] != 0x03)
											cNewBuff[lWrStarPost++] = bufSource[iFCur++]; 
										cNewBuff[lWrStarPost++] = bufSource[iFCur++];

										if(bufSource[iFCur] == 0x0d)
											cNewBuff[lWrStarPost++] = bufSource[iFCur++];
										if(bufSource[iFCur] == 0x0a)
										{
											cNewBuff[lWrStarPost++] = bufSource[iFCur++];
											if(cNewNum != cNum)
												cNum = cNewNum;
											ptrCF[cNum].Write(cNewBuff, lWrStarPost);
											break;
										}

									//	ptrCF[cNum].Write(cNewBuff, lWrStarPost);
										continue;
									}
									else
									{
										cNewBuff[lWrStarPost++] = bufSource[iFCur++]; 
										cNewBuff[lWrStarPost++] = bufSource[iFCur++];
										if(bufSource[iFCur] == ';')
										{
											cNewBuff[lWrStarPost++] = bufSource[iFCur++];
											if(bufSource[iFCur] == 0x0d)
												cNewBuff[lWrStarPost++] = bufSource[iFCur++];
											if(bufSource[iFCur] == 0x0a)
											{
												cNewBuff[lWrStarPost++] = bufSource[iFCur++];
												if(cNewNum != cNum)
													cNum = cNewNum;
												ptrCF[cNum].Write(cNewBuff, lWrStarPost);
												break;
											}
											ptrCF[cNum].Write(cNewBuff, lWrStarPost);
											continue;
										}
									}
								}
								
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
										if(lGetXlen/lToalSecHPGLLen > cNum)
										{
											cNum = lGetXlen/lToalSecHPGLLen;
										}

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
										{
											iFCur++;
											break;
										}
									}
									iFCur++;
									cOdd++;
								}
								//ptrCF[cNum].Write(cNewBuff, lWrStarPost);
							}
						}*/


// 2: ET ����ָ�ҳ�� 1:������������PG  0��û��PG
int CPlotCenterDlg::QuickScanFilePG(CString strFileName)
{
	CFile file;
	long nLength, iReadCout;
	long iHavePG;
	char cReadChr[10];//for ET	//cReadChr[2] for RP
	
	iHavePG=0;
	if(file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
	{
		nLength=file.GetLength();

		iReadCout = 0;
		while(iReadCout < nLength)
		{
			file.Read(&cReadChr[0], 1); 
			iReadCout++;
			
			//split the output file for RP
			if(cReadChr[0] == 'P')
			{
				file.Read(&cReadChr[1], 1);
				iReadCout++;
				if(cReadChr[1] == 'G')
				{
					iHavePG++;
					/*
					file.Read(&cReadChr[2], 1);
					iReadCout++;
					if (isdigit(cReadChr[2])&&(cReadChr[2] !=1)&&(cReadChr[2] != 0))
					{
						iHavePG++;
					}
					*/
				}
			}
			if(iHavePG == 1 && (iReadCout==nLength || iReadCout==nLength-1 || iReadCout==nLength-2))
			{
                iHavePG=0;
				break;
			}
			if(iHavePG >1)
			{
				break;
			}

			//split the output file for ET
			if(cReadChr[0] == 'S')
			{
				file.Read(&cReadChr[1], 1);
				iReadCout++;
				if(cReadChr[1] == 'P')
				{
					//iHavePG++;
					file.Read(&cReadChr[2], 1);
					iReadCout++;
					if (isdigit(cReadChr[2])&&(cReadChr[2] !='1')&&(cReadChr[2] != '0'))
					{
						file.Read(&cReadChr[3], 1);
						iReadCout++;
						while ((ispunct(cReadChr[3]) != 0) ||(IsAlpha(cReadChr[3]) == 0))
						{
							file.Read(&cReadChr[3], 1);
							iReadCout++;
						}
						if (cReadChr[3] == 'P')
						{
							file.Read(&cReadChr[4], 1);
							iReadCout++;
							if (cReadChr[4] == 'U')
							{
								file.Read(&cReadChr[5], 1);
								iReadCout++;
								while((ispunct(cReadChr[5]) != 0) ||(IsAlpha(cReadChr[5]) == 0))
								{
									file.Read(&cReadChr[5], 1);
									iReadCout++;
								}
								if (cReadChr[5] == 'S')
								{
									file.Read(&cReadChr[6], 1);
									iReadCout++;
									if (cReadChr[6] == 'P')
									{
										file.Read(&cReadChr[7], 1);
										iReadCout++;
										if (isdigit(cReadChr[7])&&(cReadChr[7] !='1')&&(cReadChr[7] != '0'))
										{
											return 2;		////split the output file for ET
										}
									}
								}
							}

						}
					}
					
				}
			}
			/*
			if(iHavePG > 1)
				break;
			*/
		}
		file.Close();
		if(iHavePG >= 1)//20160125���ԥ³�����ļ��ֶδ����޸�
			return 1;
	}
	
	return 0;
}

CString intToString(int iNum)
{
	CString cstr;
	if(iNum < 10)
		cstr.Format(_T("00%d"),iNum);
	else if(iNum <100)
		cstr.Format(_T("0%d"),iNum);
	else
		cstr.Format(_T("%d"),iNum);
	return cstr;
}

extern CString GetOldFileName(CString FileName, CString &ExtendName);

//�����ļ���ֽ�ų���
CString CPlotCenterDlg::ScanFileSection(CString strFileName, long lPictureLength)
{
	CFile file;
	CFile ptrCF[50];
	CString  sNewFileName, sExtName, strFirstFileName;//strFileName,
	//long cNum=0, cNewNum = 0;
	long cNum=0;
	long nFileToalLength, iFCur;
	char cGlobalCMD[100], cGCount;
	char *bufSource;
	long lWrStarPost, lWrEndPost;

	long  iToalFileNum;
	char cGetXPost[20], cigp;
	char cNewBuff[7168];
	long lGetXlen;
	long lToalSecHPGLLen;
	long cOdd;
	long iMinX, iMinY, iMaxX, iMaxY;
	char cMia[4][10], cmix, cmiy;

	//��Դ�ļ������ļ����ݶ����ڴ�
	//strFileName=pPlotItem->m_strPlotName;
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return strFileName;	
	
	nFileToalLength=file.GetLength();
	bufSource = (char *)malloc(nFileToalLength);
	file.Read(bufSource, nFileToalLength);
	file.Close();
	//�������ļ���
	iToalFileNum = lPictureLength / (long)gl_dPaperLength;
	if((lPictureLength % (long)gl_dPaperLength) != 0)
		iToalFileNum += 1;
	if(iToalFileNum==1)//20160129����PGָ��ֶ��ļ�
       return strFileName;

	//ȡ���ļ���
	sNewFileName = GetOldFileName(strFileName, sExtName);

	//�����ļ�
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString strWorkPath1, strWorkPath2, sFirstName;
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);

	strWorkPath1.Format(_T("%s%sPlot\\*.*"),szDrive,szDir);

	strWorkPath2.Format(_T("%s%sPlot\\%s%d_.%s"),szDrive,szDir,sNewFileName,0,sExtName);
	strFirstFileName = strWorkPath2;
	//20140822 �жϴ򿪵��ļ��Ƿ����Զ���ͼĿ¼�Ѿ�����
	CFileFind cFd;
	int isFile = cFd.FindFile(strWorkPath1);
	CString sCutName;
	int i=0;
	//***********

	while(i<iToalFileNum)
	{
		strWorkPath2.Format(_T("%s%sPlot\\%s%d_.%s"),szDrive,szDir,sNewFileName,cNum,sExtName);
		
		// 20140822 �жϴ򿪵��ļ��Ƿ����Զ���ͼĿ¼�Ѿ�����
		sCutName.Format(_T("%s%d_.%s"),sNewFileName,cNum,sExtName);
		isFile = cFd.FindFile(strWorkPath1);
		while(isFile)
		{
			isFile = cFd.FindNextFile();
			CString cTemp = cFd.GetFileName();
			if(cFd.GetFileName().Compare(sCutName)==0)
			{
				(++cNum) += 10;
				strWorkPath2.Format(_T("%s%sPlot\\%s%d_.%s"),szDrive,szDir,sNewFileName,cNum,sExtName);
				sCutName.Format(_T("%s%d_.%s"),sNewFileName,cNum,sExtName);
				isFile = cFd.FindFile(strWorkPath1);
			}
		}
		//*************

		if(!ptrCF[i].Open(strWorkPath2 , CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
		{
			AfxMessageBox("�ļ�����ʧ�ܣ�");
			return strFirstFileName;
		}
		
		cNum += 1;
		i++;
	}
	//cFd.Close();

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
	cNum = 0;

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
					if(bufSource[iFCur+1] == 'P' || bufSource[iFCur+1] == 'C' 
 						|| bufSource[iFCur+1] == 'I' || bufSource[iFCur+1] == 'S')
					{
						//201501291321 rwt pu0,0��ӵ�SP0����������
						//sp0����д��ÿ���ֶ��ļ���
						if ((bufSource[iFCur+1] == 'P') && (bufSource[iFCur+2] =='0'))
						{
							iFCur+=4;
							break;
						}

						if ((bufSource[iFCur+1] == 'P') && (isdigit(bufSource[iFCur+2]) !=0)
							&&(bufSource[iFCur+3] != ';'))
						{
							cGCount = 0;
							cGlobalCMD[cGCount++] = bufSource[iFCur++];
							cGlobalCMD[cGCount++] = bufSource[iFCur++];
							cGlobalCMD[cGCount++] = bufSource[iFCur++];
							cGlobalCMD[cGCount++] = ';';
							while(cNum<iToalFileNum)
								ptrCF[cNum++].Write(cGlobalCMD, cGCount);	
							cNum = 0;
							continue;
						}
						
						cGCount = 0;
						while(bufSource[iFCur] != ';')
						{
							cGlobalCMD[cGCount++] = bufSource[iFCur++];
						}
						cGlobalCMD[cGCount++] = bufSource[iFCur++];
						
						cNum = 0;
						//201501291321 rwt pu0,0��ӵ�SP0����������
						//sp0����д��ÿ���ֶ��ļ���
						if ((bufSource[iFCur+1] == 'P') && (bufSource[iFCur+2] =='0'))
						{
							iFCur+=4;
							break;
						}
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
							if(bufSource[iFCur] == 0x03)
							{
								if(bufSource[iFCur+1] == 0x0d)
								{
									iFCur++;
									lWrEndPost++;
									if(bufSource[iFCur+1] == 0x0a)
									{
										iFCur++;
										lWrEndPost++;
									}
								}
								//lWrEndPost++;//20150119 rwt ��0x03��βʱ�����ֺ�
								//��Щ�ļ�0X03��β���޷ֺţ��ʶ���ȥ
								break;
							}
							//if((bufSource[iFCur] == 0x0a) || (bufSource[iFCur] == 0x03))
							//	break;
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
						//20150427-rwt ����PU;PA****,******;ָ���
						if (bufSource[iFCur+1] == 'U' && bufSource[iFCur+2] == ';'
							&& bufSource[iFCur+3] == 'P' && bufSource[iFCur+4] == 'A')
						{
							bufSource[iFCur+2] = ' ';
							bufSource[iFCur+3] = ' ';
							bufSource[iFCur+4] = ' ';
						}

						if (bufSource[iFCur+1] == 'U' && bufSource[iFCur+2] == 'S'
							&& bufSource[iFCur+3] == 'P' )
						{
							iFCur+=2;
							break;
						}

						if ((bufSource[iFCur+1] == 'A' && bufSource[iFCur+2] == 'P'))
						{
							iFCur+=2;
							break;
						}
						
						lWrStarPost = iFCur;
						iFCur+=2;
						cigp = 0;
						
						/* pgm 20150514-rwt
						if (bufSource[iFCur+1] == 'U' && bufSource[iFCur+2] == 'S'
							&& bufSource[iFCur+3] == 'P' &&( isalpha(bufSource[iFCur+4]) != 0))
						{
							lWrStarPost = iFCur;
							lWrEndPost = iFCur+5;
							while(cNum<iToalFileNum)
							{
								ptrCF[cNum].Write(&bufSource[lWrStarPost], lWrEndPost - lWrStarPost);
								bufSource[iFCur] = ';';
								ptrCF[cNum].Write(&bufSource[iFCur], 1);
								cNum++;
							}
							iFCur+=5;
							continue;
						}
						*/
						
						
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
						
						//20150120 rwt ָ�����������Χ������ָ������
						if (cNum > (iToalFileNum-1))
						{
							while(bufSource[iFCur++] != ';')
								;
							break;
						}
						
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
							}//while(bufSource[iFCur] != ';')
							if(lWrStarPost > 7168)
								lWrStarPost = 7168;
							ptrCF[cNum].Write(cNewBuff, lWrStarPost);
						}//if(cNum != 0)
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
					else
						iFCur++;
				break;

				//20150104 rwt DIָ�������
				case 'D':
					if(bufSource[iFCur+1] == 'I' || bufSource[iFCur+1] == 'R')
					{
						lWrStarPost = iFCur;
						//lWrEndPost = iFCur;
						while ((bufSource[lWrStarPost] != 'P') &&(bufSource[lWrStarPost+1] != 'U'))
						{
							lWrStarPost++;
							//lWrEndPost++;
						}
						
						cigp = 0;
						if(bufSource[lWrStarPost+1] == ',')
							lWrStarPost++;
						while(bufSource[lWrStarPost] != ';')
						{
							if(IsDigit(bufSource[lWrStarPost]))
							{
								cGetXPost[cigp++] = bufSource[lWrStarPost];
							}
							lWrStarPost++;
							if(bufSource[lWrStarPost] == ',')
							{
								cGetXPost[cigp] = '\0';
								break;
							}
						}
						lGetXlen = atoi(cGetXPost);
						if(lGetXlen > lToalSecHPGLLen)
						{
							cNum =  lGetXlen/lToalSecHPGLLen;
						}
						else
						{
							cNum = 0;
						}
						
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
						if (cNum == 0)
						{
							cNum = 0;
							while(cNum<iToalFileNum)
								ptrCF[cNum++].Write(&bufSource[lWrStarPost], lWrEndPost - lWrStarPost);
							cNum = 0;
						}
						else
						{
							ptrCF[cNum].Write(&bufSource[lWrStarPost], lWrEndPost - lWrStarPost);
						}
						iFCur++;

					}
					else		//20150513-rwt:PGM ָ����޷��ţ�����DFPAPU13245,67890;��
					{
						lWrStarPost = iFCur;
						lWrEndPost = iFCur;

						/*
						while(bufSource[iFCur] != ';')
						{
							lWrEndPost++;
							iFCur++;
							//if((bufSource[iFCur] == 0x0a) || (bufSource[iFCur] == 0x03))
							//	break;
						}
						*/
						iFCur++;
						lWrEndPost+=2;	
						ptrCF[cNum].Write(&bufSource[lWrStarPost], lWrEndPost - lWrStarPost);
						iFCur++;
						
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
		{
			iFCur++;
			if (bufSource[iFCur] == ';')
			{
				while(cNum<iToalFileNum)
				{
					ptrCF[cNum++].Write(&bufSource[iFCur], 1);
				}
			}
		}
	}//WHILE
	free(bufSource);

	strFirstFileName.Format(_T("%s%sPlot\\%s"),szDrive,szDir, ptrCF[0].GetFileName());
	//�ر��ļ�
	cNum = 0;
	char CTcmd[] = "CT;";
	while(cNum<iToalFileNum){
		if(cNum == iToalFileNum){
		ptrCF[cNum].SeekToEnd();
		ptrCF[cNum].Write(CTcmd, strlen(CTcmd));
		ptrCF[cNum++].Close();
		break;
		}
		ptrCF[cNum++].Close();
	}
	return strFirstFileName;

	
}


int CheckExistDirFileName(CString sNewFileName, int iNum, CString sExtName)
{
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString strWorkPath1, sCutName;

	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strWorkPath1.Format(_T("%s%sPlot\\*.*"),szDrive,szDir);
	CFileFind cFd;
	int isFile = cFd.FindFile(strWorkPath1);
	//int i=0;

	sCutName.Format(_T("%s%s_.%s"),sNewFileName,intToString(iNum),sExtName);
	while(isFile)
	{
		isFile = cFd.FindNextFile();
		if(cFd.GetFileName().Compare(sCutName)==0)
		{
			++iNum;
			sCutName.Format(_T("%s%s_.%s"),sNewFileName,intToString(iNum),sExtName);
			isFile = cFd.FindFile(strWorkPath1);
		}
	}
	return iNum;
}

//�����ļ�PGָ������µ��ļ�װ���Զ���ͼĿ¼
CString CPlotCenterDlg::NewFileReentrant(CString strFileName)
{
	CFile file;
	CFile fileNew;
	CString  sNewFileName, sExtName;
	long cNum=0;
	long nLength, iReadCout;
	char cHavePG;
	long lStartCn, lEndCn;
	char *bufSource;
	
	//20100406����CFile::shareExclusive���ԣ���������CAD�ļ�д�Զ���ͼĿ¼ʱд�벻�ɹ����ӡȱҳ
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return strFileName;	
	CString strNewFileInfo;
	bool isNeedCut = FALSE;
	sNewFileName = GetOldFileName(strFileName, sExtName);

	if(sNewFileName[sNewFileName.GetLength()-1] == '_')
	{
		file.Close();
		return strFileName;
	}

	nLength=file.GetLength();
	bufSource = (char *)malloc(nLength);
	file.Read(bufSource, nLength);
	file.Close();

	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString strWorkPath1, strWorkPath2, sFirstName;

	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);

	cNum += 1;
	cNum = CheckExistDirFileName(sNewFileName, cNum, sExtName);
	strWorkPath2.Format(_T("%s%sPlot\\%s%s_.%s"),szDrive,szDir,sNewFileName,intToString(cNum),sExtName);

	iReadCout = 0;
	sFirstName = strWorkPath2;
	cHavePG = 0;
	lStartCn = 0 ;
	while(iReadCout < nLength)
	{

		if( !fileNew.Open(strWorkPath2 , CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
			return sFirstName;

		lStartCn = iReadCout;	
		lEndCn = 0;
		while(iReadCout < nLength)
		{		
			if(bufSource[iReadCout] == 'P')
			{
				if(bufSource[iReadCout+1] == 'G')
				{
					while(bufSource[iReadCout] != ';')
					{	
						iReadCout++;
					}
					iReadCout++;
					cHavePG = 1;
					break;
				}
				else
				{
					iReadCout += 2;
				}
			}
			else
			{
				iReadCout++;
			}
		}
		lEndCn = iReadCout - lStartCn;
		fileNew.Write(&bufSource[lStartCn], lEndCn);
		//���һ���ֶ��ļ�ĩβ����Զ���ָֽ��
		//label 20150204 rwt
		///*
		if (iReadCout >= nLength)
		{
			char iAutoCutOffPaper[4] = "CT;";
			//fileNew.SeekToEnd();
			//int ii = strlen(iAutoCutOffPaper);
			fileNew.Write(iAutoCutOffPaper, strlen(iAutoCutOffPaper));
		}
		
		strNewFileInfo += strWorkPath2 + "|";
		//*/
		///////////////////////////////////////////////////////////////
		fileNew.Close();

		if (iReadCout < nLength && !isNeedCut && JudgeFileNeedCutData(strWorkPath2,m_nCommunicateMode,m_socket) == 4)
		{
			isNeedCut = TRUE;
		}
		if(cHavePG==1)
		{
			cNum += 1;
			cNum = CheckExistDirFileName(sNewFileName, cNum, sExtName);
			strWorkPath2.Format(_T("%s%sPlot\\%s%s_.%s"),szDrive,szDir,sNewFileName,intToString(cNum),sExtName);

		}
	}
	free(bufSource);
	
	if (isNeedCut)
	{
		m_NeedCutDataFiles.Add(strNewFileInfo);
		//WriteLogTest(strNewFileInfo);
	}
	
	if(cHavePG==0)
	{
		DeleteFile(sFirstName);
//		g_iNewFileSplitNum = 0;
		return strFileName;
	}
	else
	{
		strWorkPath2.Format(_T("%s%sPlot\\%s00%d_.%s"),szDrive,szDir,sNewFileName,2,sExtName);

		if( !fileNew.Open(strWorkPath2 , CFile::modeRead))
		{
			DeleteFile(sFirstName);
//			g_iNewFileSplitNum = 0;	
			return strFileName;
		}
		else
		{
			fileNew.Close();
			strWorkPath2.Format(_T("%s%sPlot\\%s.%s"),szDrive,szDir,sNewFileName,sExtName);

			if(strcmp(strFileName, strWorkPath2) == 0)
				DeleteFile(strFileName);
//			g_iNewFileSplitNum = cNum;	
		}
	}

	//g_iNewFileSplitNum = cNum;		//20140530
	
	//DeleteFile(strFileName);
	return sFirstName;
}

bool	CPlotCenterDlg::LoadAsHpgl(const CString & content)//20160122�и�ָ��ʶ��
{
	long	n = content.GetLength();
	if (n == 0)
		return true;

	CStringArray & array = m_hpglArray;

	long	i;
	char	ch;

	CString	 str;
	str.Empty();

	for (i = 0; i < n; i++)
	{
		ch = content.GetAt(i);
		if (ch == 0x1B)
		{
			if (!str.IsEmpty())
			{
				array.Add(str);
				str.Empty();
			}
			for (; i < n; i++)
			{
				ch = content.GetAt(i);
				if (ch == ':')
					break;
				str += ch;
			}
			if (!str.IsEmpty())
			{
				array.Add(str);
				str.Empty();
			}
		}
		else if (ch == ';' || ch == 0x0D || ch == 0x0A  || ch == '\n')// || 
		{
			if (str.GetLength() != 0)
			{
				if (str.IsEmpty())
					continue;
				array.Add(str);
				str.Empty();
			}
		}
		else
		{
			if (str.GetLength() < 2)
			{
				if (ch <= 'Z' && ch >= 'A' || ch <= 'z' && ch >= 'a')
					str += ch;
				else
				{
					str.Empty();
					continue;
				}
			}
			else
			{
				if (str.GetAt(0) == 'L' && str.GetAt(1) == 'B')
				{
					for (; i < n; )
					{
						if ( ch == 0x02 || ch == 0x03 )
						{
							array.Add(str);
							str.Empty();
							break;
						}
						str += ch;
						i++;
						ch = content.GetAt(i);
					}
				}
				else
				{
					if (ch <= 'Z' && ch >= 'A' || ch <= 'z' && ch >= 'a')
					{
						if (!str.IsEmpty())
						{
							array.Add(str);
							str.Empty();
						}
					}
					str += ch;
				}
			}
		}
	}

	if (str.GetLength() > 0)
	{
		array.Add(str);
	}

	return true;
}


bool	CPlotCenterDlg::GetOnLinePt(CMyPoint * ptBeg, CMyPoint * ptEnd, long value, long & result, bool valX)//20160122�и�ָ��ʶ��
{
	double	temp;
	long	min;
	long	max;

	if (valX)
	{
		if (ptEnd->x > ptBeg->x && value > ptEnd->x && value < ptBeg->x
			|| ptBeg->x > ptEnd->x && value > ptBeg->x && value < ptEnd->x)
			return false;

		if (ptBeg->y == ptEnd->y)
		{
			if ((value <= ptEnd->x && value >= ptBeg->x)
				|| (value <= ptBeg->x && value >= ptEnd->x))
			{
				result = ptBeg->y;
				return true;
			}
			return false;
		}

		if (ptBeg->x == ptEnd->x)		// ����
		{
			return false;
		}

		if ((value < ptEnd->x && value > ptBeg->x)
			|| (value < ptBeg->x && value > ptEnd->x))
		{
			temp = ptEnd->y - ptBeg->y;
			temp *= (value - ptBeg->x);
			temp /= (ptEnd->x - ptBeg->x);
			temp += ptBeg->y;
			result = (long)(temp + 0.5);
			min = ptEnd->y < ptBeg->y ? ptEnd->y : ptBeg->y;
			max = ptEnd->y > ptBeg->y ? ptEnd->y : ptBeg->y;
			if (result > max)
				result = max;
			if (result < min)
				result = min;
			return true;
		}
		else if (value == ptEnd->x)
		{
			result = ptEnd->y;
			return true;
		}
		else if (value == ptBeg->x)
		{
			result = ptBeg->y;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (ptEnd->y > ptBeg->y && value > ptEnd->y && value < ptBeg->y
			|| ptBeg->y > ptEnd->y && value > ptBeg->y && value < ptEnd->y)
			return false;

		if (ptBeg->x == ptEnd->x)
		{
			if ((value <= ptEnd->y && value >= ptBeg->y)
				|| (value <= ptBeg->y && value >= ptEnd->y))
			{
				result = ptBeg->x;
				return true;
			}
			return false;
		}

		if (ptBeg->y == ptEnd->y)		// ����
		{
			return false;
		}

		if ((value < ptEnd->y && value > ptBeg->y)
			|| (value < ptBeg->y && value > ptEnd->y))
		{
			temp = ptEnd->x - ptBeg->x;
			temp *= (value - ptBeg->y);
			temp /= (ptEnd->y - ptBeg->y);
			temp += ptBeg->x;
			result = (long)(temp + 0.5);
			min = ptEnd->x < ptBeg->x ? ptEnd->x : ptBeg->x;
			max = ptEnd->x > ptBeg->x ? ptEnd->x : ptBeg->x;
			if (result > max)
				result = max;
			if (result < min)
				result = min;

			return true;
		}
		else if (value == ptEnd->y)
		{
			result = ptEnd->x;
			return true;
		}
		else if (value == ptBeg->y)
		{
			result = ptBeg->x;
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}



void	CPlotCenterDlg::SetClosedFlags()//20160122�и�ָ��ʶ��
{
	CShapeSet * pClosedSet = m_plotContext.GetClosedSet();
	int	n = pClosedSet->GetShapeCount();
	int	i;
	CShape * pShape;
	CPolyLine *	pPolyLine;

	for (i = 0; i < n; i++)
	{
		pShape = pClosedSet->GetAShape(i);
		if (pShape->m_type != SP_POLYLINE)
			continue;
		pPolyLine = (CPolyLine *)pShape;
		pPolyLine->m_bClosed = true;
	}
}




void	CPlotCenterDlg::ConnectUnbentLines()//20160122�и�ָ��ʶ��
{
	return;

	CShapeSet * pClosedSet = m_plotContext.GetClosedSet();
	int n = pClosedSet->GetShapeCount();
	if (n <= 0)
		return;
	CShape * pShape;
	CPolyLine * pPolyLine;

	CMyPoint * pPitBeg, * pPitEnd;

	int	i, j, m;

	for (i = 0; i < n; i++)
	{
		pShape = pClosedSet->GetAShape(i);
		if (pShape->m_type != SP_POLYLINE)
			continue;
		pPolyLine = (CPolyLine *)pShape;

		m = pPolyLine->GetPointCount();

		if (m <= 1)
			continue;
		pPitBeg = pPolyLine->GetPoint(0);

		for (j = 1; j < m; )
		{
			pPitEnd = pPolyLine->GetPoint(j);

			if ((pPitEnd->x == pPitBeg->x) && (pPitEnd->y == pPitBeg->y))
			{
				pPolyLine->RemovePoint(j);
				m = pPolyLine->GetPointCount();
				continue;
			}
			pPitBeg = pPitEnd;
			j++;
		}
	}
}




void	CPlotCenterDlg::RemovePointLine()//20160122�и�ָ��ʶ��
{// ȥ�����鵱��ǰ���ظ��ĵ�
	CShapeSet * pClosedSet = m_plotContext.GetClosedSet();
	int n = pClosedSet->GetShapeCount();
	if (n <= 0)
		return;
	CShape * pShape;
	CPolyLine * pPolyLine;

	CMyPoint * pPitBeg, * pPitEnd;

	int	i, j, m;

	for (i = 0; i < n; )
	{
		pShape = pClosedSet->GetAShape(i);
		if (pShape->m_type != SP_POLYLINE)
		{
			i++;
			continue;
		}
		pPolyLine = (CPolyLine *)pShape;

		m = pPolyLine->GetPointCount();

		if (m <= 1)
			continue;
		pPitBeg = pPolyLine->GetPoint(0);

		for (j = 1; j < m; )
		{
			pPitEnd = pPolyLine->GetPoint(j);

			if ((pPitEnd->x == pPitBeg->x) && (pPitEnd->y == pPitBeg->y))
			{
				pPolyLine->RemovePoint(j);
				m = pPolyLine->GetPointCount();
				continue;
			}
			pPitBeg = pPitEnd;
			j++;
		}
		if (m == 1)
		{
			pClosedSet->RemoveShape(i);
			n = pClosedSet->GetShapeCount();
			continue;
		}
		i++;
	}

}




void	CPlotCenterDlg::ConnectLines(CIntPointSet & intPtSet)//20160122�и�ָ��ʶ��
{

	int	n = intPtSet.GetPitsCount();
	if (n <= 0)
		return;
	int	i, idx, m;

	CShapeSet * pClosedSet = m_plotContext.GetClosedSet();

	bool	change;
	CIntPoint * pIntpit;
	CIntPoint * pIntpited;

	CPolyLine *	pPolyLine;
	CPolyLine *	pPolyLine1;
	CMyPoint	pit;

	do
	{
		change = false;
		n = intPtSet.GetPitsCount();

		for (i = 0; i < n; )							// 	����"�㼯��"�е�ÿһ����
		{
			pIntpit = intPtSet.GetIntPoint(i);
			m = pIntpit->GetPolyLineCount();
			if (m == 2)									//	�����ǰ��p��Ӧ�����鼯��ֻ������Ԫ��
			{
				pPolyLine = pIntpit->GetPolyLine(0);
				pPolyLine1= pIntpit->GetPolyLine(1);
				if (pPolyLine == pPolyLine1)			// ��������Ԫ����ͬһ��Ԫ��
				{										// �򽫸�������ӵ�������߼�����
					pClosedSet->AddShape(pPolyLine);
					pIntpit->RemovePolyLine(1);
					pIntpit->RemovePolyLine(0);			// �ӵ�p�Ķ�Ӧ�����鼯����ɾ��������Ԫ��
					intPtSet.RemoveIntPoint(i);			//	��p��ӵ㼯����ɾ��
					delete	pIntpit;
					pIntpit = NULL;
					n = intPtSet.GetPitsCount();
					continue;
				}

				if (pPolyLine->GetPointCount() < pPolyLine1->GetPointCount())	// �ҵ��۵���һ������
				{
					CPolyLine * tmp;
					tmp = pPolyLine1;
					pPolyLine1 = pPolyLine;
					pPolyLine = tmp;
				}

				if (pPolyLine1->GetFirstPoint() == pIntpit->GetIntPoint())		// �õ�Դ���ߵķǽ���˵�
				{
					pit = pPolyLine1->GetLastPoint();
				}
				else if (pPolyLine1->GetLastPoint() ==pIntpit->GetIntPoint())
				{
					pit = pPolyLine1->GetFirstPoint();
				}
				else
				{
					ASSERT(0);
				}

				CoalitionPolyLine(pPolyLine, pPolyLine1);	// �ϲ�����

				pPolyLine1->RemoveAll();

				idx = intPtSet.PointInSet(pit);

				ASSERT(idx >= 0);

				change = true;

				pIntpited = intPtSet.GetIntPoint(idx);
				pIntpited->RemovePolyLine(pPolyLine1);
				pIntpited->AddPolyLine(pPolyLine);		// ����һ�˵ĵ��Ӧ�ĵ�����߼����е���ӦԪ���滻Ϊ�ϲ��������

				intPtSet.RemoveIntPoint(i);			//	��p��ӵ㼯����ɾ��
				delete	pIntpit;
				pIntpit = NULL;
				n = intPtSet.GetPitsCount();
				continue;
			}
			i++;
		}

	}while (change);
}


bool	CPlotCenterDlg::CoalitionPolyLine(CPolyLine * pTargetPL, CPolyLine * pSourcePL)//20160122�и�ָ��ʶ��
{
	int	n = pSourcePL->GetPointCount();
	int	i;
	CMyPoint * pPoint;

	if (pTargetPL->GetFirstPoint() == pSourcePL->GetFirstPoint())
	{
		for (i = 0; i < n; i++)
		{
			pPoint = pSourcePL->GetPoint(i);
			pTargetPL->InsertLine(0, pPoint);
		}
		return true;
	}
	else if (pTargetPL->GetFirstPoint() == pSourcePL->GetLastPoint())
	{
		for (i = n - 1; i >= 0; i--)
		{
			pPoint = pSourcePL->GetPoint(i);
			pTargetPL->InsertLine(0, pPoint);
		}
		return true;
	}
	else if (pTargetPL->GetLastPoint() == pSourcePL->GetFirstPoint())
	{
		for (i = 0; i < n; i++)
		{
			pPoint = pSourcePL->GetPoint(i);
			pTargetPL->AddPoint(pPoint);
		}
		return true;
	}
	else if (pTargetPL->GetLastPoint() == pSourcePL->GetLastPoint())
	{
		for (i = n - 1; i >= 0; i--)
		{
			pPoint = pSourcePL->GetPoint(i);
			pTargetPL->AddPoint(pPoint);
		}
		return true;
	}
	else
	{
		ASSERT(0);
	}
	return false;
}




void	CPlotCenterDlg::FindLineUnicursal(CIntPointSet & intPtSet)//20160122�и�ָ��ʶ��
{
	int n = intPtSet.GetPitsCount();
	if (n <= 0)
		return;
	int	m;
	int	i, j, k;

	CShapeSet * pClosedSet = m_plotContext.GetClosedSet();

	CPolyLine *	pPolyLine;
	CPolyLine *	pPolyLine1;
	CIntPoint * pIntpit;
	bool	change = false;

	for (i = 0; i < n; i++)				//	����"�㼯��"�е�ÿһ����
	{
		pIntpit = intPtSet.GetIntPoint(i);
		m = pIntpit->GetPolyLineCount();
		if (m < 2)						// ���ĳ�����Ӧ������С������
			continue;
		for (j = 0; j < m; j++)
		{
			if (j > m - 2)
				break;
			pPolyLine = pIntpit->GetPolyLine(j);
			for (k = j + 1; k < m; k++)
			{
				pPolyLine1 = pIntpit->GetPolyLine(k);
				if (pPolyLine == pPolyLine1)		// ���ĳ����p��Ӧ�����߼�����������Ԫ����ͬһ��Ԫ��
				{									// �򽫸�������ӵ�"�ѷ�����鼯��"��
					change = true;
					pClosedSet->AddShape(pPolyLine);
					pIntpit->RemovePolyLine(k);
					pIntpit->RemovePolyLine(j);
					m = pIntpit->GetPolyLineCount();
					break;
				}
			}
		}
	}

	if (change)
	{
		ClearUpIntPtSet(intPtSet);
	}

}




void	CPlotCenterDlg::ClearUpIntPtSet(CIntPointSet & intPtSet)//20160122�и�ָ��ʶ��
{
	int	n = intPtSet.GetPitsCount();
	if (n <= 0)
		return;

	int i, idx;
	bool	change;
	CIntPoint * pIntpit;
	CIntPoint * pIntpited;
	CPolyLine *	pPolyLine;
	CMyPoint	pit;

	int	m;

	do
	{
		change = false;
		n = intPtSet.GetPitsCount();
		for (i = 0; i < n; )						//	����"�㼯��"�е�ÿһ����
		{
			pIntpit = intPtSet.GetIntPoint(i);
			m = pIntpit->GetPolyLineCount();
			if (m == 0)	// ���ĳ����p��Ӧ�����鼯��û��Ԫ��
			{										// ���"�㼯��"ɾ���õ�
				intPtSet.RemoveIntPoint(i);
				delete	pIntpit;
				pIntpit = NULL;
				n = intPtSet.GetPitsCount();
				change = true;
				continue;
			}

			if (m == 1)			// ���ĳ����p��Ӧ�����鼯��ֻ��һ��Ԫ��
			{										// ������һ�����˵���
				pPolyLine = pIntpit->GetPolyLine(0);
				pit = pPolyLine->GetFirstPoint();
				if (pIntpit->GetIntPoint() == pit)
					pit = pPolyLine->GetLastPoint();
				idx = intPtSet.PointInSet(pit);
				ASSERT(idx != -1);
				ASSERT(idx != i);

				pIntpited = intPtSet.GetIntPoint(idx);	//	���ҵ����������һ����q

				pIntpited->RemovePolyLine(pPolyLine);	// �� q ���Ӧ�����鼯���г�ȥ������ i

				change = true;

				intPtSet.RemoveIntPoint(i);
				n = intPtSet.GetPitsCount();
				delete	pIntpit;
				pIntpit = NULL;
				continue;
			}

			i++;
		}

	}while(change);

}


bool	CPlotCenterDlg::FetchANumber(const CString & hpglCmd, int & idx, long & result)//20160122�и�ָ��ʶ��
{
	CString	num;

	num.Empty();

	int	n = hpglCmd.GetLength();
	if (idx >= n)
		return false;

	char	ch;

	for (; idx < n; idx++)
	{
		ch = hpglCmd.GetAt(idx);
		if ((ch <= '9' && ch >= '0')||ch=='-')//�����ŵ��ļ���ʾ����20160225,����||ch=='-'
			break;
	}									// �������ַ���ʼ
	if (idx >= n)
		return false;
	for (; idx < n; idx++)
	{
		ch = hpglCmd.GetAt(idx);
		if ((ch <= '9' && ch >= '0')||ch=='-')//�����ŵ��ļ���ʾ����20160225,����||ch=='-'
			num += ch;
		else
			break;
	}									// ���������ַ�����

	result = atol(num.GetBuffer(num.GetLength()));

	return true;
}

bool	CPlotCenterDlg::FetchANumber(const CString & hpglCmd, int &idx, double & result)//20160122�и�ָ��ʶ��
{
	CString	num;

	num.Empty();

	int	n = hpglCmd.GetLength();
	if (idx >= n)
		return false;

	char	ch;
	bool	sign = true;

	for (; idx < n; idx++)
	{
		ch = hpglCmd.GetAt(idx);
		if (ch == '-')
		{
			sign = false;
			break;
		}
		else if (ch == '+')
		{
			sign = true;
			break;
		}
		else if (ch <= '9' && ch >= '0' || ch == '.')
			break;
	}									// �����ţ����ֻ�С�������
	if (idx >= n)
		return false;
	for (; idx < n; idx++)
	{
		ch = hpglCmd.GetAt(idx);
		if (ch <= '9' && ch >= '0' || ch == '.')
			break;
	}									// �������ַ���С���㿪ʼ
	if (idx >= n)
		return false;
	for (; idx < n; idx++)
	{
		ch = hpglCmd.GetAt(idx);
		if (ch <= '9' && ch >= '0' || ch == '.')
			num += ch;
		else
			break;
	}									// ���������ַ�����, ������С����

	result = atof(num.GetBuffer(num.GetLength()));
	if (result == 0)
	{
		if (sign)
		{
			result = 0.0000000000001;
		}
		else
		{
			result = -0.0000000000001;
		}
	}
	else
	{
		if (!sign)
		{
			result = 0 - result;
		}
	}
	return true;
}


int		CPlotCenterDlg::GetAngleX(double x, double y)
{
	int	result;
	double	temp;

	if (y == 0)
	{
		if (x >= 0)
			result = 0;
		else
			result = 1800;
	}
	else
	{
		if (x == 0)
		{
			if (y > 0)
				result = 900;
			else
				result = 2700;
		}
		else
		{
			temp = atan2(y, x);
			temp /= 3.14159265;
			result = int (1800 * temp);
		}
	}

	return result;
}

bool	CPlotCenterDlg::ProcessHpglCmd(CString & hpglCmd, CPlotContext & plotContext)//20160122�и�ָ��ʶ��
{

	int n = hpglCmd.GetLength();
	if (n < 2)
		return true;
	char cm1, cm2;

	cm1 = hpglCmd.GetAt(0);
	cm2 = hpglCmd.GetAt(1);

	int	i;
	long	result;
	double	dbrestlt;
	CMyPoint	point;
	CPolyLine * pPolyLine = NULL;
	CLabel	 *	pLab = NULL;
	CString		str;
	double		direction;
	CSize		size;

	if (cm1 == 0x1B)
		return true;	// ����pclָ�������
	
	if (m_bIPValid && m_bSCValid)
	{
		m_dRatioX=(m_dpP2.x-m_dpP1.x)/(m_struSC.dXMax-m_struSC.dXMin);
		m_dRatioY=(m_dpP2.y-m_dpP1.y)/(m_struSC.dYMax-m_struSC.dYMin);
		m_dpOrigin.x=ftoi(m_dpP1.x-m_struSC.dXMin*m_dRatioX);
		m_dpOrigin.y=ftoi(m_dpP1.y-m_struSC.dYMin*m_dRatioY);
	}

	switch (cm1)
	{
	case 'D' :
		switch (cm2)
		{
		case 'F':
			break;
		case 'I':		// ���ַ���
			plotContext.m_strDI = hpglCmd;

			i = 2;
			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			direction = dbrestlt;
			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			plotContext.m_escapement = GetAngleX(direction, dbrestlt);
			break;
		case 'T':
			break;
		default :
			break;
		};
		break;
	case 'I' :
		switch (cm2)
		{
		case 'N':
			if (plotContext.m_inited == false)
			{
				plotContext.InitContext();
				plotContext.m_inited = true;
			}
			break;
		case 'P':
			cm2 = cm2;
			//g_dpP1=CPoint(ftoi(pInstruct->m_adPara.GetAt(0)),ftoi(pInstruct->m_adPara.GetAt(1)));
			//g_dpP2=CPoint(ftoi(pInstruct->m_adPara.GetAt(2)),ftoi(pInstruct->m_adPara.GetAt(3)));
			//gzr20161025 ����plt��IP����
			i = 2;
			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			m_dpP1.x = ftoi(dbrestlt);
			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			m_dpP1.y = ftoi(dbrestlt);

			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			m_dpP2.x = ftoi(dbrestlt);
			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			m_dpP2.y = ftoi(dbrestlt);
			m_bIPValid = TRUE;
			break;
		default:
			break;
		};
		break;

	case 'L' :
		switch (cm2)
		{
		case 'B':
			str.Format("%s", (hpglCmd.GetBuffer(n) + 2));
			if (!str.IsEmpty())
			{
				pLab = new CLabel;
//					pLab->m_cmd	DI = plotContext.m_strDI;
//					pLab->m_cmdSI = plotContext.m_strSI;
				pLab->m_type = SP_LABEL;
				pLab->m_content = str;
				pLab->m_penCode = plotContext.GetCurrentPenCode();
				pLab->m_chSizer = plotContext.m_charSize;
				pLab->m_startPoint = plotContext.GetCurrentPenPos();
				pLab->m_arrange = plotContext.m_arrange;
				pLab->m_escapement = plotContext.m_escapement;
				plotContext.GetShapeSet()->AddShape(pLab);
				pLab = NULL;
			}
			str.Empty();
			break;
		case 'O':			// �������з�ʽ
			i = 2;
			if (!FetchANumber(hpglCmd, i, result))
				result = 0;
			if (result % 10 <= 3)
				plotContext.m_arrange = ARG_LEFT;
			else if (result % 10 <= 6)
				plotContext.m_arrange = ARG_CENTER;
			else
				plotContext.m_arrange = ARG_RIGHT;
			break;
		case 'T':			// ����
			break;
		default:
			break;
		};

		break;

	case 'P' :
		switch (cm2)
		{
		case 'A':
			plotContext.m_penMode = PENABSOLUTE;
			if (plotContext.m_penStatus == PENUP)
				goto	PUCMD;
			else
				goto	PDCMD;
			break;

		case 'D':
PDCMD:
			if (plotContext.m_penStatus == PENUP || plotContext.m_curPolyLine == NULL)	// ������µ�PD
			{
				plotContext.m_penStatus = PENDOWN;
				pPolyLine = new CPolyLine;
				pPolyLine->m_type = SP_POLYLINE;
				pPolyLine->m_penCode = plotContext.GetCurrentPenCode();
				pPolyLine->AddPoint(plotContext.GetCurrentPenPos());
				plotContext.m_curPolyLine = pPolyLine;
				plotContext.GetShapeSet()->AddShape(pPolyLine);
			}

			if (n <= 2)
				break;

			pPolyLine = plotContext.m_curPolyLine;

			for (i = 2; i < n; )
			{
				if (!FetchANumber(hpglCmd, i, result))
					break;
				////gzr20161025
				//point.x = result + plotContext.m_xPage;
				if (m_bSCValid && m_bIPValid)
				{
					point.x=ftoi(result*m_dRatioX+m_dpOrigin.x);
				}
				else
					point.x = result + plotContext.m_xPage;

				if (m_plotContext.m_xMax < point.x)
					m_plotContext.m_xMax = point.x;

				if (m_plotContext.m_xMin > point.x)
					m_plotContext.m_xMin = point.x;

				if (!FetchANumber(hpglCmd, i, result))
					break;
				//gzr20161025
				if (m_bSCValid && m_bIPValid)
				{
					point.y=ftoi(result*m_dRatioY+m_dpOrigin.y);
				}
				else
					point.y = result;
				

				plotContext.SetCurrentPenPos(point);
				pPolyLine->AddPoint(point);
			}

			break;

		case 'G':
			plotContext.m_xPage = plotContext.m_xMax;	
			break;
		case 'U':
PUCMD:
			plotContext.m_penStatus = PENUP;
			plotContext.m_curPolyLine = NULL;
			if (n <= 2)
				break;
			i = 2;
			if (!FetchANumber(hpglCmd, i, result))
				break;
			//gzr20161025
			//point.x = result + plotContext.m_xPage;
			if (m_bSCValid && m_bIPValid)
			{
				point.x=ftoi(result*m_dRatioX+m_dpOrigin.x);
			}
			else
				point.x = result + plotContext.m_xPage;

			if (m_plotContext.m_xMax < point.x)
				m_plotContext.m_xMax = point.x;

			if (m_plotContext.m_xMin > point.x)
					m_plotContext.m_xMin = point.x;


			if (!FetchANumber(hpglCmd, i, result))
				break;
			//gzr20161025
			//point.y = result;
			if (m_bSCValid && m_bIPValid)
			{
				point.y=ftoi(result*m_dRatioY+m_dpOrigin.y);
			}
			else
				point.y = result;

			plotContext.SetCurrentPenPos(point);

			break;
		default:
			break;
		};

		break;
	case 'S' :
		switch (cm2)
		{
		case 'C':
			//gzr20161025 ����plt��SC����
			i = 2;
			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			m_struSC.dXMin=dbrestlt;
			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			m_struSC.dXMax=dbrestlt;
			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			m_struSC.dYMin=dbrestlt;
			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			m_struSC.dYMax=dbrestlt;
			m_struSC.nType=0;
			m_bSCValid = TRUE;
			break;
		case 'I':	// �ַ��Ĵ�С
			plotContext.m_strSI = hpglCmd;
			i = 2;
			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			dbrestlt *= 10;
			dbrestlt /= plotContext.m_inUnit;
			size.cx = (long)(dbrestlt + 0.5);
			if (!FetchANumber(hpglCmd, i, dbrestlt))
				break;
			dbrestlt *= 10;
			dbrestlt /= plotContext.m_inUnit;
			size.cy = (long)(dbrestlt + 0.5);
			plotContext.m_charSize = size;

			break;
		case 'P':
			i = 2;
			if (!FetchANumber(hpglCmd, i, result))
				break;
			plotContext.SetCurrentPenCode(BYTE(result));
			break;
		default:
			break;
		};

		break;

	default :
		break;
	}
/*
	if (cm1 == 'P' && (cm2 == 'D' || cm2 == 'A' && plotContext.m_penStatus == PENDOWN))
	{
	}
	else
	{
	}
*/
	return true;
}


bool	CPlotCenterDlg::FillShapeArray(const CStringArray & hpglArray)//20160122�и�ָ��ʶ��
{
	long	n = hpglArray.GetSize();
	if (n <= 0)
		return true;

	m_plotContext.m_xPage = 0;
	m_plotContext.m_xMax = 0x80000000;
	m_plotContext.m_xMin = 0x7fffffff;
	m_plotContext.m_bMutiLine = false;
	m_plotContext.m_curPolyLine = NULL;

//	m_plotContext.m_bSerial = false;

	long	i;
	for (i = 0; i < n; i++)
	{
		ProcessHpglCmd(hpglArray.GetAt(i), m_plotContext);
	}

	return true;
}


void	CPlotCenterDlg::SetDrawPens(BYTE pen)//20160122�и�ָ��ʶ��
{
	CShapeSet * pShapeSet = m_plotContext.GetShapeSet();

	int	n = pShapeSet->GetShapeCount();
	int	i;

	CShape * pShape;

	for (i = 0; i < n; i++)
	{
		pShape = pShapeSet->GetAShape(i);
		pShape->m_penCode = pen;
	}
}



bool	CPlotCenterDlg::PointOnLine(CMyPoint * pPtLBg, CMyPoint * pPtLEd, CMyPoint * pPit)//20160122�и�ָ��ʶ��
{
	ASSERT (pPtLBg);
	ASSERT (pPtLEd);
	ASSERT (pPit);

	if (pPit->x > (pPtLBg->x > pPtLEd->x ? pPtLBg->x : pPtLEd->x)
		|| pPit->x < (pPtLBg->x < pPtLEd->x ? pPtLBg->x : pPtLEd->x)
		|| pPit->y > (pPtLBg->y > pPtLEd->y ? pPtLBg->y : pPtLEd->y)
		|| pPit->y < (pPtLBg->y < pPtLEd->y ? pPtLBg->y : pPtLEd->y))
	{
		return false;
	}

	if (pPtLBg->x == pPtLEd->x)
	{
		if (pPit->x == pPtLBg->x)
		{
			if (pPtLBg->y <= pPtLEd->y && pPit->y <= pPtLEd->y && pPit->y >= pPtLBg->y
				|| pPtLBg->y >= pPtLEd->y && pPit->y >= pPtLEd->y && pPit->y <= pPtLBg->y)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else if (pPtLBg->y == pPtLEd->y)
	{
		if (pPit->y == pPtLBg->y)
		{
			if (pPtLBg->x <= pPtLEd->x && pPit->x <= pPtLEd->x && pPit->x >= pPtLBg->x
				|| pPtLBg->x >= pPtLEd->x && pPit->x >= pPtLEd->x && pPit->x <= pPtLBg->x)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else
	{
		int	dy1 = pPtLEd->y - pPtLBg->y;
		int	dx1 = pPtLEd->x - pPtLBg->x;
		int	dy0 = pPit->y - pPtLBg->y;
		int	dx0 = pPit->x - pPtLBg->x;
		int	tp1 = dy1 * dx0;
		int	tp2 = dy0 * dx1;
		//double	dtp0 = (double)dy0 / dx0;
		//double	dtp1 = (double)dy1 / dx1;

		if (tp1 == tp2)
			return true;
		else
			return false;

	}

	return false;
}


bool	CPlotCenterDlg::PointOnPolyLine(CPolyLine * pPolyLine, CMyPoint * pPtBg, CMyPoint * pPtEd)//20160122�и�ָ��ʶ��
{
	int n = pPolyLine->GetPointCount();
	if (n <= 1)
		return false;

	CMyPoint * pLastPoint, * pCurPoint;

	pCurPoint = pPolyLine->GetPoint(0);
	int i;

	for (i = 1; i < n; i++)
	{
		pLastPoint = pCurPoint;
		pCurPoint = pPolyLine->GetPoint(i);

		if (PointOnLine(pLastPoint, pCurPoint, pPtBg))
		{
			// �ж���ֱ�߼н��Ƿ�Ϊ�ӽ�90��
			if (pLastPoint->x == pCurPoint->x && pLastPoint->y == pCurPoint->y)
				return true;
			else if (pLastPoint->x == pCurPoint->x)
			{
				if (pPtBg->y - pPtEd->y < 5 || pPtBg->y - pPtEd->y > -5)
					return true;
			}
			else if (pLastPoint->y == pCurPoint->y)
			{
				if (pPtBg->x - pPtEd->x < 5 || pPtBg->x - pPtEd->x > -5)
					return true;
			}
			else
			{
				double	temp, result;
				temp = pCurPoint->x - pLastPoint->x;
				temp = (pCurPoint->y - pLastPoint->y) / temp;
				temp = atan(temp);
				temp /= 3.14159265;
				result = 1800 * temp;

				temp = pPtEd->x - pPtBg->x;
				temp = (pPtEd->y - pPtBg->y) / temp;
				temp = atan(temp);
				temp /= 3.14159265;
				temp = 1800 * temp;

				if (abs(int (result - temp)) - 900 < 10
					|| abs(int (result - temp)) - 900 > -10
					|| abs(int (result - temp)) - 2700 < 10
					|| abs(int (result - temp)) - 2700 > -10)
				{
					return true;
				}
			}
		}
	}

	return false;
}


void	CPlotCenterDlg::GetCircumRect(CPolyLine * pPolyLine, CRect * pRect)//20160122�и�ָ��ʶ��
{
	// �õ��������Ӿ���
	int i;
	int	n = pPolyLine->GetPointCount();

	long	minx, miny, maxx, maxy;

	minx = miny = 0x7fffffff;
	maxx = maxy = 0x80000000;

	pRect->SetRectEmpty();
	CMyPoint * pPoint;
	bool	change = false;

	for (i = 0; i < n; i++)
	{
		pPoint = pPolyLine->GetPoint(i);
		if (pPoint->x < minx)
		{
			minx = pPoint->x;
			change = true;
		}
		if (pPoint->y < miny)
		{
			miny = pPoint->y;
			change = true;
		}

		if (pPoint->x > maxx)
		{
			maxx = pPoint->x;
			change = true;
		}
		if (pPoint->y > maxy)
		{
			maxy = pPoint->y;
			change = true;
		}
	}

	if (change)
	{
		pRect->left = minx;
		pRect->right = maxx;
		pRect->top = miny;
		pRect->bottom = maxy;
	}
}


void	CPlotCenterDlg::IdentifyTTypeCut()//20160122�и�ָ��ʶ��
{
	// ʶ��T�ͼ���
	CShapeSet * pShapeSet = m_plotContext.GetShapeSet();

	int	n = pShapeSet->GetShapeCount();

	if (n <= 0 )
		return;

	CShapeSet * pClosedSet = m_plotContext.GetClosedSet();

	int	m = pClosedSet->GetShapeCount();
	if (m <= 0)
		return;

	CTTypeCutSet * pTTypeSet = new CTTypeCutSet;
	CTTypeCut	* pTtpCut;

	CShapeSet * pLineShapeSet = new CShapeSet;

	int	i, j;
	CShape * pShape;
	CPolyLine * pPolyLine;
	CPolyLine * pClosedLine;

	// �ҳ����еĵ���ֱ��
	for (i = 0; i < n; i++)
	{
		pShape = pShapeSet->GetAShape(i);
		if (pShape->m_type != SP_POLYLINE)
			continue;
		pPolyLine = (CPolyLine *)pShape;
		if (pPolyLine->m_bClosed || pPolyLine->m_bCut)
			continue;
		if (pPolyLine->GetPointCount() == 2)
		{
			pLineShapeSet->AddShape(pPolyLine);
		}
	}

	n = pLineShapeSet->GetShapeCount();
	if (n <= 0)
		return;

	// �õ����з��ͼ�ε���Ӿ���
	int	rectCount = m;
	CRect * rectary = new CRect[rectCount];
	for (i = 0; i < rectCount; i++)
	{
		pShape = pClosedSet->GetAShape(i);
		if (pShape->m_type == SP_POLYLINE)
		{
			pPolyLine = (CPolyLine *)pShape;
			GetCircumRect(pPolyLine, &(rectary[i]));
		}
	}

	// �õõ��ĵ���ֱ�߼��ϲ���
	pShapeSet = pLineShapeSet;

	long x0, y0, x1, y1;
	CRect	rect;

	// �ҵ����кͷ�����������ӵĵ���ֱ��
	for (i = 0; i >= 0 && i < n; i++)
	{
		pShape = pShapeSet->GetAShape(i);

		ASSERT (pShape->m_type == SP_POLYLINE);
		pPolyLine = (CPolyLine *)pShape;
		ASSERT (pPolyLine->GetPointCount() == 2);

		for (j = 0; j < m; j++)
		{
			x0 = pPolyLine->GetPoint(0)->x;
			y0 = pPolyLine->GetPoint(0)->y;
			x1 = pPolyLine->GetPoint(1)->x;
			y1 = pPolyLine->GetPoint(1)->y;
			rect = rectary[j];

			if (x0 > rect.right || x0 < rect.left
				|| y0 > rect.bottom || y0 < rect.top
				|| x1 > rect.right || x1 < rect.left
				|| y1 > rect.bottom || y1 < rect.top)
			{
				continue;
			}

			double l = sqrt(((double)x1-x0)*((double)x1-x0) + ((double)y1-y0)*((double)y1-y0));
			if (l > 10/m_plotContext.m_inUnit)		// ���ڲ��ܴ���15mm
			{
				continue;
			}

			pShape = pClosedSet->GetAShape(j);
			if (pShape->m_type != SP_POLYLINE)
				continue;
			pClosedLine = (CPolyLine *)pShape;

			if (PointOnPolyLine(pClosedLine, pPolyLine->GetPoint(0), pPolyLine->GetPoint(1)))
			{
				pTtpCut = new CTTypeCut;
				pTtpCut->pLineOnClosed = pPolyLine;
				pTtpCut->pPtOnClosed = pPolyLine->GetPoint(0);
				pTtpCut->pPtOnBand = pPolyLine->GetPoint(1);
				pTtpCut->m_rectIdx = j;
				pTTypeSet->AddTCut(pTtpCut);
				pShapeSet->RemoveShape(i);
				n = pShapeSet->GetShapeCount();
				i--;
				break;
			}
			else if (PointOnPolyLine(pClosedLine, pPolyLine->GetPoint(1), pPolyLine->GetPoint(0)))
			{
				pTtpCut = new CTTypeCut;
				pTtpCut->pLineOnClosed = pPolyLine;
				pTtpCut->pPtOnClosed = pPolyLine->GetPoint(1);
				pTtpCut->pPtOnBand = pPolyLine->GetPoint(0);
				pTtpCut->m_rectIdx = j;
				pTTypeSet->AddTCut(pTtpCut);
				pShapeSet->RemoveShape(i);
				n = pShapeSet->GetShapeCount();
				i--;
				break;
			}
			else
			{
				continue;
			}
		}
	}

	m = pTTypeSet->GetTCutCount();
	n = pShapeSet->GetShapeCount();
	if (m <= 0 || n <= 0)
		return;

	// �ҳ��ͷ�����������ӵ�ֱ�����ӵ�ֱ��
	int	idx;

	for (j = 0; j < m; j++)
	{
		pTtpCut = pTTypeSet->GetATCut(j);
		idx = pTtpCut->m_rectIdx;
		ASSERT(idx >= 0 && idx < rectCount);
		rect = rectary[idx];

		n = pShapeSet->GetShapeCount();
		for (i = 0; i < n; i++)
		{
			pShape = pShapeSet->GetAShape(i);
			ASSERT (pShape->m_type == SP_POLYLINE);
			pPolyLine = (CPolyLine *)pShape;
			ASSERT (pPolyLine->GetPointCount() == 2);

			x0 = pPolyLine->GetPoint(0)->x;
			y0 = pPolyLine->GetPoint(0)->y;
			x1 = pPolyLine->GetPoint(1)->x;
			y1 = pPolyLine->GetPoint(1)->y;

			if (x0 > rect.right	|| x0 < rect.left
				|| y0 > rect.bottom	|| y0 < rect.top
				|| x1 > rect.right || x1 < rect.left
				|| y1 > rect.bottom	|| y1 < rect.top)
			{
				continue;
			}
			else
			{
				double l = sqrt(((double)x1-x0)*((double)x1-x0) + ((double)y1-y0)*((double)y1-y0));
				if (l > 10/m_plotContext.m_inUnit)		// ���ڲ��ܴ���10mm
				{
					continue;
				}

				if (abs(x0 + x1 - 2 * pTtpCut->pPtOnBand->x) <= 1
					&& abs(y0 + y1 - 2 * pTtpCut->pPtOnBand->y) <= 1)
				{
					// ��ӽǶ��ж�.....

					pTtpCut->pLineCrsBand = pPolyLine;
					pTtpCut->m_done = true;
					pShapeSet->RemoveShape(i);
					break;
				}
			}
		}
	}


	// ���������еı�־
	CShapeSet * pTCutSet = m_plotContext.GetTCutSet();

	for (i = 0; i < m; i++)
	{
		pTtpCut = pTTypeSet->GetATCut(i);
		if (pTtpCut->pLineOnClosed != NULL
			&& pTtpCut->pPtOnBand != NULL
			&& pTtpCut->pPtOnClosed != NULL)
		{
			if (pTtpCut->pLineCrsBand != NULL)
			{
				pTtpCut->pLineCrsBand->m_bCut = true;
				pTCutSet->AddShape(pTtpCut->pLineCrsBand);
			}
			pTtpCut->pLineOnClosed->m_bCut = true;
			pTCutSet->AddShape(pTtpCut->pLineOnClosed);
		}
		else
			continue;
	}

	delete	rectary;
	delete	pLineShapeSet;
	pTTypeSet->DeleteAll();
	delete	pTTypeSet;

	return;
}

void	CPlotCenterDlg::SetCuttedFlags()//20160122�и�ָ��ʶ��
{
	CShapeSet * pClosedSet = m_plotContext.GetClosedSet();

	int	n = pClosedSet->GetShapeCount();
	int	i;
	CShape * pShape;
	CPolyLine *	pPolyLine;

	for (i = 0; i < n; i++)
	{
		pShape = pClosedSet->GetAShape(i);
		if (pShape->m_type != SP_POLYLINE)
			continue;
		pPolyLine = (CPolyLine *)pShape;
		if (pPolyLine->m_bClosed == false)
			continue;
		pPolyLine->m_bCut = true;
	}
}


void	CPlotCenterDlg::OrderScanValue(CUIntArray & array)//20160122�и�ָ��ʶ��
{ // ����, ��С����
	int n = array.GetSize();
	if (n < 2)
		return;
	int i, j, k;
	k=0;
	UINT	tmp;
	bool	change = false;

	for (i = 0; i < n; i++)
	{
		tmp = array.GetAt(i);

		for (j = i; j < n; j++)
		{
			if (array.GetAt(j) < tmp)
			{
				tmp = array.GetAt(j);
				k = j;
				change = true;
			}
		}
		if (!change)
			continue;
		if (k == i)
			continue;
		else
		{
			array.SetAt(k, array.GetAt(i));
			array.SetAt(i, tmp);
		}
	}
}



bool	CPlotCenterDlg::PointInClosedGraph(CPolyLine *	pPolyLine, CPoint pit)//20160122�и�ָ��ʶ��
{
// ������������, �жϺ�ͼ�εĽ����������ȷ�����Ƿ�����ͼ����

	int	m, n;
	int	i, j;

	n = pPolyLine->GetPointCount();
	if (n <= 0)
		return true;
	else if (n < 3)
		return false;

	long	result;

	CMyPoint * ptBeg, * ptEnd, * ptLst;
	ptBeg = NULL;
	ptEnd = NULL;
	ptLst = NULL;

	CUIntArray	yArray, xArray;
	yArray.RemoveAll();
	xArray.RemoveAll();

	ptEnd = pPolyLine->GetPoint(0);
	ptLst = pPolyLine->GetPoint(n - 1);

	if ((ptLst->x == ptEnd->x) && (ptLst->y == ptEnd->y))
	{
		ptLst = pPolyLine->GetPoint(n - 2);
	}

	for (i = 1; i < n; i++)
	{
		if (i > 1)
		{
			ptLst = ptBeg;
		}
		ptBeg = ptEnd;
		ptEnd = pPolyLine->GetPoint(i);
		if (ptBeg->x == ptEnd->x && pit.x == ptBeg->x)		// ����, ��������������
		{
			result = ptBeg->y < ptEnd->y ? ptBeg->y : ptEnd->y;
			yArray.Add(result);
			result = ptBeg->y > ptEnd->y ? ptBeg->y : ptEnd->y;
			yArray.Add(result);
		}
		else
		{
			if (GetOnLinePt(ptBeg, ptEnd, pit.x, result))
			{
				if ((ptBeg->x == pit.x) && (ptBeg->y == result))
				{	// ����������߶εĿ�ʼ��
					// ��ô�ж��������Ƿ����� y = pit.x ���ͬ��
					if (((ptLst->x < pit.x) && (ptEnd->x < pit.x))
						|| ((ptLst->x > pit.x) && (ptEnd->x > pit.x)))
					{ // ����ͬ��
						// ���������������, ��Ҫ��ӵ�ǰ��
						yArray.Add(result);

					}
					else	// ����, ֻ����Ϊһ���㲻��ӵ�ǰ��
					{
					}
				}
				else		// ����ֱ����ӵ�
				{
					yArray.Add(result);
				}
			}
		}

		if (ptBeg->y == ptEnd->y && pit.y == ptBeg->y)		// ����
		{
			result = ptBeg->x < ptEnd->x ? ptBeg->x : ptEnd->x;
			xArray.Add(result);
			result = ptBeg->x > ptEnd->x ? ptBeg->x : ptEnd->x;
			xArray.Add(result);
		}
		else
		{
			if (GetOnLinePt(ptBeg, ptEnd, pit.y, result, false))
			{
				if ((ptBeg->y == pit.y) && (ptBeg->x == result))
				{	// ����������߶εĿ�ʼ��
					// ��ô�ж��������Ƿ����� x = pit.y ���ͬ��
					if (((ptLst->y < pit.y) && (ptEnd->y < pit.y))
						|| ((ptLst->y > pit.y) && (ptEnd->y > pit.y)))
					{ // ����ͬ��
						// ���������������, ��Ҫ��ӵ�ǰ��
						xArray.Add(result);
					}
					else	// ����, ֻ����Ϊһ���㲻��ӵ�ǰ��
					{
					}
				}
				else		// ����ֱ����ӵ�
				{
					xArray.Add(result);
				}
			}
		}
	}

	m = yArray.GetSize();			// y�������
	if (m <= 0)						// û�н���
		return false;
	n = xArray.GetSize();			// x�������
	if (n <= 0)						// û�н���
		return false;
	if (m == 1 && n == 1)
		return false;				// ����н���, �к�xy���㶼ֻ��һ��,����Ϊ����ͼ����

	OrderScanValue(yArray);		// ����, ��С����
	OrderScanValue(xArray);		// ����, ��С����

	if ( UINT(pit.y) < yArray.GetAt(0) || UINT(pit.y) > yArray.GetAt(m - 1)
		||	UINT(pit.x) < xArray.GetAt(0) || UINT(pit.x) > xArray.GetAt(n - 1))
	{
		return false;
	}

	bool isin = false;
	UINT tmp;
	tmp = yArray.GetAt(0);
	j = 0;
	for (i = 1; i < m; i++)			// ��С��������
	{

		if ((i % 2) == 0)
		{
			if (UINT(pit.y) < yArray.GetAt(i))
				isin = false;
			else
				continue;
		}
		else
		{
			if (UINT(pit.y) < yArray.GetAt(i))
				isin = true;
			else
				continue;
		}
	}

	if (isin == false)
		return false;

	for (i = 0; i < n; i++)			// ��С��������
	{
		if ((i % 2) == 0)
		{
			if (UINT(pit.x) < xArray.GetAt(i))
				isin = false;
			else
				continue;
		}
		else
		{
			if (UINT(pit.x) < xArray.GetAt(i))
				isin = true;
			else
				continue;
		}
	}

	return isin;

}




void	CPlotCenterDlg::FilterClosedLines()//20160122�и�ָ��ʶ��
{
	CShapeSet * pClosedSet = m_plotContext.GetClosedSet();
	int n = pClosedSet->GetShapeCount();
	if (n <= 0)
		return;
	CShape * pShape;
	CPolyLine * pPolyLine1, * pPolyLine2;
	int	i, j;

	CRect *	pRect1, * pRect2;

	CRect * rectary = new CRect[n];
	for (i = 0; i < n; )							// �õ����е���Ӿ���
	{
		pShape = pClosedSet->GetAShape(i);
		if (pShape->m_type == SP_POLYLINE)
		{
			pPolyLine1 = (CPolyLine *)pShape;
			GetCircumRect(pPolyLine1, &(rectary[i]));
			if (rectary[i].IsRectEmpty())
			{
				pClosedSet->RemoveShape(i);
				n = pClosedSet->GetShapeCount();
				continue;
			}
		}
		i++;
	}

	n = pClosedSet->GetShapeCount();

	// ȥ����Ӿ���̫С�ķ��ͼ��, ӦΪ����������

	for (i = 0; i < n; )
	{
		if (rectary[i].Width() < m_maxWordWidth && rectary[i].Height() < m_maxWordHeight)
		{
			pClosedSet->RemoveShape(i);
			for (j = i; j < n - 1; j++)
				rectary[j] = rectary[j + 1];
			rectary[n - 1].SetRectEmpty();
			n = pClosedSet->GetShapeCount();
			continue;
		}
		i++;
	}

	n = pClosedSet->GetShapeCount();

	// �ж�������Ӿ����ǲ��Ǳ�Ե��, ����ǾͲ���Ҫ�и�Ĳ���

	if (n <= 1)
		return;
	pRect1 = NULL;

	for (i = 0, j = 0; i < n; i++)			// �õ�������
	{
		if (pRect1 == NULL)
		{
			j = i;
			pRect1 = &(rectary[i]);
		}
		else
		{
			if ((pRect1->Width() <= rectary[i].Width()) && pRect1->Height() <= rectary[i].Height())
			{
				j = i;
				pRect1 = &(rectary[i]);
			}
		}
	}
	if (pRect1 != NULL)
	{
		pShape = pClosedSet->GetAShape(j);
		if (pShape->m_type == SP_POLYLINE)
		{
			pPolyLine1 = (CPolyLine *)pShape;
			int n1 = pPolyLine1->GetPointCount();
			if (n1 >= 5)
			{
				CMyPoint	* pit1, * pit2;
				pit2 = pPolyLine1->GetPoint(0);
				for (i = 1; i < n1; i++)				// �ж��Ƿ�Ϊ����
				{
					pit1 = pit2;
					pit2 = pPolyLine1->GetPoint(i);
					if (pit1->x == pit2->x || pit1->y == pit2->y)
						continue;
					break;
				}
				if (i >= n1)
				{
					// �Ƿ��������ͼ�ζ�����, �������, �������δӷ�ռ�����ɾ��
					for (i = 0; i < n; i++)
					{
						if (i == j)
							continue;
						pRect2 = &(rectary[i]);
						if (pRect1->left <= pRect2->left
							&& pRect1->right >= pRect2->right
							&& pRect1->top <= pRect2->top
							&& pRect1->bottom >= pRect2->bottom)	// rect1 ���� rect2
						{
							break;
						}
					}
					if (i < n)
					{
						pClosedSet->RemoveShape(j);
						for (i = j; i < n - 1; i++)
							rectary[i] = rectary[i + 1];
					}
				}
			}
		}
	}

	n = pClosedSet->GetShapeCount();
	// ȥ���������������ͼ���еķ��ͼ��

	for (i = 0; i < n; )
	{
		pShape = pClosedSet->GetAShape(i);
		if (pShape->m_type == SP_POLYLINE)
		{
			pPolyLine1 = (CPolyLine *)pShape;
			pRect1 = &(rectary[i]);
			for (j = 0; j < n; j++)
			{
				if (j == i)
					continue;
				pShape = pClosedSet->GetAShape(j);
				if (pShape->m_type == SP_POLYLINE)
				{
					pPolyLine2 = (CPolyLine *)pShape;
					pRect2 = &(rectary[j]);
					if (pRect2->left <= pRect1->left
						&& pRect2->top <= pRect1->top
						&& pRect2->right >= pRect1->right
						&& pRect2->bottom >= pRect1->bottom)	// line1 ������ line2 ��
					{
						int k;
						int ptCount = pPolyLine1->GetPointCount();
						CPoint point;
						int	inCount = 0;
						if (i == 31)
						{
							i = 31;
						}

						for (k = 0; k < ptCount; k++)				// ��� line1 ��ÿ���㶼�� line2 ��, ��ô line1 һ���� line2 ��
						{
							point.x = pPolyLine1->GetPoint(k)->x;
							point.y = pPolyLine1->GetPoint(k)->y;
							if (PointInClosedGraph(pPolyLine2, point))	// �жϵ��Ƿ���ͼ����
							{
								inCount++;
							}
						}
						if (inCount == ptCount)		// ���еĵ㶼��ͼ����
						{
							break;					// ɾ��
						}
						else if (inCount == 0)		// ���еĵ㶼����ͼ����
						{
							continue;
						}
						else						// �в��ֵ���ͼ���У���һ�ֱ߽����⡣
						{							// ���ø��ʽ������������ȫ�ķ���
							// 
							double	db = (double)inCount / ptCount;
							
							if (ptCount < 5)
							{
								if (db > 0.5)
									break;
								else
									continue;
							}
							else if (ptCount < 20)
							{
								if (db > 0.45) 
									break;
								else
									continue;
							}
							else if (ptCount < 50)
							{
								if (db > 0.35)
									break;
								else
									continue;
							}
							else if (ptCount < 100)
							{
								if (db > 0.2)
									break;
								else
									continue;
							}
							else
							{
								if (db > 0.1)
									break;
								else
									continue;
							}
//							break;
						}
					}
				}
			}
			if (j < n)
			{
				pClosedSet->RemoveShape(i);
				for (j = i; j < n - 1; j++)
					rectary[j] = rectary[j + 1];
				n = pClosedSet->GetShapeCount();
				continue;
			}
			i++;
		}
	}

	delete	[]rectary;
}



bool	CPlotCenterDlg::GroupLines()//20160122�и�ָ��ʶ��
{
	CIntPointSet	intPtSet;

	CShapeSet * pShapeSet = m_plotContext.GetShapeSet();

	int	n =	pShapeSet->GetShapeCount();
	int	i, idx;
	CShape * pShape;
	CPolyLine *	pPolyLine;
    CMyPoint point;
	CIntPoint *	pIntPit;
	
	for (i = 0; i < n; i++)					// ���������鰴��ͣ�������ӵ�"�㼯��"��
	{
		pShape = pShapeSet->GetAShape(i);
		if (pShape->m_type != SP_POLYLINE)
			continue;
		pPolyLine = (CPolyLine *)pShape;
		point = pPolyLine->GetFirstPoint();	// ���
		idx = intPtSet.PointInSet(point);
		if (idx == -1)						// �������"�㼯��"��
		{
			pIntPit = new CIntPoint;
			pIntPit->SetIntPoint(point);
			intPtSet.AddIntPoint(pIntPit);	// ����ӵ�"�㼯��"��
		}
		else								// ����
		{
			pIntPit = intPtSet.GetIntPoint(idx);
		}
		pIntPit->AddPolyLine(pPolyLine);		// ����������


		point = pPolyLine->GetLastPoint();	// �յ�
		idx = intPtSet.PointInSet(point);
		if (idx == -1)						// �������"�㼯��"��
		{
			pIntPit = new CIntPoint;
			pIntPit->SetIntPoint(point);
			intPtSet.AddIntPoint(pIntPit);	// ����ӵ�"�㼯��"��
		}
		else								// ����
		{
			pIntPit = intPtSet.GetIntPoint(idx);
		}
		pIntPit->AddPolyLine(pPolyLine);		// ����������
	}

	n = intPtSet.GetPitsCount();
	for (i = 0; i < n; i++)
	{
		pIntPit = intPtSet.GetIntPoint(i);
		point = pIntPit->GetIntPoint();
	}

	// ȥ����ͷ������
	ClearUpIntPtSet(intPtSet);

	// �ҳ�һ�ʻ��ɵķ������
	FindLineUnicursal(intPtSet);

	// ����һ�������ӵ��������ߺϲ���һ��
	n = intPtSet.GetPitsCount();
	int m;
	do
	{
		m = n;
		ConnectLines(intPtSet);			// �����߶�
		FindLineUnicursal(intPtSet);	// �ҵ����ͼ��
		ClearUpIntPtSet(intPtSet);
		n = intPtSet.GetPitsCount();

	}while (n != m);

	// ������Ӳ��ҷ��ͼ���㷨


	// �ҵ������еķ��ͼ��, 

	// ȥ�����ͼ���еĵ�ֱ��
	RemovePointLine();

	ConnectUnbentLines();

	// ���÷�ձ�־
	SetClosedFlags();

	// �����еķ�������й��˵�һЩ���Ϸ��ķ������
	FilterClosedLines();

	// �����ͼ��������־, �Ա���ٷ���
	SetCuttedFlags();

	// ʶ��T�ͼ���
	IdentifyTTypeCut();

	//if (!isCutCommand)
	{
		SetDrawPens(theApp.m_config.m_curPen);
	}
	

	// ɾ�����㼯��
	n = intPtSet.GetPitsCount();
	for (i = 0; i < n; i++)
	{
		pIntPit = intPtSet.GetIntPoint(i);
		delete	pIntPit;
	}
	intPtSet.RemoveAll();

	n = intPtSet.GetPitsCount();
	for (i = 0; i < n; i++)
	{
		pIntPit = intPtSet.GetIntPoint(i);
		point = pIntPit->GetIntPoint();
	}

	return true;
}

bool	CPlotCenterDlg::GetDirectionX(int escapement, double & x, double & y)//20160122�и�ָ��ʶ��
{
	if (escapement == 0)
	{
		x = 1;
		y = 0;
	}
	else if (escapement == 900 || escapement == -2700)
	{
		x = 0;
		y = 1;
	}
	else if (abs(escapement) == 1800)
	{
		x = -1;
		y = 0;
	}
	else if (escapement == 2700 || escapement == -900)
	{
		x = 0;
		y = -1;
	}
	else
	{
		double	result;
		result = escapement;

		result = tan(result * 3.14159265 / 1800);
		x = 1;
		y = result;
	}

	return true;
}


void CPlotCenterDlg::InitPara()//20160122�и�ָ��ʶ��
{
	m_filePath.Empty();
	m_fileContent.Empty();
	m_hpglArray.RemoveAll();

	// ����������������Ǵ������еõ�, ���������û�����
	m_maxWordWidth = 300;
	m_maxWordHeight = 400;
}

void CPlotCenterDlg::OnOpen1() //20160122�и�ָ��ʶ��
{
	//20160122�и�ָ��ʶ��
	m_plotContext.DeleteAll();
	
	char Custer[80]="All file(*.*)\0*.*\0\0";
	BOOL bWaitConnected;

	bWaitConnected = m_bWaitConnected;
	m_bWaitConnected = false;
	CFileDialog dlg(TRUE);
	dlg.m_ofn.lpstrFilter=Custer;
	if(dlg.DoModal()==IDOK)
	{
		CFile file;
		CString strFileName;
		strFileName=dlg.GetPathName();
		m_bOpenFile=true;
	}
	CFile file;
	CString strFileName;
	strFileName=dlg.GetPathName();

	InitPara();

	CFileException e;

	m_filePath = strFileName;

	char *	pBuf = new	char[64*1024 + 1];
	if (pBuf == NULL)
		return;
	TRY
	{
		CFile file(m_filePath, CFile::modeRead | CFile::typeBinary);
		UINT	n = file.GetLength();
		UINT	i, j;
		i = 0;
		j = (n - i) < 64 * 1024 ? (n - i) : 64 * 1024;
		file.SeekToBegin();

		while(i < n && j >0)
		{
			file.Read(pBuf, j);
			pBuf[j] = '\0';
			m_fileContent += pBuf;
			i += j;
			j = (n - i) < 64 * 1024 ? (n - i) : 64 * 1024;
		};
		file.Close();					// �ر��ļ�
	}
	CATCH( CFileException, e )
	{
		#ifdef _DEBUG
			afxDump << "File could not be opened " << e->m_cause << "\n";
		#endif
//		AfxMessageBox ("File could not be opened ");

		InitPara();
		delete	[]pBuf;
		return;
	}
	END_CATCH
	delete	[]pBuf;


	LoadAsHpgl(m_fileContent);

	FillShapeArray(m_hpglArray);

	GroupLines();

	CShapeSet * pShapeSet = m_plotContext.GetShapeSet();


    //m_pPltContxt = GetDocument()->GetPlotContext();
	m_plotContext.SetCuttedPenCode(4);

	Invalidate();

	CString		m_hpgl;
	double		m_unit;
	long m_length;

	//m_length = 350;

	m_length = 350;
	m_unit = 0.025;

	// �õ��������
	double	page = m_length;
	page /= m_unit;
	// ����ÿһ����ҳ���߼���λ
	//m_pDoc->Pagination((long)(page+0.5), m_hpgl);
	// ��ҳ���, ҳ��С width length ���߼���λ

	

    long length=(long)(page+0.5);

	pShapeSet = new CShapeSet;
	pShapeSet->CreateSimilarSet(m_plotContext.GetShapeSet());

	CString hpgl;

	hpgl.Empty();

	hpgl += "IN;DF;PA;PU;SP1;";

	CMyPoint ptThis, ptLast, *pptThis, *pptLast;

	CShape	* pShape;
	CPolyLine * pPolyLine;
	CLabel	*	pLabel;
	CString	str;

	bool	first = true;
	bool	firstline;
	CSize		m_charSize;
	int			m_escapement;
	Arrange		m_arrange;
	double	x, y;

	long	result;

	bool	bNewShape;
	BYTE	curpen = 1;
	bool	changePage = false;//gzr20160913 �жϷ�ҳ
	int i, n, j, m;

	do
	{
		n = pShapeSet->GetShapeCount();
		for (i = 0; i < n; )
		{
			pShape = pShapeSet->GetAShape(i);

			if (pShape->m_type == SP_POLYLINE)
			{
				pPolyLine = (CPolyLine *)pShape;
				m = pPolyLine->GetPointCount();

				if (m < 2)								// ��������еĵ�ĸ���С��2����ɾ��������
				{
					if (m == 1)
					{
						delete	pPolyLine->GetPoint(0);
						pPolyLine->RemoveAll();
					}
					pShapeSet->RemoveShape(i);
					delete	pPolyLine;
					n = pShapeSet->GetShapeCount();
					continue;
				}

				if (curpen != pPolyLine->m_penCode)		// �Ƿ񻻱�
				{
					if(pPolyLine->m_penCode==0)//20160127�޸Ĺ�
					{
					str.Empty();
					str.Format("PU;SP%d;", 1);//20160127�޸Ĺ�
					//gzr20160909  2
					hpgl += str;
					curpen = pPolyLine->m_penCode;
					}
					else
					{
	                str.Empty();
					str.Format("PU;SP%d;", pShape->m_penCode);//20160127�޸Ĺ�
					//gzr20160909  1
					hpgl += str;
					curpen = pPolyLine->m_penCode;
					}
				}
				pptLast = pptThis = NULL;

				bNewShape = false;
				firstline = true;
				changePage = false;//gzr20160913 �жϷ�ҳ
				for (j = 1; j < m;)
				{
					pptLast = pPolyLine->GetPoint(j - 1);
					pptThis = pPolyLine->GetPoint(j);
				
					if (pptLast->x == pptThis->x && pptLast->x == length)
					{
						pptLast->x -= length;
						j++;
					}
					else if (pptLast->x <= length && pptThis->x <= length)
					{
						str.Empty();
						if (firstline)
						{
							firstline = false;

							str.Format("PU%d,%d;PD%d,%d;", pptLast->x, pptLast->y, pptThis->x, pptThis->y);
						}
						else
						{

							str.Format("PD%d,%d;", pptThis->x, pptThis->y);
						}
						//gzr20160909  3
						hpgl += str;
						//gzr20160913 �ж��Ƿ��з�ҳ���� start
						
						if (j != 1 && changePage)
						{
							//�з�ҳ����һ�㱣������һҳ
							pptLast->x -= length;
							j++;
						}
						else
						{
							//�޷�ҳ����һ��ɾ��
							pPolyLine->RemovePoint(pptLast);
							delete	pptLast;
						}
						m = pPolyLine->GetPointCount();
						//�ж����޷�ҳ�͵�ǰ�߶���ѭ����
						if (!changePage && j == m)
						{
							//�޷�ҳ���ҵ�ǰ�߶���ѭ���꣬ɾ����ǰ��
							pPolyLine->RemovePoint(pptThis);
							delete	pptThis;
 						}
						//gzr20160913 �жϷ�ҳ
						m = pPolyLine->GetPointCount();
 					
						changePage = false;
						// continue;
						//gzr20160913 �ж��Ƿ��з�ҳ���� end
					}
					else if (pptLast->x < length && pptThis->x > length)
					{
						GetOnLinePt(pptLast, pptThis, length, result);
						str.Empty();
						if (firstline)
						{
							firstline = false;

							str.Format("PU%d,%d;PD%d,%d;", pptLast->x, pptLast->y, length, result);
						}
						else
						{
							str.Format("PD%d,%d;", length, result);
						}
						//gzr20160909  4
						hpgl += str;
						firstline = true;
						pptLast->x = length;
						pptLast->y = result;
					}
					else if ((pptLast->x > length && pptThis->x < length))
					{
						GetOnLinePt(pptLast, pptThis, length, result);

						// �ָ�
						CMyPoint * tppit = new CMyPoint;	// �µ�this
						tppit->x = pptThis->x;
						tppit->y = pptThis->y;				// ���� this

						pptThis->x = length;
						pptThis->y = result;				// this ����

						CMyPoint * tppit1 = new CMyPoint;	// �µ�last
						tppit1->x = pptThis->x;
						tppit1->y = pptThis->y;				// ���ƺ��this

						pptThis->x -= length;
						pptLast->x -= length;				// ���߶�ǰ����, Ϊ��һҳ׼��

						CPolyLine * tppline = new CPolyLine;
						tppline->m_penCode = pPolyLine->m_penCode;
						tppline->AddPoint(tppit1);			
						tppline->AddPoint(tppit);			// ���ƺ��point��Ϊ��ʼ
						j++;
						while (j < m)		// �����ߺ����ĵ��ƶ����µ�������
						{
							tppit = pPolyLine->GetPoint(j);
							tppline->AddPoint(tppit);
							pPolyLine->RemovePoint(j);
							m = pPolyLine->GetPointCount();
						}
						i++;
						pShapeSet->InsertShape(i, tppline);
						n = pShapeSet->GetShapeCount();
						bNewShape = true;
					}
					else if (pptLast->x >= length && pptThis->x >= length)
					{
						pptLast->x -= length;
						j++;
						if (j >= m)
							pptThis->x -= length;
						//gzr20160913 �жϷ�ҳ
						changePage = true;
					}
					else
					{
						i = i;
					}
				}
				if (bNewShape)
				{
					continue;
				}
				if (m < 2)
				{

					if (m == 1)
					{
						delete	pPolyLine->GetPoint(0);
						pPolyLine->RemoveAll();
					}
					pShapeSet->RemoveShape(i);
					delete	pPolyLine;
					n = pShapeSet->GetShapeCount();
					continue;
				}
			}

			else if (pShape->m_type == SP_LABEL)
			{
				pLabel = (CLabel *)pShape;

				//if (curpen != pLabel->m_penCode && pLabel->m_penCode!=0)		// �Ƿ񻻱�//20160418���Ĺ�HHYע�͵��ˣ�ֻҪ���־ͻ�ΪSP1
				{
					str.Empty();
					//str.Format("PU;SP%d;", pShape->m_penCode);
					str.Format("PU;SP%d;", 1);
					hpgl += str;
					//curpen = pLabel->m_penCode;
					curpen = 1;
				}

				pptLast = pptThis = NULL;

				if (pLabel->m_startPoint.x < length)
				{
					if (first)
					{
						first = false;
						m_escapement = pLabel->m_escapement;
						m_charSize = pLabel->m_chSizer;
						m_arrange = pLabel->m_arrange;


						GetDirectionX(pLabel->m_escapement, x, y);
						str.Format("DI%1.4f,%1.4f;", x, y);
						hpgl += str;

						x = pLabel->m_chSizer.cx * 0.025 / 10;
						y = pLabel->m_chSizer.cy * 0.025 / 10;
						str.Format("SI%1.4f,%1.4f;", x, y);
						hpgl += str;

						str.Format("LO%d;", pLabel->m_arrange);
						hpgl += str;

						str.Format("PU%d,%d;", pLabel->m_startPoint.x, pLabel->m_startPoint.y);
						hpgl += str;
						str.Empty();

						str.Format("LB%s%c;", pLabel->m_content, 0x03);
						hpgl += str;
						str.Empty();
					}
					else
					{
						if (m_escapement != pLabel->m_escapement)
						{
							GetDirectionX(pLabel->m_escapement, x, y);
							m_escapement = pLabel->m_escapement;
							str.Format("DI%1.4f,%1.4f;", x, y);
							hpgl += str;
						}
						if (m_charSize != pLabel->m_chSizer)
						{
							x = pLabel->m_chSizer.cx * 0.025 / 10;
							y = pLabel->m_chSizer.cy * 0.025 / 10;
							str.Format("SI%1.4f,%1.4f;", x, y);
							hpgl += str;
						}


						if (m_arrange != pLabel->m_arrange)
						{
							str.Format("LO%d;", pLabel->m_arrange);
							hpgl += str;
						}

						str.Format("PU%d,%d;", pLabel->m_startPoint.x, pLabel->m_startPoint.y);
						hpgl += str;
						str.Empty();

						str.Format("LB%s%c;", pLabel->m_content, 0x03);
						hpgl += str;
						str.Empty();
					}
					pShapeSet->RemoveShape(i);
					n = pShapeSet->GetShapeCount();
					continue;
				}
				else
				{
					pLabel->m_startPoint.x -= length;
				}
			}
			else
			{
				// ����ͼ��
			}
			i++;
		}

		hpgl += "PU;SP1;PG;";
		curpen = 1;
		if (n <= 0)							// ���
			break;

	}while (true);

//gzr20160913 ��ʱע�� start

	pShapeSet->DeleteAll();



// �����ļ�
	char *	temp = new char[MAX_PATH + 1];
	if (temp == NULL)
		return ;
	ZeroMemory(temp, MAX_PATH);
	GetModuleFileName(AfxGetInstanceHandle(), temp, MAX_PATH);
		i;
	for(i = MAX_PATH; i >= 0;  i--)
	{
		if (temp[i]=='\\')
		{
			temp[i] = '\0';
			break;
		}
	}



		//�����ļ�ɾ������þ��ļ���
		CString oldHBname;
		oldHBname.Empty();
		char *	pBuf1 = new	char[64*1024 + 1];
	    if (pBuf1 == NULL)
		return;
	    TRY
		{
		CFile file2("C:\\LIQIECUT.txt", CFile::modeRead | CFile::typeBinary);
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
		file2.Close();					// �ر��ļ�
		}
	    CATCH( CFileException, e )
		{
		CFile file3("C:\\LIQIECUT.txt", CFile::modeCreate | CFile::modeWrite);
		file3.Write("", 0);
		file3.Close();
		}
    	END_CATCH
	    delete	[]pBuf1;

	strFileName1=dlg.GetFileName();
	//CString	name;
	strFileName.Format("%s\\cut\\"+strFileName1, temp);
	//m_pDoc->SaveHPGLFile(name, m_hpgl);

		char oldfilename[100];
        strncpy(oldfilename,(LPCTSTR)oldHBname,sizeof(oldfilename));
        //int bbb=rename(oldfilename,newfilename);//Ϊ��Ļ��������ɹ�
		if(oldHBname!=strFileName)
		{
		remove(oldfilename);//Ϊ��Ļ�ɾ���ļ��ɹ�
		}

	 int n1=strFileName.GetLength();
	 CFile file1("C:\\LIQIECUT.txt", CFile::modeCreate | CFile::modeWrite);
	 file1.Write(strFileName, n1);
	 file1.Close();


	n = hpgl.GetLength();
	LPTSTR	p = hpgl.GetBuffer(n);

	TRY
	{
		CFile file(strFileName, CFile::modeCreate | CFile::modeWrite );

		file.Write(p, n);

		file.Close();
	}

	CATCH( CFileException, e )
	{
		#ifdef _DEBUG
			afxDump << "File could not be opened " << e->m_cause << "\n";
			//AfxMessageBox ("File could not be opened ");
		#endif
		hpgl.ReleaseBuffer();
		return ;
	}
	END_CATCH
	hpgl.ReleaseBuffer();



	delete	[]temp;

		n = m_hpgl.GetLength();
	//LPTSTR	pp = m_hpgl.GetBuffer(n);
	if(n <= 0)
	{
		m_hpgl.Empty();
	}


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
				
				if(m_lMinPlotLength < 0)
					m_lMinPlotLength = 0;
				dlgPicture.m_lFileMinLength = m_lMinPlotLength;
				
				if(m_lMinPlotWidth < 0)
					m_lMinPlotWidth = 0;
				dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;

				dlgPicture.m_lTotalX = (m_lMaxPlotLength + 10 * 40) * long((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth) - 10 * 40;
				dlgPicture.m_lTotalY = lPaperWidth;
			}
			else
			{
				dlgPicture.m_lFileLength = m_lMaxPlotLength;
				dlgPicture.m_lFileWidth = m_lMaxPlotWidth;

				if(m_lMinPlotLength < 0)
					m_lMinPlotLength = 0;
				dlgPicture.m_lFileMinLength = m_lMinPlotLength;

				if(m_lMinPlotWidth < 0)
					m_lMinPlotWidth = 0;
				dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;

				dlgPicture.m_lTotalX = m_lMaxPlotLength;
				dlgPicture.m_lTotalY = m_lMaxPlotWidth;
			}
			pShapeSet = m_plotContext.GetShapeSet();
			dlgPicture.m_strPictureName = strFileName;
			
            pShapeSet = m_plotContext.GetShapeSet();
			if(dlgPicture.DoModal()==IDOK)
			{
				//����ļ� 20140715
				char cExist = 0;
				int cPGExist = 0;
			    cPGExist = QuickScanFilePG(strFileName);
				//201501230945 rwt 
				

				if((cPGExist == 0)&&((g_cPrintCutSelect&0x02) == 0x02))
				{
					strFileName = ScanFileSection(strFileName, ftoi(m_lMaxPlotLength*0.025));
					cExist = 1;
				}
				if((cPGExist == 1)&&((g_cPrintCutSelect == 1))//20160113ֻ��ӡ�ļ����ֶ�
					&&(ftoi(m_lMaxPlotLength*0.025) > gl_dPaperLength))	
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
				}

				else if((cPGExist == 1)&&((g_cPrintCutSelect == 3)||(g_cPrintCutSelect == 2)))
				{
					strFileName = NewFileReentrant(strFileName);
					cExist = 1;
				}
			
				else if (cPGExist == 2)			//ET ����ָ��ļ�ҳ��
				{
					strFileName = EtFileSection(strFileName);
					cExist = 1;
				}
				
				if(cExist == 1)
				{
					OpenFile(strFileName);
					if(m_lMinPlotLength < 0)
						m_lMinPlotLength = 0;
					if(m_lMinPlotWidth < 0)
						m_lMinPlotWidth = 0;

					dlgPicture.m_lFileMinLength = m_lMinPlotLength;
					dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
					dlgPicture.m_lFileLength = m_lMaxPlotLength;
					dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
					dlgPicture.m_lTotalY = m_lMaxPlotWidth;
					dlgPicture.m_lTotalX = m_lMaxPlotLength;
					//cExist = 1;
					
				}
				//*******************
				CString strCaption,strCaption1;
				TCHAR szDir[_MAX_DIR];
				TCHAR szDrive[_MAX_DRIVE];
				TCHAR szFullPath[_MAX_PATH];
				::GetModuleFileName(NULL,szFullPath,_MAX_PATH);

				_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
				strCaption1.Format(_T("%s%sPlot\\"),szDrive,szDir);
				
				if(!cExist)
				{
					_splitpath(strFileName,szDrive,szDir,NULL,NULL);
					strCaption.Format(_T("%s%s"),szDrive,szDir);
				}

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
						m_nStartPrint = 1; //20140729
						m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,true);//20140729
					}
				}
			}
		}
		else if(nReturnValue==1)
		{//�ļ��򲻿�
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING106);
			else
				s1.LoadString(IDS_STRING106_EN);
			AfxMessageBox(s1);
		}
		else
		{//��HP-GL�ļ�!
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING108);
			else
				s1.LoadString(IDS_STRING108_EN);
			AfxMessageBox(s1);
		}
		RemoveGLBuffer();
		m_bOpenFile=false;

		m_bWaitConnected =bWaitConnected;

	//return;
	//20160122�и�ָ��ʶ��

}


//����ET��������PLT�ļ������µ��ļ�װ���Զ���ͼĿ¼20150514rwt
CString CPlotCenterDlg::EtFileSection(CString strFileName)//20150514rwt
{
	CFile file;
	CFile fileNew;
	CString  sNewFileName, sExtName;
	long lOldMaxXlength;
	long lNewMaxXlength;
	CString iNewMaxXlength;
	long cNum=0;
	long nLength, iReadCout;
	char cHavePG;
	char cHaveET;
	bool isEvenNumber;
	bool isHavePG;
	long lStartCn, lEndCn;
	char *bufSource;
	

	//����CFile::shareExclusive���ԣ���������CAD�ļ�д�Զ���ͼĿ¼ʱд�벻�ɹ����ӡȱҳ
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return strFileName;	

	sNewFileName = GetOldFileName(strFileName, sExtName);
	if(sNewFileName[sNewFileName.GetLength()-1] == '_')
	{
		file.Close();
		return strFileName;
	}

	nLength=file.GetLength();
	bufSource = (char *)malloc(nLength);
	file.Read(bufSource, nLength);
	file.Close();

	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString strWorkPath1, strWorkPath2, sFirstName;

	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);

	cNum += 1;
	cNum = CheckExistDirFileName(sNewFileName, cNum, sExtName);
	strWorkPath2.Format(_T("%s%sPlot\\%s%s_.%s"),szDrive,szDir,sNewFileName,intToString(cNum),sExtName);
	iReadCout = 0;
	sFirstName = strWorkPath2;
	cHavePG = 0;
	cHaveET = 0;
	lStartCn = 0 ;
	while(iReadCout < nLength)
	{
		if( !fileNew.Open(strWorkPath2 , CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
			return sFirstName;

		lStartCn = iReadCout;	
		lEndCn = 0;
		lOldMaxXlength = 0;
		iNewMaxXlength.Empty();
		
		while (iReadCout < nLength)
		{		
			//split the output file for ET
			if (bufSource[iReadCout] == 'S')
			{
				iReadCout++;
				if (bufSource[iReadCout] == 'P')
				{
					iReadCout++;
					if ((IsDigit(bufSource[iReadCout]) != 0) && (bufSource[iReadCout] != '0') 
						&& (bufSource[iReadCout] != '1'))
					{
						cHaveET = 1;
						iReadCout++;
					}
					else if (bufSource[iReadCout] == '1')
					{
						if (cHaveET == 1)
						{
							cHaveET = 0;
							cHavePG = 1;
							iReadCout -= 2;
							break;
						}
						
					}
				}
			}
			else if (bufSource[iReadCout] == 'P')
			{
				iReadCout++;
				if (bufSource[iReadCout] == 'D')
				{
					isEvenNumber = true;
					iReadCout++;
					while(bufSource[iReadCout] != ';')
					{
						if (bufSource[iReadCout] == ',')
						{
							if (isEvenNumber == true)
							{
								lNewMaxXlength = atoi(iNewMaxXlength);
								if (lNewMaxXlength > lOldMaxXlength)
								{
									lOldMaxXlength = lNewMaxXlength;
								}
								iNewMaxXlength.Empty();
								isEvenNumber = false;
							}
							else
							{
								isEvenNumber = true;
							}
							iReadCout++;
							continue;
						}
						if ((isEvenNumber == true) && (IsDigit(bufSource[iReadCout]) != 0))
						{
							iNewMaxXlength += bufSource[iReadCout];
						}
						iReadCout++;
					}
				}
			}
			else
			{
				iReadCout++;
			}
		}
		lEndCn = iReadCout - lStartCn;
		fileNew.Write(&bufSource[lStartCn], lEndCn);

		//��β��PDָ���ļ����֣�������һ�ļ�
		isHavePG = false;
		lStartCn = iReadCout;
		while(lStartCn < nLength)
		{
			if (bufSource[lStartCn] == 'P')
			{
				lStartCn++;
				if (bufSource[lStartCn] == 'U')
				{
					lStartCn++;
					while((IsAlpha(bufSource[lStartCn]) == 0))
					{
						lStartCn++;
					}
					if (bufSource[lStartCn] == 'P')
					{
						lStartCn++;
						if (bufSource[lStartCn] == 'G')
						{
							isHavePG = true;
						}
					}
				}
			}
			else if (bufSource[lStartCn] == 'S')
			{
				lStartCn++;
				if (bufSource[lStartCn] == 'P')
				{
					lStartCn++;
					if (bufSource[lStartCn] != '1')
					{
						break;
					}
				}
			}
			else
			{
				lStartCn++;
			}
		}
		
		if (isHavePG == true)
		{
			lEndCn = nLength - iReadCout;
			fileNew.Write(&bufSource[iReadCout], lEndCn);
			iReadCout = nLength;
		}
		
		//���һ���ֶ��ļ�ĩβ����Զ���ָֽ��
		//label 20150204 rwt
		///*

		if ((cNum == 1) && (lOldMaxXlength * 0.025 > 500))
		{
			AfxMessageBox("��ܷ�ҳ�ߴ糬��(����30~50cm)");
		}
		
		if (iReadCout == nLength)
		{
			char iAutoCutOffPaper[4] = "CT;";
			//fileNew.SeekToEnd();
			//int ii = strlen(iAutoCutOffPaper);
			fileNew.Write(iAutoCutOffPaper, strlen(iAutoCutOffPaper));
		}
		//*/
		fileNew.Close();
		if(cHavePG==1)
		{
			cNum += 1;
			cNum = CheckExistDirFileName(sNewFileName, cNum, sExtName);
			strWorkPath2.Format(_T("%s%sPlot\\%s%s_.%s"),szDrive,szDir,sNewFileName,intToString(cNum),sExtName);
		}
	}
	free(bufSource);

	if(cHavePG==0)
	{
		DeleteFile(sFirstName);
//		g_iNewFileSplitNum = 0;
		return strFileName;
	}
	else
	{
		strWorkPath2.Format(_T("%s%sPlot\\%s00%d_.%s"),szDrive,szDir,sNewFileName,2,sExtName);
		if( !fileNew.Open(strWorkPath2 , CFile::modeRead))
		{
			DeleteFile(sFirstName);
//			g_iNewFileSplitNum = 0;	
			return strFileName;
		}
		else
		{
			fileNew.Close();
			strWorkPath2.Format(_T("%s%sPlot\\%s.%s"),szDrive,szDir,sNewFileName,sExtName);
			if(strcmp(strFileName, strWorkPath2) == 0)
				DeleteFile(strFileName);
//			g_iNewFileSplitNum = cNum;	
		}
	}

	//g_iNewFileSplitNum = cNum;		
	
	//DeleteFile(strFileName);
	return sFirstName;
}

void CPlotCenterDlg::OnOpen() 
{
	// TODO: Add your command handler code here
	char Custer[80]="HP-GL(*.plt)\0*.plt\0All file(*.*)\0*.*\0\0";
	BOOL bWaitConnected;

	bWaitConnected = m_bWaitConnected;
	m_bWaitConnected = false;
	CFileDialog dlg(TRUE);
	dlg.m_ofn.lpstrFilter=Custer;
	if(dlg.DoModal()==IDOK)
	{
		CFile file;
		CString strFileName;
		strFileName=dlg.GetPathName();
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
				
				if(m_lMinPlotLength < 0)
					m_lMinPlotLength = 0;
				dlgPicture.m_lFileMinLength = m_lMinPlotLength;
				
				if(m_lMinPlotWidth < 0)
					m_lMinPlotWidth = 0;
				dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;

				dlgPicture.m_lTotalX = (m_lMaxPlotLength + 10 * 40) * long((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth) - 10 * 40;
				dlgPicture.m_lTotalY = lPaperWidth;
			}
			else
			{
				dlgPicture.m_lFileLength = m_lMaxPlotLength;
				dlgPicture.m_lFileWidth = m_lMaxPlotWidth;

				if(m_lMinPlotLength < 0)
					m_lMinPlotLength = 0;
				dlgPicture.m_lFileMinLength = m_lMinPlotLength;

				if(m_lMinPlotWidth < 0)
					m_lMinPlotWidth = 0;
				dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;

				dlgPicture.m_lTotalX = m_lMaxPlotLength;
				dlgPicture.m_lTotalY = m_lMaxPlotWidth;
			}
			dlgPicture.m_strPictureName = strFileName;
			//AfxMessageBox(strFileName);//20150721 test
			if(dlgPicture.DoModal()==IDOK)
			{
				/*
				{        //20150721 test
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
				}
				*/
				//����ļ� 20140715
				char cExist = 0;
				int cPGExist = 0;
				cPGExist = QuickScanFilePG(strFileName);
				//gzr20161028test
				if(nReturnValue==0 && cPGExist == 0 &&(g_cPrintCutSelect == 3 || g_cPrintCutSelect == 2))
				{
					strFileName = PltFileChangePage(strFileName);
					cPGExist = QuickScanFilePG(strFileName);
					cExist = 1;
					if (cPGExist != 0 && cPGExist != 1)
					{
						cPGExist = 1;
					}
				}
					//ֻ��ӡ���и�ʱ�ļ����ֶ�
				if((cPGExist == 0)&&((g_cPrintCutSelect&0x02) == 0x02))
					//&&(ftoi(m_lMaxPlotLength*0.025) > gl_dPaperLength))	//201502111740rwt//20160113δ��ǰ
				{			
					
					strFileName = ScanFileSection(strFileName, ftoi(m_lMaxPlotLength*0.025));
					//20160128����PG���ļ��ֶ�
                     cExist = 1;
					//20160128����PG���ļ��ֶ�
				}
			
				if((cPGExist == 1)&&((g_cPrintCutSelect == 1))//20160113ֻ��ӡ�ļ����ֶ�
					&&(ftoi(m_lMaxPlotLength*0.025) > gl_dPaperLength))	
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
				}
             
				else if((cPGExist == 1)&&((g_cPrintCutSelect == 3)||(g_cPrintCutSelect == 2)))
				{
					strFileName = NewFileReentrant(strFileName);
					cExist = 1;
				}
			
				else if (cPGExist == 2)			//ET ����ָ��ļ�ҳ��
				{
					strFileName = EtFileSection(strFileName);
					cExist = 1;
				}
				
				/*else
				{
					if(!file.Open(strFileName,CFile::modeReadWrite | CFile::shareExclusive))
						return ;	

					char iAutoCutOffPaper[4] = "CT;";
					file.SeekToEnd();
					file.Write(iAutoCutOffPaper, strlen(iAutoCutOffPaper));
					file.Close();
				}
				*/
				
				if(cExist == 1)
				{
					OpenFile(strFileName);

					if(m_lMinPlotLength < 0)
						m_lMinPlotLength = 0;
					if(m_lMinPlotWidth < 0)
						m_lMinPlotWidth = 0;

					dlgPicture.m_lFileMinLength = m_lMinPlotLength;
					dlgPicture.m_lFileMinWidth = m_lMinPlotWidth;
					dlgPicture.m_lFileLength = m_lMaxPlotLength;
					dlgPicture.m_lFileWidth = m_lMaxPlotWidth;
					dlgPicture.m_lTotalY = m_lMaxPlotWidth;
					dlgPicture.m_lTotalX = m_lMaxPlotLength;
					//cExist = 1;
					
				}
				//*******************
				CString strCaption,strCaption1;
				TCHAR szDir[_MAX_DIR];
				TCHAR szDrive[_MAX_DRIVE];
				TCHAR szFullPath[_MAX_PATH];
				::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
				_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
				strCaption1.Format(_T("%s%sPlot\\"),szDrive,szDir);
				
				if(!cExist)
				{
					_splitpath(strFileName,szDrive,szDir,NULL,NULL);
					strCaption.Format(_T("%s%s"),szDrive,szDir);
				}

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

					if ((lFixedPointLength>=ftoi(m_lMaxPlotLength*0.025))&&(m_lMaxPlotLength!=0))//20160128����&&������
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
						//m_nStartPrint = 1; //20140729
						//m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,true);//20140729
					}
				}
			}
		}
		else if(nReturnValue==1)
		{//�ļ��򲻿�
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING106);
			else
				s1.LoadString(IDS_STRING106_EN);
			AfxMessageBox(s1);
		}
		else
		{//��HP-GL�ļ�!
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
	m_bWaitConnected =bWaitConnected;
}

//bDisplayFlag=true��ʾ����;=false����ʾ����
//����0�������ļ�;=1�ļ�������;=2�ļ���ʽ��
int CPlotCenterDlg::OpenFile(CString strFileName)
{
	CFile file;
	//20100406����CFile::shareExclusive���ԣ���������CAD�ļ�д�Զ���ͼĿ¼ʱд�벻�ɹ����ӡȱҳ
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
			nReturnValue = OpenSecretFile(strFileName);
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
	//20100406����CFile::shareExclusive���ԣ���������CAD�ļ�д�Զ���ͼĿ¼ʱд�벻�ɹ����ӡȱҳ
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
			{//Ѱ����һ��';'
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
			else if (nProcess == PROCESS_MNEMONIC)//���Ƿ�
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
			{//����
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
							//20150212rwt
							g_cTerminator = m_cTerminator;
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

//bDisplayFlag=true��ʾ����;=false����ʾ����
//����0�������ļ�;=1�ļ�������;=2�ļ���ʽ��
int CPlotCenterDlg::OpenSecretFile(CString strFileName)
{
	CFile file;
	//20100406����CFile::shareExclusive���ԣ���������CAD�ļ�д�Զ���ͼĿ¼ʱд�벻�ɹ����ӡȱҳ
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
	//�ж��ļ��Ƿ�Ϊ�����ļ�
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
			if (nProcess == PROCESS_MNEMONIC)//���Ƿ�
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

	// �Ի����Ⱥ͸߶ȵ����� 
	int iIncrementX = cx - m_iClientWidth;
	int iIncrementY = cy - m_iClientHeight;

	// ��С��ʱ����Ϊ0
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
		
		// ��Чֵ
		if ((iPercent < 0) || (iPercent > 100))
			continue;
		
		// �õ��ؼ�ָ��
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
				case MOVEX: // X�����ƶ�
					iLeft += (iIncrementX * iPercent / 100);
					break;
				case MOVEY: // Y�����ƶ�
					iTop += (iIncrementY * iPercent / 100);
					break;
				case MOVEXY: // X�����Y����ͬʱ�ƶ�
					iLeft += (iIncrementX * iPercent / 100);
					iTop += (iIncrementY * iPercent / 100);
					break;
				case ELASTICX: // X����ı��С
					iWidth += (iIncrementX * iPercent / 100);
					break;
				case ELASTICY: // Y����ı��С
					iHeight += (iIncrementY * iPercent / 100);
					break;
				case ELASTICXY: // X�����Y����ͬʱ�ı��С
					iWidth += (iIncrementX * iPercent / 100);
					iHeight += (iIncrementY * iPercent / 100);
					break;
				default:
					break;
			}

			// �ѿؼ��ƶ�����λ��
			pWndCtrl->MoveWindow(iLeft, iTop, iWidth, iHeight);
		}
	}

	// ��ͼ���ƶ����Ի������½�
	if (IsWindow(m_wndSizeIcon.GetSafeHwnd()))
		m_wndSizeIcon.MoveWindow(cx - m_bitmap.bmWidth, cy - m_bitmap.bmHeight, m_bitmap.bmWidth, m_bitmap.bmHeight);

	// ��¼�Ի���client����Ĵ�С
	if (nType != SIZE_MINIMIZED)
	{
		m_iClientWidth = cx;
		m_iClientHeight = cy;
	}
}

void CPlotCenterDlg::OnSizing(UINT nSide, LPRECT lpRect)
{
	CDialog::OnSizing(nSide, lpRect);

	// �Ի�����С�ڳ�ʼ��С

	int iWidth = lpRect->right - lpRect->left;
	int iHeight = lpRect->bottom - lpRect->top;

	if (iWidth <= m_iMinWidth)
		lpRect->right = lpRect->left + m_iMinWidth;

	if(iHeight <= m_iMinHeight)
		lpRect->bottom = lpRect->top + m_iMinHeight;
}

BOOL CPlotCenterDlg::SetControlProperty(PDLGCTLINFO lp, int nElements)
{	
	// ���ÿؼ�������Ϣ

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
/*
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
		theApp.m_dGapDown=SetupPictureDlg.m_dGapDown;
		theApp.m_dGapRight=SetupPictureDlg.m_dGapRight;
		theApp.m_dMarkSpace=SetupPictureDlg.m_dMark;
		theApp.m_dPaperWidth=SetupPictureDlg.m_dPaperWidth;
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
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
			//20100413 ������ֽ���
			//�������Ƿ�����ֽ���20100413
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
					
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
	SetupPlotterDlg.m_dMalposedError = m_dMalposedError;
	SetupPlotterDlg.m_dOverlapError = m_dOverlapError;//20100413 ��ֽ������ֵ����
	SetupPlotterDlg.m_sSprayErrorX12 = m_sSprayErrorX12;
	SetupPlotterDlg.m_sSprayErrorX23 = m_sSprayErrorX23;
	SetupPlotterDlg.m_sSprayErrorX34 = m_sSprayErrorX34;
	SetupPlotterDlg.m_sSprayErrorY12 = m_sSprayErrorY12;
	SetupPlotterDlg.m_sSprayErrorY23 = m_sSprayErrorY23;
	SetupPlotterDlg.m_sSprayErrorY34 = m_sSprayErrorY34;
	SetupPlotterDlg.m_dPlotterXError = m_dPlotterXError;
	SetupPlotterDlg.m_dPlotterYError = m_dPlotterYError;
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
			//���ô�ӡģʽ
			strcpy(SendBuf,"Msg_Comm");
			SendBuf[8]=0;
			SendBuf[9]=12;
			SendBuf[10]=0;
			SendBuf[11]=SetupPlotterDlg.m_cPrintStyle;
			if (m_nCommunicateMode == 0)
			{
				mLength = 12;
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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

			//������ͷѡ��
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
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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

			//������ͷ�����
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
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
			
			//����˫���ӡ���
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
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
			m_dMalposedError = SetupPlotterDlg.m_dMalposedError;
			
			//����ͼ�����
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
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
			//��ֽ�������׼��20100413
			//�������Ƿ�����ֽ���20100413 1:�������>= 3.24
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
					
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
				m_dOverlapError = SetupPlotterDlg.m_dOverlapError;//20100413��ֽ���
				m_dMalposedError = SetupPlotterDlg.m_dMalposedError;
				m_sSprayErrorX12 = SetupPlotterDlg.m_sSprayErrorX12;
				m_sSprayErrorX23 = SetupPlotterDlg.m_sSprayErrorX23;
				m_sSprayErrorX34 = SetupPlotterDlg.m_sSprayErrorX34;
				m_sSprayErrorY12 = SetupPlotterDlg.m_sSprayErrorY12;
				m_sSprayErrorY23 = SetupPlotterDlg.m_sSprayErrorY23;
				m_sSprayErrorY34 = SetupPlotterDlg.m_sSprayErrorY34;
				m_dPlotterXError = SetupPlotterDlg.m_dPlotterXError;
				m_dPlotterYError = SetupPlotterDlg.m_dPlotterYError;
// 20120802
			m_cPenNo = SetupPlotterDlg.m_cPenNo;//20101206 �ʺŵ���
			m_cCutNo = SetupPlotterDlg.m_cCutNo;
			g_cPenNo = m_cPenNo;
			g_cCutNo = m_cCutNo;
			
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
*/

//   �и����������  20120725
void CPlotCenterDlg::OnSetupPicture() 
{
	// TODO: Add your command handler code here

	m_bTimerBusy=false;
	LinkAndReadPara(false);

	CSetupPicture SetupPictureDlg;
	SetupPictureDlg.m_nLineWidth = m_nLineWidth;
	SetupPictureDlg.m_bConnected = m_bConnected;
	SetupPictureDlg.m_bNetworkBusy = m_bNetworkBusy;
	if (SetupPictureDlg.DoModal() == IDOK)
	{
		theApp.m_dGapDown=SetupPictureDlg.m_dGapDown;
		theApp.m_dGapRight=SetupPictureDlg.m_dGapRight;
		theApp.m_dMarkSpace=SetupPictureDlg.m_dMark;
		theApp.m_dPaperWidth=SetupPictureDlg.m_dPaperWidth;
		theApp.m_dPaperLength=SetupPictureDlg.m_dPaperLength;
		gl_dPaperLength = SetupPictureDlg.m_dPaperLength;
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

			strcpy(SendBuf,"Msg_Comm");// if (m_nVersion == 0)
			SendBuf[8]=0;
			SendBuf[9]=3;
			SendBuf[10]=0;
			SendBuf[11]=(char)SetupPictureDlg.m_nLineWidth;
			if (m_nCommunicateMode == 0)
			{
				mLength = 12;
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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

			sSprayError=m_sSprayErrorY12;
			SendBuf[20]=*(cPin+1);
			SendBuf[21]=*cPin;
			sSprayError=m_sSprayErrorY23;
			SendBuf[22]=*(cPin+1);
			SendBuf[23]=*cPin;
			sSprayError=m_sSprayErrorY34;
			SendBuf[24]=*(cPin+1);
			SendBuf[25]=*cPin;
			//��ֽ�������׼��20100413
			//�������Ƿ�����ֽ���20100413 1:�������>= 3.24
			if(g_nNewVersion == 1)//#1 20100413
				dPlotterError = m_dOverlapError;
			else
				dPlotterError = m_dMalposedError;
			cPin = (unsigned char *)&dPlotterError;
			for(i=26;i<34;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}

			if(g_nNewVersion == 1)//#2 20100413
				dPlotterError = m_dMalposedError;
			else
				dPlotterError = m_dPlotterXError;			
			cPin = (unsigned char *)&dPlotterError;
			for(;i<42;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}

			if(g_nNewVersion == 1)//#3 20100413
				dPlotterError = m_dPlotterXError;
			else
				dPlotterError = m_dPlotterYError;
			cPin = (unsigned char *)&dPlotterError;
			for(;i<50;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			
			if(g_nNewVersion == 1)//#4 20100413
			{
				dPlotterError = m_dPlotterYError;
				cPin = (unsigned char *)&dPlotterError;
				for(;i<58;i++)
				{
					SendBuf[i]=*cPin;
					cPin++;
				}	
			}
			
			//20101206 �ʺŵ���
			SendBuf[i++]=0;
			SendBuf[i++]=m_cPenNo;
			SendBuf[i++]=0;
			SendBuf[i++]=m_cCutNo;
			SendBuf[i++]=0;
			SendBuf[i++]=g_cPrintCutSelect&0x03;//20101206 ��ӡ�и�ѡ��
			SendBuf[i++]=0;

			if (m_nCommunicateMode == 0)
			{
				if(g_nNewVersion == 1)//#5 20100413
					mLength = 65;
				else
					mLength = 60;
					
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
				}
			}
			else
			{
				if(g_nNewVersion == 1)//#6 20100413
					send(m_socket,SendBuf,65,0);
				else
					send(m_socket,SendBuf,60,0);					
				
				recv(m_socket,SendBuf,30,0);
			}
			
			if (SendBuf[7] == 9)
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING293);
				else
					s1.LoadString(IDS_STRING293_EN);
				AfxMessageBox(s1);
				return;
			}
		
		}
/*
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
			//20100413 ������ֽ���
			//�������Ƿ�����ֽ���20100413
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
			//20101206 �ʺŵ���
			SendBuf[i++]=m_cPenNo;
			SendBuf[i++]=0;
			SendBuf[i++]=m_cCutNo;
			SendBuf[i++]=0;
			SendBuf[i++]=g_cPrintCutSelect; //20101206 ��ӡ�и�ѡ��
			SendBuf[i++]=0;
			
			if (m_nCommunicateMode == 0)
			{
				if(g_nNewVersion == 1)//#5 20100413
					mLength = 66;
				else
					mLength = 58;
					
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
				}
			}
			else
			{
				if(g_nNewVersion == 1)//#6 20100413
					send(m_socket,SendBuf,66,0);
				else
					send(m_socket,SendBuf,58,0);					
				
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
		*/
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING250);
		else
			s1.LoadString(IDS_STRING250_EN);
		AfxMessageBox(s1);
	}
	m_bTimerBusy=true;
}

void CPlotCenterDlg::OnSetupPlotter() 
{
	// TODO: Add your command handler code here

	m_bTimerBusy=false;
	

	LinkAndReadPara(false);
	if(m_bConnected == false)
		return;
	CSetupPlotter SetupPlotterDlg;
	SetupPlotterDlg.m_pParent=this;
	SetupPlotterDlg.m_cPrintStyle = m_cPrintStyle;
	SetupPlotterDlg.m_cSprinklerCount = m_cSprinklerCount;
	SetupPlotterDlg.m_cSprinklerNo = m_cSprinklerNo;
	SetupPlotterDlg.m_dMalposedError = m_dMalposedError;
	SetupPlotterDlg.m_dOverlapError = m_dOverlapError;//20100413 ��ֽ������ֵ����
	SetupPlotterDlg.m_sSprayErrorX12 = m_sSprayErrorX12;
	//SetupPlotterDlg.m_sSprayErrorX23 = m_sSprayErrorX23;
	SetupPlotterDlg.m_sSprayErrorX34 = m_sSprayErrorX34;
	SetupPlotterDlg.m_sSprayErrorY12 = m_sSprayErrorY12;
	SetupPlotterDlg.m_sSprayErrorY23 = m_sSprayErrorY23;
	SetupPlotterDlg.m_sSprayErrorY34 = m_sSprayErrorY34;
	SetupPlotterDlg.m_dPlotterXError = m_dPlotterXError;
	SetupPlotterDlg.m_dPlotterYError = m_dPlotterYError;
	SetupPlotterDlg.m_cPenNo = m_cPenNo;//20101206 �ʺ� ���� 
	SetupPlotterDlg.m_cCutNo = m_cCutNo;
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
			//���ô�ӡģʽ
			strcpy(SendBuf,"Msg_Comm");// if (m_nVersion == 0)
			SendBuf[8]=0;
			SendBuf[9]=12;
			SendBuf[10]=0;
			SendBuf[11]=SetupPlotterDlg.m_cPrintStyle;
			if (m_nCommunicateMode == 0)
			{
				mLength = 12;
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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

			//������ͷѡ��
			strcpy(SendBuf,"Msg_Comm");// if (m_nVersion == 0)
			SendBuf[8]=0;
			SendBuf[9]=13;
			SendBuf[10]=0;
			SendBuf[11]=SetupPlotterDlg.m_cSprinklerCount;
			SendBuf[12]=0;
			SendBuf[13]=SetupPlotterDlg.m_cSprinklerNo;
			if (m_nCommunicateMode == 0)
			{
				mLength = 14;
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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

			//������ͷ�����
			strcpy(SendBuf,"Msg_Comm");// if (m_nVersion == 0)
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
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
			
			//����˫���ӡ���
			strcpy(SendBuf,"Msg_Comm");// if (m_nVersion == 0)
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
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
			m_dMalposedError = SetupPlotterDlg.m_dMalposedError;
			
			//����ͼ�����
			strcpy(SendBuf,"Msg_Comm");// if (m_nVersion == 0)
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
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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

			sSprayError=SetupPlotterDlg.m_sSprayErrorY12;
			SendBuf[20]=*(cPin+1);
			SendBuf[21]=*cPin;
			sSprayError=SetupPlotterDlg.m_sSprayErrorY23;
			SendBuf[22]=*(cPin+1);
			SendBuf[23]=*cPin;
			sSprayError=SetupPlotterDlg.m_sSprayErrorY34;
			SendBuf[24]=*(cPin+1);
			SendBuf[25]=*cPin;
			//��ֽ�������׼��20100413
			//�������Ƿ�����ֽ���20100413 1:�������>= 3.24
			if(g_nNewVersion == 1)//#1 20100413
				dPlotterError = SetupPlotterDlg.m_dOverlapError;
			else
				dPlotterError = SetupPlotterDlg.m_dMalposedError;
			cPin = (unsigned char *)&dPlotterError;
			for(i=26;i<34;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}

			if(g_nNewVersion == 1)//#2 20100413
				dPlotterError = SetupPlotterDlg.m_dMalposedError;
			else
				dPlotterError = SetupPlotterDlg.m_dPlotterXError;			
			cPin = (unsigned char *)&dPlotterError;
			for(;i<42;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}

			if(g_nNewVersion == 1)//#3 20100413
				dPlotterError = SetupPlotterDlg.m_dPlotterXError;
			else
				dPlotterError = SetupPlotterDlg.m_dPlotterYError;
			cPin = (unsigned char *)&dPlotterError;
			for(;i<50;i++)
			{
				SendBuf[i]=*cPin;
				cPin++;
			}
			
			if(g_nNewVersion == 1)//#4 20100413
			{
				dPlotterError = SetupPlotterDlg.m_dPlotterYError;
				cPin = (unsigned char *)&dPlotterError;
				for(;i<58;i++)
				{
					SendBuf[i]=*cPin;
					cPin++;
				}	
			}
			
			//20101206 �ʺŵ���
			SendBuf[i++]=0;
			SendBuf[i++]=SetupPlotterDlg.m_cPenNo;
			SendBuf[i++]=0;
			SendBuf[i++]=SetupPlotterDlg.m_cCutNo;
			SendBuf[i++]=0;
			SendBuf[i++]=g_cPrintCutSelect&0x03;//20101206 ��ӡ�и�ѡ��
			SendBuf[i++]=0;

			if (m_nCommunicateMode == 0)
			{
				if(g_nNewVersion == 1)//#5 20100413
					mLength = 65;
				else
					mLength = 60;
					
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
				}
			}
			else
			{
				if(g_nNewVersion == 1)//#6 20100413
					send(m_socket,SendBuf,65,0);
				else
					send(m_socket,SendBuf,60,0);					
				
				recv(m_socket,SendBuf,30,0);
			}
			
			if (SendBuf[7] == 9)
			{
				m_cPrintStyle = SetupPlotterDlg.m_cPrintStyle;
				m_cSprinklerCount = SetupPlotterDlg.m_cSprinklerCount;
				m_cSprinklerNo = SetupPlotterDlg.m_cSprinklerNo;
				m_dOverlapError = SetupPlotterDlg.m_dOverlapError;//20100413��ֽ���
				m_cPenNo = SetupPlotterDlg.m_cPenNo;//20101206 �ʺŵ���
				m_cCutNo = SetupPlotterDlg.m_cCutNo;
				g_cPenNo = m_cPenNo;
				g_cCutNo = m_cCutNo;
				
				m_dMalposedError = SetupPlotterDlg.m_dMalposedError;
				m_sSprayErrorX12 = SetupPlotterDlg.m_sSprayErrorX12;
				m_sSprayErrorX23 = SetupPlotterDlg.m_sSprayErrorX23;
				m_sSprayErrorX34 = SetupPlotterDlg.m_sSprayErrorX34;
				m_sSprayErrorY12 = SetupPlotterDlg.m_sSprayErrorY12;
				m_sSprayErrorY23 = SetupPlotterDlg.m_sSprayErrorY23;
				m_sSprayErrorY34 = SetupPlotterDlg.m_sSprayErrorY34;
				m_dPlotterXError = SetupPlotterDlg.m_dPlotterXError;
				m_dPlotterYError = SetupPlotterDlg.m_dPlotterYError;
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
	m_bTimerBusy=true;
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
	CPoint dp,dp1;
	POSITION pos=NULL;
	double d1;
	double d2;
	
	if(strcmp(pInstruct->m_cName,"PD")==0)//���
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
					dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
					dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y;
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
				//20150729
				if (m_nPen == g_cPenNo)
				{
					p->m_nType=PRIMITIVE_LINE;
				}
				else if (m_nPen != g_cPenNo)
				{
					p->m_nType=PRIMITIVE_CUTLINE;
				}
				//p->m_nType=PRIMITIVE_LINE;//20150729
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
						//2
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
	else if(strcmp(pInstruct->m_cName,"PU")==0)//���
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			if (m_bAbsolute)
			{
				//1
				dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
				dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y;
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
	else if(strcmp(pInstruct->m_cName,"PR")==0)//��Ի�ͼ
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
	else if(strcmp(pInstruct->m_cName,"PA")==0)//���Ի�ͼ
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
	/*	else if(strcmp(pInstruct->m_cName,"CI")==0)//Բ
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
		//�յ㲻��
	}
	else if(strcmp(pInstruct->m_cName,"EA")==0)//���Ծ���
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
		//�յ㲻��
	}
	else if(strcmp(pInstruct->m_cName,"ER")==0)//��Ծ���
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
		//�յ㲻��
	}
	else if(strcmp(pInstruct->m_cName,"AT")==0)//3�㻭��
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
				else//���㹲��
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
					else //�������
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
			//�����յ�
			m_dpCurrent=dp1;
		}
	}
	else if(strcmp(pInstruct->m_cName,"RT")==0)//���3�㻭��
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
				else//���㹲��
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
					else //�������
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
			//�����յ�
			m_dpCurrent=dp1;
		}
	}
	else if(strcmp(pInstruct->m_cName,"AR")==0)//��Ի���
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
			//�����յ�
			double dAngle=GetAngle(dp0,m_dpCurrent);
			dAngle+=pInstruct->m_alPara.GetAt(3)*PAI/180;
			double r=GetDistance(dp0,m_dpCurrent);
			m_dpCurrent.y=r*sin(dAngle)+dp0.x;
			m_dpCurrent.x=r*cos(dAngle)+dp0.y;
		}
	}
	else if(strcmp(pInstruct->m_cName,"AA")==0)//���Ի���
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
			//�����յ�
			double dAngle=GetAngle(dp0,m_dpCurrent);
			dAngle+=pInstruct->m_alPara.GetAt(3)*PAI/180;
			double r=GetDistance(dp0,m_dpCurrent);
			m_dpCurrent.y=r*sin(dAngle)+dp0.x;
			m_dpCurrent.x=r*cos(dAngle)+dp0.y;
		}
	}*/
	else if((strcmp(pInstruct->m_cName,"PG")==0)||(strcmp(pInstruct->m_cName,"FR")==0))//�ƽ�ҳ��
	{
		m_lMaxPlotLength=m_lMaxPGLength;
		if (m_lMaxPlotWidth < m_lMaxPGWidth)
			m_lMaxPlotWidth = m_lMaxPGWidth;
	}
	else if(strcmp(pInstruct->m_cName,"LT")==0)//����
	{
		nSize=pInstruct->m_adPara.GetSize();
		p=new CGLPrimitive;
		theApp.m_listPrimitive.AddTail(p);
		p->m_nType=PRIMITIVE_LT;
		for(i=0;i<nSize;i++)
			p->m_anData.Add(ftoi(pInstruct->m_adPara.GetAt(i)));
	}
	else if(strcmp(pInstruct->m_cName,"SP")==0)//ѡ��
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
	else if(strcmp(pInstruct->m_cName,"LO")==0)//�ַ����ԭ��λ��
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
	else if(strcmp(pInstruct->m_cName,"SI")==0)//�ַ���С
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
	else if(strcmp(pInstruct->m_cName,"DI")==0)//�ַ�����
	{
		//gzr20161102DI�����޸�
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			d1=pInstruct->m_adPara.GetAt(0);
			d2=pInstruct->m_adPara.GetAt(1);
			if((d1==0)&&(d2!=0))
			{
				//gzr20161102����ѡת�Ƕ�
				if (d2 > 0)
					m_dTextAngle=90;
				else
					m_dTextAngle=-90;
			}
			else if((d1!=0)&&(d2==0))
			{
				if(d1 > 0)
					m_dTextAngle=0;
				else
					m_dTextAngle=180;
			}
			else if((d1!=0)&&(d2!=0))
			{
				//gzr20161102����ѡת�Ƕ�
				m_dTextAngle=atan2(d2,d1) * 180 / PAI;
			}
		}
		else
			m_dTextAngle=0;
		if (theApp.m_nFont==0)
		{
			pText=new CGLText;
			theApp.m_listTextPrimitive.AddTail(pText);
			pText->m_nType=PRIMITIVE_DI;
			for(i=0;i<nSize;i++)
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(i));
		}
	}
	else if(strcmp(pInstruct->m_cName,"DR")==0)//�ַ�����
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
	else if(strcmp(pInstruct->m_cName,"LB")==0)//�ַ�
	{	
		nSize=m_strText.GetLength();
		int j=0;
		unsigned char str1[257];
		CPoint dp0;
		dp=m_dpCurrent;
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
	else if(strcmp(pInstruct->m_cName,"DF")==0)//ȱʡ
	{
		m_nLineType=LINE_TYPE_SOLID;
		m_bAbsolute=TRUE;
		m_cTerminator=theApp.m_cTerminator;//��־�ս��
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
	else if(strcmp(pInstruct->m_cName,"IN")==0)//��ʼ��
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
		m_cTerminator=theApp.m_cTerminator;//��־�ս��
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
	else if(strcmp(pInstruct->m_cName,"PS")==0)//��ͼ�ߴ�
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
	else if(strcmp(pInstruct->m_cName,"IP")==0)//���ŵ�
	{
		//return;
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
	else if(strcmp(pInstruct->m_cName,"IR")==0)//������ŵ�
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
	else if(strcmp(pInstruct->m_cName,"SC")==0)//����
	{
		//return;
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
/*	else if(strcmp(pInstruct->m_cName,"IW")==0)//�������
	{
		
	}*/
/*	else if(strcmp(pInstruct->m_cName,"RP")==0)//���»�ͼ
	{
	}
	else if(strcmp(pInstruct->m_cName,"RO")==0)//��ת�Ƕ�
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

int CPlotCenterDlg::GetParameterType(char cName[])
{
	int nType=-1;

	if(strcmp(cName,"DF")==0) nType=NONE_TYPE;//ȱʡ
	else if(strcmp(cName,"IN")==0) nType=DIGIT_TYPE;//��ʼ��
	else if(strcmp(cName,"IP")==0) nType=DIGIT_TYPE;//���ŵ�
	else if(strcmp(cName,"IR")==0) nType=DIGIT_TYPE;//������ŵ�
	else if(strcmp(cName,"SC")==0) nType=DIGIT_TYPE;//����
	else if(strcmp(cName,"PG")==0) nType=DIGIT_TYPE;//�ƽ�ҳ��
	else if(strcmp(cName,"FR")==0) nType=DIGIT_TYPE;//�ƽ�ҳ��
	else if(strcmp(cName,"PA")==0) nType=DIGIT_TYPE;//���Ի�ͼ
	else if(strcmp(cName,"PD")==0) nType=DIGIT_TYPE;//���
	else if(strcmp(cName,"PU")==0) nType=DIGIT_TYPE;//���
	else if(strcmp(cName,"PE")==0) nType=PE_TYPE;//���߱���
	else if(strcmp(cName,"SP")==0) nType=DIGIT_TYPE;//ѡ��
	else if(strcmp(cName,"SS")==0) nType=NONE_TYPE;//ѡ���׼����
	else if(strcmp(cName,"DI")==0) nType=DIGIT_TYPE;//���Է���------------
	else if(strcmp(cName,"DR")==0) nType=DIGIT_TYPE;//��Է���
	else if(strcmp(cName,"SI")==0) nType=DIGIT_TYPE;//�����ַ��ߴ�
	else if(strcmp(cName,"LO")==0) nType=DIGIT_TYPE;//��־ԭ��172
	else if(strcmp(cName,"DT")==0) nType=DT_TYPE;//�����־�ս��
	else if(strcmp(cName,"LB")==0)
	{ 
		nType=STRING_TYPE;//��־171
		m_strText.Empty();
	}
	else if(strcmp(cName,"LT")==0) nType=DIGIT_TYPE;//����119
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
		case 0://�첽
			m_dRatioX=(m_dpP2.x-m_dpP1.x)/(m_struSC.dXMax-m_struSC.dXMin);
			m_dRatioY=(m_dpP2.y-m_dpP1.y)/(m_struSC.dYMax-m_struSC.dYMin);
			m_dpOrigin.x=ftoi(m_dpP1.x-m_struSC.dXMin*m_dRatioX);
			m_dpOrigin.y=ftoi(m_dpP1.y-m_struSC.dYMin*m_dRatioY);
			break;
		case 1://ͬ��
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
		case 2://������
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
		int i=0;
		//m_lMaxPGLength=ftoi(dp0.x);
	else if (dp0.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp0.x);
	if(dp0.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp0.y);
	else if(dp0.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp0.y);
	dp.x=dp0.x+(nLen*m_dTextWidth*400)*cos(dTextAngle);
	dp.y=dp0.y+(nLen*m_dTextWidth*400)*sin(dTextAngle);
	if(dp.x>m_lMaxPGLength)
		int i=0;//m_lMaxPGLength=ftoi(dp.x);
	else if (dp.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp.x);
	if(dp.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp.y);
	else if(dp.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp.y);
	dp.x=dp.x+(m_dTextHeight*400)*sin(dTextAngle);
	dp.y=dp.y-(m_dTextHeight*400)*cos(dTextAngle);
	if(dp.x>m_lMaxPGLength)
		int i=0;//m_lMaxPGLength=ftoi(dp.x);
	else if (dp.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp.x);
	if(dp.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp.y);
	else if(dp.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp.y);
	dp.x=dp0.x+(m_dTextHeight*400)*sin(dTextAngle);
	dp.y=dp0.y-(m_dTextHeight*400)*cos(dTextAngle);
	if(dp.x>m_lMaxPGLength)
		int i=0;//m_lMaxPGLength=ftoi(dp.x);
	else if (dp.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp.x);
	if(dp.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp.y);
	else if(dp.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp.y);
}

//20140609
extern CString GetOldFileName(CString FileName, CString &ExtendName);
extern CString GetOldFilePath(CString FileName);
void CPlotCenterDlg::SortByTitle()
{
	CString tempbuff;
	CPlotItem *pPlotItem1;
	CPlotItem *pPlotItem2;
	CPlotItem *pPlotItem3;
	CPlotItem *pPlotItem4;
	POSITION pos1,pos2,pos3,pos4;
	CObList  cListTempPlotItem;
	CString csStrTemp, ExtendName, sNewFileName, sFilePath1, sFilePath2;

	int nItemCount=0;
	int i,j;
	int iSortStart=0;
	
	nItemCount = m_listPlotItem.GetCount();
	if (nItemCount < 2 )
		return;

	pos1=m_listPlotItem.FindIndex(0);
	pPlotItem1 = (CPlotItem*)m_listPlotItem.GetAt(pos1);
	if( 1 == pPlotItem1->m_nState)
		return;
	
	sFilePath1 = GetOldFilePath(pPlotItem1->m_strPlotName);
	pos2=m_listPlotItem.FindIndex(nItemCount-1);
	pPlotItem2 = (CPlotItem*)m_listPlotItem.GetAt(pos2);
	sFilePath2 = GetOldFilePath(pPlotItem2->m_strPlotName);
	if( strcmp(sFilePath1, sFilePath2) != 0 )
		return;

	
	for(i=nItemCount-1; i>0; i--)
	{
		pos2=m_listPlotItem.FindIndex(i);
		pPlotItem2 = (CPlotItem*)m_listPlotItem.GetAt(pos2);
		sFilePath2 = GetOldFilePath(pPlotItem2->m_strPlotName);
		if( strcmp(sFilePath1, sFilePath2) != 0 )
		{
			//m_listPlotItem.RemoveAt(pos2);
			pos2 = m_listPlotItem.FindIndex(1);
			pPlotItem2 = (CPlotItem*)m_listPlotItem.GetAt(pos2);

			pos3 = pos1;
			pPlotItem3 = (CPlotItem*)m_listPlotItem.GetAt(pos3);

			m_listPlotItem.GetNext(pos1);
			pPlotItem1 = (CPlotItem*)m_listPlotItem.GetAt(pos1);

			for(j=nItemCount-i; j<nItemCount; j++)
			{
				pos4=m_listPlotItem.FindIndex(j);
				pPlotItem4 = (CPlotItem*)m_listPlotItem.GetAt(pos4);
				pos3 = m_listPlotItem.InsertAfter(pos3,pPlotItem4);
				m_listPlotItem.RemoveAt(pos4);
			}
			m_listPlotItem.InsertAfter(pos3,pPlotItem2);
			m_listPlotItem.RemoveAt(pos2);
			i=0;
		}
		//else
			//m_listPlotItem.RemoveAt(pos2);
	}
	
	nItemCount = m_listPlotItem.GetCount();
	for(i=0; i<nItemCount; i++)
	{
		pos1=m_listPlotItem.FindIndex(i);
		pPlotItem1 = (CPlotItem*)m_listPlotItem.GetAt(pos1);
		//m_listPlotItem.RemoveAt(pos2);
	}
}



void CPlotCenterDlg::FillListCtrl(int nSelect)
{
	LV_ITEM lvitem;
  int nItem;
  CString strItem1;
  POSITION pos1;
  CPlotItem *pPlotItem;
  TCHAR szFName[_MAX_FNAME];
//  TCHAR szExt[_MAX_EXT];
	int nTotalFile=0;
	long lTotalLength=0;

  m_wndPlotItem.DeleteAllItems();

  SortByTitle();

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
    _splitpath(strItem1,NULL,NULL,szFName,NULL);
		strItem1.Format(_T("%s"),szFName);
		strItem1=strItem1 + _T(".plt");
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
    m_wndPlotItem.InsertItem(&lvitem);

    lvitem.iSubItem=1;
		strItem1.Format("%ldmm",pPlotItem->m_lPictureLength);
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		m_wndPlotItem.SetItem(&lvitem);

    lvitem.iSubItem=2;
		strItem1.Format("%ldmm",pPlotItem->m_lPictureWidth);
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		m_wndPlotItem.SetItem(&lvitem);

		lvitem.iSubItem=3;
		strItem1.Format("%ldmm",pPlotItem->m_lFixedPointLength);
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		m_wndPlotItem.SetItem(&lvitem);

		lvitem.iSubItem=4;
		strItem1.Format("%ldmm",pPlotItem->m_lSendLength);
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		m_wndPlotItem.SetItem(&lvitem);

		lvitem.iSubItem=5;
		strItem1.Format("%d",pPlotItem->m_nCopyNum);
		lvitem.pszText=strItem1.GetBuffer(0);
    lvitem.iImage=0;
		m_wndPlotItem.SetItem(&lvitem);

		lvitem.iSubItem=6;
		if (m_nLanguage == 0)
		{
			switch (pPlotItem->m_nState)
			{
				case 0:
				case 3:
				case 4:
					strItem1.LoadString(IDS_PDSTRANSMIT);
					break;
				case 1:
					strItem1.LoadString(IDS_PDSWAIT);
					break;
				case 2:
				case 5:
				case 6:
					  strItem1.LoadString(IDS_PDSPAUSE);
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
    nItem++;
  }
  if(nSelect>=0)
  {
		m_wndPlotItem.SetItemState(nSelect,LVIS_SELECTED,LVIS_SELECTED);
  }
  m_wndPlotItem.SetFocus();
	m_strTotalFile.Format("%d",nTotalFile);
	m_strTotalLength.Format("%ldmm",lTotalLength);

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
	m_dRatioX=1;//��ǰ�����ű���
	m_dRatioY=1;//��ǰ�����ű���
	m_nRotate=0;
	m_dpCurrent=CPoint(0,0);
	m_bPenDown=FALSE;
	m_nLineType=LINE_TYPE_SOLID;
	m_bAbsolute=TRUE;
	m_nPen=1;
	m_cTerminator=theApp.m_cTerminator;//��־�ս��
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
//extern int g_iNewFileSplitNum;
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
		{//20140708
//			g_iNewFileSplitNum = 0;
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
				DeleteCutInfo(pPlotItem->m_strPlotName);
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
	{  //�ļ�����,�ر�ͨ�ſ�
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
				CH375CloseDevice(0);
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
	m_socket=socket(AF_INET,SOCK_STREAM,0);
	if(INVALID_SOCKET != m_socket)
	{	
		SOCKADDR_IN addrSock;
		addrSock.sin_family=AF_INET;
		addrSock.sin_port=htons(1024);
		addrSock.sin_addr.S_un.S_addr=htonl(theApp.m_dwRemoteIP);

		struct timeval timeout;
		fd_set set;
		unsigned long ul = 1;
	
		retval = ioctlsocket(m_socket,FIONBIO,&ul); //����Ϊ������ģʽ
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
				retval = ioctlsocket(m_socket,FIONBIO,&ul); //����Ϊ����ģʽ
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
	for (i=0;i<7;i++)
		m_wndPlotItem.DeleteColumn(0);
	if (m_nLanguage == 0)
	{
		strItem1.LoadString(IDS_PLOTCENTERNAME);
		AfxGetMainWnd()->SetWindowText(strItem1);
		SetMenu(&m_chinesemenu);

		GetMenu()->GetSubMenu(2)->CheckMenuItem(ID_SIMPLIFIEDCHINESE,MF_CHECKED);
		strItem1.LoadString(IDS_COLUMN0);
		m_wndPlotItem.InsertColumn(0,strItem1,LVCFMT_LEFT,rect1.Width()*5/21,0);
		strItem1.LoadString(IDS_COLUMN1);
		m_wndPlotItem.InsertColumn(1,strItem1,LVCFMT_LEFT,rect1.Width()*3/21,0);
		strItem1.LoadString(IDS_COLUMN2);
		m_wndPlotItem.InsertColumn(2,strItem1,LVCFMT_LEFT,rect1.Width()*3/21,0);
		strItem1.LoadString(IDS_COLUMN3);
		m_wndPlotItem.InsertColumn(3,strItem1,LVCFMT_LEFT,rect1.Width()*3/21,0);
		strItem1.LoadString(IDS_COLUMN4);
		m_wndPlotItem.InsertColumn(4,strItem1,LVCFMT_LEFT,rect1.Width()*3/21,0);
		strItem1.LoadString(IDS_COLUMN5);
		m_wndPlotItem.InsertColumn(5,strItem1,LVCFMT_LEFT,rect1.Width()*2/21,0);
		strItem1.LoadString(IDS_COLUMN6);
		m_wndPlotItem.InsertColumn(6,strItem1,LVCFMT_LEFT,rect1.Width()*2/21,1);
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
		m_wndPlotItem.InsertColumn(0,strItem1,LVCFMT_LEFT,rect1.Width()*5/21,0);
		strItem1.LoadString(IDS_COLUMN1_EN);
		m_wndPlotItem.InsertColumn(1,strItem1,LVCFMT_LEFT,rect1.Width()*3/21,0);
		strItem1.LoadString(IDS_COLUMN2_EN);
		m_wndPlotItem.InsertColumn(2,strItem1,LVCFMT_LEFT,rect1.Width()*3/21,0);
		strItem1.LoadString(IDS_COLUMN3_EN);
		m_wndPlotItem.InsertColumn(3,strItem1,LVCFMT_LEFT,rect1.Width()*3/21,0);
		strItem1.LoadString(IDS_COLUMN4_EN);
		m_wndPlotItem.InsertColumn(4,strItem1,LVCFMT_LEFT,rect1.Width()*3/21,0);
		strItem1.LoadString(IDS_COLUMN5_EN);
		m_wndPlotItem.InsertColumn(5,strItem1,LVCFMT_LEFT,rect1.Width()*2/21,0);
		strItem1.LoadString(IDS_COLUMN6_EN);
		m_wndPlotItem.InsertColumn(6,strItem1,LVCFMT_LEFT,rect1.Width()*2/21,1);
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
	
	if (m_bTimerBusy)
	{
		m_bTimerBusy=false;

		if (m_bConnected)
		{
			//�������״̬ gzr20160902
			if (m_nCommunicateMode == 0)
			{
				//m_bWaitConnected = TRUE;
				unsigned long lUsb_Pid_Vid;
				lUsb_Pid_Vid = CH375GetUsbID(0);
				//if (lUsb_Pid_Vid != USB_PID_VID)
				
				if ((lUsb_Pid_Vid == 0))
				{
					if (breakLinkTimes > 10)
					{
						OnBreak();
					}
					else
					{
						breakLinkTimes++;
					}
				}
				else
				{
					breakLinkTimes = 0;
				}
			}	
			else
			{
				DWORD dw;
				if(!IsNetworkAlive(&dw))  
				{
					if (breakLinkTimes > 10)
					{
						OnBreak();
					}
					else
					{
						breakLinkTimes++;
					}
				}
				
				else
				{
					breakLinkTimes = 0;
				}
			}
			//�������״̬ gzr20160902
		}

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
 			SearchNewJob();
		CheckWorkStatus();
		StartAJob();

		//DistributeJob();
		m_bTimerBusy=true;
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
/*
//��ȡ���屣��Ļ�ͼ�ǲ��� 20100413
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
		double dOverlapError;//20100413 PrintPara��������
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
	{//USBģʽ
		if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
		{
			mLength = 64;
			if (CH375ReadData(0,SendBuf,&mLength))  // ���ճɹ�
			{
				if (mLength == 0)
					return 2;
				if ((unsigned long)SendBuf[0] > (mLength - 1))
				{
					if (CH375ReadData(0,&SendBuf[mLength],&mTotal))  // ���ճɹ�
					{
						if ((unsigned long)SendBuf[0] != (mTotal + mLength - 1))
							return 2;
					}
					else
						return 2;
				}
				j = sizeof(PrintPara) + 8;
				if (SendBuf[0] > j)
				{
					if (strncmp(&SendBuf[1],"Msg_ParaNew Version3.3",22) == 0)
					{//�����¾ɰ汾���������¼���20100413 ��ȡ��ǰ�汾��ϢUSB
						m_nVersion = 1;
						g_nNewVersion = 1 ;
						mPin = (unsigned char *)SendBuf + 22 + 1;
					}
					else if (strncmp(&SendBuf[1],"Msg_ParaNew Version",19) == 0)
					{//�����¾ɰ汾���������¼���20100413 ��ȡ��ǰ�汾��ϢUSB
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
	{//����ģʽ
		send(m_socket,SendBuf,mLength,0);
		recv(m_socket,SendBuf,100,0);
		if (strncmp(SendBuf,"Msg_ParaNew Version3.3",22) == 0)
		{//�����¾ɰ汾���������¼���20100413 ��ȡ��ǰ�汾��Ϣ����
			m_nVersion = 1;
			g_nNewVersion = 1 ;//20100413
			sPin = (unsigned short *)SendBuf + 11;
		
		}
		else if (strncmp(SendBuf,"Msg_ParaNew Version",19) == 0)
		{//�����¾ɰ汾���������¼���20100413 ��ȡ��ǰ�汾��Ϣ����
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
	//20100413 �ж����崫�͵� ��ֽ�������ֵ�ķ�Χ
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
	m_dOverlapError = PrintPara.dOverlapError;//��ֽ���20100413
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
*/
//   20120725
//��ȡ���屣��Ļ�ͼ�ǲ��� 20100413
int CPlotCenterDlg::ReadParaFromPlotter()
{
	if (!m_bConnected)
		return 1;

	struct
	{
		//unsigned char cChinaEn;
		unsigned char cSprinklerCount;
		unsigned char cSprinklerNo;
		unsigned char cPrintStyle;
		unsigned char cLineWidth;
		unsigned char cSprayErrorX12;
	//	unsigned char cSprayErrorX23;
	//	unsigned char cSprayErrorX34;
		unsigned char cPenNo;//20101206  �ʺŵ���
		unsigned char cCutNo;
		unsigned char cPrintCutSelect;//20101206 �и��ӡѡ��		
		unsigned short MyIP[2];
		unsigned short SubnetMask[2];
		unsigned short GatewayIP[2];
		unsigned short sPlotType;
		unsigned short sMaxYMM;
		//int iOriginalPos;
		int	iSprayErrorY12;
		int	iSprayErrorY23;
		int	iSprayErrorY34;
		int iSNull;
		double dOverlapError;//20100413 PrintPara��������
		double dMalposedError;
		double dPlotterXError;
		double dPlotterYError;
	}PrintPara;
	unsigned short *sPin,*sDest;
	unsigned short sdata1,sdata2;
	char SendBuf[120];
	int i,j;
	unsigned long mTotal,mLength;
	unsigned char *mPin,*mDest;
	CString s1;

	strcpy(SendBuf,"Msg_Read");
	mLength = 8;
	if (m_nCommunicateMode == 0)
	{//USBģʽ
		if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
		{
			mLength = 64;
			if (CH375ReadData(0,SendBuf,&mLength))  // ���ճɹ�
			{
				if (mLength == 0)
				{
					s1="���ղ�������0";
					AfxMessageBox(s1);
					//return 2;
				}
				if ((unsigned long)SendBuf[0] > (mLength - 1))
				{
					if (CH375ReadData(0,&SendBuf[mLength],&mTotal))  // ���ճɹ�
					{
						if ((unsigned long)SendBuf[0] != (mTotal + mLength - 1))
							return 2;
					}
					else
						return 2;
				}
				
				j = sizeof(PrintPara) + 8;
				if (SendBuf[0] > j)
				{
					if (strncmp(&SendBuf[1],"New Version3.3",14) == 0)
					{//�����¾ɰ汾���������¼���20100413 ��ȡ��ǰ�汾��ϢUSB
						m_nVersion = 1;
						g_nNewVersion = 1 ;
						mPin = (unsigned char *)SendBuf + 14 + 1;
						char cc;
						cc = *mPin;
					}
					else if (strncmp(&SendBuf[1],"Msg_ParaNew Version",19) == 0)
					{//�����¾ɰ汾���������¼���20100413 ��ȡ��ǰ�汾��ϢUSB
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
	{//����ģʽ
		send(m_socket,SendBuf,mLength,0);
		recv(m_socket,SendBuf,100,0);
		if (strncmp(SendBuf,"Msg_ParaNew Version3.3",22) == 0)
		{//�����¾ɰ汾���������¼���20100413 ��ȡ��ǰ�汾��Ϣ����
			m_nVersion = 1;
			g_nNewVersion = 1 ;//20100413
			sPin = (unsigned short *)SendBuf + 11;
		
		}
		else if (strncmp(SendBuf,"Msg_ParaNew Version",19) == 0)
		{//�����¾ɰ汾���������¼���20100413 ��ȡ��ǰ�汾��Ϣ����
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
	{
		s1="��ӡģʽ���մ�";
		AfxMessageBox(s1);
		//return 2;
	}
	if ((PrintPara.cSprinklerCount < 1) || (PrintPara.cSprinklerCount > 4))
	{
		s1="��ͷ�����մ�";
		AfxMessageBox(s1);
		//return 2;
	}

	switch (PrintPara.cSprinklerCount)
	{
		case 1:
			if ((PrintPara.cSprinklerNo < 0) || (PrintPara.cSprinklerNo > 3))
			{
				s1="13���մ�";
				AfxMessageBox(s1);
			}
			break;
		case 2:
			if ((PrintPara.cSprinklerNo < 0) || (PrintPara.cSprinklerNo > 2))
			{
				s1="13���մ�";
				AfxMessageBox(s1);
			}
			break;
		case 3:
			if ((PrintPara.cSprinklerNo != 0) && (PrintPara.cSprinklerNo != 1))
			{
				s1="13���մ�";
				AfxMessageBox(s1);
			}
			break;
		case 4:
			if (PrintPara.cSprinklerNo != 0)
			{
				s1="13���մ�";
				AfxMessageBox(s1);
			}
			break;
	}
	//20100413 �ж����崫�͵� ��ֽ�������ֵ�ķ�Χ
	if ((PrintPara.dOverlapError < -20.0) || (PrintPara.dOverlapError > 20.0))
	{
		s1="��ֽ�����մ�";
		AfxMessageBox(s1);
		//return 2;
	}
	if ((PrintPara.dMalposedError < -6.0) || (PrintPara.dMalposedError > 6.0))
	{
		s1="˫�������մ�";
		AfxMessageBox(s1);
		//return 2;
	}
	if ((PrintPara.dPlotterXError < 900.0) || (PrintPara.dPlotterXError > 1100.0))
	{
		s1="XУ��ֵ���մ�";
		AfxMessageBox(s1);
		//return 2;
	}
	if ((PrintPara.dPlotterYError < 900.0) || (PrintPara.dPlotterYError > 1100.0))
	{
		s1="YУ��ֵ���մ�";
		AfxMessageBox(s1);
		//return 2;
	}
	if ((PrintPara.cLineWidth < 1) || (PrintPara.cLineWidth > 9))
	{
		s1="�߿���մ�";
		AfxMessageBox(s1);
		//return 2;
	}
	if ((PrintPara.cSprayErrorX12 < 0) || (PrintPara.cSprayErrorX12 > 150))
	{
		s1="12���մ�";
		AfxMessageBox(s1);
		//return 2;
	}
	if ((PrintPara.iSprayErrorY12 < -20) || (PrintPara.iSprayErrorY12 > 20))
	{
		s1="13���մ�";
		AfxMessageBox(s1);
		//return 2;
	}
	if ((PrintPara.iSprayErrorY23 < -20) || (PrintPara.iSprayErrorY23 > 20))
	{
		s1="14���մ�";
		AfxMessageBox(s1);
		//return 2;
	}
	if ((PrintPara.iSprayErrorY34 < -20) || (PrintPara.iSprayErrorY34 > 20))
	{
		s1="15���մ�";
		AfxMessageBox(s1);
		//return 2;
	}
	//20101206 �ʺŵ���
//	if (((PrintPara.cPenNo < 1)&&(PrintPara.cPenNo > 9)) || ((PrintPara.cCutNo < 1)&&(PrintPara.cCutNo > 9)))
//		return 2;
		
		//20101206 �и��ӡѡ��
//	if (PrintPara.cPrintCutSelect < 1 || PrintPara.cPrintCutSelect > 3)
//		return 2;

	m_cPrintStyle = PrintPara.cPrintStyle;
	m_cSprinklerCount = PrintPara.cSprinklerCount;
	m_cSprinklerNo = PrintPara.cSprinklerNo;
	
	m_cPenNo = PrintPara.cPenNo;//20101206 �ʺŵ���
	m_cCutNo = PrintPara.cCutNo;
	g_cPenNo = m_cPenNo;
	g_cCutNo = m_cCutNo;
	g_cPrintCutSelect = PrintPara.cPrintCutSelect;//20101206 ��ӡ�и�ѡ��
	
	m_dOverlapError = PrintPara.dOverlapError;//��ֽ���20100413
	m_dMalposedError = PrintPara.dMalposedError;
	m_dPlotterXError = PrintPara.dPlotterXError;
	m_dPlotterYError = PrintPara.dPlotterYError;
	m_nLineWidth = PrintPara.cLineWidth;
	m_sSprayErrorX12 = PrintPara.cSprayErrorX12;
	m_sSprayErrorX23 = 0;//PrintPara.cSprayErrorX23;
	m_sSprayErrorX34 = 0;//PrintPara.cSprayErrorX34;
	m_sSprayErrorY12 = PrintPara.iSprayErrorY12;
	m_sSprayErrorY23 = 0;//PrintPara.iSprayErrorY23;
	m_sSprayErrorY34 = 0;//PrintPara.iSprayErrorY34;
	m_sPlotType = PrintPara.sPlotType;
	return 0;
}
//
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
			CH375CloseDevice(0);
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

/* 
������ڣ��ļ��еľ���·�� 
          const char*  dirPath 
 
�������ܣ�ɾ�����ļ��У������������е��ļ����ļ��� 
 
����ֵ��  0  ɾ��  
         -1  ·�����ԣ������������û��ִ��ɾ������ 
*/  
int  removeDir(const char*  dirPath)  
{  
  
    struct _finddata_t fb;   //������ͬ�����ļ��Ĵ洢�ṹ��  
    char  path[250];            
    long    handle;  
    int  resultone;  
    int   noFile;            //��ϵͳ�����ļ��Ĵ�����  
      
    noFile = 0;  
    handle = 0;  
  
      
    //����·��  
    strcpy(path,dirPath);  
    strcat (path,"/*");  
  
    handle = _findfirst(path,&fb);  
    //�ҵ���һ��ƥ����ļ�  
    if (handle != 0)  
    {  
        //�����Լ����ҵ�ƥ����ļ�������ִ��  
        while (0 == _findnext(handle,&fb))  
        {  
            //windows�£����и�ϵͳ�ļ�����Ϊ��..��,������������  
            noFile = strcmp(fb.name,"..");  
              
            if (0 != noFile)  
            {  
                //��������·��  
                memset(path,0,sizeof(path));  
                strcpy(path,dirPath);  
                strcat(path,"/");  
                strcat (path,fb.name);  
                //����ֵΪ16����˵�����ļ��У�����  
                if (fb.attrib == 16)  
                {  
                     removeDir(path);     
                }  
                //���ļ��е��ļ���ֱ��ɾ�������ļ�����ֵ�����û����ϸ���飬���ܻ������������  
                else  
                {  
                    remove(path);  
                }  
            }     
        }  
        //�ر��ļ��У�ֻ�йر��˲���ɾ����������������˺ܾã���׼c���õ���closedir  
        //������ܣ�һ�����Handle�ĺ���ִ�к󣬶�Ҫ���йرյĶ�����  
        _findclose(handle);  
    }  
        //�Ƴ��ļ���  
        resultone = rmdir(dirPath);  
        return  resultone;  
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
				DeleteCutInfo(pPlotItem->m_strPlotName);
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
	//ɾ��tempĿ¼���ļ�
	CString strSavePathName;
	char *	temp = new char[MAX_PATH + 1];
	if (temp == NULL)
		return;
	ZeroMemory(temp, MAX_PATH);
	GetModuleFileName(AfxGetInstanceHandle(), temp, MAX_PATH);
	for(int i = MAX_PATH; i >= 0;  i--)
	{
		if (temp[i]=='\\')
		{
			temp[i] = '\0';
			break;
		}
	}
	strSavePathName.Format("%s\\temp", temp);
	if (access(strSavePathName,0) == 0) 
	{
		removeDir(strSavePathName);
	}
	m_NeedCutDataFiles.RemoveAll();
}
 

// ��ѯ�Ƿ��ӡ����
void CPlotCenterDlg::CheckWorkStatus()
{	
	char SendBuf[100];
	unsigned long mLength;
	
	if (!m_bConnected)
	{
		if (!m_bWaitConnected)
		{//��������
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

	if (m_nStartPrint == 1)
	{
			while(1)
			{
                int i;
				for(i=0;i<=1500*1000;i++)
				{
				}
				strcpy(SendBuf,"Msg_Comm");//�����ļ����ͽ���֡
				SendBuf[8] = 0;
				SendBuf[9] = 8;
				mLength = 10;
				if (m_nCommunicateMode == 0)
				{
					
					if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
					{
						mLength = 64;
						CH375ReadData(0,SendBuf,&mLength);
					}
				}
				else
				{
					send(m_socket,SendBuf,mLength,0);
					recv(m_socket,SendBuf,100,0);
				}
				if (memcmp(SendBuf,"Msg_",4) == 0)
				{
					if ((SendBuf[5] == 8) && (SendBuf[7] == 7))
					{//��λ�������ҵ�ǰ�Ѱ��ظ���ӡ����
						m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,false);
						m_nStartPrint = 2;
						m_wndPrintState.SetPos(0);	// ����������
						break;
					}
					else if ((SendBuf[5] == 8) && (SendBuf[7] == 6))
					{//��λ�������ҵ�ǰ�Ѱ��ظ���ӡ����
						//m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,false);
						m_nStartPrint = 0;
						m_wndPrintState.SetPos(0);	// ����������
						break;
					}
					else
						break;
				}
			}
	}
}

//extern UINT SendCutDataFile(int nCommunicateMode, CString strFileName, SOCKET sock);
//extern BOOL g_bPrintEnd;
void CPlotCenterDlg::StartAJob()
{
	
	CPlotItem *pPlotItem;
	int nReturnValue;
	bool bRunPrint=false;

	if ((m_bNetworkBusy) || (m_nStartPrint == 1))//20140729
		return;//����û��ɣ����ܴ�ӡ��һ�ļ�

	if (m_listPlotItem.GetCount() <= 0)
		return;//û���ļ��ȴ���ӡ

	//20100413�ж��Ƿ�����ѡ���ļ����Զ���ӡ  StartAJob
	if(theApp.m_nAutoPrintStyle==0)
	{
 		if (m_nStartPrint == 0)
		{//�Ѿ��������ʼ����ӡ
			m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,true);//
			m_nStartPrint = 1;
			return;
		}
	}
	else
	{//20100517������޸������Զ���ӡ����������ӡ�����⣻
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
		{//��������
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
	if (nReturnValue == 0 || nReturnValue == 2)
	{// 20120710 �������2�����ǵ���ģʽ����
		TCHAR szFullPath[_MAX_PATH];
		TCHAR szDir[_MAX_DIR];
		TCHAR szDrive[_MAX_DRIVE];
		CString sWorkPath;
		FILE *pFile;
		SYSTEMTIME st;

		//д��ʷ��¼
		::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
		_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
		sWorkPath.Format(_T("%s%shistory.ini"),szDrive,szDir);
		pFile=fopen((LPCTSTR)sWorkPath,"ab");
		GetLocalTime(&st);
		fprintf(pFile,"%s|",pPlotItem->m_strPlotName);
		fprintf(pFile,"%d-%d-%d ",st.wYear,st.wMonth,st.wDay);
		fprintf(pFile,"%d:%d:%d|",st.wHour,st.wMinute,st.wSecond);
		fclose(pFile);
		pPlotItem->m_nState=0;
		m_PlotThreadInfo.m_pParent=this;
		m_PlotThreadInfo.sock=m_socket;
		m_PlotThreadInfo.m_pPlotItem=pPlotItem;
		m_bNetworkBusy = true;
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAIN,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(ID_PARAOUT,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		FillListCtrl();

		//if ((g_cPrintCutSelect&0x01) == 0x01)
		//{// 20120710 �����ǰģʽѡ���˴�ӡ
			//gzr20161109	
			//if (FALSE)
			{
			
			if (m_nCommunicateMode == 0)
			{//USB��ӡ
				m_pBmpGenerationThread=AfxBeginThread(BmpGenerationThreadProc,&m_PlotThreadInfo);
				m_pSendDataThread=AfxBeginThread(UsbSendDataThreadProc,&m_PlotThreadInfo);
			}
			else
			{//�����ӡ
				m_pBmpGenerationThread=AfxBeginThread(BmpGenerationThreadProc,&m_PlotThreadInfo);
				m_pSendDataThread=AfxBeginThread(SendDataThreadProc,&m_PlotThreadInfo);
			}
			}
			bRunPrint = true;
		//}
	}
	else
	{
		CString s1;
		if (nReturnValue == 1)
		{//��һ����ͼ�ļ�ʧ��
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
			{//��HP-GL�ļ�!
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
			//log gzr20161027
		}
		if (nReturnValue == 5)
		{
			DeleteCutInfo(pPlotItem->m_strPlotName);
			m_wndPlotItem.DeleteItem(0);
			m_listPlotItem.RemoveHead();
			delete pPlotItem;
		}
		else
		{
			DeleteCutInfo(pPlotItem->m_strPlotName);
			m_wndPlotItem.DeleteItem(0);
			m_listPlotItem.RemoveHead();
			delete pPlotItem;
		}
		
		if (m_listPlotItem.GetCount() == 0)
		{//�ļ�����,�ر�ͨ�ſ�
			if (m_nCommunicateMode == 0)
				CH375CloseDevice(0);
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
	//����ͼ�ο�ȣ��ֽ�����
	while (1)
	{
		strcpy(SendBuf,"Msg_Comm");//���Ͳ�ѯ֡
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
			if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
			{
				mLength = 64;
				CH375ReadData(0,SendBuf,&mLength);
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
		
	//��ѯ�Ͳ�ͨѶ����������
	while (1)
	{
		strcpy(SendBuf,"Buf_Long");//���Ͳ�ѯ֡
		if (m_nCommunicateMode == 0)
		{
			mLength = 8;
			if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
			{
				mLength = 64;
				CH375ReadData(0,SendBuf,&mLength);
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
	strcpy(SendBuf,"Snd_Long");//���Ͳ�ѯ֡
	pBits1 = (BYTE *)&iCountByte;
	SendBuf[8] = *(pBits1+3);
	SendBuf[9] = *(pBits1+2);
	SendBuf[10] = *(pBits1+1);
	SendBuf[11] = *pBits1;
	if (m_nCommunicateMode == 0)
	{
		mLength = 12;
		if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
		{
			mLength = 64;
			CH375ReadData(0,SendBuf,&mLength);
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
			if (CH375WriteData(0,(char *)pBits1,&mLength))  // ���ͳɹ�
			{
				if (iCountByte > mLength)
				{
					iCountByte -= mLength;
					pBits1 += mLength;
				}
				else
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
		strcpy(SendBuf,"Msg_Comm");//�����ļ����ͽ���֡
		SendBuf[8] = 0;
		SendBuf[9] = 2;
		if (m_nCommunicateMode == 0)
		{
			mLength = 10;
			if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
			{
				mLength = 64;
				CH375ReadData(0,SendBuf,&mLength);
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
						{//20100412 �����Զ���ͼĿ¼����ܹ���������������
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

					//����ļ� 20140715
					char cExist = 0;
					char cPGExist = 0;
					CString oldPathName;
					cPGExist = QuickScanFilePG(strPathName);
					if((cPGExist == 0)&&(ftoi(m_lMaxPlotLength*0.025) > gl_dPaperLength))

					{	
						
						oldPathName = strPathName;
	
						strPathName = ScanFileSection(strPathName, ftoi(m_lMaxPlotLength*0.025));
				
						//AfxMessageBox("finish!");
						DeleteFile(oldPathName);
						OpenFile(strPathName);
						if(m_lMinPlotLength < 0)
							m_lMinPlotLength = 0;
						
						if(m_lMinPlotWidth < 0)
							m_lMinPlotWidth = 0;
						cExist = 1;
						
					}

					else if(cPGExist == 1)

					{
						oldPathName = strPathName;
						strPathName = NewFileReentrant(strPathName);
						if(strcmp(strPathName, oldPathName) == 0)
							cExist = 0;
						else
							cExist = 1;
					}
					else if (cPGExist == 2)
					{
						oldPathName = strPathName;
						strPathName = EtFileSection(strPathName);
						if(strcmp(strPathName, oldPathName) == 0)
							cExist = 0;
						else
							cExist = 1;
					}

					if(cExist == 1)
					{
						DeleteFile(oldPathName);
						OpenFile(strPathName);
					}
					
					if(m_lMinPlotLength < 0)
						m_lMinPlotLength = 0;
						
					if(m_lMinPlotWidth < 0)
						m_lMinPlotWidth = 0;
					//*******************
					if (m_lMaxPlotWidth > lPaperWidth)
					{//20100412 �����Զ���ͼĿ¼����ܹ���������������
						m_lMaxPlotLength = m_lMaxPlotLength * ((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth);
						m_lMaxPlotWidth = lPaperWidth;
					}

					pPlotItem=new CPlotItem(strPathName,1,0,ftoi(m_lMaxPlotLength*0.025),ftoi(m_lMaxPlotWidth*0.025),lPaperWidth,m_lMaxPlotLength,m_lMaxPlotWidth,m_lMinPlotLength,m_lMinPlotWidth);
					m_listPlotItem.AddTail(pPlotItem);
					bAddNewJob=true;
//					m_nStartPrint = 1; //20140729
//					m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,true);//20140729
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
  					
     //20160323�Զ���ͼĿ¼���ļ��ֶ�
	//����ļ� 20140715
					char cExist = 0;
					char cPGExist = 0;
					CString oldPathName;
					cPGExist = QuickScanFilePG(strPathName);

					CString strFileName = strPathName;
					if(nReturnValue == 0 && cPGExist == 0 && (g_cPrintCutSelect == 3 || g_cPrintCutSelect == 2))
					{
						strPathName = PltFileChangePage(strPathName);
						cPGExist = QuickScanFilePG(strPathName);
						cExist = 1;
						if (cPGExist != 0 && cPGExist != 1)
						{
							cPGExist = 1;
						}
  					}

					if((cPGExist == 0)&&(ftoi(m_lMaxPlotLength*0.025) > gl_dPaperLength)&&(g_cPrintCutSelect&0x02) == 0x02)

					{						
						oldPathName = strPathName;
						strPathName = ScanFileSection(strPathName, ftoi(m_lMaxPlotLength*0.025));
						DeleteFile(oldPathName);
						OpenFile(strPathName);
						if(m_lMinPlotLength < 0)
							m_lMinPlotLength = 0;
						
						if(m_lMinPlotWidth < 0)
							m_lMinPlotWidth = 0;
						cExist = 1;
					}

					else if((cPGExist == 1)&&((g_cPrintCutSelect == 3)||(g_cPrintCutSelect == 2)))

					{
						oldPathName = strPathName;
						strPathName = NewFileReentrant(strPathName);
						if(strcmp(strPathName, oldPathName) == 0)
							cExist = 0;
						else
							cExist = 1;
		
					}
					else if (cPGExist == 2)
					{
						oldPathName = strPathName;
						strPathName = EtFileSection(strPathName);
						if(strcmp(strPathName, oldPathName) == 0)
							cExist = 0;
						else
							cExist = 1;
					}

					if(cExist == 1)
					{
						DeleteFile(strFileName);
						if (strFileName.CompareNoCase(oldPathName) != 0)
							DeleteFile(oldPathName);
						OpenFile(strPathName);
					}
					
					if(m_lMinPlotLength < 0)
						m_lMinPlotLength = 0;
						
					if(m_lMinPlotWidth < 0)
						m_lMinPlotWidth = 0;
           //20160323�Զ���ͼĿ¼���ļ��ֶ�



					RemoveGLBuffer();
					if ((nReturnValue == 0) && (m_lMaxPlotLength!=0) && (m_lMaxPlotWidth!=0))
					{
						lPaperWidth = (long)(theApp.m_dPaperWidth * 40);
						if (m_lMaxPlotWidth > lPaperWidth)
						{//20100412 �����Զ���ͼĿ¼����ܹ���������������
							m_lMaxPlotLength = m_lMaxPlotLength * ((m_lMaxPlotWidth + lPaperWidth - 1) / lPaperWidth);
							m_lMaxPlotWidth = lPaperWidth;
						}
						//gzr20161115�����Զ���ͼĿ¼����ʱ����ļ���һ�κͺ���ָ�
						//pPlotItem=new CPlotItem(strPathName,1,0,ftoi(m_lMaxPlotLength*0.025),ftoi(m_lMaxPlotWidth*0.025),lPaperWidth,m_lMaxPlotLength,m_lMaxPlotWidth,m_lMinPlotLength,m_lMinPlotWidth);
						//m_listPlotItem.AddTail(pPlotItem);
						bAddNewJob=true;
					}
					else if (nReturnValue != 1)
						DeleteFile(strPathName.GetBuffer(0));
        }
      }
    }
  }
  if (bAddNewJob)
		FillListCtrl();
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
			dPercent = (double)(pPlotItem->m_lSendLength) / (double)(pPlotItem->m_lPictureLength - pPlotItem->m_lFixedPointLength) * 100.0;
			m_wndPrintState.SetPos((int)dPercent);
			break;
    }
    nItem++;
  }
  return true;
}

LRESULT CPlotCenterDlg::OnEndTransmitFile(WPARAM wParam,LPARAM lParam)
{//�ļ�������
	CString strFileName,strCaption;
	CPlotItem *pPlotItem;

	m_pBmpGenerationThread=NULL;//��ֹ�߳�
	m_pSendDataThread=NULL;

	if (m_listPlotItem.GetCount() > 0)
	{//��ӡ�б��л����ļ�
		pPlotItem=(CPlotItem*)m_listPlotItem.GetHead();
		//�ж��ļ�״̬
		if ((pPlotItem->m_nState == 0) || 
			(pPlotItem->m_nState == 3) || 
			(pPlotItem->m_nState == 4))
		{//����ӡ��ɾ�����ļ�
			if (pPlotItem->m_nCopyNum > 1)
			{//��ӡ�����Ƿ�Ϊ���
				TCHAR szFullPath[_MAX_PATH];
				TCHAR szDir[_MAX_DIR];
				TCHAR szDrive[_MAX_DRIVE];
				FILE *pFile;
				SYSTEMTIME st;
			
				//д��ʷ��¼
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
					m_wndPrintState.SetPos(0);//���������
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
				DeleteCutInfo(pPlotItem->m_strPlotName);
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
			m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,false);
			//m_wndtoolbar.GetToolBarCtrl().EnableButton(IDC_STARTORSTOP,true);//20140729
			m_nStartPrint = 1;
			
			//20140814
			if (m_nCommunicateMode == 0)
				CH375CloseDevice(0);
			else
				closesocket(m_socket);
			m_bConnected=false;
		}
		else//20140814
		{  //�ļ�����,�ر�ͨ�ſ�
			if (m_nCommunicateMode == 0)
				CH375CloseDevice(0);
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
	{  //��Ӧ�ó����Զ��������ļ�����
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
	{//�ļ��򲻿�
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING106);
		else
			s1.LoadString(IDS_STRING106_EN);
		AfxMessageBox(s1);
	}
	else
	{//��HP-GL�ļ�!
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

	strcpy(SendBuf,"Msg_Comm");// PrintErrorY
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
		if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
		{
			mLength = 64;
			CH375ReadData(0,SendBuf,&mLength);
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

//20100413���Ͳ�����ֽ�������
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

	strcpy(SendBuf,"Msg_Comm");// PrintTrackPaperError
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
		if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
		{
			mLength = 64;
			CH375ReadData(0,SendBuf,&mLength);
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
	strcpy(SendBuf,"Msg_Comm"); //SprayError
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
		if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
		{
			mLength = 64;
			CH375ReadData(0,SendBuf,&mLength);
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

//���ַ�ת��������
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

//�������:
//  ptPointLU  ��ʾ�ַ��������Ͻ�����(��TextOut(...)��x,yֵ)
//  pbyData �ַ�����������
//  wBytes �ַ����������ݸ���
//  nLeft,nDown �ַ������½�����(����Ӣ���ֿ�Ĺ�����λ������ˮƽ�����ַ���ʱ,�ַ��ĵ����½�����)
//  nFontScale �ֿ�ĳ��ȱ���
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
  dLToDScale=(double)nHeight / 	(m_dTextHeight*400);//ʵ�ʺ�����

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

        //����Բ��
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

        //����Բ��
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
  fFile1.Open((LPCTSTR)strCharFileName,CFile::modeRead | CFile::typeBinary);
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
  m_ptPrevPosition.x=m_ptPrevPosition.x +1;  //Ϊ��LineTo���Ż���
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
  dStep=PI / 180.0 * (360.0 / (double)nCount);  //3��
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
      ((x2==x3) && (y2==y3)))  {  //��������������ͬ
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
    if (x < 0)  {  //�ڶ�������
      q1=pi+q1;
    }
    else  {
      if ((x > 0) && (y < 0))  {  //��������
        q1=(double)2.0*pi+q1;
      }
    }
  }
  return q1;
}

//����ֱ�ߵĽ���
//��ڲ���
//  a1,b1,c1 ֱ��a1*x+b1*y+c1=0
//  a2,b2,c2 ֱ��a2*x+b2*y+c2=0
//���ڲ���
//  (x,y) ��������
//����ֵ
//  =0,û�н���
//  =1,�н���(x,y)
//  =2,��ֱ���غ�
int CPlotCenterDlg::IntOfTwoLine(double a1, double b1, double c1, double a2, double b2, double c2, double &x, double &y)
{
  double p,q;
  int nResult;

  p=a1*b2-a2*b1;
  q=c2*b1-c1*b2;
  if ((fabs(p)+1.0) <= 1.0)  {  //��ֱ�߶�ƽ��
    if ((fabs(q)+1.0) <= 1.0)  {  //��ֱ���غ�
      nResult=2;
    }
    else  {  //��ֱ�߶�ƽ�е����غ�
      nResult=0;
    }
  }
  else  {  //��ֱ�߶��н���
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
	{//�ļ��򲻿�
		CString s1;
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING106);
		else
			s1.LoadString(IDS_STRING106_EN);
		AfxMessageBox(s1);
	}
	else
	{//��HP-GL�ļ�!
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

void CPlotCenterDlg::ConnectUSB(BOOL bDisplayFlag)
{
	int iReadParaState;		// 20120809 �����ȡ�������У������Ӧ��ʾ�û��������ã�
	CString s1;

	if (m_bConnected)
	{
		iReadParaState = ReadParaFromPlotter();
		if(iReadParaState == 2)
		{
			if (m_nLanguage == 0)
			{
				s1="������ȡ�����������л��������";
			}
			else
			{
				s1 = "Read parameters error, please reset parameters!";
			}
			//AfxMessageBox(s1);//20160115  Release�汾���Ǳ��������ȡ�����������л������
		}
		return;
	}

	if (CH375OpenDevice(0) == INVALID_HANDLE_VALUE)
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
		lUsb_Pid_Vid = CH375GetUsbID(0);
		//if (lUsb_Pid_Vid != USB_PID_VID)
		if ((lUsb_Pid_Vid != USB_PID_VID_0) && ( lUsb_Pid_Vid != USB_PID_VID_1 ))
		{
			CH375CloseDevice(0);
			m_bConnected = FALSE;
			/*
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING303);
			else
				s1.LoadString(IDS_STRING303_EN);
			AfxMessageBox(s1);
			*/
			return;
		}
		m_bConnected = TRUE;
		
		//ReadParaFromPlotter();
		iReadParaState = ReadParaFromPlotter();
		int i=0;
		while(iReadParaState == 2)
		{
			if(i>200)
				break;
			iReadParaState = ReadParaFromPlotter();	
			SetTimer(ID_TIMER,T_TIMES,NULL);
			i++;
		}
/*
		if(i>100)
		{
			s1="������ȡ�����������л��������";
			AfxMessageBox(s1);
		}
		*/
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
	}
}
//20100413��������
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
	unsigned char cPenNo;//20101206 �ʺŵ���
	unsigned char cCutNo;

	double dOverlapError; //20100413 ������ȡ ��ֽ���
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
		//20101206 �ʺŵ���
		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"PenNo=",8) != 0)
			break;
		cPenNo = (unsigned char)atoi(&RecBuf[8]);

		if (file.ReadString(RecBuf,50) == false)
			break;
		if (strncmp(RecBuf,"CutNo=",8) != 0)
			break;
		cCutNo = (unsigned char)atoi(&RecBuf[8]);

		//20100413��ȡ�����ļ��е�--��ֽ���ֵ
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
			//���ô�ӡģʽ
			strcpy(SendBuf,"Msg_Comm");// if (m_nVersion == 0)
			SendBuf[8]=0;
			SendBuf[9]=12;
			SendBuf[10]=0;
			SendBuf[11]=cPrintStyle;
			if (m_nCommunicateMode == 0)
			{
				mLength = 12;
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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

			//������ͷѡ��
			strcpy(SendBuf,"Msg_Comm");// if (m_nVersion == 0)
			SendBuf[8]=0;
			SendBuf[9]=13;
			SendBuf[10]=0;
			SendBuf[11]=cSprinklerCount;
			SendBuf[12]=0;
			SendBuf[13]=cSprinklerNo;
			if (m_nCommunicateMode == 0)
			{
				mLength = 14;
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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

			//������ͷ�����
			strcpy(SendBuf,"Msg_Comm");// if (m_nVersion == 0)
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
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
			
			//����˫���ӡ���
			strcpy(SendBuf,"Msg_Comm");// if (m_nVersion == 0)
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
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
				
			//����ͼ�����
			strcpy(SendBuf,"Msg_Comm");// if (m_nVersion == 0)
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
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
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
			strcpy(SendBuf,"Msg_Comm");//  ��������
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
			//20100413 ������ֽ���
			//�������Ƿ�����ֽ���20100413
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
			
			//20101206 �ʺŵ���
			SendBuf[i++]=m_cPenNo;
			SendBuf[i++]=0;
			SendBuf[i++]=m_cCutNo;
			SendBuf[i++]=0;

			if (m_nCommunicateMode == 0)
			{
				if(g_nNewVersion == 1)//#5 20100413
					mLength = 66;
				else
					mLength = 58;
					
				if (CH375WriteData(0,SendBuf,&mLength))  // ���ͳɹ�
				{
					mLength = 64;
					CH375ReadData(0,SendBuf,&mLength);
				}
			}
			else
			{
				if(g_nNewVersion == 1)//#6 20100413
					send(m_socket,SendBuf,66,0);
				else
					send(m_socket,SendBuf,58,0);					
				
				recv(m_socket,SendBuf,30,0);
			}
			if (SendBuf[7] == 9)
			{//�������óɹ�
				m_cPrintStyle = cPrintStyle;
				m_cSprinklerCount = cSprinklerCount;
				m_cSprinklerNo = cSprinklerNo;
				m_cPenNo = cPenNo; //20101206 �ʺŵ���
				m_cCutNo = cCutNo; 
				g_cPenNo = m_cPenNo;
				g_cCutNo = m_cCutNo;
				
				m_nLineWidth = nLineWidth;
				m_dOverlapError = dOverlapError;//20100413 �������óɹ�����
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
			{//��������ʧ��
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
//��������20100413
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
	fprintf(pFile,"PenNo=%d\n",m_cPenNo);//20101206 �ʺŵ���
	fprintf(pFile,"CutNo=%d\n",m_cCutNo);
	fprintf(pFile,"OverlapError=%f\n",m_dOverlapError);//20100413 ��ֽ����������
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
			pSubMenu->EnableMenuItem(ID_BUTTONPREVIEW2, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED); //20140715
			pSubMenu->EnableMenuItem(ID_VERIFYFILECOUNT,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pSubMenu->EnableMenuItem(ID_VERIFYFIXEDPOINT,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else if (nSelCount <= 0)
		{
			pSubMenu->EnableMenuItem(ID_BUTTONPREVIEW2, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED); //20140715
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

//��������ʾ
BOOL CPlotCenterDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	
	// UNICODE��Ϣ
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	//TCHAR szFullText[512];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;
	
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFromΪ��������HWND 
		nID = ::GetDlgCtrlID((HWND)nID);
	}
	
	if (nID != 0) //��Ϊ�ָ���
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
		
		// ʹ��������ʾ������������
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,SWP_NOACTIVATE|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER); 
		return TRUE;
	}
	return TRUE;
}


void CPlotCenterDlg::OnStartOrStop() 
{
	// TODO: Add your control notification handler code here
	//CheckWorkStatus();//20140729 ���ò�ѯ�и�����������������
	if (m_nStartPrint == 1)	//202140729
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

void CPlotCenterDlg::OnOpenFile1() //20160122�и�ָ��ʶ��
{
	// TODO: Add your command handler code here
	OnOpen1();
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


CString CPlotCenterDlg::PltFileChangePage(CString filePath)
{
	m_plotContext.DeleteAll();
	
	char Custer[80]="All file(*.*)\0*.*\0\0";
	BOOL bWaitConnected;
	
	bWaitConnected = m_bWaitConnected;
	m_bWaitConnected = false;
	//CFileDialog dlg(TRUE);
	//dlg.m_ofn.lpstrFilter=Custer;
	//if(dlg.DoModal()==IDOK)
	{
		CFile file;
		CString strFileName;
		//strFileName=dlg.GetPathName();
		strFileName=filePath;
		m_bOpenFile=true;
	}
	CFile file;
	CString strFileName;
	//strFileName=dlg.GetPathName();
	strFileName=filePath;
	InitPara();
	
	CFileException e;
	
	m_filePath = strFileName;
	
	char *	pBuf = new	char[64*1024 + 1];
	if (pBuf == NULL)
		return strFileName;
	TRY
	{
		CFile file(m_filePath, CFile::modeRead | CFile::typeBinary);
		UINT	n = file.GetLength();
		UINT	i, j;
		i = 0;
		j = (n - i) < 64 * 1024 ? (n - i) : 64 * 1024;
		file.SeekToBegin();
		
		while(i < n && j >0)
		{
			file.Read(pBuf, j);
			pBuf[j] = '\0';
			m_fileContent += pBuf;
			i += j;
			j = (n - i) < 64 * 1024 ? (n - i) : 64 * 1024;
		};
		file.Close();					// �ر��ļ�
	}
	CATCH( CFileException, e )
	{
#ifdef _DEBUG
		afxDump << "File could not be opened " << e->m_cause << "\n";
#endif
		//		AfxMessageBox ("File could not be opened ");
		
		InitPara();
		delete	[]pBuf;
		return strFileName;
	}
	END_CATCH
		delete	[]pBuf;
	
	
	LoadAsHpgl(m_fileContent);
	
	FillShapeArray(m_hpglArray);
	
	//GroupLines();

	CShapeSet * pShapeSet = m_plotContext.GetShapeSet();

	Invalidate();

	CString		m_hpgl;
	double		m_unit;
	long m_length;

	//m_length = 350;

	m_length = 300;
	m_unit = 0.025;

	// �õ��������
	double	page = m_length;
	page /= m_unit;
	// ����ÿһ����ҳ���߼���λ
	//m_pDoc->Pagination((long)(page+0.5), m_hpgl);
	// ��ҳ���, ҳ��С width length ���߼���λ

    long length=(long)(page+0.5);

	pShapeSet = new CShapeSet;
	pShapeSet->CreateSimilarSet(m_plotContext.GetShapeSet());

	CString hpgl;

	hpgl.Empty();

	hpgl += "IN;DF;PA;PU;SP1;";
	
/*	//gzr20161025 start
	//plt IP������� m_bSCValid && m_bIPValid
	if (m_bIPValid)
	{
		CString strip;
		strip.Format("IP%d,%d,%d,%d;",m_dpP1.x,m_dpP1.y,m_dpP2.x,m_dpP2.y);
		hpgl += strip;
	}
	//plt SC�������
	if (m_bSCValid)
	{
		CString strsc;
		strsc.Format("SC%lf,%lf,%lf,%lf;",m_struSC.dXMin,m_struSC.dXMax,m_struSC.dYMin,m_struSC.dYMax);
		hpgl += strsc;
	}
	//gzr20161025 end*/
	
	CMyPoint ptThis, ptLast, *pptThis, *pptLast;

	CShape	* pShape;
	CPolyLine * pPolyLine;
	CLabel	*	pLabel;
	CString	str;

	bool	first = true;
	bool	firstline;
	CSize		m_charSize;
	int			m_escapement;
	 
	Arrange		m_arrange;
	double	x, y;
	long	result;

	bool	bNewShape;
	BYTE	curpen = 1;
	bool	changePage = false;//gzr20160913 �жϷ�ҳ
	int i, n, j, m;
	do
	{		
		m_escapement = 0;
		n = pShapeSet->GetShapeCount();
		for (i = 0; i < n; )
		{
			pShape = pShapeSet->GetAShape(i);

			if (pShape->m_type == SP_POLYLINE)
			{
				pPolyLine = (CPolyLine *)pShape;
				m = pPolyLine->GetPointCount();

				if (m < 2)								// ��������еĵ�ĸ���С��2����ɾ��������
				{
					if (m == 1)
					{
						delete	pPolyLine->GetPoint(0);
						pPolyLine->RemoveAll();
					}
					pShapeSet->RemoveShape(i);
					delete	pPolyLine;
					n = pShapeSet->GetShapeCount();
					continue;
				}

				if (curpen != pPolyLine->m_penCode)		// �Ƿ񻻱�
				{
					if(pPolyLine->m_penCode==0)//20160127�޸Ĺ�
					{
					str.Empty();
					str.Format("PU;SP%d;", 1);//20160127�޸Ĺ�
					//gzr20160909  2
					hpgl += str;
					curpen = pPolyLine->m_penCode;
					}
					else
					{
	                str.Empty();
					str.Format("PU;SP%d;", pShape->m_penCode);//20160127�޸Ĺ�
					//gzr20160909  1
					hpgl += str;
					curpen = pPolyLine->m_penCode;
					}
				}
				pptLast = pptThis = NULL;

				bNewShape = false;
				firstline = true;
				changePage = false;//gzr20160913 �жϷ�ҳ
				for (j = 1; j < m;)
				{
					pptLast = pPolyLine->GetPoint(j - 1);
					pptThis = pPolyLine->GetPoint(j);
				
					if (pptLast->x == pptThis->x && pptLast->x == length)
					{
						pptLast->x -= length;
						j++;
					}
					else if (pptLast->x <= length && pptThis->x <= length)
					{
						str.Empty();
						if (firstline)
						{
							firstline = false;

							str.Format("PU%d,%d;PD%d,%d;", pptLast->x, pptLast->y, pptThis->x, pptThis->y);
						}
						else
						{
							//gzr20161109ȥ���ظ���
							if(pptThis->x == pptLast->x && pptThis->y == pptLast->y)
							{
								
							}
							else
								str.Format("PD%d,%d;", pptThis->x, pptThis->y);
						}
						//gzr20160909  3
						hpgl += str;
						//gzr20160913 �ж��Ƿ��з�ҳ���� start
						
						if (j != 1 && changePage)
						{
							//�з�ҳ����һ�㱣������һҳ
							pptLast->x -= length;
							j++;
						}
						else
						{
							//�޷�ҳ����һ��ɾ��
							pPolyLine->RemovePoint(pptLast);
							delete	pptLast;
						}
						m = pPolyLine->GetPointCount();
						//�ж����޷�ҳ�͵�ǰ�߶���ѭ����
						if (!changePage && j == m)
						{
							//�޷�ҳ���ҵ�ǰ�߶���ѭ���꣬ɾ����ǰ��
							pPolyLine->RemovePoint(pptThis);
							delete	pptThis;
 						}
						//gzr20160913 �жϷ�ҳ
						m = pPolyLine->GetPointCount();
 					
						changePage = false;
						// continue;
						//gzr20160913 �ж��Ƿ��з�ҳ���� end
					}
					else if (pptLast->x < length && pptThis->x > length)
					{
						GetOnLinePt(pptLast, pptThis, length, result);
						str.Empty();
						if (firstline)
						{
							firstline = false;

							str.Format("PU%d,%d;PD%d,%d;", pptLast->x, pptLast->y, length, result);
						}
						else
						{
							//gzr20161109ȥ���ظ���
							if(pptThis->x == pptLast->x && pptThis->y == pptLast->y)
							{
								
							}
							else
								str.Format("PD%d,%d;", length, result);
						}
						//gzr20160909  4
						hpgl += str;
						firstline = true;
						pptLast->x = length;
						pptLast->y = result;
					}
					else if ((pptLast->x > length && pptThis->x < length))
					{
						GetOnLinePt(pptLast, pptThis, length, result);

						// �ָ�
						CMyPoint * tppit = new CMyPoint;	// �µ�this
						tppit->x = pptThis->x;
						tppit->y = pptThis->y;				// ���� this

						pptThis->x = length;
						pptThis->y = result;				// this ����

						CMyPoint * tppit1 = new CMyPoint;	// �µ�last
						tppit1->x = pptThis->x;
						tppit1->y = pptThis->y;				// ���ƺ��this

						pptThis->x -= length;
						pptLast->x -= length;				// ���߶�ǰ����, Ϊ��һҳ׼��

						CPolyLine * tppline = new CPolyLine;
						tppline->m_penCode = pPolyLine->m_penCode;
						tppline->AddPoint(tppit1);			
						tppline->AddPoint(tppit);			// ���ƺ��point��Ϊ��ʼ
						j++;
						while (j < m)		// �����ߺ����ĵ��ƶ����µ�������
						{
							tppit = pPolyLine->GetPoint(j);
							tppline->AddPoint(tppit);
							pPolyLine->RemovePoint(j);
							m = pPolyLine->GetPointCount();
						}
						i++;
						pShapeSet->InsertShape(i, tppline);
						n = pShapeSet->GetShapeCount();
						bNewShape = true;
					}
					else if (pptLast->x >= length && pptThis->x >= length)
					{
						pptLast->x -= length;
						j++;
						if (j >= m)
							pptThis->x -= length;
						//gzr20160913 �жϷ�ҳ
						changePage = true;
					}
					else
					{
						i = i;
					}
				}
				if (bNewShape)
				{
					continue;
				}
				if (m < 2)
				{

					if (m == 1)
					{
						delete	pPolyLine->GetPoint(0);
						pPolyLine->RemoveAll();
					}
					pShapeSet->RemoveShape(i);
					delete	pPolyLine;
					n = pShapeSet->GetShapeCount();
					continue;
				}
			}

			else if (pShape->m_type == SP_LABEL)
			{
				pLabel = (CLabel *)pShape;

				//if (curpen != pLabel->m_penCode && pLabel->m_penCode!=0)		// �Ƿ񻻱�//20160418���Ĺ�HHYע�͵��ˣ�ֻҪ���־ͻ�ΪSP1
				{
					str.Empty();
					//str.Format("PU;SP%d;", pShape->m_penCode);
					str.Format("PU;SP%d;", 1);
					hpgl += str;
					//curpen = pLabel->m_penCode;
					curpen = 1;
				}

				pptLast = pptThis = NULL;

				if (pLabel->m_startPoint.x < length)
				{
					if (first)
					{
						first = false;
						m_escapement = pLabel->m_escapement;
						m_charSize = pLabel->m_chSizer;
						m_arrange = pLabel->m_arrange;


						GetDirectionX(pLabel->m_escapement, x, y);
						str.Format("DI%1.4f,%1.4f;", x, y);
						hpgl += str;

						x = pLabel->m_chSizer.cx * 0.025 / 10;
						y = pLabel->m_chSizer.cy * 0.025 / 10;
					
						str.Format("SI%1.4f,%1.4f;", x, y);
						hpgl += str;

						str.Format("LO%d;", pLabel->m_arrange);
						hpgl += str;

						str.Format("PU%d,%d;", pLabel->m_startPoint.x, pLabel->m_startPoint.y);
						hpgl += str;
						str.Empty();

						str.Format("LB%s%c;", pLabel->m_content, 0x03);
						hpgl += str;
						str.Empty();
					}
					else
					{
						//gzr20161107 ��ҳ�����ֱ���ֽ���������ǰ���DI��SI��LO���ȥ���ж�
						//if (m_escapement != pLabel->m_escapement)
						{
							GetDirectionX(pLabel->m_escapement, x, y);
							m_escapement = pLabel->m_escapement;
							str.Format("DI%1.4f,%1.4f;", x, y);
							hpgl += str;	
						}
						//if (m_charSize != pLabel->m_chSizer)
						{
							x = pLabel->m_chSizer.cx * 0.025 / 10;
							y = pLabel->m_chSizer.cy * 0.025 / 10;
							
							str.Format("SI%1.4f,%1.4f;", x, y);
							hpgl += str;
						}

						if (m_arrange != pLabel->m_arrange)
						{
							str.Format("LO%d;", pLabel->m_arrange);
							hpgl += str;
						}
						
						str.Format("PU%d,%d;", pLabel->m_startPoint.x, pLabel->m_startPoint.y);
						hpgl += str;
						str.Empty();
						CString str_t;
						str_t = pLabel->m_content;
						str_t.TrimRight(" ");
						str.Format("LB%s%c;", str_t, 0x03);
						hpgl += str;
						str.Empty();
					}
					pShapeSet->RemoveShape(i);
					n = pShapeSet->GetShapeCount();
					continue;
				}
				else
				{
					pLabel->m_startPoint.x -= length;
				}
			}
			else
			{
				// ����ͼ��
			}
			i++;
		}
		//hpgl += "PU;SP1;PG;";
		//hpgl += "PG;SP1;";
		//hpgl += "PG;";
		curpen = 1;
		if (n <= 0)						// ���
		{
			hpgl += "CT;PG;SP1;";
			break;
		}
		else
			hpgl += "PG;SP1;";

	}while (true);

//gzr20160913 ��ʱע�� start
	//hpgl.Replace("PU;SP1;PU;SP4;","");
	pShapeSet->DeleteAll();

// �����ļ�
	char *	temp = new char[MAX_PATH + 1];
	if (temp == NULL)
		return strFileName;
	ZeroMemory(temp, MAX_PATH);
	GetModuleFileName(AfxGetInstanceHandle(), temp, MAX_PATH);
		i;
	for(i = MAX_PATH; i >= 0;  i--)
	{
		if (temp[i]=='\\')
		{
			temp[i] = '\0';
			break;
		}
	}



		//�����ļ�ɾ������þ��ļ���
		CString oldHBname;
		oldHBname.Empty();
		char *	pBuf1 = new	char[64*1024 + 1];
	    if (pBuf1 == NULL)
		return strFileName;
	    TRY
		{
		CFile file2("C:\\LIQIECUT.txt", CFile::modeRead | CFile::typeBinary);
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
		file2.Close();					// �ر��ļ�
		}
	    CATCH( CFileException, e )
		{
		CFile file3("C:\\LIQIECUT.txt", CFile::modeCreate | CFile::modeWrite);
		file3.Write("", 0);
		file3.Close();
		}
    	END_CATCH
	    delete	[]pBuf1;
	//CString strFileName;
	//strFileName = filePath;
	
 	char fn[300],*p111;
 	//�Ͼ������ĳ�ַ�ʽ��õ�ȫ�ļ�����pathname��,"..."��ֻ�Ǿ���
    strcpy(fn,(p111=strrchr(filePath,'\\')) ? p111+1 : filePath);
	strFileName1=fn;
	//CString	name;   temp
	strFileName.Format("%s\\temp\\"+strFileName1, temp);
	CString strSavePathName;
	strSavePathName.Format("%s\\temp", temp);
	if (access(strSavePathName,0) == -1) 
	{
		mkdir(strSavePathName);
	}
	//m_pDoc->SaveHPGLFile(name, m_hpgl);

// 	char oldfilename[100];
//     strncpy(oldfilename,(LPCTSTR)oldHBname,sizeof(oldfilename));
//     //int bbb=rename(oldfilename,newfilename);//Ϊ��Ļ��������ɹ�
// 	if(oldHBname!=strFileName)
// 	{
// 		remove(oldfilename);//Ϊ��Ļ�ɾ���ļ��ɹ�
// 	}
// 
// 	int n1=strFileName.GetLength();
// 	CFile file1("C:\\LIQIECUT.txt", CFile::modeCreate | CFile::modeWrite);
// 	file1.Write(strFileName, n1);
// 	file1.Close();
	//hpgl += "CT;";
	
	n = hpgl.GetLength();		
	
	LPTSTR	p = hpgl.GetBuffer(n);

	TRY
	{
		CFile file(strFileName, CFile::modeCreate | CFile::modeWrite );

		file.Write(p, n);

		file.Close();
	}

	CATCH( CFileException, e )
	{
		#ifdef _DEBUG
			afxDump << "File could not be opened " << e->m_cause << "\n";
			//AfxMessageBox ("File could not be opened ");
		#endif
		hpgl.ReleaseBuffer();
		return filePath;
	}
	END_CATCH
	hpgl.ReleaseBuffer();

	delete	[]temp;

	n = m_hpgl.GetLength();
	if(n <= 0)
	{
		m_hpgl.Empty();
	}
	m_bOpenFile = FALSE;
	//bWaitConnected = m_bWaitConnected;
	m_bWaitConnected = bWaitConnected;	
	return strFileName;
}
