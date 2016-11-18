#if !defined(AFX_VIEWHISTORY_H__44E979AE_0334_401A_816E_0DBEF06F8399__INCLUDED_)
#define AFX_VIEWHISTORY_H__44E979AE_0334_401A_816E_0DBEF06F8399__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewHistory.h : header file
//
#include "PlotListCtrl.h"
#include "PlotCenterDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CViewHistory dialog

class CViewHistory : public CDialog
{
// Construction
public:
	CViewHistory(CWnd* pParent = NULL);   // standard constructor
	CPlotCenterDlg *m_pParent;

	typedef struct _dlgControlTag
	{
		int iId;
		int iFlag;
		int iPercent;
	} DLGCTLINFO, *PDLGCTLINFO;

	enum
	{
		MOVEX=0,
		MOVEY,
		MOVEXY,
		ELASTICX,
		ELASTICY,
		ELASTICXY
	};

	// 设置控件信息
	BOOL SetControlProperty(PDLGCTLINFO lp, int nElements);

	// 是否在对话框右下角显示表示可改变大小的图标
	void ShowSizeIcon(BOOL bShow = TRUE);

// Dialog Data
	//{{AFX_DATA(CViewHistory)
	enum { IDD = IDD_VIEWHISTORY };
	CPlotListCtrl	m_wndHistory;
	//}}AFX_DATA
public:
	void DisplayRecord(void);
	int m_nLanguage;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewHistory)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewHistory)
	virtual BOOL OnInitDialog();
	afx_msg void OnClear();
	afx_msg void OnDraw();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int m_iClientWidth;		// 对话框client区域的宽度
	int m_iClientHeight;	// 对话框client区域的高度
	int m_iMinWidth;			// 对话框的最小宽度
	int m_iMinHeight;			// 对话框的最小高度
	PDLGCTLINFO m_pControlArray; // 控件信息数组指针
	int m_iControlNumber;	// 设置控件信息的控件个数
	BOOL m_bShowSizeIcon;	// 是否显示表示可改变大小的图标
	CStatic m_wndSizeIcon;// 放图标的静态控件
	// 保存图标的bitmap
	CBitmap m_bmpSizeIcon;
	BITMAP m_bitmap;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWHISTORY_H__44E979AE_0334_401A_816E_0DBEF06F8399__INCLUDED_)
