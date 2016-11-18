#if !defined(AFX_FILEFIXEDPRINT_H__14A88095_4ACE_4338_9663_6D04E0A67407__INCLUDED_)
#define AFX_FILEFIXEDPRINT_H__14A88095_4ACE_4338_9663_6D04E0A67407__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileFixedPrint.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileFixedPrint dialog

class CFileFixedPrint : public CDialog
{
// Construction
public:
	CFileFixedPrint(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileFixedPrint)
	enum { IDD = IDD_FILEFIXEDPRINT };
	long	m_lFixedLength;
	//}}AFX_DATA

	int m_nLanguage;
	CString m_strFrameName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileFixedPrint)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileFixedPrint)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEFIXEDPRINT_H__14A88095_4ACE_4338_9663_6D04E0A67407__INCLUDED_)
