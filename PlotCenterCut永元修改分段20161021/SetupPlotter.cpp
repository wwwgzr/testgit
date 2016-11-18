// SetupPlotter.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "SetupPlotter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern int g_nNewVersion;//区别新旧版本，用于向下兼容20100413
extern unsigned char g_cPrintCutSelect;//20101206 打印切割选择
extern unsigned char g_cPenNo;//20101206 笔号刀号
extern unsigned char g_cCutNo;
/////////////////////////////////////////////////////////////////////////////
// CSetupPlotter dialog


CSetupPlotter::CSetupPlotter(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupPlotter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupPlotter)
	m_bSpray1 = FALSE;
	m_bSpray2 = FALSE;
	m_bSpray3 = FALSE;
	m_bSpray4 = FALSE;
	m_bPrint = ((g_cPrintCutSelect&0x01) == 0x01) ? TRUE: FALSE;//20101206 打印切割选择
	m_bCut   = ((g_cPrintCutSelect&0x02) == 0x02) ? TRUE: FALSE;
	m_dOverlapError = 0.0; //20100413 走纸误差初始化
	m_dMalposedError = 0.0;
	m_dGraphWidth = 0.0;
	m_sSprayErrorX12 = 0;
	m_sSprayErrorY12 = 0;
	m_sSprayErrorX23 = 0;
	m_sSprayErrorX34 = 0;
	m_sSprayErrorY23 = 0;
	m_sSprayErrorY34 = 0;
	m_dPlotterXError = 0.0;
	m_dPlotterYError = 0.0;
	m_nAutoPrintStyle = 0;// 20120809 theApp.m_nAutoPrintStyle;//20100413是否自动打印
	m_cPenNo = g_cPenNo;//20101206 笔号刀号
	m_cCutNo = g_cCutNo;
	//}}AFX_DATA_INIT
}


void CSetupPlotter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupPlotter)
	DDX_Control(pDX, IDC_PAPERTYPESELECT, m_PaperTypeSelect);
	DDX_Check(pDX, IDC_SPRAY1, m_bSpray1);
	DDX_Check(pDX, IDC_SPRAY2, m_bSpray2);
	DDX_Check(pDX, IDC_SPRAY3, m_bPrint);//20101206 打印切割选择
	DDX_Check(pDX, IDC_SPRAY4, m_bCut);
	DDX_Text(pDX, IDC_MALPOSEDERROR, m_dMalposedError);
	DDV_MinMaxDouble(pDX, m_dMalposedError, -6., 6.);
	DDX_Text(pDX, IDC_GRAPHWIDTH, m_dGraphWidth);
	DDV_MinMaxDouble(pDX, m_dGraphWidth, 0., 2500.);
	DDX_Text(pDX, IDC_SPRAYERRORX12, m_sSprayErrorX12);
	DDV_MinMaxInt(pDX, m_sSprayErrorX12, 0, 150);	
	DDX_Text(pDX, IDC_SPRAYERRORY12, m_sSprayErrorY12);
	DDV_MinMaxInt(pDX, m_sSprayErrorY12, -20, 20);
	DDX_Text(pDX, IDC_PLOTTERXERROR, m_dPlotterXError);
	DDV_MinMaxDouble(pDX, m_dPlotterXError, 900., 1100.);
	DDX_Text(pDX, IDC_PLOTTERYERROR, m_dPlotterYError);
	DDV_MinMaxDouble(pDX, m_dPlotterYError, 900., 1100.);
	DDX_Check(pDX, IDC_SPRAY5, m_nAutoPrintStyle);
	DDX_Text(pDX, IDC_MALPOSEDERROR2, m_cPenNo);//20101206 笔号刀号
	DDV_MinMaxDouble(pDX, m_cPenNo, 1, 9);
	DDX_Text(pDX, IDC_MALPOSEDERROR3, m_cCutNo);
	DDV_MinMaxDouble(pDX, m_cCutNo, 1, 9);
	//}}AFX_DATA_MAP

	//20141219 rwt
	//DDX_Control(pDX, IDC_PAPERTYPESELECT, m_sPaperTypeSelect);
	
}


