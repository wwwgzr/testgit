// PictureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "PlotCenterDlg.h"
#include "PictureDlg.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 表示可改变大小的图标ID
#ifndef OBM_SIZE
#define OBM_SIZE 32766
#endif
extern BOOL bIsEncrypt;
extern unsigned char g_cPenNo; // 20101206 笔号刀号切割指令识别

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
	//}}AFX_DATA_INIT	
	m_multiple = 0.01;//20160121切割指令识别
    m_mulConst = 0;
	m_ofstX = 0;
	m_ofstY = 0;
	picvar = 0;


}


void CPictureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPictureDlg)
	DDX_Text(pDX, IDC_TOTALX, m_strTotalX);
	DDX_Text(pDX, IDC_TOTALY, m_strTotalY);
	DDX_Text(pDX, IDC_FIXEDPOINT, m_lFixedPointLength);
	DDX_Text(pDX, IDC_COPYNUM, m_nCopyNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPictureDlg, CDialog)
	//{{AFX_MSG_MAP(CPictureDlg)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_DRAW, OnDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_EN_CHANGE(IDC_FIXEDPOINT, OnChangeFixedPoint)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_COPYNUM, OnChangeCopyNum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPictureDlg message handlers

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
	m_nCopyNum = 1;
	m_lFixedPointLength = 0;
	m_strTotalX.Format("%ldmm",ftoi(m_lTotalX*0.025));
	m_strTotalY.Format("%ldmm",ftoi(m_lTotalY*0.025));
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
	strItem1.Format("%s(%ldmm * %ldmm)",m_strPictureName,ftoi(m_lTotalX*0.025),ftoi(m_lTotalY*0.025));
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
	}
	
	pWnd = GetDlgItem(IDC_FIXEDPOINT);	// 起始点
	pWnd->EnableWindow(false);	
	
	pWnd = GetDlgItem(IDC_COPYNUM);	// 份数
	pWnd->EnableWindow(false);	

	UpdateData(false);

	ShowWindow(SW_SHOWMAXIMIZED);//20160114初始化窗口最大化切割指令识别

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
	m_plotContext.DeleteAll();//20160114切割指令识别
}

/*! 
*  @brief 判断当前鼠标单击点是否选中线段 
* 
*  @param [in]p  当前鼠标单击点坐标 
*  @param [in]p1 直线的一个端点的坐标 
*  @param [in]p2 直线的另一个端点的坐标 
*  @param [in]dbLimit 选取的阈值 
*  @return 是否选中。TRUE为选中，FALSE表示没有选中。 
*/  
BOOL IsSelLine(CPoint p,CPoint p1,CPoint p2,double dbLimit)  
{  
	/* 判断当前单击点是否在直线的*/  
	int nRadius = static_cast<int>(dbLimit);  
	int l = (p1.x<=p2.x?p1.x:p2.x);  
	int t = (p1.y<=p2.y?p1.y:p2.y);  
	int r = (p1.x>p2.x?p1.x:p2.x);  
	int b = (p1.y>p2.y?p1.y:p2.y);  
	CRect rt(l-nRadius,t-nRadius,r+nRadius,b+nRadius);  
	if(!rt.PtInRect(p))  
		return FALSE;  
	//坐标变换，即把选段平移到原点为(p2.x,p2.y)的坐标系上  
	p1.x-=p2.x;  
	p1.y-=p2.y;  
	p.x-=p2.x;  
	p.y-=p2.y;  
	p2.x=0;  
	p2.y=0;  
	//计算点到线段的距离  
	double d=fabs(static_cast<double>(p1.y*p.x-p1.x*p.y))/sqrt(static_cast<double>(p1.y*p1.y+p1.x*p1.x));  
	if (d<dbLimit)  
	{  
		return TRUE;  
	}   
	return FALSE;  
}

bool	CPictureDlg::ChangePen(CPoint & point, BYTE cuepen)//20160120切割指令识别
{

	CRect	rect;
	GetClientRect(rect);

	if (!rect.PtInRect(point))
		return false;

	//if (m_penAry.GetPenCount() <= cuepen)//20160127更改过
		//return false;//20160127更改过

	CShapeSet * pShapeSet = m_plotContext.GetShapeSet();

	CPolyLine * pPolyLineS;
	CShape *	pShapeS;
	CMyPoint	*ptMBeg, *ptMEnd;
	CPoint		ptBeg, ptEnd;
	int	i, j;
	int	m = pShapeSet->GetShapeCount();

	long	minx, miny, maxx, maxy;
	int n;
	bool isFind = false;

	//gzr20160914 选取存储变量 start
	//图形中和鼠标点最短距离
	double mind = 6;
	//图形中和鼠标点最短线段索引
	double minIndex = -1;
	//gzr20160914 选取存储变量 end

	for (j = 0; j < m; j++)
	{
		pShapeS = pShapeSet->GetAShape(j);
		if (pShapeS->m_type != SP_POLYLINE)
			continue;
		pPolyLineS = (CPolyLine *)pShapeS;

		n = pPolyLineS->GetPointCount();
		if (n < 2)
			continue;
		ptMEnd = pPolyLineS->GetPoint(0);
		for (i = 1; i < n; i++)					//搜索线段, 看点是否在某根线上
		{
			ptMBeg = ptMEnd;
			ptMEnd = pPolyLineS->GetPoint(i);
			ptBeg = LogPointToScr(ptMBeg);
			ptEnd = LogPointToScr(ptMEnd);
			

			minx = (ptBeg.x < ptEnd.x) ? ptBeg.x : ptEnd.x;
			maxx = (ptBeg.x >= ptEnd.x) ? ptBeg.x : ptEnd.x;
			miny = (ptBeg.y < ptEnd.y) ? ptBeg.y : ptEnd.y;
			maxy = (ptBeg.y >= ptEnd.y) ? ptBeg.y : ptEnd.y;

			if(nLarge1==0)
			{
				if (point.x <= maxx + 5 + 7 //20160201切割指令识别加了7
					&& point.x >= minx - 5 + 7//20160201切割指令识别加了7
					&& point.y <= maxy + 5 + 7 //20160201切割指令识别加了7
					&& point.y >= miny - 5 + 7)//20160201切割指令识别加了7
				{
					if (minx == maxx || miny == maxy)		//
						break;

					double	k = (ptEnd.y - ptBeg.y) * (point.x- ptBeg.x) + (ptBeg.y - point.y) * (ptEnd.x - ptBeg.x);
					k = k * k;
					double	d = (ptEnd.x - ptBeg.x);
					d = d * d * d * d;
					d += (ptEnd.y - ptBeg.y) * (ptEnd.y - ptBeg.y);
					d = k / d;

					//gzr20160914 start
					//将负数距离转成正数
					if (d < 0)
					{
						d = -d;
					}
					if (d < 5 && d > -5)		// 点到直线的距离小于5
					{
						// pPolyLineS->m_penCode = cuepen;
						isFind = true;
						//将距离最短的索引距离存储
						if (mind > d)
						{
							mind = d;
							minIndex = j;
						}
						continue;
					}
					//gzr20160914  end
				}
			}
			else if(nLarge1==1)
			{
				 if (point.x <= maxx + 5 + 7 //20160201切割指令识别加了7
					&& point.x >= minx - 5 + 7
					&& point.y <= maxy + 5 + 7 
					&& point.y >= miny - 5 + 7)
				{
					if (minx == maxx || miny == maxy)		//
						break;

					double	k = (ptEnd.y - ptBeg.y) * (point.x- ptBeg.x) + (ptBeg.y - point.y) * (ptEnd.x - ptBeg.x);
					k = k * k;
					double	d = (ptEnd.x - ptBeg.x);
					d = d * d * d * d;
					d += (ptEnd.y - ptBeg.y) * (ptEnd.y - ptBeg.y);
					d = k / d;
 					if (d < 5 && d > 0 || d < 0 && d > -5)		// 点到直线的距离小于5
 					{
 						break;
 					}
				}
			}
		}
}
//gzr20160914 start
//判断是否有线段被选中
if (!isFind)
{
	return false;
}
else
{
	//根据最短距离索引 获取线段 设置线段绘制的画笔
	pShapeS = pShapeSet->GetAShape((int)minIndex);
	if (pShapeS->m_type == SP_POLYLINE)
	{
		pPolyLineS = (CPolyLine *)pShapeS;
		pPolyLineS->m_penCode = cuepen;
	}
}
//gzr20160914 end

// 		if (i == n)				// 点不是在该线组中的线上
// 			continue;

		// 将同一组的线的属性更改

//		BYTE	penCode = ((CMyPen *)(m_penAry.GetPen(cuepen)))->m_penNum;

		/*pPolyLineS->m_penCode = cuepen;*/

//		Invalidate();

		pShapeSet = m_plotContext.GetShapeSet();//20160201切割指令识别


//20160201切割指令识别
	CString		m_hpgl;
	double		m_unit;
	long m_length;

	m_length = 350;
	m_unit = 0.025;

		// 得到输出内容
	double	page = m_length;
	page /= m_unit;
	// 计算每一个分页的逻辑单位
	//m_pDoc->Pagination((long)(page+0.5), m_hpgl);
	// 分页输出, 页大小 width length 是逻辑单位

	pShapeSet = new CShapeSet;

    long length=(long)(page+0.5);

	pShapeSet->CreateSimilarSet(m_plotContext.GetShapeSet());
	CString hpgl;

	hpgl.Empty();

	hpgl += "IN;DF;PA;PU;SP1;";
	//gzr20161025 start
	//plt IP命令输出
	if (m_bIPValid)
	{
		CString strip;
		strip.Format("IP%d,%d,%d,%d;",m_dpP1.x,m_dpP1.y,m_dpP2.x,m_dpP2.y);
		hpgl += strip;
	}
	//plt SC命令输出
	if (m_bSCValid)
	{
		CString strsc;
		strsc.Format("SC%lf,%lf,%lf,%lf;",m_struSC.dXMin,m_struSC.dXMax,m_struSC.dYMin,m_struSC.dYMax);
		hpgl += strsc;
	}
	//gzr20161025 end

	CMyPoint ptThis, ptLast, *pptThis, *pptLast;

	CShape	* pShape;
	CPolyLine * pPolyLine;
	CLabel	*	pLabel;
	CString	str;

	bool	first = true;
	bool	firstline;
	CSize		m_charSize;
	int			m_escapement;
	Arrange		m_arrange;
	double	x, y;

	long	result;

	bool	bNewShape;
	BYTE	curpen = 1;
	bool	changePage = false;//gzr20160913 判断分页

	//int i, n, j, m;
	do
	{
		n = pShapeSet->GetShapeCount();
		for (i = 0; i < n; )
		{
			pShape = pShapeSet->GetAShape(i);

			if (pShape->m_type == SP_POLYLINE)
			{
				pPolyLine = (CPolyLine *)pShape;
				m = pPolyLine->GetPointCount();

				if (m < 2)								// 如果折线中的点的个数小于2，则删除该折线
				{
					if (m == 1)
					{
						delete	pPolyLine->GetPoint(0);
						pPolyLine->RemoveAll();
					}
					pShapeSet->RemoveShape(i);
					delete	pPolyLine;
					n = pShapeSet->GetShapeCount();
					continue;
				}

				if (curpen != pPolyLine->m_penCode)		// 是否换笔
				{
					if(pPolyLine->m_penCode==0)//20160127修改过
					{
					str.Empty();
					str.Format("PU;SP%d;", 1);//20160127修改过
					hpgl += str;
					curpen = pPolyLine->m_penCode;
					}
					else
					{
	                str.Empty();
					str.Format("PU;SP%d;", pShape->m_penCode);//20160127修改过
					hpgl += str;
					curpen = pPolyLine->m_penCode;
					}

				}
				pptLast = pptThis = NULL;

				bNewShape = false;
				firstline = true;
				for (j = 1; j < m;)
				{
					pptLast = pPolyLine->GetPoint(j - 1);
					pptThis = pPolyLine->GetPoint(j);
					if (pptLast->x == pptThis->x && pptLast->x == length)
					{
						pptLast->x -= length;
						j++;
					}
					else if (pptLast->x <= length && pptThis->x <= length)
					{
						str.Empty();
						if (firstline)
						{
							firstline = false;

							str.Format("PU%d,%d;PD%d,%d;", pptLast->x, pptLast->y, pptThis->x, pptThis->y);
						}
						else
						{

							str.Format("PD%d,%d;", pptThis->x, pptThis->y);
						}
						hpgl += str;
						//gzr20160913 判断是否有分页操作 start
						
						if (j != 1 && changePage)
						{
							//有分页将上一点保留到下一页
							pptLast->x -= length;
							j++;
						}
						else
						{
							//无分页将上一点删除
							pPolyLine->RemovePoint(pptLast);
							delete	pptLast;
						}
						m = pPolyLine->GetPointCount();
						//判断有无分页和当前线段已循环完
						if (!changePage && j == m)
						{
							//无分页并且当前线段已循环完，删除当前点
							pPolyLine->RemovePoint(pptThis);
							delete	pptThis;
						}
						//gzr20160913 判断分页
						m = pPolyLine->GetPointCount();
						
						changePage = false;
						
// 						pPolyLine->RemovePoint(pptLast);
// 						delete	pptLast;
// 						m = pPolyLine->GetPointCount();
// 						continue;
						//gzr20160913 判断是否有分页操作 end
					}
					else if (pptLast->x < length && pptThis->x > length)
					{
						GetOnLinePt(pptLast, pptThis, length, result);
						str.Empty();
						if (firstline)
						{
							firstline = false;

							str.Format("PU%d,%d;PD%d,%d;", pptLast->x, pptLast->y, length, result);
						}
						else
						{
							str.Format("PD%d,%d;", length, result);
						}
						hpgl += str;
						firstline = true;
						pptLast->x = length;
						pptLast->y = result;

					}
					else if (pptLast->x > length && pptThis->x < length)
					{
						GetOnLinePt(pptLast, pptThis, length, result);

						// 分割
						CMyPoint * tppit = new CMyPoint;	// 新的this
						tppit->x = pptThis->x;
						tppit->y = pptThis->y;				// 保存 this

						pptThis->x = length;
						pptThis->y = result;				// this 后移

						CMyPoint * tppit1 = new CMyPoint;	// 新的last
						tppit1->x = pptThis->x;
						tppit1->y = pptThis->y;				// 后移后的this

						pptThis->x -= length;
						pptLast->x -= length;				// 将线段前移移, 为下一页准备

						CPolyLine * tppline = new CPolyLine;
						tppline->m_penCode = pPolyLine->m_penCode;
						tppline->AddPoint(tppit1);			
						tppline->AddPoint(tppit);			// 后移后的point作为开始
						j++;
						while (j < m)		// 将折线后续的点移动到新的折线中
						{
							tppit = pPolyLine->GetPoint(j);
							tppline->AddPoint(tppit);
							pPolyLine->RemovePoint(j);
							m = pPolyLine->GetPointCount();
						}
						i++;
						pShapeSet->InsertShape(i, tppline);
						n = pShapeSet->GetShapeCount();
						bNewShape = true;
					}
					else if (pptLast->x >= length && pptThis->x >= length)
					{
						pptLast->x -= length;
						j++;
						if (j >= m)
							pptThis->x -= length;
						//gzr20160913 判断分页
						changePage = true;
					}
					else
					{
						i = i;
					}
				}
				if (bNewShape)
				{
					continue;
				}
				if (m < 2)
				{

					if (m == 1)
					{
						delete	pPolyLine->GetPoint(0);
						pPolyLine->RemoveAll();
					}
					pShapeSet->RemoveShape(i);
					delete	pPolyLine;
					n = pShapeSet->GetShapeCount();
					continue;
				}
			}

			else if (pShape->m_type == SP_LABEL)
			{
				pLabel = (CLabel *)pShape;

				//if (curpen != pLabel->m_penCode && pLabel->m_penCode!=0)		// 是否换笔//20160418更改过HHY注释掉了，只要是字就换为SP1
				{
					str.Empty();
					//str.Format("PU;SP%d;", pShape->m_penCode);
					str.Format("PU;SP%d;", 1);
					hpgl += str;
					//curpen = pLabel->m_penCode;
					curpen = 1;
				}

				pptLast = pptThis = NULL;

				if (pLabel->m_startPoint.x < length)
				{
					if (first)
					{
						first = false;
						m_escapement = pLabel->m_escapement;
						m_charSize = pLabel->m_chSizer;
						m_arrange = pLabel->m_arrange;
/*
						if (!pLabel->m_cmdDI.IsEmpty())
						{
							str.Format("%s;", pLabel->m_cmdDI);
							hpgl += str;
						}

						if (!pLabel->m_cmdSI.IsEmpty())
						{
							str.Format("%s;", pLabel->m_cmdSI);
							hpgl += str;
						}

*/

						GetDirectionX(pLabel->m_escapement, x, y);
						str.Format("DI%1.4f,%1.4f;", x, y);
						hpgl += str;

						x = pLabel->m_chSizer.cx * 0.025 / 10;
						y = pLabel->m_chSizer.cy * 0.025 / 10;
						str.Format("SI%1.4f,%1.4f;", x, y);
						hpgl += str;

						str.Format("LO%d;", pLabel->m_arrange);
						hpgl += str;

						str.Format("PU%d,%d;", pLabel->m_startPoint.x, pLabel->m_startPoint.y);
						hpgl += str;
						str.Empty();

						str.Format("LB%s%c;", pLabel->m_content, 0x03);
						hpgl += str;
						str.Empty();
					}
					else
					{
						if (m_escapement != pLabel->m_escapement)
						{
							GetDirectionX(pLabel->m_escapement, x, y);
							str.Format("DI%1.4f,%1.4f;", x, y);
							hpgl += str;
						}
						if (m_charSize != pLabel->m_chSizer)
						{
							x = pLabel->m_chSizer.cx * 0.025 / 10;
							y = pLabel->m_chSizer.cy * 0.025 / 10;
							str.Format("SI%1.4f,%1.4f;", x, y);
							hpgl += str;
						}
/*
						if (!pLabel->m_cmdDI.IsEmpty())
						{
							str.Format("%s;", pLabel->m_cmdDI);
							hpgl += str;
						}
*/

						if (m_arrange != pLabel->m_arrange)
						{
							str.Format("LO%d;", pLabel->m_arrange);
							hpgl += str;
						}

						str.Format("PU%d,%d;", pLabel->m_startPoint.x, pLabel->m_startPoint.y);
						hpgl += str;
						str.Empty();

						str.Format("LB%s%c;", pLabel->m_content, 0x03);
						hpgl += str;
						str.Empty();
					}
					pShapeSet->RemoveShape(i);
					n = pShapeSet->GetShapeCount();
					continue;
				}
				else
				{
					pLabel->m_startPoint.x -= length;
				}
			}
			else
			{
				// 其他图形
			}
			i++;
		}

		hpgl += "PU;SP1;PG;";
		curpen = 1;
		if (n <= 0)							// 完成
			break;

	}while (true);



	pShapeSet->DeleteAll();





// 保存文件
	char *	temp = new char[MAX_PATH + 1];
	if (temp == NULL)
		return false ;
	ZeroMemory(temp, MAX_PATH);
	GetModuleFileName(AfxGetInstanceHandle(), temp, MAX_PATH);
		i;
	for(i = MAX_PATH; i >= 0;  i--)
	{
		if (temp[i]=='\\')
		{
			temp[i] = '\0';
			break;
		}
	}



			//将旧文件删除，获得旧文件命
		CString oldHBname;
		oldHBname.Empty();
		char *	pBuf1 = new	char[64*1024 + 1];
	    if (pBuf1 == NULL)
		return false;
	    TRY
		{
		CFile file2("C:\\LIQIECUT.txt", CFile::modeRead | CFile::typeBinary);
		UINT	n1 = file2.GetLength();
		UINT	i3, j1;
		i3 = 0;
		j1 = n1;
		file2.SeekToBegin();

		while(i3 < n1 && j1 >0)
		{
			file2.Read(pBuf1, j1);
			pBuf1[j1] = '\0';
			oldHBname += pBuf1;
			i3 += j1;
			j1 = n1;
		};
		file2.Close();					// 关闭文件
		}
	    CATCH( CFileException, e )
		{
		CFile file3("C:\\LIQIECUT.txt", CFile::modeCreate | CFile::modeWrite);
		file3.Write("", 0);
		file3.Close();
		}
    	END_CATCH
	    delete	[]pBuf1;

	CString	strFileName;
	//CString	name;
	strFileName.Format("%s\\cut\\"+strFileName1, temp);
	//m_pDoc->SaveHPGLFile(name, m_hpgl);



		char oldfilename[100];
        strncpy(oldfilename,(LPCTSTR)oldHBname,sizeof(oldfilename));
        //int bbb=rename(oldfilename,newfilename);//为零的话重命名成功
		if(oldHBname!=strFileName)
		{
		remove(oldfilename);//为零的话删除文件成功
		}



	 int n1=strFileName.GetLength();
	 CFile file1("C:\\LIQIECUT.txt", CFile::modeCreate | CFile::modeWrite);
	 file1.Write(strFileName, n1);
	 file1.Close();



	n = hpgl.GetLength();
	LPTSTR	p = hpgl.GetBuffer(n);

	TRY
	{
		CFile file(strFileName, CFile::modeCreate | CFile::modeWrite );

		file.Write(p, n);

		file.Close();
	}

	CATCH( CFileException, e )
	{
		#ifdef _DEBUG
			afxDump << "File could not be opened " << e->m_cause << "\n";
			//AfxMessageBox ("File could not be opened ");
		#endif
		hpgl.ReleaseBuffer();
		return false;
	}
	END_CATCH
	hpgl.ReleaseBuffer();



	delete	[]temp;

		n = m_hpgl.GetLength();
	//LPTSTR	pp = m_hpgl.GetBuffer(n);
	if(n <= 0)
	{
		m_hpgl.Empty();
	}




     OpenDissecretFile(strFileName);
    

	DrawPicture(3);


			
//20160201切割指令识别

		return true;
// 	}
// 
// 	return false;
}

