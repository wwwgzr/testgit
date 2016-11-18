using netDxf;
using netDxf.Entities;
using netDxf.Tables;
using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace DrawGraphicsDxf.Draw
{
    /// <summary>
    /// 两点直线
    /// </summary>
    public class LineD : Shape
    {
        public LineD(EntityObject eo):base(eo)
        {

        }

        public override void InitialShape()
        {
            Line line = (Line)shape;
            GraphicsPath gp = new GraphicsPath();
            //2016/8/12 图形旋转 begin
            //gp.AddLine((float)line.StartPoint.X, (float)-line.StartPoint.Y, (float)line.EndPoint.X, (float)-line.EndPoint.Y);
            gp.AddLine(-(float)line.StartPoint.Y, -(float)line.StartPoint.X, -(float)line.EndPoint.Y, -(float)line.EndPoint.X);
            //2016/8/12 图形旋转 end
            handle = line.Handle;
            graphicsPath = gp;
            shape = line;
            shapeType = "线";
            pen = new Pen[1];
            Color color = line.Color.ToColor();
            if (line.Color.IsByLayer)
            {
                color = line.Layer.Color.ToColor();
            }
            float w = ComputePenWidth(line.Lineweight.Value);
            if (line.Lineweight.Value == Lineweight.ByLayer.Value)
            {
                w = ComputePenWidth(line.Layer.Lineweight.Value);
                //w = line.Layer.Lineweight.Value;
                if (line.Layer.Lineweight.Value == Lineweight.Default.Value)
                {
                    w = 1;
                }
            }

            pen[0] = new Pen(color, w);
            pen[0].DashStyle = DashStyle.Solid;
            if (line.LineType.Name.ToUpper() == "BYLAYER")
            {
                if (line.Layer.LineType.Name.ToUpper() == LineType.Dot.Name.ToUpper() ||
                    line.Layer.LineType.Name.ToUpper() == LineType.Dashed.Name.ToUpper() ||
                    line.Layer.LineType.Name.ToUpper() == LineType.DashDot.Name.ToUpper() ||
                    line.Layer.LineType.Name.ToUpper() == LineType.Center.Name.ToUpper())
                {
                    double[] seg = line.LineType.Segments.ToArray();
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
                if (line.LineType.Name.ToUpper() == LineType.Dot.Name.ToUpper() ||
                    line.LineType.Name.ToUpper() == LineType.Dashed.Name.ToUpper() ||
                    line.LineType.Name.ToUpper() == LineType.DashDot.Name.ToUpper() ||
                    line.LineType.Name.ToUpper() == LineType.Center.Name.ToUpper())
                {
                    double[] seg = line.LineType.Segments.ToArray();
                    float[] dashPattern = new float[seg.Length];
                    for (int j = 0; j < seg.Length; j++)
                    {
                        dashPattern[j] = Math.Abs((float)(seg[j] * line.LineTypeScale));
                    }
                    pen[0].DashPattern = dashPattern;
                    pen[0].DashStyle = DashStyle.Custom;
                }
                else
                    pen[0].DashStyle = DashStyle.Solid;
            }
        }
        public override void Draw(Graphics g, Pen p = null)
        {
            if (p == null)
                g.DrawPath(pen[0], this.graphicsPath);
            else
                g.DrawPath(p, this.graphicsPath);
        }

        public override void Transform(Matrix matrix)
        {
            graphicsPath.Transform(matrix);
            if(pen[0].DashStyle == DashStyle.Custom)
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