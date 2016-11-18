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
	m_nAutoPrintStyle = theApp.m_nAutoPrintStyle;//20100413是否自动打印
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
	//}}AFX_DATA_INIT
}


void CSetupPlotter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupPlotter)
	DDX_Check(pDX, IDC_SPRAY1, m_bSpray1);
	DDX_Check(pDX, IDC_SPRAY2, m_bSpray2);
	DDX_Check(pDX, IDC_SPRAY3, m_bSpray3);
	DDX_Check(pDX, IDC_SPRAY4, m_bSpray4);
	//20100413是否自动打印
	DDX_Check(pDX, IDC_SPRAY5, m_nAutoPrintStyle);
	//**********
	DDX_Text(pDX, IDC_MALPOSEDERROR, m_dMalposedError);
	DDV_MinMaxDouble(pDX, m_dMalposedError, -6., 6.);
	//20100413 设置文本编辑控件属性
	DDX_Text(pDX, IDC_PLOTTERYERROR2, m_dOverlapError);
	DDV_MinMaxDouble(pDX, m_dOverlapError, -20., 20.);
	//*******
	DDX_Text(pDX, IDC_GRAPHWIDTH, m_dGraphWidth);
	DDV_MinMaxDouble(pDX, m_dGraphWidth, 0., 3030.);	//20100926 修改可设置纸张范围为3030mm
	DDX_Text(pDX, IDC_SPRAYERRORX12, m_sSprayErrorX12);
	DDV_MinMaxInt(pDX, m_sSprayErrorX12, 0, 150);
	DDX_Text(pDX, IDC_SPRAYERRORY12, m_sSprayErrorY12);
	DDV_MinMaxInt(pDX, m_sSprayErrorY12, -20, 20);
	DDX_Text(pDX, IDC_SPRAYERRORX23, m_sSprayErrorX23);
	DDV_MinMaxInt(pDX, m_sSprayErrorX23, 0, 150);
	DDX_Text(pDX, IDC_SPRAYERRORX34, m_sSprayErrorX34);
	DDV_MinMaxInt(pDX, m_sSprayErrorX34, 0, 150);
	DDX_Text(pDX, IDC_SPRAYERRORY23, m_sSprayErrorY23);
	DDV_MinMaxInt(pDX, m_sSprayErrorY23, -20, 20);
	DDX_Text(pDX, IDC_SPRAYERRORY34, m_sSprayErrorY34);
	DDV_MinMaxInt(pDX, m_sSprayErrorY34, -20, 20);
	DDX_Text(pDX, IDC_PLOTTERXERROR, m_dPlotterXError);
	DDV_MinMaxDouble(pDX, m_dPlotterXError, 900., 1100.);
	DDX_Text(pDX, IDC_PLOTTERYERROR, m_dPlotterYError);
	DDV_MinMaxDouble(pDX, m_dPlotterYError, 900., 1100.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupPlotter, CDialog)
	//{{AFX_MSG_MAP(CSetupPlotter)
	ON_BN_CLICKED(IDC_PLOTSQUARE, OnPlotSquare)
	ON_BN_CLICKED(IDC_SPRAY1, OnSpray1)
	ON_BN_CLICKED(IDC_SPRAY2, OnSpray2)
	ON_BN_CLICKED(IDC_SPRAY3, OnSpray3)
	ON_BN_CLICKED(IDC_SPRAY4, OnSpray4)
	ON_BN_CLICKED(IDC_UNIDIRECTIONAL, OnUnidirectional)
	ON_BN_CLICKED(IDC_BIDIRECTIONAL, OnBidirectional)
	ON_BN_CLICKED(IDC_PMALPOSEDGRAPH, OnPMalposedGraph)
	ON_BN_CLICKED(IDC_SPRAYERROR, OnSprayError)
	ON_BN_CLICKED(IDC_PLOTSQUARE3, OnPlotsquare3)
	ON_BN_CLICKED(IDC_SPRAY5, OnSelectAutoPrint)
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

	CString strItem1;
	CWnd* pWnd;

	if (m_cPrintStyle == 0)
		CheckRadioButton(IDC_UNIDIRECTIONAL,IDC_BIDIRECTIONAL,IDC_UNIDIRECTIONAL);
	else
		CheckRadioButton(IDC_UNIDIRECTIONAL,IDC_BIDIRECTIONAL,IDC_BIDIRECTIONAL);

	//当前主板程序是V3.23之前的版本，走纸误差锁定不能设置20100413
	if(g_nNewVersion == 0)
	{
		pWnd = GetDlgItem(IDC_PLOTSQUARE3);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_PLOTTERYERROR2);
		pWnd->EnableWindow(false);			
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
			pWnd = GetDlgItem(IDC_SPRAYERRORY23);
			pWnd->EnableWindow(false);
			pWnd = GetDlgItem(IDC_SPRAYERRORY34);
			pWnd->EnableWindow(false);
			pWnd = GetDlgItem(IDC_SPRAYERRORX12);
			pWnd->EnableWindow(false);
			pWnd = GetDlgItem(IDC_SPRAYERRORX23);
			pWnd->EnableWindow(false);
			pWnd = GetDlgItem(IDC_SPRAYERRORX34);
			pWnd->EnableWindow(false);
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
					pWnd = GetDlgItem(IDC_SPRAYERRORY23);
					pWnd->EnableWindow(false);
					pWnd = GetDlgItem(IDC_SPRAYERRORY34);
					pWnd->EnableWindow(false);
					pWnd = GetDlgItem(IDC_SPRAYERRORX23);
					pWnd->EnableWindow(false);
					pWnd = GetDlgItem(IDC_SPRAYERRORX34);
					pWnd->EnableWindow(false);
					break;
				case 1:
					m_bSpray1 = FALSE;
					m_bSpray2 = TRUE;
					m_bSpray3 = TRUE;
					m_bSpray4 = FALSE;
					pWnd = GetDlgItem(IDC_SPRAYERRORY12);
					pWnd->EnableWindow(false);
					pWnd = GetDlgItem(IDC_SPRAYERRORY34);
					pWnd->EnableWindow(false);
					pWnd = GetDlgItem(IDC_SPRAYERRORX12);
					pWnd->EnableWindow(false);
					pWnd = GetDlgItem(IDC_SPRAYERRORX34);
					pWnd->EnableWindow(false);
					break;
				default:
					m_bSpray1 = FALSE;
					m_bSpray2 = FALSE;
					m_bSpray3 = TRUE;
					m_bSpray4 = TRUE;
					pWnd = GetDlgItem(IDC_SPRAYERRORY12);
					pWnd->EnableWindow(false);
					pWnd = GetDlgItem(IDC_SPRAYERRORY23);
					pWnd->EnableWindow(false);
					pWnd = GetDlgItem(IDC_SPRAYERRORX12);
					pWnd->EnableWindow(false);
					pWnd = GetDlgItem(IDC_SPRAYERRORX23);
					pWnd->EnableWindow(false);
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
				pWnd = GetDlgItem(IDC_SPRAYERRORY34);
				pWnd->EnableWindow(false);
				pWnd = GetDlgItem(IDC_SPRAYERRORX34);
				pWnd->EnableWindow(false);
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
	/*20150724
	if (m_nLanguage == 0)
	{
		this->m_dGraphWidth=theApp.m_dGraphWidth;
		this->m_dMalposedError = theApp.m_dMalposedError;
		this->m_dOverlapError = theApp.m_dOverlapError;
		this->m_dPlotterYError = theApp.m_dPlotterYError;
		this->m_dPlotterXError = theApp.m_dPlotterXError;
	}
	else
	{
		this->m_dGraphWidth=theApp.m_dGraphWidth/2.54;
		this->m_dMalposedError = theApp.m_dMalposedError/2.54;
		this->m_dOverlapError = theApp.m_dOverlapError/2.54;
		this->m_dPlotterYError = theApp.m_dPlotterYError/2.54;
		this->m_dPlotterXError = theApp.m_dPlotterXError/2.54;
	}
	*/
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
		pWnd = GetDlgItem(IDC_SPRAY3);
		strItem1.LoadString(IDS_STRING216);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPRAY4);
		strItem1.LoadString(IDS_STRING218);
		pWnd->SetWindowText(strItem1);
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
		pWnd = GetDlgItem(IDC_STATIC8);
		strItem1.LoadString(IDS_STRING232);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC9);
		strItem1.LoadString(IDS_STRING234);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC10);
		strItem1.LoadString(IDS_STRING178);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC11);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC12);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC13);
		strItem1.LoadString(IDS_STRING236);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC14);
		strItem1.LoadString(IDS_STRING238);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC15);
		strItem1.LoadString(IDS_STRING240);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC16);
		strItem1.LoadString(IDS_STRING178);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC17);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC18);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPRAYERROR);
		strItem1.LoadString(IDS_STRING242);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_PLOTTERERROR);
		strItem1.LoadString(IDS_STRING170);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_OUTPUTY);
		strItem1.LoadString(IDS_STRING172);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_OUTPUTX);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_PLOTSQUARE);
		strItem1.LoadString(IDS_STRING269);
		pWnd->SetWindowText(strItem1);
		//20100413 语言 走纸误差中文
		pWnd = GetDlgItem(IDC_PLOTTERERROR2);
		strItem1.LoadString(IDS_STRING317);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_OUTPUTY2);
		strItem1.LoadString(IDS_STRING320);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_PLOTSQUARE3);
		strItem1.LoadString(IDS_STRING322);
		pWnd->SetWindowText(strItem1);
		//20100413 打印启动方式 语言
		pWnd = GetDlgItem(IDC_STATIC21);
		strItem1.LoadString(IDS_STRING324);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_SPRAY5);
		strItem1.LoadString(IDS_STRING326);
		pWnd->SetWindowText(strItem1);
		//**********************
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING180);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182);
		pWnd->SetWindowText(strItem1);

		/*
		pWnd = GetDlgItem(IDC_MM_4);
		strItem1.LoadString(IDS_STRING161);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_5);
		strItem1.LoadString(IDS_STRING161);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_6);
		strItem1.LoadString(IDS_STRING161);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_7);
		strItem1.LoadString(IDS_STRING161);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_8);
		strItem1.LoadString(IDS_STRING161);
		pWnd->SetWindowText(strItem1);
		*/
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
		pWnd = GetDlgItem(IDC_SPRAY3);
		strItem1.LoadString(IDS_STRING216_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPRAY4);
		strItem1.LoadString(IDS_STRING218_EN);
		pWnd->SetWindowText(strItem1);
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
		pWnd = GetDlgItem(IDC_STATIC8);
		strItem1.LoadString(IDS_STRING232_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC9);
		strItem1.LoadString(IDS_STRING234_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC10);
		strItem1.LoadString(IDS_STRING178_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC11);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC12);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC13);
		strItem1.LoadString(IDS_STRING236_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC14);
		strItem1.LoadString(IDS_STRING238_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC15);
		strItem1.LoadString(IDS_STRING240_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC16);
		strItem1.LoadString(IDS_STRING178_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC17);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC18);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPRAYERROR);
		strItem1.LoadString(IDS_STRING242_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_PLOTTERERROR);
		strItem1.LoadString(IDS_STRING170_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_OUTPUTY);
		strItem1.LoadString(IDS_STRING172_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_OUTPUTX);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_PLOTSQUARE);
		strItem1.LoadString(IDS_STRING269_EN);
		pWnd->SetWindowText(strItem1);
		//20100413 语言 走纸误差英文
		pWnd = GetDlgItem(IDC_PLOTTERERROR2);
		strItem1.LoadString(IDS_STRING317_EN);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_OUTPUTY2);
		strItem1.LoadString(IDS_STRING320_EN);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_PLOTSQUARE3);
		strItem1.LoadString(IDS_STRING322_EN);
		pWnd->SetWindowText(strItem1);
		//20100413 打印启动方式 语言
		pWnd = GetDlgItem(IDC_STATIC21);
		strItem1.LoadString(IDS_STRING324_EN);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_SPRAY5);
		strItem1.LoadString(IDS_STRING326_EN);
		pWnd->SetWindowText(strItem1);
		//**********************
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING180_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182_EN);
		pWnd->SetWindowText(strItem1);

		/*
		pWnd = GetDlgItem(IDC_MM_4);
		strItem1.LoadString(IDS_STRING161_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_5);
		strItem1.LoadString(IDS_STRING161_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_6);
		strItem1.LoadString(IDS_STRING161_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_7);
		strItem1.LoadString(IDS_STRING161_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_8);
		strItem1.LoadString(IDS_STRING161_EN);
		pWnd->SetWindowText(strItem1);
		*/
	}

	//20150725
	if (theApp.m_nMeasureUnit == 0)
	{
		
		pWnd = GetDlgItem(IDC_MM_4);
		strItem1.LoadString(IDS_STRING161);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_5);
		strItem1.LoadString(IDS_STRING161);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_6);
		strItem1.LoadString(IDS_STRING161);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_7);
		strItem1.LoadString(IDS_STRING161);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_8);
		strItem1.LoadString(IDS_STRING161);
		pWnd->SetWindowText(strItem1);
	}
	else if(theApp.m_nMeasureUnit == 1)
	{
		
		pWnd = GetDlgItem(IDC_MM_4);
		strItem1.LoadString(IDS_STRING161_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_5);
		strItem1.LoadString(IDS_STRING161_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_6);
		strItem1.LoadString(IDS_STRING161_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_7);
		strItem1.LoadString(IDS_STRING161_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_8);
		strItem1.LoadString(IDS_STRING161_EN);
		pWnd->SetWindowText(strItem1);
	}

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
		pWnd = GetDlgItem(IDC_SPRAYERRORY23);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAYERRORY34);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAYERRORX12);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAYERRORX23);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAYERRORX34);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAYERROR);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_PLOTTERYERROR);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_PLOTTERXERROR);
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

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupPlotter::OnOK() 
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
		if (theApp.m_nMeasureUnit == 0)
		{
			theApp.m_dGraphWidth = this->m_dGraphWidth;// 当前打印模式是双向打印
		}
		else if(theApp.m_nMeasureUnit == 1)
		{
			theApp.m_dGraphWidth = this->m_dGraphWidth*25.4;
		}
	else
		m_dMalposedError = m_dMalposedErrorBak;// 当设置未单向打印时，恢复原来双向打印误差的设置
	
	if (m_nLanguage == 0)
	{
		//theApp.m_dGraphWidth = this->m_dGraphWidth;
		theApp.m_dMalposedError = this->m_dMalposedError;
		theApp.m_dOverlapError = this->m_dOverlapError;
		theApp.m_dPlotterYError = this->m_dPlotterYError;
		theApp.m_dPlotterXError = this->m_dPlotterXError;
	}
	else
	{
		//theApp.m_dGraphWidth = this->m_dGraphWidth*2.54;
		theApp.m_dMalposedError = this->m_dMalposedError*2.54;
		theApp.m_dOverlapError = this->m_dOverlapError*2.54;
		theApp.m_dPlotterYError = this->m_dPlotterYError*2.54;
		theApp.m_dPlotterXError = this->m_dPlotterXError*2.54;
	}

	UpdateData(true);

	CDialog::OnOK();
}

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
		pWnd = GetDlgItem(IDC_SPRAYERRORY23);
		pWnd->EnableWindow(true);
		pWnd = GetDlgItem(IDC_SPRAYERRORX23);
		pWnd->EnableWindow(true);
		m_cSprinklerCount++;
	}
	else
	{
		pWnd = GetDlgItem(IDC_SPRAYERRORY23);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAYERRORX23);
		pWnd->EnableWindow(false);
	}
	if (m_bSpray3 && m_bSpray4)
	{
		pWnd = GetDlgItem(IDC_SPRAYERRORY34);
		pWnd->EnableWindow(true);
		pWnd = GetDlgItem(IDC_SPRAYERRORX34);
		pWnd->EnableWindow(true);
		m_cSprinklerCount++;
	}
	else
	{
		pWnd = GetDlgItem(IDC_SPRAYERRORY34);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SPRAYERRORX34);
		pWnd->EnableWindow(false);
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
void CSetupPlotter::OnPlotsquare3() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if ((m_dOverlapError > 20.0) || (m_dOverlapError < -20.0))
		return;
	GetDlgItem(IDC_PLOTSQUARE3)->EnableWindow(false);
	m_pParent->PrintTrackPaperError(m_dOverlapError);
	GetDlgItem(IDC_PLOTSQUARE3)->EnableWindow(true);
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
