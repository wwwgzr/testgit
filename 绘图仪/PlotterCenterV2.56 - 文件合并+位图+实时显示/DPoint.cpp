// DPoint.cpp: implementation of the CDPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlotCenter.h"
#include "DPoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDPoint::CDPoint()
{

}

CDPoint::CDPoint(DPOINT pt)
{
	x=pt.x;
	y=pt.y;
}

CDPoint::CDPoint(CPoint pt)
{
	x=pt.x;
	y=pt.y;
}

CDPoint::CDPoint(double x0,double y0)
{
	x=x0;
	y=y0;
}

CDPoint::~CDPoint()
{
	x=0;
	y=0;
}
