#if !defined(AFX_SELECTDIALOG_H__2ED6A0F2_5B7F_4E60_930C_5934F685C92F__INCLUDED_)
#define AFX_SELECTDIALOG_H__2ED6A0F2_5B7F_4E60_930C_5934F685C92F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectDialog dialog

class CSelectDialog : public CDialog
{
// Construction
public:
	CSelectDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectDialog)
	enum { IDD = IDD_SELECTDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	int m_nLanguage;
	CString m_strPrompting;
	CString m_strOK;
	CString m_strNO;
	CString m_strCancel;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnNo();
	afx_msg void OnYes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTDIALOG_H__2ED6A0F2_5B7F_4E60_930C_5934F685C92F__INCLUDED_)
