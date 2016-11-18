// PictureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "PlotCenterDlg.h"
#include "PictureDlg.h"
#include "math.h"
#include "PlotBitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 表示可改变大小的图标ID
#ifndef OBM_SIZE
#define OBM_SIZE 32766
#endif

#define ID_TIMER_3  3  //定时器  ID_TIMER  20150714
#define T_TIMES_3  500  //定时,unit:ms   T_TIMES  20150714
extern BOOL bIsEncrypt;

/////////////////////////////////////////////////////////////////////////////
// CPictureDlg dialog


CPictureDlg::CPictureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPictureDlg::IDD, pParent)
, m_iClientWidth(0)
, m_iClientHeight(0)
, m_iMinWidth(0)
, m_iMinHeight(0)
, m_pControlArray(NULL)
, m_iControlNumber(0)
, m_bShowSizeIcon(TRUE)
{
	//{{AFX_DATA_INIT(CPictureDlg)
	m_lFixedPointLength = 0;
	m_nCopyNum = 0;
	m_busyPrint = FALSE;
	//}}AFX_DATA_INIT
}


void CPictureDlg::DoDataExchange(CDataExchange* pDX)
{
	//if (m_nLanguage == 0)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPictureDlg)
	DDX_Text(pDX, IDC_TOTALX, m_strTotalX);
	DDX_Text(pDX, IDC_TOTALY, m_strTotalY);
	DDX_Text(pDX, IDC_FIXEDPOINT, m_lFixedPointLength);
	DDX_Text(pDX, IDC_COPYNUM, m_nCopyNum);
	//}}AFX_DATA_MAP
	}
	/*
	else
	{
		//m_strTotalX = int(ftoi(m_strTotalX)/2.54);
		//m_strTotalY = int(m_strTotalY/2.54)
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CPictureDlg)
		DDX_Text(pDX, IDC_TOTALX, ((_wtoi(m_strTotalX))/2.54));
		DDX_Text(pDX, IDC_TOTALY, ((_wtoi(m_strTotalY))/2.54));
		DDX_Text(pDX, IDC_FIXEDPOINT, m_lFixedPointLength);
		DDX_Text(pDX, IDC_COPYNUM, m_nCopyNum);
	}
	*/
}


BEGIN_MESSAGE_MAP(CPictureDlg, CDialog)
	//{{AFX_MSG_MAP(CPictureDlg)
	ON_WM_TIMER()//实时显示
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_DRAW, OnDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_EN_CHANGE(IDC_FIXEDPOINT, OnChangeFixedPoint)
	ON_EN_CHANGE(IDC_COPYNUM, OnChangeCopyNum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPictureDlg message handlers

void CPictureDlg::OnTimer() //实时显示
{
	if(displaybusy==1 && m_busyPrint)
	{
      DrawPicture(0);
	}
}

void CPictureDlg::OnClose() 
{
	CDialog::OnCancel();
}
void CPictureDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
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

void CPictureDlg::OnSizing(UINT nSide, LPRECT lpRect)
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

BOOL CPictureDlg::SetControlProperty(PDLGCTLINFO lp, int nElements)
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

void CPictureDlg::ShowSizeIcon(BOOL bShow /*=NULL*/)
{
	m_bShowSizeIcon = bShow;
}

BOOL CPictureDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	displaybusy=1;
	SetTimer(1,500,NULL);//实时显示
	
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
		{IDC_STATICX, MOVEXY, 100},
		{IDC_TOTALX, MOVEXY, 100},
		{IDC_STATICY, MOVEXY, 100},
		{IDC_TOTALY, MOVEXY, 100},
		{IDC_STARTPOINT, MOVEXY, 100},
		{IDC_FIXEDPOINT, MOVEXY, 100},
		{IDC_MM, MOVEXY, 100},
		{IDC_NUM, MOVEXY, 100},
		{IDC_AGAINNUM, MOVEXY, 100},
		{ID_DRAW, MOVEXY, 100},
		{IDC_DRAWPICTURE, ELASTICXY, 100},
	};

	// 设置控件信息
	SetControlProperty(dcMenuGroup, sizeof(dcMenuGroup)/sizeof(DLGCTLINFO));

	this->m_nLanguage = theApp.m_nLanguage;
	m_bLButtonDown = false;
	// 20110402 打印预览
	if(m_nCopyNum == 0)
	{
		m_nCopyNum = 1;
		m_lFixedPointLength = 0;
	}
	if (theApp.m_nMeasureUnit== 0)
	{
		m_strTotalX.Format("%ldmm",ftoi(m_lTotalX*0.025));
		m_strTotalY.Format("%ldmm",ftoi(m_lTotalY*0.025));
	}
	else if(theApp.m_nMeasureUnit == 0)
	{
		m_strTotalX.Format("%ld inch",ftoi(m_lTotalX*0.025/25.4));
		m_strTotalY.Format("%ld inch",ftoi(m_lTotalY*0.025/25.4));
	}
	m_ptMouseDownPos.x=0;
	m_ptMouseDownPos.y=0;
	m_ptMouseUpPos=m_ptMouseDownPos;
	CWnd *pWnd = GetDlgItem(IDC_DRAWPICTURE);   
    CRect rtDrawPicture;
	pWnd->GetClientRect(&rtDrawPicture);
	m_rtDrawPicture=rtDrawPicture;
	m_dProportion=(double)(rtDrawPicture.Height()-10)/(double)m_lTotalY;
	double d=(double)(rtDrawPicture.Width()-10)/(double)m_lTotalX;
	if(m_dProportion>d)
	{
		m_dProportion=d;
		m_ptStartPos.x = 5;
		m_ptStartPos.y = (long)((rtDrawPicture.Height() - m_lTotalY * m_dProportion) / 2);
	}
	else
	{
		m_ptStartPos.y = 5;
		m_ptStartPos.x = (long)((rtDrawPicture.Width() - m_lTotalX *m_dProportion) / 2);
	}
	CString strItem1;
	if (theApp.m_nMeasureUnit == 0)
	{
		strItem1.Format("%s(%ldmm * %ldmm)",m_strPictureName,ftoi(m_lTotalX*0.025),ftoi(m_lTotalY*0.025));
	}
	else if(theApp.m_nMeasureUnit == 1)
	{
		strItem1.Format("%s(%ld inch * %ld inch)",m_strPictureName,ftoi(m_lTotalX*0.025/25.4),ftoi(m_lTotalY*0.025/25.4));
	}
	SetWindowText(strItem1);
	if (m_nLanguage == 0)
	{
		pWnd = GetDlgItem(IDC_STARTPOINT);
		strItem1.LoadString(IDS_STRING154);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_NUM);
		strItem1.LoadString(IDS_STRING156);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(ID_DRAW);
		strItem1.LoadString(IDS_STRING158);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM);
		strItem1.LoadString(IDS_STRING159);
		pWnd->SetWindowText(strItem1);
	}
	else
	{
		pWnd = GetDlgItem(IDC_STARTPOINT);
		strItem1.LoadString(IDS_STRING154_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_NUM);
		strItem1.LoadString(IDS_STRING156_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(ID_DRAW);
		strItem1.LoadString(IDS_STRING158_EN);
		pWnd->SetWindowText(strItem1);
		pWnd = GetDlgItem(IDC_MM);
		strItem1.LoadString(IDS_STRING159_EN);
		pWnd->SetWindowText(strItem1);
	}

	if (theApp.m_nMeasureUnit == 0)
	{
		pWnd = GetDlgItem(IDC_MM);
		strItem1.LoadString(IDS_STRING159);
		pWnd->SetWindowText(strItem1);
	}
	else if(theApp.m_nMeasureUnit == 1)
	{
		pWnd = GetDlgItem(IDC_MM);
		strItem1.LoadString(IDS_STRING159_EN);
		pWnd->SetWindowText(strItem1);
	}
	
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPictureDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CWnd *pWnd = GetDlgItem(IDC_DRAWPICTURE);   
  CRect rtDrawPicture;
	pWnd->GetClientRect(&rtDrawPicture);
	if (m_rtDrawPicture != rtDrawPicture)
	{
		DrawPicture(0);
		m_rtDrawPicture = rtDrawPicture;
	}
	else
		DrawPicture(3);
	// Do not call CDialog::OnPaint() for painting messages*/
}

void CPictureDlg::DocToClient(CPoint &pt)
{
	double d;

	d=m_dProportion*pt.x;
	pt.x=ftoi(d);
	d=m_dProportion*pt.y;
	pt.y=ftoi(d);
	pt.Offset(m_ptStartPos);
}

void CPictureDlg::OnDraw() 
{
	// TODO: Add your control notification handler code here

	UpdateData(true);
	CDialog::OnOK();
}

