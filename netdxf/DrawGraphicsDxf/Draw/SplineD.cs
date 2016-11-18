

using netDxf;
using netDxf.Entities;
using netDxf.Tables;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;

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

        }

        public override void InitialShape()
        {
            Spline line = (Spline)shape;
            GraphicsPath gp = new GraphicsPath();
            PointF[] ptList = new PointF[line.ControlPoints.Count];

            for (int j = 0; j < line.ControlPoints.Count; j++)
            {
                ptList[j] = new PointF((float)line.ControlPoints[j].Location.X, -(float)line.ControlPoints[j].Location.Y);
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
                ptBlist.Add(ptList[0]);
                ptBlist.Add(ptList[1]);
                for (int i = 3; i < ptList.Length - 1;)
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
                    i+=1;
                }
                ptBlist.Add(ptList[ptList.Length - 2]);
                ptBlist.Add(ptList[ptList.Length - 1]);
                gp.AddBeziers(ptBlist.ToArray());
            }
            
            if (line.IsClosed)
                gp.CloseFigure();

            graphicsPath = gp;
            shapeType = "Spline";
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
        public override void Draw(Graphics g)
        {
            g.DrawPath(pen[0], this.graphicsPath);
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

        public PointF GetBezier2Point(PointF p1,PointF p2,PointF p3, float t)
        {
            if (t < 0)
                return p1;
            if (t > 1)
                return p3;
            PointF pt = new PointF();
            pt.X = (1 - t) * (1 - t) * p1.X + 2 * t * (1 - t) * p2.X + t * t * p3.X;
            pt.Y = (1 - t) * (1 - t) * p1.Y + 2 * t * (1 - t) * p2.Y + t * t * p3.Y;
            return pt;
        }
        public PointF GetBezier3Point(PointF p1, PointF p2, PointF p3, PointF p4, float t)
        {
            if (t < 0)
                return p1;
            if (t > 1)
                return p4;
            PointF pt = new PointF();
            pt.X = (1 - t) * (1 - t) * (1 - t) * p1.X + 3 * t * (1 - t) * (1 - t) * p2.X + 3 * (1 - t) * t * t * p3.X + t * t * t * p4.X;
            pt.Y = (1 - t) * (1 - t) * (1 - t) * p1.Y + 3 * t * (1 - t) * (1 - t) * p2.Y + 3 * (1 - t) * t * t * p3.Y + t * t * t * p4.Y;
            return pt;
        }

        public double ComputeDistance(PointF p1, PointF p2)
        {
            return Math.Sqrt((p1.X - p2.X) * (p1.X - p2.X) + (p1.Y - p2.Y) * (p1.Y - p2.Y));
        }
        public double ComputeDistance(PointF[] pt)
        {
            double d = 0;
            for (int i = 0; i < pt.Length - 1; i++)
            {
                d += ComputeDistance(pt[0],pt[1]);
            }
            return d;
        }

        /// <summary>  
        /// 根据余弦定理求两个线段夹角  
        /// </summary>  
        /// <param name="o">端点</param>  
        /// <param name="s">start点</param>  
        /// <param name="e">end点</param>  
        /// <returns></returns>  
        double Angle(PointF o, PointF s, PointF e)
        {
            double cosfi = 0, fi = 0, norm = 0;
            double dsx = s.X - o.X;
            double dsy = s.Y - o.Y;
            double dex = e.X - o.X;
            double dey = e.Y - o.Y;

            cosfi = dsx * dex + dsy * dey;
            norm = (dsx * dsx + dsy * dsy) * (dex * dex + dey * dey);
            cosfi /= Math.Sqrt(norm);

            if (cosfi >= 1.0) return 0;
            if (cosfi <= -1.0) return Math.PI;
            fi = Math.Acos(cosfi);

            if (180 * fi / Math.PI < 180)
            {
                return 180 * fi / Math.PI;
            }
            else
            {
                return 360 - 180 * fi / Math.PI;
            }
        }

        /// <summary>  
        /// 以中心点旋转Angle角度  
        /// </summary>  
        /// <param name="center">中心点</param>  
        /// <param name="p1">待旋转的点</param>  
        /// <param name="angle">旋转角度（弧度）</param>  
        private void PointRotate(PointF center, ref PointF p1, double angle)
        {
            double x1 = (p1.X - center.X) * Math.Cos(angle) + (p1.Y - center.Y) * Math.Sin(angle) + center.X;
            double y1 = -(p1.X - center.X) * Math.Sin(angle) + (p1.Y - center.Y) * Math.Cos(angle) + center.Y;
            p1.X = (float)x1;
            p1.Y = (float)y1;
        }

        /// <summary>
        /// 判断点是否在直线上 为0则在直线上
        /// </summary>
        /// <param name="p1">线段起点</param>
        /// <param name="p2">线段终点</param>
        /// <param name="p">被判断的点</param>
        /// <returns></returns>
        double direction(PointF p1, PointF p2, PointF p)
        {
            return (p1.X - p.X) * (p2.Y - p.Y) - (p2.X - p.X) * (p1.Y - p.Y);
        }
        /// <summary>
        /// 判断点是否在线段上 为1则在线段范围内
        /// </summary>
        /// <param name="p1">线段起点</param>
        /// <param name="p2">线段终点</param>
        /// <param name="p">被判断的点</param>
        /// <returns></returns>
        int on_segment(PointF p1, PointF p2, PointF p)
        {
            double max = p1.X > p2.X ? p1.X : p2.X;
            double min = p1.X < p2.X ? p1.X : p2.X;
            double max1 = p1.Y > p2.Y ? p1.Y : p2.Y;
            double min1 = p1.Y < p2.Y ? p1.Y : p2.Y;
            if (p.X >= min && p.X <= max &&
          p.Y >= min1 && p.Y <= max1)
                return 1;
            else
                return 0;
        }
 
    }
}
