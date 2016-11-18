#if !defined(AFX_PICTUREDLG_H__8BC8E9D0_C3DF_40FF_B137_7CD6ADA597F2__INCLUDED_)
#define AFX_PICTUREDLG_H__8BC8E9D0_C3DF_40FF_B137_7CD6ADA597F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PictureDlg.h : header file
//
#include "plotCenterDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CPictureDlg dialog
class CPictureDlg : public CDialog
{
// Construction
public:
	CPictureDlg(CWnd* pParent = NULL);   // standard constructor
//	CPlotCenterDlg *m_pParent;

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
	//{{AFX_DATA(CPictureDlg)
	enum { IDD = IDD_PICTURE };
	CString	m_strTotalX;
	CString	m_strTotalY;
	long	m_lFixedPointLength;
	int		m_nCopyNum;
	//}}AFX_DATA
public:
	void DrawPicture(int nLarge);
	void DocToClient(CPoint &pt);
	long m_lFileLength;
	long m_lFileWidth;
	long m_lFileMinLength;
	long m_lFileMinWidth;
	long m_lTotalX;
	long m_lTotalY;
	double m_dProportion;
	CString m_strPictureName;
	BOOL m_bLButtonDown;
	CPoint m_ptMouseDownPos;
	CPoint m_ptMouseUpPos;
	CPoint m_ptStartPos;
	int m_nLanguage;
	CRect m_rtDrawPicture;

	BOOL m_busyPrint;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPictureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPictureDlg)
	afx_msg void OnTimer();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDraw();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChangeFixedPoint();
	afx_msg void OnChangeCopyNum();
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

#endif // !defined(AFX_PICTUREDLG_H__8BC8E9D0_C3DF_40FF_B137_7CD6ADA597F2__INCLUDED_)
