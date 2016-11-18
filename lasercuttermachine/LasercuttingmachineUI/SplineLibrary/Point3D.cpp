#include "stdafx.h"
#include "Point3D.h"
#include <math.h>

CPoint_3D::CPoint_3D()
{
  m_dCoord[ 0 ] = 0.0;
  m_dCoord[ 1 ] = 0.0;
  m_dCoord[ 2 ] = 0.0;
}

CPoint_3D::CPoint_3D(double xi, double yi, double zi)
{
  m_dCoord[ 0 ] = xi;
  m_dCoord[ 1 ] = yi;
  m_dCoord[ 2 ] = zi;
}

// Construct a CPoint_3D from an array of three doubles.
CPoint_3D::CPoint_3D(double p[3])
{
  m_dCoord[ 0 ] = p[ 0 ];
  m_dCoord[ 1 ] = p[ 1 ];
  m_dCoord[ 2 ] = p[ 2 ];
}

// Copy a CPoint_3D
CPoint_3D::CPoint_3D(CPoint_3D const &p)
{
  m_dCoord[ 0 ] = p.m_dCoord[ 0 ];
  m_dCoord[ 1 ] = p.m_dCoord[ 1 ];
  m_dCoord[ 2 ] = p.m_dCoord[ 2 ];
}

// Extract a coordinate value.
double CPoint_3D::x() const
{
  return m_dCoord[ 0 ]; 
}

double CPoint_3D::y() const
{
  return m_dCoord[ 1 ]; 
}

double CPoint_3D::z() const
{
  return m_dCoord[ 2 ]; 
}

double CPoint_3D::Coordinate(int i)
{
  return m_dCoord[ i ]; 
}

// Set coordinate values.
void CPoint_3D::Set_x(double new_x)
{
  m_dCoord[ 0 ] = new_x; 
}

void CPoint_3D::Set_y(double new_y)
{
  m_dCoord[ 1 ] = new_y; 
}

void CPoint_3D::Set_z(double new_z)
{
  m_dCoord[ 2 ] = new_z; 
}

void CPoint_3D::Set_coordinate(int i, double new_c)
{
  m_dCoord[ i ] = new_c; 
}

void CPoint_3D::Set_xyz(double new_x, double new_y, double new_z)
{
  m_dCoord[0] = new_x;
  m_dCoord[1] = new_y;
  m_dCoord[2] = new_z;
}

// Translate a CPoint_3D by a vector.
CPoint_3D operator+(CPoint_3D pos, CVector vect)
{
  double  pp[3];

  pp[0] = pos.x()+vect.x();
  pp[1] = pos.y()+vect.y();
  pp[2] = pos.z()+vect.z();

  return(CPoint_3D(pp));
}

CPoint_3D operator+(CVector vect, CPoint_3D pos)
{
  double  pp[3];

  pp[0] = pos.x()+vect.x();
  pp[1] = pos.y()+vect.y();
  pp[2] = pos.z()+vect.z();

  return(CPoint_3D(pp));
}

CPoint_3D operator+(CPoint_3D ps1,CPoint_3D ps2)
{
  double pp[3];

  pp[0] = ps1.x()+ps2.x();
  pp[1] = ps1.y()+ps2.y();
  pp[2] = ps1.z()+ps2.z();

  return(CPoint_3D(pp));
}

CPoint_3D operator-(CPoint_3D pos, CVector vect)
{
  double pp[3];

  pp[0] = pos.x()-vect.x();
  pp[1] = pos.y()-vect.y();
  pp[2] = pos.z()-vect.z();

  return(CPoint_3D(pp));
}

CPoint_3D operator-(CVector vect, CPoint_3D pos)
{
  double pp[3];

  pp[0] = pos.x()-vect.x();
  pp[1] = pos.y()-vect.y();
  pp[2] = pos.z()-vect.z();

  return(CPoint_3D(pp));
}

// Position operators
// Get displacement, i.e. a vector, as difference of two positions.
CVector operator-(CPoint_3D pos1, CPoint_3D pos2)
{
  double pp[3];

  pp[0] = pos1.x() - pos2.x();
  pp[1] = pos1.y() - pos2.y();
  pp[2] = pos1.z() - pos2.z();

  return(CVector(pp));
}

