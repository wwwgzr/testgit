// PlotContext.h: interface for the CPlotContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLOTCONTEXT_H__533D6736_47F8_49FD_9E91_18E84E4F8C44__INCLUDED_)
#define AFX_PLOTCONTEXT_H__533D6736_47F8_49FD_9E91_18E84E4F8C44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Shape.h"


class	CIntPoint : public	CObject
{
public:
	CIntPoint();
	~CIntPoint();
public:
	CPolyLine *	GetPolyLine(int idx);
	CMyPoint	GetIntPoint();
	void	SetIntPoint(CMyPoint newpoint);
	int		GetPolyLineCount();
	int		AddPolyLine(CPolyLine * pNewPolyLine);
	void	RemovePolyLine(int idx);
	void	RemovePolyLine(CPolyLine * pPolyLine);

private:
	bool	m_first;
private:
	CMyPoint		m_intPoint;		// 交点
	CShapeSet		m_polylines;		// 
};

inline	CPolyLine * CIntPoint::GetPolyLine(int idx)
{
	CShape	* pShape = m_polylines.GetAShape(idx);
	ASSERT(pShape->m_type == SP_POLYLINE);
	return (CPolyLine *)pShape;
}

inline	CMyPoint	CIntPoint::GetIntPoint()
{
	ASSERT(!m_first);
	return	m_intPoint;
}

inline	void	CIntPoint::SetIntPoint(CMyPoint newpoint)
{
	ASSERT(m_first);
	m_first = false;
	m_intPoint = newpoint;
}

inline	int	CIntPoint::GetPolyLineCount()
{
	return	m_polylines.GetShapeCount();
}

inline	int	CIntPoint::AddPolyLine(CPolyLine * pNewPolyLine)
{
	return m_polylines.AddShape(pNewPolyLine);
}

inline	void	CIntPoint::RemovePolyLine(int idx)
{
	m_polylines.RemoveShape(idx);
}

//////////////////////////////////////////////////////////////////////

class	CIntPointSet
{
public:
	CIntPointSet();
	~CIntPointSet();
public:
	int		AddIntPoint(CIntPoint * newIPit);
	int		GetPitsCount();
	CIntPoint * GetIntPoint(int idx);
	int		PointInSet(CMyPoint pit);
	void	RemoveIntPoint(int idx);
	void	RemoveAll();

protected:
	CObArray	m_intPits;
};

inline	int	CIntPointSet::GetPitsCount()
{
	return m_intPits.GetSize();
}

inline	CIntPoint * CIntPointSet::GetIntPoint(int idx)
{
	ASSERT(idx < GetPitsCount());
	return (CIntPoint *)(m_intPits.GetAt(idx));
}


inline	void	CIntPointSet::RemoveIntPoint(int idx)
{
	m_intPits.RemoveAt(idx);
}

inline	void	CIntPointSet::RemoveAll()
{
	m_intPits.RemoveAll();
}

//////////////////////////////////////////////////////////////////////

// 绘图用的上下文环境。该环境中只用到笔

enum	PenStatus{PENUP = 0, PENDOWN};
enum	LineType{SOLID = 1};
enum	PlotMode{PENABSOLUTE = 0, PENRELATIVE};

class	CPlotContext
{
public:
	CPlotContext();
	virtual ~CPlotContext();
public:
	void	InitContext();
public:
	DWORD	m_originX;
	DWORD	m_originY;
	DWORD	m_lengthX;
	DWORD	m_lengthY;

public:
	CMyPoint	GetCurrentPenPos();
	void		SetCurrentPenPos(CMyPoint	newPos);
	void		InvalidatePen();
	void		SetCurrentPenCode(BYTE	newCode);
	BYTE		GetCurrentPenCode();
	void		DeleteAll();
public:
	void		SetCuttedPenCode(BYTE penCode);
protected:
	CMyPoint	m_curPenPos;		// 当前笔所在的位置
	bool		m_penPosEff;
	BYTE		m_curPenCode;		// 当前的笔的笔号
	bool		m_penCodeEff;
public:
	PenStatus	m_penStatus;
	PlotMode	m_penMode;
public:
	double		m_inUnit;
public:
	CSize		m_charSize;			// 字符大小
	int			m_escapement;		// 字符串方向
	Arrange		m_arrange;
	CString		m_strDI;
	CString		m_strSI;
public:
	long		m_xPage;
	long		m_xMax;
	long		m_xMin;//图形最小坐标
public:
	CShapeSet *	GetShapeSet();
	CShapeSet * GetClosedSet();
	CShapeSet * GetTCutSet();
protected:
	CShapeSet	m_shapeSet;				// 所有图形的集合
	CShapeSet	m_closedPolyLineSet;	// 封闭的折线集合
	CShapeSet	m_tTypeCutSet;
public:
	bool		m_bMutiLine;

	bool		m_inited;
	CPolyLine * m_curPolyLine;
};


inline	void	CPlotContext::SetCurrentPenPos(CMyPoint newPos)
{
	m_penPosEff = true;
	m_curPenPos = newPos;
}


inline	CMyPoint	CPlotContext::GetCurrentPenPos()
{
	ASSERT(m_penPosEff);
	return	m_curPenPos;
}

inline	void	CPlotContext::InvalidatePen()
{
	m_penPosEff = false;
}

inline	BYTE	CPlotContext::GetCurrentPenCode()
{
	return	m_curPenCode;
}

inline	void	CPlotContext::SetCurrentPenCode(BYTE newPen)
{
	m_curPenCode = newPen;
}

inline	CShapeSet *	CPlotContext::GetShapeSet()
{
	return	&m_shapeSet;
}

inline	CShapeSet * CPlotContext::GetClosedSet()
{
	return &m_closedPolyLineSet;
}

inline	CShapeSet * CPlotContext::GetTCutSet()
{
	return &m_tTypeCutSet;
}


//////////////////////////////////////////////////////////////////////

class	CTTypeCut : public CObject
{
public:
	CTTypeCut();
	~CTTypeCut();

public:
	CPolyLine * pLineOnClosed;
	CMyPoint *	pPtOnClosed;
	CMyPoint *	pPtOnBand;
	CPolyLine * pLineCrsBand;
	int			m_rectIdx;
	bool		m_done;
};

//////////////////////////////////////////////////////////////////////

class	CTTypeCutSet
{
public:
	CTTypeCutSet();
	~CTTypeCutSet();
public:
	int	AddTCut(CTTypeCut * newTCut);
	CTTypeCut * GetATCut(int idx);
	int		GetTCutCount();
	void	RemoveAll();
	void	DeleteAll();
	int		CutInSet(CPolyLine * pLine);
private:
	CObArray	m_tcutAry;
};

inline	int CTTypeCutSet::AddTCut(CTTypeCut * newTCut)
{
	return m_tcutAry.Add(newTCut);
}

inline	CTTypeCut * CTTypeCutSet::GetATCut(int idx)
{
	return (CTTypeCut *)(m_tcutAry.GetAt(idx));
}

inline	int	CTTypeCutSet::GetTCutCount()
{
	return m_tcutAry.GetSize();
}

inline	void CTTypeCutSet::RemoveAll()
{
	m_tcutAry.RemoveAll();
}

#endif // !defined(AFX_PLOTCONTEXT_H__533D6736_47F8_49FD_9E91_18E84E4F8C44__INCLUDED_)
