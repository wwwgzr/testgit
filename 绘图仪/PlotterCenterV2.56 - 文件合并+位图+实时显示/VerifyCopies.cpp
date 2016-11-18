// VerifyCopies.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "VerifyCopies.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVerifyCopies dialog


CVerifyCopies::CVerifyCopies(CWnd* pParent /*=NULL*/)
	: CDialog(CVerifyCopies::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVerifyCopies)
	m_lWriteValue = 0;
	//}}AFX_DATA_INIT
}


void CVerifyCopies::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVerifyCopies)
	DDX_Text(pDX, IDC_FILECOPIES, m_lWriteValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVerifyCopies, CDialog)
	//{{AFX_MSG_MAP(CVerifyCopies)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVerifyCopies message handlers

void CVerifyCopies::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	if (m_nDialogFlag == 1)
	{
		if ((m_lWriteValue < 1) || (m_lWriteValue > 255))
		{
			CString s1;
			if (theApp.m_nLanguage == 0)
				s1.LoadString(IDS_STRING311);
			else
				s1.LoadString(IDS_STRING311_EN);
			AfxMessageBox(s1);
			return;
		}
	}
	else if (m_nDialogFlag == 2)
	{
		if ((m_lWriteValue < 0) || (m_lWriteValue > m_lPictureLength))
		{
			CString s1;
			if (theApp.m_nLanguage == 0)
				s1.LoadString(IDS_STRING299);
			else
				s1.LoadString(IDS_STRING299_EN);
			AfxMessageBox(s1);
			return;
		}
	}
	CDialog::OnOK();
}

BOOL CVerifyCopies::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strItem1;
	CWnd* pWnd;
	TCHAR szFName[_MAX_FNAME];

	_splitpath(m_strFileName,NULL,NULL,szFName,NULL);
	m_strFileName.Format(_T("%s"),szFName);
	m_strFileName=m_strFileName + _T(".plt");
	SetWindowText(m_strFileName);
	if (m_nDialogFlag == 1)
	{
		if (theApp.m_nLanguage == 0)
		{
			pWnd = GetDlgItem(IDC_FENSHU);
			strItem1.LoadString(IDS_STRING156);
			pWnd->SetWindowText(strItem1);
			pWnd = GetDlgItem(IDOK);
			strItem1.LoadString(IDS_STRING180);
			pWnd->SetWindowText(strItem1);
			pWnd = GetDlgItem(IDCANCEL);
			strItem1.LoadString(IDS_STRING182);
			pWnd->SetWindowText(strItem1);
		}
		else
		{
			pWnd = GetDlgItem(IDC_FENSHU);
			strItem1.LoadString(IDS_STRING156_EN);
			pWnd->SetWindowText(strItem1);
			pWnd = GetDlgItem(IDOK);
			strItem1.LoadString(IDS_STRING180_EN);
			pWnd->SetWindowText(strItem1);
			pWnd = GetDlgItem(IDCANCEL);
			strItem1.LoadString(IDS_STRING182_EN);
			pWnd->SetWindowText(strItem1);
		}
		pWnd = GetDlgItem(IDC_RANGE);
		strItem1.Format(_T("(0-255)"));
		pWnd->SetWindowText(strItem1);
	}
	else if (m_nDialogFlag == 2)
	{
		if (theApp.m_nLanguage == 0)
		{
			pWnd = GetDlgItem(IDC_FENSHU);
			strItem1.LoadString(IDS_STRING154);
			pWnd->SetWindowText(strItem1);
			pWnd = GetDlgItem(IDOK);
			strItem1.LoadString(IDS_STRING180);
			pWnd->SetWindowText(strItem1);
			pWnd = GetDlgItem(IDCANCEL);
			strItem1.LoadString(IDS_STRING182);
			pWnd->SetWindowText(strItem1);
		}
		else
		{
			pWnd = GetDlgItem(IDC_FENSHU);
			strItem1.LoadString(IDS_STRING154_EN);
			pWnd->SetWindowText(strItem1);
			pWnd = GetDlgItem(IDOK);
			strItem1.LoadString(IDS_STRING180_EN);
			pWnd->SetWindowText(strItem1);
			pWnd = GetDlgItem(IDCANCEL);
			strItem1.LoadString(IDS_STRING182_EN);
			pWnd->SetWindowText(strItem1);
		}
		pWnd = GetDlgItem(IDC_RANGE);
		strItem1.Format(_T("(0-%ld)"),m_lPictureLength);
		pWnd->SetWindowText(strItem1);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