// Scale the object space.
// This requires care, and so the operator is private.
CPoint_3D operator*(CPoint_3D pos, double ss)
{
  double pp[3];

  pp[0] = pos.x()*ss;
  pp[1] = pos.y()*ss;
  pp[2] = pos.z()*ss;

  return(CPoint_3D(pp));
}

// Interpolate between two positions. The parameter gives the
// proportion of the segment between p1 and p2. In fact, the
// parameter can be < 0 or > 1, in which case the function
// extrapolates.
CPoint_3D Interpolate(double para, CPoint_3D pos1, CPoint_3D pos2)
{
  double   pp[3];

  pp[0] = pos1.x() + para*(pos2.x() - pos1.x());
  pp[1] = pos1.y() + para*(pos2.y() - pos1.y());
  pp[2] = pos1.z() + para*(pos2.z() - pos1.z());

  return(CPoint_3D(pp));
}

// STI let: add some general functions
// Determine if 2 positions are equal, given some resolution
BOOL Same_point(CPoint_3D pos1, CPoint_3D pos2, double tol)
{
  double   res;

  res = 0.0;
  res += fabs(pos1.x() - pos2.x());
  res += fabs(pos1.y() - pos2.y());
  res += fabs(pos1.z() - pos2.z());

  if(res <= tol)  return(TRUE);
  else  return(FALSE);
}

// calculate distance between two CPoint_3D
//3.calculate the distance of two point
double Cal_two_pos_length(CPoint_3D pos1, CPoint_3D pos2)
{
  double    len = 0.0;
  len = (pos1.x() - pos2.x()) * (pos1.x() - pos2.x());
  len = len + (pos1.y() - pos2.y()) * (pos1.y() - pos2.y());
  len = len + (pos1.z() - pos2.z()) * (pos1.z() - pos2.z());
  len = sqrt(len);

  return len;
}

double Cal_two_pos_length(double x1,double y1,double z1,double x2,double y2,double z2)
{
  double    len = 0.0;
  len = (x1 - x2) * (x1 - x2);
  len = len + (y1 - y2) * (y1 - y2);
  len = len + (z1 - z2) * (z1 - z2);
  len = sqrt(len);

  return len;
}

CPoint_3D Cal_two_pos_centre(CPoint_3D pos1, CPoint_3D pos2)
{
  double x = (pos1.x()+pos2.x())/2.0;
  double y = (pos1.y()+pos2.y())/2.0;
  double z = (pos1.z()+pos2.z())/2.0;
  CPoint_3D centreP(x,y,z);

  return centreP;
}

CVector::CVector()
{
  m_dComp[0] = 0.0;
  m_dComp[1] = 0.0;
  m_dComp[2] = 0.0;
}

// Construct a vector from three doubles.
CVector::CVector(double x, double y, double z)
{
  m_dComp[0] = x;
  m_dComp[1] = y;
  m_dComp[2] = z;
}

// Construct a vector from an array of three doubles.
CVector::CVector(double v[3])
{
  m_dComp[0] = v[0];
  m_dComp[1] = v[1];
  m_dComp[2] = v[2];
}

// Copy a vector.
CVector::CVector(CVector const &v)
{
  m_dComp[0] = v.m_dComp[0];
  m_dComp[1] = v.m_dComp[1];
  m_dComp[2] = v.m_dComp[2];
}

// Extract the components of a vector.
double CVector::x() const
{
  return m_dComp[0];
}

double CVector::y() const
{
  return m_dComp[1];
}

double CVector::z() const
{
  return m_dComp[2];
}

double CVector::Component(int i) const
{
  return m_dComp[i];
}

// Set component values.
void CVector::Set_x(double new_x)
{
  m_dComp[0] = new_x;
}

void CVector::Set_y(double new_y)
{
  m_dComp[1] = new_y;
}

void CVector::Set_z(double new_z)
{
  m_dComp[2] = new_z;
}

void CVector::Set_xyz(double new_x, double new_y, double new_z)
{
  m_dComp[0] = new_x;
  m_dComp[1] = new_y;
  m_dComp[2] = new_z;
}

void CVector::Set_component(int i, double new_c)
{
  m_dComp[i] = new_c;
}

// Length of a vector.
// len^2
double CVector::Len_sq()
{
  return(m_dComp[0]*m_dComp[0]+m_dComp[1]*m_dComp[1]+m_dComp[2]*m_dComp[2]);
}

