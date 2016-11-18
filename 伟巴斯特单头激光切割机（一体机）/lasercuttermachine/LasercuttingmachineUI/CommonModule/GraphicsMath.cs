using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;

namespace CommonModule
{
    public class GraphicsMath
    {
        public static PointF GetBezier2Point(PointF p1, PointF p2, PointF p3, float t)
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
        public static PointF GetBezier3Point(PointF p1, PointF p2, PointF p3, PointF p4, float t)
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

        public static double ComputeDistance(PointF p1, PointF p2)
        {
            return Math.Sqrt((p1.X - p2.X) * (p1.X - p2.X) + (p1.Y - p2.Y) * (p1.Y - p2.Y));
        }
        public static double ComputeDistance(PointF[] pt)
        {
            double d = 0;
            for (int i = 0; i < pt.Length - 1; i++)
            {
                d += ComputeDistance(pt[0], pt[1]);
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
        public static double Angle(PointF o, PointF s, PointF e)
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
        public static void PointRotate(PointF center, ref PointF p1, double angle)
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
        public static double direction(PointF p1, PointF p2, PointF p)
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
        public static int on_segment(PointF p1, PointF p2, PointF p)
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

        /// <summary>
        /// 判断点是否在直线上
        /// </summary>
        /// <param name="p"></param>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <returns></returns>
        public static bool isPointOnSegment(PointF p1, PointF p2, PointF p)
        {
            //差积是否为0，判断是否在同一直线上
            //trace(( p1.x -p.x )*( p2.y-p.y) -( p2.x -p.x )*( p1.y-p.y));
            if ((p1.X - p.X) * (p2.Y - p.Y) - (p2.X - p.X) * (p1.Y - p.Y) > 0.1)
            {
                return false;
            }
            //判断是否在线段上
            if ((p.X > p1.X && p.X > p2.X) || (p.X < p1.X && p.X < p2.X))
            {
                return false;
            }
            if ((p.Y > p1.Y && p.Y > p2.Y) || (p.Y < p1.Y && p.Y < p2.Y))
            {
                return false;
            }
            return true;
        }
        /// <summary>
        /// 按距离打断线段
        /// </summary>
        /// <param name="ptList"></param>
        /// <param name="len"></param>
        /// <returns></returns>
        public static PointFlat[] FlattenLine(PointF[] ptList, double len,int pathI)
        {
            if (ptList != null && ptList.Length > 1 && len > 0)
            {
                List<PointFlat> pointFlatList = new List<PointFlat>();
                AddPoint(ptList[0], pointFlatList, 0, true, 0, pathI);

                double restLen = 0;
                for (int i = 1; i < ptList.Length; i++)
                {
                    double L = ComputeDistance(ptList[i - 1], ptList[i]);
                    if (L + restLen > len)
                    {
                        L = L + restLen - len;
                        int partNum = (int)(L / len);
                        double x = (ptList[i - 1].X - ptList[i].X) * len / L;
                        double y = (ptList[i - 1].Y - ptList[i].Y) * len / L;
                        double p = (len - restLen) / len;
                        for (int j = 0; j < partNum; j++)
                        {
                            PointF pt = new PointF((float)(ptList[i - 1].X - j * x - p * x), (float)(ptList[i - 1].Y - j * y - p * y));
                            AddPoint(pt, pointFlatList, len, false, i, pathI, true);
                        }
                        restLen = L - partNum * len;
                    }
                    else
                    {
                        restLen += L;
                    }
                    
                    if (restLen > 0)
                    {
                        AddPoint(ptList[i], pointFlatList, restLen, true, i, pathI);
                    }
                    else 
                    {
                        if (!pointFlatList[pointFlatList.Count - 1].isKeyPoint)
                            AddPoint(ptList[i], pointFlatList, restLen, true, i, pathI);
                    }
                }
                return pointFlatList.ToArray();
            }
            return null;
        }

        private static void AddPoint(PointF pt, List<PointFlat> pointFlatList,double len,bool isKey,int pointI,int pathI,bool isMove=false)
        {
            PointFlat ptf = new PointFlat();
            ptf.position = pt;
            ptf.len = len;
            ptf.isKeyPoint = isKey;
            ptf.isMovePoint = isMove;
            ptf.pathIndex = pathI;
            ptf.pointIndex = pointI;
            pointFlatList.Add(ptf);
        }
    }

    public class PointFlat
    {
        public PointF position;
        public double len;
        public int pathIndex;
        public int pointIndex;
        public bool isKeyPoint;
        public bool isMovePoint;
    }
}