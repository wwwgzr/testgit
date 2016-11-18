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
    public class LwPolylineD : Shape
    {
        public LwPolylineD(EntityObject eo)
            : base(eo)
        {

        }

        public override void InitialShape()
        {
            LwPolyline line = (LwPolyline)shape;
            GraphicsPath gp = new GraphicsPath();
            PointF[] ptList = new PointF[line.Vertexes.Count];
            for (int j = 0; j < line.Vertexes.Count; j++)
            {
                ptList[j] = new PointF((float)line.Vertexes[j].Location.X, -(float)line.Vertexes[j].Location.Y);
            }
            gp.AddLines(ptList);
            if (line.IsClosed)
                gp.CloseFigure();

            handle = line.Handle;
            graphicsPath = gp;
            shape = line;
            shapeType = "LwPolyline";
            //样式
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