BEGIN_MESSAGE_MAP(CSetupPlotter, CDialog)
	//{{AFX_MSG_MAP(CSetupPlotter)
	ON_BN_CLICKED(IDC_SPRAY1, OnSpray1)
	ON_BN_CLICKED(IDC_SPRAY2, OnSpray2)
	ON_BN_CLICKED(IDC_UNIDIRECTIONAL, OnUnidirectional)
	ON_BN_CLICKED(IDC_BIDIRECTIONAL, OnBidirectional)
	ON_BN_CLICKED(IDC_PMALPOSEDGRAPH, OnPMalposedGraph)
	ON_BN_CLICKED(IDC_SPRAYERROR, OnSprayError)
	ON_BN_CLICKED(IDC_SPRAY3, OnPrint)
	ON_BN_CLICKED(IDC_SPRAY4, OnCut)
	ON_BN_CLICKED(IDC_SPRAY5, OnSelectAutoPrint)
	ON_BN_CLICKED(IDC_ADDPAPERTYPE, OnAddPaperType)
	ON_BN_CLICKED(IDC_USEPAPERTYPE, OnUsePaperType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupPlotter message handlers

BOOL CSetupPlotter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_dMalposedErrorBak = m_dMalposedError;
	m_dOverlapErrorBak  = m_dOverlapError; //20100413 备份当前设置的走的误差
	m_sSprayErrorX12Bak = m_sSprayErrorX12;
	m_sSprayErrorY12Bak = m_sSprayErrorY12;
	m_sSprayErrorX23Bak = m_sSprayErrorX23;
	m_sSprayErrorX34Bak = m_sSprayErrorX34;
	m_sSprayErrorY23Bak = m_sSprayErrorY23;
	m_sSprayErrorY34Bak = m_sSprayErrorY34;
 m_nAutoPrintStyle = theApp.m_nAutoPrintStyle ;
 this->m_dGraphWidth = theApp.m_dGraphWidth;
	CString strItem1;
	CWnd* pWnd;

	if (m_cPrintStyle == 0)
		CheckRadioButton(IDC_UNIDIRECTIONAL,IDC_BIDIRECTIONAL,IDC_UNIDIRECTIONAL);
	else
		CheckRadioButton(IDC_UNIDIRECTIONAL,IDC_BIDIRECTIONAL,IDC_BIDIRECTIONAL);

	//当前主板程序是V3.23之前的版本，走纸误差锁定不能设置20100413
	if(g_nNewVersion == 0)
	{
//		pWnd = GetDlgItem(IDC_PLOTSQUARE3);
//		pWnd->EnableWindow(false);
//		pWnd = GetDlgItem(IDC_PLOTTERYERROR2);
//		pWnd->EnableWindow(false);			
	}


	switch (m_cSprinklerCount)
	{
		case 0:
		case 1:
			switch (m_cSprinklerNo)
			{
				case 0:
					m_bSpray1 = TRUE;
					m_bSpray2 = FALSE;
					m_bSpray3 = FALSE;
					m_bSpray4 = FALSE;
					break;
				case 1:
					m_bSpray1 = FALSE;
					m_bSpray2 = TRUE;
					m_bSpray3 = FALSE;
					m_bSpray4 = FALSE;
					break;
				case 2:
					m_bSpray1 = FALSE;
					m_bSpray2 = FALSE;
					m_bSpray3 = TRUE;
					m_bSpray4 = FALSE;
					break;
				default:
					m_bSpray1 = FALSE;
					m_bSpray2 = FALSE;
					m_bSpray3 = FALSE;
					m_bSpray4 = TRUE;
					break;
			}
			pWnd = GetDlgItem(IDC_SPRAYERRORY12);
			pWnd->EnableWindow(false);
//			pWnd = GetDlgItem(IDC_SPRAYERRORY23);
//			pWnd->EnableWindow(false);
//			pWnd = GetDlgItem(IDC_SPRAYERRORY34);
//			pWnd->EnableWindow(false);
			pWnd = GetDlgItem(IDC_SPRAYERRORX12);
			pWnd->EnableWindow(false);
//			pWnd = GetDlgItem(IDC_SPRAYERRORX23);
//			pWnd->EnableWindow(false);
//			pWnd = GetDlgItem(IDC_SPRAYERRORX34);
//			pWnd->EnableWindow(false);
			pWnd = GetDlgItem(IDC_SPRAYERROR);
			pWnd->EnableWindow(false);
			break;
		case 2:
			switch (m_cSprinklerNo)
			{
				case 0:
					m_bSpray1 = TRUE;
					m_bSpray2 = TRUE;
					m_bSpray3 = FALSE;
					m_bSpray4 = FALSE;
//					pWnd = GetDlgItem(IDC_SPRAYERRORY23);
//					pWnd->EnableWindow(false);
//					pWnd = GetDlgItem(IDC_SPRAYERRORY34);
//					pWnd->EnableWindow(false);
//					pWnd = GetDlgItem(IDC_SPRAYERRORX23);
//					pWnd->EnableWindow(false);
//					pWnd = GetDlgItem(IDC_SPRAYERRORX34);
//					pWnd->EnableWindow(false);
					break;
				case 1:
					m_bSpray1 = FALSE;
					m_bSpray2 = TRUE;
					m_bSpray3 = TRUE;
					m_bSpray4 = FALSE;
					pWnd = GetDlgItem(IDC_SPRAYERRORY12);
					pWnd->EnableWindow(false);
//					pWnd = GetDlgItem(IDC_SPRAYERRORY34);
//					pWnd->EnableWindow(false);
					pWnd = GetDlgItem(IDC_SPRAYERRORX12);
					pWnd->EnableWindow(false);
//					pWnd = GetDlgItem(IDC_SPRAYERRORX34);
//					pWnd->EnableWindow(false);
					break;
				default:
					m_bSpray1 = FALSE;
					m_bSpray2 = FALSE;
					m_bSpray3 = TRUE;
					m_bSpray4 = TRUE;
					pWnd = GetDlgItem(IDC_SPRAYERRORY12);
					pWnd->EnableWindow(false);
//					pWnd = GetDlgItem(IDC_SPRAYERRORY23);
//					pWnd->EnableWindow(false);
					pWnd = GetDlgItem(IDC_SPRAYERRORX12);
					pWnd->EnableWindow(false);
//					pWnd = GetDlgItem(IDC_SPRAYERRORX23);
//					pWnd->EnableWindow(false);
					break;
			}
			break;
		case 3:
			if (m_cSprinklerNo == 0)
			{
				m_bSpray1 = TRUE;
				m_bSpray2 = TRUE;
				m_bSpray3 = TRUE;
				m_bSpray4 = FALSE;
//				pWnd = GetDlgItem(IDC_SPRAYERRORY34);
//				pWnd->EnableWindow(false);
//				pWnd = GetDlgItem(IDC_SPRAYERRORX34);
//				pWnd->EnableWindow(false);
			}
			else
			{
				m_bSpray1 = FALSE;
				m_bSpray2 = TRUE;
				m_bSpray3 = TRUE;
				m_bSpray4 = TRUE;
				pWnd = GetDlgItem(IDC_SPRAYERRORY12);
				pWnd->EnableWindow(false);
				pWnd = GetDlgItem(IDC_SPRAYERRORX12);
				pWnd->EnableWindow(false);
			}
			break;
		default:
			m_bSpray1 = TRUE;
			m_bSpray2 = TRUE;
			m_bSpray3 = TRUE;
			m_bSpray4 = TRUE;
			break;
	}

	this->m_dGraphWidth=theApp.m_dGraphWidth;
	this->m_nLanguage=theApp.m_nLanguage;

	if (m_nLanguage == 0)
	{//20100413绘图仪设置语言标识符
		strItem1.LoadString(IDS_STRING202);
		SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING204);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_UNIDIRECTIONAL);
		strItem1.LoadString(IDS_STRING206);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_BIDIRECTIONAL);
		strItem1.LoadString(IDS_STRING208);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC2);
		strItem1.LoadString(IDS_STRING210);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPRAY1);
		strItem1.LoadString(IDS_STRING212);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPRAY2);
		strItem1.LoadString(IDS_STRING214);
		pWnd->SetWindowText(strItem1);
		//20101206 切割打印选择
		pWnd = GetDlgItem(IDC_SPRAY3);
		strItem1.LoadString(IDS_STRING329);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_SPRAY4);
		strItem1.LoadString(IDS_STRING330);
		pWnd->SetWindowText(strItem1);
		//******
		pWnd = GetDlgItem(IDC_STATIC3);
		strItem1.LoadString(IDS_STRING220);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC4);
		strItem1.LoadString(IDS_STRING222);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC5);
		strItem1.LoadString(IDS_STRING224);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_PMALPOSEDGRAPH);
		strItem1.LoadString(IDS_STRING226);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC6);
		strItem1.LoadString(IDS_STRING228);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC7);
		strItem1.LoadString(IDS_STRING230);
		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC8);
