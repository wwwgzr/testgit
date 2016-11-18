// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DE750986_22FD_43BE_A10D_7FBEFD570F1F__INCLUDED_)
#define AFX_STDAFX_H__DE750986_22FD_43BE_A10D_7FBEFD570F1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

#define CM_PER_INCH 2.54
	#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
class CPlotCenterModule : public CComModule
{
public:
	LONG Unlock();
	LONG Lock();
	LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2);
	DWORD dwThreadID;
};

#include "PlotContext.h"//20160122�и�ָ��ʶ��

extern CPlotCenterModule _Module;
extern CPlotContext m_plotContext;//20160122�и�ָ��ʶ��
extern int nLarge1;//20160122�и�ָ��ʶ��
extern unsigned char g_cPrintCutSelect; //20101206 �и��ӡѡ��
extern CString strFileName1;//20160122�и�ָ��ʶ��

#include <atlcom.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__DE750986_22FD_43BE_A10D_7FBEFD570F1F__INCLUDED_)
