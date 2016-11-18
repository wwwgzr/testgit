/////////////////////////////////////////////////////////////////////////////
//
//	3BSpline.h - the class of B Spline
//
//	����B���������� 
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(_BSPLINE_H)
#define _BSPLINE_H

#include <afxtempl.h>
#include "Point3D.h"

class C3BSpline;

typedef class C3BSpline * LP3BSpline;

class C3BSpline : public CObject
{
  // Attribute
private:
  CArray <CPoint_3D,CPoint_3D> m_arrayControlPoints;	  //Array of Control Points�����Ƶ�����
  CArray <double,double> m_arrayWeight;   //Array of Weight Ȩ������
  UINT m_nControlPointsCount;   //Number of Control Points Array (also of Weight Array)���Ƶ����飨Ȩ�����飩��Ŀ

  CArray <double,double> m_arrayKnots;   //Sequence of Knots �ڵ�����
  UINT m_nKnotsCount;   //Number of Knots �ڵ����

  UINT m_nDegree;   //Degree of Nurbs Curve (=k-1) �Ǿ�������B�������ߵĽ�
  char m_cCurveType;    //Type of Nurbs Curve �Ǿ�������B�������ߵ�����
                        //'U'��Uniform B-Spline ��������B��������
                        //'Q'��Quasi Uniform B-Spline  ׼��������B��������
                        //'P'��Piecewise Bezier Spline �ֶ�Bezier����
                        //'N'��Non-Uniform B Spline �Ǿ�������B��������
                        //'C'��Set Knots Custom  ������ƽڵ�

  BOOL m_bInputByValuePoints;  //Sign of Input Points Type ��������ͱ��

  UINT m_nDispCount;   //Display Count between two Knots ���ڵ�֮�����ʾ����

  CArray <CPoint_3D,CPoint_3D> m_arrayDisplayPoints;   //Value Points to Display    ����ʾ��ȡֵ��
  CArray <double,double> m_arrayCurrentParameter;  //Parameter Array of Display Points  ��ʾ���������
  CArray <double,double> m_arrayCurrentDistance;   //Distance Array of Display Points   ��ʾ���������
  BOOL m_bGetDisplayPoints;   //Sign of Calculate of Value Points  ȡֵ�������
  BOOL m_bModified;   //Sigh of Modifying of Curve (Need of reCalculate) ���߸��ı�ǣ������¼��㣩

  CArray <CPoint_3D,CPoint_3D> m_arrayInputPoints;   //Input Points (may be Control Points or Value Points)
  CArray <double,double> m_arrayInPntPara;   //Input Points parameter

  CVector m_BaseVector;   //Base Vector of the Curve

public:
  BOOL m_bKnotsWeightsSeted;   //Sign of Knots and Weights Seted

  BOOL m_bHighLightBegin;   // sign of begin point highlight
  BOOL m_bHighLightEnd;   // sign of end point highlight
  BOOL m_bDispBE;   // sign of start and  end point highlight

  int m_nFlag;   // 0: neither TP nor CP points is diplayed;
                 //1:only TP is displayed;
                 // 2:only CP is displayed; 3:both TP and CP is displayed
                 // 4: TP and curve direction
                 // 5: display length
                 // 6: diaply length ruler

// Operation
public:
  // Is close curve
  BOOL IsClose();

  // C++ constructor, creating a C3BSpline, set some value as default
  C3BSpline();

  // C++ constructor, creating a C3BSpline from a C3BSpline object
  C3BSpline(C3BSpline const& bsc);

  // C++ deconstructor, free unused memory
  virtual ~C3BSpline();

  // Get curve control points count( sequence number From 0 to n, totally: n+1, return: n+1 )
  // Return count in 'UINT' data type
  UINT GetControlPointsCount();

  // Set curve control points count, this function is ready for the function "SetControlPoint"
  void SetControlPointsCount(UINT nCount);

  // Get curve control point value by index( begin from 0 to n )
  // Modify input parameter( 3 of double) as return
  void GetControlPoint(UINT nIndex, double &x, double &y, double &z);