//		strItem1.LoadString(IDS_STRING232);
//		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC9);
//		strItem1.LoadString(IDS_STRING234);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC10);
		strItem1.LoadString(IDS_STRING178);
		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC11);
//		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC12);
//		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC13);
		strItem1.LoadString(IDS_STRING236);
		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC14);
//		strItem1.LoadString(IDS_STRING238);
//		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC15);
//		strItem1.LoadString(IDS_STRING240);
//		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC16);
		strItem1.LoadString(IDS_STRING178);
		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC17);
//		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC18);
//		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPRAYERROR);
		strItem1.LoadString(IDS_STRING242);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_PLOTTERERROR);
		strItem1.LoadString(IDS_STRING170);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_OUTPUTY);
		strItem1.LoadString(IDS_STRING172);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_ADDPAPERTYPE);   
		strItem1.LoadString(IDS_STRING335);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_USEPAPERTYPE);   
		strItem1.LoadString(IDS_STRING336);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_PAPERTYPESELECTTEXT);   
		strItem1.LoadString(IDS_STRING337);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_OUTPUTX);
		strItem1.LoadString(IDS_STRING338);
		pWnd->SetWindowText(strItem1);

//		pWnd = GetDlgItem(IDC_PLOTSQUARE);
//		strItem1.LoadString(IDS_STRING269);
//		pWnd->SetWindowText(strItem1);
		//20100413 语言 走纸误差中文
//		pWnd = GetDlgItem(IDC_PLOTTERERROR2);
//		strItem1.LoadString(IDS_STRING317);
//		pWnd->SetWindowText(strItem1);

//		pWnd = GetDlgItem(IDC_OUTPUTY2);
//		strItem1.LoadString(IDS_STRING320);
//		pWnd->SetWindowText(strItem1);

//		pWnd = GetDlgItem(IDC_PLOTSQUARE3);
//		strItem1.LoadString(IDS_STRING322);
//		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC23);
		strItem1.LoadString(IDS_STRING331);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_STATIC22);
		strItem1.LoadString(IDS_STRING332);
		pWnd->SetWindowText(strItem1);
		
		//20101206 笔号刀号 语言
		pWnd = GetDlgItem(IDC_STATIC17);
		strItem1.LoadString(IDS_STRING327);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_STATIC18);
		strItem1.LoadString(IDS_STRING328);
		pWnd->SetWindowText(strItem1);		

		//20100413 打印启动方式 语言
		pWnd = GetDlgItem(IDC_STATIC21);
		strItem1.LoadString(IDS_STRING324);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_SPRAY5);
		strItem1.LoadString(IDS_STRING326);
		pWnd->SetWindowText(strItem1);
		//pWnd->EnableWindow(false);//20140729
		//**********************
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING180);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182);
		pWnd->SetWindowText(strItem1);
	}
	else
	{
		strItem1.LoadString(IDS_STRING202_EN);
		SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING204_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_UNIDIRECTIONAL);
		strItem1.LoadString(IDS_STRING206_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_BIDIRECTIONAL);
		strItem1.LoadString(IDS_STRING208_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC2);
		strItem1.LoadString(IDS_STRING210_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPRAY1);
		strItem1.LoadString(IDS_STRING212_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPRAY2);
		strItem1.LoadString(IDS_STRING214_EN);
		pWnd->SetWindowText(strItem1);
		//20101206 切割打印选择
		pWnd = GetDlgItem(IDC_SPRAY3);
		strItem1.LoadString(IDS_STRING329_EN);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_SPRAY4);
		strItem1.LoadString(IDS_STRING330_EN);
		pWnd->SetWindowText(strItem1);
		//******
		pWnd = GetDlgItem(IDC_STATIC3);
		strItem1.LoadString(IDS_STRING220_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC4);
		strItem1.LoadString(IDS_STRING222_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC5);
		strItem1.LoadString(IDS_STRING224_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_PMALPOSEDGRAPH);
		strItem1.LoadString(IDS_STRING226_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC6);
		strItem1.LoadString(IDS_STRING228_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC7);
		strItem1.LoadString(IDS_STRING230_EN);
		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC8);
//		strItem1.LoadString(IDS_STRING232_EN);
//		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC9);
//		strItem1.LoadString(IDS_STRING234_EN);
//		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC10);
		strItem1.LoadString(IDS_STRING178_EN);
		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC11);
//		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC12);
//		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC13);
		strItem1.LoadString(IDS_STRING236_EN);
		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC14);
