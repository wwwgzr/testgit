//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	3BSpline.cpp - the class of B Spline
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "3BSpline.h"

#define MAX_DOUBLE 1.79e308
#define MIN_DOUBLE 1.0E-6

// C++ constructor, creating a C3BSpline, set some value as default
C3BSpline::C3BSpline()
{
  m_nControlPointsCount = 0;					//Number of Control Points Array (also of Weight Array)
  m_nKnotsCount = 0;							//Number of Knots
  m_bKnotsWeightsSeted = FALSE;				//Sign of Knots and Weights Seted
  m_nDegree = 3;								//Degree of Nurbs Curve (=k-1)
  m_cCurveType = 'U';						//Type of Nurbs Curve
  m_bInputByValuePoints = FALSE;				//Sign of Input Points Type
  m_nDispCount = 16;							//Display Count between two Knots
  m_bGetDisplayPoints = FALSE;				//Sign of Calculate of Value Points
  m_bModified = FALSE;						//Sigh of Modifying of Curve (Need of reCalculate)
  m_BaseVector.Set_xyz(0.0, 0.0, 0.0);		//Base Vector of the Curve

  m_nFlag=0;

  m_bHighLightBegin = FALSE;		// sign of begin point highlight
  m_bHighLightEnd = FALSE;		// sign of end point highlight
  m_bDispBE = TRUE;
}

// C++ constructor, creating a C3BSpline from a C3BSpline object
C3BSpline::C3BSpline(C3BSpline const& bsc)
{
  UINT i;

	m_arrayControlPoints.RemoveAll();
	m_arrayWeight.RemoveAll();
	m_nControlPointsCount = bsc.m_arrayControlPoints.GetSize();
	m_arrayControlPoints.SetSize(m_nControlPointsCount);
	m_arrayWeight.SetSize(m_nControlPointsCount);
	
	for(i=0; i<m_nControlPointsCount; i++)
	{
		m_arrayControlPoints[i] = bsc.m_arrayControlPoints[i];
		m_arrayWeight[i] = bsc.m_arrayWeight[i];
	}

	m_arrayKnots.RemoveAll();
	m_nKnotsCount = bsc.m_nKnotsCount;
	m_arrayKnots.SetSize(m_nKnotsCount);
	for(i=0; i<m_nKnotsCount; i++)
		m_arrayKnots[i] = bsc.m_arrayKnots[i];

	m_nDegree = bsc.m_nDegree;
	m_cCurveType = bsc.m_cCurveType;
	m_bInputByValuePoints = bsc.m_bInputByValuePoints;

	m_nDispCount = bsc.m_nDispCount;

	UINT nCount;
	
	m_arrayDisplayPoints.RemoveAll();
	nCount = bsc.m_arrayDisplayPoints.GetSize();
	m_arrayDisplayPoints.SetSize(nCount);
	m_arrayCurrentParameter.SetSize(nCount);
	m_arrayCurrentDistance.SetSize(nCount);
	for(i=0; i<nCount; i++)
	{
		m_arrayDisplayPoints[i] = bsc.m_arrayDisplayPoints[i];
		m_arrayCurrentParameter[i] = bsc.m_arrayCurrentParameter[i];
		m_arrayCurrentDistance[i] = bsc.m_arrayCurrentDistance[i];
	}
	m_bGetDisplayPoints = bsc.m_bGetDisplayPoints;
	m_bModified = bsc.m_bModified;

	m_arrayInputPoints.RemoveAll();
	nCount = bsc.m_arrayInputPoints.GetSize();
	m_arrayInputPoints.SetSize(nCount);
	for(i=0; i<nCount; i++)
		m_arrayInputPoints[i] = bsc.m_arrayInputPoints[i];

	m_arrayInPntPara.RemoveAll();
	nCount = bsc.m_arrayInPntPara.GetSize();
	m_arrayInPntPara.SetSize(nCount);
	for(i=0; i<nCount; i++)
		m_arrayInPntPara[i] = bsc.m_arrayInPntPara[i];

	m_BaseVector.Set_xyz(bsc.m_BaseVector.x(), bsc.m_BaseVector.y(), bsc.m_BaseVector.z());

	m_bKnotsWeightsSeted = bsc.m_bKnotsWeightsSeted;

	m_bHighLightBegin = bsc.m_bHighLightBegin;
	m_bHighLightEnd = bsc.m_bHighLightEnd;
	m_bDispBE = bsc.m_bDispBE;

	m_nFlag = bsc.m_nFlag;
}

// C++ deconstructor, free unused memory
C3BSpline::~C3BSpline()
{
	m_arrayControlPoints.RemoveAll();		//Array of Control Points
	m_arrayWeight.RemoveAll();				//Array of Weight
	m_arrayKnots.RemoveAll();				//Sequence of Knots
	m_arrayDisplayPoints.RemoveAll();		//Value Points to Display
	m_arrayCurrentParameter.RemoveAll();	//Parameter Array of Display Points
	m_arrayCurrentDistance.RemoveAll();	//Distance Array of Display Points
	m_arrayInputPoints.RemoveAll();		//Input Points (may be Control Points or Value Points)
	m_arrayInPntPara.RemoveAll();			//Input Points parameter
}

// Get curve control points count
// Return count in 'UINT' data type
UINT C3BSpline::GetControlPointsCount()
{
	m_nControlPointsCount = m_arrayControlPoints.GetSize();
	return m_nControlPointsCount;
}

// Set curve control points count, this function is ready for the function "SetControlPoint"
void C3BSpline::SetControlPointsCount(UINT nCount)
{
	m_arrayControlPoints.RemoveAll();
	m_arrayWeight.RemoveAll();
	m_arrayControlPoints.SetSize(nCount);
	m_arrayWeight.SetSize(nCount);
	m_nControlPointsCount = nCount;
}

// Get curve control point value by index( begin from 0 to n )
void C3BSpline::GetControlPoint(UINT nIndex, double &x, double &y, double &z)
{
	ASSERT((int)nIndex<m_arrayControlPoints.GetSize());
	if((int)nIndex>=m_arrayControlPoints.GetSize())
		return;
	x = m_arrayControlPoints[nIndex].x();
	y = m_arrayControlPoints[nIndex].y();
	z = m_arrayControlPoints[nIndex].z();
}

// Get curve control point value by index( begin from 0 to n )
// Return the value in 'position' data type
CPoint_3D C3BSpline::GetControlPoint(UINT nIndex)
{
	CPoint_3D rtnP;

	ASSERT((int)nIndex<m_arrayControlPoints.GetSize());
	if((int)nIndex>=m_arrayControlPoints.GetSize())
		return rtnP;

	rtnP = m_arrayControlPoints[nIndex];

	return rtnP;
}

// Set curve control point by index( begin from 0 to n ) and its X, Y, Z value( double )
void C3BSpline::SetControlPoint(UINT nIndex, double x, double y, double z)
{
	ASSERT((int)nIndex<m_arrayControlPoints.GetSize());
	if((int)nIndex>=m_arrayControlPoints.GetSize())
		return;
	m_arrayControlPoints[nIndex].Set_xyz(x, y, z);
}

// Set curve control point by index( begin from 0 to n ) and the position value
void C3BSpline::SetControlPoint(UINT nIndex, CPoint_3D CP)
{
	ASSERT((int)nIndex<m_arrayControlPoints.GetSize());
	if((int)nIndex>=m_arrayControlPoints.GetSize())
		return;
	m_arrayControlPoints[nIndex] = CP;
}

// Set curve control point value with two dimension double array( double v[count][3], each v[*] contains X, Y, Z value )
void C3BSpline::SetControlPoints(double **nPoint)
{
	/*UINT SetCount = sizeof(nPoint)/sizeof(double[3]);
	m_arrayControlPoints.RemoveAll();
	SetControlPointsCount(SetCount);*/
	for(UINT i=0; i<m_nControlPointsCount; i++)
		m_arrayControlPoints[i].Set_xyz(nPoint[i][0], nPoint[i][1], nPoint[i][2]);
}

// Get curve weight value by index( begin from 0 to n )
// Return weight in 'double' data type
double C3BSpline::GetWeight(UINT nIndex)
{
	ASSERT((int)nIndex<m_arrayControlPoints.GetSize());
	if((int)nIndex>=m_arrayControlPoints.GetSize())
		return MAX_DOUBLE;
	return m_arrayWeight[nIndex];
}

// Set curve weight by index( begin from 0 to n ) and the double value
void C3BSpline::SetWeight(UINT nIndex, double nWeight)
{
	ASSERT((int)nIndex<m_arrayControlPoints.GetSize());
	if((int)nIndex>=m_arrayControlPoints.GetSize())
		return;
	m_arrayWeight[nIndex] = nWeight;
	m_bKnotsWeightsSeted = TRUE;
	m_cCurveType = 'C';
}

// Set curve weights by double array
void C3BSpline::SetWeights(double *nWeight)
{
	/*UINT SetCount = sizeof(nWeight)/sizeof(double);
	ASSERT(SetCount==m_nControlPointsCount);
	if(SetCount!=m_nControlPointsCount)
		return;*/
	for(UINT i=0; i<m_nControlPointsCount; i++)
		m_arrayWeight[i] = nWeight[i];
	m_bKnotsWeightsSeted = TRUE;
	m_cCurveType = 'C';
}

// Get curve knots count( if everything get right, it must be m_nControlPointsCount+m_nDegree+1 )
// Return count in 'UINT' data type
UINT C3BSpline::GetKnotsCount()
{
	m_nKnotsCount = m_arrayKnots.GetSize();
	return m_nKnotsCount;
}

// Set curve knots count, this function is ready for the function "SetKnot"
void C3BSpline::SetKnotsCount(UINT nCount)
{
	m_arrayKnots.RemoveAll();
	m_arrayKnots.SetSize(nCount);
	m_nKnotsCount = nCount;
}

// Get curve knot value by index( begin from 0 to n )
// Return knot in 'double' data type
double C3BSpline::GetKnot(UINT nIndex)
{
	ASSERT((int)nIndex<m_arrayKnots.GetSize());
	if((int)nIndex>=m_arrayKnots.GetSize())
		return MAX_DOUBLE;
	return m_arrayKnots[nIndex];
}

// Set curve knot by index( begin from 0 to n ) and the double value
void C3BSpline::SetKnot(UINT nIndex, double nKnot)
{
	ASSERT((int)nIndex<m_arrayKnots.GetSize());
	if((int)nIndex>=m_arrayKnots.GetSize())
		return;
	m_arrayKnots[nIndex] = nKnot;
	m_bKnotsWeightsSeted = TRUE;
	m_cCurveType = 'N';
}

// Set curve knots by double array
void C3BSpline::SetKnots(double *nKnot)
{
	/*UINT SetCount = sizeof(nKnot)/sizeof(double);
	m_arrayKnots.RemoveAll();
	SetKnotsCount(SetCount);*/
	for(UINT i=0; i<m_nKnotsCount; i++)
		m_arrayKnots[i] = nKnot[i];
	m_bKnotsWeightsSeted = TRUE;
	m_cCurveType = 'N';
}

// We presume that you have set the control points, you can set the curve's weights and knots by this function
// or you can set them yourself. We supply 4 modes list( all modes set weght as 1.0f ), Mode set to be 1 as default( no parameter input ):
// Mode==1: Uniform B-Spline. The knots space between are as same as 1.0f, knots begin from (-m_nDegree).
// Mode==2: Quasi Uniform B-Spline. Just as Uniform B-Spline but the knots space between in first (m_nDegree+1) knots and terminal (m_nDegree+1) knots are 0.0f.
// Mode==3: Piecewise Bezier Spline. First (m_nDegree+1) knots are set to be 0.0f, and terminal (m_nDegree+1) knots are set to be 1.0f. The middle left are set to be 0.5f.
// Mode==4: Non-Uniform B Spline. First (m_nDegree+1) knots are set to be 0.0f, and terminal (m_nDegree+1) knots are set to be 1.0f. The middle left knots space between are determined by length of two control points border upon.
void C3BSpline::SetCurveKnotsWeights(int Mode)
{
	int i;
	//double deltaU;

	int InCount = m_arrayInputPoints.GetSize();
	ASSERT(InCount>0);
	if(InCount<0)
		return;

	//Set Weights
	for(i=0; i<(int)m_nControlPointsCount; i++)
		m_arrayWeight[i] = 1.0;

	//Set Knots
	switch(Mode)
	{
	case 1:		//Uniform B-Spline
		m_cCurveType = 'U';
		SetKnotsCount(m_nControlPointsCount+m_nDegree+1);
		for(i=0; i<=(int)(m_nControlPointsCount+m_nDegree); i++)
			m_arrayKnots[i] = (double)i-m_nDegree;//deltaU*((double)i);
		break;
	case 2:		//Quasi Uniform B-Spline
		m_cCurveType = 'Q';
		SetKnotsCount(m_nControlPointsCount+m_nDegree+1);
		//deltaU = 1.0/((double)(m_nControlPointsCount-m_nDegree));
		for(i=0; i<=(int)m_nDegree; i++)
			m_arrayKnots[i] = 0.0;
		for(i=(m_nDegree+1); i<(int)m_nControlPointsCount; i++)
			m_arrayKnots[i] = (double)(i-m_nDegree);//deltaU*((double)(i-m_nDegree));
		for(i=m_nControlPointsCount; i<=(int)(m_nControlPointsCount+m_nDegree); i++)
			m_arrayKnots[i] = (double)(m_nControlPointsCount-m_nDegree);//1.0;
		break;
	case 3:		//Piecewise Bezier Spline
		m_cCurveType = 'P';
		SetKnotsCount(m_nControlPointsCount+m_nDegree+1);
		for(i=0; i<=(int)m_nDegree; i++)
			m_arrayKnots[i] = 0.0;
		for(i=(m_nDegree+1); i<(int)m_nControlPointsCount; i++)
			m_arrayKnots[i] = 0.5;
		for(i=m_nControlPointsCount; i<=(int)(m_nControlPointsCount+m_nDegree); i++)
			m_arrayKnots[i] = 1.0;
		break;
	case 4:		//Non-Uniform B Spline
		ASSERT(m_nDegree==3);
		if(m_nDegree!=3)
			return;
		ASSERT(m_bInputByValuePoints);
		if(!m_bInputByValuePoints)
			return;
		m_arrayInPntPara.SetSize(m_arrayInputPoints.GetSize());
		SetKnotsCount(m_nControlPointsCount+m_nDegree+1);
		for(i=0; i<=(int)m_nDegree; i++)
			m_arrayKnots[i] = 0.0;
		m_arrayInPntPara[0] = 0.0;
		m_arrayKnots[m_nDegree+1] = Dist_2P(1);
		m_arrayInPntPara[1] = m_arrayKnots[m_nDegree+1];
		for(i=(m_nDegree+2); i<=(int)m_nControlPointsCount; i++)
		{
			m_arrayKnots[i] = m_arrayKnots[i-1]+Dist_2P(i-3);
			m_arrayInPntPara[i-3] = m_arrayKnots[i];
		}
		for(i=m_nControlPointsCount+1; i<=(int)(m_nControlPointsCount+m_nDegree); i++)
			m_arrayKnots[i] = m_arrayKnots[m_nControlPointsCount];
		m_cCurveType = 'N';
		break;
	default:	//Same as Quasi Uniform B-Spline
		m_cCurveType = 'Q';
		SetKnotsCount(m_nControlPointsCount+m_nDegree+1);
		//deltaU = 1.0/((double)(m_nControlPointsCount-m_nDegree));
		for(i=0; i<=(int)m_nDegree; i++)
			m_arrayKnots[i] = 0.0;
		for(i=(m_nDegree+1); i<(int)m_nControlPointsCount; i++)
			m_arrayKnots[i] = (double)(i-m_nDegree);//deltaU*((double)(i-m_nDegree));
		for(i=m_nControlPointsCount; i<=(int)(m_nControlPointsCount+m_nDegree); i++)
			m_arrayKnots[i] = (double)(m_nControlPointsCount-m_nDegree);//1.0;
		break;
	}

	m_bKnotsWeightsSeted = TRUE;
}

