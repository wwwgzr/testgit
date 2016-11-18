// Instruct.cpp : implementation file
//

#include "stdafx.h"
#include "Instruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstruct

IMPLEMENT_DYNCREATE(CInstruct, CCmdTarget)

CInstruct::CInstruct()
{
}

CInstruct::~CInstruct()
{
}


BEGIN_MESSAGE_MAP(CInstruct, CCmdTarget)
	//{{AFX_MSG_MAP(CInstruct)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstruct message handlers