//		strItem1.LoadString(IDS_STRING238_EN);
//		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC15);
//		strItem1.LoadString(IDS_STRING240_EN);
//		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC16);
		strItem1.LoadString(IDS_STRING178_EN);
		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC17);
//		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_STATIC18);
//		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPRAYERROR);
		strItem1.LoadString(IDS_STRING242_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_PLOTTERERROR);
		strItem1.LoadString(IDS_STRING170_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_OUTPUTY);
		strItem1.LoadString(IDS_STRING172_EN);
		pWnd->SetWindowText(strItem1);

		
		pWnd = GetDlgItem(IDC_ADDPAPERTYPE);   
		strItem1.LoadString(IDS_STRING335_EN);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_USEPAPERTYPE);   
		strItem1.LoadString(IDS_STRING336_EN);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_PAPERTYPESELECTTEXT);   
		strItem1.LoadString(IDS_STRING337_EN);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_OUTPUTX);
		strItem1.LoadString(IDS_STRING338_EN);
		pWnd->SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_PLOTSQUARE);
//		strItem1.LoadString(IDS_STRING269_EN);
//		pWnd->SetWindowText(strItem1);
		//20100413 语言 走纸误差英文
//		pWnd = GetDlgItem(IDC_PLOTTERERROR2);
//		strItem1.LoadString(IDS_STRING317_EN);
//		pWnd->SetWindowText(strItem1);

//		pWnd = GetDlgItem(IDC_OUTPUTY2);
//		strItem1.LoadString(IDS_STRING320_EN);
//		pWnd->SetWindowText(strItem1);

//		pWnd = GetDlgItem(IDC_PLOTSQUARE3);
//		strItem1.LoadString(IDS_STRING322_EN);
//		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC23);
		strItem1.LoadString(IDS_STRING331_EN);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_STATIC22);
		strItem1.LoadString(IDS_STRING332_EN);
		pWnd->SetWindowText(strItem1);
		//20101206 笔号刀号 语言
		pWnd = GetDlgItem(IDC_STATIC17);
		strItem1.LoadString(IDS_STRING327_EN);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_STATIC18);
		strItem1.LoadString(IDS_STRING328_EN);
		pWnd->SetWindowText(strItem1);
		//20100413 打印启动方式 语言
		pWnd = GetDlgItem(IDC_STATIC21);
		strItem1.LoadString(IDS_STRING324_EN);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_SPRAY5);
		strItem1.LoadString(IDS_STRING326_EN);
		pWnd->SetWindowText(strItem1);
		//pWnd->EnableWindow(false);//20140729
		//**********************
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING180_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182_EN);
		pWnd->SetWindowText(strItem1);
	}

	//20141224 rwt 添加纸张类型选择功能
	ReadAddPaperTypeFile();

	if ((!m_bConnected) || m_bNetworkBusy)
	{
		pWnd = GetDlgItem(IDC_UNIDIRECTIONAL);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_BIDIRECTIONAL);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAY1);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAY2);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAY3);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAY4);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_MALPOSEDERROR);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_GRAPHWIDTH);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_PMALPOSEDGRAPH);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAYERRORY12);
		pWnd->EnableWindow(false);
//		pWnd = GetDlgItem(IDC_SPRAYERRORY23);
//		pWnd->EnableWindow(false);
//		pWnd = GetDlgItem(IDC_SPRAYERRORY34);
//		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAYERRORX12);
		pWnd->EnableWindow(false);