  // Get curve control point value by index( begin from 0 to n )
  // Return the value in 'CPoint_3D' data type
  CPoint_3D GetControlPoint(UINT nIndex);

  // Set curve control point by index( begin from 0 to n ) and its X, Y, Z value( double )
  void SetControlPoint(UINT nIndex, double x, double y, double z);

  // Set curve control point by index( begin from 0 to n ) and the position value
  void SetControlPoint(UINT nIndex, const CPoint_3D CP);

  // Set curve control point value with two dimension double array( double v[count][3], each v[*] contains X, Y, Z value )
  void SetControlPoints(double **nPoint);

  // Remember: we don't get the weights count because it's just as same as the control points count
  // Get curve weight value by index( begin from 0 to n )
  // Return weight in 'double' data type
  double GetWeight(UINT nIndex);

  // Set curve weight by index( begin from 0 to n ) and the double value
  void SetWeight(UINT nIndex, double nWeight);

  // Set curve weights by double array
  void SetWeights(double *nWeight);

  // Get curve knots count( if everything is all right, it must be m_nControlPointsCount+m_nDegree+1 )
  // Return count in 'UINT' data type
  UINT GetKnotsCount();

  // Set curve knots count, this function is ready for the function "SetKnot"
  void SetKnotsCount(UINT nCount);

  // Get curve knot value by index( begin from 0 to n )
  // Return knot in 'double' data type
  double GetKnot(UINT nIndex);

  // Set curve knot by index( begin from 0 to n ) and the double value
  void SetKnot(UINT nIndex, double nKnot);

  // Set curve knots by double array
  void SetKnots(double *nKnot);

  // We presume that you have set the control points, you can set the curve's weights and knots by this function
  // or you can set them yourself. We supply 4 modes list( all modes set weght as 1.0f ), Mode set to be 1 as default( no parameter input ):
  // Mode==1: Uniform B-Spline. The knots space between are as same as 1.0f, knots begin from (-m_nDegree).
  // Mode==2: Quasi Uniform B-Spline. Just as Uniform B-Spline but the knots space between in first (m_nDegree+1) knots and terminal (m_nDegree+1) knots are 0.0f.
  // Mode==3: Piecewise Bezier Spline. First (m_nDegree+1) knots are set to be 0.0f, and terminal (m_nDegree+1) knots are set to be 1.0f. The middle left are set to be 0.5f.
  // Mode==4: Non-Uniform B Spline. First (m_nDegree+1) knots are set to be 0.0f, and terminal (m_nDegree+1) knots are set to be 1.0f. The middle left knots space between are determined by length of two input points one by one.
  void SetCurveKnotsWeights(int Mode=1);

  // Set curve degree( the constructor set degree to 3 as default, we advise you don't use others )
  void SetDegree(UINT nDegree);

  // Get curve degree value
  // return degree value in 'UINT' data type
  UINT GetDegree();

  // Get point coordinate that in the curve by parameter
  // Modify input parameter( 3 of double) as return
  void GetPointCoordinate(double u, double &x, double &y ,double &z);

  // Get point coordinate that in the curve by parameter
  // Return coordinate in 'CPoint_3D' data type
  CPoint_3D GetPointCoordinate(double u);

  // Set curve input type( input by control points as 'C'(default) and input by value points as 'V' )
  void SetInputType(char inType='C');

  // Set curve input by value points
  void SetInputByValuePoints();

  // Set curve input by control points
  void SetInputByControlPoints();

  // Get curve input type( input by control points as 'C'(default) and input by value points as 'V' )
  // Return type in 'char' data type
  char GetInputType();

  // Test if curve is input by value points
  // Return logical result in 'BOOL' data type
  BOOL IsInputByValuePoints();

  // Test if curve is input by control points
  // Return logical result in 'BOOL' data type
  BOOL IsInputByControlPoints();

  // Set curve Display Count����������ʾ���ȣ�
  void SetDisplayCount(UINT Count);

  // Get curve Display Count���������ʾ���ȣ�
  UINT GetDisplayCount();

  // Get curve display points count( sequence number From 0 to n, totally: n+1, return: n+1 )
  // Return count in 'UINT' data type
  UINT GetDisplayPointsCount();

