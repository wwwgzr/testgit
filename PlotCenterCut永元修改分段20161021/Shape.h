// Shape.h: interface for the CShape class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAPE_H__5E3EA2AE_9443_4370_9DCD_3D111B65862C__INCLUDED_)
#define AFX_SHAPE_H__5E3EA2AE_9443_4370_9DCD_3D111B65862C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum	ShapeType	{SP_NULL = 0, SP_LINE, SP_RECT, SP_CIRCLE, SP_ARCLINE, SP_LABEL, SP_POLYLINE};
enum	Arrange		{ARG_LEFT = 0, ARG_CENTER = 5, ARG_RIGHT = 8};

/////////////////////////////////////////////////////////////////////////////////
class CShape : public CObject
{
public:
	CShape();
	virtual ~CShape();
public:

	virtual	bool	LoadFromHpgl(const CString & str) {return true;};
	virtual	bool	SaveAsHpgl(CString & str) {return true;};
	virtual	void	CreateSimilar(CShape * pShape) {};
	virtual	void	Delete() {};
	virtual	bool	Draw(CDC * pDC) {return true;};

public:
	ShapeType	m_type;
	BYTE		m_penCode;
	bool		m_bCut;
};

/////////////////////////////////////////////////////////////////////////////////


class	CMyPoint : public CObject
{
public:
	CMyPoint();
	CMyPoint(const CMyPoint & pit);

	virtual	~CMyPoint();
public:
	bool operator == (CMyPoint point) const;
	CMyPoint & operator = (const CMyPoint & point);
public:
	long	x;
	long	y;
};


class	CSLine	: public CShape
{
public:
	CSLine();
	~CSLine();
public:
	CMyPoint	m_ptBeg;
	CMyPoint	m_ptEnd;
};


/////////////////////////////////////////////////////////////////////////////////
 
class	CRectangle : public CShape
{
public:
	CRectangle();
	virtual	~CRectangle();
public:

};


/////////////////////////////////////////////////////////////////////////////////

class	CCircle : public CShape
{
public:
	CCircle();
	virtual	~CCircle();
public:

};

/////////////////////////////////////////////////////////////////////////////////

class	CArcLine : public CShape
{
public:
	CArcLine();
	virtual	~CArcLine();
public:

};

/////////////////////////////////////////////////////////////////////////////////

class	CLabel : public CShape
{
public:
	CLabel();
	virtual	~CLabel();
	void	CreateSimilar(CLabel * pLabel);

public:
	CString		m_content;
	CMyPoint	m_startPoint;
	CSize		m_chSizer;
	Arrange		m_arrange;
	int			m_escapement;
//	CString		m_cmdDI;
//	CString		m_cmdSI;
};
/////////////////////////////////////////////////////////////////////////////////


class	CPolyLine : public CShape
{
public:
	CPolyLine();
	virtual	~CPolyLine();
	void	CreateSimilar(CPolyLine * pPolyLine);

public:
	CMyPoint * GetPoint(int idx);
	CMyPoint GetFirstPoint();
	CMyPoint GetLastPoint();

	void	RemoveAll();
	void	RemovePoint(int idx);
	void	RemovePoint(CMyPoint * pPit);
	void	InsertLine(int idx, CMyPoint * pNewPit);
	int		AddPoint(CMyPoint * pNewPit);
	int		AddPoint(CMyPoint newPit);
	int		GetPointCount();
	void	FreeExtra();
private:
	CObArray	m_pitAry;
public:
	bool		m_bClosed;
	bool		m_bSelected;
};

inline	CMyPoint * CPolyLine::GetPoint(int	idx)
{
	return (CMyPoint *)m_pitAry.GetAt(idx);
}

inline	CMyPoint CPolyLine::GetFirstPoint()
{
	return *((CMyPoint *)m_pitAry.GetAt(0));
}

inline	CMyPoint CPolyLine::GetLastPoint()
{
	return	*((CMyPoint *)m_pitAry.GetAt(m_pitAry.GetSize() - 1));
}

inline	int	CPolyLine::GetPointCount()
{
	return m_pitAry.GetSize();
}

inline	void	CPolyLine::FreeExtra()
{
	m_pitAry.FreeExtra();
}

inline	void	CPolyLine::RemoveAll()
{
	m_pitAry.RemoveAll();
}

inline	void	CPolyLine::RemovePoint(int idx)
{
	m_pitAry.RemoveAt(idx);
}

inline	void	CPolyLine::InsertLine(int idx, CMyPoint * pNewPit)
{
	m_pitAry.InsertAt(idx, pNewPit);
}

inline	int		CPolyLine::AddPoint(CMyPoint * pNewPit)
{
	return m_pitAry.Add(pNewPit);
}

/////////////////////////////////////////////////////////////////////////////////

class	CShapeSet
{
public:
	CShapeSet();
	~CShapeSet();
public:
	void	CreateSimilarSet(CShapeSet * oldSet);
public:
	CShape *	GetAShape(int idx);
	int		GetShapeCount();
	void	RemoveShape(int idx);
	void	InsertShape(int idx, CPolyLine * pNewShape);
	void	RemoveAll();
	int		AddShape(CShape * pNewShape);
	int		AddShape(CSLine * pNewShape);
	int		AddShape(CArcLine * pNewShape);
	int		AddShape(CCircle * pNewShape);
	int		AddShape(CLabel * pNewShape);
	int		AddShape(CRectangle * pNewShape);
	int		AddShape(CPolyLine * pNewShape);
	int		AddShapeInOrder(CPolyLine * pNewShape);
public:
	void	DeleteAll();

protected:
	CObArray	m_shapes;

};

inline	int	CShapeSet::GetShapeCount()
{
	return m_shapes.GetSize();
}

inline	void	CShapeSet::RemoveShape(int idx)
{
	m_shapes.RemoveAt(idx);
}

inline	void	CShapeSet::InsertShape(int idx, CPolyLine * pNewShape)
{
	m_shapes.InsertAt(idx, pNewShape);
}

inline	void	CShapeSet::RemoveAll()
{
	m_shapes.RemoveAll();
}

inline	CShape * CShapeSet::GetAShape(int idx)
{
	return (CShape *)(m_shapes.GetAt(idx));
}

inline	int	CShapeSet::AddShape(CShape * pNewShape)
{
	ASSERT(pNewShape->m_type != SP_NULL);
	return m_shapes.Add(pNewShape);
}

inline	int	CShapeSet::AddShape(CSLine * pNewShape)
{
	return m_shapes.Add(pNewShape);
}

inline	int	CShapeSet::AddShape(CArcLine * pNewShape)
{
	return m_shapes.Add(pNewShape);
}

inline	int	CShapeSet::AddShape(CCircle * pNewShape)
{
	return m_shapes.Add(pNewShape);
}

inline	int	CShapeSet::AddShape(CLabel * pNewShape)
{
	return m_shapes.Add(pNewShape);
}

inline	int	CShapeSet::AddShape(CRectangle * pNewShape)
{
	return m_shapes.Add(pNewShape);
}

inline	int	CShapeSet::AddShape(CPolyLine * pNewShape)
{
	return m_shapes.Add(pNewShape);
}

/////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SHAPE_H__5E3EA2AE_9443_4370_9DCD_3D111B65862C__INCLUDED_)