//		pWnd = GetDlgItem(IDC_SPRAYERRORX23);
//		pWnd->EnableWindow(false);
//		pWnd = GetDlgItem(IDC_SPRAYERRORX34);
//		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAYERROR);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_PLOTTERYERROR);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_PLOTTERXERROR);
		pWnd->EnableWindow(false);

		pWnd = GetDlgItem(IDC_MALPOSEDERROR2);//20101206 笔号刀号
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_MALPOSEDERROR3);
		pWnd->EnableWindow(false);

		pWnd = GetDlgItem(IDC_PLOTSQUARE);
		pWnd->EnableWindow(false);
		//20100413 通讯未成功设置为不可使用
		pWnd = GetDlgItem(IDC_PLOTSQUARE3);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_PLOTTERYERROR2);
		pWnd->EnableWindow(false);

		pWnd = GetDlgItem(IDC_SPRAY5);
		pWnd->EnableWindow(false);
		//******
		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(false);
	}

	//20141223 rwt 选择纸张类型功能
	
	
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupPlotter::OnOK() //20101206 设置成功
{
	// TODO: Add extra validation here
	UpdateData(true);

	theApp.m_nAutoPrintStyle = m_nAutoPrintStyle;
	CString s1;
	switch (m_cSprinklerCount)
	{
	case 0:
		if (m_nLanguage == 0)
			s1.LoadString(IDS_STRING117);
		else
			s1.LoadString(IDS_STRING117_EN);
		AfxMessageBox(s1);
		return;
	case 1:
		m_sSprayErrorX12 = m_sSprayErrorX12Bak;
		m_sSprayErrorY12 = m_sSprayErrorY12Bak;
		m_sSprayErrorX23 = m_sSprayErrorX23Bak;
		m_sSprayErrorY23 = m_sSprayErrorY23Bak;
		m_sSprayErrorX34 = m_sSprayErrorX34Bak;
		m_sSprayErrorY34 = m_sSprayErrorY34Bak;
		break;
	case 2:
		switch (m_cSprinklerNo)
		{
		case 0:
			m_sSprayErrorX23 = m_sSprayErrorX23Bak;
			m_sSprayErrorY23 = m_sSprayErrorY23Bak;
			m_sSprayErrorX34 = m_sSprayErrorX34Bak;
			m_sSprayErrorY34 = m_sSprayErrorY34Bak;
			break;
		case 1:
			m_sSprayErrorX12 = m_sSprayErrorX12Bak;
			m_sSprayErrorY12 = m_sSprayErrorY12Bak;
			m_sSprayErrorX34 = m_sSprayErrorX34Bak;
			m_sSprayErrorY34 = m_sSprayErrorY34Bak;
			break;
		case 2:
			m_sSprayErrorX12 = m_sSprayErrorX12Bak;
			m_sSprayErrorY12 = m_sSprayErrorY12Bak;
			m_sSprayErrorX23 = m_sSprayErrorX23Bak;
			m_sSprayErrorY23 = m_sSprayErrorY23Bak;
			break;
		default:
			m_cSprinklerNo = 0;
			m_sSprayErrorX23 = m_sSprayErrorX23Bak;
			m_sSprayErrorY23 = m_sSprayErrorY23Bak;
			m_sSprayErrorX34 = m_sSprayErrorX34Bak;
			m_sSprayErrorY34 = m_sSprayErrorY34Bak;
			break;
		}
		break;
	case 3:
		switch (m_cSprinklerNo)
		{
		case 0:
			m_sSprayErrorX34 = m_sSprayErrorX34Bak;
			m_sSprayErrorY34 = m_sSprayErrorY34Bak;
			break;
		case 1:
			m_sSprayErrorX12 = m_sSprayErrorX12Bak;
			m_sSprayErrorY12 = m_sSprayErrorY12Bak;
			break;
		default:
			m_cSprinklerNo = 0;
			m_sSprayErrorX34 = m_sSprayErrorX34Bak;
			m_sSprayErrorY34 = m_sSprayErrorY34Bak;
			break;
		}
		break;
	default:
		m_cSprinklerNo = 0;
		m_cSprinklerCount = 4;
		break;
	}

	if (m_cPrintStyle == 1)
		theApp.m_dGraphWidth = this->m_dGraphWidth;// 当前打印模式是双向打印
	else
		m_dMalposedError = m_dMalposedErrorBak;// 当设置未单向打印时，恢复原来双向打印误差的设置
theApp.WriteIniFile();
	UpdateData(false);

	CDialog::OnOK();
}
/*
void CSetupPlotter::OnPlotSquare() 
{
	// TODO: Add your control notification handler code here
	double dScaleX1,dScaleY1;

	UpdateData(true);

	if ((m_dPlotterXError >= 900.0) && (m_dPlotterXError <= 1100.0) && (m_dPlotterYError >= 900.0) && (m_dPlotterYError <= 1100.0))
	{
		dScaleX1 = 1000.0 / m_dPlotterXError;
		dScaleY1 = 1000.0 / m_dPlotterYError;
		m_pParent->SendErrorBlock(dScaleX1,dScaleY1);
	}
}
*/
void CSetupPlotter::OnSpray1() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if (m_bSpray1)
	{
		if ((!m_bSpray2) && (m_bSpray3 || m_bSpray4))
		{
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING116);
			else
				s1.LoadString(IDS_STRING116_EN);
			AfxMessageBox(s1);
			m_bSpray1 = false;
			UpdateData(false);
			return;
		}
	}
	DisplaySprayError();
}

void CSetupPlotter::OnSpray2() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if (m_bSpray2)
	{
		if ((!m_bSpray3) && m_bSpray4)
		{
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING116);
			else
				s1.LoadString(IDS_STRING116_EN);
			AfxMessageBox(s1);
			m_bSpray2 = false;
			UpdateData(false);
			return;
		}
	}
	else if (m_bSpray1 && (m_bSpray3 || m_bSpray4))
	{
		m_bSpray3 = false;
		m_bSpray4 = false;
		UpdateData(false);
	}
	DisplaySprayError();
}

void CSetupPlotter::OnSpray3() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if (m_bSpray3)
	{
		if ((!m_bSpray2) && m_bSpray1)
		{
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING116);
			else
				s1.LoadString(IDS_STRING116_EN);
			AfxMessageBox(s1);
			m_bSpray3 = false;
			UpdateData(false);
			return;
		}
	}
	else if (m_bSpray2 && m_bSpray4)
	{
		m_bSpray4 = false;
		UpdateData(false);
	}
	DisplaySprayError();
}

void CSetupPlotter::OnSpray4() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if (m_bSpray4)
	{
		if ((!m_bSpray3) && (m_bSpray2 || m_bSpray1))
		{
			CString s1;
			if (m_nLanguage == 0)
				s1.LoadString(IDS_STRING116);
			else
				s1.LoadString(IDS_STRING116_EN);
			AfxMessageBox(s1);
			m_bSpray4 = false;
			UpdateData(false);
			return;
		}
	}
	DisplaySprayError();
}