BOOL CPictureDlg::IsAlpha(char ch)//20160201切割指令识别
{
	if(ch>='a' && ch<='z') return TRUE;
	if(ch>='A' && ch<='Z') return TRUE;
	return FALSE;
}

int CPictureDlg::GetParameterType(char cName[])//20160201切割指令识别
{
	int nType=-1;

	if(strcmp(cName,"DF")==0) nType=NONE_TYPE;//缺省
	else if(strcmp(cName,"IN")==0) nType=DIGIT_TYPE;//初始化
	else if(strcmp(cName,"IP")==0) nType=DIGIT_TYPE;//缩放点
	else if(strcmp(cName,"IR")==0) nType=DIGIT_TYPE;//相对缩放点
	else if(strcmp(cName,"SC")==0) nType=DIGIT_TYPE;//缩放
	else if(strcmp(cName,"PG")==0) nType=DIGIT_TYPE;//推进页面
	else if(strcmp(cName,"FR")==0) nType=DIGIT_TYPE;//推进页面
	else if(strcmp(cName,"PA")==0) nType=DIGIT_TYPE;//绝对绘图
	else if(strcmp(cName,"PD")==0) nType=DIGIT_TYPE;//落笔
	else if(strcmp(cName,"PU")==0) nType=DIGIT_TYPE;//提笔
	else if(strcmp(cName,"PE")==0) nType=PE_TYPE;//折线编码
	else if(strcmp(cName,"SP")==0) nType=DIGIT_TYPE;//选笔
	else if(strcmp(cName,"SS")==0) nType=NONE_TYPE;//选择标准字体
	else if(strcmp(cName,"DI")==0) nType=DIGIT_TYPE;//绝对方向------------
	else if(strcmp(cName,"DR")==0) nType=DIGIT_TYPE;//相对方向
	else if(strcmp(cName,"SI")==0) nType=DIGIT_TYPE;//绝对字符尺寸
	else if(strcmp(cName,"LO")==0) nType=DIGIT_TYPE;//标志原点172
	else if(strcmp(cName,"DT")==0) nType=DT_TYPE;//定义标志终结符
	else if(strcmp(cName,"LB")==0)
	{ 
		nType=STRING_TYPE;//标志171
		m_strText.Empty();
	}
	else if(strcmp(cName,"LT")==0) nType=DIGIT_TYPE;//线型119
	if(nType==-1)
	{
		//int k=0;
		nType=DIGIT_TYPE;
	}

	return nType;
}


int CPictureDlg::OpenDissecretFile(CString strFileName)//20160201切割指令识别
{
	CFile file;
	//20100406新增CFile::shareExclusive属性，避免其他CAD文件写自动绘图目录时写入不成功或打印缺页
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return 1;

	int nLength=file.GetLength();
	BOOL bError=FALSE;
	CInstruct Instruct;
	int nProcess;
	int nMnemonicCount;
	CString strParameter;
	int nParameterType=NONE_TYPE;
	int nEscCount;
	char cFileBuf[1001];
	int i,j,k,m;
	unsigned char ch;

	InitHPGL();
	InitTextChangeLine();
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
	nProcess=PROCESS_MNEMONIC;
	nMnemonicCount=0;
	m=j=0;
	bIsEncrypt=FALSE;//位图
	
	for(i=0;i<nLength;)
	{
		if (m < j)
		{
			k=0;
			while(k<(j-m))
			{
				cFileBuf[k] = cFileBuf[m+k];
				k++;
			}
			if ((i + 1000 - k) < nLength)
				j = 1000 - k;
			else
				j = nLength - i;
		}
		else
		{
			if ((i + 1000) < nLength)
				j = 1000;
			else
				j = nLength - i;
			k = 0;
		}
		file.Read(&cFileBuf[k],j);
		i += j;
		j += k;
		k = 0;
		m = j;
		while (k < j)
		{
			if (nProcess == PROCESS_FIND)
			{//寻找下一个';'
				while (k < j)
				{
					ch = cFileBuf[k++];
					if(ch==';')
					{
						nProcess=PROCESS_MNEMONIC;
						break;
					}
				}
			}
			else if (nProcess == PROCESS_MNEMONIC)//助记符(命令)
			{
				ch = cFileBuf[k++];
				if (nMnemonicCount == 0)
				{
					if(IsAlpha(ch))
						Instruct.m_cName[nMnemonicCount++]=toupper(ch);
					else if(ch==27)
					{
						nProcess=PROCESS_PARAMETER;
						nParameterType=ESC_TYPE;
						nEscCount=4;
					}
				}
				else
				{
					if(IsAlpha(ch))
					{
						Instruct.m_cName[nMnemonicCount++]=toupper(ch);
						nProcess=PROCESS_PARAMETER;
						nParameterType=GetParameterType(Instruct.m_cName);
						strParameter.Empty();
						if(nParameterType==PE_TYPE)
						{
							nProcess=PROCESS_FIND;
							nMnemonicCount=0;
						}
					}
					else
						nMnemonicCount=0;
				}
			}
			else
			{//参数
				switch(nParameterType)
				{
					case ESC_TYPE:
						if ((k+nEscCount) <= j)
						{
							k+=nEscCount;
							nProcess=PROCESS_MNEMONIC;
						}
						else
						{
							nEscCount-=(j-k);
							k=j;
						}
						break;
					case NONE_TYPE:
						ch = cFileBuf[k++];
						if(ch==';')
						{
							ProcessInstruct(&Instruct);
							nProcess=PROCESS_MNEMONIC;
							nMnemonicCount=0;
						}
						else if(IsAlpha(ch))
						{
							ProcessInstruct(&Instruct);
							nProcess=PROCESS_MNEMONIC;
							nMnemonicCount=0;
							Instruct.m_cName[nMnemonicCount++]=toupper(ch);
						}
						else if(IsDigit(ch))
							bError=TRUE;
						break;
					case DIGIT_TYPE:
						m=k;
						while (k < j)
						{
							ch = cFileBuf[k++];
							if (!IsDigit(ch))
							{
								if (ch=='.')
									continue;
								if (m != (k-1))
									Instruct.m_adPara.Add(atof(&cFileBuf[m]));
								if (ch==';')
								{
									ProcessInstruct(&Instruct);
									Instruct.m_adPara.RemoveAll();
									nProcess=PROCESS_MNEMONIC;
									nMnemonicCount=0;
									m = j;
									break;
								}
								else if (IsAlpha(ch))
								{
									ProcessInstruct(&Instruct);
									Instruct.m_adPara.RemoveAll();
									nMnemonicCount=0;
									nProcess=PROCESS_MNEMONIC;
									Instruct.m_cName[nMnemonicCount++]=toupper(ch);
									m = j;
									break;
								}
								else if(ch==27)
								{
									ProcessInstruct(&Instruct);
									Instruct.m_adPara.RemoveAll();
									nMnemonicCount=0;
									nProcess=PROCESS_PARAMETER;
									nParameterType=ESC_TYPE;
									nEscCount=4;
									m = j;
									break;
								}
								else
									m=k;
							}
						}
						break;
					case STRING_TYPE:
						ch = cFileBuf[k++];
						if(ch==m_cTerminator)
						{
							if(m_cTerminatorMode==0)
							{
								CString str="1";
								str.SetAt(0,ch);
								m_strText+=str;
							}
							ProcessInstruct(&Instruct);
							Instruct.m_adPara.RemoveAll();
							nProcess=PROCESS_MNEMONIC;
							nMnemonicCount=0;
						}
						else
						{
							CString str="1";
							str.SetAt(0,ch);
							m_strText+=str;
						}
						break;
					case DT_TYPE:
						ch = cFileBuf[k++];
						if(m_cTerminatorMode==0x81)
						{
							if(ch==',')
								m_cTerminatorMode=0x82;
							else
							{
								m_cTerminatorMode=1;
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
								k--;
							}
						}
						else if(m_cTerminatorMode==0x82)
						{
							if(ch==0 || ch==1)
								m_cTerminatorMode=ch;
							else
							{
								m_cTerminatorMode=1;
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
								k--;
							}
						}
						else
						{
							if(ch==';')
							{
								m_cTerminator=theApp.m_cTerminator;
								m_cTerminatorMode=1;
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
							}
							else if(ch!=0 && ch!=5 && ch!=27)
							{
								m_cTerminator=ch;
								m_cTerminatorMode=0x81;
							}
							else
							{
								nProcess=PROCESS_MNEMONIC;
								nMnemonicCount=0;
							}
						}
						break;
					default:
						break;
				}
			}
			if(bError)
				break;
		}
		if(bError)
			break;
	}
	if(!bError)
		ProcessInstruct(&Instruct);
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
	file.Close();
	if(bError)
		return 2;
	m_lMaxPlotLength=m_lMaxPGLength-m_lMinPlotLength;
	if (m_lMaxPlotWidth < m_lMaxPGWidth)
		m_lMaxPlotWidth = m_lMaxPGWidth;
	m_lMaxPlotWidth -= m_lMinPlotWidth;
	if (theApp.m_nRotate == 1)
	{
		long lMaxPlotLength=m_lMaxPlotLength;
		m_lMaxPlotLength=m_lMaxPlotWidth;
		m_lMaxPlotWidth=lMaxPlotLength;
	}
	return 0;
}



