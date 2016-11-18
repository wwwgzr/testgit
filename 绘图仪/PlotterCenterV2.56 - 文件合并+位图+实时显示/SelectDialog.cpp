// SelectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "SelectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectDialog dialog


CSelectDialog::CSelectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectDialog, CDialog)
	//{{AFX_MSG_MAP(CSelectDialog)
	ON_BN_CLICKED(IDNO, OnNo)
	ON_BN_CLICKED(IDYES, OnYes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectDialog message handlers

BOOL CSelectDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	this->m_nLanguage=theApp.m_nLanguage;
		
	CString strItem1;
	CWnd* pWnd;
	if (m_nLanguage == 0)
		strItem1.LoadString(IDS_STRING281);
	else
		strItem1.LoadString(IDS_STRING281_EN);
	SetWindowText(strItem1);
	pWnd = GetDlgItem(IDC_PROMPTING);
	pWnd->SetWindowText(m_strPrompting);
	pWnd = GetDlgItem(IDYES);
	pWnd->SetWindowText(m_strOK);
	pWnd = GetDlgItem(IDNO);
	pWnd->SetWindowText(m_strNO);
	pWnd = GetDlgItem(IDCANCEL);
	pWnd->SetWindowText(m_strCancel);
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectDialog::OnYes() 
{
	// TODO: Add your control notification handler code here
	CDialog::EndDialog(IDYES);

	/*退出后保存文件
		CString strCaption,strCaption1;
	    TCHAR szDir[_MAX_DIR];
		TCHAR szDrive[_MAX_DRIVE];
		TCHAR szFullPath[_MAX_PATH];
		::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
		_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
		strCaption1.Format(_T("%s%sPlot\\1.plt"),szDrive,szDir);
		_splitpath(savefilename,szDrive,szDir,NULL,NULL);
		strCaption.Format(_T("%s%s"),szDrive,szDir);
		int i=CopyFile(savefilename,strCaption1,TRUE);
	退出后保存文件*/

}

void CSelectDialog::OnNo() 
{
	// TODO: Add your control notification handler code here
	CDialog::EndDialog(IDNO);
}
