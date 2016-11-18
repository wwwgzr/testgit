// FileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "plotcenter.h"
#include "FileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDlg

// Dialog Data
   //{{AFX_DATA(CFileDlg)
    enum { IDD = IDD_DIALOG1 };
    //}}AFX_DATA



IMPLEMENT_DYNAMIC(CFileDlg, CFileDialog)

CFileDlg::CFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
    m_ofn.Flags |= dwFlags;
    m_ofn.Flags |= OFN_EXPLORER;
    m_ofn.Flags |= OFN_ENABLETEMPLATE;
    SetTemplate(0, IDD_DIALOG1);

}


BEGIN_MESSAGE_MAP(CFileDlg, CFileDialog)
	//{{AFX_MSG_MAP(CFileDlg)
	ON_BN_CLICKED(IDC_HBWJ, OnHbwj)
	ON_BN_CLICKED(IDC_HBWJ2, OnHbwj2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (theApp.m_nLanguage == 1)
	{
		GetDlgItem(IDC_HBWJ)->SetWindowText("X Merge file");
	}
	else
		GetDlgItem(IDC_HBWJ)->SetWindowText("X合并文件");
	if (theApp.m_nLanguage == 1)
	{
		GetDlgItem(IDC_HBWJ2)->SetWindowText("Y Merge file");
	}
	else
		GetDlgItem(IDC_HBWJ2)->SetWindowText("Y合并文件");
	return true;
}
//打开文件对话框下的合并文件复选框事件
void CFileDlg::OnHbwj() 
{
	// TODO: Add your control notification handler code here
  if ( BST_CHECKED == IsDlgButtonChecked( IDC_HBWJ ) )
  {
       hbwj=2;
	   CButton* pBtn = (CButton*)GetDlgItem(IDC_HBWJ2);
	   pBtn->SetCheck(0);
  }
  else if ( BST_CHECKED == IsDlgButtonChecked( IDC_HBWJ2 ) )
  {
	  hbwj=3;
  }
  else
  {
	  hbwj=1;
	}

}
//打开文件对话框下的Y合并文件复选框事件
void CFileDlg::OnHbwj2() 
{
	// TODO: Add your control notification handler code here
	if ( BST_CHECKED == IsDlgButtonChecked( IDC_HBWJ2 ) )
	{
		hbwj=3;
		CButton* pBtn = (CButton*)GetDlgItem(IDC_HBWJ);
		pBtn->SetCheck(0);
	}
	else if ( BST_CHECKED == IsDlgButtonChecked( IDC_HBWJ ) )
	{
		hbwj=2;
	}
	else
	{
		hbwj=1;
	}
}
