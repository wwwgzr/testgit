#if !defined(AFX_LINE_H__2597D202_848C_4BC2_8D09_89FE28AAD8F7__INCLUDED_)
#define AFX_LINE_H__2597D202_848C_4BC2_8D09_89FE28AAD8F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Line.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CLine command target

class CLine : public CCmdTarget
{
	DECLARE_DYNCREATE(CLine)

	CLine();           // protected constructor used by dynamic creation

// Attributes
public:
	int m_nCount;
	POINT *m_pPoint;
	int m_nPen;
	int m_nLineType;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLine)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLine();

	// Generated message map functions
	//{{AFX_MSG(CLine)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINE_H__2597D202_848C_4BC2_8D09_89FE28AAD8F7__INCLUDED_)