// Calculate distance between two input points( this function only used in function "SetCurveKnotsWeights" )
double C3BSpline::Dist_2P(UINT nIndex)
{
	ASSERT((int)nIndex<m_arrayInputPoints.GetSize());
	if((int)nIndex>=m_arrayInputPoints.GetSize())
		return MAX_DOUBLE;

	if(nIndex==0)
		return 0.0;
	
	double dist;
	dist = (m_arrayInputPoints[nIndex].x()-m_arrayInputPoints[nIndex-1].x())*(m_arrayInputPoints[nIndex].x()-m_arrayInputPoints[nIndex-1].x())
		+(m_arrayInputPoints[nIndex].y()-m_arrayInputPoints[nIndex-1].y())*(m_arrayInputPoints[nIndex].y()-m_arrayInputPoints[nIndex-1].y())
		+(m_arrayInputPoints[nIndex].z()-m_arrayInputPoints[nIndex-1].z())*(m_arrayInputPoints[nIndex].z()-m_arrayInputPoints[nIndex-1].z());

	return sqrt(dist);
}

// Set curve degree( the constructor set degree to 3 as default, we advise you don't use others )
void C3BSpline::SetDegree(UINT nDegree)
{
	m_nDegree = nDegree;
}

// Get curve degree value
// return degree value in 'UINT' data type
UINT C3BSpline::GetDegree()
{
	return m_nDegree;
}

// Get point coordinate that in the curve by parameter
// Modify input parameter( 3 of double) as return
void C3BSpline::GetPointCoordinate(double u, double &x, double &y, double &z)
{
	UINT i;
	double a1,a2;
	CPoint_3D calcP;

	if(u<m_arrayKnots[m_nDegree])
		u = m_arrayKnots[m_nDegree];
	if(u>m_arrayKnots[m_nControlPointsCount])
		u = m_arrayKnots[m_nControlPointsCount];

	if(m_cCurveType=='U' || m_cCurveType=='Q' || m_cCurveType=='N' || m_cCurveType=='P')
	{
		i = FoundKnotsDomain(u);
		calcP = P_u(i, u);
		x = calcP.x();
		y = calcP.y();
		z = calcP.z();
	}
	else
	{
		a1 = 0.0;
		a2 = 0.0;
		for(i=0; i<m_nControlPointsCount; i++)
		{
			a1 += m_arrayWeight[i]*N_u(i,m_nDegree,u)*m_arrayControlPoints[i].x();
			a2 += m_arrayWeight[i]*N_u(i,m_nDegree,u);
		}
		if(a2<MIN_DOUBLE)
			x = 0.0;
		else
			x = a1/a2;

		a1 = 0.0;
		a2 = 0.0;
		for(i=0; i<m_nControlPointsCount; i++)
		{
			a1 += m_arrayWeight[i]*N_u(i,m_nDegree,u)*m_arrayControlPoints[i].y();
			a2 += m_arrayWeight[i]*N_u(i,m_nDegree,u);
		}
		if(a2<MIN_DOUBLE)
			y = 0.0;
		else
			y = a1/a2;

		a1 = 0.0;
		a2 = 0.0;
		for(i=0; i<m_nControlPointsCount; i++)
		{
			a1 += m_arrayWeight[i]*N_u(i,m_nDegree,u)*m_arrayControlPoints[i].z();
			a2 += m_arrayWeight[i]*N_u(i,m_nDegree,u);
		}
		if(a2<MIN_DOUBLE)
			z = 0.0;
		else
			z = a1/a2;
	}
}

// Get point coordinate that in the curve by parameter
// Return coordinate in 'position' data type
CPoint_3D C3BSpline::GetPointCoordinate(double u)
{
	UINT i;
	double a1,a2;
	CPoint_3D rtnP;

	if(u<m_arrayKnots[m_nDegree])
		u = m_arrayKnots[m_nDegree];
	if(u>m_arrayKnots[m_nControlPointsCount])
		u = m_arrayKnots[m_nControlPointsCount];

	if(m_cCurveType=='U' || m_cCurveType=='Q' || m_cCurveType=='N' || m_cCurveType=='P')
	{
		i = FoundKnotsDomain(u);
		rtnP = P_u(i, u);
	}
	else
	{
		a1 = 0.0;
		a2 = 0.0;
		for(i=0; i<m_nControlPointsCount; i++)
		{
			a1 += m_arrayWeight[i]*N_u(i,m_nDegree,u)*m_arrayControlPoints[i].x();
			a2 += m_arrayWeight[i]*N_u(i,m_nDegree,u);
		}
		if(a2<MIN_DOUBLE)
			rtnP.Set_x(0.0);
		else
			rtnP.Set_x(a1/a2);

		a1 = 0.0;
		a2 = 0.0;
		for(i=0; i<m_nControlPointsCount; i++)
		{
			a1 += m_arrayWeight[i]*N_u(i,m_nDegree,u)*m_arrayControlPoints[i].y();
			a2 += m_arrayWeight[i]*N_u(i,m_nDegree,u);
		}
		if(a2<MIN_DOUBLE)
			rtnP.Set_y(0.0);
		else
			rtnP.Set_y(a1/a2);

		a1 = 0.0;
		a2 = 0.0;
		for(i=0; i<m_nControlPointsCount; i++)
		{
			a1 += m_arrayWeight[i]*N_u(i,m_nDegree,u)*m_arrayControlPoints[i].z();
			a2 += m_arrayWeight[i]*N_u(i,m_nDegree,u);
		}
		if(a2<MIN_DOUBLE)
			rtnP.Set_z(0.0);
		else
			rtnP.Set_z(a1/a2);
	}

	return rtnP;
}

// Get B_Spline basic function value
// Return the value in 'double' data type
double C3BSpline::N_u(UINT i, UINT k, double u)
{
	if(k==0)
	{
		if(u>=m_arrayKnots[i] && u<m_arrayKnots[i+1])
			return 1.0;
		else
			return 0.0;
	}
	else
	{
		double a1, b1, c1, a2, b2, c2;
		double total;

		a1 = u-m_arrayKnots[i];
		b1 = m_arrayKnots[i+k]-m_arrayKnots[i];
		if(b1<MIN_DOUBLE)
			c1 = 0.0;
		else
			c1 = a1/b1;

		a2 = m_arrayKnots[i+k+1]-u;
		b2 = m_arrayKnots[i+k+1]-m_arrayKnots[i+1];
		if(b2<MIN_DOUBLE)
			c2 = 0.0;
		else
			c2 = a2/b2;

		total = c1*N_u(i,k-1,u)+c2*N_u(i+1,k-1,u);

		return total;
	}
}

double C3BSpline::NA(UINT i, UINT k, double u)
{
	double na = 0.0;
	if((m_arrayKnots[i+k]-m_arrayKnots[i])>MIN_DOUBLE)
		na = (u-m_arrayKnots[i])/(m_arrayKnots[i+k]-m_arrayKnots[i]);
	return na;
}

double C3BSpline::NB(UINT i, UINT k, double u)
{
	double nb = 0.0;
	if((m_arrayKnots[i+k+1]-m_arrayKnots[i+1])>MIN_DOUBLE)
		nb = (m_arrayKnots[i+k+1]-u)/(m_arrayKnots[i+k+1]-m_arrayKnots[i+1]);
	return nb;
}

double C3BSpline::N_u3(UINT i, double u)
{
	ASSERT(m_nDegree==3);
	if(m_nDegree!=3)
		return 0.0;

	double total = 0.0;

	if(u>=m_arrayKnots[i] && u<m_arrayKnots[i+1])
		total = NA(i,3,u)*NA(i,2,u)*NA(i,1,u);
	else if(u>=m_arrayKnots[i+1] && u<m_arrayKnots[i+2])
		total = NA(i,3,u)*(NA(i,2,u)*NB(i,1,u)+NB(i,2,u)*NA(i+1,1,u))+NB(i,3,u)*NA(i+1,2,u)*NA(i+1,1,u);
	else if(u>=m_arrayKnots[i+2] && u<m_arrayKnots[i+3])
		total = NA(i,3,u)*NB(i,2,u)*NB(i+1,1,u)+NB(i,3,u)*(NA(i+1,2,u)*NB(i+1,1,u)+NB(i+1,2,u)*NA(i+2,1,u));
	else if(u>=m_arrayKnots[i+3] && u<m_arrayKnots[i+4])
		total = NB(i,3,u)*NB(i+1,2,u)*NB(i+2,1,u);

	return total;
}

// Get point in the curve by index and parameter value use "de Boor Arithmetic of B_Spline"
// Return the value in 'position' data type
CPoint_3D C3BSpline::P_u(UINT i, double u)
{
	UINT j, k, l;
	double denom, alpha;
	CPoint_3D rtn_P;

	double **d = new double *[m_nDegree+1];
	for(k=0; k<m_nDegree+1; k++)
		d[k] = new double[m_nDegree+1];

	for(k=0; k<m_nDegree+1; k++)
		d[0][k] = m_arrayControlPoints[i-m_nDegree+k].x();
	for(l=1; l<=m_nDegree; l++)
	{	
		for(j=l; j<=m_nDegree; j++)
		{
			denom = m_arrayKnots[i+j+1-l]-m_arrayKnots[i-m_nDegree+j];
			if(denom<MIN_DOUBLE)
				alpha = 0.0;
			else
				alpha = (u-m_arrayKnots[i-m_nDegree+j])/denom;
			d[l][j] = (1.0-alpha)*d[l-1][j-1]+alpha*d[l-1][j];
		}
	}
	rtn_P.Set_x(d[m_nDegree][m_nDegree]);

	for(k=0; k<m_nDegree+1; k++)
		d[0][k] = m_arrayControlPoints[i-m_nDegree+k].y();
	for(l=1; l<=m_nDegree; l++)
	{
		for(j=l; j<=m_nDegree; j++)
		{
			denom = m_arrayKnots[i+j+1-l]-m_arrayKnots[i-m_nDegree+j];
			if(denom<MIN_DOUBLE)
				alpha = 0.0;
			else
				alpha = (u-m_arrayKnots[i-m_nDegree+j])/denom;
			d[l][j] = (1.0-alpha)*d[l-1][j-1]+alpha*d[l-1][j];
		}
	}
	rtn_P.Set_y(d[m_nDegree][m_nDegree]);

	for(k=0; k<m_nDegree+1; k++)
		d[0][k] = m_arrayControlPoints[i-m_nDegree+k].z();
	for(l=1; l<=m_nDegree; l++)
	{
		for(j=l; j<=m_nDegree; j++)
		{
			denom = m_arrayKnots[i+j+1-l]-m_arrayKnots[i-m_nDegree+j];
			if(denom<MIN_DOUBLE)
				alpha = 0.0;
			else
				alpha = (u-m_arrayKnots[i-m_nDegree+j])/denom;
			d[l][j] = (1.0-alpha)*d[l-1][j-1]+alpha*d[l-1][j];
		}
	}
	rtn_P.Set_z(d[m_nDegree][m_nDegree]);

	for(k=0; k<m_nDegree+1; k++)
		delete d[k];
	delete []d;

	return rtn_P;
}

// Set curve input type( input by control points as 'C'(default) and input by value points as 'V' )
void C3BSpline::SetInputType(char inType)
{
	if(inType=='V' || inType=='v')
		m_bInputByValuePoints = TRUE;
	else
		m_bInputByValuePoints = FALSE;
}

// Set curve input by value points
void C3BSpline::SetInputByValuePoints()
{
	m_bInputByValuePoints = TRUE;
}

// Set curve input by control points
void C3BSpline::SetInputByControlPoints()
{
	m_bInputByValuePoints = FALSE;
}

// Get curve input type( input by control points as 'C'(default) and input by value points as 'V' )
// Return type in 'char' data type
char C3BSpline::GetInputType()
{
	if(m_bInputByValuePoints)
		return 'V';
	else
		return 'C';
}

// Test if curve is input by value points
// Return logical result in 'BOOL' data type
BOOL C3BSpline::IsInputByValuePoints()
{
	return m_bInputByValuePoints;
}

// Test if curve is input by control points
// Return logical result in 'BOOL' data type
BOOL C3BSpline::IsInputByControlPoints()
{
	return !m_bInputByValuePoints;
}

// Set curve Display Count
void C3BSpline::SetDisplayCount(UINT Count)
{
	ASSERT(Count>5);
	m_nDispCount = Count;
}

// Get curve Display Count
UINT C3BSpline::GetDisplayCount()
{
	return m_nDispCount;
}

// Calculate curve display points also calculate every point's parameter and cumulating chordal length
// The value store in class member data
void C3BSpline::CalculateDisplayPoints()
{
	UINT i;
	double u;
	CPoint_3D dispP;

	ASSERT(m_nControlPointsCount>=m_nDegree+1);
	if(m_nControlPointsCount<m_nDegree+1)
		return;	
	
	if(!m_bKnotsWeightsSeted)
		SetCurveKnotsWeights();

	m_arrayDisplayPoints.RemoveAll();
	m_arrayCurrentParameter.RemoveAll();
	m_arrayCurrentDistance.RemoveAll();

	double Tolerance = (m_arrayKnots[m_nControlPointsCount]-m_arrayKnots[m_nDegree])/((m_nControlPointsCount-m_nDegree)*m_nDispCount);

	if(m_nDegree!=3)
	{
		if(m_cCurveType=='C')
		{
			u = m_arrayKnots[m_nDegree];
			while(u<=m_arrayKnots[m_nControlPointsCount])
			{
				m_arrayDisplayPoints.Add(GetPointCoordinate(u));
				m_arrayCurrentParameter.Add(u);
				u += 0.005;
			}
		}
		else if(m_cCurveType=='U' || m_cCurveType=='Q' || m_cCurveType=='N' || m_cCurveType=='P')
		{
			for(i=m_nDegree; i<m_nControlPointsCount; i++)
			{
				u = m_arrayKnots[i];
				while(u<=m_arrayKnots[i+1])
				{
					m_arrayDisplayPoints.Add(P_u(i, u));
					m_arrayCurrentParameter.Add(u);
					u += Tolerance;
				}
			}
		}
	}
	else if(m_nDegree==3)
	{
		if(m_cCurveType=='N' || m_cCurveType=='U' || m_cCurveType=='Q' || m_cCurveType=='P')
		{
			BOOL firstCal;
			for(i=m_nDegree; i<m_nControlPointsCount; i++)
			{
				u = m_arrayKnots[i];
				firstCal = TRUE;
				while(u<m_arrayKnots[i+1]+Tolerance)
				{
					if(i!=m_nDegree && firstCal)
					{
						u += Tolerance;
						firstCal = FALSE;
						continue;
					}
					if(u>m_arrayKnots[i+1])
					{
						u = m_arrayKnots[i+1];
						UINT dspCount = m_arrayDisplayPoints.GetSize();
						CPoint_3D EndP = P_u(i, u);
						double dist = Cal_two_pos_length(EndP, m_arrayDisplayPoints[dspCount-1]);
						if(dist<0.1)
						{
							m_arrayDisplayPoints[dspCount-1] = EndP;
							m_arrayCurrentParameter[dspCount-1] = u;
						}
						else
						{
							m_arrayDisplayPoints.Add(P_u(i, u));
							m_arrayCurrentParameter.Add(u);
						}
					}
					else
					{
						m_arrayDisplayPoints.Add(P_u(i, u));
						m_arrayCurrentParameter.Add(u);
					}
					u += Tolerance;
				}
			}
		}
		else
		{
			u = m_arrayKnots[m_nDegree];
			while(u<=m_arrayKnots[m_nControlPointsCount])
			{
				m_arrayDisplayPoints.Add(GetPointCoordinate(u));
				m_arrayCurrentParameter.Add(u);
				u += 0.001;
			}
		}
	}

	double dist = 0.0;
	m_arrayCurrentDistance.Add(0.0);
	UINT nCount = m_arrayDisplayPoints.GetSize();
	for(i=1; i<nCount; i++)
	{
		dist += Cal_two_pos_length(m_arrayDisplayPoints[i-1], m_arrayDisplayPoints[i]);
		m_arrayCurrentDistance.Add(dist);
	}

	m_bGetDisplayPoints = TRUE;
	m_bModified = FALSE;

	// check TP, 
	if(!m_bInputByValuePoints)
		ResetInputValuePoints(1); 
}