void CSetupPlotter::DisplaySprayError()
{
	CString strItem1;
	CWnd* pWnd;

	if (m_cPrintStyle == 0)
	{
		pWnd = GetDlgItem(IDC_MALPOSEDERROR);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_GRAPHWIDTH);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_PMALPOSEDGRAPH);
		pWnd->EnableWindow(false);
	}
	else
	{
		pWnd = GetDlgItem(IDC_MALPOSEDERROR);
		pWnd->EnableWindow(true);
		pWnd = GetDlgItem(IDC_GRAPHWIDTH);
		pWnd->EnableWindow(true);
		pWnd = GetDlgItem(IDC_PMALPOSEDGRAPH);
		pWnd->EnableWindow(true);
	}
	if (m_bSpray1)
	{
		m_cSprinklerNo = 0;
		m_cSprinklerCount = 1;
	}
	else if (m_bSpray2)
	{
		m_cSprinklerNo = 1;
		m_cSprinklerCount = 1;
	}
	else if (m_bSpray3)
	{
		m_cSprinklerNo = 2;
		m_cSprinklerCount = 1;
	}
	else if (m_bSpray4)
	{
		m_cSprinklerNo = 3;
		m_cSprinklerCount = 1;
	}
	else
	{
		m_cSprinklerNo = 0;
		m_cSprinklerCount = 0;
	}
	if (m_bSpray1 && m_bSpray2)
	{
		pWnd = GetDlgItem(IDC_SPRAYERRORY12);
		pWnd->EnableWindow(true);
		pWnd = GetDlgItem(IDC_SPRAYERRORX12);
		pWnd->EnableWindow(true);
		m_cSprinklerCount++;
	}
	else
	{
		pWnd = GetDlgItem(IDC_SPRAYERRORY12);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAYERRORX12);
		pWnd->EnableWindow(false);
	}
	if (m_bSpray2 && m_bSpray3)
	{
//		pWnd = GetDlgItem(IDC_SPRAYERRORY23);
//		pWnd->EnableWindow(true);
//		pWnd = GetDlgItem(IDC_SPRAYERRORX23);
//		pWnd->EnableWindow(true);
		m_cSprinklerCount++;
	}
	else
	{
//		pWnd = GetDlgItem(IDC_SPRAYERRORY23);
//		pWnd->EnableWindow(false);
//		pWnd = GetDlgItem(IDC_SPRAYERRORX23);
//		pWnd->EnableWindow(false);
	}
	if (m_bSpray3 && m_bSpray4)
	{
//		pWnd = GetDlgItem(IDC_SPRAYERRORY34);
//		pWnd->EnableWindow(true);
//		pWnd = GetDlgItem(IDC_SPRAYERRORX34);
//		pWnd->EnableWindow(true);
		m_cSprinklerCount++;
	}
	else
	{
//		pWnd = GetDlgItem(IDC_SPRAYERRORY34);
//		pWnd->EnableWindow(false);
//		pWnd = GetDlgItem(IDC_SPRAYERRORX34);
//		pWnd->EnableWindow(false);
	}
	if (m_cSprinklerCount < 2)
	{
		pWnd = GetDlgItem(IDC_SPRAYERROR);
		pWnd->EnableWindow(false);
	}
	else
	{
		pWnd = GetDlgItem(IDC_SPRAYERROR);
		pWnd->EnableWindow(true);
	}
}

void CSetupPlotter::OnUnidirectional() 
{
	// TODO: Add your control notification handler code here1
	UpdateData(true);
	if (GetCheckedRadioButton(IDC_UNIDIRECTIONAL,IDC_BIDIRECTIONAL) == IDC_UNIDIRECTIONAL)
		m_cPrintStyle = 0;
	else
		m_cPrintStyle = 1;
	DisplaySprayError();
}

void CSetupPlotter::OnBidirectional() 
{
	// TODO: Add your control notification handler code here2
	UpdateData(true);
	if (GetCheckedRadioButton(IDC_UNIDIRECTIONAL,IDC_BIDIRECTIONAL) == IDC_UNIDIRECTIONAL)
		m_cPrintStyle = 0;
	else
		m_cPrintStyle = 1;
	DisplaySprayError();
}

void CSetupPlotter::OnPMalposedGraph() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if ((m_dMalposedError > 6) || (m_dMalposedError < -6))
		return;
	GetDlgItem(IDC_PMALPOSEDGRAPH)->EnableWindow(false);
	m_pParent->PrintErrorY(m_dGraphWidth,m_dMalposedError);
	GetDlgItem(IDC_PMALPOSEDGRAPH)->EnableWindow(true);
}

