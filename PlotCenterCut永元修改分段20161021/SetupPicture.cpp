// SetupPicture.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "SetupPicture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern double gl_dPaperLength;

extern int ftoi(double f);
/////////////////////////////////////////////////////////////////////////////
// CSetupPicture dialog

CSetupPicture::CSetupPicture(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupPicture::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupPicture)
	m_dMark = 0.0;
	m_dPaperWidth = 0.0;
	m_dPaperLength = 0.0;
	m_nLineWidth = 0;
	m_bRotate = FALSE;
	m_dGapDown = 0.0;
	m_dGapRight = 0.0;
	m_bFont = FALSE;
	//}}AFX_DATA_INIT
}


void CSetupPicture::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupPicture)
	DDX_Text(pDX, IDC_MARK, m_dMark);
	DDV_MinMaxDouble(pDX, m_dMark, 20., 1000.);
	DDX_Text(pDX, IDC_PAPER_WIDTH, m_dPaperWidth);
	DDV_MinMaxDouble(pDX, m_dPaperWidth, 50., 2600.);
	DDX_Text(pDX, IDC_PAPER_LENGTH, m_dPaperLength);	//20140709纸张长度
	DDV_MinMaxDouble(pDX, m_dPaperLength, 50., 5000.);	//20141211 rwt
	DDX_Text(pDX, IDC_LINEWIDTH, m_nLineWidth);
	DDV_MinMaxInt(pDX, m_nLineWidth, 1, 9);
//	DDX_Check(pDX, IDC_ROTATE, m_bRotate);
	DDX_Text(pDX, IDC_GAP_DOWN, m_dGapDown);
	DDV_MinMaxDouble(pDX, m_dGapDown, 0., 500.);
	DDX_Text(pDX, IDC_GAP_RIGHT, m_dGapRight);	
	DDV_MinMaxDouble(pDX, m_dGapRight, 0., 1300.); 
	DDX_Check(pDX, IDC_FONT, m_bFont);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupPicture, CDialog)
	//{{AFX_MSG_MAP(CSetupPicture)
	ON_BN_CLICKED(IDC_SETFONT, OnSetFont)
	ON_BN_CLICKED(IDC_FONT, OnFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupPicture message handlers

BOOL CSetupPicture::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	this->m_nLanguage=theApp.m_nLanguage;
	this->m_dGapDown=theApp.m_dGapDown;
	this->m_dGapRight=theApp.m_dGapRight;  
	this->m_dMark=theApp.m_dMarkSpace;
	this->m_dPaperWidth=theApp.m_dPaperWidth;
	this->m_dPaperLength=theApp.m_dPaperLength;
	gl_dPaperLength = theApp.m_dPaperLength;	//20140709 纸张长度
	if (theApp.m_nRotate == 0)
		this->m_bRotate=false;
	else
		this->m_bRotate=true;
	if (theApp.m_nFont == 0)
		this->m_bFont=false;
	else
		this->m_bFont=true;
	if (theApp.m_nFont == 0)
		GetDlgItem(IDC_SETFONT)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_SETFONT)->EnableWindow(FALSE);
	
	CString strItem1;
	CWnd* pWnd;
	if (m_nLanguage == 0)
	{
		strItem1.LoadString(IDS_STRING200);
		SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_ROTATE);
//		strItem1.LoadString(IDS_STRING160);
//		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_FONT);
		strItem1.LoadString(IDS_STRING297);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_PAPERWIDTH);
		strItem1.LoadString(IDS_STRING166);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPACE);
		strItem1.LoadString(IDS_STRING168);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SETUPWIDTH);
		strItem1.LoadString(IDS_STRING174);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_XIANKUAN);
		strItem1.LoadString(IDS_STRING176);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_ZHIZHANGKUANDU);
		strItem1.LoadString(IDS_STRING333);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_ZHIZHANGCHANGDU);
		strItem1.LoadString(IDS_STRING334);
		pWnd->SetWindowText(strItem1);
		
		pWnd = GetDlgItem(IDC_XIANGSU);
		strItem1.LoadString(IDS_STRING178);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING114);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC2);
		strItem1.LoadString(IDS_STRING162);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC3);
		strItem1.LoadString(IDS_STRING164);
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
		strItem1.LoadString(IDS_STRING200_EN);
		SetWindowText(strItem1);
