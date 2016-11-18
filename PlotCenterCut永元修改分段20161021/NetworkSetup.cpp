// NetworkSetup.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "NetworkSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetworkSetup dialog


CNetworkSetup::CNetworkSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CNetworkSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetworkSetup)
	//}}AFX_DATA_INIT
}


void CNetworkSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkSetup)
	DDX_Control(pDX, IDC_SUBNETMASK, m_sSubnetMask);
	DDX_Control(pDX, IDC_REMOTEIP, m_sRemoteIP);
	DDX_Control(pDX, IDC_GATEWAYIP, m_sGatewayIP);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkSetup, CDialog)
	//{{AFX_MSG_MAP(CNetworkSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkSetup message handlers

BOOL CNetworkSetup::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_sRemoteIP.SetAddress(theApp.m_dwRemoteIP);
	m_sSubnetMask.SetAddress(theApp.m_dwSubnetMask);
	m_sGatewayIP.SetAddress(theApp.m_dwGatewayIP);

	CWnd* pWnd;
	CString strItem1;
	
	this->m_nLanguage = theApp.m_nLanguage;
	if (m_nLanguage == 0)
	{
		strItem1.LoadString(IDS_STRING190);
		SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_PlotterIP);
		strItem1.LoadString(IDS_STRING192);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_IPDIZHI);
		strItem1.LoadString(IDS_STRING194);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MASKDIZHI);
		strItem1.LoadString(IDS_STRING196);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_GATEWAYDIZHI);
		strItem1.LoadString(IDS_STRING198);
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
		strItem1.LoadString(IDS_STRING190_EN);
		SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_PlotterIP);
		strItem1.LoadString(IDS_STRING192_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_IPDIZHI);
		strItem1.LoadString(IDS_STRING194_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MASKDIZHI);
		strItem1.LoadString(IDS_STRING196_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_GATEWAYDIZHI);
		strItem1.LoadString(IDS_STRING198_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING180_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182_EN);
		pWnd->SetWindowText(strItem1);
	}

	if (m_bConnected)
	{
		pWnd = GetDlgItem(IDC_REMOTEIP);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_SUBNETMASK);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_GATEWAYIP);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDOK);
		pWnd->EnableWindow(false);
	}

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNetworkSetup::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	m_sRemoteIP.GetAddress(theApp.m_dwRemoteIP);
	m_sSubnetMask.GetAddress(theApp.m_dwSubnetMask);
	m_sGatewayIP.GetAddress(theApp.m_dwGatewayIP);

	CDialog::OnOK();
}
