#if !defined(AFX_VERIFYCOPIES_H__3F0D89A1_79B6_41F3_9502_71C633DDD660__INCLUDED_)
#define AFX_VERIFYCOPIES_H__3F0D89A1_79B6_41F3_9502_71C633DDD660__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VerifyCopies.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVerifyCopies dialog

class CVerifyCopies : public CDialog
{
// Construction
public:
	CVerifyCopies(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVerifyCopies)
	enum { IDD = IDD_VERIFYCOPIES };
	long	m_lWriteValue;
	//}}AFX_DATA
	CString m_strFileName;
	int m_nDialogFlag;
	long m_lPictureLength;	//Í¼ÐÎ³¤¶È
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVerifyCopies)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVerifyCopies)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VERIFYCOPIES_H__3F0D89A1_79B6_41F3_9502_71C633DDD660__INCLUDED_)