void CPictureDlg::DrawPicture(int nLarge)
{
	CWnd *pWnd = GetDlgItem(IDC_DRAWPICTURE);   
  CDC* pDC = pWnd->GetWindowDC(); 
  CRect rtDrawPicture;
 
	pWnd->UpdateWindow();
	if (nLarge == 0)
	{//画全部图形
		pWnd->GetClientRect(&rtDrawPicture);
		m_dProportion=(double)(rtDrawPicture.Height()-10)/(double)m_lTotalY;
		double d=(double)(rtDrawPicture.Width()-10)/(double)m_lTotalX;
		if(m_dProportion>d)
		{
			m_dProportion=d;
			m_ptStartPos.x = 5;
			m_ptStartPos.y = (long)((rtDrawPicture.Height() - m_lTotalY * m_dProportion) / 2);
		}
		else
		{
			m_ptStartPos.y = 5;
			m_ptStartPos.x = (long)((rtDrawPicture.Width() - m_lTotalX *m_dProportion) / 2);
		}
	}
	else if (nLarge == 1)
	{//画鼠标框放大部分图形
		pWnd->GetClientRect(&rtDrawPicture);
		double dProportion,dProportion1,dProportion2;

		dProportion1 = fabs(rtDrawPicture.Width()) / fabs(m_ptMouseDownPos.x-m_ptMouseUpPos.x);
		dProportion2 = fabs(rtDrawPicture.Height()) / fabs(m_ptMouseDownPos.y-m_ptMouseUpPos.y);
		if (dProportion1 > dProportion2)
		{
			dProportion = dProportion2;
			dProportion1 = fabs(rtDrawPicture.Width()) / dProportion2;
			dProportion1 = (dProportion1 - fabs(m_ptMouseDownPos.x-m_ptMouseUpPos.x)) / 2;
			dProportion2 = 0;
		}
		else
		{
			dProportion = dProportion1;
			dProportion2 = fabs(rtDrawPicture.Height()) / dProportion1;
			dProportion2 = (dProportion2 - fabs(m_ptMouseDownPos.y-m_ptMouseUpPos.y)) / 2;
			dProportion1 = 0;
		}
		m_dProportion *= dProportion;
		if (m_ptMouseDownPos.x > m_ptMouseUpPos.x)
			m_ptStartPos.x = -(long)((m_ptMouseUpPos.x - dProportion1 - m_ptStartPos.x) * dProportion);
		else
			m_ptStartPos.x = -(long)((m_ptMouseDownPos.x - dProportion1 - m_ptStartPos.x) * dProportion);
		if (m_ptMouseDownPos.y > m_ptMouseUpPos.y)
			m_ptStartPos.y = -(long)((m_ptMouseUpPos.y - dProportion2 - m_ptStartPos.y) * dProportion);
		else
			m_ptStartPos.y = -(long)((m_ptMouseDownPos.y - dProportion2 - m_ptStartPos.y) * dProportion);
	}

if (display==0 || display==3)//0、3等待
{
	//无闪烁重画
  CDC memDC;
  CBitmap bitmap;
  CBitmap* pOldBitmap;
	CRect rectClient; 
  
  pWnd->GetClientRect(&rectClient);
  memDC.CreateCompatibleDC(pDC);

  bitmap.CreateCompatibleBitmap(pDC, rectClient.right+2, rectClient.bottom+2);
 	pOldBitmap=memDC.SelectObject(&bitmap);
	//memDC.FillSolidRect(0,0,rectClient.right+2,rectClient.bottom+2,RGB(255,255,255));

	int nLineType=LINE_TYPE_SOLID;
	int nLineType1=LINE_TYPE_SOLID;
	long lMinLength = m_lFileLength + 10 * 40;
	long lMaxLength;
	CPoint pt,pt1,pt2;
	int nSize;
	POSITION pos=theApp.m_listPrimitive.GetHeadPosition();
	CPen pen(nLineType1,1,RGB(0,0,0));
	CPen *pOldPen=memDC.SelectObject(&pen);
	memDC.Rectangle(0,0,rectClient.Width()+2,rectClient.Height()+2);
	while(pos)
	{
		CGLPrimitive *p=(CGLPrimitive*)theApp.m_listPrimitive.GetNext(pos);
		switch(p->m_nType)
		{
		case PRIMITIVE_LINE:
			nSize=p->m_anData.GetSize()/2;
			if(nSize)
			{
				if(theApp.m_nRotate==1)
				{
					pt.y=p->m_anData.GetAt(0)-m_lFileMinLength;
					pt.x=m_lFileLength-(p->m_anData.GetAt(1)-m_lFileMinWidth);
				}
				else
				{
					pt.x=p->m_anData.GetAt(0)-m_lFileMinLength;
					pt.y=p->m_anData.GetAt(1)-m_lFileMinWidth;
				}
				for(int i=1;i<nSize;i++)
				{
					if(theApp.m_nRotate==1)
					{
						pt1.y=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt1.x=m_lFileLength-(p->m_anData.GetAt(i*2+1)-m_lFileMinWidth);
					}
					else
					{
						pt1.x=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt1.y=p->m_anData.GetAt(i*2+1)-m_lFileMinWidth;
					}
					if(pt1.y<pt.y)
					{
						pt2=pt;
						pt=pt1;
						pt1=pt2;
					}
					if(pt.y>m_lTotalY)
					{
						pt.x+=((pt.y/m_lTotalY)*lMinLength);
						pt1.x+=((pt.y/m_lTotalY)*lMinLength);
						pt1.y-=((pt.y/m_lTotalY)*m_lTotalY);
						pt.y=pt.y%m_lTotalY;
					}
					while(pt1.y>m_lTotalY)
					{
						pt2.y=m_lTotalY;
						pt2.x=(pt2.y-pt.y)*(pt1.x-pt.x)/(pt1.y-pt.y)+pt.x;
						pt.y=m_lTotalY-pt.y;
						DocToClient(pt);
						memDC.MoveTo(pt);
						pt.x=pt2.x;
						pt.y=m_lTotalY-pt2.y;
						DocToClient(pt);
						memDC.LineTo(pt);
						pt.y=0;
						pt.x=pt2.x+lMinLength;
						pt1.y-=m_lTotalY;
						pt1.x+=lMinLength;
					}
					pt.y=m_lTotalY-pt.y;
					DocToClient(pt);
					memDC.MoveTo(pt);
					pt1.y=m_lTotalY-pt1.y;
					DocToClient(pt1);
					memDC.LineTo(pt1);


					if(theApp.m_nRotate==1)
					{
						pt.y=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt.x=m_lFileLength-(p->m_anData.GetAt(i*2+1)-m_lFileMinWidth);
					}
					else
					{
						pt.x=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt.y=p->m_anData.GetAt(i*2+1)-m_lFileMinWidth;
					}
				}

				//pt.x=p->m_anData.GetAt(0);
				//pt.y=m_lTotalY-p->m_anData.GetAt(1);
				//DocToClient(pt);
				//memDC.MoveTo(pt);
				
				//for(int i=1;i<nSize;i++)
				//{
					//pt.x=p->m_anData.GetAt(i*2);
					//pt.y=m_lTotalY-p->m_anData.GetAt(i*2+1);
					//DocToClient(pt);
					//memDC.LineTo(pt);
				//}
			}
			break;
		case PRIMITIVE_LT:
			nSize=p->m_anData.GetSize();
			if(nSize==0)
			{
				nLineType=nLineType1;
				nLineType1=LINE_TYPE_SOLID;
				pen.DeleteObject();
				pen.CreatePen(nLineType1,1,RGB(0,0,0));
				memDC.SelectObject(&pen);
			}
			else
			{
				nSize=p->m_anData.GetAt(0);
				if(nSize==99)
				{
					if(nLineType1==LINE_TYPE_SOLID)
					{
						nLineType1=nLineType;
						pen.DeleteObject();
						pen.CreatePen(nLineType1,1,RGB(0,0,0));
						memDC.SelectObject(&pen);
					}
				}
				else if((nSize>=-8)&&(nSize<=8))
				{
					nLineType1=abs(nSize);
					if(nLineType1<=1)
						nLineType1=LINE_TYPE_DOT;
					else if(nLineType1<=3)
						nLineType1=LINE_TYPE_DASH;
					else
						nLineType1=LINE_TYPE_DASHDOT;
					pen.DeleteObject();
					pen.CreatePen(nLineType1,1,RGB(0,0,0));
					memDC.SelectObject(&pen);
				}
			}
			break;
		}
	}
	if (lMinLength < m_lTotalX)
	{
		pen.DeleteObject();
		pen.CreatePen(LINE_TYPE_DOT,1,RGB(0,0,0));
		memDC.SelectObject(&pen);
		lMinLength = 0;
		lMaxLength = m_lFileLength;
		if (theApp.m_dMarkSpace < 50)
			theApp.m_dMarkSpace = 50;
		while (lMinLength < m_lTotalX)
		{
			if (lMaxLength < m_lTotalX)
			{
				pt.x = lMinLength + 20 * 40;
				pt.y = 0;
				while (pt.x < lMaxLength)
				{
					pt1 = pt;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 10 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt.x += long(theApp.m_dMarkSpace * 40);
				}
				pt1.y = 0;
				pt1.x = lMaxLength + 5 * 40;
				DocToClient(pt1);
				memDC.MoveTo(pt1);
				pt1.y = m_lTotalY;
				pt1.x = lMaxLength + 5 * 40;
				DocToClient(pt1);
				memDC.LineTo(pt1);
			}
			if (lMinLength > m_lFileLength)
			{
				pt.x = lMinLength + 20 * 40;
				pt.y = m_lTotalY;
				while (pt.x < lMaxLength)
				{
					pt1 = pt;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 10 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt.x += long(theApp.m_dMarkSpace * 40);
				}
			}
			lMinLength += (m_lFileLength + 10 * 40);
			lMaxLength += (m_lFileLength + 10 * 40);
		}
	}
	memDC.SelectObject(pOldPen);
	pen.DeleteObject();

		// 位图显示
	pos = theApp.m_listBitListPrimitive.GetHeadPosition();
	//POSITION pos1 = theApp.m_listBitDataPrimitive.GetHeadPosition();
	while(pos != NULL)// && pos1 != NULL)
	{
		//BitPrimitive *pBitPtr = (BitPrimitive*)theApp.m_listTextPrimitive.GetNext(pos);
		BitList *pBitPtr = (BitList*)theApp.m_listBitListPrimitive.GetNext(pos);
		//BitPrimitive *pBitPos = (BitPrimitive*)theApp.m_listBitDataPrimitive.GetNext(pos1);

		CPoint  cPostLU,cPostLD, cPostRU, cPostRD, ptLUNEW;
		cPostLU.x = pBitPtr->bPostLU.x-m_lFileMinLength; cPostLU.y = m_lTotalY-(pBitPtr->bPostLU.y-m_lFileMinWidth);
		cPostLD.x = pBitPtr->bPostLD.x-m_lFileMinLength; cPostLD.y = m_lTotalY-(pBitPtr->bPostLD.y-m_lFileMinWidth);
		cPostRU.x = pBitPtr->bPostRU.x-m_lFileMinLength; cPostRU.y = m_lTotalY-(pBitPtr->bPostRU.y-m_lFileMinWidth);
		cPostRD.x = pBitPtr->bPostRD.x-m_lFileMinLength; cPostRD.y = m_lTotalY-(pBitPtr->bPostRD.y-m_lFileMinWidth);

		DocToClient(cPostLU);
		//memDC.MoveTo(cPostLU);
		
		DocToClient(cPostLD);
		//memDC.LineTo(cPostRU);
		
		DocToClient(cPostRU);
		//memDC.LineTo(cPostRD);

		DocToClient(cPostRD);
		//memDC.LineTo(cPostLD);
		
		//cPostLU.x = pBitPos->m_BitPost[0]-m_lFileMinLength; cPostLU.y = m_lTotalY-(pBitPos->m_BitPost[1]-m_lFileMinWidth);
		//DocToClient(cPostLU);
		//memDC.LineTo(cPostLU);

		DrawBitmap(&memDC,pBitPtr->pBitmap,pBitPtr->m_bTransparent , RGB(255,255,255), cPostLU, cPostLD, cPostRU, cPostRD);
	}

if (theApp.m_nFont==0)
	{
		memDC.SetBkMode(TRANSPARENT);
		pos=theApp.m_listTextPrimitive.GetHeadPosition();
		double dTextAngle=0;
		double dTextHeight=0.375;
		double dTextWidth=0.285;
		CString  cFontName;
		unsigned char IsItali;
		int Weight;
		lMinLength = m_lFileLength + 10 * 40;
		if(theApp.m_nRotate==1)
			dTextAngle+=90;
		while(pos != NULL)
		{
			CGLText *pText=(CGLText*)theApp.m_listTextPrimitive.GetNext(pos);
			if(pText->m_nType==PRIMITIVE_LB)
			{
				if(dTextHeight==0)
					break;
				CFont font;
				if(!font.CreateStockObject(DEFAULT_GUI_FONT)) 
					VERIFY(font.CreateStockObject(SYSTEM_FONT));
				CFont *pOldFont=memDC.SelectObject(&font);
				LOGFONT lf;
				pOldFont->GetLogFont(&lf);
				
				if(bIsEncrypt)
					strcpy(lf.lfFaceName,cFontName);
				else
					strcpy(lf.lfFaceName,"HP Roman 8");
				lf.lfHeight=ftoi(dTextHeight*400*m_dProportion);
				
				//lf.lfHeight=ftoi(dTextHeight*m_dProportion);
				//if(abs(lf.lfHeight)<1)
					//lf.lfHeight=1;
				//lf.lfWidth=ftoi(dTextWidth*400*m_dProportion);//dTextWidth宽度厘米×400（1mm的像素点数）
				//lf.lfWidth=ftoi(dTextWidth*m_dProportion);
				//if(abs(lf.lfWidth)<1)
				//	lf.lfWidth=1;
				
				lf.lfWidth=0;
				if(bIsEncrypt)
				{
					lf.lfWeight = pText->m_iWeight;
					lf.lfItalic = pText->m_cIsItali;
				}
				else
					lf.lfWeight=FW_THIN;
				lf.lfOrientation=(long)dTextAngle*10;//ftoi(dTextAngle*10);
				lf.lfEscapement=(long)dTextAngle*10;//ftoi(dTextAngle*10);
				CFont fontNew;
				BOOL b=fontNew.CreateFontIndirect(&lf);
				memDC.SelectObject(&fontNew);
				
				CPoint pt;
				if(theApp.m_nRotate==1)
				{
					pt.y=ftoi(pText->m_adPara.GetAt(0))-m_lFileMinLength;
					pt.x=m_lFileLength-(ftoi(pText->m_adPara.GetAt(1))-m_lFileMinWidth);
				}
				else
				{
					pt.x=ftoi(pText->m_adPara.GetAt(0))-m_lFileMinLength;
					pt.y=ftoi(pText->m_adPara.GetAt(1))-m_lFileMinWidth;
				}
				if(pt.y>m_lTotalY)
				{
					pt.x+=((pt.y/m_lTotalY)*lMinLength);
					pt.y=m_lTotalY-pt.y%m_lTotalY;
				}
				else
					pt.y=m_lTotalY-pt.y;
				DocToClient(pt);
				nSize=pText->m_strText.GetLength();
				if(nSize>256)
					nSize=256;
				memDC.SetTextAlign(TA_LEFT | TA_BASELINE);//文本对齐方式
				memDC.TextOut(pt.x,pt.y,pText->m_strText);
				memDC.SelectObject(pOldFont);
				font.DeleteObject();
				fontNew.DeleteObject();
			}
			else if(pText->m_nType==PRIMITIVE_SI)
			{
				nSize=pText->m_adPara.GetSize();
				if(nSize>=2)
				{
					dTextHeight=pText->m_adPara.GetAt(1);
					dTextWidth=pText->m_adPara.GetAt(0);
				}
				else 
				{
					dTextHeight=0.375;
					dTextWidth=0.285;
				}
			}
			else if(pText->m_nType==PRIMITIVE_DI)
			{
				double d1;
				double d2;
				nSize=pText->m_adPara.GetSize();
				
				if(bIsEncrypt)
				{
					dTextAngle = pText->m_adPara.GetAt(0);
					IsItali = pText->m_cIsItali;
					Weight = pText->m_iWeight;
					continue;
				}
				
				if(nSize>=2)
				{
					/*d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);
					if((d1==0)&&(d2!=0))
						dTextAngle=90;
					else if((d1!=0)&&(d2==0))
						dTextAngle=0;
					else if((d1!=0)&&(d2!=0))
						dTextAngle=atan2(d1,d2);
					else
						continue;*/
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);

					if(d1<0.10 && d1>-0.10)
						d1=0;
					if(d2<0.10 && d2>-0.10)
						d2=0;

					if(d1==0 && d2==1)
						dTextAngle=90;
					else if(d1==1 && d2==0)
						dTextAngle=0;
					else if(d1==0 && d2==-1)
						dTextAngle=270;
					else if(d1==-1 && d2==0)
						dTextAngle=-180;
					else
						dTextAngle=atan2(d2,d1)*180/3.14159265359;
					double tmp = (dTextAngle - (int)dTextAngle);
					if(abs((int)(tmp*10)) >= 5)
					{
						dTextAngle = dTextAngle < 0 ? dTextAngle += -1 : dTextAngle+=1;
					}
					dTextAngle = (int)dTextAngle;
				}
				else
					dTextAngle=0;
				if(theApp.m_nRotate==1)
					dTextAngle+=90;
			}
			else if(pText->m_nType == PRIMITIVE_FN)
			{
				cFontName = pText->m_cFontName;
			}
			else if(pText->m_nType==PRIMITIVE_DR)
			{
				double d1;
				double d2;
				nSize=pText->m_adPara.GetSize();
				if(nSize==0)
					dTextAngle=0;
				else if(nSize==2)
				{
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);
					if((d1==0)&&(d2!=0))
						dTextAngle=90;
					else if((d1!=0)&&(d2==0))
						dTextAngle=0;
					else
						continue;
				}
				if(nSize==4)
				{
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);
					if((d1==0)&&(d2!=0))
						dTextAngle=90;
					else if((d1!=0)&&(d2==0))
						dTextAngle=0;
					else if((d1!=0)&&(d2!=0)&&(fabs(d1)<=100)&&(fabs(d2)<=100))
					{
						d1=d1*pText->m_adPara.GetAt(2);
						d2=d2*pText->m_adPara.GetAt(3);
						dTextAngle=atan2(d1,d2);
					}
					else
						continue;
				}
				if(theApp.m_nRotate==1)
					dTextAngle+=90;
			}
		}
	}
	pDC->BitBlt(0, 0, rectClient.right+2, rectClient.bottom+2, &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
}

