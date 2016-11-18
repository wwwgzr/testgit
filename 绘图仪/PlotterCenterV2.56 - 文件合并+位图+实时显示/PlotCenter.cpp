// PlotCenter.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PlotCenter.h"
#include "PlotCenterDlg.h"
#include <initguid.h>
#include "PlotCenter_i.c"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlotCenterApp

BEGIN_MESSAGE_MAP(CPlotCenterApp, CWinApp)
	//{{AFX_MSG_MAP(CPlotCenterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotCenterApp construction

CPlotCenterApp::CPlotCenterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
  TCHAR szFullPath[_MAX_PATH];
  TCHAR szDir[_MAX_DIR];
  TCHAR szDrive[_MAX_DRIVE]; 
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
  _splitpath(szFullPath,szDrive,szDir,NULL,NULL);
  CString str;
	str.Format(_T("%s%s"),szDrive,szDir);

  m_pszProfileName=_tcsdup(str+"rpplot.ini");
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPlotCenterApp object

CPlotCenterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPlotCenterApp initialization

BOOL CPlotCenterApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(CG_IDS_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	ReadIniFile();

	if (!InitATL())
		return FALSE;

	AfxEnableControlContainer();

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		return TRUE;
	}



	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	CPlotCenterDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

	
CPlotCenterModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

LONG CPlotCenterModule::Unlock()
{
	AfxOleUnlockApp();
	return 0;
}

LONG CPlotCenterModule::Lock()
{
	AfxOleLockApp();
	return 1;
}
LPCTSTR CPlotCenterModule::FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
	while (*p1 != NULL)
	{
		LPCTSTR p = p2;
		while (*p != NULL)
		{
			if (*p1 == *p)
				return CharNext(p1);
			p = CharNext(p);
		}
		p1++;
	}
	return NULL;
}


int CPlotCenterApp::ExitInstance()
{

	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	WriteIniFile();
	if (m_bATLInited)
	{
		_Module.RevokeClassObjects();
		_Module.Term();
		CoUninitialize();
	}

	return CWinApp::ExitInstance();

}

BOOL CPlotCenterApp::InitATL()
{
	m_bATLInited = TRUE;

#if _WIN32_WINNT >= 0x0400
	HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
#else
	HRESULT hRes = CoInitialize(NULL);
#endif

	if (FAILED(hRes))
	{
		m_bATLInited = FALSE;
		return FALSE;
	}

	_Module.Init(ObjectMap, AfxGetInstanceHandle());
	_Module.dwThreadID = GetCurrentThreadId();

	LPTSTR lpCmdLine = GetCommandLine(); //this line necessary for _ATL_MIN_CRT
	TCHAR szTokens[] = _T("-/");

	BOOL bRun = TRUE;
	LPCTSTR lpszToken = _Module.FindOneOf(lpCmdLine, szTokens);
	while (lpszToken != NULL)
	{
		if (lstrcmpi(lpszToken, _T("UnregServer"))==0)
		{
			_Module.UpdateRegistryFromResource(IDR_PLOTCENTER, FALSE);
			_Module.UnregisterServer(TRUE); //TRUE means typelib is unreg'd
			bRun = FALSE;
			break;
		}
		if (lstrcmpi(lpszToken, _T("RegServer"))==0)
		{
			_Module.UpdateRegistryFromResource(IDR_PLOTCENTER, TRUE);
			_Module.RegisterServer(TRUE);
			bRun = FALSE;
			break;
		}
		lpszToken = _Module.FindOneOf(lpszToken, szTokens);
	}

	if (!bRun)
	{
		m_bATLInited = FALSE;
		_Module.Term();
		CoUninitialize();
		return FALSE;
	}

	hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, 
		REGCLS_MULTIPLEUSE);
	if (FAILED(hRes))
	{
		m_bATLInited = FALSE;
		CoUninitialize();
		return FALSE;
	}	

	return TRUE;

}

