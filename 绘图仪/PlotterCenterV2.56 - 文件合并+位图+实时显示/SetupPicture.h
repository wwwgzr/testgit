#if !defined(AFX_SETUPPICTURE_H__FDC0FD32_D89A_4D4B_99EB_9D6E27496CBF__INCLUDED_)
#define AFX_SETUPPICTURE_H__FDC0FD32_D89A_4D4B_99EB_9D6E27496CBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupPicture.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CSetupPicture dialog

class CSetupPicture : public CDialog
{
// Construction
public:
	CSetupPicture(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupPicture)
	enum { IDD = IDD_SETUPPICTURE };
	double	m_dMark;
	double	m_dPaperWidth;
	int		m_nLineWidth;
	BOOL	m_bRotate;
	double	m_dGapDown;
	double	m_dGapRight;
	BOOL	m_bFont;
	//}}AFX_DATA
	int m_nUnitOld;
	int m_nLanguage;
	BOOL m_bConnected;
	BOOL m_bNetworkBusy;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupPicture)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupPicture)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPPICTURE_H__FDC0FD32_D89A_4D4B_99EB_9D6E27496CBF__INCLUDED_)