void CPictureDlg::InitHPGL()//20160201切割指令识别
{
	m_bPSValid=false;
	m_bIPValid=false;
	m_bSCValid=false;
	m_rtHardLimitation=CRect(0,0,40000,40000);
	m_dpOrigin=CPoint(0,0);
	m_dpP1=CPoint(0,0);
	m_dpP2=CPoint(40000,40000);
	m_dRatioX=1;//当前的缩放比率
	m_dRatioY=1;//当前的缩放比率
	m_nRotate=0;
	m_dpCurrent=CPoint(0,0);
	m_bPenDown=FALSE;
	m_nLineType=LINE_TYPE_SOLID;
	m_bAbsolute=TRUE;
	m_nPen=1;
	m_cTerminator=theApp.m_cTerminator;//标志终结符
	m_cTerminatorMode=1;
	m_dTextHeight=0.375;//cm
	m_dTextWidth=0.285;//cm
	m_dTextAngle=0;
	m_nTextPosition=1;
	m_lMaxPlotLength=0;
	m_lMaxPlotWidth=0;
	m_lMinPlotLength=0;
	m_lMinPlotWidth=0;
	m_lMaxPGLength=0;
	m_lMaxPGWidth=0;
	RemoveGLBuffer();

	//m_dFixedPointValue = 0;
}


void CPictureDlg::RemoveGLBuffer()//20160201切割指令识别
{
	POSITION pos=theApp.m_listPrimitive.GetHeadPosition();
	while(pos)
	{
		CGLPrimitive *p=(CGLPrimitive*)theApp.m_listPrimitive.GetNext(pos);
		p->m_anData.RemoveAll();
		delete p;
	}
	theApp.m_listPrimitive.RemoveAll();

	pos=theApp.m_listTextPrimitive.GetHeadPosition();
	while(pos)
	{
		CGLText *pText=(CGLText *)theApp.m_listTextPrimitive.GetNext(pos);
		pText->m_adPara.RemoveAll();
		delete pText;
	}
	theApp.m_listTextPrimitive.RemoveAll();

	/*
	int i,listCount = 0;
	listCount = theApp.m_listBitDataPrimitive.GetCount();
	pos = theApp.m_listBitDataPrimitive.GetHeadPosition();
	for(i=0; i<listCount && pos!=NULL; i++)
	{
		BitPrimitive *pBitPrimitive = (BitPrimitive*)theApp.m_listBitDataPrimitive.GetNext(pos);
		if(pBitPrimitive != NULL)
		{
			delete pBitPrimitive;
			theApp.m_listBitDataPrimitive.RemoveTail();
		}
	}
	theApp.m_listBitDataPrimitive.RemoveAll();
*/
	//位图
	//listCount = theApp.m_listBitListPrimitive.GetCount();
	//pos = theApp.m_listBitListPrimitive.GetHeadPosition();
	//for(i=0; i<listCount && pos!=NULL; i++)
	//{
		//BitList *pBitList = (BitList*)theApp.m_listBitListPrimitive.GetNext(pos);
		//if(pBitList != NULL)
		//{
			//delete pBitList;
			//theApp.m_listBitListPrimitive.RemoveTail();
		//}
	//}
	//theApp.m_listBitListPrimitive.RemoveAll();
//位图
}


void CPictureDlg::InitTextChangeLine(void)//20160201切割指令识别
{
	CString strPL;
  TCHAR szFullPath[_MAX_PATH];
  TCHAR szDir[_MAX_DIR];
  TCHAR szDrive[_MAX_DRIVE];
  CString sWorkPath,strCharFileName;
  CFile fFile1;
  int nCountChar;
  CChinese_char Chinese_char1;
  CEnglish_char English_char1;
  BYTE byte[10];
  int nCharCount_C,nCharCount_E;
  CString strNetworkPath,string1;
  CFileFind filefind;

	m_ptCurrentPosition=NULL;

  ::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
  _splitpath(szFullPath,szDrive,szDir,NULL,NULL);
  sWorkPath.Format(_T("%s%s"),szDrive,szDir);

  strCharFileName=sWorkPath;
  strCharFileName=strCharFileName + _T("english.fnt");
  fFile1.Open((LPCTSTR)strCharFileName,CFile::modeRead | CFile::typeBinary);
  fFile1.SeekToBegin();
  fFile1.Read(byte,10);
  m_nInternalLeading_E=(int)((WORD)byte[0] | ((WORD)byte[1] << 8));
  m_nHeight_E=(int)((WORD)byte[2] | ((WORD)byte[3] << 8));
  m_nDescent_E=(int)((WORD)byte[4] | ((WORD)byte[5] << 8));
  nCharCount_E=(int)((DWORD)byte[6] | ((DWORD)byte[7] << 8) | ((DWORD)byte[9] << 16) | ((DWORD)byte[9] << 24));
  m_aEnglish_char.RemoveAll();
  m_aEnglish_char.SetSize(nCharCount_E);
  fFile1.Seek(10,CFile::begin);
  nCountChar=nCharCount_E;
  while (nCountChar > 0)
  {
    fFile1.Read(byte,10);

    English_char1.m_wCharCode=(WORD)byte[0] | ((WORD)byte[1] << 8);
    English_char1.m_wBytes=(WORD)byte[2] | ((WORD)byte[3] << 8);
    English_char1.m_wWidth=(WORD)byte[4] | ((WORD)byte[5] << 8);
    English_char1.m_dwPosition=(DWORD)byte[6] | ((DWORD)byte[7] << 8) | ((DWORD)byte[9] << 16) | ((DWORD)byte[9] << 24);

    m_aEnglish_char.SetAt(nCharCount_E-nCountChar,English_char1);

    nCountChar--;
  }
  fFile1.Close();

  strCharFileName=sWorkPath;
  strCharFileName=strCharFileName + _T("hz.fnt");
  fFile1.Open((LPCTSTR)strCharFileName,CFile::modeRead | CFile::typeBinary);
  fFile1.SeekToBegin();
  fFile1.Read(byte,8);
  m_nWidth_C=(int)((WORD)byte[0] | ((WORD)byte[1] << 8));
  m_nHeight_C=(int)((WORD)byte[2] | ((WORD)byte[3] << 8));
  nCharCount_C=(int)((DWORD)byte[4] | ((DWORD)byte[5] << 8) | ((DWORD)byte[6] << 16) | ((DWORD)byte[7] << 24));

  m_aChinese_char.RemoveAll();
  m_aChinese_char.SetSize(nCharCount_C);
  fFile1.Seek(8,CFile::begin);
  nCountChar=nCharCount_C;
  while (nCountChar > 0)
  {
    fFile1.Read(byte,8);

    Chinese_char1.m_wCharCode=(WORD)byte[0] | ((WORD)byte[1] << 8);
    Chinese_char1.m_wBytes=(WORD)byte[2] | ((WORD)byte[3] << 8);
    Chinese_char1.m_dwPosition=(DWORD)byte[4] | ((DWORD)byte[5] << 8) | ((DWORD)byte[6] << 16) | ((DWORD)byte[7] << 24);

    m_aChinese_char.SetAt(nCharCount_C-nCountChar,Chinese_char1);

    nCountChar--;
  }
  fFile1.Close();
}


