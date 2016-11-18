// PlotCenter.h : main header file for the PLOTCENTER application
//

#if !defined(AFX_PLOTCENTER_H__F19DE229_995E_4DDD_A680_D87748458176__INCLUDED_)
#define AFX_PLOTCENTER_H__F19DE229_995E_4DDD_A680_D87748458176__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "PlotCenter_i.h"

/////////////////////////////////////////////////////////////////////////////
// CPlotCenterApp:
// See PlotCenter.cpp for the implementation of this class
//

class CPlotCenterApp : public CWinApp
{
public:
	unsigned char m_cTerminator;
	int m_nAutoPrintStyle;//�Ƿ��Զ���ӡ��־20100413
	int m_nLanguage;
	int m_nMeasureUnit;//20150728 ��ӵ�λѡ����
	int m_nCommunicateMode;
	int m_nRotate;
	int m_nFont;
	double m_dPaperWidth;
	double m_dGapRight;
	double m_dPlotterXError;
	double m_dPlotterYError;
	double m_dMalposedError;
	double m_dOverlapError;
	double m_dGapDown;
	double m_dMarkSpace;
	double m_dGraphWidth;
	DWORD m_dwRemoteIP;
	DWORD m_dwSubnetMask;
	DWORD m_dwGatewayIP;
	CString m_strDataDir;		//�Զ���ͼĿ¼
	CString m_strPrintFile;	//û�д�����ļ�
	long m_lFixedPointLength;	//��ʼ��ӡ��
	long m_lPaperWidth;
	CObList m_listPrimitive;//ͼԪ��

	CObList m_listTextPrimitive;
	CObList	m_listBitDataPrimitive; // 20121204 λͼ����
	CObList	m_listBitListPrimitive; // 20121204 λͼ����
  unsigned long *m_gdiplusToken;

public:
	UINT m_bSortByTitle;
	UINT m_bSortByMtime;
	UINT m_bAutoSort;

	void WriteIniFile();
	void ReadIniFile();
	CPlotCenterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotCenterApp)
	public:
	virtual BOOL InitInstance();
		virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPlotCenterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bATLInited;
private:
	BOOL InitATL();
};

extern CPlotCenterApp theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTCENTER_H__F19DE229_995E_4DDD_A680_D87748458176__INCLUDED_)
