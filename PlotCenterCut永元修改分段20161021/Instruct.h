#if !defined(AFX_INSTRUCT_H__845C0D6E_83DB_46A4_8CC1_B87ECF7DBB48__INCLUDED_)
#define AFX_INSTRUCT_H__845C0D6E_83DB_46A4_8CC1_B87ECF7DBB48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Instruct.h : header file
//
#include <afxtempl.h>
#define	DEFCMD_IN	0x8A	//IN
#define DEFCMD_SP	0x7C	//SP
#define DEFCMD_LT	0xE6	//LT 0:PS_SOLID; 1:PS_DASH; 2:PS_DOT; 3:PS_DASHDOT
#define DEFCMD_PG	0x97	//PG
#define DEFCMD_PU	0xB5	//PU 单个PU
#define DEFCMD_PD	0x3D	//PD 单个PD
#define DEFCMD_LPU	0x5E	//PU 连续PU
#define DEFCMD_LPD	0x3B    //PD 连续PD
#define DEFCMD_DI	0x5B	//AG 角度
#define DEFCMD_SI	0x5D	//SI 64位双精度浮点
#define DEFCMD_FN	0x4B	//FN 字体名
#define DEFCMD_LB	0x4D	//LB
#define DEFCMD_BT	0x9A	//BIT 位图


/////////////////////////////////////////////////////////////////////////////
// CInstruct command target
typedef CArray<double,double> CDoubleArray;

class CInstruct : public CCmdTarget
{
	DECLARE_DYNCREATE(CInstruct)

	CInstruct();           // protected constructor used by dynamic creation

// Attributes
public:
	char m_cName[3];
	CDoubleArray m_adPara;
	CString  m_cFontName;
	CString  m_cString;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstruct)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInstruct();

	// Generated message map functions
	//{{AFX_MSG(CInstruct)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTRUCT_H__845C0D6E_83DB_46A4_8CC1_B87ECF7DBB48__INCLUDED_)
