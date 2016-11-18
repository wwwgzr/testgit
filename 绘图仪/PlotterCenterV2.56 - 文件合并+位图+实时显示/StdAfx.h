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

#ifndef ULONG_PTR
#define ULONG_PTR unsigned long*
#endif
#include "Gdiplus.h"
using namespace Gdiplus;

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
extern CPlotCenterModule _Module;
extern int times;
extern int hbwj;
extern int pg;
extern int pgnum;
extern int pgg;
extern int hbwjplotwidth;
extern int display;
extern int displayx1;
extern int displayx2;
extern int displayspeed;
extern int displaybusy;
extern CString hbwjname;
extern CString savefilename;

#include <atlcom.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__DE750986_22FD_43BE_A10D_7FBEFD570F1F__INCLUDED_)
