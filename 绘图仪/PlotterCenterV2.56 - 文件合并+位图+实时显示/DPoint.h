// DPoint.h: interface for the CDPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DPOINT_H__3D56D5E1_742E_43AA_B4EE_A0DF74BF5015__INCLUDED_)
#define AFX_DPOINT_H__3D56D5E1_742E_43AA_B4EE_A0DF74BF5015__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct  tagDPoint
{
	double x,y;
} *LPDPOINT,DPOINT;

class CDPoint : public tagDPoint  
{
public:
	CDPoint();
	CDPoint(tagDPoint pt);
	CDPoint(CPoint pt);
	CDPoint(double x0,double y0);
	virtual ~CDPoint();

	inline BOOL operator==(CDPoint pt) const
	{
		if(x!=pt.x) return FALSE;
		if(y!=pt.y) return FALSE;
		return TRUE;
	}
	inline BOOL operator==(CPoint pt) const
	{
		if(x!=pt.x) return FALSE;
		if(y!=pt.y) return FALSE;
		return TRUE;
	}
	inline BOOL operator!=(CDPoint pt) const
	{
		if(x!=pt.x) return TRUE;
		if(y!=pt.y) return TRUE;
		return FALSE;
	}
	inline void operator=(CDPoint pt){x=pt.x;y=pt.y;}
	inline void operator=(DPOINT pt){x=pt.x;y=pt.y;}
	inline void operator=(POINT pt){x=pt.x;y=pt.y;}
	inline void operator=(CPoint pt){x=pt.x;y=pt.y;}
	inline void operator+=(CDPoint dp){x+=dp.x;y+=dp.y;}
	inline CDPoint operator+(CDPoint dp){return CDPoint(x+dp.x,y+dp.y);}
};

#endif // !defined(AFX_DPOINT_H__3D56D5E1_742E_43AA_B4EE_A0DF74BF5015__INCLUDED_)
