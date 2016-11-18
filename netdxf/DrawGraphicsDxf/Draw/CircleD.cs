
using netDxf.Entities;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace DrawGraphicsDxf.Draw
{
    /// <summary>
    /// 圆
    /// </summary>
    public class CircleD : Shape
    {
        public CircleD(EntityObject eo)
            : base(eo)
        {

        }

        public override void InitialShape()
        {
            Circle cir = (Circle)shape;
            GraphicsPath gp = new GraphicsPath();
            gp.AddEllipse((float)(cir.Center.X - cir.Radius), (float)(-cir.Center.Y - cir.Radius), (float)(cir.Radius * 2), (float)(cir.Radius * 2));
            
            graphicsPath = gp;
            shapeType = "Circle";
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