void CPictureDlg::GLTextMinXMaxX(CGLText *pText)//20160201切割指令识别
{
	int nLen;
	double PI=3.14159265359;
	double dTextAngle=m_dTextAngle/180.0*PI;
	double dLength;

	nLen=pText->m_strText.GetLength();
	if(nLen==0)
		return;

	CDPoint dp0,dp;
	dp0.x=pText->m_adPara.GetAt(0);
	dp0.y=pText->m_adPara.GetAt(1);
			
	switch(m_nTextPosition)
	{
		case 1:
			dp0.x=dp0.x-(m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(m_dTextHeight*400)*cos(dTextAngle);
			break;
		case 2:
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			break;
		case 4:
			dp0.x=dp0.x-(m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 5:
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 6:
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 7:
			dp0.x=dp0.x-(m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 8:
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 9:
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 11:
			dTextAngle=atan2(0.25*m_dTextHeight*400,0.25*m_dTextWidth*400)+m_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*m_dTextHeight*400)*(0.25*m_dTextHeight*400)+(0.25*m_dTextWidth*400)*(0.25*m_dTextWidth*400));
			dp0.x=dp0.x+dLength*cos(dTextAngle);
			dp0.y=dp0.y+dLength*sin(dTextAngle);
			dTextAngle=m_dTextAngle/180.0*PI;
			dp0.x=dp0.x-(m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(m_dTextHeight*400)*cos(dTextAngle);
			break;
		case 12:
			dp0.x=dp0.x+(0.25*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y+(0.25*m_dTextWidth*400)*sin(dTextAngle);
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			break;
		case 13:
			dTextAngle=atan2(0.25*m_dTextHeight*400,0.25*m_dTextWidth*400)-m_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*m_dTextHeight*400)*(0.25*m_dTextHeight*400)+(0.25*m_dTextWidth*400)*(0.25*m_dTextWidth*400));
			dp0.x=dp0.x+dLength*cos(dTextAngle);
			dp0.y=dp0.y-dLength*sin(dTextAngle);
			dTextAngle=m_dTextAngle/180.0*PI;
			break;
		case 14:
			dp0.x=dp0.x-(1.25*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(1.25*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 15:
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 16:
			dp0.x=dp0.x+(0.25*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y-(0.25*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(0.5*nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.5*nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 17:
			dTextAngle=atan2(0.25*m_dTextHeight*400,0.25*m_dTextWidth*400)-m_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*m_dTextHeight*400)*(0.25*m_dTextHeight*400)+(0.25*m_dTextWidth*400)*(0.25*m_dTextWidth*400));
			dp0.x=dp0.x-dLength*cos(dTextAngle);
			dp0.y=dp0.y+dLength*sin(dTextAngle);
			dTextAngle=m_dTextAngle/180.0*PI;
			dp0.x=dp0.x-(m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 18:
			dp0.x=dp0.x-(0.25*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(0.25*m_dTextWidth*400)*sin(dTextAngle);
			dp0.x=dp0.x-(0.5*m_dTextHeight*400)*sin(dTextAngle);
			dp0.y=dp0.y+(0.5*m_dTextHeight*400)*cos(dTextAngle);
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
		case 19:
			dTextAngle=atan2(0.25*m_dTextHeight*400,0.25*m_dTextWidth*400)+m_dTextAngle;
			dTextAngle=dTextAngle/180.0*PI;
			dLength=sqrt((0.25*m_dTextHeight*400)*(0.25*m_dTextHeight*400)+(0.25*m_dTextWidth*400)*(0.25*m_dTextWidth*400));
			dp0.x=dp0.x-dLength*cos(dTextAngle);
			dp0.y=dp0.y-dLength*sin(dTextAngle);
			dTextAngle=m_dTextAngle/180.0*PI;
			dp0.x=dp0.x-(nLen*m_dTextWidth*400)*cos(dTextAngle);
			dp0.y=dp0.y-(nLen*m_dTextWidth*400)*sin(dTextAngle);
			break;
	}
	pText->m_adPara.RemoveAll();
	pText->m_adPara.Add(dp0.x);
	pText->m_adPara.Add(dp0.y);
	if(dp0.x>m_lMaxPGLength)
		int i=0;
		//m_lMaxPGLength=ftoi(dp0.x);
	else if (dp0.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp0.x);
	if(dp0.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp0.y);
	else if(dp0.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp0.y);
	dp.x=dp0.x+(nLen*m_dTextWidth*400)*cos(dTextAngle);
	dp.y=dp0.y+(nLen*m_dTextWidth*400)*sin(dTextAngle);
	if(dp.x>m_lMaxPGLength)
		int i=0;//m_lMaxPGLength=ftoi(dp.x);
	else if (dp.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp.x);
	if(dp.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp.y);
	else if(dp.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp.y);
	dp.x=dp.x+(m_dTextHeight*400)*sin(dTextAngle);
	dp.y=dp.y-(m_dTextHeight*400)*cos(dTextAngle);
	if(dp.x>m_lMaxPGLength)
		int i=0;//m_lMaxPGLength=ftoi(dp.x);
	else if (dp.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp.x);
	if(dp.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp.y);
	else if(dp.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp.y);
	dp.x=dp0.x+(m_dTextHeight*400)*sin(dTextAngle);
	dp.y=dp0.y-(m_dTextHeight*400)*cos(dTextAngle);
	if(dp.x>m_lMaxPGLength)
		int i=0;//m_lMaxPGLength=ftoi(dp.x);
	else if (dp.x<m_lMinPlotLength)
		m_lMinPlotLength=ftoi(dp.x);
	if(dp.y>m_lMaxPGWidth)
		m_lMaxPGWidth=ftoi(dp.y);
	else if(dp.y<m_lMinPlotWidth)
		m_lMinPlotWidth=ftoi(dp.y);
}


void CPictureDlg::TextChangeLine(CPoint dp, LPCTSTR lpszString, int nCount)//20160201切割指令识别
{
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString sWorkPath,strEnglish,strChinese;
	CFile fileEnglish,fileChinese;
	BYTE *pbyData;
	int nLeft;
	BYTE byByte1,byByte2;
	WORD wCharCode;
	int nIndex1,nIndex2,nIndex3,nIndex4;
	BOOL bEnglishChar;

	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	sWorkPath.Format(_T("%s%s"),szDrive,szDir);
	strEnglish=sWorkPath;
	strEnglish=strEnglish + _T("english.fnt");
	strChinese=sWorkPath;
	strChinese=strChinese + _T("hz.fnt");

	fileEnglish.Open((LPCTSTR)strEnglish,CFile::modeRead | CFile::typeBinary);
	fileChinese.Open((LPCTSTR)strChinese,CFile::modeRead | CFile::typeBinary);

	nLeft=0;
	nIndex1=0;
	while (nIndex1 < nCount)
	{
		pbyData=NULL;
		byByte1=(BYTE)lpszString[nIndex1];
		if ((byByte1 >= 128) && ((nIndex1+1) < nCount))
		{
			bEnglishChar=false;
			nIndex1++;
			byByte2=(BYTE)lpszString[nIndex1];
			wCharCode=(((WORD)byByte1) << 8) | (WORD)byByte2;
			nIndex2=0;
			nIndex3=m_aChinese_char.GetSize() - 1;
			if (m_aChinese_char[nIndex2].m_wCharCode == wCharCode)
				nIndex4=nIndex2;
      else if (m_aChinese_char[nIndex3].m_wCharCode == wCharCode)
				nIndex4=nIndex3;
      else
				nIndex4=(nIndex2 + nIndex3) / 2;
      while (((nIndex3 - nIndex2) > 1) && (m_aChinese_char[nIndex4].m_wCharCode != wCharCode))
			{
        if (m_aChinese_char[nIndex4].m_wCharCode > wCharCode)
				{
          nIndex3=nIndex4;
          if (nIndex2 > nIndex3)
						break;
          nIndex4=(nIndex2 + nIndex3) / 2;
        }
        else
				{
          nIndex2=nIndex4;
          if (nIndex2 > nIndex3)
						break;
          nIndex4=(nIndex2 + nIndex3) / 2;
        }
      }
      if ((m_aChinese_char[nIndex4].m_wCharCode == wCharCode) &&
          (m_aChinese_char[nIndex4].m_wBytes > 0))
			{
        pbyData=new BYTE[m_aChinese_char[nIndex4].m_wBytes + 1];
        fileChinese.Seek(m_aChinese_char[nIndex4].m_dwPosition,CFile::begin);
        fileChinese.Read(pbyData,m_aChinese_char[nIndex4].m_wBytes);
      }
    }
    else
		{
      bEnglishChar=true;
      wCharCode=(WORD)byByte1;
      nIndex2=0;
      nIndex3=m_aEnglish_char.GetSize() - 1;
      if (m_aEnglish_char[nIndex2].m_wCharCode == wCharCode)
				nIndex4=nIndex2;
      else if (m_aEnglish_char[nIndex3].m_wCharCode == wCharCode)
				nIndex4=nIndex3;
      else
				nIndex4=(nIndex2 + nIndex3) / 2;
      while (((nIndex3 - nIndex2) > 1) && (m_aEnglish_char[nIndex4].m_wCharCode != wCharCode))
			{
        if (m_aEnglish_char[nIndex4].m_wCharCode > wCharCode)
				{
          nIndex3=nIndex4;
          if (nIndex2 > nIndex3)
						break;
          nIndex4=(nIndex2 + nIndex3) / 2;
        }
        else
				{
          nIndex2=nIndex4;
          if (nIndex2 > nIndex3)
						break;
          nIndex4=(nIndex2 + nIndex3) / 2;
        }
      }
      if ((m_aEnglish_char[nIndex4].m_wCharCode == wCharCode) &&
          (m_aEnglish_char[nIndex4].m_wBytes > 0))
			{
        pbyData=new BYTE[m_aEnglish_char[nIndex4].m_wBytes + 1];

        fileEnglish.Seek(m_aEnglish_char[nIndex4].m_dwPosition,CFile::begin);
        fileEnglish.Read(pbyData,m_aEnglish_char[nIndex4].m_wBytes);
      }
    }
    if (pbyData != NULL)
		{
      if (bEnglishChar)
				PlotChar(dp,pbyData,m_aEnglish_char[nIndex4].m_wBytes,nLeft,0,m_nHeight_C);
     else
				PlotChar(dp,pbyData,m_aChinese_char[nIndex4].m_wBytes,nLeft,0,m_nInternalLeading_E + m_nHeight_E);
      delete pbyData;
      if (bEnglishChar)
				nLeft=nLeft + m_aEnglish_char[nIndex4].m_wWidth * m_nHeight_C;
      else
				nLeft=nLeft + m_nWidth_C * (m_nInternalLeading_E + m_nHeight_E);
    }
    else
		{
      if (bEnglishChar)
				nLeft=nLeft + m_aEnglish_char[0].m_wWidth * m_nHeight_C;
      else
				nLeft=nLeft + m_nWidth_C * (m_nInternalLeading_E + m_nHeight_E);
    }
    nIndex1++;
  }
  fileEnglish.Close();
  fileChinese.Close();
}


CPoint CPictureDlg::CPToLP(CPoint ptCP, int nHeight, CPoint ptPointLU, double dSin, double dCos, double dScale)//20160201切割指令识别
{
	double dx,dy;
  CPoint ptPoint1;

  dx=(double)ptCP.x;
  dy=(double)ptCP.y-(double)nHeight;
  ptPoint1.x=ptPointLU.x + IntRound((dx*dCos - dy*dSin) / dScale);
  ptPoint1.y=ptPointLU.y + IntRound((dx*dSin + dy*dCos) / dScale);

  return ptPoint1;
}

int CPictureDlg::IntRound(double x)//20160201切割指令识别
{
	double d;
  int nResult;

  if (x<0)
    d=x-0.5;
  else
    d=x+0.5;

  if ((d > 0) && (d > (double)INT_MAX))  {
    nResult=INT_MAX;
  }
  else  {
    if ((d < 0) && (d < (double)INT_MIN))  {
      nResult=INT_MIN;
    }
    else  {
      nResult=(int)d;
    }
  }

  return nResult;
}


void CPictureDlg::MoveTo(long x, long y)//20160201切割指令识别
{
	m_ptCurrentPosition.x=x;
	m_ptCurrentPosition.y=y;

  m_ptPrevPosition=m_ptCurrentPosition;
  m_ptPrevPosition.x=m_ptPrevPosition.x +1;  //为在LineTo中优化用
}


void CPictureDlg::LineTo(long x, long y)//20160201切割指令识别
{
  CPoint ptCurrentPoint;
	CPoint ptPoint1;
	CPoint ptPoint2;
	CRect rect1;
  
	ptCurrentPoint.x=x;
	ptCurrentPoint.y=y;
//	rect1=CRect(0,0,m_sizePageSize.cx,m_sizePageSize.cy);

	ptPoint1=m_ptCurrentPosition;
	ptPoint2=ptCurrentPoint;
	if ((m_ptCurrentPosition == m_ptPrevPosition) &&
			(0 < m_ptCurrentPosition.x) && (0 < m_ptCurrentPosition.y))
	{
	}
	else
	{
		WriteTextData(true,ptPoint1.x,ptPoint1.y);
	}
	WriteTextData(false,ptPoint2.x,ptPoint2.y);
  m_ptPrevPosition = ptCurrentPoint;
  m_ptCurrentPosition = ptCurrentPoint;
}


void CPictureDlg::WriteTextData(BOOL bFirst,long x,long y)//20160201切割指令识别
{
	CGLPrimitive *p;

	if(bFirst)
	{
		p=new CGLPrimitive;
		theApp.m_listPrimitive.AddTail(p);
		p->m_nType=PRIMITIVE_LINE;
		p->m_anData.Add(x);
		p->m_anData.Add(y);
	}
	else
	{
		p=(CGLPrimitive*)theApp.m_listPrimitive.GetTail();
		p->m_anData.Add(x);
		p->m_anData.Add(y);
	}
}


void CPictureDlg::Arc(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, int nXStartArc, int nYStartArc, int nXEndArc, int nYEndArc)//20160201切割指令识别
{
  double dXC,dYC,dRadius;
  double dx,dy;
  double dAngleS,dAngleE,dAngle,dStep;
  double dCos,dSin,PI=3.14159265359;
  int nCount;

  dXC=((double)nLeftRect + (double)nRightRect) / 2.0;
  dYC=((double)nTopRect + (double)nBottomRect) / 2.0;
  dRadius=((double)nRightRect - (double)nLeftRect) / 2.0;

  dAngleS=angle_2(dXC,dYC,(double)nXStartArc,(double)nYStartArc);
  dAngleE=angle_2(dXC,dYC,(double)nXEndArc,(double)nYEndArc);
  while (dAngleE >= dAngleS) dAngleE=dAngleE - 2.0 * PI;

  if ((nXStartArc == nXEndArc) && (nYStartArc == nYEndArc))  {
    dAngleS=0.0;
    dAngleE=-2.0 * PI;
  }

  dSin=sin(dAngleS);
  dCos=cos(dAngleS);
  dx=dRadius * dCos + dXC;
  dy=dRadius * dSin + dYC;
  MoveTo((int)dx,(int)dy);

  nCount=120;
  dStep=PI / 180.0 * (360.0 / (double)nCount);  //3度
  dAngle=dAngleS;
  while (dAngle > dAngleE)  {
    dAngle=dAngle - dStep;
    if (dAngle < dAngleE)  dAngle=dAngleE;
    dSin=sin(dAngle);
    dCos=cos(dAngle);
    dx=dRadius * dCos + dXC;
    dy=dRadius * dSin + dYC;

    LineTo((int)dx,(int)dy);
  }
}


double CPictureDlg::angle_2(double startx, double starty, double endx, double endy)//20160201切割指令识别
{
  double pi,q1;
  double x,y;

  pi=3.14159265359;

  q1=0.0;
  x=endx - startx;
  if (x==0)  {
    if (starty==endy)  return(q1);
    if (starty<endy)  q1=0.5*pi;
    else  q1=1.5*pi;
  }
  else  {
    y=endy-starty;
    q1=atan(y/x);
    if (x < 0)  {  //第二三象限
      q1=pi+q1;
    }
    else  {
      if ((x > 0) && (y < 0))  {  //第四象限
        q1=(double)2.0*pi+q1;
      }
    }
  }
  return q1;
}


BOOL CPictureDlg::CircleCR(double x1, double y1, double x2, double y2, double x3, double y3, double &cx, double &cy, double &cr)//20160201切割指令识别
{
  double a1,b1,c1,a2,b2,c2;
  BOOL bOk;

  if (((x1==x2) && (y1==y2)) ||
      ((x1==x3) && (y1==y3)) ||
      ((x2==x3) && (y2==y3)))  {  //三点中有两点相同
    return false;
  }

  a1=x1 - x2;
  b1=y1 - y2;
  c1=-a1 * (x1 + x2) / 2.0 - b1 * (y1 + y2) / 2.0;
  a2=x3 - x2;
  b2=y3 - y2;
  c2=-a2 * (x2 + x3) / 2.0 - b2 * (y2 + y3) / 2.0;
  bOk=IntOfTwoLine(a1,b1,c1,a2,b2,c2,cx,cy);

  if (bOk)  {
    cr=(x1 - cx) * (x1 - cx) + (y1 - cy) * (y1 - cy);
    cr=sqrt(cr);
  }

  return bOk;
}


int CPictureDlg::IntOfTwoLine(double a1, double b1, double c1, double a2, double b2, double c2, double &x, double &y)//20160201切割指令识别
{
  double p,q;
  int nResult;

  p=a1*b2-a2*b1;
  q=c2*b1-c1*b2;
  if ((fabs(p)+1.0) <= 1.0)  {  //两直线段平行
    if ((fabs(q)+1.0) <= 1.0)  {  //两直线重合
      nResult=2;
    }
    else  {  //两直线段平行但不重合
      nResult=0;
    }
  }
  else  {  //两直线段有交点
    x=q/p;
    q=a2*c1-a1*c2;
    y=q/p;
    nResult=1;
  }
  return nResult;
}


void CPictureDlg::PlotChar(CPoint ptPointLU, BYTE *pbyData, WORD wBytes, int nLeft, int nDown, int nFontScale)//20160201切割指令识别
{
  BYTE byByteX,byByteY;
  CPoint ptCurrentPos,ptCenter,ptPointS,ptPointE;
  CPoint ptPoint1;
  WORD wIndex1;
  WORD wDirection,wLength;
  BOOL bPenUp,bVerTextCommand,bClockwise;
  WORD wRadius,wArcS,wSpan;
  int nRadius;
  double dAngleS,dAngleE,dAngleOffsetS,dAngleOffsetE;
  double dx1,dy1,dx2,dy2,dxc,dyc,dr,dD,dH,dBulge;
  double dCos,dSin;
  double dLToDScale;
	double PI=3.14159265359;
  int nHeight;

  if (wBytes == 0)
		return;

  nHeight=(m_nInternalLeading_E + m_nHeight_E) * m_nHeight_C;
  dLToDScale=(double)nHeight / 	(m_dTextHeight*400);//实际毫米数

  dCos=cos(m_dTextAngle/180.0*PI);
  dSin=sin(m_dTextAngle/180.0*PI);

  ptCurrentPos=CPoint(nLeft,nDown);
  ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
  MoveTo(ptPoint1.x,ptPoint1.y);
  bVerTextCommand=false;
  bPenUp=false;
  wIndex1=0;
  while (wIndex1 < wBytes)
  {
    switch (pbyData[wIndex1])
		{
    case 0x0:
      wIndex1++;
      break;
    case 0x1:
      wIndex1++;
      bPenUp=false;
      break;
    case 0x2:
      wIndex1++;
      bPenUp=true;
      break;
    case 0x3:
      wIndex1++;
      wIndex1++;
      bVerTextCommand=false;
      break;
    case 0x4:
      wIndex1++;
      wIndex1++;
      bVerTextCommand=false;
      break;
    case 0x5:
      break;
    case 0x6:
      break;
    case 0x7:
      break;
    case 0x8:
      if (bVerTextCommand)
			{
        wIndex1=wIndex1+3;
      }
      else
			{
        wIndex1++;
        ptCurrentPos.x=ptCurrentPos.x + (char)pbyData[wIndex1] * nFontScale;
        wIndex1++;
        ptCurrentPos.y=ptCurrentPos.y + (char)pbyData[wIndex1] * nFontScale;

        ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        if (bPenUp)
					MoveTo(ptPoint1.x,ptPoint1.y);
        else
					LineTo(ptPoint1.x,ptPoint1.y);

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    case 0x9:
      if (bVerTextCommand)
			{
        wIndex1++;
        byByteX=pbyData[wIndex1];
        wIndex1++;
        byByteY=pbyData[wIndex1];
        while ((byByteX != 0) || (byByteY != 0))
				{
          wIndex1++;
          byByteX=pbyData[wIndex1];
          wIndex1++;
          byByteY=pbyData[wIndex1];
        }
        wIndex1++;
      }
      else
			{
        wIndex1++;
        byByteX=pbyData[wIndex1];
        wIndex1++;
        byByteY=pbyData[wIndex1];
        while ((byByteX != 0) || (byByteY != 0))
				{
          ptCurrentPos.x=ptCurrentPos.x + (char)byByteX * nFontScale;
          ptCurrentPos.y=ptCurrentPos.y + (char)byByteY * nFontScale;

          ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
          if (bPenUp)
						MoveTo(ptPoint1.x,ptPoint1.y);
          else
						LineTo(ptPoint1.x,ptPoint1.y);

          wIndex1++;
          byByteX=pbyData[wIndex1];
          wIndex1++;
          byByteY=pbyData[wIndex1];
        }

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    case 0xA:
      if (bVerTextCommand)
			{
        wIndex1=wIndex1+3;
      }
      else
			{
        wIndex1++;
        wRadius=(WORD)pbyData[wIndex1];
        wIndex1++;
        if (((char)pbyData[wIndex1]) < 0)
					bClockwise=true;
        else
					bClockwise=false;
        wArcS=(WORD)pbyData[wIndex1];
        wArcS=wArcS & 0x70;
        wArcS=wArcS >> 4;
        wSpan=(WORD)pbyData[wIndex1];
        wSpan=wSpan & 0x7;

        //计算圆心
        ptCenter=ptCurrentPos;
        ptCenter.x=ptCenter.x - IntRound((double)wRadius * (double)nFontScale * cos((double)wArcS * 45.0 / 180.0 * 3.14159265359));
        ptCenter.y=ptCenter.y - IntRound((double)wRadius * (double)nFontScale * sin((double)wArcS * 45.0 / 180.0 * 3.14159265359));

        dAngleS=(double)wArcS * 45.0 / 180.0 * 3.14159265359;
        if (bClockwise)
					dAngleE=dAngleS - (double)wSpan * 45.0 / 180.0 * 3.14159265359;
        else
					dAngleE=dAngleS + (double)wSpan * 45.0 / 180.0 * 3.14159265359;

        if (bClockwise)
				{
          ptPointS.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleE));
          ptPointS.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleE));
          ptPointE.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleS));
          ptPointE.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleS));
          ptCurrentPos=ptPointS;
        }
        else
				{
          ptPointS.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleS));
          ptPointS.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleS));
          ptPointE.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleE));
          ptPointE.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleE));
          ptCurrentPos=ptPointE;
        }

        ptCenter=CPToLP(ptCenter,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointS=CPToLP(ptPointS,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointE=CPToLP(ptPointE,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        nRadius=IntRound((double)wRadius * (double)nFontScale / dLToDScale);
        Arc(ptCenter.x - nRadius,ptCenter.y - nRadius,
            ptCenter.x + nRadius,ptCenter.y + nRadius,
            ptPointS.x,ptPointS.y,ptPointE.x,ptPointE.y);

        ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        MoveTo(ptPoint1.x,ptPoint1.y);

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    case 0xB:
      if (bVerTextCommand)
			{
        wIndex1=wIndex1+6;
      }
      else
			{
        wIndex1++;
        dAngleOffsetS=(double)((WORD)pbyData[wIndex1]);
        wIndex1++;
        dAngleOffsetE=(double)((WORD)pbyData[wIndex1]);
        wIndex1++;
        wRadius=(WORD)pbyData[wIndex1];
        wRadius=wRadius << 8;
        wIndex1++;
        wRadius=wRadius | (WORD)pbyData[wIndex1];
        wIndex1++;
        if (((char)pbyData[wIndex1]) < 0)
					bClockwise=true;
        else
					bClockwise=false;
        wArcS=(WORD)pbyData[wIndex1];
        wArcS=wArcS & 0x70;
        wArcS=wArcS >> 4;
        wSpan=(WORD)pbyData[wIndex1];
        wSpan=wSpan & 0x7;

        //计算圆心
        ptCenter=ptCurrentPos;
        ptCenter.x=ptCenter.x - IntRound((double)wRadius * (double)nFontScale * cos((double)wArcS * 45.0 / 180.0 * 3.14159265359));
        ptCenter.y=ptCenter.y - IntRound((double)wRadius * (double)nFontScale * sin((double)wArcS * 45.0 / 180.0 * 3.14159265359));

        dAngleS=(double)wArcS * 45.0;
        dAngleS=dAngleOffsetS * 45.0 / 256.0 + dAngleS;
        if (bClockwise)
					dAngleE=-(double)wSpan * 45.0;
        else
					dAngleE=(double)wSpan * 45.0;
        dAngleE=dAngleOffsetE * 45.0 / 256.0 + dAngleE;
        dAngleE=dAngleOffsetE * 45.0 / 256.0 + dAngleE;
        dAngleS=dAngleS / 180.0 * 3.14159265359;
        dAngleE=dAngleE / 180.0 * 3.14159265359;

        if (bClockwise)
				{
          ptPointS.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleE));
          ptPointS.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleE));
          ptPointE.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleS));
          ptPointE.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleS));
          ptCurrentPos=ptPointS;
        }
        else
				{
          ptPointS.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleS));
          ptPointS.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleS));
          ptPointE.x=ptCenter.x + IntRound((double)wRadius * (double)nFontScale * cos(dAngleE));
          ptPointE.y=ptCenter.y + IntRound((double)wRadius * (double)nFontScale * sin(dAngleE));
          ptCurrentPos=ptPointE;
        }

        ptCenter=CPToLP(ptCenter,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointS=CPToLP(ptPointS,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointE=CPToLP(ptPointE,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        nRadius=IntRound((double)wRadius * (double)nFontScale / dLToDScale);

        Arc(ptCenter.x - nRadius,ptCenter.y - nRadius,
            ptCenter.x + nRadius,ptCenter.y + nRadius,
            ptPointS.x,ptPointS.y,ptPointE.x,ptPointE.y);

        ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        MoveTo(ptPoint1.x,ptPoint1.y);

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    case 0xC:
      if (bVerTextCommand)
			{
        wIndex1=wIndex1+4;
      }
      else
			{
        wIndex1++;
        dx1=(double)((char)pbyData[wIndex1]) * (double)nFontScale;
        wIndex1++;
        dy1=(double)((char)pbyData[wIndex1]) * (double)nFontScale;
        wIndex1++;
        dBulge=(double)((char)pbyData[wIndex1]);
        if (((char)pbyData[wIndex1]) < 0)
					bClockwise=true;
        else
					bClockwise=false;

        dD=sqrt(dx1*dx1 + dy1*dy1);
        dH=fabs(dBulge) * dD / 127.0 / 2.0;

        if (bClockwise)
				{
          dx2=-dy1;
          dy2=dx1;
        }
        else
				{
          dx2=dy1;
          dy2=-dx1;
        }
        dx2=dx2 / dD * dH + dx1 / 2.0;
        dy2=dy2 / dD * dH + dy1 / 2.0;

        CircleCR(0.0,0.0,dx2,dy2,dx1,dy1,dxc,dyc,dr);
        ptCenter.x=ptCurrentPos.x + IntRound(dxc);
        ptCenter.y=ptCurrentPos.y + IntRound(dyc);

        if (bClockwise)
				{
          ptPointS.x=ptCurrentPos.x + IntRound(dx1);
          ptPointS.y=ptCurrentPos.y + IntRound(dy1);
          ptPointE=ptCurrentPos;
          ptCurrentPos=ptPointS;
        }
        else
				{
          ptPointS=ptCurrentPos;
          ptPointE.x=ptCurrentPos.x + IntRound(dx1);
          ptPointE.y=ptCurrentPos.y + IntRound(dy1);
          ptCurrentPos=ptPointE;
        }

        ptCenter=CPToLP(ptCenter,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointS=CPToLP(ptPointS,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        ptPointE=CPToLP(ptPointE,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        nRadius=IntRound(dr / dLToDScale);

        Arc(ptCenter.x - nRadius,ptCenter.y - nRadius,
            ptCenter.x + nRadius,ptCenter.y + nRadius,
            ptPointS.x,ptPointS.y,ptPointE.x,ptPointE.y);

        ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        MoveTo(ptPoint1.x,ptPoint1.y);

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    case 0xD:
      break;
    case 0xE:
      bVerTextCommand=true;
      wIndex1++;
      break;
    default:
      if (bVerTextCommand)
			{
        wIndex1++;
      }
      else
			{
        wDirection=(WORD)(pbyData[wIndex1] % 16);
        wLength=(WORD)(pbyData[wIndex1] / 16) * (WORD)nFontScale;
        ptCurrentPos=GetNextCoodinate(wDirection,wLength,ptCurrentPos);

        ptPoint1=CPToLP(ptCurrentPos,nHeight,ptPointLU,dSin,dCos,dLToDScale);
        if (bPenUp)
					MoveTo(ptPoint1.x,ptPoint1.y);
        else
					LineTo(ptPoint1.x,ptPoint1.y);

        wIndex1++;
      }
      bVerTextCommand=false;
      break;
    }
  }
}



CPoint CPictureDlg::GetNextCoodinate(WORD wDirection, WORD wLength, CPoint ptPoint)//20160201切割指令识别
{
  CPoint ptPoint1;

  switch (wDirection)  {
  case 0:
    ptPoint1.x = ptPoint.x + (int)wLength;
    ptPoint1.y = ptPoint.y;
    break;
  case 1:
    ptPoint1.x = ptPoint.x + (int)wLength;
    ptPoint1.y = ptPoint.y + (int)wLength/2;
    break;
  case 2:
    ptPoint1.x = ptPoint.x + (int)wLength;
    ptPoint1.y = ptPoint.y + (int)wLength;
    break;
  case 3:
    ptPoint1.x = ptPoint.x + (int)wLength/2;
    ptPoint1.y = ptPoint.y + (int)wLength;
    break;
  case 4:
    ptPoint1.x = ptPoint.x;
    ptPoint1.y = ptPoint.y + (int)wLength;
    break;
  case 5:
    ptPoint1.x = ptPoint.x - (int)wLength/2;
    ptPoint1.y = ptPoint.y + (int)wLength;
    break;
  case 6:
    ptPoint1.x = ptPoint.x - (int)wLength;
    ptPoint1.y = ptPoint.y + (int)wLength;
    break;
  case 7:
    ptPoint1.x = ptPoint.x - (int)wLength;
    ptPoint1.y = ptPoint.y + (int)wLength/2;
    break;
  case 8:
    ptPoint1.x = ptPoint.x - (int)wLength;
    ptPoint1.y = ptPoint.y;
    break;
  case 9:
    ptPoint1.x = ptPoint.x - (int)wLength;
    ptPoint1.y = ptPoint.y - (int)wLength/2;
    break;
  case 10:
    ptPoint1.x = ptPoint.x - (int)wLength;
    ptPoint1.y = ptPoint.y - (int)wLength;
    break;
  case 11:
    ptPoint1.x = ptPoint.x - (int)wLength/2;
    ptPoint1.y = ptPoint.y - (int)wLength;
    break;
  case 12:
    ptPoint1.x = ptPoint.x;
    ptPoint1.y = ptPoint.y - (int)wLength;
    break;
  case 13:
    ptPoint1.x = ptPoint.x + (int)wLength/2;
    ptPoint1.y = ptPoint.y - (int)wLength;
    break;
  case 14:
    ptPoint1.x = ptPoint.x + (int)wLength;
    ptPoint1.y = ptPoint.y - (int)wLength;
    break;
  case 15:
    ptPoint1.x = ptPoint.x + (int)wLength;
    ptPoint1.y = ptPoint.y - (int)wLength/2;
    break;
  }

  return ptPoint1;
}


void CPictureDlg::SetRatio()//20160201切割指令识别
{
	double d1,d2;
	if(m_dpP2.x==m_dpP1.x) m_dpP2.x+=1;
	if(m_dpP2.y==m_dpP1.y) m_dpP2.y+=1;
	if (!m_bIPValid || !m_bSCValid)
		return;
	
	switch(m_struSC.nType)
	{
		case 0://异步
			m_dRatioX=(m_dpP2.x-m_dpP1.x)/(m_struSC.dXMax-m_struSC.dXMin);
			m_dRatioY=(m_dpP2.y-m_dpP1.y)/(m_struSC.dYMax-m_struSC.dYMin);
			m_dpOrigin.x=ftoi(m_dpP1.x-m_struSC.dXMin*m_dRatioX);
			m_dpOrigin.y=ftoi(m_dpP1.y-m_struSC.dYMin*m_dRatioY);
			break;
		case 1://同步
			d1=(m_dpP2.x-m_dpP1.x)/(m_struSC.dXMax-m_struSC.dXMin);
			d2=(m_dpP2.y-m_dpP1.y)/(m_struSC.dYMax-m_struSC.dYMin);
			if(d1>d2) //x>y
			{
				m_dRatioY=d2;
				m_dRatioX=d2;
				m_dpOrigin.y=m_dpP1.y;
				m_dpOrigin.x=ftoi(((m_dpP2.x-m_dpP1.x)-(m_struSC.dXMax-m_struSC.dXMin)*d2)*m_struSC.dLeft/100.0);
			}
			else //x<y
			{
				m_dRatioX=d1;
				m_dRatioY=d1;
				m_dpOrigin.x=m_dpP1.x;
				m_dpOrigin.y=ftoi(((m_dpP2.y-m_dpP1.y)-(m_struSC.dYMax-m_struSC.dYMin)*d1)*m_struSC.dBottom/100.0);
			}
			break;
		case 2://点因子
			m_dRatioX=m_struSC.dXMax;
			m_dRatioY=m_struSC.dYMax;
			m_dpOrigin.x=ftoi(m_dpP1.x-m_struSC.dXMin*m_dRatioX);
			m_dpOrigin.y=ftoi(m_dpP1.y-m_struSC.dYMin*m_dRatioY);
			break;
	}
}


void CPictureDlg::ProcessInstruct(CInstruct *pInstruct)//20160201切割指令识别
{
	int nSize;
	int i;
	CString str;
	CGLPrimitive *p;
	CGLText *pText;
	//BitPrimitive *pBitPtr;
	CPoint dp,dp1;
	//POSITION pos=NULL;
	double d1;
	double d2;
	
	if(strcmp(pInstruct->m_cName,"PD")==0)//落笔
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			dp=m_dpCurrent;
			if(m_nPen==0)
			{			
				if(m_bAbsolute)
				{
					dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
					dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y;
				}
				else
				{
					for(int i=0;i<nSize;)
					{
						dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX);
						dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY);
					}
				}
				m_dpCurrent=dp;
			}
			else
			{
				p=new CGLPrimitive;
				theApp.m_listPrimitive.AddTail(p);
						//20150729
				//AfxMessageBox (str(m_nPen));
				if (m_nPen == g_cPenNo)
				{
					p->m_nType=PRIMITIVE_LINE;
				}
				else if (m_nPen != g_cPenNo)
				{
					p->m_nType=PRIMITIVE_CUTLINE;
				}
				//p->m_nType=PRIMITIVE_LINE;//20150729
				p->m_anData.Add(dp.x);
				p->m_anData.Add(dp.y);

				if (dp.x>m_lMaxPGLength)
					m_lMaxPGLength=dp.x;
				else if (dp.x<m_lMinPlotLength)
					m_lMinPlotLength=dp.x;
				if (dp.y>m_lMaxPGWidth)
					m_lMaxPGWidth=dp.y;
				else if (dp.y<m_lMinPlotWidth)
					m_lMinPlotWidth=dp.y;
							
				if(m_bAbsolute)
				{
					for(int i=0;i<nSize;)
					{
						dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
						dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY)+m_dpOrigin.y;
						p->m_anData.Add(dp.x);
						p->m_anData.Add(dp.y);
						if (dp.x>m_lMaxPGLength)
							m_lMaxPGLength=dp.x;
						else if (dp.x<m_lMinPlotLength)
							m_lMinPlotLength=dp.x;
						if (dp.y>m_lMaxPGWidth)
							m_lMaxPGWidth=dp.y;
						else if (dp.y<m_lMinPlotWidth)
							m_lMinPlotWidth=dp.y;
					}
				}
				else
				{
					for(int i=0;i<nSize;)
					{
						dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX);
						dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY);
						p->m_anData.Add(dp.x);
						p->m_anData.Add(dp.y);
						if (dp.x>m_lMaxPGLength)
							m_lMaxPGLength=dp.x;
						else if (dp.x<m_lMinPlotLength)
							m_lMinPlotLength=dp.x;
						if (dp.y>m_lMaxPGWidth)
							m_lMaxPGWidth=dp.y;
						else if (dp.y<m_lMinPlotWidth)
							m_lMinPlotWidth=dp.y;
					}
				}
				m_dpCurrent=dp;
			}
		}
		m_bPenDown=TRUE;
	}
	else if(strcmp(pInstruct->m_cName,"PU")==0)//提笔
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			if (m_bAbsolute)
			{
				dp.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
				dp.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y;
			}
			else
			{
				dp=m_dpCurrent;
				for(int i=0;i<nSize;)
				{
					dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX);
					dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY);
				}
			}
			m_dpCurrent=dp;
		}
		m_bPenDown=FALSE;
	}
	else if(strcmp(pInstruct->m_cName,"PR")==0)//相对绘图
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			dp=m_dpCurrent;
			if((m_bPenDown)&&(m_nPen!=0))
			{
				p=new CGLPrimitive;
				theApp.m_listPrimitive.AddTail(p);
				p->m_nType=PRIMITIVE_LINE;
				p->m_anData.Add(dp.x);
				p->m_anData.Add(dp.y);
				if (dp.x>m_lMaxPGLength)
					m_lMaxPGLength=dp.x;
				else if (dp.x<m_lMinPlotLength)
					m_lMinPlotLength=dp.x;
				if (dp.y>m_lMaxPGWidth)
					m_lMaxPGWidth=dp.y;
				else if (dp.y<m_lMinPlotWidth)
					m_lMinPlotWidth=dp.y;
				for(int i=0;i<nSize;)
				{
					dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX);
					dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY);
					p->m_anData.Add(dp.x);
					p->m_anData.Add(dp.y);
					if (dp.x>m_lMaxPGLength)
						m_lMaxPGLength=dp.x;
					else if (dp.x<m_lMinPlotLength)
						m_lMinPlotLength=dp.x;
					if (dp.y>m_lMaxPGWidth)
						m_lMaxPGWidth=dp.y;
					else if (dp.y<m_lMinPlotWidth)
						m_lMinPlotWidth=dp.y;
				}
				m_dpCurrent=dp;
			}
			else
			{
				for(int i=0;i<nSize;)
				{
					dp.x=dp.x+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX);
					dp.y=dp.y+ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY);
				}
				m_dpCurrent=dp;
			}
		}
		m_bAbsolute=FALSE;
	}
	else if(strcmp(pInstruct->m_cName,"PA")==0)//绝对绘图
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			nSize--;
			if((m_bPenDown)&&(m_nPen!=0))
			{
				dp=m_dpCurrent;
				p=new CGLPrimitive;
				theApp.m_listPrimitive.AddTail(p);
				p->m_nType=PRIMITIVE_LINE;
				p->m_anData.Add(dp.x);
				p->m_anData.Add(dp.y);
				if (dp.x>m_lMaxPGLength)
					m_lMaxPGLength=dp.x;
				else if (dp.x<m_lMinPlotLength)
					m_lMinPlotLength=dp.x;
				if (dp.y>m_lMaxPGWidth)
					m_lMaxPGWidth=dp.y;
				else if (dp.y<m_lMinPlotWidth)
					m_lMinPlotWidth=dp.y;
				for(int i=0;i<nSize;)
				{
					dp.x=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
					dp.y=ftoi(pInstruct->m_adPara.GetAt(i++)*m_dRatioY)+m_dpOrigin.y;
					p->m_anData.Add(dp.x);
					p->m_anData.Add(dp.y);
					if (dp.x>m_lMaxPGLength)
						m_lMaxPGLength=dp.x;
					else if (dp.x<m_lMinPlotLength)
						m_lMinPlotLength=dp.x;
					if (dp.y>m_lMaxPGWidth)
						m_lMaxPGWidth=dp.y;
					else if (dp.y<m_lMinPlotWidth)
						m_lMinPlotWidth=dp.y;
				}
				m_dpCurrent=dp;
			}
			else
			{
				m_dpCurrent.x=ftoi(pInstruct->m_adPara.GetAt(nSize-1)*m_dRatioX)+m_dpOrigin.x+m_lMaxPlotLength;
				m_dpCurrent.y=ftoi(pInstruct->m_adPara.GetAt(nSize)*m_dRatioY)+m_dpOrigin.y;
			}
		}
		m_bAbsolute=TRUE;
	}
	else if((strcmp(pInstruct->m_cName,"PG")==0)||(strcmp(pInstruct->m_cName,"FR")==0))//推进页面
	{
		m_lMaxPlotLength=m_lMaxPGLength;
		if (m_lMaxPlotWidth < m_lMaxPGWidth)
			m_lMaxPlotWidth = m_lMaxPGWidth;
	}
	else if(strcmp(pInstruct->m_cName,"LT")==0)//线型
	{
		nSize=pInstruct->m_adPara.GetSize();
		p=new CGLPrimitive;
		theApp.m_listPrimitive.AddTail(p);
		p->m_nType=PRIMITIVE_LT;
		for(i=0;i<nSize;i++)
			p->m_anData.Add(ftoi(pInstruct->m_adPara.GetAt(i)));
	}
	else if(strcmp(pInstruct->m_cName,"SP")==0)//选笔
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>MAaxPenNum)
		{
			m_nPen=ftoi(pInstruct->m_adPara.GetAt(0));
			m_nPen=((m_nPen-1)%MAaxPenNum-1)+1;
		}
		else if(nSize>0)
			m_nPen=ftoi(pInstruct->m_adPara.GetAt(0));
		else
			m_nPen=0;
