// ViewHistory.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "ViewHistory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 表示可改变大小的图标ID
#ifndef OBM_SIZE
#define OBM_SIZE 32766
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewHistory dialog


CViewHistory::CViewHistory(CWnd* pParent /*=NULL*/)
	: CDialog(CViewHistory::IDD, pParent)
, m_iClientWidth(0)
, m_iClientHeight(0)
, m_iMinWidth(0)
, m_iMinHeight(0)
, m_pControlArray(NULL)
, m_iControlNumber(0)
, m_bShowSizeIcon(TRUE)
{
	//{{AFX_DATA_INIT(CViewHistory)
	//}}AFX_DATA_INIT
}


void CViewHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewHistory)
	DDX_Control(pDX, IDC_HISTORYRECORD, m_wndHistory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewHistory, CDialog)
	//{{AFX_MSG_MAP(CViewHistory)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_DRAW, OnDraw)
	ON_WM_SIZE()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewHistory message handlers

BOOL CViewHistory::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// 设置对话框为可变大小的
	ModifyStyle(0,WS_SIZEBOX);

	// 以对话框的初始大小作为对话框的宽度和高度的最小值
	CRect rectDlg;
	GetWindowRect(rectDlg);
	m_iMinWidth = rectDlg.Width();
	m_iMinHeight = rectDlg.Height();

	// 得到对话框client区域的大小 
	CRect rectClient;
	GetClientRect(&rectClient);
	m_iClientWidth = rectClient.Width();
	m_iClientHeight = rectClient.Height();

	// Load图标
	m_bmpSizeIcon.LoadOEMBitmap(OBM_SIZE);
	m_bmpSizeIcon.GetBitmap(&m_bitmap);

	// 创建显示图标的静态控件并放在对话框右下角
	m_wndSizeIcon.Create(NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, CRect(0, 0, m_bitmap.bmWidth, m_bitmap.bmHeight), this, 0);
	m_wndSizeIcon.SetBitmap(m_bmpSizeIcon);
	m_wndSizeIcon.MoveWindow(m_iClientWidth - m_bitmap.bmWidth, m_iClientHeight - m_bitmap.bmHeight, m_bitmap.bmWidth, m_bitmap.bmHeight);

	// 显示图标
	m_wndSizeIcon.ShowWindow(m_bShowSizeIcon);

	// 控件信息数组
	static DLGCTLINFO dcMenuGroup[] = 
	{
		{IDC_DRAW, MOVEY, 100},
		{IDC_CLEAR, MOVEY, 100},
		{IDCANCEL, MOVEY, 100},
		{IDC_DRAW, MOVEX, 50},
		{IDC_CLEAR, MOVEX, 50},
		{IDCANCEL, MOVEX, 50},
		{IDC_HISTORYRECORD, ELASTICXY, 100},
	};

	// 设置控件信息
	SetControlProperty(dcMenuGroup, sizeof(dcMenuGroup)/sizeof(DLGCTLINFO));

	this->m_nLanguage = theApp.m_nLanguage;
	CWnd* pWnd;
	CString strItem1;
	CRect rect1;
	int i;
	
	m_wndHistory.GetWindowRect(&rect1);
	for (i=0;i<7;i++)
		m_wndHistory.DeleteColumn(0);
	if (m_nLanguage == 0)
	{
		strItem1.LoadString(IDS_STRING283);
		SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_DRAW);
		strItem1.LoadString(IDS_STRING158);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_CLEAR);
		strItem1.LoadString(IDS_STRING285);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182);
		pWnd->SetWindowText(strItem1);
		strItem1.LoadString(IDS_COLUMN0);
		m_wndHistory.InsertColumn(0,strItem1,LVCFMT_LEFT,rect1.Width()*39/100,0);
		strItem1.LoadString(IDS_HCOLUMN1);
		m_wndHistory.InsertColumn(1,strItem1,LVCFMT_LEFT,rect1.Width()*34/100,0);
		strItem1.LoadString(IDS_HCOLUMN2);
		m_wndHistory.InsertColumn(2,strItem1,LVCFMT_LEFT,rect1.Width()*14/100,0);
		strItem1.LoadString(IDS_HCOLUMN3);
		m_wndHistory.InsertColumn(3,strItem1,LVCFMT_LEFT,rect1.Width()*12/100,0);
	}
	else
	{
		strItem1.LoadString(IDS_STRING283_EN);
		SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_DRAW);
		strItem1.LoadString(IDS_STRING158_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_CLEAR);
		strItem1.LoadString(IDS_STRING285_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDCANCEL);
		strItem1.LoadString(IDS_STRING182_EN);
		pWnd->SetWindowText(strItem1);
		strItem1.LoadString(IDS_COLUMN0_EN);
		m_wndHistory.InsertColumn(0,strItem1,LVCFMT_LEFT,rect1.Width()*39/100,0);
		strItem1.LoadString(IDS_HCOLUMN1_EN);
		m_wndHistory.InsertColumn(1,strItem1,LVCFMT_LEFT,rect1.Width()*34/100,0);
		strItem1.LoadString(IDS_HCOLUMN2_EN);
		m_wndHistory.InsertColumn(2,strItem1,LVCFMT_LEFT,rect1.Width()*14/100,0);
		strItem1.LoadString(IDS_HCOLUMN3_EN);
		m_wndHistory.InsertColumn(3,strItem1,LVCFMT_LEFT,rect1.Width()*12/100,0);
	}
	m_wndHistory.SetExtendedStyle(m_wndHistory.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT); 
	DisplayRecord();
	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CViewHistory::OnClear() 
{
	// TODO: Add your control notification handler code here
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString strFileName;

	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strFileName.Format(_T("%s%shistory.ini"),szDrive,szDir);
	DeleteFile(strFileName);
	DisplayRecord();
	UpdateData(false);
}