// Get curve display points count( sequence number From 0 to n, totally: n+1, return: n+1 )
// Return count in 'UINT' data type
UINT C3BSpline::GetDisplayPointsCount()
{
	if(m_bModified)
		CalculateDisplayPoints();
	else if(!m_bGetDisplayPoints)
		CalculateDisplayPoints();
	return m_arrayDisplayPoints.GetSize();
}

// Get curve control point value by index( begin from 0 to n )
// Modify input parameter( 3 of double) as return
void C3BSpline::GetDisplayPoint(UINT nIndex, double &x, double &y, double &z)
{
	if(m_bModified)
		CalculateDisplayPoints();
	else if(!m_bGetDisplayPoints)
		CalculateDisplayPoints();
	ASSERT((int)nIndex<m_arrayDisplayPoints.GetSize());
	if((int)nIndex>=m_arrayDisplayPoints.GetSize())
		return;
	x = m_arrayDisplayPoints[nIndex].x();
	y = m_arrayDisplayPoints[nIndex].y();
	z = m_arrayDisplayPoints[nIndex].z();
}

// Get curve control point value by index( begin from 0 to n )
// Return the value in 'position' data type
CPoint_3D C3BSpline::GetDisplayPoint(UINT nIndex)
{
	if(m_bModified)
		CalculateDisplayPoints();
	else if(!m_bGetDisplayPoints)
		CalculateDisplayPoints();

	CPoint_3D rtnP;

	ASSERT((int)nIndex<m_arrayDisplayPoints.GetSize());
	if((int)nIndex>=m_arrayDisplayPoints.GetSize())
		return rtnP;

	rtnP = m_arrayDisplayPoints[nIndex];

	return rtnP;
}

// get start point of the curve
CPoint_3D C3BSpline::Start_pos()
{
	if(m_bModified)
		CalculateDisplayPoints();
	else if(!m_bGetDisplayPoints)
		CalculateDisplayPoints();

	return m_arrayDisplayPoints[0];
}

// get end point of the curve
CPoint_3D C3BSpline::End_pos()
{
	if(m_bModified)
		CalculateDisplayPoints();
	else if(!m_bGetDisplayPoints)
		CalculateDisplayPoints();
	
	int nCount = m_arrayDisplayPoints.GetSize();
	return m_arrayDisplayPoints[nCount-1];
}

// Add curve's input point
int C3BSpline::AddInputPoints(CPoint_3D new_InP)
{
	m_arrayInputPoints.Add(new_InP);
	m_bModified = TRUE;
	return m_arrayInputPoints.GetSize();
}

// Get input value points count
int C3BSpline::GetValuePointsCount()
{
	if(!m_bInputByValuePoints)
		return 0;
	else
		return m_arrayInputPoints.GetSize();
}

// Set curve input points count, this function is ready for the function "SetInputPoint"
void C3BSpline::SetInputPointsCount(UINT nCount)
{
	m_arrayInputPoints.RemoveAll();
	m_arrayInputPoints.SetSize(nCount);
}

// Set curve input point by index and the position of point
void C3BSpline::SetInputPoint(UINT nIndex, const CPoint_3D mod_InP)
{
	ASSERT((int)nIndex<m_arrayInputPoints.GetSize());
	if((int)nIndex>=m_arrayInputPoints.GetSize())
		return;
	m_arrayInputPoints[nIndex] = mod_InP;
	m_bModified = TRUE;
}

// Set curve input point by index and the X, Y, Z value of point
void C3BSpline::SetInputPoint(UINT nIndex, double x, double y, double z)
{
	ASSERT((int)nIndex<m_arrayInputPoints.GetSize());
	if((int)nIndex>=m_arrayInputPoints.GetSize())
		return;
	m_arrayInputPoints[nIndex].Set_xyz(x, y, z);
	m_bModified = TRUE;
}

// Get curve input points count
int C3BSpline::GetInputPointsCount()
{
	return m_arrayInputPoints.GetSize();
}

// Get curve input point value by index( begin from 0 to n )
// Modify input parameter( 3 of double) as return
void C3BSpline::GetInputPoint(UINT nIndex, double &x, double &y, double &z)
{
	ASSERT((int)nIndex<m_arrayInputPoints.GetSize());
	if((int)nIndex>=m_arrayInputPoints.GetSize())
		return;

	x = m_arrayInputPoints[nIndex].x();
	y = m_arrayInputPoints[nIndex].y();
	z = m_arrayInputPoints[nIndex].z();
}

// Get curve input point value by index( begin from 0 to n )
// Return the value in 'position' data type
CPoint_3D C3BSpline::GetInputPoint(UINT nIndex)
{
	CPoint_3D rtnP;

	ASSERT((int)nIndex<m_arrayInputPoints.GetSize());
	if((int)nIndex>=m_arrayInputPoints.GetSize())
		return rtnP;

	rtnP = m_arrayInputPoints[nIndex];

	return rtnP;
}

// Get curve input point parameter by index( begin from 0 to n )
double C3BSpline::GetInputPointPara(UINT nIndex)
{
	ASSERT(m_bInputByValuePoints);
	if(!m_bInputByValuePoints)
		return MAX_DOUBLE;
	ASSERT(m_bKnotsWeightsSeted);
	if(!m_bKnotsWeightsSeted)
		return MAX_DOUBLE;
	ASSERT((int)nIndex<m_arrayInputPoints.GetSize());
	if((int)nIndex>=m_arrayInputPoints.GetSize())
		return MAX_DOUBLE;

	return m_arrayInPntPara[nIndex];
}

// Clear all input points
void C3BSpline::ClearInputPoints()
{
	m_arrayInputPoints.RemoveAll();
	m_bModified = TRUE;
}

// Init value points
void C3BSpline::InitValuePoints()
{
	ASSERT(m_bInputByValuePoints);
	if(!m_bInputByValuePoints)
		return;
	
	// valid number check
	int  node_num = m_arrayInputPoints.GetSize();
	if(node_num==2) return;
	
	for(int i=1; i<m_arrayInputPoints.GetSize(); i++)
	{
		if(Dist_2P(i)<1.0E-6)
		{
			m_arrayInputPoints.RemoveAt(i);
			i--;
			
			// valid number check
			if(m_arrayInputPoints.GetSize() <= 2)
				return;
		}
	}
}

// Set curve base vector to move the curve in vector value
void C3BSpline::SetBaseVector(double x, double y, double z)
{
	int i;
	int nCount = m_arrayInputPoints.GetSize();

	m_BaseVector.Set_xyz(x, y, z);

	if(m_bInputByValuePoints)
	{
		for(i=0; i<nCount; i++)
		{
			m_arrayInputPoints[i].Set_x(m_arrayInputPoints[i].x()+m_BaseVector.x());
			m_arrayInputPoints[i].Set_y(m_arrayInputPoints[i].y()+m_BaseVector.y());
			m_arrayInputPoints[i].Set_z(m_arrayInputPoints[i].z()+m_BaseVector.z());
		}
	}

	for(i=0; i<(int)m_nControlPointsCount; i++)
	{
		m_arrayControlPoints[i].Set_x(m_arrayControlPoints[i].x()+m_BaseVector.x());
		m_arrayControlPoints[i].Set_y(m_arrayControlPoints[i].y()+m_BaseVector.y());
		m_arrayControlPoints[i].Set_z(m_arrayControlPoints[i].z()+m_BaseVector.z());
	}

	CalculateDisplayPoints();
}

// Get curve last movement value
// Modify input parameter as return	
void C3BSpline::GetBaseVector(double &x, double &y, double &z)
{
	x = m_BaseVector.x();
	y = m_BaseVector.y();
	z = m_BaseVector.z();
}

// Get curve control points by input points. If the input points are control points, we set the knots and weights by 'Mode' input.
// If the input points are value points, we calculate the control points by 'Mode' and set knots and weights in Mode=4.
// That is to say: 'Mode' has different meanings in different input type.
void C3BSpline::GetControlPointsByInputPoints(int Mode)
{
	UINT nCount = m_arrayInputPoints.GetSize();
	UINT i;

	if(!m_bInputByValuePoints)
	{
		SetControlPointsCount(nCount);
		for(i=0; i<nCount; i++)
			m_arrayControlPoints[i] = m_arrayInputPoints[i];
		SetCurveKnotsWeights(Mode);
	}
	else
		GetCPByVP(Mode);//GetControlPointsByValuePoints(Mode);
}

//函数功能：全选主元高斯消去法求解n阶线性代数方程组。
//返回值：	本函数返回一个整型标志值。若返回的标志值为0，则表示原方程组的系数矩阵奇异，弹出"fail"对话框；
//			若返回非0标志值，则表示正常返回。
//形参说明：a——双精度实型一维数组，长度为n*n。存放方程组的系数矩阵，返回时将被破坏。
//			b——双精度实型一维数组，长度为n。存放方程组右端的常数向量；返回方程组的解向量。
//			n——整型变量。存放方程组的阶数。
int agaus(double a[], double b[], int n)
{
	ASSERT(n>1);
	if(n<2)
	{
		//AfxMessageBox("error !");
		return 0;
	}

	int *js, i, j, k, l, is, p, q;
	double d, t;

	js = new int[n];
	l = 1;
	for(k=0; k<=n-2; k++)
	{
		d = 0.0;
		for(i=k; i<=n-1; i++)
		{
			for(j=k; j<=n-1; j++)
			{
				t = fabs(a[i*n+j]);
				if(t>d)
				{
					d = t;
					js[k] = j;
					is = i;
				}
			}
		}
		if(d+1.0==1.0)
			l = 0;
		else
		{
			if(js[k]!=k)
			{
				for(i=0;i<=n-1;i++)
				{
					p = i*n+k;
					q = i*n+js[k];
					t = a[p];
					a[p] = a[q];
					a[q] = t;
				}
			}
			if(is!=k)
			{
				for(j=k; j<=n-1; j++)
				{
					p = k*n+j;
					q = is*n+j;
					t = a[p];
					a[p] = a[q];
					a[q] = t;
				}
				t = b[k];
				b[k] = b[is];
				b[is] = t;
			}
		}
		if(l==0)
		{
			delete js;
			//AfxMessageBox("error !");
			return 0;
		}
		d = a[k*n+k];
		for(j=k+1; j<=n-1; j++)
		{
			p = k*n+j;
			a[p] = a[p]/d;
		}
		b[k] = b[k]/d;
		for(i=k+1; i<=n-1; i++)
		{
			for(j=k+1; j<=n-1; j++)
			{
				p = i*n+j;
				a[p] = a[p]-a[i*n+k]*a[k*n+j];
			}
			b[i] = b[i]-a[i*n+k]*b[k];
		}
	}
	d = a[(n-1)*n+n-1];
	if(fabs(d)+1.0==1.0)
	{
		delete js;
		//AfxMessageBox("error !");
		return 0;
	}
	b[n-1] = b[n-1]/d;
	for(i=n-2; i>=0; i--)
	{
		t = 0.0;
		for(j=i+1; j<=n-1; j++)
			t = t+a[i*n+j]*b[j];
		b[i] = b[i]-t;
	}
	js[n-1] = n-1;
	for(k=n-1; k>=0; k--)
	{
		if(js[k]!=k)
		{
			t = b[k];
			b[k] = b[js[k]];
			b[js[k]] = t;
		}
	}
	
	delete js;
	return 1;
}

//函数功能：追赶法求解三对角线方程组
//返回值：	本函数返回一个整型标志值。若返回的标志值小于0，则表示输入的参数不正确，弹出"error"对话框；
//			若返回的标志值为0，则表示程序工作失败（如主元素的绝对值太小或为0），弹出"fail"对话框；
//			若返回的标志值大于0，则表示正常返回。
//形参说明：b——双精度实型一维数组，长度为m。以行为主存放三对角线矩阵中三对角线上的元素，即在b中依次存放元素：
//				a00，a01，a10，a11，a12，a21，a22，a23，…，an-1,an-2，an-1,an-1	此数组在返回时被破坏。
//			n——整型变量。方程组的阶数。
//			m——整型变量。n阶三对角矩阵三条对角线上的元素个数，也是一维数组b的长度。它的值应为m=3n-2。
//			d——双精度实型一维数组，长度为n。存放方程组右端的常数向量；返回时存放方程组的解向量。
int atrde(double b[], int n, int m, double d[])
{
	ASSERT(n>1);
	if(n<2)
	{
		//AfxMessageBox("error !");
		return -1;
	}
	ASSERT(m==(3*n-2));
	if(m!=(3*n-2))
	{
		//AfxMessageBox("error !");
		return -1;
	}

	int k, j;
	double s;

	for(k=0; k<=n-2; k++)
	{
		j = 3*k;
		s = b[j];
		if(fabs(s)+1.0==1.0)
		{
			//AfxMessageBox("error !");
			return 0;
		}
		b[j+1] = b[j+1]/s;
		d[k] = d[k]/s;
		b[j+3] = b[j+3]-b[j+2]*b[j+1];
		d[k+1] = d[k+1]-b[j+2]*d[k];
	}
	s = b[3*n-3];
	if(fabs(s)+1.0==1.0)
	{
		//AfxMessageBox("error !");
		return 0;
	}
	d[n-1] = d[n-1]/s;
	for(k=n-2; k>=0; k--)
		d[k] = d[k]-b[3*k+1]*d[k+1];
	return 1;
}