  // Get curve control point value by index( begin from 0 to n )
  // Modify input parameter( 3 of double) as return
  void GetDisplayPoint(UINT nIndex, double &x, double &y, double &z);

  // Get curve control point value by index( begin from 0 to n )
  // Return the value in 'CPoint_3D' data type
  CPoint_3D GetDisplayPoint(UINT nIndex);

  /*// Set curve display tolerance
  void SetDisplayTolerance(double tlrnc);

  // Get curve display tolerance
  // Return the value in 'double' type
  double GetDisplayTolerance();*/

  // Calculate curve display points also calculate every point's parameter and cumulating chordal length
  // The value store in class member data
  void CalculateDisplayPoints();

  // get start point of the curve
  CPoint_3D Start_pos();

  // get end point of the curve
  CPoint_3D End_pos();

  // Add curve input point
  int AddInputPoints(CPoint_3D new_InP);

  // Get input value points count
  int GetValuePointsCount();

  // Set curve input points count, this function is ready for the function "SetInputPoint"
  void SetInputPointsCount(UINT nCount);

  // Set curve input point by index and the CPoint_3D of point
  void SetInputPoint(UINT nIndex, const CPoint_3D mod_InP);

  // Set curve input point by index and the X, Y, Z value of point
  void SetInputPoint(UINT nIndex, double x, double y, double z);

  // Get curve input points count
  int GetInputPointsCount();

  // Get curve input point value by index( begin from 0 to n )
  // Modify input parameter( 3 of double) as return
  void GetInputPoint(UINT nIndex, double &x, double &y, double &z);

  // Get curve control point value by index( begin from 0 to n )
  // Return the value in 'CPoint_3D' data type
  CPoint_3D GetInputPoint(UINT nIndex);

  // Get curve input point parameter by index( begin from 0 to n )
  double GetInputPointPara(UINT nIndex);

  // Clear all input points
  void ClearInputPoints();

  // Init value points
  void InitValuePoints();

  // Set curve base vector to move the curve in vector value
  void SetBaseVector(double x, double y, double z);

  // Get curve last movement value
  // Modify input parameter as return
  void GetBaseVector(double &x, double &y, double &z);

  // Get curve control points by input points. If the input points are control points, we set the knots and weights by 'Mode' input.
  // If the input points are value points, we calculate the control points by 'Mode' and set knots and weights in Mode=4.
  // That is to say: 'Mode' has different meanings in different input type.
  void GetControlPointsByInputPoints(int Mode=3);

  // Calculate control points by input value points. Here the knots sequence is uniform: from (-m_nDegree) add 1 to (m_nControlPointsCount+m_nDegree)
  // P: Control Points; Q: Value Points; Q`: Unit Vector of Value Points
  // Mode==1	Condition: P1=Q1, Pn=Qn  ==>>  P0=2*P1-P2, Pn+1=2Pn-Pn-1
  // Mode==2	Closed period B-Spline (m_nDegree==3)  ==>>  P0=Pn, Pn+1=P1
  // Mode==3	P0=Pn, Pn=1=Pn
  // Mode==4	Know Q`1 and Q`n (we calculate it with input points)
  //			Q`1=(P2-P0)/2, Q1=(P0+4*P1+P2)/6  ==>>  P0=P2-2*Q`1, 2*P1/3+P2/3=Q1+Q`1/3
  //			Q`n=(Pn+1-Pn)/2, Qn=(Pn-1+4*Pn+Pn+1)/6  ==>>  Pn-1=Pn+1-2*Q`n, Pn-1/3+2*Pn/3=Qn-Q`n/3
  void GetControlPointsByValuePoints(int Mode=3);