//		if (m_nPen==0)
//			m_nPen++;
	}
	else if(strcmp(pInstruct->m_cName,"PE")==0)//PE
	{
		
	}
	else if(strcmp(pInstruct->m_cName,"LO")==0)//字符相对原点位置
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=1)
		{
			int n=ftoi(pInstruct->m_adPara.GetAt(0));
			if(n<1 || n>19 || n==10)
				n=1;
			m_nTextPosition=n;
		}
		else m_nTextPosition=1;
	}
	else if(strcmp(pInstruct->m_cName,"FN")==0)//字体 20121204
	{
		pText=new CGLText;
		theApp.m_listTextPrimitive.AddTail(pText);
		pText->m_nType=PRIMITIVE_FN;
		pText->m_cFontName = pInstruct->m_cFontName;
	}
	else if(strcmp(pInstruct->m_cName,"BT")==0)//位图20121204
	{
	//	pBitPtr=new BitPrimitive;	
	//	theApp.m_listBitDataPrimitive.AddTail(pBitPtr);
	//	pBitPtr->m_BitPost[0]=pInstruct->m_BitPost[0];
	//	pBitPtr->m_BitPost[1]=pInstruct->m_BitPost[1];
	//	pBitPtr->m_BitPost[2]=pInstruct->m_BitPost[2];
	//	pBitPtr->m_BitPost[3]=pInstruct->m_BitPost[3];
	//	pBitPtr->m_BitDispStyle[0]= pInstruct->m_BitDispStyle[0];
	//	pBitPtr->m_BitDispStyle[1]= pInstruct->m_BitDispStyle[2];
	//	pBitPtr->m_BitGDI[0]= pInstruct->m_BitGDI[0];
	//	pBitPtr->m_BitGDI[1]= pInstruct->m_BitGDI[1];
	//	pBitPtr->m_BitGDI[2]= pInstruct->m_BitGDI[2];
	//	pBitPtr->m_BitGDI[3]= pInstruct->m_BitGDI[3];
	//	pBitPtr->m_BitDATA= pInstruct->m_BitData;		
	}
	else if(strcmp(pInstruct->m_cName,"SI")==0)//字符大小
	{
		nSize=pInstruct->m_adPara.GetSize();
		if (theApp.m_nFont==0)
		{
			pText=new CGLText;
			theApp.m_listTextPrimitive.AddTail(pText);
			pText->m_nType=PRIMITIVE_SI;
			for(i=0;i<nSize;i++)
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(i));
		}
		if(nSize>=2)
		{
			m_dTextHeight=pInstruct->m_adPara.GetAt(1);
			m_dTextWidth=pInstruct->m_adPara.GetAt(0);
		}
		else 
		{
			m_dTextHeight=0.375;
			m_dTextWidth=0.285;
		}
	}
	else if(strcmp(pInstruct->m_cName,"DI")==0)//字符方向
	{
		nSize=pInstruct->m_adPara.GetSize();
		// 20121218 加密文件
		if(bIsEncrypt)
		{
			m_dTextAngle=pInstruct->m_adPara.GetAt(0)*0.1;
			if (theApp.m_nFont==0)
			{
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_DI;
				//角度
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(0)*0.1);
				pText->m_cIsItali = (unsigned char)pInstruct->m_adPara.GetAt(1);
				pText->m_iWeight = (int)pInstruct->m_adPara.GetAt(2);
			}
		}
		else{
		if(nSize>=2)
		{
			d1=pInstruct->m_adPara.GetAt(0);
			d2=pInstruct->m_adPara.GetAt(1);

			if(d1>1 || d1<-1.0)
				d1=cos(pInstruct->m_adPara.GetAt(0));
			if(d2>1 || d2<-1.0)
				d2=sin(pInstruct->m_adPara.GetAt(1));
			
			if(d1<0.10 && d1>-0.10)
				d1=0;
			if(d2<0.10 && d2>-0.10)
				d2=0;

			if(d1==0 && d2==1)
				m_dTextAngle=90;
			else if(d1==1 && d2==0)
				m_dTextAngle=0;
			else if(d1==0 && d2==-1)
				m_dTextAngle=270;
			else if(d1==-1 && d2==0)
				m_dTextAngle=-180;
			else
				m_dTextAngle=atan2(d2,d1)*180/3.14159265359;
			double tmp = (m_dTextAngle - (int)m_dTextAngle);
			if(abs((int)(tmp*10)) >= 5)
			{
				m_dTextAngle = m_dTextAngle < 0 ? m_dTextAngle += -1 : m_dTextAngle+=1;
			}
			m_dTextAngle = (int)m_dTextAngle;
		}
		else
			m_dTextAngle=0;
		if (theApp.m_nFont==0)
		{
			pText=new CGLText;
			theApp.m_listTextPrimitive.AddTail(pText);
			pText->m_nType=PRIMITIVE_DI;
			for(i=0;i<nSize;i++)
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(i));//*0.1
		}
		}
	}
	else if(strcmp(pInstruct->m_cName,"DR")==0)//字符方向
	{
		nSize=pInstruct->m_adPara.GetSize();
		if((nSize==0)||((nSize==2)&&((pInstruct->m_adPara.GetAt(0)==0)||(pInstruct->m_adPara.GetAt(1)==0))))
		{
			if(nSize==0)
				m_dTextAngle=0;
			else if(nSize==2)
			{
				d1=pInstruct->m_adPara.GetAt(0);
				d2=pInstruct->m_adPara.GetAt(1);
				if((d1==0)&&(d2!=0))
					m_dTextAngle=90;
				else if((d1!=0)&&(d2==0))
					m_dTextAngle=0;
			}
			if (theApp.m_nFont==0)
			{
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_DR;
				for(i=0;i<nSize;i++)
					pText->m_adPara.Add(pInstruct->m_adPara.GetAt(i));
			}
		}
		else if((nSize==2)&&(m_bIPValid))
		{
			d1=pInstruct->m_adPara.GetAt(0);
			d2=pInstruct->m_adPara.GetAt(1);
			if((d1==0)&&(d2!=0))
				m_dTextAngle=90;
			else if((d1!=0)&&(d2==0))
				m_dTextAngle=0;
			else if((d1!=0)&&(d2!=0)&&(fabs(d1)<=100)&&(fabs(d2)<=100))
			{
				d1=d1*(m_dpP2.x-m_dpP1.x);
				d2=d2*(m_dpP2.y-m_dpP1.y);
				m_dTextAngle=atan2(d1,d2);
			}
			if (theApp.m_nFont==0)
			{
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_DR;
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(0));
				pText->m_adPara.Add(pInstruct->m_adPara.GetAt(1));
				pText->m_adPara.Add(m_dpP2.x-m_dpP1.x);
				pText->m_adPara.Add(m_dpP2.y-m_dpP1.y);
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"LB")==0)//字符
	{	
		nSize=m_strText.GetLength();
		int j=0;
		unsigned char str1[257];
		CPoint dp0;
		///////20121218
		if(bIsEncrypt)
		{
			dp.x=ftoi(pInstruct->m_adPara.GetAt(0)*m_dRatioX);
			dp.y=ftoi(pInstruct->m_adPara.GetAt(1)*m_dRatioY);
			m_dpCurrent=dp;
		}
		else
			dp=m_dpCurrent;
		/////////////
		while(nSize>0)
		{
			if(nSize>256)
				nSize=256;
			for(i=0;i<nSize;i++)
			{
				if(m_strText[j]==0x0d)
				{
					str1[i]='\0';
					if(i!=0)
					{
						pText=new CGLText;
						pText->m_nType=PRIMITIVE_LB;
						pText->m_adPara.Add(dp.x);
						pText->m_adPara.Add(dp.y);
						pText->m_strText=str1;
						GLTextMinXMaxX(pText);
						if (theApp.m_nFont==0)
							theApp.m_listTextPrimitive.AddTail(pText);
						else
						{
							dp0.x=ftoi(pText->m_adPara.GetAt(0));
							dp0.y=ftoi(pText->m_adPara.GetAt(1));
							TextChangeLine(dp0,(LPCTSTR)str1,i);
							pText->m_adPara.RemoveAll();
							delete pText;
						}
					}
					j++;
					dp.x=m_dpCurrent.x;
					break;
				}
				else if(m_strText[j]==0x0a)
				{
					str1[i]='\0';
					if(i!=0)
					{
						pText=new CGLText;
						pText->m_nType=PRIMITIVE_LB;
						pText->m_adPara.Add(dp.x);
						pText->m_adPara.Add(dp.y);
						pText->m_strText=str1;
						GLTextMinXMaxX(pText);
						if (theApp.m_nFont==0)
							theApp.m_listTextPrimitive.AddTail(pText);
						else
						{
							dp0.x=ftoi(pText->m_adPara.GetAt(0));
							dp0.y=ftoi(pText->m_adPara.GetAt(1));
							TextChangeLine(dp0,(LPCTSTR)str1,i);
							pText->m_adPara.RemoveAll();
							delete pText;
						}
					}
					j++;
					dp.y+=ftoi(m_dTextHeight*400);
					break;
				}
				else
					str1[i]=m_strText[j];
				j++;
			}
			if((i==nSize)&&(i!=0))
			{
				str1[i]='\0';
				pText=new CGLText;
				pText->m_nType=PRIMITIVE_LB;
				pText->m_adPara.Add(dp.x);
				pText->m_adPara.Add(dp.y);
				pText->m_strText=str1;
				GLTextMinXMaxX(pText);
				if (theApp.m_nFont==0)
					theApp.m_listTextPrimitive.AddTail(pText);
				else
				{
					dp0.x=ftoi(pText->m_adPara.GetAt(0));
					dp0.y=ftoi(pText->m_adPara.GetAt(1));
					TextChangeLine(dp0,(LPCTSTR)str1,i);
					pText->m_adPara.RemoveAll();
					delete pText;
				}
			}
			else
				i++;
			nSize-=i;
		}
	}
	else if(strcmp(pInstruct->m_cName,"DF")==0)//缺省
	{
		m_nLineType=LINE_TYPE_SOLID;
		m_bAbsolute=TRUE;
		m_cTerminator=theApp.m_cTerminator;//标志终结符
		m_cTerminatorMode=1;
		m_bSCValid=false;
		m_dpOrigin=CPoint(0,0);
		m_dRatioX=1;
		m_dRatioY=1;
		m_dTextHeight=0.375;//cm
		m_dTextWidth=0.285;//cm
		m_dTextAngle=0;
		m_nTextPosition=1;
		if(theApp.m_listPrimitive.GetCount()!=0)
		{
			p=new CGLPrimitive;
			theApp.m_listPrimitive.AddTail(p);
			p->m_nType=PRIMITIVE_LT;
			if (theApp.m_nFont==0)
			{
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_SI;
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_DI;
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"IN")==0)//初始化
	{
		m_dpCurrent=CPoint(0,0);
		m_bPenDown=FALSE;
		m_nRotate=0;
		m_dpP1=CPoint(m_rtHardLimitation.left,m_rtHardLimitation.top);
		m_dpP2=CPoint(m_rtHardLimitation.right,m_rtHardLimitation.bottom);
		m_dpOrigin=CPoint(0,0);
		m_dRatioX=1;
		m_dRatioY=1;
		m_nLineType=LINE_TYPE_SOLID;
		m_bAbsolute=TRUE;
		m_cTerminator=theApp.m_cTerminator;//标志终结符
		m_cTerminatorMode=1;
		m_dTextHeight=0.375;//cm
		m_dTextWidth=0.285;//cm
		m_dTextAngle=0;
		m_nTextPosition=1;
		m_bIPValid=m_bPSValid;
		m_bSCValid=false;
		if(theApp.m_listPrimitive.GetCount()!=0)
		{
			p=new CGLPrimitive;
			theApp.m_listPrimitive.AddTail(p);
			p->m_nType=PRIMITIVE_LT;
			if (theApp.m_nFont==0)
			{
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_SI;
				pText=new CGLText;
				theApp.m_listTextPrimitive.AddTail(pText);
				pText->m_nType=PRIMITIVE_DI;
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"PS")==0)//绘图尺寸
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=2)
		{
			if(theApp.m_listPrimitive.GetCount()==0)
			{
				dp.x=ftoi(pInstruct->m_adPara.GetAt(0));
				dp.y=ftoi(pInstruct->m_adPara.GetAt(1));
				if((dp.x>0)&&(dp.y>0))
				{
					m_rtHardLimitation.left=0;
					m_rtHardLimitation.top=0;
					m_rtHardLimitation.right=dp.x;
					m_rtHardLimitation.bottom=dp.y;
					m_dpP1.x=0;
					m_dpP1.y=0;
					m_dpP2=dp;
					m_bPSValid=true;
					m_bIPValid=true;
				}
			}
		}
	}
	else if(strcmp(pInstruct->m_cName,"IP")==0)//缩放点
	{
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=4)
		{
			m_dpP1=CPoint(ftoi(pInstruct->m_adPara.GetAt(0)),ftoi(pInstruct->m_adPara.GetAt(1)));
			m_dpP2=CPoint(ftoi(pInstruct->m_adPara.GetAt(2)),ftoi(pInstruct->m_adPara.GetAt(3)));
			m_bIPValid=true;
		}
		else if(nSize>=2)
		{
			int xd,yd;
			xd=m_dpP2.x-m_dpP1.x;
			yd=m_dpP2.y-m_dpP1.y;
			m_dpP1=CPoint(ftoi(pInstruct->m_adPara.GetAt(0)),ftoi(pInstruct->m_adPara.GetAt(1)));
			m_dpP2=m_dpP1+CPoint(xd,yd);
			if(m_bIPValid || m_bPSValid)
				m_bIPValid=true;
		}
		else
		{
			m_dpP1=CPoint(m_rtHardLimitation.left,m_rtHardLimitation.top);
			m_dpP2=CPoint(m_rtHardLimitation.right,m_rtHardLimitation.bottom);
			m_bIPValid=m_bPSValid;
		}
		SetRatio();
	}
	else if(strcmp(pInstruct->m_cName,"IR")==0)//相对缩放点
	{
		int xd,yd;
		nSize=pInstruct->m_adPara.GetSize();
		if(nSize>=4)
		{
			xd=m_rtHardLimitation.Width();
			yd=m_rtHardLimitation.Height();
			m_dpP1=CPoint(ftoi(xd*pInstruct->m_adPara.GetAt(0)),ftoi(yd*pInstruct->m_adPara.GetAt(1)));
			m_dpP2=CPoint(ftoi(xd*pInstruct->m_adPara.GetAt(2)),ftoi(yd*pInstruct->m_adPara.GetAt(3)));
		}
		else if(nSize>=2)
		{
			xd=m_dpP2.x-m_dpP1.x;
			yd=m_dpP2.y-m_dpP1.y;
			dp=CPoint(xd,yd);
			xd=m_rtHardLimitation.Width();
			yd=m_rtHardLimitation.Height();
			m_dpP1=CPoint(ftoi(xd*pInstruct->m_adPara.GetAt(0)),ftoi(yd*pInstruct->m_adPara.GetAt(1)));
			m_dpP2=m_dpP1+dp;
		}
		else
		{
			m_dpP1=CPoint(m_rtHardLimitation.left,m_rtHardLimitation.top);
			m_dpP2=CPoint(m_rtHardLimitation.right,m_rtHardLimitation.bottom);
		}
		m_bIPValid=m_bPSValid;
		SetRatio();
	}
	else if(strcmp(pInstruct->m_cName,"SC")==0)//缩放
	{
		int n;
		nSize=pInstruct->m_adPara.GetSize();
		if((nSize>=4)&&(nSize!=6))
		{
			if(nSize==4)
				n=0;
			else
				n=ftoi(pInstruct->m_adPara.GetAt(4));
			if(n!=2)
			{
				if(pInstruct->m_adPara.GetAt(0)!=pInstruct->m_adPara.GetAt(1) &&
					pInstruct->m_adPara.GetAt(2)!=pInstruct->m_adPara.GetAt(3) )
				{
					m_struSC.dXMin=pInstruct->m_adPara.GetAt(0);
					m_struSC.dXMax=pInstruct->m_adPara.GetAt(1);
					m_struSC.dYMin=pInstruct->m_adPara.GetAt(2);
					m_struSC.dYMax=pInstruct->m_adPara.GetAt(3);
					m_struSC.nType=n;
					if(n==1)
					{
						if(nSize>5)
						{
							m_struSC.dLeft=pInstruct->m_adPara.GetAt(5);
							m_struSC.dBottom=pInstruct->m_adPara.GetAt(6);
						}
						else
						{
							m_struSC.dLeft=50;
							m_struSC.dBottom=50;
						}
					}
					m_bSCValid=true;
					SetRatio();
				}
			}
			else
			{
				if(pInstruct->m_adPara.GetAt(1)!=0 && pInstruct->m_adPara.GetAt(3)!=0)
				{
					m_struSC.dXMin=pInstruct->m_adPara.GetAt(0);
					m_struSC.dXMax=pInstruct->m_adPara.GetAt(1);
					m_struSC.dYMin=pInstruct->m_adPara.GetAt(2);
					m_struSC.dYMax=pInstruct->m_adPara.GetAt(3);
					m_struSC.nType=n;
					m_bSCValid=true;
					SetRatio();
				}
			}
		}
	}
}



