#if !defined(AFX_NETWORKSETUP_H__12ADAB1C_D829_4AE5_A2A4_8FDE78FCE7C8__INCLUDED_)
#define AFX_NETWORKSETUP_H__12ADAB1C_D829_4AE5_A2A4_8FDE78FCE7C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetworkSetup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetworkSetup dialog

class CNetworkSetup : public CDialog
{
// Construction
public:
	CNetworkSetup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNetworkSetup)
	enum { IDD = IDD_NETWORKSETUP };
	CIPAddressCtrl	m_sSubnetMask;
	CIPAddressCtrl	m_sRemoteIP;
	CIPAddressCtrl	m_sGatewayIP;
	//}}AFX_DATA
	int m_nLanguage;
	BOOL m_bConnected;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetworkSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNetworkSetup)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETWORKSETUP_H__12ADAB1C_D829_4AE5_A2A4_8FDE78FCE7C8__INCLUDED_)
