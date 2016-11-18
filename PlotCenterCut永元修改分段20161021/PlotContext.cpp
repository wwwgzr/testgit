// PlotContext.cpp: implementation of the CPlotContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlotCenter.h"
#include "PlotContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIntPoint::CIntPoint()
{
	m_first = true;
}

CIntPoint::~CIntPoint()
{


}

void	CIntPoint::RemovePolyLine(CPolyLine * pPolyLine)
{
	ASSERT(!m_first);
	int	n = m_polylines.GetShapeCount();
	int	i;
	for (i = 0; i < n; i++)
	{
		if (m_polylines.GetAShape(i) == pPolyLine)
		{
			m_polylines.RemoveShape(i);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIntPointSet::CIntPointSet()
{
	m_intPits.SetSize(0, 100);
}

CIntPointSet::~CIntPointSet()
{

}

int	CIntPointSet::AddIntPoint(CIntPoint * newIPit)
{
	ASSERT(newIPit != NULL);

	// 按x,y 从小到大的顺序添加点

	int	n = m_intPits.GetSize();
	if (n == 0)
		return m_intPits.Add(newIPit);

	CIntPoint * pIpt;

	int	min = 0;
	int	max = n - 1;
	int	mid;
	do
	{
		if (min > max)
		{
			break;
		}

		pIpt = (CIntPoint *)(m_intPits.GetAt(min));
		if (pIpt->GetIntPoint().x > newIPit->GetIntPoint().x)
		{
			m_intPits.InsertAt(min, newIPit);
			return min;
		}

		pIpt = (CIntPoint *)(m_intPits.GetAt(max));
		if (pIpt->GetIntPoint().x < newIPit->GetIntPoint().x)
		{
			m_intPits.InsertAt(max + 1, newIPit);
			return max + 1;
		}

		mid = min + ((max - min) - ((max - min) % 2)) / 2;
		pIpt = (CIntPoint *)(m_intPits.GetAt(mid));
		if (pIpt->GetIntPoint().x < newIPit->GetIntPoint().x)
		{
			min = mid + 1;
		}
		else if (pIpt->GetIntPoint().x > newIPit->GetIntPoint().x)
		{
			max = mid - 1;
		}
		else
		{
			if (pIpt->GetIntPoint().y < newIPit->GetIntPoint().y)
			{
				mid++;
				for (mid; mid < n; mid++)
				{
					pIpt = (CIntPoint *)(m_intPits.GetAt(mid));
					if (pIpt->GetIntPoint().x == newIPit->GetIntPoint().x)
					{
						if (pIpt->GetIntPoint().y > newIPit->GetIntPoint().y)
							break;
						else if (pIpt->GetIntPoint().y < newIPit->GetIntPoint().y)
							continue;
						else
						{
						}
					}
					else
						break;
				}
				if (mid > n)
					mid = n;
				m_intPits.InsertAt(mid, newIPit);
				return mid;
			}
			else if (pIpt->GetIntPoint().y > newIPit->GetIntPoint().y)
			{
				mid--;
				for (mid; mid >= 0; mid--)
				{
					pIpt = (CIntPoint *)(m_intPits.GetAt(mid));
					if (pIpt->GetIntPoint().x == newIPit->GetIntPoint().x)
					{
						if (pIpt->GetIntPoint().y < newIPit->GetIntPoint().y)
						{
							mid++;
							break;
						}
						else if (pIpt->GetIntPoint().y > newIPit->GetIntPoint().y)
							continue;
						else
						{
						}
					}
					else
					{
						mid++;
						break;
					}
				}
				if (mid < 0)
					mid = 0;
				m_intPits.InsertAt(mid, newIPit);
				return mid;
			}
			else
			{
			}
		}

	}while(true);

	return -1;
}

int		CIntPointSet::PointInSet(CMyPoint pit)
{
	// 用二分法查找点

	int	n = m_intPits.GetSize();

	if (n <= 0)
	{
		return -1;
	}
	int	mid, min, max;
	min = 0;
	max = n - 1;
	CIntPoint * pIpt;

	do
	{
		if (min > max)
		{
			return -1;
		}
		mid = min + ((max - min) - ((max - min) % 2)) / 2;

		pIpt = (CIntPoint *)(m_intPits.GetAt(mid));
		if (pIpt->GetIntPoint().x < pit.x)
		{
			min = mid + 1;
		}
		else if (pIpt->GetIntPoint().x > pit.x)
		{
			max = mid - 1;
		}
		else
		{
			if (pIpt->GetIntPoint().y < pit.y)
			{
				mid++;
				for (; mid <= max; mid++)
				{
					pIpt = (CIntPoint *)(m_intPits.GetAt(mid));
					if (pIpt->GetIntPoint().x == pit.x)
					{
						if (pIpt->GetIntPoint().y == pit.y)
							return mid;
						else if (pIpt->GetIntPoint().y > pit.y)
							break;
					}
					else
						break;
				}
				return -1;
			}
			else if (pIpt->GetIntPoint().y > pit.y)
			{
				for (; mid >= min; mid--)
				{
					pIpt = (CIntPoint *)(m_intPits.GetAt(mid));
					if (pIpt->GetIntPoint().x == pit.x)
					{
						if (pIpt->GetIntPoint().y == pit.y)
							return mid;
						else if (pIpt->GetIntPoint().y < pit.y)
							break;
					}
					else
						break;
				}
				return -1;
			}
			else
				return mid;
		}

	}while(true);

	return -1;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




CPlotContext::CPlotContext()
{
	InitContext();
}

CPlotContext::~CPlotContext()
{
	InitContext();
}

void	CPlotContext::InitContext()
{
	m_penPosEff = false;
	m_inited = false;
	m_curPenPos.x = 0;
	m_curPenPos.y = 0;

	m_penCodeEff = false;
	m_curPenCode = 0;

	m_penStatus = PENUP;
	m_penMode = PENABSOLUTE;

	m_charSize.cx = 0;
	m_charSize.cy = 0;
	m_arrange = ARG_CENTER;
	m_strDI.Empty();
	m_strSI.Empty();

	m_inUnit = 0.025;

	m_xPage = 0;


	m_shapeSet.DeleteAll();
}

void	CPlotContext::SetCuttedPenCode(BYTE penCode)
{
	int	i;
	int	n = m_closedPolyLineSet.GetShapeCount();
	CShape * pShape;

	for (i = 0; i < n; i++)
	{
		pShape = m_closedPolyLineSet.GetAShape(i);
		if (pShape->m_type != SP_POLYLINE)
			continue;
		pShape->m_penCode = penCode;
	}

	n = m_tTypeCutSet.GetShapeCount();

	for (i = 0; i < n; i++)
	{
		pShape = m_tTypeCutSet.GetAShape(i);
		if (pShape->m_type != SP_POLYLINE)
			continue;
		pShape->m_penCode = penCode;
	}

}


void	CPlotContext::DeleteAll()
{
    	m_shapeSet.DeleteAll();
		//20160203切割指令识别
	    //m_arrange="";
        m_bMutiLine=NULL;
		m_charSize=NULL;
		m_closedPolyLineSet.RemoveAll();
		m_curPenCode=NULL;
		m_curPolyLine=NULL;
		m_escapement=NULL;
		m_inited=NULL;
		m_inUnit=NULL;
		m_lengthX=NULL;
		m_lengthY=NULL;
		m_originX=NULL;
		m_originY=NULL;
		m_penCodeEff=NULL;
		//m_penMode="";
		m_curPenPos.x=NULL;
		m_curPenPos.y=NULL;
		m_penPosEff=NULL;
		//m_penStatus=NULL;
		m_shapeSet.DeleteAll();
		m_strDI="";
		m_strSI="";
		m_tTypeCutSet.RemoveAll();
		m_xMax=NULL;
		m_xMin=NULL;
		m_xPage=NULL;
		//20160203切割指令识别
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTTypeCut::CTTypeCut()
{
	pLineOnClosed = NULL;
	pLineCrsBand = NULL;
	pPtOnClosed = NULL;
	pPtOnBand = NULL;
	m_rectIdx = -1;
	m_done = false;
}

CTTypeCut::~CTTypeCut()
{

}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTTypeCutSet::CTTypeCutSet()
{

}

CTTypeCutSet::~CTTypeCutSet()
{

}

void	CTTypeCutSet::DeleteAll()
{
	int n = m_tcutAry.GetSize();
	int i;
	for (i = 0; i < n; i++)
	{
		delete	m_tcutAry.GetAt(i);
	}
	m_tcutAry.RemoveAll();
}

int	CTTypeCutSet::CutInSet(CPolyLine * pLine)
{
	int	n = m_tcutAry.GetSize();
	int i;
	for (i = 0; i < n; i++)
	{
		if (pLine == GetATCut(i)->pLineOnClosed)
		{
			return i;
		}
	}
	return -1;
}