double CVector::Len()
{
  return(sqrt(m_dComp[0]*m_dComp[0]+m_dComp[1]*m_dComp[1]+m_dComp[2]*m_dComp[2]));
}

// Determine if a vector is a zero vector,
// i.e., its length is less than a given tolerance
BOOL CVector::Is_zero(double tol)
{
  if(Len()<=tol)  return(TRUE);
  else  return(FALSE);
}

CUnitVector::CUnitVector()	// allow unitialised unit_vectors
{
}

// Construct a unit vector from three components.
// Always normalises result.
CUnitVector::CUnitVector(double x, double y, double z)
{
  double len;

  len = sqrt(x*x+y*y+z*z);
  if(len<1.0E-6)  len = 1.0;

  m_dComp[0] = x/len;
  m_dComp[1] = y/len;
  m_dComp[2] = z/len;
}

// Construct a unit vector from an array of three doubles.
// Always normalises result.
CUnitVector::CUnitVector(double u[3])
{
  double   len;

  len = sqrt(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]);
  if(len<1.0E-6)  len = 1.0;

  m_dComp[0] = u[0]/len;
  m_dComp[1] = u[1]/len;
  m_dComp[2] = u[2]/len;
}

// Unary minus.
CUnitVector operator-( CUnitVector unit_vect )
{
  double  pp[3];

  pp[0] = -unit_vect.x();
  pp[1] = -unit_vect.y();
  pp[2] = -unit_vect.z();

  return(CUnitVector(pp));
}

// Scalar product of a position. This is just as for a vector,
// but declared explicitly to avoid an ambiguity.
double operator%( CPoint_3D pos, CUnitVector unit_vect )
{
  double res;

  res = 0.0;
  res += pos.x()*unit_vect.x();
  res += pos.y()*unit_vect.y();
  res += pos.z()*unit_vect.z();

  return(res);
}

double operator%( CUnitVector unit_vect, CPoint_3D pos )
{
  double res;

  res = 0.0;
  res += pos.x()*unit_vect.x();
  res += pos.y()*unit_vect.y();
  res += pos.z()*unit_vect.z();

  return(res);
}

double operator%( CVector vect1, CVector vect2 )
{
	double   pp;

	pp = 0.0;
	pp += vect1.x() * vect2.x();
	pp += vect1.y() * vect2.y();
	pp += vect1.z() * vect2.z();

	return(pp);
}

double operator%( CPoint_3D pos, CVector vect )
{
	double   pp;

	pp = 0.0;
	pp += pos.x()*vect.x();
	pp += pos.y()*vect.y();
	pp += pos.z()*vect.z();

	return(pp);
}

double operator%( CVector vect, CPoint_3D pos )
{
	double   pp;

	pp = 0.0;
	pp += pos.x()*vect.x();
	pp += pos.y()*vect.y();
	pp += pos.z()*vect.z();

	return(pp);
}

// Find a position as the cross-product of a position with a 
// unit vector.
CPoint_3D operator*( CPoint_3D pos, CUnitVector unit_vect )
{
  double pp[3];

  pp[0] = pos.y()*unit_vect.z() - pos.z()*unit_vect.y();
  pp[1] = pos.z()*unit_vect.x() - pos.x()*unit_vect.z();
  pp[2] = pos.x()*unit_vect.y() - pos.y()*unit_vect.x();

  return(CPoint_3D(pp));
}

CPoint_3D operator*( CUnitVector unit_vect, CPoint_3D pos )
{
  double pp[3];

  pp[0] = unit_vect.y()*pos.z() - unit_vect.z()*pos.y();
  pp[1] = unit_vect.z()*pos.x() - unit_vect.x()*pos.z();
  pp[2] = unit_vect.x()*pos.y() - unit_vect.y()*pos.x();

  return(CPoint_3D(pp));
}

CVector operator*( CVector vect1, CVector vect2 )
{
	double   pp[3];

	pp[0] = vect1.y()*vect2.z() - vect1.z()*vect2.y();
	pp[1] = vect1.z()*vect2.x() - vect1.x()*vect2.z();
	pp[2] = vect1.x()*vect2.y() - vect1.y()*vect2.x();

	return(CVector(pp));
}

// Form a CUnitVector by normalising a vector.
CUnitVector CVector::Normalise()
{
	return(CUnitVector(x(),y(),z()));
}
