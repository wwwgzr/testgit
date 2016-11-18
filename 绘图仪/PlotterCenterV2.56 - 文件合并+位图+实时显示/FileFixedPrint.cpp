// FileFixedPrint.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "FileFixedPrint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileFixedPrint dialog


CFileFixedPrint::CFileFixedPrint(CWnd* pParent /*=NULL*/)
	: CDialog(CFileFixedPrint::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileFixedPrint)
	m_lFixedLength = 0;
	//}}AFX_DATA_INIT
}


void CFileFixedPrint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileFixedPrint)
	DDX_Text(pDX, IDC_FIXEDLENGTH, m_lFixedLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileFixedPrint, CDialog)
	//{{AFX_MSG_MAP(CFileFixedPrint)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileFixedPrint message handlers

BOOL CFileFixedPrint::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	this->m_nLanguage = theApp.m_nLanguage;
	this->m_lFixedLength = theApp.m_lFixedPointLength;

	CString strItem1;
	CWnd* pWnd;

	SetWindowText(m_strFrameName);
	if (m_nLanguage == 0)
	{
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING271);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC2);
		strItem1.LoadString(IDS_STRING273);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING277);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDNO);
		strItem1.LoadString(IDS_STRING275);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182);
		pWnd->SetWindowText(strItem1);
	}
	else
	{
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING271_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC2);
		strItem1.LoadString(IDS_STRING273_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING277_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDNO);
		strItem1.LoadString(IDS_STRING275_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182_EN);
		pWnd->SetWindowText(strItem1);
	}
	
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileFixedPrint::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);

	theApp.m_lFixedPointLength = this->m_lFixedLength;

	CDialog::OnOK();
}

void CFileFixedPrint::OnNo() 
{
	// TODO: Add your control notification handler code here
	theApp.m_lFixedPointLength = 0;

	CDialog::OnOK();
}

