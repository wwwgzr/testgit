
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
            PointF[] ptList = new PointF[line.Vertexes.Count];
            for (int j = 0; j < line.Vertexes.Count; j++)
            {
                ptList[j] = new PointF((float)line.Vertexes[j].Location.X, -(float)line.Vertexes[j].Location.Y);
            }
            gp.AddLines(ptList);
            if (line.IsClosed)
                gp.CloseFigure();

            graphicsPath = gp;
            shapeType = "Polyline";
            InitialPenStyle();
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
    }
}