if (display==1)//1打印中
{
	//无闪烁重画
  CDC memDC;
  CBitmap bitmap;
  CBitmap* pOldBitmap;
	CRect rectClient; 
  
  pWnd->GetClientRect(&rectClient);
  memDC.CreateCompatibleDC(pDC);

  bitmap.CreateCompatibleBitmap(pDC, rectClient.right+2, rectClient.bottom+2);
 	pOldBitmap=memDC.SelectObject(&bitmap);
	//memDC.FillSolidRect(0,0,rectClient.right+2,rectClient.bottom+2,RGB(255,255,255));

	int nLineType=LINE_TYPE_SOLID;
	int nLineType1=LINE_TYPE_SOLID;
	long lMinLength = m_lFileLength + 10 * 40;
	long lMaxLength;
	CPoint pt,pt1,pt2;
	int nSize;
	POSITION pos=theApp.m_listPrimitive.GetHeadPosition();
	CPen pen(nLineType1,1,RGB(0,0,0));
	CPen *pOldPen=memDC.SelectObject(&pen);
	memDC.Rectangle(0,0,rectClient.Width()+2,rectClient.Height()+2);
	while(pos)
	{
		CGLPrimitive *p=(CGLPrimitive*)theApp.m_listPrimitive.GetNext(pos);
		switch(p->m_nType)
		{
		case PRIMITIVE_LINE:
			nSize=p->m_anData.GetSize()/2;
			if(nSize)
			{
				if(theApp.m_nRotate==1)
				{
					pt.y=p->m_anData.GetAt(0)-m_lFileMinLength;
					pt.x=m_lFileLength-(p->m_anData.GetAt(1)-m_lFileMinWidth);
				}
				else
				{
					pt.x=p->m_anData.GetAt(0)-m_lFileMinLength;
					pt.y=p->m_anData.GetAt(1)-m_lFileMinWidth;
				}
				for(int i=1;i<nSize;i++)
				{
					if(theApp.m_nRotate==1)
					{
						pt1.y=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt1.x=m_lFileLength-(p->m_anData.GetAt(i*2+1)-m_lFileMinWidth);
					}
					else
					{
						pt1.x=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt1.y=p->m_anData.GetAt(i*2+1)-m_lFileMinWidth;
					}
					if(pt1.y<pt.y)
					{
						pt2=pt;
						pt=pt1;
						pt1=pt2;
					}
					if(pt.y>m_lTotalY)
					{
						pt.x+=((pt.y/m_lTotalY)*lMinLength);
						pt1.x+=((pt.y/m_lTotalY)*lMinLength);
						pt1.y-=((pt.y/m_lTotalY)*m_lTotalY);
						pt.y=pt.y%m_lTotalY;
					}
					while(pt1.y>m_lTotalY)
					{
						pt2.y=m_lTotalY;
						pt2.x=(pt2.y-pt.y)*(pt1.x-pt.x)/(pt1.y-pt.y)+pt.x;
						pt.y=m_lTotalY-pt.y;
						DocToClient(pt);
						memDC.MoveTo(pt);
						pt.x=pt2.x;
						pt.y=m_lTotalY-pt2.y;
						DocToClient(pt);
						memDC.LineTo(pt);
						pt.y=0;
						pt.x=pt2.x+lMinLength;
						pt1.y-=m_lTotalY;
						pt1.x+=lMinLength;
					}
					pt.y=m_lTotalY-pt.y;
					DocToClient(pt);
					memDC.MoveTo(pt);

					if(displayx1==0)//实时显示
					{
					displayx1=pt.x;
					}
					else if(displayx1!=0 && displayx1>pt.x)//实时显示
					{
                    displayx1=pt.x;
					}

					pt1.y=m_lTotalY-pt1.y;
					DocToClient(pt1);
					memDC.LineTo(pt1);

					if(displayx2==0)//实时显示
					{
					displayx2=pt1.x;
					}
					else if(displayx2!=0 && displayx2<pt1.x)//实时显示
					{
                    displayx2=pt1.x;
					}

					if(theApp.m_nRotate==1)
					{
						pt.y=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt.x=m_lFileLength-(p->m_anData.GetAt(i*2+1)-m_lFileMinWidth);
					}
					else
					{
						pt.x=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt.y=p->m_anData.GetAt(i*2+1)-m_lFileMinWidth;
					}
				}

				//pt.x=p->m_anData.GetAt(0);
				//pt.y=m_lTotalY-p->m_anData.GetAt(1);
				//DocToClient(pt);
				//memDC.MoveTo(pt);
				
				//for(int i=1;i<nSize;i++)
				//{
					//pt.x=p->m_anData.GetAt(i*2);
					//pt.y=m_lTotalY-p->m_anData.GetAt(i*2+1);
					//DocToClient(pt);
					//memDC.LineTo(pt);
				//}
			}
			break;
		case PRIMITIVE_LT:
			nSize=p->m_anData.GetSize();
			if(nSize==0)
			{
				nLineType=nLineType1;
				nLineType1=LINE_TYPE_SOLID;
				pen.DeleteObject();
				pen.CreatePen(nLineType1,1,RGB(0,0,0));
				memDC.SelectObject(&pen);
			}
			else
			{
				nSize=p->m_anData.GetAt(0);
				if(nSize==99)
				{
					if(nLineType1==LINE_TYPE_SOLID)
					{
						nLineType1=nLineType;
						pen.DeleteObject();
						pen.CreatePen(nLineType1,1,RGB(0,0,0));
						memDC.SelectObject(&pen);
					}
				}
				else if((nSize>=-8)&&(nSize<=8))
				{
					nLineType1=abs(nSize);
					if(nLineType1<=1)
						nLineType1=LINE_TYPE_DOT;
					else if(nLineType1<=3)
						nLineType1=LINE_TYPE_DASH;
					else
						nLineType1=LINE_TYPE_DASHDOT;
					pen.DeleteObject();
					pen.CreatePen(nLineType1,1,RGB(0,0,0));
					memDC.SelectObject(&pen);
				}
			}
			break;
		}
	}
	if (lMinLength < m_lTotalX)
	{
		pen.DeleteObject();
		pen.CreatePen(LINE_TYPE_DOT,1,RGB(0,0,0));
		memDC.SelectObject(&pen);
		lMinLength = 0;
		lMaxLength = m_lFileLength;
		if (theApp.m_dMarkSpace < 50)
			theApp.m_dMarkSpace = 50;
		while (lMinLength < m_lTotalX)
		{
			if (lMaxLength < m_lTotalX)
			{
				pt.x = lMinLength + 20 * 40;
				pt.y = 0;
				while (pt.x < lMaxLength)
				{
					pt1 = pt;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 10 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt.x += long(theApp.m_dMarkSpace * 40);
				}
				pt1.y = 0;
				pt1.x = lMaxLength + 5 * 40;
				DocToClient(pt1);
				memDC.MoveTo(pt1);
				pt1.y = m_lTotalY;
				pt1.x = lMaxLength + 5 * 40;
				DocToClient(pt1);
				memDC.LineTo(pt1);
			}
			if (lMinLength > m_lFileLength)
			{
				pt.x = lMinLength + 20 * 40;
				pt.y = m_lTotalY;
				while (pt.x < lMaxLength)
				{
					pt1 = pt;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 10 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt.x += long(theApp.m_dMarkSpace * 40);
				}
			}
			lMinLength += (m_lFileLength + 10 * 40);
			lMaxLength += (m_lFileLength + 10 * 40);
		}
	}
	memDC.SelectObject(pOldPen);
	pen.DeleteObject();

			// 位图显示
	pos = theApp.m_listBitListPrimitive.GetHeadPosition();
	//POSITION pos1 = theApp.m_listBitDataPrimitive.GetHeadPosition();
	while(pos != NULL)// && pos1 != NULL)
	{
		//BitPrimitive *pBitPtr = (BitPrimitive*)theApp.m_listTextPrimitive.GetNext(pos);
		BitList *pBitPtr = (BitList*)theApp.m_listBitListPrimitive.GetNext(pos);
		//BitPrimitive *pBitPos = (BitPrimitive*)theApp.m_listBitDataPrimitive.GetNext(pos1);

		CPoint  cPostLU,cPostLD, cPostRU, cPostRD, ptLUNEW;
		cPostLU.x = pBitPtr->bPostLU.x-m_lFileMinLength; cPostLU.y = m_lTotalY-(pBitPtr->bPostLU.y-m_lFileMinWidth);
		cPostLD.x = pBitPtr->bPostLD.x-m_lFileMinLength; cPostLD.y = m_lTotalY-(pBitPtr->bPostLD.y-m_lFileMinWidth);
		cPostRU.x = pBitPtr->bPostRU.x-m_lFileMinLength; cPostRU.y = m_lTotalY-(pBitPtr->bPostRU.y-m_lFileMinWidth);
		cPostRD.x = pBitPtr->bPostRD.x-m_lFileMinLength; cPostRD.y = m_lTotalY-(pBitPtr->bPostRD.y-m_lFileMinWidth);

		DocToClient(cPostLU);
		//memDC.MoveTo(cPostLU);
		
		DocToClient(cPostLD);
		//memDC.LineTo(cPostRU);
		
		DocToClient(cPostRU);
		//memDC.LineTo(cPostRD);

		DocToClient(cPostRD);
		//memDC.LineTo(cPostLD);
		
		//cPostLU.x = pBitPos->m_BitPost[0]-m_lFileMinLength; cPostLU.y = m_lTotalY-(pBitPos->m_BitPost[1]-m_lFileMinWidth);
		//DocToClient(cPostLU);
		//memDC.LineTo(cPostLU);

		DrawBitmap(&memDC,pBitPtr->pBitmap,pBitPtr->m_bTransparent , RGB(255,255,255), cPostLU, cPostLD, cPostRU, cPostRD);
	}

if (theApp.m_nFont==0)
	{
		memDC.SetBkMode(TRANSPARENT);
		pos=theApp.m_listTextPrimitive.GetHeadPosition();
		double dTextAngle=0;
		double dTextHeight=0.375;
		double dTextWidth=0.285;
		CString  cFontName;
		unsigned char IsItali;
		int Weight;
		lMinLength = m_lFileLength + 10 * 40;
		if(theApp.m_nRotate==1)
			dTextAngle+=90;
		while(pos != NULL)
		{
			CGLText *pText=(CGLText*)theApp.m_listTextPrimitive.GetNext(pos);
			if(pText->m_nType==PRIMITIVE_LB)
			{
				if(dTextHeight==0)
					break;
				CFont font;
				if(!font.CreateStockObject(DEFAULT_GUI_FONT)) 
					VERIFY(font.CreateStockObject(SYSTEM_FONT));
				CFont *pOldFont=memDC.SelectObject(&font);
				LOGFONT lf;
				pOldFont->GetLogFont(&lf);
				
				if(bIsEncrypt)
					strcpy(lf.lfFaceName,cFontName);
				else
					strcpy(lf.lfFaceName,"HP Roman 8");
				lf.lfHeight=ftoi(dTextHeight*400*m_dProportion);
				
				//lf.lfHeight=ftoi(dTextHeight*m_dProportion);
				//if(abs(lf.lfHeight)<1)
					//lf.lfHeight=1;
				//lf.lfWidth=ftoi(dTextWidth*400*m_dProportion);//dTextWidth宽度厘米×400（1mm的像素点数）
				//lf.lfWidth=ftoi(dTextWidth*m_dProportion);
				//if(abs(lf.lfWidth)<1)
				//	lf.lfWidth=1;
				
				lf.lfWidth=0;
				if(bIsEncrypt)
				{
					lf.lfWeight = pText->m_iWeight;
					lf.lfItalic = pText->m_cIsItali;
				}
				else
					lf.lfWeight=FW_THIN;
				lf.lfOrientation=(long)dTextAngle*10;//ftoi(dTextAngle*10);
				lf.lfEscapement=(long)dTextAngle*10;//ftoi(dTextAngle*10);
				CFont fontNew;
				BOOL b=fontNew.CreateFontIndirect(&lf);
				memDC.SelectObject(&fontNew);
				
				CPoint pt;
				if(theApp.m_nRotate==1)
				{
					pt.y=ftoi(pText->m_adPara.GetAt(0))-m_lFileMinLength;
					pt.x=m_lFileLength-(ftoi(pText->m_adPara.GetAt(1))-m_lFileMinWidth);
				}
				else
				{
					pt.x=ftoi(pText->m_adPara.GetAt(0))-m_lFileMinLength;
					pt.y=ftoi(pText->m_adPara.GetAt(1))-m_lFileMinWidth;
				}
				if(pt.y>m_lTotalY)
				{
					pt.x+=((pt.y/m_lTotalY)*lMinLength);
					pt.y=m_lTotalY-pt.y%m_lTotalY;
				}
				else
					pt.y=m_lTotalY-pt.y;
				DocToClient(pt);
				nSize=pText->m_strText.GetLength();
				if(nSize>256)
					nSize=256;
				memDC.SetTextAlign(TA_LEFT | TA_BASELINE);//文本对齐方式
				memDC.TextOut(pt.x,pt.y,pText->m_strText);
				memDC.SelectObject(pOldFont);
				font.DeleteObject();
				fontNew.DeleteObject();
			}
			else if(pText->m_nType==PRIMITIVE_SI)
			{
				nSize=pText->m_adPara.GetSize();
				if(nSize>=2)
				{
					dTextHeight=pText->m_adPara.GetAt(1);
					dTextWidth=pText->m_adPara.GetAt(0);
				}
				else 
				{
					dTextHeight=0.375;
					dTextWidth=0.285;
				}
			}
			else if(pText->m_nType==PRIMITIVE_DI)
			{
				double d1;
				double d2;
				nSize=pText->m_adPara.GetSize();
				
				if(bIsEncrypt)
				{
					dTextAngle = pText->m_adPara.GetAt(0);
					IsItali = pText->m_cIsItali;
					Weight = pText->m_iWeight;
					continue;
				}
				
				if(nSize>=2)
				{
					/*d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);
					if((d1==0)&&(d2!=0))
						dTextAngle=90;
					else if((d1!=0)&&(d2==0))
						dTextAngle=0;
					else if((d1!=0)&&(d2!=0))
						dTextAngle=atan2(d1,d2);
					else
						continue;*/
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);

					if(d1<0.10 && d1>-0.10)
						d1=0;
					if(d2<0.10 && d2>-0.10)
						d2=0;

					if(d1==0 && d2==1)
						dTextAngle=90;
					else if(d1==1 && d2==0)
						dTextAngle=0;
					else if(d1==0 && d2==-1)
						dTextAngle=270;
					else if(d1==-1 && d2==0)
						dTextAngle=-180;
					else
						dTextAngle=atan2(d2,d1)*180/3.14159265359;
					double tmp = (dTextAngle - (int)dTextAngle);
					if(abs((int)(tmp*10)) >= 5)
					{
						dTextAngle = dTextAngle < 0 ? dTextAngle += -1 : dTextAngle+=1;
					}
					dTextAngle = (int)dTextAngle;
				}
				else
					dTextAngle=0;
				if(theApp.m_nRotate==1)
					dTextAngle+=90;
			}
			else if(pText->m_nType == PRIMITIVE_FN)
			{
				cFontName = pText->m_cFontName;
			}
			else if(pText->m_nType==PRIMITIVE_DR)
			{
				double d1;
				double d2;
				nSize=pText->m_adPara.GetSize();
				if(nSize==0)
					dTextAngle=0;
				else if(nSize==2)
				{
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);
					if((d1==0)&&(d2!=0))
						dTextAngle=90;
					else if((d1!=0)&&(d2==0))
						dTextAngle=0;
					else
						continue;
				}
				if(nSize==4)
				{
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);
					if((d1==0)&&(d2!=0))
						dTextAngle=90;
					else if((d1!=0)&&(d2==0))
						dTextAngle=0;
					else if((d1!=0)&&(d2!=0)&&(fabs(d1)<=100)&&(fabs(d2)<=100))
					{
						d1=d1*pText->m_adPara.GetAt(2);
						d2=d2*pText->m_adPara.GetAt(3);
						dTextAngle=atan2(d1,d2);
					}
					else
						continue;
				}
				if(theApp.m_nRotate==1)
					dTextAngle+=90;
			}
		}
	}

	pDC->BitBlt(0, 0, rectClient.right+2, rectClient.bottom+2, &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	display=11;
}