void CPlotCenterApp::ReadIniFile()
{
	int nTerminator;
	nTerminator=GetProfileInt(_T("Para"),_T("Terminator"),3);
	m_cTerminator=(unsigned char)nTerminator;
	//是否自动打印标志20100413
	m_nAutoPrintStyle=GetProfileInt(_T("Para"),_T("AutoPrintStyle"),0);
	//***********
	m_nLanguage=GetProfileInt(_T("Para"),_T("Language"),0);
	m_nMeasureUnit=GetProfileInt(_T("Para"),_T("MeasureUnit"),0);
	m_nCommunicateMode=GetProfileInt(_T("Para"),_T("CommunicateMode"),0);
	m_nRotate=GetProfileInt(_T("Para"),_T("Rotate"),0);
	m_nFont=GetProfileInt(_T("Para"),_T("Font"),0);
	m_dPaperWidth=atof(GetProfileString(_T("Para"), _T("PaperWidth"), "160"));
	m_dGapDown=atof(GetProfileString(_T("Para"), _T("GapDown"), "0"));
	m_dGapRight=atof(GetProfileString(_T("Para"), _T("GapRight"), "0"));
	m_dMarkSpace=atof(GetProfileString(_T("Para"), _T("MarkSpace"), "200"));
	m_dGraphWidth=atof(GetProfileString(_T("Para"), _T("GraphWidth"), "1600"));
	CString str;
	int nPos1,nPos2,nPos3,nPos4;
	str=GetProfileString(_T("Para"), _T("RemoteIP"), "192.168.1.116");
	sscanf(str,"%u.%u.%u.%u",&nPos1,&nPos2,&nPos3,&nPos4);
	m_dwRemoteIP=((DWORD)nPos1<<24)+((DWORD)nPos2<<16)+((DWORD)nPos3<<8)+(DWORD)nPos4;
	str=GetProfileString(_T("Para"), _T("SubnetMask"), "255.255.255.0");
	sscanf(str,"%u.%u.%u.%u",&nPos1,&nPos2,&nPos3,&nPos4);
	m_dwSubnetMask=((DWORD)nPos1<<24)+((DWORD)nPos2<<16)+((DWORD)nPos3<<8)+(DWORD)nPos4;
	str=GetProfileString(_T("Para"), _T("GatewayIP"), "192.168.1.1");
	sscanf(str,"%u.%u.%u.%u",&nPos1,&nPos2,&nPos3,&nPos4);
	m_dwGatewayIP=((DWORD)nPos1<<24)+((DWORD)nPos2<<16)+((DWORD)nPos3<<8)+(DWORD)nPos4;
	TCHAR szFullPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	CString strDataDir;
	::GetModuleFileName(NULL,szFullPath,_MAX_PATH);
	_splitpath(szFullPath,szDrive,szDir,NULL,NULL);
	strDataDir.Format(_T("%s%sPlot\\"),szDrive,szDir);
	m_strDataDir=GetProfileString(_T("Para"), _T("DataDir"), strDataDir);
	m_strPrintFile=GetProfileString(_T("Para"), _T("PrintFile"), " ");
	m_lFixedPointLength=atol(GetProfileString(_T("Para"), _T("FixedPointLength"), "0"));
	m_lPaperWidth=atol(GetProfileString(_T("Para"), _T("PictureWidth"), "2600"));

	m_bAutoSort = GetProfileInt(_T("Para"), _T("AutoSort"),1);
	m_bSortByMtime = GetProfileInt(_T("Para"), _T("SortByMtime"), 1);
	m_bSortByTitle = GetProfileInt(_T("Para"), _T("SortByTitle"), 0);
}

void CPlotCenterApp::WriteIniFile()
{
	WriteProfileInt(_T("Para"),_T("Terminator"),(int)m_cTerminator);
	WriteProfileInt(_T("Para"),_T("AutoPrintStyle"),m_nAutoPrintStyle);//是否自动打印标志20100413
	WriteProfileInt(_T("Para"),_T("Language"),m_nLanguage);
	WriteProfileInt(_T("Para"),_T("MeasureUnit"),m_nMeasureUnit);//
	WriteProfileInt(_T("Para"),_T("CommunicateMode"),m_nCommunicateMode);
	WriteProfileInt(_T("Para"),_T("Rotate"),m_nRotate);
	WriteProfileInt(_T("Para"),_T("Font"),m_nFont);
	CString str;
	str.Format("%f",m_dPaperWidth);
	WriteProfileString(_T("Para"), _T("PaperWidth"),str);
	str.Format("%f",m_dGapDown);
	WriteProfileString(_T("Para"), _T("GapDown"),str);
	str.Format("%f",m_dGapRight);
	WriteProfileString(_T("Para"), _T("GapRight"),str);
	str.Format("%f",m_dMarkSpace);
	WriteProfileString(_T("Para"), _T("MarkSpace"),str);
	str.Format("%f",m_dGraphWidth);
	WriteProfileString(_T("Para"), _T("GraphWidth"),str);
	str.Format("%u.%u.%u.%u",((BYTE)(m_dwRemoteIP>>24)),((BYTE)(m_dwRemoteIP>>16)),((BYTE)(m_dwRemoteIP>>8)),((BYTE)m_dwRemoteIP));
	WriteProfileString(_T("Para"), _T("RemoteIP"),str);
	str.Format("%u.%u.%u.%u",((BYTE)(m_dwSubnetMask>>24)),((BYTE)(m_dwSubnetMask>>16)),((BYTE)(m_dwSubnetMask>>8)),((BYTE)m_dwSubnetMask));
	WriteProfileString(_T("Para"), _T("SubnetMask"),str);
	str.Format("%u.%u.%u.%u",((BYTE)(m_dwGatewayIP>>24)),((BYTE)(m_dwGatewayIP>>16)),((BYTE)(m_dwGatewayIP>>8)),((BYTE)m_dwGatewayIP));
	WriteProfileString(_T("Para"), _T("GatewayIP"),str);
	WriteProfileString(_T("Para"), _T("DataDir"),m_strDataDir);
	WriteProfileString(_T("Para"), _T("PrintFile"),m_strPrintFile);
	str.Format("%ld",m_lFixedPointLength);
	WriteProfileString(_T("Para"), _T("FixedPointLength"),str);
	str.Format("%ld",m_lPaperWidth);
	WriteProfileString(_T("Para"), _T("PictureWidth"),str);
	str.Format("%d",m_bAutoSort);
	WriteProfileString(_T("Para"),_T("AutoSort"),str);
	str.Format("%d",m_bSortByMtime);
	WriteProfileString(_T("Para"),_T("SortByMtime"),str);
	str.Format("%d",m_bSortByTitle);
	WriteProfileString(_T("Para"),_T("SortByTitle"),str);
}