//根据输入的型值点，反算控制点。注意：这里用到的节点序列是均匀的，即从-m_nDegree顺序加1递增到m_nControlPointsCount+m_nDegree
//Mode：不同边界条件的情况
//		==1	首末两点过Q1和Qn的非周期三次B样条曲线：即有P1=Q1，Pn=Qn。为使曲线首末两点过Q1和Qn，需要两个附加顶点P0=2*P1-P2，Pn+1=2Pn-Pn-1。
//		==2	封闭周期的三次B样条曲线：即有P0=Pn，Pn+1=P1。
//		==3	端点有二重控制点的三次B样条曲线：即有P0=P1，Pn+1=Pn。
//		==4	给定始、终点的切矢量Q`1、Q`n（实际计算中取首末型值点连线的方向）
//			在始点，由于Q`1=(P2-P0)/2，Q1=(P0+4*P1+P2)/6，有：P0=P2-2*Q`1，2*P1/3+P2/3=Q1+Q`1/3
//			在终点，由于Q`n=(Pn+1-Pn)/2，Qn=(Pn-1+4*Pn+Pn+1)/6，有：Pn-1=Pn+1-2*Q`n，Pn-1/3+2*Pn/3=Qn-Q`n/3
// Calculate control points by input value points. Here the knots sequence is uniform: from (-m_nDegree) add 1 to (m_nControlPointsCount+m_nDegree)
// P: Control Points; Q: Value Points; Q`: Unit Vector of Value Points
// Mode==1	Condition: P1=Q1, Pn=Qn  ==>>  P0=2*P1-P2, Pn+1=2Pn-Pn-1
// Mode==2	Closed period B-Spline (m_nDegree==3)  ==>>  P0=Pn, Pn+1=P1
// Mode==3	P0=Pn, Pn=1=Pn
// Mode==4	Know Q`1 and Q`n (we calculate it with input points)
//			Q`1=(P2-P0)/2, Q1=(P0+4*P1+P2)/6  ==>>  P0=P2-2*Q`1, 2*P1/3+P2/3=Q1+Q`1/3
//			Q`n=(Pn+1-Pn)/2, Qn=(Pn-1+4*Pn+Pn+1)/6  ==>>  Pn-1=Pn+1-2*Q`n, Pn-1/3+2*Pn/3=Qn-Q`n/3
void C3BSpline::GetControlPointsByValuePoints(int Mode)
{
	ASSERT(Mode>0 && Mode<5);
	if(Mode<=0 || Mode>4)
		Mode = 3;	//Set 3 as Default
	
	ASSERT(m_bInputByValuePoints);
	if(!m_bInputByValuePoints)
		return;
	int nCount = m_arrayInputPoints.GetSize();

	m_nDegree = 3;
	
	ASSERT(nCount>=(int)m_nDegree-1);
	if(nCount<(int)m_nDegree-1)
		return;

	int i, j;

	if(Mode==1)		//首末两点过Q1和Qn的非周期三次B样条曲线
	{
		double *cvpx = new double[nCount];
		double *cvpy = new double[nCount];
		double *cvpz = new double[nCount];
		double *matrix = new double[3*nCount-2];

		SetControlPointsCount(nCount+2);
		SetCurveKnotsWeights(1);

		for(i=0; i<nCount; i++)
		{
			cvpx[i] = 6.0*m_arrayInputPoints[i].x();
			cvpy[i] = 6.0*m_arrayInputPoints[i].y();
			cvpz[i] = 6.0*m_arrayInputPoints[i].z();
		}

		matrix[0] = 6.0;
		matrix[1] = 0.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = 1.0;
			matrix[3*i] = 4.0;
			matrix[3*i+1] = 1.0;
		}
		matrix[3*nCount-4] = 0.0;
		matrix[3*nCount-3] = 6.0;
		atrde(matrix, nCount, 3*nCount-2, cvpx);

		matrix[0] = 6.0;
		matrix[1] = 0.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = 1.0;
			matrix[3*i] = 4.0;
			matrix[3*i+1] = 1.0;
		}
		matrix[3*nCount-4] = 0.0;
		matrix[3*nCount-3] = 6.0;
		atrde(matrix, nCount, 3*nCount-2, cvpy);

		matrix[0] = 6.0;
		matrix[1] = 0.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = 1.0;
			matrix[3*i] = 4.0;
			matrix[3*i+1] = 1.0;
		}
		matrix[3*nCount-4] = 0.0;
		matrix[3*nCount-3] = 6.0;
		atrde(matrix, nCount, 3*nCount-2, cvpz);

		m_arrayControlPoints[0].Set_xyz(2.0*cvpx[0]-cvpx[1], 2.0*cvpy[0]-cvpy[1], 2.0*cvpz[0]-cvpz[1]);
		for(i=0; i<nCount; i++)
			SetControlPoint(i+1, cvpx[i], cvpy[i], cvpz[i]);
		m_arrayControlPoints[nCount+1].Set_xyz(2.0*cvpx[nCount-1]-cvpx[nCount-2], 2.0*cvpy[nCount-1]-cvpy[nCount-2], 2.0*cvpz[nCount-1]-cvpz[nCount-2]);

		delete cvpx;
		delete cvpy;
		delete cvpz;
		delete matrix;
	}
	else if(Mode==2)	//封闭周期的三次B样条曲线
	{
		double *cvpx = new double[nCount];
		double *cvpy = new double[nCount];
		double *cvpz = new double[nCount];
		double *matrix = new double[nCount*nCount];

		SetControlPointsCount(nCount+2);
		SetCurveKnotsWeights(1);

		for(i=0; i<nCount; i++)
		{
			cvpx[i] = 6.0*m_arrayInputPoints[i].x();
			cvpy[i] = 6.0*m_arrayInputPoints[i].y();
			cvpz[i] = 6.0*m_arrayInputPoints[i].z();
		}

		for(i=0; i<nCount; i++)
			for(j=0; j<nCount; j++)
				matrix[i*nCount+j] = 0.0;
		matrix[0] = 4.0;
		matrix[1] = 1.0;
		matrix[nCount-1] = 1.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[i*nCount+i-1] = 1.0;
			matrix[i*nCount+i] = 4.0;
			matrix[i*nCount+i+1] = 1.0;
		}
		matrix[(nCount-1)*nCount] = 1.0;
		matrix[nCount*nCount-2] = 1.0;
		matrix[nCount*nCount-1] = 4.0;
		agaus(matrix, cvpx, nCount);

		for(i=0; i<nCount; i++)
			for(j=0; j<nCount; j++)
				matrix[i*nCount+j] = 0.0;
		matrix[0] = 4.0;
		matrix[1] = 1.0;
		matrix[nCount-1] = 1.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[i*nCount+i-1] = 1.0;
			matrix[i*nCount+i] = 4.0;
			matrix[i*nCount+i+1] = 1.0;
		}
		matrix[(nCount-1)*nCount] = 1.0;
		matrix[nCount*nCount-2] = 1.0;
		matrix[nCount*nCount-1] = 4.0;
		agaus(matrix, cvpy, nCount);

		for(i=0; i<nCount; i++)
			for(j=0; j<nCount; j++)
				matrix[i*nCount+j] = 0.0;
		matrix[0] = 4.0;
		matrix[1] = 1.0;
		matrix[nCount-1] = 1.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[i*nCount+i-1] = 1.0;
			matrix[i*nCount+i] = 4.0;
			matrix[i*nCount+i+1] = 1.0;
		}
		matrix[(nCount-1)*nCount] = 1.0;
		matrix[nCount*nCount-2] = 1.0;
		matrix[nCount*nCount-1] = 4.0;
		agaus(matrix, cvpz, nCount);

		m_arrayControlPoints[0].Set_xyz(cvpx[nCount-1], cvpy[nCount-1], cvpz[nCount-1]);
		for(i=0; i<nCount; i++)
			SetControlPoint(i+1, cvpx[i], cvpy[i], cvpz[i]);
		m_arrayControlPoints[nCount+1].Set_xyz(cvpx[0], cvpy[0], cvpz[0]);

		delete cvpx;
		delete cvpy;
		delete cvpz;
		delete matrix;
	}
	else if(Mode==3)	//端点有二重控制点的三次B样条曲线
	{
		double *cvpx = new double[nCount+2];
		double *cvpy = new double[nCount+2];
		double *cvpz = new double[nCount+2];
		double *matrix = new double[3*nCount+4];

		SetControlPointsCount(nCount+2);
		SetCurveKnotsWeights(1);

		cvpx[0] = cvpy[0] = cvpz[0] = 0.0;
		for(i=1; i<=nCount; i++)
		{
			cvpx[i] = 6.0*m_arrayInputPoints[i-1].x();
			cvpy[i] = 6.0*m_arrayInputPoints[i-1].y();
			cvpz[i] = 6.0*m_arrayInputPoints[i-1].z();
		}
		cvpx[nCount+1] = cvpy[nCount+1] = cvpz[nCount+1] = 0.0;

		matrix[0] = 6.0;
		matrix[1] = -6.0;
		for(i=1; i<nCount+1; i++)
		{
			matrix[3*i-1] = 1.0;
			matrix[3*i] = 4.0;
			matrix[3*i+1] = 1.0;
		}
		matrix[3*nCount+2] = -6.0;
		matrix[3*nCount+3] = 6.0;
		atrde(matrix, nCount+2, 3*nCount+4, cvpx);

		matrix[0] = 6.0;
		matrix[1] = -6.0;
		for(i=1; i<nCount+1; i++)
		{
			matrix[3*i-1] = 1.0;
			matrix[3*i] = 4.0;
			matrix[3*i+1] = 1.0;
		}
		matrix[3*nCount+2] = -6.0;
		matrix[3*nCount+3] = 6.0;
		atrde(matrix, nCount+2, 3*nCount+4, cvpy);

		matrix[0] = 6.0;
		matrix[1] = -6.0;
		for(i=1; i<nCount+1; i++)
		{
			matrix[3*i-1] = 1.0;
			matrix[3*i] = 4.0;
			matrix[3*i+1] = 1.0;
		}
		matrix[3*nCount+2] = -6.0;
		matrix[3*nCount+3] = 6.0;
		atrde(matrix, nCount+2, 3*nCount+4, cvpz);

		for(i=0; i<nCount+2; i++)
			SetControlPoint(i, cvpx[i], cvpy[i], cvpz[i]);

		delete cvpx;
		delete cvpy;
		delete cvpz;
		delete matrix;
	}
	else if(Mode==4)	//给定始、终点的切矢量Q`1、Q`n（实际计算中取首末型值点连线的方向）
	{
		double *cvpx = new double[nCount];
		double *cvpy = new double[nCount];
		double *cvpz = new double[nCount];
		double *matrix = new double[3*nCount-2];

		double dist0, fx0, fy0, fz0;
		double distn, fxn, fyn, fzn;

		SetControlPointsCount(nCount+2);
		SetCurveKnotsWeights(1);

		dist0 = sqrt((m_arrayInputPoints[1].x()-m_arrayInputPoints[0].x())*(m_arrayInputPoints[1].x()-m_arrayInputPoints[0].x())
			+(m_arrayInputPoints[1].y()-m_arrayInputPoints[0].y())*(m_arrayInputPoints[1].y()-m_arrayInputPoints[0].y())
			+(m_arrayInputPoints[1].z()-m_arrayInputPoints[0].z())*(m_arrayInputPoints[1].z()-m_arrayInputPoints[0].z()));
		fx0 = (m_arrayInputPoints[1].x()-m_arrayInputPoints[0].x())/dist0;
		fy0 = (m_arrayInputPoints[1].y()-m_arrayInputPoints[0].y())/dist0;
		fz0 = (m_arrayInputPoints[1].z()-m_arrayInputPoints[0].z())/dist0;
		distn = sqrt((m_arrayInputPoints[nCount-1].x()-m_arrayInputPoints[nCount-2].x())*(m_arrayInputPoints[nCount-1].x()-m_arrayInputPoints[nCount-2].x())
			+(m_arrayInputPoints[nCount-1].y()-m_arrayInputPoints[nCount-2].y())*(m_arrayInputPoints[nCount-1].y()-m_arrayInputPoints[nCount-2].y())
			+(m_arrayInputPoints[nCount-1].z()-m_arrayInputPoints[nCount-2].z())*(m_arrayInputPoints[nCount-1].z()-m_arrayInputPoints[nCount-2].z()));
		fxn = (m_arrayInputPoints[nCount-1].x()-m_arrayInputPoints[nCount-2].x())/distn;
		fyn = (m_arrayInputPoints[nCount-1].y()-m_arrayInputPoints[nCount-2].y())/distn;
		fzn = (m_arrayInputPoints[nCount-1].z()-m_arrayInputPoints[nCount-2].z())/distn;

		cvpx[0] = 6.0*m_arrayInputPoints[0].x()+2.0*fx0;
		cvpy[0] = 6.0*m_arrayInputPoints[0].y()+2.0*fy0;
		cvpz[0] = 6.0*m_arrayInputPoints[0].z()+2.0*fz0;
		for(i=1; i<nCount-1; i++)
		{
			cvpx[i] = 6.0*m_arrayInputPoints[i].x();
			cvpy[i] = 6.0*m_arrayInputPoints[i].y();
			cvpz[i] = 6.0*m_arrayInputPoints[i].z();
		}
		cvpx[nCount-1] = 6.0*m_arrayInputPoints[nCount-1].x()-2.0*fxn;
		cvpy[nCount-1] = 6.0*m_arrayInputPoints[nCount-1].y()-2.0*fyn;
		cvpz[nCount-1] = 6.0*m_arrayInputPoints[nCount-1].z()-2.0*fzn;

		matrix[0] = 4.0;
		matrix[1] = 2.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = 1.0;
			matrix[3*i] = 4.0;
			matrix[3*i+1] = 1.0;
		}
		matrix[3*nCount-4] = 2.0;
		matrix[3*nCount-3] = 4.0;
		atrde(matrix, nCount, 3*nCount-2, cvpx);

		matrix[0] = 4.0;
		matrix[1] = 2.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = 1.0;
			matrix[3*i] = 4.0;
			matrix[3*i+1] = 1.0;
		}
		matrix[3*nCount-4] = 2.0;
		matrix[3*nCount-3] = 4.0;
		atrde(matrix, nCount, 3*nCount-2, cvpy);

		matrix[0] = 4.0;
		matrix[1] = 2.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = 1.0;
			matrix[3*i] = 4.0;
			matrix[3*i+1] = 1.0;
		}
		matrix[3*nCount-4] = 2.0;
		matrix[3*nCount-3] = 4.0;
		atrde(matrix, nCount, 3*nCount-2, cvpz);

		m_arrayControlPoints[0].Set_xyz(cvpx[1]-2.0*fx0, cvpy[1]-2.0*fy0, cvpz[1]-2.0*fz0);
		for(i=0; i<nCount; i++)
			SetControlPoint(i+1, cvpx[i], cvpy[i], cvpz[i]);
		m_arrayControlPoints[nCount+1].Set_xyz(cvpx[nCount-2]+2.0*fxn, cvpy[nCount-2]+2.0*fyn, cvpz[nCount-2]+2.0*fzn);

		delete cvpx;
		delete cvpy;
		delete cvpz;
		delete matrix;
	}
	CalculateDisplayPoints();
}