if (display==11)//1打印中
{
	//无闪烁重画
  CDC memDC;
  CBitmap bitmap;
  CBitmap* pOldBitmap;
	CRect rectClient; 
  
  pWnd->GetClientRect(&rectClient);
  memDC.CreateCompatibleDC(pDC);

  bitmap.CreateCompatibleBitmap(pDC, rectClient.right+2, rectClient.bottom+2);
 	pOldBitmap=memDC.SelectObject(&bitmap);
	//memDC.FillSolidRect(0,0,rectClient.right+2,rectClient.bottom+2,RGB(255,255,255));

	int nLineType=LINE_TYPE_SOLID;
	int nLineType1=LINE_TYPE_SOLID;
	long lMinLength = m_lFileLength + 10 * 40;
	long lMaxLength;
	CPoint pt,pt1,pt2;
	int nSize;
	POSITION pos=theApp.m_listPrimitive.GetHeadPosition();
	CPen pen(nLineType1,1,RGB(255,0,0));
	CPen *pOldPen=memDC.SelectObject(&pen);
	memDC.Rectangle(0,0,rectClient.Width()+2,rectClient.Height()+2);
	while(pos)
	{
		CGLPrimitive *p=(CGLPrimitive*)theApp.m_listPrimitive.GetNext(pos);
		switch(p->m_nType)
		{
		case PRIMITIVE_LINE:
			nSize=p->m_anData.GetSize()/2;
			if(nSize)
			{
				if(theApp.m_nRotate==1)
				{
					pt.y=p->m_anData.GetAt(0)-m_lFileMinLength;
					pt.x=m_lFileLength-(p->m_anData.GetAt(1)-m_lFileMinWidth);
				}
				else
				{
					pt.x=p->m_anData.GetAt(0)-m_lFileMinLength;
					pt.y=p->m_anData.GetAt(1)-m_lFileMinWidth;
				}
				for(int i=1;i<nSize;i++)
				{
					if(theApp.m_nRotate==1)
					{
						pt1.y=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt1.x=m_lFileLength-(p->m_anData.GetAt(i*2+1)-m_lFileMinWidth);
					}
					else
					{
						pt1.x=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt1.y=p->m_anData.GetAt(i*2+1)-m_lFileMinWidth;
					}
					if(pt1.y<pt.y)
					{
						pt2=pt;
						pt=pt1;
						pt1=pt2;
					}
					if(pt.y>m_lTotalY)
					{
						pt.x+=((pt.y/m_lTotalY)*lMinLength);
						pt1.x+=((pt.y/m_lTotalY)*lMinLength);
						pt1.y-=((pt.y/m_lTotalY)*m_lTotalY);
						pt.y=pt.y%m_lTotalY;
					}
					while(pt1.y>m_lTotalY)
					{
						pt2.y=m_lTotalY;
						pt2.x=(pt2.y-pt.y)*(pt1.x-pt.x)/(pt1.y-pt.y)+pt.x;
						pt.y=m_lTotalY-pt.y;
						DocToClient(pt);
						memDC.MoveTo(pt);
						pt.x=pt2.x;
						pt.y=m_lTotalY-pt2.y;
						DocToClient(pt);
						memDC.LineTo(pt);
						pt.y=0;
						pt.x=pt2.x+lMinLength;
						pt1.y-=m_lTotalY;
						pt1.x+=lMinLength;
					}
					pt.y=m_lTotalY-pt.y;
					DocToClient(pt);
					memDC.MoveTo(pt);

					if(displayx1==0)//实时显示
					{
					displayx1=pt.x;
					}
					else if(displayx1!=0 && displayx1>pt.x)//实时显示
					{
                    displayx1=pt.x;
					}

					pt1.y=m_lTotalY-pt1.y;
					DocToClient(pt1);
					memDC.LineTo(pt1);

					if(displayx2==0)//实时显示
					{
					displayx2=pt1.x;
					}
					else if(displayx2!=0 && displayx2<pt1.x)//实时显示
					{
                    displayx2=pt1.x;
					}

					if(theApp.m_nRotate==1)
					{
						pt.y=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt.x=m_lFileLength-(p->m_anData.GetAt(i*2+1)-m_lFileMinWidth);
					}
					else
					{
						pt.x=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt.y=p->m_anData.GetAt(i*2+1)-m_lFileMinWidth;
					}
				}

				//pt.x=p->m_anData.GetAt(0);
				//pt.y=m_lTotalY-p->m_anData.GetAt(1);
				//DocToClient(pt);
				//memDC.MoveTo(pt);
				
				//for(int i=1;i<nSize;i++)
				//{
					//pt.x=p->m_anData.GetAt(i*2);
					//pt.y=m_lTotalY-p->m_anData.GetAt(i*2+1);
					//DocToClient(pt);
					//memDC.LineTo(pt);
				//}
			}
			break;
		case PRIMITIVE_LT:
			nSize=p->m_anData.GetSize();
			if(nSize==0)
			{
				nLineType=nLineType1;
				nLineType1=LINE_TYPE_SOLID;
				pen.DeleteObject();
				pen.CreatePen(nLineType1,1,RGB(255,0,0));
				memDC.SelectObject(&pen);
			}
			else
			{
				nSize=p->m_anData.GetAt(0);
				if(nSize==99)
				{
					if(nLineType1==LINE_TYPE_SOLID)
					{
						nLineType1=nLineType;
						pen.DeleteObject();
						pen.CreatePen(nLineType1,1,RGB(255,0,0));
						memDC.SelectObject(&pen);
					}
				}
				else if((nSize>=-8)&&(nSize<=8))
				{
					nLineType1=abs(nSize);
					if(nLineType1<=1)
						nLineType1=LINE_TYPE_DOT;
					else if(nLineType1<=3)
						nLineType1=LINE_TYPE_DASH;
					else
						nLineType1=LINE_TYPE_DASHDOT;
					pen.DeleteObject();
					pen.CreatePen(nLineType1,1,RGB(255,0,0));
					memDC.SelectObject(&pen);
				}
			}
			break;
		}
	}
	if (lMinLength < m_lTotalX)
	{
		pen.DeleteObject();
		pen.CreatePen(LINE_TYPE_DOT,1,RGB(255,0,0));
		memDC.SelectObject(&pen);
		lMinLength = 0;
		lMaxLength = m_lFileLength;
		if (theApp.m_dMarkSpace < 50)
			theApp.m_dMarkSpace = 50;
		while (lMinLength < m_lTotalX)
		{
			if (lMaxLength < m_lTotalX)
			{
				pt.x = lMinLength + 20 * 40;
				pt.y = 0;
				while (pt.x < lMaxLength)
				{
					pt1 = pt;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 10 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt.x += long(theApp.m_dMarkSpace * 40);
				}
				pt1.y = 0;
				pt1.x = lMaxLength + 5 * 40;
				DocToClient(pt1);
				memDC.MoveTo(pt1);
				pt1.y = m_lTotalY;
				pt1.x = lMaxLength + 5 * 40;
				DocToClient(pt1);
				memDC.LineTo(pt1);
			}
			if (lMinLength > m_lFileLength)
			{
				pt.x = lMinLength + 20 * 40;
				pt.y = m_lTotalY;
				while (pt.x < lMaxLength)
				{
					pt1 = pt;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 10 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt.x += long(theApp.m_dMarkSpace * 40);
				}
			}
			lMinLength += (m_lFileLength + 10 * 40);
			lMaxLength += (m_lFileLength + 10 * 40);
		}
	}
	memDC.SelectObject(pOldPen);
	pen.DeleteObject();

			// 位图显示
	pos = theApp.m_listBitListPrimitive.GetHeadPosition();
	//POSITION pos1 = theApp.m_listBitDataPrimitive.GetHeadPosition();
	while(pos != NULL)// && pos1 != NULL)
	{
		//BitPrimitive *pBitPtr = (BitPrimitive*)theApp.m_listTextPrimitive.GetNext(pos);
		BitList *pBitPtr = (BitList*)theApp.m_listBitListPrimitive.GetNext(pos);
		//BitPrimitive *pBitPos = (BitPrimitive*)theApp.m_listBitDataPrimitive.GetNext(pos1);

		CPoint  cPostLU,cPostLD, cPostRU, cPostRD, ptLUNEW;
		cPostLU.x = pBitPtr->bPostLU.x-m_lFileMinLength; cPostLU.y = m_lTotalY-(pBitPtr->bPostLU.y-m_lFileMinWidth);
		cPostLD.x = pBitPtr->bPostLD.x-m_lFileMinLength; cPostLD.y = m_lTotalY-(pBitPtr->bPostLD.y-m_lFileMinWidth);
		cPostRU.x = pBitPtr->bPostRU.x-m_lFileMinLength; cPostRU.y = m_lTotalY-(pBitPtr->bPostRU.y-m_lFileMinWidth);
		cPostRD.x = pBitPtr->bPostRD.x-m_lFileMinLength; cPostRD.y = m_lTotalY-(pBitPtr->bPostRD.y-m_lFileMinWidth);

		DocToClient(cPostLU);
		//memDC.MoveTo(cPostLU);
		
		DocToClient(cPostLD);
		//memDC.LineTo(cPostRU);
		
		DocToClient(cPostRU);
		//memDC.LineTo(cPostRD);

		DocToClient(cPostRD);
		//memDC.LineTo(cPostLD);
		
		//cPostLU.x = pBitPos->m_BitPost[0]-m_lFileMinLength; cPostLU.y = m_lTotalY-(pBitPos->m_BitPost[1]-m_lFileMinWidth);
		//DocToClient(cPostLU);
		//memDC.LineTo(cPostLU);

		DrawBitmap(&memDC,pBitPtr->pBitmap,pBitPtr->m_bTransparent , RGB(255,0,0), cPostLU, cPostLD, cPostRU, cPostRD);
	}

if (theApp.m_nFont==0)
	{
		memDC.SetBkMode(TRANSPARENT);
		pos=theApp.m_listTextPrimitive.GetHeadPosition();
		double dTextAngle=0;
		double dTextHeight=0.375;
		double dTextWidth=0.285;
		CString  cFontName;
		unsigned char IsItali;
		int Weight;
		lMinLength = m_lFileLength + 10 * 40;
		if(theApp.m_nRotate==1)
			dTextAngle+=90;
		while(pos != NULL)
		{
			CGLText *pText=(CGLText*)theApp.m_listTextPrimitive.GetNext(pos);
			if(pText->m_nType==PRIMITIVE_LB)
			{
				if(dTextHeight==0)
					break;
				CFont font;
				if(!font.CreateStockObject(DEFAULT_GUI_FONT)) 
					VERIFY(font.CreateStockObject(SYSTEM_FONT));
				CFont *pOldFont=memDC.SelectObject(&font);
				LOGFONT lf;
				pOldFont->GetLogFont(&lf);
				
				if(bIsEncrypt)
					strcpy(lf.lfFaceName,cFontName);
				else
					strcpy(lf.lfFaceName,"HP Roman 8");
				lf.lfHeight=ftoi(dTextHeight*400*m_dProportion);
				
				//lf.lfHeight=ftoi(dTextHeight*m_dProportion);
				//if(abs(lf.lfHeight)<1)
					//lf.lfHeight=1;
				//lf.lfWidth=ftoi(dTextWidth*400*m_dProportion);//dTextWidth宽度厘米×400（1mm的像素点数）
				//lf.lfWidth=ftoi(dTextWidth*m_dProportion);
				//if(abs(lf.lfWidth)<1)
				//	lf.lfWidth=1;
				
				lf.lfWidth=0;
				if(bIsEncrypt)
				{
					lf.lfWeight = pText->m_iWeight;
					lf.lfItalic = pText->m_cIsItali;
				}
				else
					lf.lfWeight=FW_THIN;
				lf.lfOrientation=(long)dTextAngle*10;//ftoi(dTextAngle*10);
				lf.lfEscapement=(long)dTextAngle*10;//ftoi(dTextAngle*10);
				CFont fontNew;
				BOOL b=fontNew.CreateFontIndirect(&lf);
				memDC.SelectObject(&fontNew);
				
				CPoint pt;
				if(theApp.m_nRotate==1)
				{
					pt.y=ftoi(pText->m_adPara.GetAt(0))-m_lFileMinLength;
					pt.x=m_lFileLength-(ftoi(pText->m_adPara.GetAt(1))-m_lFileMinWidth);
				}
				else
				{
					pt.x=ftoi(pText->m_adPara.GetAt(0))-m_lFileMinLength;
					pt.y=ftoi(pText->m_adPara.GetAt(1))-m_lFileMinWidth;
				}
				if(pt.y>m_lTotalY)
				{
					pt.x+=((pt.y/m_lTotalY)*lMinLength);
					pt.y=m_lTotalY-pt.y%m_lTotalY;
				}
				else
					pt.y=m_lTotalY-pt.y;
				DocToClient(pt);
				nSize=pText->m_strText.GetLength();
				if(nSize>256)
					nSize=256;
				memDC.SetTextAlign(TA_LEFT | TA_BASELINE);//文本对齐方式
				memDC.TextOut(pt.x,pt.y,pText->m_strText);
				memDC.SelectObject(pOldFont);
				font.DeleteObject();
				fontNew.DeleteObject();
			}
			else if(pText->m_nType==PRIMITIVE_SI)
			{
				nSize=pText->m_adPara.GetSize();
				if(nSize>=2)
				{
					dTextHeight=pText->m_adPara.GetAt(1);
					dTextWidth=pText->m_adPara.GetAt(0);
				}
				else 
				{
					dTextHeight=0.375;
					dTextWidth=0.285;
				}
			}
			else if(pText->m_nType==PRIMITIVE_DI)
			{
				double d1;
				double d2;
				nSize=pText->m_adPara.GetSize();
				
				if(bIsEncrypt)
				{
					dTextAngle = pText->m_adPara.GetAt(0);
					IsItali = pText->m_cIsItali;
					Weight = pText->m_iWeight;
					continue;
				}
				
				if(nSize>=2)
				{
					/*d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);
					if((d1==0)&&(d2!=0))
						dTextAngle=90;
					else if((d1!=0)&&(d2==0))
						dTextAngle=0;
					else if((d1!=0)&&(d2!=0))
						dTextAngle=atan2(d1,d2);
					else
						continue;*/
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);

					if(d1<0.10 && d1>-0.10)
						d1=0;
					if(d2<0.10 && d2>-0.10)
						d2=0;

					if(d1==0 && d2==1)
						dTextAngle=90;
					else if(d1==1 && d2==0)
						dTextAngle=0;
					else if(d1==0 && d2==-1)
						dTextAngle=270;
					else if(d1==-1 && d2==0)
						dTextAngle=-180;
					else
						dTextAngle=atan2(d2,d1)*180/3.14159265359;
					double tmp = (dTextAngle - (int)dTextAngle);
					if(abs((int)(tmp*10)) >= 5)
					{
						dTextAngle = dTextAngle < 0 ? dTextAngle += -1 : dTextAngle+=1;
					}
					dTextAngle = (int)dTextAngle;
				}
				else
					dTextAngle=0;
				if(theApp.m_nRotate==1)
					dTextAngle+=90;
			}
			else if(pText->m_nType == PRIMITIVE_FN)
			{
				cFontName = pText->m_cFontName;
			}
			else if(pText->m_nType==PRIMITIVE_DR)
			{
				double d1;
				double d2;
				nSize=pText->m_adPara.GetSize();
				if(nSize==0)
					dTextAngle=0;
				else if(nSize==2)
				{
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);
					if((d1==0)&&(d2!=0))
						dTextAngle=90;
					else if((d1!=0)&&(d2==0))
						dTextAngle=0;
					else
						continue;
				}
				if(nSize==4)
				{
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);
					if((d1==0)&&(d2!=0))
						dTextAngle=90;
					else if((d1!=0)&&(d2==0))
						dTextAngle=0;
					else if((d1!=0)&&(d2!=0)&&(fabs(d1)<=100)&&(fabs(d2)<=100))
					{
						d1=d1*pText->m_adPara.GetAt(2);
						d2=d2*pText->m_adPara.GetAt(3);
						dTextAngle=atan2(d1,d2);
					}
					else
						continue;
				}
				if(theApp.m_nRotate==1)
					dTextAngle+=90;
			}
		}
	}

	float displayx=(float(displayx2)-float(displayx1))/100;
	pDC->BitBlt(0, 0, displayx1+displayx*displayspeed+2, rectClient.bottom+2, &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	if(displayspeed==100)
	{

	 displayspeed=0;
	 displaybusy=0;
     AfxMessageBox("finish!");
	 OnClose();
	 displaybusy=1;
	}
}