void CSetupPlotter::OnSprayError() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);

	short sSprayError[6];

	switch (m_cSprinklerCount)
	{
	case 0:
		return;
	case 1:
		return;
	case 2:
		switch (m_cSprinklerNo)
		{
		case 0:
			if ((m_sSprayErrorX12 > 150) || (m_sSprayErrorX12 < 0))
				return;
			if ((m_sSprayErrorY12 > 20) || (m_sSprayErrorY12 < -20))
				return;
			sSprayError[0] = m_sSprayErrorX12;
			sSprayError[1] = m_sSprayErrorX23Bak;
			sSprayError[2] = m_sSprayErrorX34Bak;
			sSprayError[3] = m_sSprayErrorY12;
			sSprayError[4] = m_sSprayErrorY23Bak;
			sSprayError[5] = m_sSprayErrorY34Bak;
			break;
		case 1:
			if ((m_sSprayErrorX23 > 150) || (m_sSprayErrorX23 < 0))
				return;
			if ((m_sSprayErrorY23 > 20) || (m_sSprayErrorY23 < -20))
				return;
			sSprayError[0] = m_sSprayErrorX12Bak;
			sSprayError[1] = m_sSprayErrorX23;
			sSprayError[2] = m_sSprayErrorX34Bak;
			sSprayError[3] = m_sSprayErrorY12Bak;
			sSprayError[4] = m_sSprayErrorY23;
			sSprayError[5] = m_sSprayErrorY34Bak;
			break;
		case 2:
			if ((m_sSprayErrorX34 > 150) || (m_sSprayErrorX34 < 0))
				return;
			if ((m_sSprayErrorY34 > 20) || (m_sSprayErrorY34 < -20))
				return;
			sSprayError[0] = m_sSprayErrorX12Bak;
			sSprayError[1] = m_sSprayErrorX23Bak;
			sSprayError[2] = m_sSprayErrorX34;
			sSprayError[3] = m_sSprayErrorY12Bak;
			sSprayError[4] = m_sSprayErrorY23Bak;
			sSprayError[5] = m_sSprayErrorY34;
			break;
		default:
			m_cSprinklerNo = 0;
			if ((m_sSprayErrorX12 > 150) || (m_sSprayErrorX12 < 0))
				return;
			if ((m_sSprayErrorY12 > 20) || (m_sSprayErrorY12 < -20))
				return;
			sSprayError[0] = m_sSprayErrorX12;
			sSprayError[1] = m_sSprayErrorX23Bak;
			sSprayError[2] = m_sSprayErrorX34Bak;
			sSprayError[3] = m_sSprayErrorY12;
			sSprayError[4] = m_sSprayErrorY23Bak;
			sSprayError[5] = m_sSprayErrorY34Bak;
			break;
		}
		break;
	case 3:
		switch (m_cSprinklerNo)
		{
		case 0:
			if ((m_sSprayErrorX12 > 150) || (m_sSprayErrorX12 < 0))
				return;
			if ((m_sSprayErrorY12 > 20) || (m_sSprayErrorY12 < -20))
				return;
			if ((m_sSprayErrorX23 > 150) || (m_sSprayErrorX23 < 0))
				return;
			if ((m_sSprayErrorY23 > 20) || (m_sSprayErrorY23 < -20))
				return;
			sSprayError[0] = m_sSprayErrorX12;
			sSprayError[1] = m_sSprayErrorX23;
			sSprayError[2] = m_sSprayErrorX34Bak;
			sSprayError[3] = m_sSprayErrorY12;
			sSprayError[4] = m_sSprayErrorY23;
			sSprayError[5] = m_sSprayErrorY34Bak;
			break;
		case 1:
			if ((m_sSprayErrorX23 > 150) || (m_sSprayErrorX23 < 0))
				return;
			if ((m_sSprayErrorY23 > 20) || (m_sSprayErrorY23 < -20))
				return;
			if ((m_sSprayErrorX34 > 150) || (m_sSprayErrorX34 < 0))
				return;
			if ((m_sSprayErrorY34 > 20) || (m_sSprayErrorY34 < -20))
				return;
			sSprayError[0] = m_sSprayErrorX12Bak;
			sSprayError[1] = m_sSprayErrorX23;
			sSprayError[2] = m_sSprayErrorX34;
			sSprayError[3] = m_sSprayErrorY12Bak;
			sSprayError[4] = m_sSprayErrorY23;
			sSprayError[5] = m_sSprayErrorY34;
			break;
		default:
			m_cSprinklerNo = 0;
			if ((m_sSprayErrorX12 > 150) || (m_sSprayErrorX12 < 0))
				return;
			if ((m_sSprayErrorY12 > 20) || (m_sSprayErrorY12 < -20))
				return;
			if ((m_sSprayErrorX23 > 150) || (m_sSprayErrorX23 < 0))
				return;
			if ((m_sSprayErrorY23 > 20) || (m_sSprayErrorY23 < -20))
				return;
			sSprayError[0] = m_sSprayErrorX12;
			sSprayError[1] = m_sSprayErrorX23;
			sSprayError[2] = m_sSprayErrorX34Bak;
			sSprayError[3] = m_sSprayErrorY12;
			sSprayError[4] = m_sSprayErrorY23;
			sSprayError[5] = m_sSprayErrorY34Bak;
			break;
		}
		break;
	default:
		m_cSprinklerNo = 0;
		m_cSprinklerCount = 4;
		if ((m_sSprayErrorX12 > 150) || (m_sSprayErrorX12 < 0))
			return;
		if ((m_sSprayErrorY12 > 20) || (m_sSprayErrorY12 < -20))
			return;
		if ((m_sSprayErrorX23 > 150) || (m_sSprayErrorX23 < 0))
			return;
		if ((m_sSprayErrorY23 > 20) || (m_sSprayErrorY23 < -20))
			return;
		if ((m_sSprayErrorX34 > 150) || (m_sSprayErrorX34 < 0))
			return;
		if ((m_sSprayErrorY34 > 20) || (m_sSprayErrorY34 < -20))
			return;
		sSprayError[0] = m_sSprayErrorX12;
		sSprayError[1] = m_sSprayErrorX23;
		sSprayError[2] = m_sSprayErrorX34;
		sSprayError[3] = m_sSprayErrorY12;
		sSprayError[4] = m_sSprayErrorY23;
		sSprayError[5] = m_sSprayErrorY34;
		break;
	}
	GetDlgItem(IDC_SPRAYERROR)->EnableWindow(false);
	m_pParent->SprayError(sSprayError);
	GetDlgItem(IDC_SPRAYERROR)->EnableWindow(true);
}

//20100413测试走纸误差启动
/*
void CSetupPlotter::OnPlotsquare3() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if ((m_dOverlapError > 20.0) || (m_dOverlapError < -20.0))
		return;
//	GetDlgItem(IDC_PLOTSQUARE3)->EnableWindow(false);
//	m_pParent->PrintTrackPaperError(m_dOverlapError);
//	GetDlgItem(IDC_PLOTSQUARE3)->EnableWindow(true);
}
*/
/*
//20100413设置打开文件后自动打印
void CSetupPlotter::OnSelectAutoPrint() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	m_nAutoPrintStyle = 0;
}
*/
//20101206  全半刀
void CSetupPlotter::OnSetPenNo()
{
	UpdateData(true);
	if(m_cPenNo < 0 || m_cPenNo > 9)
		m_cPenNo = 1;
	//if(m_cPenNo == m_cCutNo)
	//	m_cPenNo = (m_cCutNo+1)%9;
}

void CSetupPlotter::OnSetCutNo()
{
	UpdateData(true);
	if(m_cCutNo < 0 || m_cCutNo > 9)
		m_cCutNo = 1;
	//if(m_cCutNo == m_cPenNo)
	//	m_cCutNo = (m_cPenNo+1)%9;
}

void CSetupPlotter::OnPrint()
{
	UpdateData(true);
	if(m_bPrint)
		g_cPrintCutSelect |= 0x01;
	else
		g_cPrintCutSelect &= 0xfe;

}

void CSetupPlotter::OnCut()
{
	UpdateData(true);
	if(m_bCut)
		g_cPrintCutSelect |= 0x02;
	else
		g_cPrintCutSelect &= 0xfd;
}