//		pWnd = GetDlgItem(IDC_ROTATE);
//		strItem1.LoadString(IDS_STRING160_EN);
//		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_FONT);
		strItem1.LoadString(IDS_STRING297_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_PAPERWIDTH);
		strItem1.LoadString(IDS_STRING166_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SPACE);
		strItem1.LoadString(IDS_STRING168_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_SETUPWIDTH);
		strItem1.LoadString(IDS_STRING174_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_XIANKUAN);
		strItem1.LoadString(IDS_STRING176_EN);
		pWnd->SetWindowText(strItem1);

		pWnd = GetDlgItem(IDC_ZHIZHANGKUANDU);
		strItem1.LoadString(IDS_STRING333_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_ZHIZHANGCHANGDU);
		strItem1.LoadString(IDS_STRING334_EN);
		pWnd->SetWindowText(strItem1);

		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_XIANGSU);
		strItem1.LoadString(IDS_STRING178_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC1);
		strItem1.LoadString(IDS_STRING114_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC2);
		strItem1.LoadString(IDS_STRING162_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_STATIC3);
		strItem1.LoadString(IDS_STRING164_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDOK);
		strItem1.LoadString(IDS_STRING180_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182_EN);
		pWnd->SetWindowText(strItem1);
	}
	if ((!m_bConnected) || m_bNetworkBusy)
	{
		pWnd = GetDlgItem(IDC_LINEWIDTH);
		pWnd->EnableWindow(false);
	}
	
	// 20131206
	pWnd = GetDlgItem(IDC_GAP_RIGHT);	//右边距 
	pWnd->EnableWindow(true);	       //使控件可用HHY

	pWnd = GetDlgItem(IDC_GAP_DOWN);	//马克间距
	pWnd->EnableWindow(true);	        //使控件可用HHY

	pWnd = GetDlgItem(IDC_MARK);	//对接符间距
	pWnd->EnableWindow(true);	//使控件可用HHY


	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupPicture::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	
	CDialog::OnOK();
}

//gzr20160906 设置字体对话框中控件的enable start
UINT CALLBACK CFHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uiMsg)
    {
	case WM_INITDIALOG:
        {
			HWND hCheckBox = ::FindWindowEx(hdlg, NULL, "Button", NULL);
			BOOL isDelete = FALSE;
			BOOL isDownLine = FALSE;
			while(hCheckBox)
			{
				char szText[MAX_PATH] = {0};
				GetWindowText(hCheckBox, szText, MAX_PATH);
				if(!isDelete && strstr(szText, "删除线") != 0)
				{
					EnableWindow( hCheckBox , FALSE );
					//ShowWindow(hCheckBox, FALSE);
					isDelete = TRUE;
				}
				else if(!isDownLine && strstr(szText, "下划线") != 0)
				{
					EnableWindow( hCheckBox , FALSE );
					//ShowWindow(hCheckBox, FALSE);
					isDownLine = TRUE;
				}
				
				if (isDelete && isDownLine)
				{
					break;
				}
				hCheckBox = ::FindWindowEx(hdlg, hCheckBox, "Button", NULL);
			}
			HWND hColorCombo = ::FindWindowEx(hdlg, NULL, "ComboBox", NULL);
			while(hColorCombo)
			{
				char szText[MAX_PATH] = {0};
				GetWindowText(hColorCombo, szText, MAX_PATH);
				if(strstr(szText, "黑色") != 0)
				{
					EnableWindow( hColorCombo , FALSE );
					//ShowWindow(hColorCombo, FALSE);
					break;
				}
				
				hColorCombo = ::FindWindowEx(hdlg, hColorCombo, "ComboBox", NULL);
			}
        }
        break;
    }
    return 0;
}
//gzr20160906 设置字体对话框中控件的enable end

void CSetupPicture::OnSetFont() 
{
	// TODO: Add your control notification handler code here
	//Roman
	CFontDialog dlg(&(theApp.m_config.m_lfDrawText));
	dlg.m_cf.lpfnHook = CFHookProc;
	if(dlg.DoModal()== IDOK)
	{
		dlg.GetCurrentFont(&(theApp.m_config.m_lfDrawText));
		Invalidate(true);
	}
}

void CSetupPicture::OnFont() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton;
	pButton = (CButton*)GetDlgItem(IDC_FONT);
	int i = pButton->GetCheck();
	if (i == 0)
		GetDlgItem(IDC_SETFONT)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_SETFONT)->EnableWindow(FALSE);
}