//根据输入的型值点，反算控制点。注意：采用累积弦长法进行节点设置（即非均匀节点序列）
//Mode：不同边界条件的情况
//		==1	首末两点过Q1和Qn的非周期三次B样条曲线：即有P1=Q1，Pn=Qn。为使曲线首末两点过Q1和Qn，需要两个附加顶点P0=2*P1-P2，Pn+1=2Pn-Pn-1。
//		==2	封闭周期的三次B样条曲线：即有P0=Pn，Pn+1=P1。
//		==3	端点有二重控制点的三次B样条曲线：即有P0=P1，Pn+1=Pn。
//		==4	给定始、终点的切矢量Q`1、Q`n（实际计算中取首末型值点连线的方向）
//			在始点，由于Q`1=(P2-P0)/2，Q1=(P0+4*P1+P2)/6，有：P0=P2-2*Q`1，2*P1/3+P2/3=Q1+Q`1/3
//			在终点，由于Q`n=(Pn+1-Pn)/2，Qn=(Pn-1+4*Pn+Pn+1)/6，有：Pn-1=Pn+1-2*Q`n，Pn-1/3+2*Pn/3=Qn-Q`n/3
// Calculate control points by input value points. Here the knots sequence is set by cumulating bowstring length
// P: Control Points; Q: Value Points; Q`: Unit Vector of Value Points
// Mode==1	Condition: P1=Q1, Pn=Qn  ==>>  P0=2*P1-P2, Pn+1=2Pn-Pn-1
// Mode==2	Closed period B-Spline (m_nDegree==3)  ==>>  P0=Pn, Pn+1=P1
// Mode==3	P0=Pn, Pn=1=Pn
// Mode==4	Know Q`1 and Q`n (we calculate it with input points)
//			Q`1=(P2-P0)/2, Q1=(P0+4*P1+P2)/6  ==>>  P0=P2-2*Q`1, 2*P1/3+P2/3=Q1+Q`1/3
//			Q`n=(Pn+1-Pn)/2, Qn=(Pn-1+4*Pn+Pn+1)/6  ==>>  Pn-1=Pn+1-2*Q`n, Pn-1/3+2*Pn/3=Qn-Q`n/3
void C3BSpline::GetCPByVP(int Mode)
{
	int i, j;

	ASSERT(Mode>0 && Mode<5);
	if(Mode<=0 || Mode>4)
		Mode = 3;	//Set 3 as Default
	
	ASSERT(m_bInputByValuePoints);
	if(!m_bInputByValuePoints)
		return;

	InitValuePoints(); // TP position test

	int nCount = m_arrayInputPoints.GetSize();

	m_nDegree = 3;
	
	ASSERT(nCount>=(int)m_nDegree-1);
	if(nCount<(int)m_nDegree-1)
		return;

	if(Mode==1)		//首末两点过Q1和Qn的非周期三次B样条曲线
	{
		double *cvpx = new double[nCount];
		double *cvpy = new double[nCount];
		double *cvpz = new double[nCount];
		double *matrix = new double[3*nCount-2];

		SetControlPointsCount(nCount+2);
		SetCurveKnotsWeights(4);

		for(i=0; i<nCount; i++)
		{
			cvpx[i] = m_arrayInputPoints[i].x();
			cvpy[i] = m_arrayInputPoints[i].y();
			cvpz[i] = m_arrayInputPoints[i].z();
		}

		matrix[0] = 1.0;
		matrix[1] = 0.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = N_u3(i, m_arrayKnots[i+3]);
			matrix[3*i] = N_u3(i+1, m_arrayKnots[i+3]);
			matrix[3*i+1] = N_u3(i+2, m_arrayKnots[i+3]);
		}
		matrix[3*nCount-4] = 0.0;
		matrix[3*nCount-3] = 1.0;
		atrde(matrix, nCount, 3*nCount-2, cvpx);

		matrix[0] = 1.0;
		matrix[1] = 0.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = N_u3(i, m_arrayKnots[i+3]);
			matrix[3*i] = N_u3(i+1, m_arrayKnots[i+3]);
			matrix[3*i+1] = N_u3(i+2, m_arrayKnots[i+3]);
		}
		matrix[3*nCount-4] = 0.0;
		matrix[3*nCount-3] = 1.0;
		atrde(matrix, nCount, 3*nCount-2, cvpy);

		matrix[0] = 1.0;
		matrix[1] = 0.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = N_u3(i, m_arrayKnots[i+3]);
			matrix[3*i] = N_u3(i+1, m_arrayKnots[i+3]);
			matrix[3*i+1] = N_u3(i+2, m_arrayKnots[i+3]);
		}
		matrix[3*nCount-4] = 0.0;
		matrix[3*nCount-3] = 1.0;
		atrde(matrix, nCount, 3*nCount-2, cvpz);

		m_arrayControlPoints[0].Set_xyz(m_arrayInputPoints[0].x(), m_arrayInputPoints[0].y(), m_arrayInputPoints[0].z());
		for(i=0; i<nCount; i++)
			SetControlPoint(i+1, cvpx[i], cvpy[i], cvpz[i]);
		m_arrayControlPoints[nCount+1].Set_xyz(m_arrayInputPoints[nCount-1].x(), m_arrayInputPoints[nCount-1].y(), m_arrayInputPoints[nCount-1].z());

		delete cvpx;
		delete cvpy;
		delete cvpz;
		delete matrix;
	}
	else if(Mode==2)	//封闭周期的三次B样条曲线
	{
		double *cvpx = new double[nCount];
		double *cvpy = new double[nCount];
		double *cvpz = new double[nCount];
		double *matrix = new double[nCount*nCount];

		SetControlPointsCount(nCount+2);
		SetCurveKnotsWeights(4);

		for(i=0; i<nCount; i++)
		{
			cvpx[i] = m_arrayInputPoints[i].x();
			cvpy[i] = m_arrayInputPoints[i].y();
			cvpz[i] = m_arrayInputPoints[i].z();
		}

		for(i=0; i<nCount; i++)
			for(j=0; j<nCount; j++)
				matrix[i*nCount+j] = 0.0;
		matrix[0] = N_u3(1, m_arrayKnots[3]);
		matrix[1] = N_u3(2, m_arrayKnots[3]);
		matrix[nCount-1] = N_u3(0, m_arrayKnots[3]);
		for(i=1; i<nCount-1; i++)
		{
			matrix[i*nCount+i-1] = N_u3(i, m_arrayKnots[i+3]);
			matrix[i*nCount+i] = N_u3(i+1, m_arrayKnots[i+3]);
			matrix[i*nCount+i+1] = N_u3(i+2, m_arrayKnots[i+3]);
		}
		matrix[(nCount-1)*nCount] = N_u3(nCount+1, m_arrayKnots[nCount+2]);
		matrix[nCount*nCount-2] = N_u3(nCount-1, m_arrayKnots[nCount+2]);
		matrix[nCount*nCount-1] = N_u3(nCount, m_arrayKnots[nCount+2]);
		agaus(matrix, cvpx, nCount);

		for(i=0; i<nCount; i++)
			for(j=0; j<nCount; j++)
				matrix[i*nCount+j] = 0.0;
		matrix[0] = N_u3(1, m_arrayKnots[3]);
		matrix[1] = N_u3(2, m_arrayKnots[3]);
		matrix[nCount-1] = N_u3(0, m_arrayKnots[3]);
		for(i=1; i<nCount-1; i++)
		{
			matrix[i*nCount+i-1] = N_u3(i, m_arrayKnots[i+3]);
			matrix[i*nCount+i] = N_u3(i+1, m_arrayKnots[i+3]);
			matrix[i*nCount+i+1] = N_u3(i+2, m_arrayKnots[i+3]);
		}
		matrix[(nCount-1)*nCount] = N_u3(nCount+1, m_arrayKnots[nCount+2]);
		matrix[nCount*nCount-2] = N_u3(nCount-1, m_arrayKnots[nCount+2]);
		matrix[nCount*nCount-1] = N_u3(nCount, m_arrayKnots[nCount+2]);
		agaus(matrix, cvpy, nCount);

		for(i=0; i<nCount; i++)
			for(j=0; j<nCount; j++)
				matrix[i*nCount+j] = 0.0;
		matrix[0] = N_u3(1, m_arrayKnots[3]);
		matrix[1] = N_u3(2, m_arrayKnots[3]);
		matrix[nCount-1] = N_u3(0, m_arrayKnots[3]);
		for(i=1; i<nCount-1; i++)
		{
			matrix[i*nCount+i-1] = N_u3(i, m_arrayKnots[i+3]);
			matrix[i*nCount+i] = N_u3(i+1, m_arrayKnots[i+3]);
			matrix[i*nCount+i+1] = N_u3(i+2, m_arrayKnots[i+3]);
		}
		matrix[(nCount-1)*nCount] = N_u3(nCount+1, m_arrayKnots[nCount+2]);
		matrix[nCount*nCount-2] = N_u3(nCount-1, m_arrayKnots[nCount+2]);
		matrix[nCount*nCount-1] = N_u3(nCount, m_arrayKnots[nCount+2]);
		agaus(matrix, cvpz, nCount);

		m_arrayControlPoints[0].Set_xyz(cvpx[nCount-1], cvpy[nCount-1], cvpz[nCount-1]);
		for(i=0; i<nCount; i++)
			SetControlPoint(i+1, cvpx[i], cvpy[i], cvpz[i]);
		m_arrayControlPoints[nCount+1].Set_xyz(cvpx[0], cvpy[0], cvpz[0]);

		delete cvpx;
		delete cvpy;
		delete cvpz;
		delete matrix;
	}
	else if(Mode==3)	//端点有二重控制点的三次B样条曲线
	{
		double *cvpx = new double[nCount+2];
		double *cvpy = new double[nCount+2];
		double *cvpz = new double[nCount+2];
		double *matrix = new double[3*nCount+4];

		SetControlPointsCount(nCount+2);
		SetCurveKnotsWeights(4);

		cvpx[0] = cvpy[0] = cvpz[0] = 0.0;
		for(i=1; i<=nCount; i++)
		{
			cvpx[i] = m_arrayInputPoints[i-1].x();
			cvpy[i] = m_arrayInputPoints[i-1].y();
			cvpz[i] = m_arrayInputPoints[i-1].z();
		}
		cvpx[nCount+1] = cvpy[nCount+1] = cvpz[nCount+1] = 0.0;

		matrix[0] = 1.0;
		matrix[1] = -1.0;
		for(i=1; i<nCount+1; i++)
		{
			matrix[3*i-1] = N_u3(i-1, m_arrayKnots[i+2]);
			matrix[3*i] = N_u3(i, m_arrayKnots[i+2]);
			matrix[3*i+1] = N_u3(i+1, m_arrayKnots[i+2]);
		}
		matrix[3*nCount+2] = -1.0;
		matrix[3*nCount+3] = 1.0;
		atrde(matrix, nCount+2, 3*nCount+4, cvpx);

		matrix[0] = 1.0;
		matrix[1] = -1.0;
		for(i=1; i<nCount+1; i++)
		{
			matrix[3*i-1] = N_u3(i-1, m_arrayKnots[i+2]);
			matrix[3*i] = N_u3(i, m_arrayKnots[i+2]);
			matrix[3*i+1] = N_u3(i+1, m_arrayKnots[i+2]);
		}
		matrix[3*nCount+2] = -1.0;
		matrix[3*nCount+3] = 1.0;
		atrde(matrix, nCount+2, 3*nCount+4, cvpy);

		matrix[0] = 1.0;
		matrix[1] = -1.0;
		for(i=1; i<nCount+1; i++)
		{
			matrix[3*i-1] = N_u3(i-1, m_arrayKnots[i+2]);
			matrix[3*i] = N_u3(i, m_arrayKnots[i+2]);
			matrix[3*i+1] = N_u3(i+1, m_arrayKnots[i+2]);
		}
		matrix[3*nCount+2] = -1.0;
		matrix[3*nCount+3] = 1.0;
		atrde(matrix, nCount+2, 3*nCount+4, cvpz);

		for(i=0; i<nCount+2; i++)
			SetControlPoint(i, cvpx[i], cvpy[i], cvpz[i]);

		delete cvpx;
		delete cvpy;
		delete cvpz;
		delete matrix;
	}
	else if(Mode==4)	//给定始、终点的切矢量Q`1、Q`n（实际计算中取首末型值点连线的方向）
	{
		double *cvpx = new double[nCount];
		double *cvpy = new double[nCount];
		double *cvpz = new double[nCount];
		double *matrix = new double[3*nCount-2];

		double dist0, fx0, fy0, fz0;
		double distn, fxn, fyn, fzn;

		SetControlPointsCount(nCount+2);
		SetCurveKnotsWeights(4);

		dist0 = sqrt((m_arrayInputPoints[1].x()-m_arrayInputPoints[0].x())*(m_arrayInputPoints[1].x()-m_arrayInputPoints[0].x())
			+(m_arrayInputPoints[1].y()-m_arrayInputPoints[0].y())*(m_arrayInputPoints[1].y()-m_arrayInputPoints[0].y())
			+(m_arrayInputPoints[1].z()-m_arrayInputPoints[0].z())*(m_arrayInputPoints[1].z()-m_arrayInputPoints[0].z()));
		fx0 = (m_arrayInputPoints[1].x()-m_arrayInputPoints[0].x())/dist0;
		fy0 = (m_arrayInputPoints[1].y()-m_arrayInputPoints[0].y())/dist0;
		fz0 = (m_arrayInputPoints[1].z()-m_arrayInputPoints[0].z())/dist0;
		distn = sqrt((m_arrayInputPoints[nCount-1].x()-m_arrayInputPoints[nCount-2].x())*(m_arrayInputPoints[nCount-1].x()-m_arrayInputPoints[nCount-2].x())
			+(m_arrayInputPoints[nCount-1].y()-m_arrayInputPoints[nCount-2].y())*(m_arrayInputPoints[nCount-1].y()-m_arrayInputPoints[nCount-2].y())
			+(m_arrayInputPoints[nCount-1].z()-m_arrayInputPoints[nCount-2].z())*(m_arrayInputPoints[nCount-1].z()-m_arrayInputPoints[nCount-2].z()));
		fxn = (m_arrayInputPoints[nCount-1].x()-m_arrayInputPoints[nCount-2].x())/distn;
		fyn = (m_arrayInputPoints[nCount-1].y()-m_arrayInputPoints[nCount-2].y())/distn;
		fzn = (m_arrayInputPoints[nCount-1].z()-m_arrayInputPoints[nCount-2].z())/distn;

		cvpx[0] = m_arrayInputPoints[0].x()+(m_arrayKnots[3]-m_arrayKnots[2])*fx0/3.0;
		cvpy[0] = m_arrayInputPoints[0].y()+(m_arrayKnots[3]-m_arrayKnots[2])*fy0/3.0;
		cvpz[0] = m_arrayInputPoints[0].z()+(m_arrayKnots[3]-m_arrayKnots[2])*fz0/3.0;
		for(i=1; i<nCount-1; i++)
		{
			cvpx[i] = m_arrayInputPoints[i].x();
			cvpy[i] = m_arrayInputPoints[i].y();
			cvpz[i] = m_arrayInputPoints[i].z();
		}
		cvpx[nCount-1] = m_arrayInputPoints[nCount-1].x()-(m_arrayKnots[nCount]-m_arrayKnots[nCount-1])*fxn/3.0;
		cvpy[nCount-1] = m_arrayInputPoints[nCount-1].y()-(m_arrayKnots[nCount]-m_arrayKnots[nCount-1])*fyn/3.0;
		cvpz[nCount-1] = m_arrayInputPoints[nCount-1].z()-(m_arrayKnots[nCount]-m_arrayKnots[nCount-1])*fzn/3.0;

		matrix[0] = 1.0;
		matrix[1] = 0.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = N_u3(i, m_arrayKnots[i+3]);
			matrix[3*i] = N_u3(i+1, m_arrayKnots[i+3]);
			matrix[3*i+1] = N_u3(i+2, m_arrayKnots[i+3]);
		}
		matrix[3*nCount-4] = 0.0;
		matrix[3*nCount-3] = 1.0;
		atrde(matrix, nCount, 3*nCount-2, cvpx);

		matrix[0] = 1.0;
		matrix[1] = 0.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = N_u3(i, m_arrayKnots[i+3]);
			matrix[3*i] = N_u3(i+1, m_arrayKnots[i+3]);
			matrix[3*i+1] = N_u3(i+2, m_arrayKnots[i+3]);
		}
		matrix[3*nCount-4] = 0.0;
		matrix[3*nCount-3] = 1.0;
		atrde(matrix, nCount, 3*nCount-2, cvpy);

		matrix[0] = 1.0;
		matrix[1] = 0.0;
		for(i=1; i<nCount-1; i++)
		{
			matrix[3*i-1] = N_u3(i, m_arrayKnots[i+3]);
			matrix[3*i] = N_u3(i+1, m_arrayKnots[i+3]);
			matrix[3*i+1] = N_u3(i+2, m_arrayKnots[i+3]);
		}
		matrix[3*nCount-4] = 0.0;
		matrix[3*nCount-3] = 1.0;
		atrde(matrix, nCount, 3*nCount-2, cvpz);

		m_arrayControlPoints[0].Set_xyz(m_arrayInputPoints[0].x(), m_arrayInputPoints[0].y(), m_arrayInputPoints[0].z());
		for(i=0; i<nCount; i++)
			SetControlPoint(i+1, cvpx[i], cvpy[i], cvpz[i]);
		m_arrayControlPoints[nCount+1].Set_xyz(m_arrayInputPoints[nCount-1].x(), m_arrayInputPoints[nCount-1].y(), m_arrayInputPoints[nCount-1].z());

		delete cvpx;
		delete cvpy;
		delete cvpz;
		delete matrix;
	}
	CalculateDisplayPoints();
}