//20100413设置打开文件后自动打印
void CSetupPlotter::OnSelectAutoPrint() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if(m_nAutoPrintStyle == 1)
	{
		m_nAutoPrintStyle = 0;
	}
	else
	{
		m_nAutoPrintStyle = 1;
	}
}

//20141219 rwt X轴长度校正值在不同类型纸张上的校正参数
void CSetupPlotter::OnAddPaperType()
{
	UpdateData(true);
	int iPaperTypeTotalNum,iPaperTypeCurrentNum;
	CString str1,str2;
	CString strTemp;
	/*创建配置文件
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE]; 
	  ::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	CString str;
	  str.Format(_T("%s%s"),szDrive,szDir);
	
	m_pszProfileName=_tcsdup(str+"rpplot.ini");
	*/
	//检查ccomboboxex控件中的编辑框内容
	//如果与之前不同:    则添加到配置文件
	
	//m_PaperTypeSelect.GetWindowText(strTemp);
	//int ret = m_PaperTypeSelect.FindString(0, strTemp);
	//if ((m_PaperTypeSelect.FindStringExact(-1, strTemp)) == CB_ERR)
	//{
	//	m_PaperTypeSelect.AddString(strTemp);
	//}
		iPaperTypeCurrentNum= m_PaperTypeSelect.GetCurSel();
		iPaperTypeTotalNum = m_PaperTypeSelect.GetCount();
		//if (iPaperTypeTotalNum >= 20)
		//{
		//	AfxMessageBox("最多储存20种纸张的类型");
		//	return;
		//}
		//if (iPaperTypeCurrentNum == -1)
		//{
		//	iPaperTypeCurrentNum = iPaperTypeTotalNum;
		//}
		
		str1.Format("%d", iPaperTypeTotalNum);
		theApp.WriteProfileString(_T("PaperType"), _T("iPaperTypeTotalNum"), str1);
		str2.Format("%d", iPaperTypeCurrentNum);
		theApp.WriteProfileString(_T("PaperType"), _T("iPaperTypeCurrentNum"), str2);
		str2.Format("PaperType[%d]", iPaperTypeCurrentNum);
		str1.Format("%f",m_dPlotterYError);
		theApp.WriteProfileString(_T("PaperType"), str2, str1);
		//str2.Format("PaperTypeName[%d]",iPaperTypeCurrentNum);
		//theApp.WriteProfileString(_T("PaperType"), str2, strTemp);
		
	//}

	//iPaperTypeNum = m_PaperTypeSelect.GetCurSel();
	//str1.Format("%d", iPaperTypeNum);
	//BOOL rslt = theApp.WriteProfileString(_T("PaperType"), _T("iPaperTypeNum"), str1);
	//m_PaperTypeSelect.GetWindowText(strTemp);
}
void CSetupPlotter::OnUsePaperType()
{
	UpdateData(true);
	int iPaperTypeCurrentNum = m_PaperTypeSelect.GetCurSel();
	CString str;
	str.Format("%d", iPaperTypeCurrentNum);
	theApp.WriteProfileString(_T("PaperType"), _T("iPaperTypeCurrentNum"), str);
	str.Format("PaperType[%d]", iPaperTypeCurrentNum);
	m_dPlotterYError = atof(theApp.GetProfileString(_T("PaperType"), _T(str), "1000.000000"));
	UpdateData(false);
}

void CSetupPlotter::ReadAddPaperTypeFile()
{
	int iPaperTypeTotalNum;
	int iPaperTypeCurrentNum;
	CString str;
	//读当前纸张类型的垂直方向误差校正
	iPaperTypeTotalNum = theApp.GetProfileInt(_T("PaperType"),_T("iPaperTypeTotalNum"),2);
	iPaperTypeCurrentNum = theApp.GetProfileInt(_T("PaperType"),_T("iPaperTypeCurrentNum"),0);
	//m_PaperTypeSelect.SetCurSel(iPaperTypeCurrentNum);
	//str.Format("PaperType[%d]", iPaperTypeCurrentNum);
	//m_dPlotterYError = (double)(theApp.GetProfileInt(_T("PaperType"), _T(str), 1000));
	
	//加载纸张的所有类型到下拉菜单
	//int iCount = m_PaperTypeSelect.GetCount();
	/*
	for(int i = 0;i < iPaperTypeTotalNum+1;i++)
	{
		str.Format("PaperTypeName[%d]", i);
		str = theApp.GetProfileString(_T("PaperType"), _T(str), "白纸50克");
		m_PaperTypeSelect.AddString(str);
	}
	m_PaperTypeSelect.SetCurSel(iPaperTypeCurrentNum);
	str.Format("PaperType[%d]", iPaperTypeCurrentNum);
	m_dPlotterYError = (double)(theApp.GetProfileInt(_T("PaperType"), _T(str), 1000));
	*/
	m_PaperTypeSelect.SetCurSel(iPaperTypeCurrentNum);
	str.Format("PaperType[%d]", iPaperTypeCurrentNum);
	//m_dPlotterYError = atof(theApp.GetProfileString(_T("PaperType"), _T(str), "1000.000000"));
	
	UpdateData(false);
}

void CSetupPlotter::WriteAddPaperTypeFile()
{
	CString str;
	CString strTemp;
	int nIndex = m_PaperTypeSelect.GetCurSel();	 
	//将获取的值存放到CString类型变量strTemp  /取当前内容
	//((CComboBox*)GetDlgItem(IDC_COMBO_CF))->GetWindowText(strTemp); 
    //如果定义了关联的变量，例如m_combo,可以直接获取：  	 
	m_PaperTypeSelect.GetWindowText(strTemp);  
	str.Format("%f",m_PaperTypeSelect);
	theApp.WriteProfileString(_T("PaperType"), _T("PaperWidth"),str);
}
