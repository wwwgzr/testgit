using netDxf.Entities;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace DrawGraphicsDxf.Draw
{
    /// <summary>
    /// 圆弧
    /// </summary>
    public class ArcD:Shape
    {
        public ArcD(EntityObject eo)
            : base(eo)
        {

        }

        public override void InitialShape()
        {
            Arc arc = (Arc)shape;
            GraphicsPath gp = new GraphicsPath();
            float sweepAngle = (float)(arc.EndAngle - arc.StartAngle);
            if (sweepAngle < 0)
                sweepAngle += 360;
            gp.AddArc((float)(arc.Center.X - arc.Radius), (float)(-arc.Center.Y - arc.Radius), (float)(arc.Radius * 2), (float)(arc.Radius * 2), (float)(-arc.StartAngle), -sweepAngle);
            graphicsPath = gp;
            shapeType = "Arc";
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
