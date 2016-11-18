# include "stdafx.h"  
# include "dlldialog.h"  
# include "3BSpline.h"  
# include "Point3D.h"  
//定义输出库函数，以被MFC程序调用  
extern "C" __declspec(dllexport) void ShowDlg(void)
{
	//添加AFX_MANAGE_STATE宏，进行DLL模块状态资源的切换  
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//创建DLL对话框  
	dlldialog dllDialog;
	dllDialog.DoModal();
}

extern "C" __declspec(dllexport) int Add111(int x,int y)
{
	//添加AFX_MANAGE_STATE宏，进行DLL模块状态资源的切换  
	AFX_MANAGE_STATE(AfxGetStaticModuleState());



	return x + y;
}

extern "C" __declspec(dllexport) char Add222(char* str)
{
	//添加AFX_MANAGE_STATE宏，进行DLL模块状态资源的切换  
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//if (pstr != 0) strcpy(pstr, "hello world\n");
	str = "sssss";
	return 's';
}


extern "C" __declspec(dllexport) char Add333(byte* str,int* ss,int* length)
{
	//添加AFX_MANAGE_STATE宏，进行DLL模块状态资源的切换  
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//if (pstr != 0) strcpy(pstr, "hello world\n");
	length[0] = 9;
	return 's';
}

extern "C" __declspec(dllexport) int Add444(double* pointx, double* pointy, int& length)
{
	//添加AFX_MANAGE_STATE宏，进行DLL模块状态资源的切换  
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//CList<CPoint_3D> m_listPoint;
	C3BSpline m_3bspline;
	CList<CPoint_3D, CPoint_3D> m_listInput;
	CList<CPoint_3D, CPoint_3D> m_listPoint;
	CPoint_3D pt1;
	int iIndex1, iCount;
	m_3bspline.SetInputByValuePoints();

	for (int i = 0; i < length; i++)
	{
		pt1.Set_x(*pointx);
		pt1.Set_y(*pointy);
		pt1.Set_z(0);
		m_3bspline.AddInputPoints(pt1);
		pointx++;
		pointy++;
	}

	m_3bspline.GetCPByVP(1);

	m_listPoint.RemoveAll();
	m_3bspline.CalculateDisplayPoints();
	iCount = m_3bspline.GetDisplayPointsCount();
	
	double pointxy[50];
	for (int i = 0; i < 50; i++)
	{
		pt1 = m_3bspline.GetDisplayPoint(i);
		pointxy[i] = pt1.x();
	}

	/*for (iIndex1 = 0; iIndex1 < iCount; iIndex1++)  {
		pt1 = m_3bspline.GetDisplayPoint(iIndex1);
		m_listPoint.AddTail(pt1);
	}*/
	length = iCount;
	return iCount;
}

typedef struct PointXY1
{
	double x[50];
	double y[50];
}PointXY1;


extern "C" __declspec(dllexport) void GetPointXY(PointXY1& data)
{
	//添加AFX_MANAGE_STATE宏，进行DLL模块状态资源的切换  
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//CList<CPoint_3D> m_listPoint;
	for (size_t i = 0; i < 50; i++)
	{
		data.x[i] = i;
		data.y[i] = i;
	}
}




typedef struct PointXY
{
	double x;
	double y;
}PointXY;
extern "C" __declspec(dllexport) bool GetPointXYIntPtr(PointXY* data1, int& length)
{
	//添加AFX_MANAGE_STATE宏，进行DLL模块状态资源的切换  
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//CList<CPoint_3D> m_listPoint;
	for (size_t i = 0; i < length; i++)
	{
		data1->x = i;
		data1->y = i;
		data1++;
	}
	length = 128;
	return true;
}

extern "C" __declspec(dllexport) int GetSplinePointXYLen(double* pointx, double* pointy, int& length, int mode)
{
	//添加AFX_MANAGE_STATE宏，进行DLL模块状态资源的切换  
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//CList<CPoint_3D> m_listPoint;
	C3BSpline m_3bspline;
	CList<CPoint_3D, CPoint_3D> m_listInput;
	CList<CPoint_3D, CPoint_3D> m_listPoint;
	CPoint_3D pt1;
	int iIndex1, iCount;
	m_3bspline.SetInputByValuePoints();

	for (int i = 0; i < length; i++)
	{
		pt1.Set_x(*pointx);
		pt1.Set_y(*pointy);
		pt1.Set_z(0);
		m_3bspline.AddInputPoints(pt1);
		pointx++;
		pointy++;
	}

	//m_3bspline.GetCPByVP(1);
	m_3bspline.GetCPByVP(mode);

	m_listPoint.RemoveAll();
	m_3bspline.CalculateDisplayPoints();
	iCount = m_3bspline.GetDisplayPointsCount();
	length = iCount;
	return iCount;
}
///获得样条曲线的点坐标
extern "C" __declspec(dllexport) bool GetSplinePointXYIntPtr(PointXY* data1, int length, double* pointx, double* pointy, int inputLen, int mode)
{
	//添加AFX_MANAGE_STATE宏，进行DLL模块状态资源的切换  
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//CList<CPoint_3D> m_listPoint;
	C3BSpline m_3bspline;
	CList<CPoint_3D, CPoint_3D> m_listInput;
	CList<CPoint_3D, CPoint_3D> m_listPoint;
	CPoint_3D pt1;
	int iIndex1, iCount;
	m_3bspline.SetInputByValuePoints();

	for (int i = 0; i < inputLen; i++)
	{
		pt1.Set_x(*pointx);
		pt1.Set_y(*pointy);
		pt1.Set_z(0);
		m_3bspline.AddInputPoints(pt1);
		pointx++;
		pointy++;
	}

	//m_3bspline.GetCPByVP(1);
	m_3bspline.GetCPByVP(mode);

	m_listPoint.RemoveAll();
	m_3bspline.CalculateDisplayPoints();
	iCount = m_3bspline.GetDisplayPointsCount();

	for (size_t i = 0; i < length; i++)
	{
		pt1 = m_3bspline.GetDisplayPoint(i);
		data1->x = pt1.x();
		data1->y = pt1.y();
		data1++;
	}
	if (length == iCount)
		return true;

	return false;
}