int CPictureDlg::OpenSecretFileNew(CString strFileName)//20160201切割指令识别
{	
	CFile file;
	//20100406新增CFile::shareExclusive属性，避免其他CAD文件写自动绘图目录时写入不成功或打印缺页
	if(!file.Open(strFileName,CFile::modeRead | CFile::shareExclusive))
		return 1;

	int nLength=file.GetLength();
	BOOL bError=FALSE;
	CInstruct Instruct;
	int nProcess;
	unsigned char cParameterType;
	char cFileBuf[1025];
	char cSecretBuf[256];
	int i,j,k,m,n;
	int nRemainChar;
	unsigned char ch,*cPin;
	int nParameter;
	
	InitHPGL();
	InitTextChangeLine();
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
	nProcess=PROCESS_MNEMONIC;
	m=j=0;
	nRemainChar = 0;
	//判断文件是否为加密文件
	file.Read(&cFileBuf[0],22);
	i = 22;
	for(;i<nLength;)
	{
		k = 0;
		if (m < j)
		{
			while(k<(j-m))
			{
				cFileBuf[k] = cFileBuf[m+k];
				k++;
			}
			j = 1024 - k;
		}
		else
			j = 1024;
		if (nRemainChar > 0)
		{
			n = 252 - nRemainChar + 2;
			if (j > nRemainChar)
			{
				m = 254;
				j -= nRemainChar;
				i += nRemainChar;
			}
			else
			{
				m = n + j;
				i += j;
				j = 0;
			}
			if ((n < m) && ((nRemainChar & 0x01) != 0))
			{
				nRemainChar -= (m - n);
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
			}
			else
				nRemainChar -= (m - n);
			for (;n<m;)
			{
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
			}	
		}
		while (j > 0)
		{
			if (nLength == i)
				break;
			if ((nLength - i) < 256)
			{
				bError = TRUE;
				break;
			}
			file.Read(cSecretBuf,256);
			i += 256;
			cSecretBuf[0] ^= cSecretBuf[254];
			cSecretBuf[1] ^= cSecretBuf[255];
			cSecretBuf[0] ^= 0xac;
			cSecretBuf[1] ^= 0xe3;
			if (j < 252)
			{
				m = j + 2;
				nRemainChar = 252 - j;
				j = 0;
				i -= nRemainChar;
			}
			else
			{
				m = 254;
				j -= 252;
				nRemainChar = 0;
			}
			if ((m & 0x01) != 0)
			{
				m--;
				for (n=2;n<m;)
				{
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
				}
				cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
			}
			else
			{
				for (n=2;n<m;)
				{
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[0];
					cFileBuf[k++] = cSecretBuf[n++] ^ cSecretBuf[1];
				}
			}
		}
		j = k;
		k = 0;
		m = j;
		if (bError)
			break;
		while (k < j)
		{
			if (nProcess == PROCESS_MNEMONIC)//助记符
			{
				ch = cFileBuf[k++];
				switch (ch)
				{
				case 0x8a:
				case 0x97:
					if (cParameterType == 0x5e)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					else if (cParameterType == 0x3b)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					if (ch == 0x8a)
					{
						Instruct.m_cName[0]='I';
						Instruct.m_cName[1]='N';
					}
					else
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='G';
					}
					ProcessInstruct(&Instruct);
					break;
				case 0x7c:
				case 0xe6:
				case 0xb5:
				case 0x3d:
					if (cParameterType == 0x5e)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					else if (cParameterType == 0x3b)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;
				case 0x5e:
					if (cParameterType == 0x3b)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;
				case 0x3b:
					if (cParameterType == 0x5e)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					nProcess=PROCESS_PARAMETER;
					break;
				case 0x00:
					if (cParameterType == 0x5e)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='U';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					else if (cParameterType == 0x3b)
					{
						Instruct.m_cName[0]='P';
						Instruct.m_cName[1]='D';
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
					}
					break;
				default:
					bError=TRUE;
					break;
				}
				cParameterType = ch;
			}
			else
			{
				switch (cParameterType)
				{
				case 0x7c:
					ch = cFileBuf[k++];
					Instruct.m_cName[0]='S';
					Instruct.m_cName[1]='P';
					Instruct.m_adPara.Add((double)ch);
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					break;
				case 0xe6:
					ch = cFileBuf[k++];
					Instruct.m_cName[0]='L';
					Instruct.m_cName[1]='T';
					ch ^= 0xce;
					if (ch != 0)
					{
						if (ch == 1)
							ch = 3;
						else if (ch == 2)
							ch = 1;
						else
							ch = 8;
						Instruct.m_adPara.Add((double)ch);
					}
					ProcessInstruct(&Instruct);
					Instruct.m_adPara.RemoveAll();
					nProcess=PROCESS_MNEMONIC;
					break;
				case 0xb5:
				case 0x3d:
					if ((j - k) < 8)
					{
						m = k;
						k = j;
					}
					else
					{
						if (cParameterType == 0xb5)
						{
							Instruct.m_cName[0]='P';
							Instruct.m_cName[1]='U';
						}
						else
						{
							Instruct.m_cName[0]='P';
							Instruct.m_cName[1]='D';
						}
						cPin = (unsigned char *)&nParameter;
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						ProcessInstruct(&Instruct);
						Instruct.m_adPara.RemoveAll();
						nProcess=PROCESS_MNEMONIC;
					}
					break;
				case 0x5e:
				case 0x3b:
					if ((j - k) < 8)
					{
						m = k;
						k = j;
					}
					else
					{
						cPin = (unsigned char *)&nParameter;
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						*(cPin+3) = cFileBuf[k++];
						*(cPin+2) = cFileBuf[k++];
						*(cPin+1) = cFileBuf[k++];
						*cPin = cFileBuf[k++];
						Instruct.m_adPara.Add(nParameter);
						nProcess=PROCESS_MNEMONIC;
					}
					break;
				default:
					break;
				}
			}
		}
		if(bError)
			break;
	}
	if(!bError)
	{
		if (cParameterType == 0x5e)
		{
			Instruct.m_cName[0]='P';
			Instruct.m_cName[1]='U';
			ProcessInstruct(&Instruct);
			Instruct.m_adPara.RemoveAll();
		}
		else if (cParameterType == 0x3b)
		{
			Instruct.m_cName[0]='P';
			Instruct.m_cName[1]='D';
			ProcessInstruct(&Instruct);
			Instruct.m_adPara.RemoveAll();
		}
	}
	Instruct.m_adPara.RemoveAll();
	Instruct.m_cName[2]=0;
	file.Close();
	if(bError)
		return 2;
	m_lMaxPlotLength=m_lMaxPGLength-m_lMinPlotLength;
	if (m_lMaxPlotWidth < m_lMaxPGWidth)
		m_lMaxPlotWidth = m_lMaxPGWidth;
	m_lMaxPlotWidth -= m_lMinPlotWidth;
	if (theApp.m_nRotate == 1)
	{
		long lMaxPlotLength=m_lMaxPlotLength;
		m_lMaxPlotLength=m_lMaxPlotWidth;
		m_lMaxPlotWidth=lMaxPlotLength;
	}
	return 0;
}



