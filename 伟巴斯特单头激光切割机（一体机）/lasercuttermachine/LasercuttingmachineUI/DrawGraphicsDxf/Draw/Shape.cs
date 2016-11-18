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
        public int id;
        public string shapeType;
        public EntityObject shape;
        public Pen[] pen;
        public Brush[] brush;
        public GraphicsPath graphicsPath;
        public GraphicsPath outCutPath;
        public GraphicsPath outCutPathClone;
        public bool isSelect = false;
        public bool isClose = false;
        public bool isCanReverse = true;
        public float[,] pathMatrix = new float[3,2];
        public bool isText = false;
        public string guid = Guid.NewGuid().ToString();
        private bool isCut = true;
        public int xCrood = -1;
        public bool isContinePD = false;
        public bool IsCut
        {
            get { return isCut; }
            set 
            { 
                //取消sp过滤
                //isCut = value;
                isCut = true;
            }
        }

        public Shape(EntityObject eo)
        {
            shape = eo;
            InitialShape();
            if (graphicsPath != null)
            {
                outCutPath = (GraphicsPath)graphicsPath.Clone();

                using (Matrix m = new Matrix())
                {
                    m.Scale(40, 40);
                    outCutPath.Flatten(m, 0.1f);
                    //m.Reset();
                    //m.Scale(1 / 40.0f, 1 / 40.0f);
                    //outCutPath.Transform(m);
                }
            }
            try
            {
                if(handle != null)
                {
                    id = Convert.ToInt32("0x" + handle, 16);
                }
            }
            catch (Exception ex)
            {
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "\r\n");
                id = 0;
            }
        }

        public abstract void InitialShape();

        public abstract void Draw(Graphics g, Pen p = null);
        public virtual void DrawBounds(Graphics g)
        {
            if(isSelect)
            {
                Pen p = new Pen(Color.Red);
                if (pen != null && pen.Length > 0 && pen[0] != null)
                    p.Width = pen[0].Width + 2;
                g.DrawPath(p, graphicsPath);
                //RectangleF rect = graphicsPath.GetBounds();
                //g.DrawRectangle(Pens.Red, rect.X, rect.Y, rect.Width, rect.Height);
            }
        }
        public virtual void Transform(Matrix matrix)
        {
            if (graphicsPath == null)
                return;
            graphicsPath.Transform(matrix);
        }

        public virtual void OffSet(float x,float y,float x1,float y1)
        {
            Matrix m = new Matrix();
            m.Translate(x, y);
            graphicsPath.Transform(m);
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

        public virtual GraphicsPath GetoutCutPath()
        {
            return null;
        }

        public virtual void InitialPenStyle()
        {
            if (shape == null)
                return;
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

        public virtual bool PickedShape(System.Drawing.Point p)
        {
            if (pen != null && pen.Length > 0)
            {
                Pen pen1 = (Pen)pen[0].Clone();
                pen1.Width += 2;
                if (graphicsPath.IsOutlineVisible(p, pen1))
                    return true;
            }
            if (graphicsPath.IsVisible(p))
                return true;
            else
                return false;
        }
        public virtual Color GetColor()
        {
            if (pen != null && pen.Length > 0)
            {
                return pen[0].Color;
            }
            if (brush != null && brush.Length > 0)
            {
                return ((SolidBrush)brush[0]).Color;
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
