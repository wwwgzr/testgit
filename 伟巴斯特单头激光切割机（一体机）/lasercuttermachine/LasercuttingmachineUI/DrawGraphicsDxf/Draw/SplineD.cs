

using CommonModule;
using netDxf;
using netDxf.Entities;
using netDxf.Tables;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Runtime.InteropServices;

namespace DrawGraphicsDxf.Draw
{
    /// <summary>
    /// 样条曲线
    /// </summary>
    public class SplineD : Shape
    {
        public SplineD(EntityObject eo)
            : base(eo)
        {
            isContinePD = true;
        }

        public override void InitialShape()
        {
            Spline line = (Spline)shape;
            GraphicsPath gp = new GraphicsPath();
            PointF[] ptList = new PointF[line.ControlPoints.Count];

            for (int j = 0; j < line.ControlPoints.Count; j++)
            {
                //2016/8/12 图形旋转 begin
                ptList[j] = new PointF(-(float)line.ControlPoints[j].Location.Y, -(float)line.ControlPoints[j].Location.X);
                //ptList[j] = new PointF((float)line.ControlPoints[j].Location.X, -(float)line.ControlPoints[j].Location.Y);
                //2016/8/12 图形旋转 end
            }
            //两个点
            if (ptList.Length == 2)
                gp.AddLines(ptList);
            //三个点
            else if (ptList.Length == 3)
            {
                PointF[] temp = GetPoint2(ptList);
                gp.AddBeziers(temp);
            }
            else if (ptList.Length == 4)
            {
                gp.AddBeziers(ptList);
            }
            //大于三个点
            else if (ptList.Length > 4)
            {
                List<PointF> ptBlist = new List<PointF>();
                #region old
                ptBlist.Add(ptList[0]);
                ptBlist.Add(ptList[1]);
                for (int i = 3; i < ptList.Length - 1; )
                {
                    float x = ptList[i].X - ptList[i - 2].X;
                    float y = ptList[i].Y - ptList[i - 2].Y;
                    PointF pt1 = new PointF(ptList[i - 2].X + x * 0.5f, ptList[i - 2].Y + y * 0.5f);
                    x = ptList[i - 1].X - pt1.X;
                    y = ptList[i - 1].Y - pt1.Y;
                    PointF pt2 = new PointF(pt1.X + x * 0.5f, pt1.Y + y * 0.5f);

                    x = ptList[i].X - ptList[i - 2].X;
                    y = ptList[i].Y - ptList[i - 2].Y;
                    PointF pt3 = new PointF(pt2.X - x * 0.25f, pt2.Y - y * 0.25f);
                    PointF pt4 = new PointF(pt2.X + x * 0.25f, pt2.Y + y * 0.25f);
                    ptBlist.Add(pt3);
                    ptBlist.Add(pt2);
                    ptBlist.Add(pt4);
                    i += 1;
                }
                ptBlist.Add(ptList[ptList.Length - 2]);
                ptBlist.Add(ptList[ptList.Length - 1]);
                gp.AddBeziers(ptBlist.ToArray());
                #endregion old

                #region test
                //PointF[] ptTemp = new PointF[ptList.Length];
                //for (int i = 0; i < ptTemp.Length; i++)
                //{
                //    ptTemp[i] = new PointF(ptList[i].X, ptList[i].Y);
                //}
                //List<PointF> ptSpline = new List<PointF>();
                //float t = 0;
                //ptSpline.Add(ptTemp[0]);
                //PointF pt111;
                //for (int i = 3; i < ptTemp.Length - 1; i++)
                //{
                //    double l1 = GraphicsMath.ComputeDistance(ptTemp[i - 2], ptTemp[i - 3]);
                //    double l2 = GraphicsMath.ComputeDistance(ptTemp[i - 1], ptTemp[i - 2]);
                //    double l3 = GraphicsMath.ComputeDistance(ptTemp[i], ptTemp[i - 1]);

                //    t = (float)(1 - l1 / (l1 + l2 + l3));
                //    if (l1 + l2 + l3 == 0)
                //        t = 1;
                //    pt111 = GraphicsMath.GetBezier3Point(ptTemp[i - 3], ptTemp[i - 2], ptTemp[i - 1], ptTemp[i], t);
                //    float x = ptTemp[i - 1].X - pt111.X;
                //    float y = ptTemp[i - 1].Y - pt111.Y;
                //    pt111 = new PointF(pt111.X + x * 0.3f, pt111.Y + y * 0.3f);
                //    ptSpline.Add(pt111);

                //}
                //ptSpline.Add(ptTemp[ptTemp.Length - 1]);

                //ptBlist.Clear();
                //ptBlist.Add(ptTemp[0]);
                //ptBlist.Add(ptTemp[1]);
                //for (int i = 3; i < ptTemp.Length - 1; )
                //{
                //    PointF pt2 = new PointF(ptSpline[i - 2].X, ptSpline[i - 2].Y);

                //    float x = ptTemp[i].X - ptTemp[i - 2].X;
                //    float y = ptTemp[i].Y - ptTemp[i - 2].Y;
                //    PointF pt3 = new PointF(pt2.X - x * 0.15f, pt2.Y - y * 0.15f);
                //    PointF pt4 = new PointF(pt2.X + x * 0.15f, pt2.Y + y * 0.15f);
                //    ptBlist.Add(pt3);
                //    ptBlist.Add(pt2);
                //    ptBlist.Add(pt4);
                //    i += 1;
                //}
                //ptBlist.Add(ptTemp[ptTemp.Length - 2]);
                //ptBlist.Add(ptTemp[ptTemp.Length - 1]);
                //gp.AddBeziers(ptBlist.ToArray());
                #endregion
                #region dll
                //int mode = 1;
                //double[] pointx = new double[ptSpline.Count];
                //double[] pointy = new double[ptSpline.Count];
                //for (int i = 0; i < ptSpline.Count; i++)
                //{
                //    pointx[i] = ptSpline[i].X;
                //    pointy[i] = ptSpline[i].Y;
                //}
                //int workStationCount = pointy.Length;
                //int inputLen = pointy.Length;
                //workStationCount = GetSplinePoint.GetSplinePointXYLen(pointx, pointy, ref inputLen, mode);

                //int size = Marshal.SizeOf(typeof(CommonModule.PointXY));
                //IntPtr infosIntptr = Marshal.AllocHGlobal(size * workStationCount);
                //CommonModule.PointXY[] infos = new CommonModule.PointXY[workStationCount];
                //inputLen = pointy.Length;
                //bool ds = GetSplinePoint.GetSplinePointXYIntPtr(infosIntptr, workStationCount, pointx, pointy, inputLen, mode);

                //for (int inkIndex = 0; inkIndex < workStationCount; inkIndex++)
                //{
                //    IntPtr ptr = (IntPtr)((UInt32)infosIntptr + inkIndex * size);
                //    infos[inkIndex] = (CommonModule.PointXY)Marshal.PtrToStructure(ptr, typeof(CommonModule.PointXY));
                //}
                //Marshal.FreeHGlobal(infosIntptr);

                //gp.StartFigure();
                //PointF[] ptSplines = new PointF[infos.Length];
                //for (int i = 0; i < infos.Length; i++)
                //{
                //    ptSplines[i] = new PointF((float)infos[i].x, (float)infos[i].y);
                //}
                //gp.AddLines(ptSplines);
                #endregion
            }

            


            //if (line.IsClosed)
            //    gp.CloseFigure();
            //isClose = line.IsClosed;

            graphicsPath = gp;
            shapeType = "曲线";
            InitialPenStyle();
        }