/*
if (display==2)//2暂停
{
	//无闪烁重画
  CDC memDC;
  CBitmap bitmap;
  CBitmap* pOldBitmap;
	CRect rectClient; 
  
  pWnd->GetClientRect(&rectClient);
  memDC.CreateCompatibleDC(pDC);

  bitmap.CreateCompatibleBitmap(pDC, rectClient.right+2, rectClient.bottom+2);
 	pOldBitmap=memDC.SelectObject(&bitmap);
	//memDC.FillSolidRect(0,0,rectClient.right+2,rectClient.bottom+2,RGB(255,255,255));

	int nLineType=LINE_TYPE_SOLID;
	int nLineType1=LINE_TYPE_SOLID;
	long lMinLength = m_lFileLength + 10 * 40;
	long lMaxLength;
	CPoint pt,pt1,pt2;
	int nSize;
	POSITION pos=theApp.m_listPrimitive.GetHeadPosition();
	CPen pen(nLineType1,1,RGB(0,0,0));
	CPen *pOldPen=memDC.SelectObject(&pen);
	memDC.Rectangle(0,0,rectClient.Width()+2,rectClient.Height()+2);
	while(pos)
	{
		CGLPrimitive *p=(CGLPrimitive*)theApp.m_listPrimitive.GetNext(pos);
		switch(p->m_nType)
		{
		case PRIMITIVE_LINE:
			nSize=p->m_anData.GetSize()/2;
			if(nSize)
			{
				if(theApp.m_nRotate==1)
				{
					pt.y=p->m_anData.GetAt(0)-m_lFileMinLength;
					pt.x=m_lFileLength-(p->m_anData.GetAt(1)-m_lFileMinWidth);
				}
				else
				{
					pt.x=p->m_anData.GetAt(0)-m_lFileMinLength;
					pt.y=p->m_anData.GetAt(1)-m_lFileMinWidth;
				}
				for(int i=1;i<nSize;i++)
				{
					if(theApp.m_nRotate==1)
					{
						pt1.y=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt1.x=m_lFileLength-(p->m_anData.GetAt(i*2+1)-m_lFileMinWidth);
					}
					else
					{
						pt1.x=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt1.y=p->m_anData.GetAt(i*2+1)-m_lFileMinWidth;
					}
					if(pt1.y<pt.y)
					{
						pt2=pt;
						pt=pt1;
						pt1=pt2;
					}
					if(pt.y>m_lTotalY)
					{
						pt.x+=((pt.y/m_lTotalY)*lMinLength);
						pt1.x+=((pt.y/m_lTotalY)*lMinLength);
						pt1.y-=((pt.y/m_lTotalY)*m_lTotalY);
						pt.y=pt.y%m_lTotalY;
					}
					while(pt1.y>m_lTotalY)
					{
						pt2.y=m_lTotalY;
						pt2.x=(pt2.y-pt.y)*(pt1.x-pt.x)/(pt1.y-pt.y)+pt.x;
						pt.y=m_lTotalY-pt.y;
						DocToClient(pt);
						memDC.MoveTo(pt);
						pt.x=pt2.x;
						pt.y=m_lTotalY-pt2.y;
						DocToClient(pt);
						memDC.LineTo(pt);
						pt.y=0;
						pt.x=pt2.x+lMinLength;
						pt1.y-=m_lTotalY;
						pt1.x+=lMinLength;
					}
					pt.y=m_lTotalY-pt.y;
					DocToClient(pt);
					memDC.MoveTo(pt);
					pt1.y=m_lTotalY-pt1.y;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					if(theApp.m_nRotate==1)
					{
						pt.y=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt.x=m_lFileLength-(p->m_anData.GetAt(i*2+1)-m_lFileMinWidth);
					}
					else
					{
						pt.x=p->m_anData.GetAt(i*2)-m_lFileMinLength;
						pt.y=p->m_anData.GetAt(i*2+1)-m_lFileMinWidth;
					}
				}

				//pt.x=p->m_anData.GetAt(0);
				//pt.y=m_lTotalY-p->m_anData.GetAt(1);
				//DocToClient(pt);
				//memDC.MoveTo(pt);
				
				//for(int i=1;i<nSize;i++)
				//{
					//pt.x=p->m_anData.GetAt(i*2);
					//pt.y=m_lTotalY-p->m_anData.GetAt(i*2+1);
					//DocToClient(pt);
					//memDC.LineTo(pt);
				//}
			}
			break;
		case PRIMITIVE_LT:
			nSize=p->m_anData.GetSize();
			if(nSize==0)
			{
				nLineType=nLineType1;
				nLineType1=LINE_TYPE_SOLID;
				pen.DeleteObject();
				pen.CreatePen(nLineType1,1,RGB(0,0,0));
				memDC.SelectObject(&pen);
			}
			else
			{
				nSize=p->m_anData.GetAt(0);
				if(nSize==99)
				{
					if(nLineType1==LINE_TYPE_SOLID)
					{
						nLineType1=nLineType;
						pen.DeleteObject();
						pen.CreatePen(nLineType1,1,RGB(0,0,0));
						memDC.SelectObject(&pen);
					}
				}
				else if((nSize>=-8)&&(nSize<=8))
				{
					nLineType1=abs(nSize);
					if(nLineType1<=1)
						nLineType1=LINE_TYPE_DOT;
					else if(nLineType1<=3)
						nLineType1=LINE_TYPE_DASH;
					else
						nLineType1=LINE_TYPE_DASHDOT;
					pen.DeleteObject();
					pen.CreatePen(nLineType1,1,RGB(0,0,0));
					memDC.SelectObject(&pen);
				}
			}
			break;
		}
	}
	if (lMinLength < m_lTotalX)
	{
		pen.DeleteObject();
		pen.CreatePen(LINE_TYPE_DOT,1,RGB(0,0,0));
		memDC.SelectObject(&pen);
		lMinLength = 0;
		lMaxLength = m_lFileLength;
		if (theApp.m_dMarkSpace < 50)
			theApp.m_dMarkSpace = 50;
		while (lMinLength < m_lTotalX)
		{
			if (lMaxLength < m_lTotalX)
			{
				pt.x = lMinLength + 20 * 40;
				pt.y = 0;
				while (pt.x < lMaxLength)
				{
					pt1 = pt;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 10 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt.x += long(theApp.m_dMarkSpace * 40);
				}
				pt1.y = 0;
				pt1.x = lMaxLength + 5 * 40;
				DocToClient(pt1);
				memDC.MoveTo(pt1);
				pt1.y = m_lTotalY;
				pt1.x = lMaxLength + 5 * 40;
				DocToClient(pt1);
				memDC.LineTo(pt1);
			}
			if (lMinLength > m_lFileLength)
			{
				pt.x = lMinLength + 20 * 40;
				pt.y = m_lTotalY;
				while (pt.x < lMaxLength)
				{
					pt1 = pt;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x;
					DocToClient(pt1);
					memDC.MoveTo(pt1);
					pt1.y = pt.y;
					pt1.x = pt.x - 10 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt1.y = pt.y - 10 * 40;
					pt1.x = pt.x - 20 * 40;
					DocToClient(pt1);
					memDC.LineTo(pt1);
					pt.x += long(theApp.m_dMarkSpace * 40);
				}
			}
			lMinLength += (m_lFileLength + 10 * 40);
			lMaxLength += (m_lFileLength + 10 * 40);
		}
	}
	memDC.SelectObject(pOldPen);
	pen.DeleteObject();

	if (theApp.m_nFont==0)
	{
		memDC.SetBkMode(TRANSPARENT);
		pos=theApp.m_listTextPrimitive.GetHeadPosition();
		double dTextAngle=0;
		double dTextHeight=0.375;
		double dTextWidth=0.285;
		lMinLength = m_lFileLength + 10 * 40;
		if(theApp.m_nRotate==1)
			dTextAngle+=90;
		while(pos)
		{
			CGLText *pText=(CGLText*)theApp.m_listTextPrimitive.GetNext(pos);
			if(pText->m_nType==PRIMITIVE_LB)
			{
				if(dTextHeight==0)
					break;
				CFont font;
				if(!font.CreateStockObject(DEFAULT_GUI_FONT)) 
					VERIFY(font.CreateStockObject(SYSTEM_FONT));
				CFont *pOldFont=memDC.SelectObject(&font);
				LOGFONT lf;
				pOldFont->GetLogFont(&lf);
				strcpy(lf.lfFaceName,"HP Roman 8");//
				lf.lfHeight=ftoi(dTextHeight*400*m_dProportion);
				if(abs(lf.lfHeight)<1)
					lf.lfHeight=1;
				lf.lfWidth=ftoi(dTextWidth*400*m_dProportion);
				if(abs(lf.lfWidth)<1)
					lf.lfWidth=1;
				lf.lfWeight=FW_THIN;
				lf.lfOrientation=ftoi(dTextAngle*10);
				lf.lfEscapement=ftoi(dTextAngle*10);
				CFont fontNew;
				BOOL b=fontNew.CreateFontIndirect(&lf);
				memDC.SelectObject(&fontNew);
				
				CPoint pt;
				if(theApp.m_nRotate==1)
				{
					pt.y=ftoi(pText->m_adPara.GetAt(0))-m_lFileMinLength;
					pt.x=m_lFileLength-(ftoi(pText->m_adPara.GetAt(1))-m_lFileMinWidth);
				}
				else
				{
					pt.x=ftoi(pText->m_adPara.GetAt(0))-m_lFileMinLength;
					pt.y=ftoi(pText->m_adPara.GetAt(1))-m_lFileMinWidth;
				}
				if(pt.y>m_lTotalY)
				{
					pt.x+=((pt.y/m_lTotalY)*lMinLength);
					pt.y=m_lTotalY-pt.y%m_lTotalY;
				}
				else
					pt.y=m_lTotalY-pt.y;
				DocToClient(pt);
				nSize=pText->m_strText.GetLength();
				if(nSize>256)
					nSize=256;
				//20150910
				memDC.SetTextAlign(TA_LEFT | TA_BASELINE);//文本对齐方式
				memDC.TextOut(pt.x,pt.y,pText->m_strText);
				memDC.SelectObject(pOldFont);
				font.DeleteObject();
				fontNew.DeleteObject();
			}
			else if(pText->m_nType==PRIMITIVE_SI)
			{
				nSize=pText->m_adPara.GetSize();
				if(nSize>=2)
				{
					dTextHeight=pText->m_adPara.GetAt(1);
					dTextWidth=pText->m_adPara.GetAt(0);
				}
				else 
				{
					dTextHeight=0.375;
					dTextWidth=0.285;
				}
			}
			else if(pText->m_nType==PRIMITIVE_DI)
			{
				double d1;
				double d2;
				nSize=pText->m_adPara.GetSize();
				if(nSize>=2)
				{
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);

					if(d1<0.10 && d1>-0.10)
						d1=0;
					if(d2<0.10 && d2>-0.10)
						d2=0;

					if(d1==0 && d2==1)
						dTextAngle=90;
					else if(d1==1 && d2==0)
						dTextAngle=0;
					else if(d1==0 && d2==-1)
						dTextAngle=270;
					else if(d1==-1 && d2==0)
						dTextAngle=-180;
					else
						dTextAngle=atan2(d2,d1)*180/3.14159265359;
					double tmp = (dTextAngle - (int)dTextAngle);
					if(abs((int)(tmp*10)) >= 5)
					{
						dTextAngle = dTextAngle < 0 ? dTextAngle += -1 : dTextAngle+=1;
					}
					dTextAngle = (int)dTextAngle;
				}
				else
					dTextAngle=0;
				if(theApp.m_nRotate==1)
					dTextAngle+=90;
			}
			else if(pText->m_nType==PRIMITIVE_DR)
			{
				double d1;
				double d2;
				nSize=pText->m_adPara.GetSize();
				if(nSize==0)
					dTextAngle=0;
				else if(nSize==2)
				{
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);
					if((d1==0)&&(d2!=0))
						dTextAngle=90;
					else if((d1!=0)&&(d2==0))
						dTextAngle=0;
					else
						continue;
				}
				if(nSize==4)
				{
					d1=pText->m_adPara.GetAt(0);
					d2=pText->m_adPara.GetAt(1);
					if((d1==0)&&(d2!=0))
						dTextAngle=90;
					else if((d1!=0)&&(d2==0))
						dTextAngle=0;
					else if((d1!=0)&&(d2!=0)&&(fabs(d1)<=100)&&(fabs(d2)<=100))
					{
						d1=d1*pText->m_adPara.GetAt(2);
						d2=d2*pText->m_adPara.GetAt(3);
						dTextAngle=atan2(d1,d2);
					}
					else
						continue;
				}
				if(theApp.m_nRotate==1)
					dTextAngle+=90;
			}
		}
	}
	pDC->BitBlt(0, 0, rectClient.right+2, rectClient.bottom+2, &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();

}
	*/
}

void CPictureDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWnd *pWnd = GetDlgItem(IDC_DRAWPICTURE);   
	CRect rtDrawPicture;
	pWnd->GetWindowRect(&rtDrawPicture);
	ScreenToClient(rtDrawPicture);
	if ((point.x >= rtDrawPicture.left) && (point.x <= rtDrawPicture.right) &&
		(point.y >= rtDrawPicture.top) && (point.y <= rtDrawPicture.bottom))
	{
		m_bLButtonDown = true;
		m_ptMouseDownPos.x = point.x - rtDrawPicture.left;
		m_ptMouseDownPos.y = point.y - rtDrawPicture.top;
		m_ptMouseUpPos = m_ptMouseDownPos;
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CPictureDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bLButtonDown)
	{
		CWnd *pWnd = GetDlgItem(IDC_DRAWPICTURE);   
		CRect rtDrawPicture;
		long lPointX,lPointY;
		pWnd->GetWindowRect(&rtDrawPicture);
		ScreenToClient(rtDrawPicture);
		if ((point.x >= rtDrawPicture.left) && (point.x <= rtDrawPicture.right) &&
			(point.y >= rtDrawPicture.top) && (point.y <= rtDrawPicture.bottom))
		{
			point.x = point.x - rtDrawPicture.left;
			point.y = point.y - rtDrawPicture.top;
			if (m_ptStartPos.x > 0)
				lPointX = (long)((point.x - m_ptStartPos.x) / m_dProportion);
			else
				lPointX = (long)((point.x + fabs(m_ptStartPos.x)) / m_dProportion);
			if (m_ptStartPos.y > 0)
				lPointY = (long)((rtDrawPicture.Height() - m_ptStartPos.y - point.y) / m_dProportion);
			else
				lPointY = m_lTotalY - (long)((point.y + fabs(m_ptStartPos.y)) / m_dProportion);
			if ((lPointX < m_lTotalX) || (lPointY < m_lTotalY))
			{
				if (theApp.m_nMeasureUnit== 0)
				{
				m_strTotalX.Format("%ldmm",ftoi(lPointX*0.025));
				m_strTotalY.Format("%ldmm",ftoi(lPointY*0.025));
				}
				else if(theApp.m_nMeasureUnit== 1)
				{
					m_strTotalX.Format("%ld inch",ftoi(lPointX*0.025/25.4));
					m_strTotalY.Format("%ld inch",ftoi(lPointY*0.025/25.4));
				}
			}
			UpdateData(false);
		}
		if ((m_ptMouseUpPos.x != m_ptMouseDownPos.x) && (m_ptMouseUpPos.y != m_ptMouseDownPos.y))
			DrawPicture(1);//放大图形
		m_bLButtonDown = false;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CPictureDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	DrawPicture(0);//图形复原

	CDialog::OnRButtonUp(nFlags, point);
}

void CPictureDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWnd *pWnd = GetDlgItem(IDC_DRAWPICTURE);
  long lPointX,lPointY;
	CRect rtDrawPicture;
	pWnd->GetWindowRect(&rtDrawPicture);
	ScreenToClient(rtDrawPicture);
	if (point.x < rtDrawPicture.left)
		point.x = rtDrawPicture.left;
	else if (point.x > rtDrawPicture.right)
		point.x = rtDrawPicture.right;
	if (point.y < rtDrawPicture.top)
		point.y = rtDrawPicture.top;
	else if (point.y > rtDrawPicture.bottom)
		point.y = rtDrawPicture.bottom;
	point.x = point.x - rtDrawPicture.left;
	point.y = point.y - rtDrawPicture.top;
	if (m_bLButtonDown)
	{
		CPen pen;
		CPen *pOldPen;
		CDC *pDC;
		
		pDC = pWnd->GetWindowDC(); 
    pen.CreatePen(LINE_TYPE_DOT, 1, RGB(0,0,0));
    pOldPen = pDC->SelectObject(&pen);   
    pDC->SetROP2(R2_XORPEN);   
    pDC->MoveTo(m_ptMouseDownPos);
    pDC->LineTo(m_ptMouseUpPos.x, m_ptMouseDownPos.y);
    pDC->LineTo(m_ptMouseUpPos);
    pDC->LineTo(m_ptMouseDownPos.x, m_ptMouseUpPos.y);
    pDC->LineTo(m_ptMouseDownPos);

		m_ptMouseUpPos.x = point.x;
		m_ptMouseUpPos.y = point.y;
		if (m_ptMouseUpPos.x == m_ptMouseDownPos.x)
			m_ptMouseUpPos.x++;
		if (m_ptMouseUpPos.y == m_ptMouseDownPos.y)
			m_ptMouseUpPos.y++;
		pDC->MoveTo(m_ptMouseDownPos);
    pDC->LineTo(m_ptMouseUpPos.x, m_ptMouseDownPos.y);
    pDC->LineTo(m_ptMouseUpPos);
    pDC->LineTo(m_ptMouseDownPos.x, m_ptMouseUpPos.y);
    pDC->LineTo(m_ptMouseDownPos);
   
    pDC->SelectObject(pOldPen);
    pen.DeleteObject();
    ReleaseDC(pDC);  
	}
	else
	{
		if (m_ptStartPos.x > 0)
			lPointX = (long)((point.x - m_ptStartPos.x) / m_dProportion);
		else
			lPointX = (long)((point.x + fabs(m_ptStartPos.x)) / m_dProportion);
		if (m_ptStartPos.y > 0)
			lPointY = m_lTotalY - (long)((point.y - m_ptStartPos.y) / m_dProportion);
		else
			lPointY = m_lTotalY - (long)((point.y + fabs(m_ptStartPos.y)) / m_dProportion);
	
		if ((lPointX < m_lTotalX) || (lPointY < m_lTotalY))
		{
			if (theApp.m_nMeasureUnit == 0)
				{
				m_strTotalX.Format("%ldmm",ftoi(lPointX*0.025));
				m_strTotalY.Format("%ldmm",ftoi(lPointY*0.025));
				}
				else if(theApp.m_nMeasureUnit == 1)
				{
					m_strTotalX.Format("%ld inch",ftoi(lPointX*0.025/25.4));
					m_strTotalY.Format("%ld inch",ftoi(lPointY*0.025/25.4));
				}
		}
		UpdateData(false);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CPictureDlg::OnChangeFixedPoint() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(true);
}

void CPictureDlg::OnChangeCopyNum() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(true);
}