// Calculate control points by input value points. Here the knots sequence is set by cumulating bowstring length
// P: Control Points; Q: Value Points; Q`: Unit Vector of Value Points
// Here we know Q`1(start_dir) and Q`n(end_dir) as input
//			Q`1=(P2-P0)/2, Q1=(P0+4*P1+P2)/6  ==>>  P0=P2-2*Q`1, 2*P1/3+P2/3=Q1+Q`1/3
//			Q`n=(Pn+1-Pn)/2, Qn=(Pn-1+4*Pn+Pn+1)/6  ==>>  Pn-1=Pn+1-2*Q`n, Pn-1/3+2*Pn/3=Qn-Q`n/3	
void C3BSpline::GetCPByVPMode4(CUnitVector const& start_dir, CUnitVector const& end_dir)
{
	ASSERT(m_bInputByValuePoints);
	if(!m_bInputByValuePoints)
		return;
	
	InitValuePoints(); // TP position test

	int nCount = m_arrayInputPoints.GetSize();

	m_nDegree = 3;
	
	ASSERT(nCount>=(int)m_nDegree-1);
	if(nCount<(int)m_nDegree-1)
		return;

	int i;//, j;

	double *cvpx = new double[nCount];
	double *cvpy = new double[nCount];
	double *cvpz = new double[nCount];
	double *matrix = new double[3*nCount-2];

	SetControlPointsCount(nCount+2);
	SetCurveKnotsWeights(4);

	cvpx[0] = m_arrayInputPoints[0].x()+(m_arrayKnots[3]-m_arrayKnots[2])*start_dir.x()/3.0;
	cvpy[0] = m_arrayInputPoints[0].y()+(m_arrayKnots[3]-m_arrayKnots[2])*start_dir.y()/3.0;
	cvpz[0] = m_arrayInputPoints[0].z()+(m_arrayKnots[3]-m_arrayKnots[2])*start_dir.z()/3.0;
	for(i=1; i<nCount-1; i++)
	{
		cvpx[i] = m_arrayInputPoints[i].x();
		cvpy[i] = m_arrayInputPoints[i].y();
		cvpz[i] = m_arrayInputPoints[i].z();
	}
	cvpx[nCount-1] = m_arrayInputPoints[nCount-1].x()-(m_arrayKnots[nCount]-m_arrayKnots[nCount-1])*end_dir.x()/3.0;
	cvpy[nCount-1] = m_arrayInputPoints[nCount-1].y()-(m_arrayKnots[nCount]-m_arrayKnots[nCount-1])*end_dir.y()/3.0;
	cvpz[nCount-1] = m_arrayInputPoints[nCount-1].z()-(m_arrayKnots[nCount]-m_arrayKnots[nCount-1])*end_dir.z()/3.0;

	matrix[0] = 1.0;
	matrix[1] = 0.0;
	for(i=1; i<nCount-1; i++)
	{
		matrix[3*i-1] = N_u(i, 3, m_arrayKnots[i+3]);
		matrix[3*i] = N_u(i+1, 3, m_arrayKnots[i+3]);
		matrix[3*i+1] = N_u(i+2, 3, m_arrayKnots[i+3]);
	}
	matrix[3*nCount-4] = 0.0;
	matrix[3*nCount-3] = 1.0;
	atrde(matrix, nCount, 3*nCount-2, cvpx);

	matrix[0] = 1.0;
	matrix[1] = 0.0;
	for(i=1; i<nCount-1; i++)
	{
		matrix[3*i-1] = N_u(i, 3, m_arrayKnots[i+3]);
		matrix[3*i] = N_u(i+1, 3, m_arrayKnots[i+3]);
		matrix[3*i+1] = N_u(i+2, 3, m_arrayKnots[i+3]);
	}
	matrix[3*nCount-4] = 0.0;
	matrix[3*nCount-3] = 1.0;
	atrde(matrix, nCount, 3*nCount-2, cvpy);

	matrix[0] = 1.0;
	matrix[1] = 0.0;
	for(i=1; i<nCount-1; i++)
	{
		matrix[3*i-1] = N_u(i, 3, m_arrayKnots[i+3]);
		matrix[3*i] = N_u(i+1, 3, m_arrayKnots[i+3]);
		matrix[3*i+1] = N_u(i+2, 3, m_arrayKnots[i+3]);
	}
	matrix[3*nCount-4] = 0.0;
	matrix[3*nCount-3] = 1.0;
	atrde(matrix, nCount, 3*nCount-2, cvpz);

	m_arrayControlPoints[0].Set_xyz(m_arrayInputPoints[0].x(), m_arrayInputPoints[0].y(), m_arrayInputPoints[0].z());
	for(i=0; i<nCount; i++)
		SetControlPoint(i+1, cvpx[i], cvpy[i], cvpz[i]);
	m_arrayControlPoints[nCount+1].Set_xyz(m_arrayInputPoints[nCount-1].x(), m_arrayInputPoints[nCount-1].y(), m_arrayInputPoints[nCount-1].z());

	delete cvpx;
	delete cvpy;
	delete cvpz;
	delete matrix;


	CalculateDisplayPoints();
}

// Insert a knot value in a known curve, given the insert domain position and the repeat times of insert value
void C3BSpline::InsertKnots(UINT i, UINT r, double u)
{
	ASSERT(u>=m_arrayKnots[i] && u<m_arrayKnots[i+1]);
	if(u<m_arrayKnots[i] || u>=m_arrayKnots[i+1])
		return;
	
	UINT j;
	double denom, alpha;
	double x, y, z;

	CArray <CPoint_3D, CPoint_3D> NewCP;
	NewCP.SetSize(m_nControlPointsCount+1);
	for(j=0; j<=i-m_nDegree; j++)
		NewCP[j] = m_arrayControlPoints[j];
	for(j=i-m_nDegree+1; j<=i-r; j++)
	{
		denom = m_arrayKnots[j+m_nDegree]-m_arrayKnots[j];
		if(denom<MIN_DOUBLE)
			alpha = 0.0;
		else
			alpha = (u-m_arrayKnots[j])/denom;
		x = (1.0-alpha)*m_arrayControlPoints[j-1].x()+alpha*m_arrayControlPoints[j].x();
		y = (1.0-alpha)*m_arrayControlPoints[j-1].y()+alpha*m_arrayControlPoints[j].y();
		z = (1.0-alpha)*m_arrayControlPoints[j-1].z()+alpha*m_arrayControlPoints[j].z();
		NewCP[j].Set_xyz(x, y, z);
	}
	for(j=i-r+1; j<=m_nControlPointsCount; j++)
		NewCP[j] = m_arrayControlPoints[j-1];
	SetControlPointsCount(m_nControlPointsCount+1);
	for(j=0; j<m_nControlPointsCount; j++)
		m_arrayControlPoints[j] = NewCP[j];

	m_arrayKnots.Add(0.0);
	m_nKnotsCount = m_arrayKnots.GetSize();
	for(j=m_nKnotsCount-1; j>(i+1); j--)
		m_arrayKnots[j] = m_arrayKnots[j-1];
	m_arrayKnots[i+1] = u;
	m_cCurveType = 'N';
	m_bModified = TRUE;
	NewCP.RemoveAll();
}

// Insert a knot value in a known curve
void C3BSpline::InsertKnots(double u)
{
	ASSERT(u>=m_arrayKnots[m_nDegree] && u<=m_arrayKnots[m_nControlPointsCount]);
	if(u<m_arrayKnots[m_nDegree] || u>m_arrayKnots[m_nControlPointsCount])
		return;
	
	UINT i, End, r;

	SetInputByControlPoints();
	//1.找到区间
	for(i=m_nDegree; i<=m_nControlPointsCount; i++)
	{
		if(u<m_arrayKnots[i])
			break;
	}
	End = i;

	//2.节点重复度
	for(i=0; i<=m_nDegree; i++)
	{
		if((u-m_arrayKnots[End-i-1])>MIN_DOUBLE)
			break;
	}
	r = i;

	//3.插入节点
	InsertKnots(End-1, r, u);
}

// Get the curve's total length
// Return the value in 'double' data type
double C3BSpline::GetCurveLength()
{
	if(m_bModified)
		CalculateDisplayPoints();
	else if(!m_bGetDisplayPoints)
		CalculateDisplayPoints();

	return m_arrayCurrentDistance[m_arrayCurrentDistance.GetSize()-1];
}

// Get part length of the curve from begining, giving parameter value
// Return the part length in 'double' data type
double C3BSpline::GetLengthByParameter(double para)
{
	if(para<m_arrayKnots[m_nDegree] || para>m_arrayKnots[m_nControlPointsCount])
		return MAX_DOUBLE;

	if(m_bModified)
		CalculateDisplayPoints();
	else if(!m_bGetDisplayPoints)
		CalculateDisplayPoints();

	int c, c1, c2;
	double p, p1, p2;
	UINT nCount = m_arrayCurrentParameter.GetSize();

	c1 = 0;
	c2 = nCount-1;
	p1 = m_arrayCurrentParameter[c1];
	p2 = m_arrayCurrentParameter[c2];
	while((c2-c1)!=1)
	{
		c = (c1+c2)/2;
		p = m_arrayCurrentParameter[c];
		if(para>p)
		{
			p1 = p;
			c1 = c;
		}
		else
		{
			p2 = p;
			c2 = c;
		}
	}

	double dist = m_arrayCurrentDistance[c1]+(m_arrayCurrentDistance[c2]-m_arrayCurrentDistance[c1])*(para-m_arrayCurrentParameter[c1])/(m_arrayCurrentParameter[c2]-m_arrayCurrentParameter[c1]);

	return dist;
}

// Get curve parameter while giving the curve length from begining
// Return parameter in 'double' data type
double C3BSpline::GetParameterByLength(double leng)
{
	if(m_bModified)
		CalculateDisplayPoints();
	else if(!m_bGetDisplayPoints)
		CalculateDisplayPoints();

	UINT nCount = m_arrayCurrentParameter.GetSize();
	if(nCount<2)
		return MAX_DOUBLE;
	if(leng<0.0 || leng>m_arrayCurrentDistance[nCount-1])
		return MAX_DOUBLE;

	int c, c1, c2;
	double dist, dist1, dist2, p;

	c1 = 0;
	c2 = nCount-1;
	dist1 = m_arrayCurrentDistance[c1];
	dist2 = m_arrayCurrentDistance[c2];
	while((c2-c1)!=1)
	{
		c = (c1+c2)/2;
		dist = m_arrayCurrentDistance[c];
		if(leng>dist)
		{
			dist1 = dist;
			c1 = c;
		}
		else
		{
			dist2 = dist;
			c2 = c;
		}
	}

	/*double test = m_arrayCurrentDistance[c2]-m_arrayCurrentDistance[c1];
	double test1 = leng-m_arrayCurrentDistance[c1];
	double test2 = m_arrayCurrentParameter[c2]-m_arrayCurrentParameter[c1];
	double test3 = m_arrayCurrentParameter[c1];*/
	p = m_arrayCurrentParameter[c1]+(m_arrayCurrentParameter[c2]-m_arrayCurrentParameter[c1])*(leng-m_arrayCurrentDistance[c1])/(m_arrayCurrentDistance[c2]-m_arrayCurrentDistance[c1]);

	return p;
}

// Get the maximum length to the origin point of the curve
// Return the maximum length in 'float' data type
float C3BSpline::GetRange()
{
	if(m_bModified)
		CalculateDisplayPoints();
	else if(!m_bGetDisplayPoints)
		CalculateDisplayPoints();

	int nCount = m_arrayDisplayPoints.GetSize();
	ASSERT(nCount>0);
	if(nCount<=0)
		return 0.0f;
	
	double dist_max, dist_cmp;

	dist_max = 0.0;
	for(int i=0; i<nCount; i++)
	{
		dist_cmp = m_arrayDisplayPoints[i].x()*m_arrayDisplayPoints[i].x()
			+m_arrayDisplayPoints[i].y()*m_arrayDisplayPoints[i].y()
			+m_arrayDisplayPoints[i].z()*m_arrayDisplayPoints[i].z();
		if(dist_cmp>dist_max)
			dist_max = dist_cmp;
	}

	return (float)sqrt(dist_max);
}

void C3BSpline::Copy(C3BSpline const& bsc)
{
	UINT i;

	m_arrayControlPoints.RemoveAll();
	m_arrayWeight.RemoveAll();
	m_nControlPointsCount = bsc.m_arrayControlPoints.GetSize();
	m_arrayControlPoints.SetSize(m_nControlPointsCount);
	m_arrayWeight.SetSize(m_nControlPointsCount);
	
	for(i=0; i<m_nControlPointsCount; i++)
	{
		m_arrayControlPoints[i] = bsc.m_arrayControlPoints[i];
		m_arrayWeight[i] = bsc.m_arrayWeight[i];
	}

	m_arrayKnots.RemoveAll();
	m_nKnotsCount = bsc.m_nKnotsCount;
	m_arrayKnots.SetSize(m_nKnotsCount);
	for(i=0; i<m_nKnotsCount; i++)
		m_arrayKnots[i] = bsc.m_arrayKnots[i];

	m_nDegree = bsc.m_nDegree;
	m_cCurveType = bsc.m_cCurveType;
	m_bInputByValuePoints = bsc.m_bInputByValuePoints;

	m_nDispCount = bsc.m_nDispCount;

	UINT nCount;
	
	m_arrayDisplayPoints.RemoveAll();
	nCount = bsc.m_arrayDisplayPoints.GetSize();
	m_arrayDisplayPoints.SetSize(nCount);
	m_arrayCurrentParameter.SetSize(nCount);
	m_arrayCurrentDistance.SetSize(nCount);
	for(i=0; i<nCount; i++)
	{
		m_arrayDisplayPoints[i] = bsc.m_arrayDisplayPoints[i];
		m_arrayCurrentParameter[i] = bsc.m_arrayCurrentParameter[i];
		m_arrayCurrentDistance[i] = bsc.m_arrayCurrentDistance[i];
	}
	m_bGetDisplayPoints = bsc.m_bGetDisplayPoints;
	m_bModified = bsc.m_bModified;

	m_arrayInputPoints.RemoveAll();
	nCount = bsc.m_arrayInputPoints.GetSize();
	m_arrayInputPoints.SetSize(nCount);
	for(i=0; i<nCount; i++)
		m_arrayInputPoints[i] = bsc.m_arrayInputPoints[i];
	
	m_arrayInPntPara.RemoveAll();
	nCount = bsc.m_arrayInPntPara.GetSize();
	m_arrayInPntPara.SetSize(nCount);
	for(i=0; i<nCount; i++)
		m_arrayInPntPara[i] = bsc.m_arrayInPntPara[i];

	m_BaseVector.Set_xyz(bsc.m_BaseVector.x(), bsc.m_BaseVector.y(), bsc.m_BaseVector.z());

	m_bKnotsWeightsSeted = bsc.m_bKnotsWeightsSeted;

	m_bHighLightBegin = bsc.m_bHighLightBegin;
	m_bHighLightEnd = bsc.m_bHighLightEnd;
	m_bDispBE = bsc.m_bDispBE;

	m_nFlag = bsc.m_nFlag;
}

void C3BSpline::Copy(LP3BSpline bs)
{
	UINT i;

	m_arrayControlPoints.RemoveAll();
	m_arrayWeight.RemoveAll();
	m_nControlPointsCount = bs->m_arrayControlPoints.GetSize();
	m_arrayControlPoints.SetSize(m_nControlPointsCount);
	m_arrayWeight.SetSize(m_nControlPointsCount);
	
	for(i=0; i<m_nControlPointsCount; i++)
	{
		m_arrayControlPoints[i] = bs->m_arrayControlPoints[i];
		m_arrayWeight[i] = bs->m_arrayWeight[i];
	}

	m_arrayKnots.RemoveAll();
	m_nKnotsCount = bs->m_nKnotsCount;
	m_arrayKnots.SetSize(m_nKnotsCount);
	for(i=0; i<m_nKnotsCount; i++)
		m_arrayKnots[i] = bs->m_arrayKnots[i];

	m_nDegree = bs->m_nDegree;
	m_cCurveType = bs->m_cCurveType;
	m_bInputByValuePoints = bs->m_bInputByValuePoints;

	m_nDispCount = bs->m_nDispCount;

	UINT nCount;
	
	m_arrayDisplayPoints.RemoveAll();
	nCount = bs->m_arrayDisplayPoints.GetSize();
	m_arrayDisplayPoints.SetSize(nCount);
	m_arrayCurrentParameter.SetSize(nCount);
	m_arrayCurrentDistance.SetSize(nCount);
	for(i=0; i<nCount; i++)
	{
		m_arrayDisplayPoints[i] = bs->m_arrayDisplayPoints[i];
		m_arrayCurrentParameter[i] = bs->m_arrayCurrentParameter[i];
		m_arrayCurrentDistance[i] = bs->m_arrayCurrentDistance[i];
	}
	m_bGetDisplayPoints = bs->m_bGetDisplayPoints;
	m_bModified = bs->m_bModified;

	m_arrayInputPoints.RemoveAll();
	nCount = bs->m_arrayInputPoints.GetSize();
	m_arrayInputPoints.SetSize(nCount);
	for(i=0; i<nCount; i++)
		m_arrayInputPoints[i] = bs->m_arrayInputPoints[i];

	m_arrayInPntPara.RemoveAll();
	nCount = bs->m_arrayInPntPara.GetSize();
	m_arrayInPntPara.SetSize(nCount);
	for(i=0; i<nCount; i++)
		m_arrayInPntPara[i] = bs->m_arrayInPntPara[i];

	m_BaseVector.Set_xyz(bs->m_BaseVector.x(), bs->m_BaseVector.y(), bs->m_BaseVector.z());

	m_bKnotsWeightsSeted = bs->m_bKnotsWeightsSeted;

	m_bHighLightBegin = bs->m_bHighLightBegin;
	m_bHighLightEnd = bs->m_bHighLightEnd;
	m_bDispBE = bs->m_bDispBE;

	m_nFlag = bs->m_nFlag;
}


