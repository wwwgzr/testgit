#if !defined(AFX_FILEDLG_H__A6C207C1_8B95_4096_B602_6678C523E373__INCLUDED_)
#define AFX_FILEDLG_H__A6C207C1_8B95_4096_B602_6678C523E373__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileDlg dialog

class CFileDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CFileDlg)

public:
	CFileDlg(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(CFileDlg)
	afx_msg void OnHbwj();
	virtual BOOL OnInitDialog();
	afx_msg void OnHbwj2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEDLG_H__A6C207C1_8B95_4096_B602_6678C523E373__INCLUDED_)