void CViewHistory::OnDraw() 
{
	// TODO: Add your control notification handler code here
	int nSelCount,nSelItem1;
  CString strCaption,strCaption1,strFileName;
  POSITION pos1;
  TCHAR szDir[_MAX_DIR];
  TCHAR szDrive[_MAX_DRIVE];
  TCHAR szFullPath[_MAX_PATH];

  nSelCount = m_wndHistory.GetSelectedCount();
  if (nSelCount > 0)
  {
		pos1 = m_wndHistory.GetFirstSelectedItemPosition();
		::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
		_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
		strCaption.Format(_T("%s%sPlot\\"),szDrive,szDir);
		while (pos1 != NULL)
		{
			nSelItem1 = m_wndHistory.GetNextSelectedItem(pos1);
			strFileName = m_wndHistory.GetItemText(nSelItem1,0);
			_splitpath(strFileName,szDrive,szDir,NULL,NULL);
			strCaption1.Format(_T("%s%s"),szDrive,szDir);
			if ((strCaption1.CompareNoCase(strCaption) != 0) &&
				(strCaption1.CompareNoCase(theApp.m_strDataDir) != 0))
				m_pParent->OpenHistoryFile(strFileName);
			else
			{
				CString s1;
				if (m_nLanguage == 0)
					s1.LoadString(IDS_STRING295);
				else
					s1.LoadString(IDS_STRING295_EN);
				AfxMessageBox(s1);
			}
		}
	}
}

void CViewHistory::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// 对话框宽度和高度的增量 
	int iIncrementX = cx - m_iClientWidth;
	int iIncrementY = cy - m_iClientHeight;

	// 最小化时增量为0
	if (nType == SIZE_MINIMIZED)
	{
		iIncrementX = iIncrementY = 0;
	}

	for (int i = 0; i < m_iControlNumber; i++)
	{
		CWnd *pWndCtrl = NULL;

		int iId = m_pControlArray[i].iId;
		int iFlag = m_pControlArray[i].iFlag;
		int iPercent = m_pControlArray[i].iPercent;
		
		// 无效值
		if ((iPercent < 0) || (iPercent > 100))
			continue;
		
		// 得到控件指针
		pWndCtrl = GetDlgItem(iId);
		if ((NULL != pWndCtrl) && IsWindow(pWndCtrl->GetSafeHwnd()))
		{
			CRect rectCtrl;
			pWndCtrl->GetWindowRect(rectCtrl);

			ScreenToClient(rectCtrl);

			int iLeft = rectCtrl.left;
			int iTop = rectCtrl.top;
			int iWidth = rectCtrl.Width();
			int iHeight = rectCtrl.Height();
			
			switch (iFlag)
			{
				case MOVEX: // X方向移动
					iLeft += (iIncrementX * iPercent / 100);
					break;
				case MOVEY: // Y方向移动
					iTop += (iIncrementY * iPercent / 100);
					break;
				case MOVEXY: // X方向和Y方向同时移动
					iLeft += (iIncrementX * iPercent / 100);
					iTop += (iIncrementY * iPercent / 100);
					break;
				case ELASTICX: // X方向改变大小
					iWidth += (iIncrementX * iPercent / 100);
					break;
				case ELASTICY: // Y方向改变大小
					iHeight += (iIncrementY * iPercent / 100);
					break;
				case ELASTICXY: // X方向和Y方向同时改变大小
					iWidth += (iIncrementX * iPercent / 100);
					iHeight += (iIncrementY * iPercent / 100);
					break;
				default:
					break;
			}

			// 把控件移动到新位置
			pWndCtrl->MoveWindow(iLeft, iTop, iWidth, iHeight);
		}
	}

	// 把图标移动到对话框右下角
	if (IsWindow(m_wndSizeIcon.GetSafeHwnd()))
		m_wndSizeIcon.MoveWindow(cx - m_bitmap.bmWidth, cy - m_bitmap.bmHeight, m_bitmap.bmWidth, m_bitmap.bmHeight);

	// 记录对话框client区域的大小
	if (nType != SIZE_MINIMIZED)
	{
		m_iClientWidth = cx;
		m_iClientHeight = cy;
	}
}