        private PointF[] GetPoint2(PointF[] ptList)
        {
            PointF[] temp = new PointF[4];
            float x = ptList[1].X - ptList[0].X;
            float y = ptList[1].Y - ptList[0].Y;
            temp[0] = ptList[0];
            temp[1] = new PointF(ptList[0].X + x * 0.65f, ptList[0].Y + y * 0.65f);
            x = ptList[2].X - ptList[1].X;
            y = ptList[2].Y - ptList[1].Y;
            temp[2] = new PointF(ptList[1].X + x * 0.65f, ptList[1].Y + y * 0.65f);
            temp[3] = ptList[2];
            return temp;
        }
        private PointF[] GetPoint2(PointF pt1,PointF pt2,PointF pt3)
        {
            PointF[] temp = new PointF[4];
            float x = pt2.X - pt1.X;
            float y = pt2.Y - pt1.Y;
            temp[0] = pt1;
            temp[1] = new PointF(pt1.X + x * 0.65f, pt1.Y + y * 0.65f);
            x = pt3.X - pt2.X;
            y = pt3.Y - pt2.Y;
            temp[2] = new PointF(pt2.X + x * 0.65f, pt2.Y + y * 0.65f);
            temp[3] = pt3;
            return temp;
        }
        public override void Draw(Graphics g, Pen p = null)
        {
            if (p == null)
                g.DrawPath(pen[0], this.graphicsPath);
            else
                g.DrawPath(p, this.graphicsPath);
            //if (pen[0].DashStyle == DashStyle.Custom)
            //{
            //    graphicsPath.Flatten();
            //    PointF[] pt = graphicsPath.PathPoints;
            //}
        }

        public override void Transform(Matrix matrix)
        {
            graphicsPath.Transform(matrix);
            if (pen[0].DashStyle == DashStyle.Custom)
            {
                float[] pattern = pen[0].DashPattern;
                for (int i = 0; i < pattern.Length; i++)
                {
                    pattern[i] = pattern[i] * matrix.Elements[0];
                }
                pen[0].DashPattern = pattern;
            }
        }
    }
}