// SetupPicture.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "SetupPicture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int ftoi(double f);
/////////////////////////////////////////////////////////////////////////////
// CSetupPicture dialog

CSetupPicture::CSetupPicture(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupPicture::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupPicture)
	m_dMark = 0.0;
	m_dPaperWidth = 0.0;
	m_nLineWidth = 0;
	m_bRotate = FALSE;
	m_dGapDown = 0.0;
	m_dGapRight = 0.0;
	m_bFont = FALSE;
	//}}AFX_DATA_INIT
}


void CSetupPicture::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupPicture)
	DDX_Text(pDX, IDC_MARK, m_dMark);
	DDV_MinMaxDouble(pDX, m_dMark, 5., 1000.);//20,1000 to inch
	DDX_Text(pDX, IDC_PAPER_WIDTH, m_dPaperWidth);
	DDV_MinMaxDouble(pDX, m_dPaperWidth, 50., 3530.);//20100926 修改可设置纸张范围为3530mm
	DDX_Text(pDX, IDC_LINEWIDTH, m_nLineWidth);
	DDV_MinMaxInt(pDX, m_nLineWidth, 1, 9);
	DDX_Check(pDX, IDC_ROTATE, m_bRotate);
	DDX_Text(pDX, IDC_GAP_DOWN, m_dGapDown);
	DDV_MinMaxDouble(pDX, m_dGapDown, 0., 500.);
	DDX_Text(pDX, IDC_GAP_RIGHT, m_dGapRight);
	DDV_MinMaxDouble(pDX, m_dGapRight, 0., 1300.);
	DDX_Check(pDX, IDC_FONT, m_bFont);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupPicture, CDialog)
	//{{AFX_MSG_MAP(CSetupPicture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupPicture message handlers

BOOL CSetupPicture::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strItem1;
	CWnd* pWnd;
	this->m_nLanguage=theApp.m_nLanguage;
	if (theApp.m_nMeasureUnit == 0)
	{
		this->m_dGapDown=theApp.m_dGapDown;
		this->m_dGapRight=theApp.m_dGapRight;
		this->m_dMark=theApp.m_dMarkSpace;
		this->m_dPaperWidth=theApp.m_dPaperWidth;
		
		pWnd = GetDlgItem(IDC_MM_0);
		strItem1.LoadString(IDS_STRING159);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_1);
		strItem1.LoadString(IDS_STRING159);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_2);
		strItem1.LoadString(IDS_STRING159);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_3);
		strItem1.LoadString(IDS_STRING159);
		pWnd->SetWindowText(strItem1);
	}
	else if(theApp.m_nMeasureUnit == 1)
	{
		this->m_dGapDown=int(theApp.m_dGapDown/25.4);
		this->m_dGapRight=int(theApp.m_dGapRight/25.4);
		this->m_dMark=int(theApp.m_dMarkSpace/25.4);
		this->m_dPaperWidth=int(theApp.m_dPaperWidth/25.4);
		
		pWnd = GetDlgItem(IDC_MM_0);
		strItem1.LoadString(IDS_STRING159_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_1);
		strItem1.LoadString(IDS_STRING159_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_2);
		strItem1.LoadString(IDS_STRING159_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_3);
		strItem1.LoadString(IDS_STRING159_EN);
		pWnd->SetWindowText(strItem1);
	}
	if (theApp.m_nRotate == 0)
		this->m_bRotate=false;
	else
		this->m_bRotate=true;
	if (theApp.m_nFont == 0)
		this->m_bFont=false;
	else
		this->m_bFont=true;
	
	//CString strItem1;
	//CWnd* pWnd;
	if (m_nLanguage == 0)
	{
		strItem1.LoadString(IDS_STRING200);
		SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_ROTATE);
		strItem1.LoadString(IDS_STRING160);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_FONT);
		strItem1.LoadString(IDS_STRING297);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_PAPERWIDTH);
		strItem1.LoadString(IDS_STRING166);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPACE);
		strItem1.LoadString(IDS_STRING168);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SETUPWIDTH);
		strItem1.LoadString(IDS_STRING174);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_XIANKUAN);
		strItem1.LoadString(IDS_STRING176);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_XIANGSU);
		strItem1.LoadString(IDS_STRING178);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING114);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC2);
		strItem1.LoadString(IDS_STRING162);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC3);
		strItem1.LoadString(IDS_STRING164);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING180);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182);
		pWnd->SetWindowText(strItem1);
		/*
		pWnd = GetDlgItem(IDC_MM_0);
		strItem1.LoadString(IDS_STRING159);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_1);
		strItem1.LoadString(IDS_STRING159);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_2);
		strItem1.LoadString(IDS_STRING159);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_3);
		strItem1.LoadString(IDS_STRING159);
		pWnd->SetWindowText(strItem1);
		*/
	}
	else
	{
		strItem1.LoadString(IDS_STRING200_EN);
		SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_ROTATE);
		strItem1.LoadString(IDS_STRING160_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_FONT);
		strItem1.LoadString(IDS_STRING297_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_PAPERWIDTH);
		strItem1.LoadString(IDS_STRING166_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPACE);
		strItem1.LoadString(IDS_STRING168_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SETUPWIDTH);
		strItem1.LoadString(IDS_STRING174_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_XIANKUAN);
		strItem1.LoadString(IDS_STRING176_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_XIANGSU);
		strItem1.LoadString(IDS_STRING178_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING114_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC2);
		strItem1.LoadString(IDS_STRING162_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC3);
		strItem1.LoadString(IDS_STRING164_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING180_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182_EN);
		pWnd->SetWindowText(strItem1);
		/*
		pWnd = GetDlgItem(IDC_MM_0);
		strItem1.LoadString(IDS_STRING159_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_1);
		strItem1.LoadString(IDS_STRING159_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_2);
		strItem1.LoadString(IDS_STRING159_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM_3);
		strItem1.LoadString(IDS_STRING159_EN);
		pWnd->SetWindowText(strItem1);
		*/
	}
	if ((!m_bConnected) || m_bNetworkBusy)
	{
		pWnd = GetDlgItem(IDC_LINEWIDTH);
		pWnd->EnableWindow(false);
	}
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupPicture::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	
	CDialog::OnOK();
}
