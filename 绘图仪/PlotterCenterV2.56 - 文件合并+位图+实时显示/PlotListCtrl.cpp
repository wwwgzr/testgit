// PlotListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "PlotListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlotListCtrl

CPlotListCtrl::CPlotListCtrl()
{
}

CPlotListCtrl::~CPlotListCtrl()
{
}


BEGIN_MESSAGE_MAP(CPlotListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CPlotListCtrl)
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotListCtrl message handlers

void CPlotListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	if (nChar == VK_DELETE)
  {
    AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_DELETE,0);
  }
}

void CPlotListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CListCtrl::OnRButtonDown(nFlags, point);
}
