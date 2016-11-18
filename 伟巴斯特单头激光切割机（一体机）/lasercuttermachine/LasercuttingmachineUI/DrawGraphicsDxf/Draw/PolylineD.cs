
using netDxf;
using netDxf.Entities;
using netDxf.Tables;
using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace DrawGraphicsDxf.Draw
{
    /// <summary>
    /// 多段线
    /// </summary>
    public class PolylineD : Shape
    {
        public PolylineD(EntityObject eo)
            : base(eo)
        {

        }

        public override void InitialShape()
        {
            Polyline line = (Polyline)shape;
            GraphicsPath gp = new GraphicsPath();

            int ptLength = line.Vertexes.Count;
            if (line.IsClosed && !line.Vertexes[0].Location.Equals(line.Vertexes[ptLength - 1].Location))
            {
                ptLength = line.Vertexes.Count + 1;
            }
            PointF[] ptList = new PointF[ptLength];
            for (int j = 0; j < line.Vertexes.Count; j++)
            {
                //2016/8/12 图形旋转 begin
                ptList[j] = new PointF(-(float)line.Vertexes[j].Location.Y, -(float)line.Vertexes[j].Location.X);
                //ptList[j] = new PointF((float)line.Vertexes[j].Location.X, -(float)line.Vertexes[j].Location.Y);
                //2016/8/12 图形旋转 end
            }
            if (line.IsClosed && ptLength > line.Vertexes.Count)
            {
                ptList[ptList.Length - 1] = ptList[0];
            }

            gp.AddLines(ptList);
            
            isClose = line.IsClosed;

            graphicsPath = gp;
            shapeType = "线";
            InitialPenStyle();
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