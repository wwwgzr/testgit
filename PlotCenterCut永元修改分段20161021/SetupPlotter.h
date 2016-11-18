#if !defined(AFX_SETUPPLOTTER_H__F401847E_3F3F_409F_A21F_F51CDBFEA822__INCLUDED_)
#define AFX_SETUPPLOTTER_H__F401847E_3F3F_409F_A21F_F51CDBFEA822__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupPlotter.h : header file
//
#include "plotCenterDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CSetupPlotter dialog

class CSetupPlotter : public CDialog
{
// Construction
public:
	void DisplaySprayError(void);
	CSetupPlotter(CWnd* pParent = NULL);   // standard constructor
	CPlotCenterDlg *m_pParent;

	unsigned char m_cPrintStyle;
	unsigned char m_cSprinklerCount;
	unsigned char m_cSprinklerNo;
	unsigned char m_cPenNo;//20101206 ± ∫≈µ∂∫≈
	unsigned char m_cCutNo;
	int m_nAutoPrintStyle;// «∑Ò◊‘∂Ø¥Ú”°±Í÷æ20100413
	int m_nLanguage;
	BOOL m_bConnected;
	BOOL m_bNetworkBusy;

	double  m_dOverlapErrorBak;//20100413
	double	m_dMalposedErrorBak;
	short	m_sSprayErrorX12Bak;
	short	m_sSprayErrorY12Bak;
	short	m_sSprayErrorX23Bak;
	short	m_sSprayErrorX34Bak;
	short	m_sSprayErrorY23Bak;
	short	m_sSprayErrorY34Bak;

// Dialog Data
	//{{AFX_DATA(CSetupPlotter)
	enum { IDD = IDD_SETUPPLOTTER };
	CComboBox	m_PaperTypeSelect;
	BOOL	m_bSpray1;
	BOOL	m_bSpray2;
	BOOL	m_bSpray3;
	BOOL	m_bSpray4;
	BOOL    m_bPrint; //20101206 ¥Ú”°«–∏Ó—°‘Ò
	BOOL    m_bCut;
	double  m_dOverlapError;//20100413
	double	m_dMalposedError;
	double	m_dGraphWidth;
	short	m_sSprayErrorX12;
	short	m_sSprayErrorY12;
	short	m_sSprayErrorX23;
	short	m_sSprayErrorX34;
	short	m_sSprayErrorY23;
	short	m_sSprayErrorY34;
	double	m_dPlotterXError;
	double	m_dPlotterYError;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupPlotter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupPlotter)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSpray1();
	afx_msg void OnSpray2();
	afx_msg void OnSpray3();
	afx_msg void OnSpray4();
	afx_msg void OnUnidirectional();
	afx_msg void OnBidirectional();
	afx_msg void OnPMalposedGraph();
	afx_msg void OnSprayError();
	afx_msg void OnSetPenNo();//20101206 ± ∫≈µ∂∫≈
	afx_msg void OnSetCutNo();
	afx_msg void OnPrint();//20101206 «–∏Ó¥Ú”°—°‘Ò
	afx_msg void OnCut();
	afx_msg void OnUpdateMalposederror();
	afx_msg void OnUpdateMalposederror2();
	afx_msg void OnStatic22();
	afx_msg void OnSelectAutoPrint();
	//20141219 rwt
	afx_msg void OnAddPaperType();
	afx_msg void OnUsePaperType();
	void ReadAddPaperTypeFile();
	void WriteAddPaperTypeFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//class CPaperTypeSelect:public CComboBoxEx()
//{

//};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPPLOTTER_H__F401847E_3F3F_409F_A21F_F51CDBFEA822__INCLUDED_)