  // Calculate control points by input value points. Here the knots sequence is set by cumulating bowstring length
  // P: Control Points; Q: Value Points; Q`: Unit Vector of Value Points
  // Mode==1	Condition: P1=Q1, Pn=Qn  ==>>  P0=2*P1-P2, Pn+1=2Pn-Pn-1
  // Mode==2	Closed period B-Spline (m_nDegree==3)  ==>>  P0=Pn, Pn+1=P1
  // Mode==3	P0=Pn, Pn=1=Pn
  // Mode==4	Know Q`1 and Q`n (we calculate it with input points)
  //			Q`1=(P2-P0)/2, Q1=(P0+4*P1+P2)/6  ==>>  P0=P2-2*Q`1, 2*P1/3+P2/3=Q1+Q`1/3
  //			Q`n=(Pn+1-Pn)/2, Qn=(Pn-1+4*Pn+Pn+1)/6  ==>>  Pn-1=Pn+1-2*Q`n, Pn-1/3+2*Pn/3=Qn-Q`n/3
  void GetCPByVP(int Mode=3);

  // Calculate control points by input value points. Here the knots sequence is set by cumulating bowstring length
  // P: Control Points; Q: Value Points; Q`: Unit Vector of Value Points
  // Here we know Q`1(start_dir) and Q`n(end_dir) as input
  //			Q`1=(P2-P0)/2, Q1=(P0+4*P1+P2)/6  ==>>  P0=P2-2*Q`1, 2*P1/3+P2/3=Q1+Q`1/3
  //			Q`n=(Pn+1-Pn)/2, Qn=(Pn-1+4*Pn+Pn+1)/6  ==>>  Pn-1=Pn+1-2*Q`n, Pn-1/3+2*Pn/3=Qn-Q`n/3
  void GetCPByVPMode4(CUnitVector const& start_dir, CUnitVector const& end_dir);

  // Insert a knot value in a known curve, given the insert domain position and the repeat times of insert value
  void InsertKnots(UINT i, UINT r, double u);

  // Insert a knot value in a known curve
  void InsertKnots(double u);

  // Get the curve's total length
  // Return the value in 'double' data type
  double GetCurveLength();

  // Get part length of the curve from begining, giving parameter value
  // Return the part length in 'double' data type
  double GetLengthByParameter(double para);

  // Get curve parameter while giving the curve length from begining
  // Return parameter in 'double' data type
  double GetParameterByLength(double leng);

  // The following fuctions are from 'CGLEntity' in virtual type

  // Get the maximum length to the origin point of the curve
  // Return the maximum length in 'float' data type
  virtual float GetRange();

  // Copy a C3BSpline
  // ����һ��bs3_curve(����C3BSpline)
  void Copy(C3BSpline const& bsc);
  void Copy(LP3BSpline bs);

  // Get derivative of a point in the curve, giving parameter and number of derivatives to be evaluated
  // Return derivative in 'vector' data type
  // ����ĳ��Ĳ���ֵ��ָ������������õ��n�׵���
  CVector GetDerivative(double u, UINT order=1);

  // Change the curve's total parameter domain, giving the begin and end point
  // �ı��������ֵ��Ĭ�ϸ�Ϊ��0.0��1.0��
  void ChangeTotalDomain(double Begin=0.0, double End=1.0);

  // Get the curve's parameter domain
  // Modify given parameter as return
  // ��ò���ֵ����
  void GetCurveDomain(double &Begin, double &End);

  // Break the curve from position of given parameter
  // ���������ϵ�Ĳ���ֵ���ڸô�������ߣ��޸�ԭ����Ϊǰ�벿��������������(��벿)
  LP3BSpline BreakCurveByParameter(double u);

  // Break the curve from position of given curve length from begin
  // ����ָ�������߳��ȣ��ڸ�λ�ý����ߴ�ϣ��޸�ԭ����Ϊǰ�벿��������������(��벿)
  // �����ڸ��ݳ��������ʱ���������Բ�ֵ���ʽϾ�ȷ
  LP3BSpline BreakCurveByLength(double l);

  // Reverse Curve Direction
  // �����߷���
  void ReverseDir();

  // Rebuild Curve, Add Input Value Points by display points
  // ��ȡ����ʾ�㷽ʽ¼��������ֵ�㣬ͬʱ���ݲ����������߶˵㣬�ؽ�����
  // mode = 's' start���; 'e' end�յ�; 'b' both����; 'n' none�������˵�.
  // 001214 רΪ���������õ� mode = 'q' ��������㣻'z'�������յ㡣
  void RebuildDir(CPoint_3D * ppb, CPoint_3D * ppe, char mode);

