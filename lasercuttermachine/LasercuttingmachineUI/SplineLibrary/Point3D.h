#ifndef _POINT_3D_H
#define _POINT_3D_H

#define CONST_PI 3.1415926

class CVector;
class CUnitVector;

class CPoint_3D
{
private:
  double m_dCoord[3];	// x, y and z coordinates of the position

  // Scale the object space. 
  // This requires care, and so the operator is private.
  friend CPoint_3D operator*(CPoint_3D pos, double ss);

public:
  // Force creation of all positions to be by constructor.
  CPoint_3D();

  // Construct a CPoint_3D from three doubles.
  CPoint_3D(double xi, double yi, double zi);

  // Construct a CPoint_3D from an array of three doubles.
  CPoint_3D(double p[3]);

  // Copy a CPoint_3D
  CPoint_3D(CPoint_3D const &p);

  // Extract a coordinate value.
  double x() const;
  double y() const;
  double z() const;
  double Coordinate(int i);

  // Set coordinate values.
 	void Set_x(double new_x);
 	void Set_y(double new_y);
 	void Set_z(double new_z);
 	void Set_coordinate(int i, double new_c);
  void Set_xyz(double new_x, double new_y, double new_z);	

  // Translate a CPoint_3D by a vector.
  friend CPoint_3D operator+(CPoint_3D pos, CVector vect);
  friend CPoint_3D operator+(CVector vect, CPoint_3D pos);
  friend CPoint_3D operator+(CPoint_3D ps1,CPoint_3D ps2);
  friend CPoint_3D operator-(CPoint_3D pos, CVector vect);
  friend CPoint_3D operator-(CVector vect, CPoint_3D pos);
  // Position operators
  // Get displacement, i.e. a vector, as difference of two positions.
  friend CVector operator-(CPoint_3D pos1, CPoint_3D pos2);

  // Interpolate between two positions. The parameter gives the
  // proportion of the segment between p1 and p2. In fact, the
  // parameter can be < 0 or > 1, in which case the function
  // extrapolates.
  friend CPoint_3D Interpolate(double para, CPoint_3D pos1, CPoint_3D pos2);

  // STI let: add some general functions
  // Determine if 2 positions are equal, given some resolution
  friend BOOL Same_point(CPoint_3D pos1, CPoint_3D pos2, double tol);

  // calculate distance between two CPoint_3D
  //3.calculate the distance of two point
  friend double Cal_two_pos_length(CPoint_3D pos1, CPoint_3D pos2);

  friend double Cal_two_pos_length(double x1,double y1,double z1,double x2,double y2,double z2);

  friend CPoint_3D Cal_two_pos_centre(CPoint_3D pos1, CPoint_3D pos2);
};

class CVector
{
protected:
  double m_dComp[3];	// the x, y and z components of the vector

public:
  // default constructor
  CVector();

  // Construct a vector from three doubles.
  CVector(double x, double y, double z);

  // Construct a vector from an array of three doubles.
  CVector(double v[3]);

  // Copy a vector.
  CVector(CVector const &v);

  // Extract the components of a vector.
  double x() const;

  double y() const;

  double z() const;

  double Component(int i) const;

  // Set component values.
  void Set_x(double new_x);

  void Set_y(double new_y);

  void Set_z(double new_z);

  void Set_xyz(double new_x, double new_y, double new_z);

  void Set_component(int i, double new_c);

  // Unary minus.
  friend CVector operator-(CVector vect);

  // Addition of vectors.
  friend CVector operator+(CVector vect1, CVector vect2);

  // Binary minus.
  friend CVector operator-(CVector vect1, CVector vect2);

  // Scalar product of two vectors.
  friend double operator%(CVector vect1, CVector vect2);

  // Scalar product of a CPoint_3D.
  friend double operator%(CPoint_3D pos, CVector vect);
  friend double operator%(CVector vect, CPoint_3D pos);

  // Cross product of general vectors. Also applies to unit vectors.
  friend CVector operator*(CVector vect1, CVector vect2);

  // Multiplication of a vector by a scalar.
  friend CVector operator*(double ss, CVector vect);
  friend CVector operator*(CVector vect, double ss);

  // Division of a vector by a scalar.
  friend CVector operator/(CVector vect, double ss);

  // Length of a vector.
  double Len_sq();     // len^2
  double Len();

  // Form a CUnitVector by normalising a vector.
  CUnitVector Normalise();

  // Determine if a vector is a zero vector,
  // i.e., its length is less than a given tolerance
  BOOL Is_zero(double tol);
};

class CUnitVector: public CVector 
{
public:
  CUnitVector();	// allow unitialised unit_vectors

  // Construct a unit vector from three components.
  // Always normalises result.
  CUnitVector(double x, double y, double z);

  // Construct a unit vector from an array of three doubles.
  // Always normalises result.
  CUnitVector(double u[3]);

  // Unary minus.
  friend CUnitVector operator-(CUnitVector unit_vect);

  // Scalar product of a CPoint_3D. This is just as for a vector,
  // but declared explicitly to avoid an ambiguity.
  friend double operator%(CPoint_3D pos, CUnitVector unit_vect);
  friend double operator%(CUnitVector unit_vect, CPoint_3D pos);

  // Find a CPoint_3D as the cross-product of a CPoint_3D with a
  // unit vector.
  friend CPoint_3D operator*(CPoint_3D pos, CUnitVector unit_vect);
  friend CPoint_3D operator*(CUnitVector unit_vect, CPoint_3D pos);
};

#endif