// Found parameter domain between two knots
// Return first knot's index in 'int' data type
int C3BSpline::FoundKnotsDomain(double u)
{
	int Begin;
	if(u<m_arrayKnots[m_nDegree] || u>m_arrayKnots[m_nControlPointsCount])
		return -1;
	if(u==m_arrayKnots[m_nControlPointsCount])
		return m_nControlPointsCount-1;
	UINT i;
	for(i=m_nDegree+1; i<=m_nControlPointsCount; i++)
	{
		if(u<m_arrayKnots[i])
			break;
	}
	Begin = i-1;
	return Begin;
}

// Get derivative of a point in the curve, giving parameter and number of derivatives to be evaluated
// Return derivative in 'vector' data type
CVector C3BSpline::GetDerivative(double u, UINT order)
{
	CVector rtn_vct;

	ASSERT(order>0 && order<=m_nDegree);
	if(order<=0 || order>m_nDegree)
		return rtn_vct;

	int i = FoundKnotsDomain(u);
	if(i==-1)
		return rtn_vct;
	UINT j, k, l;
	double denom, alpha;

	double **d = new double *[order+1];
	double **nd = new double *[m_nDegree-order+1];
	for(k=0; k<order+1; k++)
		d[k] = new double[m_nDegree+1];
	for(k=0; k<m_nDegree-order+1; k++)
		nd[k] = new double[m_nDegree-order+1];

	for(k=0; k<m_nDegree+1; k++)
		d[0][k] = m_arrayControlPoints[i-m_nDegree+k].x();
	for(l=1; l<=order; l++)
	{
		for(j=l; j<=m_nDegree; j++)
		{
			denom = m_arrayKnots[i+j+1-l]-m_arrayKnots[i-m_nDegree+j];
			if(denom<MIN_DOUBLE)
				d[l][j] = 0.0;
			else
				d[l][j] = (m_nDegree+1-l)*(d[l-1][j]-d[l-1][j-1])/denom;
		}
	}
	for(k=0; k<m_nDegree-order+1; k++)
		nd[0][k] = d[order][k+order];
	for(l=1; l<=m_nDegree-order; l++)
	{
		for(j=l; j<=m_nDegree-order; j++)
		{
			denom = m_arrayKnots[i+j+1-l]-m_arrayKnots[i-m_nDegree+order+j];
			if(denom<MIN_DOUBLE)
				alpha = 0.0;
			else
				alpha = (u-m_arrayKnots[i-m_nDegree+order+j])/denom;
			nd[l][j] = (1.0-alpha)*nd[l-1][j-1]+alpha*nd[l-1][j];
		}
	}
	rtn_vct.Set_x(nd[m_nDegree-order][m_nDegree-order]);

	for(k=0; k<m_nDegree+1; k++)
		d[0][k] = m_arrayControlPoints[i-m_nDegree+k].y();
	for(l=1; l<=order; l++)
	{
		for(j=l; j<=m_nDegree; j++)
		{
			denom = m_arrayKnots[i+j+1-l]-m_arrayKnots[i-m_nDegree+j];
			if(denom<MIN_DOUBLE)
				d[l][j] = 0.0;
			else
				d[l][j] = (m_nDegree+1-l)*(d[l-1][j]-d[l-1][j-1])/denom;
		}
	}
	for(k=0; k<m_nDegree-order+1; k++)
		nd[0][k] = d[order][k+order];
	for(l=1; l<=m_nDegree-order; l++)
	{
		for(j=l; j<=m_nDegree-order; j++)
		{
			denom = m_arrayKnots[i+j+1-l]-m_arrayKnots[i-m_nDegree+order+j];
			if(denom<MIN_DOUBLE)
				alpha = 0.0;
			else
				alpha = (u-m_arrayKnots[i-m_nDegree+order+j])/denom;
			nd[l][j] = (1.0-alpha)*nd[l-1][j-1]+alpha*nd[l-1][j];
		}
	}
	rtn_vct.Set_y(nd[m_nDegree-order][m_nDegree-order]);

	for(k=0; k<m_nDegree+1; k++)
		d[0][k] = m_arrayControlPoints[i-m_nDegree+k].z();
	for(l=1; l<=order; l++)
	{
		for(j=l; j<=m_nDegree; j++)
		{
			denom = m_arrayKnots[i+j+1-l]-m_arrayKnots[i-m_nDegree+j];
			if(denom<MIN_DOUBLE)
				d[l][j] = 0.0;
			else
				d[l][j] = (m_nDegree+1-l)*(d[l-1][j]-d[l-1][j-1])/denom;
		}
	}
	for(k=0; k<m_nDegree-order+1; k++)
		nd[0][k] = d[order][k+order];
	for(l=1; l<=m_nDegree-order; l++)
	{
		for(j=l; j<=m_nDegree-order; j++)
		{
			denom = m_arrayKnots[i+j+1-l]-m_arrayKnots[i-m_nDegree+order+j];
			if(denom<MIN_DOUBLE)
				alpha = 0.0;
			else
				alpha = (u-m_arrayKnots[i-m_nDegree+order+j])/denom;
			nd[l][j] = (1.0-alpha)*nd[l-1][j-1]+alpha*nd[l-1][j];
		}
	}
	rtn_vct.Set_z(nd[m_nDegree-order][m_nDegree-order]);

	for(k=0; k<order+1; k++)
		delete d[k];
	delete []d;
	for(k=0; k<m_nDegree-order+1; k++)
		delete nd[k];
	delete []nd;

	return rtn_vct;
}

// Change the curve's total parameter domain, giving the begin and end point
void C3BSpline::ChangeTotalDomain(double Begin, double End)
{
	ASSERT(Begin<End);
	if(Begin>End)
		return;
	ASSERT(m_nKnotsCount==(m_nControlPointsCount+m_nDegree+1));
	if(m_nKnotsCount!=(m_nControlPointsCount+m_nDegree+1))
		return;
	double enlarge = (End-Begin)/(m_arrayKnots[m_nControlPointsCount]-m_arrayKnots[m_nDegree]);
	double oldBegin = m_arrayKnots[m_nDegree];
	for(UINT i=0; i<m_nKnotsCount; i++)
		m_arrayKnots[i] = Begin+(m_arrayKnots[i]-oldBegin)*enlarge;

	m_bModified = TRUE;
	m_cCurveType = 'N';
}

// Get the curve's parameter domain
// Modify given parameter as return
void C3BSpline::GetCurveDomain(double &Begin, double &End)
{
	ASSERT(m_nKnotsCount>0);
	if(m_nKnotsCount<=0)
		return;

	Begin = m_arrayKnots[m_nDegree];
	End = m_arrayKnots[m_nControlPointsCount];
}

// Break the curve from position of given parameter
LP3BSpline C3BSpline::BreakCurveByParameter(double u)
{
	//ASSERT(u>m_arrayKnots[m_nDegree] && u<m_arrayKnots[m_nControlPointsCount]);
	if(u<=m_arrayKnots[m_nDegree] || u>=m_arrayKnots[m_nControlPointsCount])
		return NULL;
	
	// end point check
	double domain[2];
	GetCurveDomain(domain[0], domain[1]);
	if((u-domain[0])<1.0E-2 || (domain[1]-u)<1.0E-2)
		return(NULL);
	
	LP3BSpline rtn_bs3 = new C3BSpline;
	UINT i, End, r;

	rtn_bs3->SetDegree(m_nDegree);
	SetInputByControlPoints();
	//1.找到区间
	for(i=m_nDegree; i<=m_nControlPointsCount; i++)
	{
		if(u<m_arrayKnots[i])
			break;
	}
	End = i;

	//2.节点重复度
	for(i=0; i<=m_nDegree; i++)
	{
		if((u-m_arrayKnots[End-i-1])>MIN_DOUBLE)
			break;
	}
	r = i;

	//3.插入节点
	for(i=0; i<m_nDegree-r+1; i++)
	{
		InsertKnots(End-1, r+i, u);
		End++;
	}

	//4.后半部赋于新曲线(待返回曲线)
	rtn_bs3->m_bInputByValuePoints = FALSE;
	rtn_bs3->m_arrayInputPoints.SetSize(m_nKnotsCount-End);
	for(i=0; i<m_nKnotsCount-End; i++)
		rtn_bs3->SetInputPoint(i, m_arrayControlPoints[End-m_nDegree+i-1]);
	rtn_bs3->GetControlPointsByInputPoints();
	
	rtn_bs3->SetKnotsCount(m_nKnotsCount-End+m_nDegree+1);
	for(i=0; i<m_nKnotsCount-End+m_nDegree+1; i++)
		rtn_bs3->SetKnot(i, m_arrayKnots[End-m_nDegree+i-1]);
	rtn_bs3->ChangeTotalDomain(0.0, 1.0);
	rtn_bs3->CalculateDisplayPoints();

	//5.修改原曲线为前半部
	for(i=m_nKnotsCount-1; i>=End; i--)
		m_arrayKnots.RemoveAt(i);
	m_arrayKnots.SetSize(End);
	m_nKnotsCount = m_arrayKnots.GetSize();
	for(i=m_nControlPointsCount-1; i>=End-m_nDegree-1; i--)
		m_arrayControlPoints.RemoveAt(i);
	m_arrayControlPoints.SetSize(End-m_nDegree-1);
	m_nControlPointsCount = m_arrayControlPoints.GetSize();
	ChangeTotalDomain(0.0, 1.0);
	CalculateDisplayPoints();

	return rtn_bs3;
}

// Break the curve from position of given curve length from begin
LP3BSpline C3BSpline::BreakCurveByLength(double l)
{
	if(l<0.0 || l>GetCurveLength())
		return NULL;

	double u = GetParameterByLength(l);

	LP3BSpline rtn_bs3 = BreakCurveByParameter(u);
	//LP3BSpline rtn_bs3 = BreakCurveByPara2(u);

	return rtn_bs3;
}

void C3BSpline::ReverseDir()
{
	int i, nCount;
	CPoint_3D tempP;

	CalculateDisplayPoints();

	m_arrayControlPoints.RemoveAll();		//Array of Control Points
	m_arrayWeight.RemoveAll();				//Array of Weight
	m_arrayKnots.RemoveAll();				//Sequence of Knots

	nCount = m_arrayInputPoints.GetSize();
	for(i=0; i<nCount/2; i++)
	{
		tempP = m_arrayInputPoints[i];
		m_arrayInputPoints[i] = m_arrayInputPoints[nCount-i-1];
		m_arrayInputPoints[nCount-i-1] = tempP;
	}
	m_bInputByValuePoints = TRUE;
	m_bKnotsWeightsSeted = FALSE;
	GetCPByVP(1);
}

void C3BSpline::RebuildDir(CPoint_3D * ppb, CPoint_3D * ppe, char mode)
// mode = 's' start起点; 'e' end终点; 'b' both两端; 'n' none不重整端点.
// 001214 专为特征线设置的 mode = 'q' 特征线起点；'z'特征线终点。
{
	int dCount, iCount;

	CalculateDisplayPoints();

	m_arrayControlPoints.RemoveAll();		//Array of Control Points
	m_arrayWeight.RemoveAll();				//Array of Weight
	m_arrayKnots.RemoveAll();				//Sequence of Knots
	m_arrayInputPoints.RemoveAll();		//Input Points (may be Control Points or Value Points)
	m_arrayInPntPara.RemoveAll();			//Input Points parameter

	if(mode=='b')
	{
		ResetInputValuePoints(0);
		m_bInputByValuePoints = TRUE; 
	}
	else
	{
		ResetInputValuePoints(0);
		m_bInputByValuePoints = TRUE; 
	}
	iCount = m_arrayInputPoints.GetSize();
	if(mode=='s' || mode=='b' || mode=='q')
	{
		ASSERT(ppb);
		if(ppb==NULL)
			return;
		m_arrayInputPoints[0].Set_xyz(ppb->x(), ppb->y(), ppb->z());
	}
	if(mode=='e' || mode=='b' || mode=='z')
	{
		ASSERT(ppe);
		if(ppe==NULL)
			return;
		m_arrayInputPoints[iCount-1].Set_xyz(ppe->x(), ppe->y(), ppe->z());
	}
	if(mode=='s' || mode=='e' || mode=='b')
	{
		if(Dist_2P(iCount-1)<1.5 && iCount!=2)
			m_arrayInputPoints.RemoveAt(iCount-2);
		m_bInputByValuePoints = TRUE;
		m_bKnotsWeightsSeted = FALSE;
		GetCPByVP(1);
		return;
	}
	else if(mode=='q' || mode=='z')
	{	//如果是特征线的打断，打断处要向回收一点以方便三角化
		if(Dist_2P(iCount-1)<1.5 && iCount!=2)
			m_arrayInputPoints.RemoveAt(iCount-2);
		m_bInputByValuePoints = TRUE;
		m_bKnotsWeightsSeted = FALSE;
		GetCPByVP(1);
		m_arrayControlPoints.RemoveAll();		//Array of Control Points
		m_arrayWeight.RemoveAll();				//Array of Weight
		m_arrayKnots.RemoveAll();				//Sequence of Knots
		//新方法，算打断点和前一显示点的距离，然后线性插值出距离为0.3处的点
		double dist;
		iCount = m_arrayInputPoints.GetSize();
		dCount = m_arrayDisplayPoints.GetSize();
		CPoint_3D tempP;
		if(mode=='q')
		{
			dist = Cal_two_pos_length(*ppb, m_arrayDisplayPoints[1]);
			tempP.Set_x(ppb->x()+0.3*(m_arrayDisplayPoints[1].x()-ppb->x())/dist);
			tempP.Set_y(ppb->y()+0.3*(m_arrayDisplayPoints[1].y()-ppb->y())/dist);
			tempP.Set_z(0.0);
			m_arrayInputPoints.SetAt(0, tempP);
		}
		else if(mode=='z')
		{
			dist = Cal_two_pos_length(*ppe, m_arrayDisplayPoints[dCount-2]);
			tempP.Set_x(ppe->x()+0.3*(m_arrayDisplayPoints[dCount-2].x()-ppe->x())/dist);
			tempP.Set_y(ppe->y()+0.3*(m_arrayDisplayPoints[dCount-2].y()-ppe->y())/dist);
			tempP.Set_z(0.0);
			m_arrayInputPoints.SetAt(iCount-1, tempP);
		}
		m_bInputByValuePoints = TRUE;
		m_bKnotsWeightsSeted = FALSE;
		GetCPByVP(1);
		return;
	}
	else if(mode=='n')
	{
		m_bInputByValuePoints = TRUE;
		m_bKnotsWeightsSeted = FALSE;
		GetCPByVP(1);
		return;
	}
}