bool	CPictureDlg::GetDirectionX(int escapement, double & x, double & y)//20160122切割指令识别
{
	if (escapement == 0)
	{
		x = 1;
		y = 0;
	}
	else if (escapement == 900)
	{
		x = 0;
		y = 1;
	}
	else if (escapement == 1800)
	{
		x = -1;
		y = 0;
	}
	else if (escapement == 2700)
	{
		x = 0;
		y = -1;
	}
	else
	{
		double	result;
		result = escapement;

		result = tan(result * 3.14159265 / 1800);
		x = 1;
		y = result;
	}

	return true;
}


bool	CPictureDlg::GetOnLinePt(CMyPoint * ptBeg, CMyPoint * ptEnd, long value, long & result, bool valX)//20160122切割指令识别
{
	double	temp;
	long	min;
	long	max;

	if (valX)
	{
		if (ptEnd->x > ptBeg->x && value > ptEnd->x && value < ptBeg->x
			|| ptBeg->x > ptEnd->x && value > ptBeg->x && value < ptEnd->x)
			return false;

		if (ptBeg->y == ptEnd->y)
		{
			if ((value <= ptEnd->x && value >= ptBeg->x)
				|| (value <= ptBeg->x && value >= ptEnd->x))
			{
				result = ptBeg->y;
				return true;
			}
			return false;
		}

		if (ptBeg->x == ptEnd->x)		// 竖线
		{
			return false;
		}

		if ((value < ptEnd->x && value > ptBeg->x)
			|| (value < ptBeg->x && value > ptEnd->x))
		{
			temp = ptEnd->y - ptBeg->y;
			temp *= (value - ptBeg->x);
			temp /= (ptEnd->x - ptBeg->x);
			temp += ptBeg->y;
			result = (long)(temp + 0.5);
			min = ptEnd->y < ptBeg->y ? ptEnd->y : ptBeg->y;
			max = ptEnd->y > ptBeg->y ? ptEnd->y : ptBeg->y;
			if (result > max)
				result = max;
			if (result < min)
				result = min;
			return true;
		}
		else if (value == ptEnd->x)
		{
			result = ptEnd->y;
			return true;
		}
		else if (value == ptBeg->x)
		{
			result = ptBeg->y;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (ptEnd->y > ptBeg->y && value > ptEnd->y && value < ptBeg->y
			|| ptBeg->y > ptEnd->y && value > ptBeg->y && value < ptEnd->y)
			return false;

		if (ptBeg->x == ptEnd->x)
		{
			if ((value <= ptEnd->y && value >= ptBeg->y)
				|| (value <= ptBeg->y && value >= ptEnd->y))
			{
				result = ptBeg->x;
				return true;
			}
			return false;
		}

		if (ptBeg->y == ptEnd->y)		// 横线
		{
			return false;
		}

		if ((value < ptEnd->y && value > ptBeg->y)
			|| (value < ptBeg->y && value > ptEnd->y))
		{
			temp = ptEnd->x - ptBeg->x;
			temp *= (value - ptBeg->y);
			temp /= (ptEnd->y - ptBeg->y);
			temp += ptBeg->x;
			result = (long)(temp + 0.5);
			min = ptEnd->x < ptBeg->x ? ptEnd->x : ptBeg->x;
			max = ptEnd->x > ptBeg->x ? ptEnd->x : ptBeg->x;
			if (result > max)
				result = max;
			if (result < min)
				result = min;

			return true;
		}
		else if (value == ptEnd->y)
		{
			result = ptEnd->x;
			return true;
		}
		else if (value == ptBeg->y)
		{
			result = ptBeg->x;
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}


POINT	CPictureDlg::LogPointToScr(CMyPoint * pit)//20160120切割指令识别
{
	POINT	point;
    m_maxY=m_lTotalY;
    
	if(nLarge1 == 0)
	{
		m_yDirection = true;

	   if (m_yDirection)
	   {
		  point.y = long ((m_maxY - pit->y) * m_dProportion + m_ptStartPos.y  + 0.5);
	   }
       else
	   {
		  point.y = long (pit->y * m_dProportion + m_ptStartPos.y  + 0.5);
	   }
	   point.x = long (pit->x * m_dProportion + m_ptStartPos.x  + 0.5);	
	}


	if(nLarge1 == 1)
	{
	  m_yDirection = true;

	   if (m_yDirection)
	   {
		  point.y = long ((m_maxY - pit->y) * m_dProportion + m_ptStartPos.y  + 0.5);
	   }
       else
	   {
		  point.y = long (pit->y * m_dProportion + m_ptStartPos.y  + 0.5);
	   }
	      point.x = long (pit->x * m_dProportion + m_ptStartPos.x  + 0.5);	
	}

 	return point;
}

BOOL CPictureDlg::IsDigit(char ch)//20160114切割指令识别
{
	if(ch>='0' && ch<='9') return TRUE;
	if(ch=='.') return TRUE;
	if(ch=='-' ||ch=='+') return TRUE;
	return FALSE;
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
			m_ptStartPos.y = 5;
		}
		else
		{
			m_ptStartPos.y = 5;
			m_ptStartPos.x = 5;
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

	//gzr20160908 start
// 	CDC memCutDC;
// 	CBitmap bitmapCut;
// 	CBitmap* pOldBitmapCut;
// 	memCutDC.CreateCompatibleDC(pDC);
// 	CPen penCut(LINE_TYPE_SOLID,1,RGB(0xff,0xd7,0x00));
// 	CPen *pOldPenCut=memCutDC.SelectObject(&penCut);
// 	//memCutDC.SetBkMode(TRANSPARENT);
// 	bitmapCut.CreateCompatibleBitmap(pDC, rectClient.right+2, rectClient.bottom+2);
//  pOldBitmapCut=memCutDC.SelectObject(&bitmapCut);
	//gzr20160908 end



	int nLineType =LINE_TYPE_SOLID;
	int nLineType1=LINE_TYPE_SOLID;
	long lMinLength = m_lFileLength + 10 * 40;
	long lMaxLength;
	CPoint pt,pt1,pt2;
	int nSize;
	POSITION pos=theApp.m_listPrimitive.GetHeadPosition();
	CPen pen(nLineType1,1,RGB(0,0,0));
	CPen *pOldPen=memDC.SelectObject(&pen);
	memDC.Rectangle(0,0,rectClient.Width()+2,rectClient.Height()+2);
	//gzr20160908 start
	while(pos)
	{
		CGLPrimitive *p=(CGLPrimitive*)theApp.m_listPrimitive.GetNext(pos);
		switch(p->m_nType)
		{
		case PRIMITIVE_LINE:
			pen.DeleteObject();
			pen.CreatePen(nLineType1,1,RGB(0,0,0));//0,0,0
			memDC.SelectObject(&pen);
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

	pos=theApp.m_listPrimitive.GetHeadPosition();
	while(pos)
	{
		CGLPrimitive *p=(CGLPrimitive*)theApp.m_listPrimitive.GetNext(pos);
		switch(p->m_nType)
		{
			//20150729 切割线换色
		case PRIMITIVE_CUTLINE:
			//gzr20160908 start
 			pen.DeleteObject();
 			pen.CreatePen(nLineType1,1,RGB(0xff,0xd7,0x00));//0,0,0,0xff
 			memDC.SelectObject(&pen);

// 			penCut.DeleteObject();
// 			penCut.CreatePen(nLineType1,1,RGB(0xff,0xd7,0x00));//0,0,0,0xff
// 			memCutDC.SelectObject(&penCut);

			//gzr20160908 end
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
						//gzr20160908 start
						memDC.MoveTo(pt);
						//memCutDC.MoveTo(pt);
						//gzr20160908 end
						pt.x=pt2.x;
						pt.y=m_lTotalY-pt2.y;
						DocToClient(pt);
						//gzr20160908 start
						memDC.LineTo(pt);
						//memCutDC.LineTo(pt);
						//gzr20160908 end
						pt.y=0;
						pt.x=pt2.x+lMinLength;
						pt1.y-=m_lTotalY;
						pt1.x+=lMinLength;
					}
					pt.y=m_lTotalY-pt.y;
					DocToClient(pt);
					//gzr20160908 start
					memDC.MoveTo(pt);
					//memCutDC.MoveTo(pt);
					//gzr20160908 start
					pt1.y=m_lTotalY-pt1.y;
					DocToClient(pt1);
					//gzr20160908 start
					memDC.LineTo(pt1);
					//memCutDC.LineTo(pt1);
					//gzr20160908 start
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
		}
	}
	//gzr20160908 end
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
				//gzr20160907 start
				lf = theApp.m_config.m_lfDrawText;
// 				lf.lfCharSet = 222;		
// 				strcpy(lf.lfFaceName,"HP Roman 8");
//gzr20160907 end

				lf.lfHeight=ftoi(dTextHeight*400*m_dProportion);
				if(abs(lf.lfHeight)<1)
					lf.lfHeight=1;
				lf.lfWidth = 0;
				//lf.lfHeight = 20;
#if 0
				lf.lfWidth=ftoi(dTextWidth*400*m_dProportion);
				if(abs(lf.lfWidth)<1)
					lf.lfWidth=1;
#endif
				//gzr20160907 start
				//lf.lfWeight=FW_THIN;
				//gzr20160907 end
				lf.lfOrientation=ftoi(dTextAngle*10);
				lf.lfEscapement=ftoi(dTextAngle*10);
				CFont fontNew;
				fontNew.CreateFontIndirect(&lf);
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
				
 					if((d1==0)&&(d2!=0))
					{
						//gzr20161101文字选转角度
						if (d2 > 0)
							dTextAngle=90;
						else
							dTextAngle=-90;
					}
 					else if((d1!=0)&&(d2==0))
					{
						if (d1 > 0)
						{
							dTextAngle=0;
						} 
						else
						{
							dTextAngle=180;
						}
					}
 					else if((d1!=0)&&(d2!=0))
					{
						//gzr20161101文字选转角度
 						dTextAngle=atan2(d2,d1) * 180 / PAI;
					}
 					else
 					{
 						continue;
 					}
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
	//memDC.SetBkColor(RGB(255,255,255));
	//memCutDC.SetBkColor(RGB(255,255,255));
	pDC->BitBlt(0, 0, rectClient.right+2, rectClient.bottom+2, &memDC, 0, 0, SRCCOPY);
	// pDC->BitBlt(0, 0, rectClient.right+2, rectClient.bottom+2, &memCutDC, 0, 0, SRCCOPY);
// 	pDC->BitBlt(0, 0, rectClient.right+2, rectClient.bottom+2, &memDC, 0, 0, SRCINVERT);
// 	pDC->BitBlt(0, 0, rectClient.right+2, rectClient.bottom+2, &memDC, 0, 0, SRCAND);
// 	pDC->BitBlt(0, 0, rectClient.right+2, rectClient.bottom+2, &memDC, 0, 0, SRCINVERT);


	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	//memCutDC.SetBkMode(TRANSPARENT);SRCCOPY
	
// 	memCutDC.SelectObject(pOldPenCut);
// 	penCut.DeleteObject();
// 	
// 	memCutDC.SelectObject(pOldBitmapCut);
// 	memCutDC.DeleteDC();
}
//鼠标点选修改图形类型（切割或打印） gzr20160908
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

	//20160127切割指令识别
	if ((nFlags & MK_CONTROL) != 0)
	{
		ChangePen(point, 4);
	}

	else if ((nFlags & MK_SHIFT) != 0)
	{
		ChangePen(point, 1);
	}
//20160127切割指令识别
	
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
				m_strTotalX.Format("%ldmm",ftoi(lPointX*0.025));
				m_strTotalY.Format("%ldmm",ftoi(lPointY*0.025));
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

	m_multiple=0;//20160122切割指令识别
	//posx = -5;
	//posy = -5;
    nLarge1=0;

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
			m_strTotalX.Format("%ldmm",ftoi(lPointX*0.025));
			m_strTotalY.Format("%ldmm",ftoi(lPointY*0.025));
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
