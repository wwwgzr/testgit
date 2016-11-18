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

	// ���ÿؼ���Ϣ
	BOOL SetControlProperty(PDLGCTLINFO lp, int nElements);

	// �Ƿ��ڶԻ������½���ʾ��ʾ�ɸı��С��ͼ��
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
	int m_iClientWidth;		// �Ի���client����Ŀ��
	int m_iClientHeight;	// �Ի���client����ĸ߶�
	int m_iMinWidth;			// �Ի������С���
	int m_iMinHeight;			// �Ի������С�߶�
	PDLGCTLINFO m_pControlArray; // �ؼ���Ϣ����ָ��
	int m_iControlNumber;	// ���ÿؼ���Ϣ�Ŀؼ�����
	BOOL m_bShowSizeIcon;	// �Ƿ���ʾ��ʾ�ɸı��С��ͼ��
	CStatic m_wndSizeIcon;// ��ͼ��ľ�̬�ؼ�
	// ����ͼ���bitmap
	CBitmap m_bmpSizeIcon;
	BITMAP m_bitmap;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWHISTORY_H__44E979AE_0334_401A_816E_0DBEF06F8399__INCLUDED_)