BOOL C3BSpline::RefreshInputPoints(CPoint_3D * ppb, CPoint_3D * ppe, char mode)
// Refresh input points by given new start/end point
// mode = 's' start起点; 'e' end终点; 'b' both两端
{
	//ASSERT(m_bInputByValuePoints);
	if(!m_bInputByValuePoints)
		return FALSE;
	
	if(mode!='s' && mode!='e' && mode!='b')
		return FALSE;

	m_arrayControlPoints.RemoveAll();		//Array of Control Points
	m_arrayWeight.RemoveAll();				//Array of Weight
	m_arrayKnots.RemoveAll();				//Sequence of Knots

	int iCount = m_arrayInputPoints.GetSize();
	if(mode=='s' || mode=='b')
	{
		ASSERT(ppb);
		if(ppb==NULL)
			return FALSE;
		m_arrayInputPoints[0].Set_xyz(ppb->x(), ppb->y(), ppb->z());
	}
	if(mode=='e' || mode=='b')
	{
		ASSERT(ppe);
		if(ppe==NULL)
			return FALSE;
		m_arrayInputPoints[iCount-1].Set_xyz(ppe->x(), ppe->y(), ppe->z());
	}

	if(Dist_2P(iCount-1)<1.5 && iCount!=2)
		m_arrayInputPoints.RemoveAt(iCount-2);
	m_bInputByValuePoints = TRUE;
	m_bKnotsWeightsSeted = FALSE;
	GetCPByVP(1);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//		API Functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// Unite two curves. The first curve's end point must be same as the second's begin point
// Return the result of unite curve with its pointer. We don't delete the origin curves
LP3BSpline C3BSpline::UniteTwoCurve(LP3BSpline FirstCurve, LP3BSpline SecondCurve)
{
	// we only unite the two curves that have same degree
	ASSERT(FirstCurve->m_nDegree==SecondCurve->m_nDegree);
	if(FirstCurve->m_nDegree!=SecondCurve->m_nDegree)
		return NULL;
	
	UINT m_nDegree = FirstCurve->m_nDegree;
	CPoint_3D Pb1, Pe1, Pb2, Pe2;

	Pe1 = FirstCurve->GetPointCoordinate(FirstCurve->m_arrayKnots[FirstCurve->m_nControlPointsCount]);
	Pb1 = FirstCurve->GetPointCoordinate(FirstCurve->m_arrayKnots[m_nDegree]);
	Pb2 = SecondCurve->GetPointCoordinate(SecondCurve->m_arrayKnots[m_nDegree]);
	Pe2 = SecondCurve->GetPointCoordinate(SecondCurve->m_arrayKnots[SecondCurve->m_nControlPointsCount]);
	// 调整为曲线1之末与曲线2之首相连
	if((Cal_two_pos_length(Pb1, Pb2)>1.0e-8 && Cal_two_pos_length(Pb1, Pe2)>1.0e-8)
		&& (Cal_two_pos_length(Pe1, Pb2)>1.0e-8 && Cal_two_pos_length(Pe1, Pe2)>1.0e-8))
		return NULL;
	if(Cal_two_pos_length(Pe1, Pb2)>1.0e-8 && Cal_two_pos_length(Pe1, Pe2)>1.0e-8)
		FirstCurve->ReverseDir();
	if(Cal_two_pos_length(Pb2, Pb1)>1.0e-8 && Cal_two_pos_length(Pb2, Pe1)>1.0e-8)
		SecondCurve->ReverseDir();

	LP3BSpline rtn_bs3 = new C3BSpline;
	UINT i, rFe, rSb;

	rFe = rSb = 1;
	//1.在曲线定义域首末节点处插入重节点并使重复度达到m_nDegree+1
	//检验(u=m_arrayKnots[m_nDegree]和u=m_arrayKnots[m_nControlPointsCount])处的重复度,前后双向搜索
	for(i=1; i<=m_nDegree; i++)
	{
		if(FirstCurve->m_arrayKnots[FirstCurve->m_nControlPointsCount]-FirstCurve->m_arrayKnots[FirstCurve->m_nControlPointsCount-i]<MIN_DOUBLE)
			rFe++;
		if(FirstCurve->m_arrayKnots[FirstCurve->m_nControlPointsCount+i]-FirstCurve->m_arrayKnots[FirstCurve->m_nControlPointsCount]<MIN_DOUBLE)
			rFe++;
		if(SecondCurve->m_arrayKnots[m_nDegree]-SecondCurve->m_arrayKnots[m_nDegree-i]<MIN_DOUBLE)
			rSb++;
		if(SecondCurve->m_arrayKnots[m_nDegree+i]-SecondCurve->m_arrayKnots[m_nDegree]<MIN_DOUBLE)
			rSb++;
	}
	//根据两端节点重复度插入节点
	for(i=0; i<m_nDegree-rFe+1; i++)
		FirstCurve->InsertKnots(FirstCurve->m_nControlPointsCount, rFe+i, FirstCurve->m_arrayKnots[FirstCurve->m_nControlPointsCount]);
	for(i=0; i<m_nDegree-rSb+1; i++)
		SecondCurve->InsertKnots(m_nDegree+i, rSb+i, SecondCurve->m_arrayKnots[m_nDegree]);

	//2.舍弃老节点,重新整理曲线
	for(i=0; i<m_nDegree-rFe+1; i++)
	{
		FirstCurve->m_arrayControlPoints.RemoveAt(FirstCurve->m_nControlPointsCount-i-1);
		FirstCurve->m_arrayKnots.RemoveAt(FirstCurve->m_nKnotsCount-i-1);
	}
	FirstCurve->m_nControlPointsCount = FirstCurve->m_arrayControlPoints.GetSize();
	FirstCurve->m_nKnotsCount = FirstCurve->m_arrayKnots.GetSize();

	for(i=0; i<m_nDegree-rSb+1; i++)
	{
		SecondCurve->m_arrayControlPoints.RemoveAt(0);
		SecondCurve->m_arrayKnots.RemoveAt(0);
	}
	SecondCurve->m_nControlPointsCount = SecondCurve->m_arrayControlPoints.GetSize();
	SecondCurve->m_nKnotsCount = SecondCurve->m_arrayKnots.GetSize();

	//3.控制点相连(找到过曲线端点的控制点)
	rtn_bs3->SetControlPointsCount(FirstCurve->m_nControlPointsCount+SecondCurve->m_nControlPointsCount);
	for(i=0; i<FirstCurve->m_nControlPointsCount; i++)
		rtn_bs3->SetControlPoint(i, FirstCurve->m_arrayControlPoints[i]);
	for(i=FirstCurve->m_nControlPointsCount; i<rtn_bs3->m_nControlPointsCount; i++)
		rtn_bs3->SetControlPoint(i, SecondCurve->m_arrayControlPoints[i-FirstCurve->m_nControlPointsCount]);

	//4.分别计算控制多边形累积边长
	double totalF, totalS;

	totalF = totalS = 0.0;
	for(i=0; i<FirstCurve->m_nControlPointsCount-1; i++)
		totalF += Cal_two_pos_length(FirstCurve->m_arrayControlPoints[i], FirstCurve->m_arrayControlPoints[i+1]);
	for(i=0; i<SecondCurve->m_nControlPointsCount-1; i++)
		totalS += Cal_two_pos_length(SecondCurve->m_arrayControlPoints[i], SecondCurve->m_arrayControlPoints[i+1]);

	//5.根据比例重新设定曲线定义域区间
	FirstCurve->ChangeTotalDomain(0.0, totalF/(totalF+totalS));
	SecondCurve->ChangeTotalDomain(totalF/(totalF+totalS), 1.0);

	//6.节点序列相连
	rtn_bs3->SetKnotsCount(rtn_bs3->m_nControlPointsCount+m_nDegree+1);
	for(i=0; i<FirstCurve->m_nKnotsCount; i++)
		rtn_bs3->SetKnot(i, FirstCurve->m_arrayKnots[i]);
	for(i=FirstCurve->m_nKnotsCount; i<rtn_bs3->m_nKnotsCount; i++)
		rtn_bs3->SetKnot(i, SecondCurve->m_arrayKnots[i-FirstCurve->m_nKnotsCount+m_nDegree+1]);

	rtn_bs3->m_cCurveType = 'N';

	rtn_bs3->CalculateDisplayPoints();

	return rtn_bs3;
}

// Get parameter of the curve, giving its start parameter and length from the start point
// Return parameter in 'double' data type
double C3BSpline::BS3_curve_length_param(double start, double length)
{
  double b_length = GetLengthByParameter(start);
  b_length += length;

  return GetParameterByLength(b_length);
}

// Gets an arbitrary number of derivatives of a curve
// Return the actual number of derivatives calculated
// Modify 'pos' to return position in the curve of the parameter 'param'
// Modify 'deriv' to return derivative of the point in the curve
int C3BSpline::BS3_curve_evaluate(double param, CPoint_3D& pos, CVector& deriv, int order)
{
  pos = GetPointCoordinate(param);

  ASSERT(order==1 || order==2);

  if(order!=1 && order!=2)  return 0;

  deriv = GetDerivative(param, order);

  return order;
}

// Evaluates the curve at the given parameter value
// Return the value in 'position' data type
CPoint_3D C3BSpline::BS3_curve_position(double param)
{
	return GetPointCoordinate(param);
}

// Gets the signed arc length of the curve between two parameter values
// Return the length in 'double' data type
double C3BSpline::BS3_curve_param_length(double start, double end)
{
  ASSERT(end>=start);
  if(end<=start)  return 0.0;

  return (GetLengthByParameter(end) - GetLengthByParameter(start));
}

// Creates a curve that interpolates or fits to the given tolerance the given points, with the given tangent directions at the two end points
LP3BSpline C3BSpline::BS3_curve_interp(int npts, CPoint_3D const* pts, CUnitVector const& start_dir, CUnitVector const& end_dir, double fitol, double& actual_tol)
{
	LP3BSpline rtn_bs3 = new C3BSpline;

	rtn_bs3->SetInputByValuePoints();
	for(int i=0; i<npts; i++)
		rtn_bs3->m_arrayInputPoints.Add(pts[i]);
	rtn_bs3->GetCPByVPMode4(start_dir, end_dir);

	actual_tol = 0.0;

	return rtn_bs3;
}

// Creates a curve that interpolates or fits to the given tolerance the given points, with the given tangent directions at the two end points
LP3BSpline C3BSpline::BS3_curve_interp(int npts, CPoint_3D const* pts)
{
	LP3BSpline rtn_bs3 = new C3BSpline;

	rtn_bs3->SetInputByValuePoints();
	for(int i=0; i<npts; i++)
		rtn_bs3->m_arrayInputPoints.Add(pts[i]);
	rtn_bs3->GetCPByVP(1);

	return rtn_bs3;
}

// Get display position while giving the curve length from begining
// Return CPoint_3D point
CPoint_3D C3BSpline::GetPositionByLength(double leng)
{
	if(m_bModified)
		CalculateDisplayPoints();
	else if(!m_bGetDisplayPoints)
		CalculateDisplayPoints();

	UINT nCount = m_arrayCurrentParameter.GetSize();
	if(nCount<2)
		return NULL;
	if(leng<0.0 || leng>m_arrayCurrentDistance[nCount-1])
		return NULL;

	int c, c1, c2;
	double dist, dist1, dist2;

	c1 = 0;
	c2 = nCount-1;
	dist1 = m_arrayCurrentDistance[c1];
	dist2 = m_arrayCurrentDistance[c2];
	while((c2-c1)!=1)
	{
		c = (c1+c2)/2;
		dist = m_arrayCurrentDistance[c];
		if(leng>dist)
		{
			dist1 = dist;
			c1 = c;
		}
		else
		{
			dist2 = dist;
			c2 = c;
		}
	}

	double px = m_arrayDisplayPoints[c1].x()+(m_arrayDisplayPoints[c2].x()-m_arrayDisplayPoints[c1].x())*(leng-m_arrayCurrentDistance[c1])/(m_arrayCurrentDistance[c2]-m_arrayCurrentDistance[c1]);
	double py = m_arrayDisplayPoints[c1].y()+(m_arrayDisplayPoints[c2].y()-m_arrayDisplayPoints[c1].y())*(leng-m_arrayCurrentDistance[c1])/(m_arrayCurrentDistance[c2]-m_arrayCurrentDistance[c1]);
	double pz = m_arrayDisplayPoints[c1].z()+(m_arrayDisplayPoints[c2].z()-m_arrayDisplayPoints[c1].z())*(leng-m_arrayCurrentDistance[c1])/(m_arrayCurrentDistance[c2]-m_arrayCurrentDistance[c1]);
	CPoint_3D rtnP(px, py, pz);

	return rtnP;
}

// check curve close or not
BOOL C3BSpline::IsClose()
{
	double   dis;
	CPoint_3D pos1,pos2;

	pos1 = Start_pos();
	pos2 = End_pos();
	dis = Cal_two_pos_length(pos1, pos2);
	if(dis<1.0E-6)
		return(TRUE);
	else
		return(FALSE);
}

// type: 1: calculate CP, 0: do not calculate CP
void C3BSpline::ResetInputValuePoints(int type)
{
	int          i,node_num,pnum;
	double       t_ang,ang,len,dis;
	CPoint_3D     node_pos[3];
	CVector       vect1,vect2;
	CUnitVector  dir1,dir2;
	CArray <CPoint_3D,CPoint_3D> node_list;

	len = GetCurveLength();

	node_num = GetDisplayPointsCount();
	if(!node_num) return;

	pnum = 0;
	t_ang = 0;
	for(i=0;i<node_num;i++)
	{
		// start point case		
		if(i==0)
		{
			node_pos[0] = GetDisplayPoint(i);
			node_list.Add(node_pos[0]);
			pnum++;
			continue;
		}

		// end point case
		if(i==node_num-2)
		{
			node_pos[0] = GetDisplayPoint(node_num-1);
			node_list.Add(node_pos[0]);
			pnum++;
			break;
		}

		// get the three point
		node_pos[0] = GetDisplayPoint(i-1);
		node_pos[1] = GetDisplayPoint(i);
		node_pos[2] = GetDisplayPoint(i+1);

		// get current angle
		vect1 = CVector(node_pos[1].x()-node_pos[0].x(),node_pos[1].y()-node_pos[0].y(),node_pos[1].z()-node_pos[0].z());
		dir1 = vect1.Normalise();
		vect2 = CVector(node_pos[2].x()-node_pos[1].x(),node_pos[2].y()-node_pos[1].y(),node_pos[2].z()-node_pos[1].z());
		dir2 = vect2.Normalise();
		ang = dir2%dir1;
		ang = acos(ang);
		t_ang += fabs(ang);
		if(t_ang > 3.0*CONST_PI/180.0)
		{
			node_list.Add(node_pos[1]);
			t_ang = 0.0;
			pnum++;
		}
	} // end of: for(i=0;i<node_num;i++)

	// rebuild curve
	if(pnum > 1)
	{
		// reset m_nDispCount
		dis = len/(pnum-1);
		m_nDispCount = (int)(dis*5.0); // 5mm-->display point
		if(m_nDispCount > 16)
			m_nDispCount = 16;
		else if(m_nDispCount < 1)
			m_nDispCount = 1;

		m_bKnotsWeightsSeted = FALSE;
		m_arrayInputPoints.RemoveAll();
		m_arrayInPntPara.RemoveAll();
		for(i=0;i<pnum;i++)
		{
			node_pos[0] = node_list.GetAt(i);
			m_arrayInputPoints.Add(node_pos[0]);
		}
		node_list.RemoveAll();
		m_bInputByValuePoints = TRUE;

		if(type) GetCPByVP(1);
	}
	else
		node_list.RemoveAll();
}

// 判断曲线是否封闭
BOOL C3BSpline::IsClosed()
{
	UINT DCount = GetDisplayPointsCount();
	double dist = Cal_two_pos_length(m_arrayDisplayPoints[0], m_arrayDisplayPoints[DCount-1]);
	if(dist<MIN_DOUBLE)
		return TRUE;
	else
		return FALSE;
}