  // Refresh input points by given new start/end point
  // ֻ�Ǹ��ݲ�������(����)���ߵĶ˵㣬���ı�������
  BOOL RefreshInputPoints(CPoint_3D * ppb, CPoint_3D * ppe, char mode);

  // ���ݴ������߳��ȣ���������ϵĵ㣬���ڽ������Բ�ֵ���ȽϾ�ȷ
  CPoint_3D GetPositionByLength(double leng);

  // �ж������Ƿ���
  BOOL IsClosed();

  // ������ֵ��
  void ResetInputValuePoints(int type);

  // Unite two curves. The first curve's end point must be same as the second's begin point
  // Return the result of unite curve with its pointer. We don't delete the origin curves
  // �ϲ��������ߣ��ϲ�����������ɺ������أ���ԭ���߲�δ�Ķ�(�������ֹ�ɾ��)
  // �������߱��뱣֤�˵㴦����
  LP3BSpline UniteTwoCurve(LP3BSpline FirstCurve, LP3BSpline SecondCurve);

  // Get parameter of the curve, giving its start parameter and length from the start point
  // Return parameter in 'double' data type
  // ����ָ����㴦����ֵ������Ӹõ㴦��ʼ����ֵΪlength���ĵ�Ĳ���
  double BS3_curve_length_param(
    double start,  // parameter to datum point
    double length);  // length

  // Gets an arbitrary number of derivatives of a curve
  // Return the actual number of derivatives calculated
  // �����߲����㴦��һ��/���� ��/��ʸ
  int BS3_curve_evaluate(
    double param,     // given parameter
    CPoint_3D& pos,    // returned CPoint_3D
    CVector& deriv,    // returned derivatives
    int order);       // number of derivatives to be evaluated, order=1,2

  // Evaluates the curve at the given parameter value
  // Return the value in 'CPoint_3D' data type
  // ������߲���ֵ���ĵ�
  CPoint_3D BS3_curve_position(
    double param);     // given parameter value

  // Gets the signed arc length of the curve between two parameter values.
  // Return the length in 'double' data type
  // ������(��������ֵ)�����ߵĳ���
  double BS3_curve_param_length(
    double start,    // parameter of start point
    double end);     // parameter of end point

  // Creates a curve that interpolates or fits to the given tolerance the given points, with the given tangent directions at the two end points
  // ������ֵ����(CPoint_3D����)������յ���ʸ��������bs3_curve����
  LP3BSpline BS3_curve_interp(
    int npts,                      // number of points to interpolate
    CPoint_3D const* pts,           // points to interpolate or fit
    CUnitVector const& start_dir,  // start direction vector
    CUnitVector const& end_dir,    // end direction vector
    double fitol,                  // fit tolerance
    double& actual_tol);           // returned actual tolerance used

  // Creates a curve given value points
  // ������ֵ����(CPoint_3D����)������bs3_curve����
  LP3BSpline BS3_curve_interp(
    int npts,						// number of points to interpolate
    CPoint_3D const* pts);			// points to interpolate or fit

private:
  // Get B_Spline basic function value
  // Return the value in 'double' data type
  double N_u(UINT i, UINT k, double u);

  // Used for function "N_u3"
  double NA(UINT i, UINT k, double u);
  double NB(UINT i, UINT k, double u);

  // Get B_Spline basic function value( m_nDegree=3 )
  // Return the value in 'double' data type
  double N_u3(UINT i, double u);

  // Get point in the curve by index and parameter value use "de Boor Arithmetic of B_Spline"
  // Return the value in 'CPoint_3D' data type
  CPoint_3D P_u(UINT i, double u);

  // Calculate distance between two input points one by one( this function only used in function "SetCurveKnotsWeights" )
  // ��������������ľ���
  double Dist_2P(UINT nIndex);

  // Found parameter domain between two knots
  // Return first knot's index in 'int' data type
  // �ҵ�����ֵ���ڵĽڵ����䣨����ֵ��ǰһ���ڵ�λ�ã�
  int FoundKnotsDomain(double u);
};

#endif