void CViewHistory::OnSizing(UINT nSide, LPRECT lpRect)
{
	CDialog::OnSizing(nSide, lpRect);

	// 对话框不能小于初始大小

	int iWidth = lpRect->right - lpRect->left;
	int iHeight = lpRect->bottom - lpRect->top;

	if (iWidth <= m_iMinWidth)
		lpRect->right = lpRect->left + m_iMinWidth;

	if(iHeight <= m_iMinHeight)
		lpRect->bottom = lpRect->top + m_iMinHeight;
}

BOOL CViewHistory::SetControlProperty(PDLGCTLINFO lp, int nElements)
{
	// 设置控件数组信息

	if (NULL == lp)
		return FALSE;

	if (nElements <= 0)
		return FALSE;

	m_pControlArray = lp;
	m_iControlNumber = nElements;
	
	return TRUE;
}

void CViewHistory::ShowSizeIcon(BOOL bShow /*=NULL*/)
{
	m_bShowSizeIcon = bShow;
}

void CViewHistory::DisplayRecord()
{
	CString strCaption,strFileName;
  TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	LV_ITEM lvitem;
  int nItem;
  CString strItem1,strItem2;
	CStdioFile file;
	int n1,n2,n3;
		
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strFileName.Format(_T("%s%shistory.ini"),szDrive,szDir);
	m_wndHistory.DeleteAllItems();
	if (!file.Open(strFileName,CFile::modeRead | CFile::typeText))
		return;
	nItem = 0;
	while (file.ReadString(strItem1))
	{
		while (1)
		{
			n1 = strItem1.Find('|');
			if (n1 == -1)
				break;
			strItem2 = strItem1.Left(n1);
			strItem2.TrimLeft();
			strItem1.Delete(0,n1+1);
			n2 = strItem2.Find(":\\");
			if (n2 == -1)
				continue;
			lvitem.mask = LVIF_TEXT | (nItem == 0? LVIF_IMAGE : 0);
			lvitem.iItem = nItem;
			lvitem.iSubItem = 0;
			lvitem.pszText=strItem2.GetBuffer(0);
			lvitem.iImage=0;
			m_wndHistory.InsertItem(&lvitem);
			n1 = strItem1.Find('|');
			if (n1 == -1)
			{
				nItem++;
				break;
			}
			strItem2 = strItem1.Left(n1);
			strItem2.TrimLeft();
			n3 = 0;
			n2 = strItem2.Find('-',0);
			while (n2 != -1)
			{
				n3++;
				n2 = strItem2.Find('-',n2+1);
			}
			if (n3 != 2)
			{
				nItem++;
				continue;
			}
			n3 = 0;
			n2 = strItem2.Find(':',0);
			while (n2 != -1)
			{
				n3++;
				n2 = strItem2.Find(':',n2+1);
			}
			if (n3 != 2)
			{
				nItem++;
				continue;
			}
			strItem1.Delete(0,n1+1);
			lvitem.iSubItem=1;
			lvitem.pszText=strItem2.GetBuffer(0);
      lvitem.iImage=0;
			m_wndHistory.SetItem(&lvitem);
			n1 = strItem1.Find('|');
			if (n1 == -1)
			{
				nItem++;
				break;
			}
			strItem2 = strItem1.Left(n1);
			strItem2.TrimLeft();
			n3 = 0;
			n2 = strItem2.Find(':',0);
			while (n2 != -1)
			{
				n3++;
				n2 = strItem2.Find(':',n2+1);
			}
			if (n3 != 2)
			{
				nItem++;
				continue;
			}
			strItem1.Delete(0,n1+1);
			lvitem.iSubItem=2;
			lvitem.pszText=strItem2.GetBuffer(0);
      lvitem.iImage=0;
			m_wndHistory.SetItem(&lvitem);
			n1 = strItem1.Find('|');
			if (n1 == -1)
			{
				nItem++;
				break;
			}
			strItem2 = strItem1.Left(n1);
			strItem2.TrimLeft();
			n2 = strItem2.Find('%');
			if (n2 == -1)
			{
				nItem++;
				continue;
			}
			strItem1.Delete(0,n1+1);
			lvitem.iSubItem=3;
			lvitem.pszText=strItem2.GetBuffer(0);
      lvitem.iImage=0;
			m_wndHistory.SetItem(&lvitem);
			nItem++;
		}
	}
	file.Close();
}
