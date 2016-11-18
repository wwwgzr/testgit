using netDxf;
using netDxf.Entities;
using netDxf.Tables;
using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace DrawGraphicsDxf.Draw
{
    /// <summary>
    /// plt线
    /// </summary>
    public class LinesPlt : Shape
    {
        public LinesPlt(EntityObject eo)
            : base(eo)
        {

        }

        public void SetLineOut()
        {
            if (graphicsPath != null)
            {
                outCutPath = (GraphicsPath)graphicsPath.Clone();

                if (graphicsPath.PathPoints[0].X == graphicsPath.PathPoints[graphicsPath.PointCount - 1].X &&
                    graphicsPath.PathPoints[0].Y == graphicsPath.PathPoints[graphicsPath.PointCount - 1].Y)
                {
                    isCanReverse = true;
                    isClose = true;
                }
                //using (Matrix m = new Matrix())
                //{
                //    m.Scale(40, 40);
                //    outCutPath.Flatten(m, 0.1f);
                //    m.Reset();
                //    m.Scale(1 / 40.0f, 1 / 40.0f);
                //    outCutPath.Transform(m);
                //}
            }
        }
        public override void InitialShape()
        {
            //Polyline line = (Polyline)shape;
            //GraphicsPath gp = new GraphicsPath();

            //int ptLength = line.Vertexes.Count;
            //if (line.IsClosed && !line.Vertexes[0].Location.Equals(line.Vertexes[ptLength - 1].Location))
            //{
            //    ptLength = line.Vertexes.Count + 1;
            //}
            //PointF[] ptList = new PointF[ptLength];
            //for (int j = 0; j < line.Vertexes.Count; j++)
            //{
            //    ptList[j] = new PointF((float)line.Vertexes[j].Location.X, -(float)line.Vertexes[j].Location.Y);
            //}
            //if (line.IsClosed && ptLength > line.Vertexes.Count)
            //{
            //    ptList[ptList.Length - 1] = ptList[0];
            //}

            //gp.AddLines(ptList);
            
            //isClose = line.IsClosed;

            //graphicsPath = gp;
            shapeType = "线";
            //InitialPenStyle();
        }
        public override void Draw(Graphics g, Pen p = null)
        {
            if (graphicsPath == null)
                return;
           
            if (pen != null && pen.Length > 0)
            {
                if (p == null)
                    g.DrawPath(pen[0], this.graphicsPath);
                else
                    g.DrawPath(p, this.graphicsPath);   
            }
             
            if (brush != null && brush.Length > 0)
                g.FillPath(brush[0], this.graphicsPath);

            //GraphicsPath gp = (GraphicsPath)graphicsPath.Clone();
            //Matrix m = new Matrix();
            //gp.Flatten(m,100);
            //if (pen[0].DashStyle == DashStyle.Custom)
            //{
            //    graphicsPath.Flatten();
            //    PointF[] pt = graphicsPath.PathPoints;
            //}
        }

        //public override void Transform(Matrix matrix)
        //{
        //    graphicsPath.Transform(matrix);
        //    if (pen[0].DashStyle == DashStyle.Custom)
        //    {
        //        float[] pattern = pen[0].DashPattern;
        //        for (int i = 0; i < pattern.Length; i++)
        //        {
        //            pattern[i] = pattern[i] * matrix.Elements[0];
        //        }
        //        pen[0].DashPattern = pattern;
        //    }
        //}
    }
}