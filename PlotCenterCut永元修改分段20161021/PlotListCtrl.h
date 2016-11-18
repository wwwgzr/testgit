#if !defined(AFX_PLOTLISTCTRL_H__0DDE6756_C624_46E4_86B2_6DD4A5549AEF__INCLUDED_)
#define AFX_PLOTLISTCTRL_H__0DDE6756_C624_46E4_86B2_6DD4A5549AEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlotListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlotListCtrl window

class CPlotListCtrl : public CListCtrl
{
// Construction
public:
	CPlotListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPlotListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlotListCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTLISTCTRL_H__0DDE6756_C624_46E4_86B2_6DD4A5549AEF__INCLUDED_)
