using netDxf.Entities;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace DrawGraphicsDxf.Draw
{
    /// <summary>
    /// 椭圆
    /// </summary>
    public class EllipseD : Shape
    {
        public EllipseD(EntityObject eo)
            : base(eo)
        {
            isContinePD = true;
            isClose = true;
        }

        public override void InitialShape()
        {
            Ellipse ellipse = (Ellipse)shape;
            GraphicsPath gp = new GraphicsPath();
            //2016/8/12 图形旋转 begin
            gp.AddEllipse((float)(-ellipse.Center.Y - ellipse.MinorAxis * 0.5), (float)(-ellipse.Center.X - ellipse.MajorAxis * 0.5), (float)(ellipse.MinorAxis), (float)(ellipse.MajorAxis));
            //gp.AddEllipse((float)(ellipse.Center.X - ellipse.MajorAxis * 0.5), (float)(-ellipse.Center.Y - ellipse.MinorAxis * 0.5), (float)(ellipse.MajorAxis), (float)(ellipse.MinorAxis));
            //2016/8/12 图形旋转 end
            
            graphicsPath = gp;
            shapeType = "椭圆";
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