// Shape.cpp: implementation of the CShape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlotCenter.h"
#include "Shape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShape::CShape()
{
	m_type = SP_NULL;
	m_penCode = 0;
	m_bCut = false;
}

CShape::~CShape()
{
}

//////////////////////////////////////////////////////////////////////

CArcLine::CArcLine()
{
	m_type = SP_ARCLINE;
}

CArcLine::~CArcLine()
{
}

//////////////////////////////////////////////////////////////////////

CCircle::CCircle()
{
	m_type = SP_CIRCLE;
}

CCircle::~CCircle()
{


}

//////////////////////////////////////////////////////////////////////

CLabel::CLabel()
{
	m_type = SP_LABEL;
	m_arrange = ARG_CENTER;
	m_chSizer.cx = 0;
	m_chSizer.cy = 0;
	m_content.Empty();
}

CLabel::~CLabel()
{

}

void	CLabel::CreateSimilar(CLabel * pLabel)
{
	m_penCode = pLabel->m_penCode;
	m_bCut = pLabel->m_bCut;
	m_type = pLabel->m_type;

	m_content = pLabel->m_content;
	m_startPoint = pLabel->m_startPoint;
	m_chSizer = pLabel->m_chSizer;
	m_arrange = pLabel->m_arrange;
	m_escapement = pLabel->m_escapement;
}

//////////////////////////////////////////////////////////////////////

CMyPoint::CMyPoint()
{
	x = 0;
	y = 0;
}

CMyPoint::CMyPoint(const CMyPoint & pit)
{
	x = pit.x;
	y = pit.y;
}

CMyPoint::~CMyPoint()
{
}

bool CMyPoint::operator == ( CMyPoint point ) const
{
	if (x == point.x && y == point.y)
		return true;
	return false;
}

CMyPoint & CMyPoint::operator = (const CMyPoint & point )
{
	x = point.x;
	y = point.y;
	return *this;
}
//////////////////////////////////////////////////////////////////////

CSLine::CSLine()
{

}

CSLine::~CSLine()
{

}


//////////////////////////////////////////////////////////////////////

CRectangle::CRectangle()
{
	m_type = SP_RECT;
}
CRectangle::~CRectangle()
{
}

//////////////////////////////////////////////////////////////////////

CPolyLine::CPolyLine()
{
	m_type = SP_POLYLINE;
	m_pitAry.SetSize(0, 10);
	m_bClosed = false;
}

CPolyLine::~CPolyLine()
{

}



void	CPolyLine::CreateSimilar(CPolyLine * pPolyLine)
{
	CMyPoint * pPit;
	int i, n;
	n = pPolyLine->GetPointCount();

	m_penCode = pPolyLine->m_penCode;
	m_bCut = pPolyLine->m_bCut;
	m_type = pPolyLine->m_type;


	for (i = 0; i < n; i++)
	{
		pPit = new CMyPoint;
		pPit->x = pPolyLine->GetPoint(i)->x;
		pPit->y = pPolyLine->GetPoint(i)->y;
		m_pitAry.Add(pPit);
	}
}

int		CPolyLine::AddPoint(CMyPoint newPit)
{
	CMyPoint * pPit = new CMyPoint;
	pPit->x = newPit.x;
	pPit->y = newPit.y;
	return m_pitAry.Add(pPit);
}

void	CPolyLine::RemovePoint(CMyPoint * pPit)
{
	int i;
	int n = m_pitAry.GetSize();
	for (i = 0; i < n; i++)
	{
		if (m_pitAry.GetAt(i) == pPit)
		{
			m_pitAry.RemoveAt(i);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////

CShapeSet::CShapeSet()
{
	m_shapes.SetSize(0, 10);
}

CShapeSet::~CShapeSet()
{


}

void	CShapeSet::CreateSimilarSet(CShapeSet * oldSet)
{
	DeleteAll();
	int	n = oldSet->GetShapeCount();
	int	i;
	CShape * pShape;
	CPolyLine * pPolyLine;
	CLabel	*	pLabel;

	for (i = 0; i < n; i++)
	{
		pShape = oldSet->GetAShape(i);
		if (pShape->m_type == SP_POLYLINE)
		{
			pPolyLine = new CPolyLine;
			pPolyLine->CreateSimilar((CPolyLine *)pShape);
			m_shapes.Add(pPolyLine);
		}
		else if (pShape->m_type == SP_LABEL)
		{
			pLabel = new CLabel;
			pLabel->CreateSimilar((CLabel *)pShape);
			m_shapes.Add(pLabel);
		}
		else
		{
			// 其他情况
		}
	}

}

void	CShapeSet::DeleteAll()
{
	int	n = m_shapes.GetSize();
	int	i;
	for (i = 0; i < n; i++)
	{
		delete	m_shapes.GetAt(i);
	}
	m_shapes.RemoveAll();
}


int		CShapeSet::AddShapeInOrder(CPolyLine * pNewShape)
{

	return 0;
}

//////////////////////////////////////////////////////////////////////
