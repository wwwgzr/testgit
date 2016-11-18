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
            isContinePD = true;
        }

        public override void InitialShape()
        {
            Arc arc = (Arc)shape;
            GraphicsPath gp = new GraphicsPath();
            float sweepAngle = (float)(arc.EndAngle - arc.StartAngle);
            if (sweepAngle < 0)
                sweepAngle += 360;
            //2016/8/12 图形旋转 begin
            gp.AddArc((float)(-arc.Center.Y - arc.Radius), (float)(-arc.Center.X - arc.Radius), (float)(arc.Radius * 2), (float)(arc.Radius * 2), (float)(-arc.StartAngle), -sweepAngle);
            Matrix m = new Matrix();
            m.Translate((float)arc.Center.Y, (float)arc.Center.X);
            gp.Transform(m);
            m.Reset();
            m.Rotate(-90);
            gp.Transform(m);
            m.Reset();
            m.Translate(-(float)arc.Center.Y, -(float)arc.Center.X);
            gp.Transform(m);
            //gp.AddArc((float)(arc.Center.X - arc.Radius), (float)(-arc.Center.Y - arc.Radius), (float)(arc.Radius * 2), (float)(arc.Radius * 2), (float)(-arc.StartAngle), -sweepAngle);
            //2016/8/12 图形旋转 end
            graphicsPath = gp;
            shapeType = "圆弧";
            InitialPenStyle();
        }
        public override void Draw(Graphics g,Pen p = null)
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
