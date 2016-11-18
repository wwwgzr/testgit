using System;
using System.Runtime.InteropServices;

namespace CommonModule
{
    [StructLayout(LayoutKind.Sequential)]
    public struct PointXY
    {
        public double x;
        public double y;
    }

    public class GetSplinePoint
    {
        //int GetSplinePointXYLen(double* pointx, double* pointy, int& length, int mode)
        [DllImport("SplineLibrary.DLL", EntryPoint = "GetSplinePointXYLen", ExactSpelling = false, SetLastError = true)]
        public static extern int GetSplinePointXYLen([MarshalAs(UnmanagedType.LPArray)]  double[] pointx, [MarshalAs(UnmanagedType.LPArray)]  double[] pointy, ref int length, int mode);
        //bool GetSplinePointXYIntPtr(PointXY* data1, int length, double* pointx, double* pointy, int inputLen, int mode)
        [DllImport("SplineLibrary.DLL", EntryPoint = "GetSplinePointXYIntPtr", ExactSpelling = false, SetLastError = true)]
        public static extern bool GetSplinePointXYIntPtr(IntPtr data1, int length, [MarshalAs(UnmanagedType.LPArray)]  double[] pointx, [MarshalAs(UnmanagedType.LPArray)]  double[] pointy, int inputLen, int mode);
    }
}