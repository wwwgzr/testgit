using netDxf;
using netDxf.Entities;
using netDxf.Tables;
using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace DrawGraphicsDxf.Draw
{
    public abstract class Shape
    {
        public string handle;
        public string shapeType;
        public EntityObject shape;
        public Pen[] pen;
        public Brush[] brush;
        public GraphicsPath graphicsPath;

        public Shape(EntityObject eo)
        {
            shape = eo;
            InitialShape();
        }

        public abstract void InitialShape();

        public abstract void Draw(Graphics g);
        public virtual void Transform(Matrix matrix)
        {
            graphicsPath.Transform(matrix);
        }

        public virtual float ComputePenWidth(double d)
        {
            float w = (float)((d * 96) / (2540));
            if (w < 1)
                w = 1;
            return w;
        }

        public virtual PointF[] GetPathPoints()
        {
            return graphicsPath.PathPoints;
        }

        public virtual void InitialPenStyle()
        {
            handle = shape.Handle;
            //样式
            pen = new Pen[1];
            Color color = shape.Color.ToColor();
            if (shape.Color.IsByLayer)
            {
                color = shape.Layer.Color.ToColor();
            }
            float w = ComputePenWidth(shape.Lineweight.Value);
            if (shape.Lineweight.Value == Lineweight.ByLayer.Value)
            {
                w = ComputePenWidth(shape.Layer.Lineweight.Value);
                //w = line.Layer.Lineweight.Value;
                if (shape.Layer.Lineweight.Value == Lineweight.Default.Value)
                {
                    w = 1;
                }
            }

            pen[0] = new Pen(color, w);
            pen[0].DashStyle = DashStyle.Solid;
            if (shape.LineType.Name.ToUpper() == "BYLAYER")
            {
                if (shape.Layer.LineType.Name.ToUpper() == LineType.Dot.Name.ToUpper() ||
                    shape.Layer.LineType.Name.ToUpper() == LineType.Dashed.Name.ToUpper() ||
                    shape.Layer.LineType.Name.ToUpper() == LineType.DashDot.Name.ToUpper() ||
                    shape.Layer.LineType.Name.ToUpper() == LineType.Center.Name.ToUpper())
                {
                    double[] seg = shape.LineType.Segments.ToArray();
                    float[] dashPattern = new float[seg.Length];
                    for (int j = 0; j < seg.Length; j++)
                    {
                        dashPattern[j] = Math.Abs((float)(seg[j]));
                    }
                    pen[0].DashPattern = dashPattern;
                    pen[0].DashStyle = DashStyle.Custom;
                }
                else
                    pen[0].DashStyle = DashStyle.Solid;
            }
            else
            {
                if (shape.LineType.Name.ToUpper() == LineType.Dot.Name.ToUpper() ||
                    shape.LineType.Name.ToUpper() == LineType.Dashed.Name.ToUpper() ||
                    shape.LineType.Name.ToUpper() == LineType.DashDot.Name.ToUpper() ||
                    shape.LineType.Name.ToUpper() == LineType.Center.Name.ToUpper())
                {
                    double[] seg = shape.LineType.Segments.ToArray();
                    float[] dashPattern = new float[seg.Length];
                    for (int j = 0; j < seg.Length; j++)
                    {
                        dashPattern[j] = Math.Abs((float)(seg[j] * shape.LineTypeScale));
                    }
                    pen[0].DashPattern = dashPattern;
                    pen[0].DashStyle = DashStyle.Custom;
                }
                else
                    pen[0].DashStyle = DashStyle.Solid;
            }

        }

        public virtual Color GetColor()
        {
            if (pen != null)
            {
                return pen[0].Color;
            }
            return shape.Color.ToColor();
        }
        //public virtual float ComputePenWidth(double d,Graphics g)
        //{
        //    float w = (float)((d * g.DpiX) / (2540));
        //    if (w < 1)
        //        w = 1;
        //    return w;
        //}

//             using (Graphics graphics = Graphics.FromHwnd(IntPtr.Zero))
//             {
//                 float dpiX = graphics.DpiX;
//                 float dpiY = graphics.DpiY;
//             }
    }
}
