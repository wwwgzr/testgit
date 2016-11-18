using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Form2 : Form
    {
        GraphicsPath gp;
        GraphicsPath gpClone;
        GraphicsPath gp2;
        GraphicsPath gp3;
        GraphicsPath gp4;
        List<PointT> pointList;
        public float maxX = float.MinValue;
        public float maxY = float.MinValue;
        public RectangleF rect;
        public double m_dProportion;
        bool isMouseDown = false;
        PointF mouseDown = PointF.Empty;
        PointF mouseMove = PointF.Empty;
        PointF m_ptStartPos = PointF.Empty;
        public Form2(List<PointT> ptList, float x, float y)
        {
            InitializeComponent();
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint, true);
            this.UpdateStyles();
            maxX = x;
            maxY = y;
            pointList = ptList;
        }
        public Form2(GraphicsPath gp1, float x, float y)
        {
            InitializeComponent();
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint, true);
            this.UpdateStyles();
            maxX = x;
            maxY = y;

            gp = gp1;
            //gp = new GraphicsPath();
            //gp.AddRectangle(new Rectangle(100, 100, 100, 100));
            gpClone = (GraphicsPath)gp.Clone();
            gp2 = new GraphicsPath();
            FontFamily ff = new FontFamily("Arial");
            //FontFamily family = new FontFamily("Arial");
            int fontStyle = (int)FontStyle.Regular;


            StringFormat sf = new StringFormat();
            sf.Alignment = StringAlignment.Center;
            sf.LineAlignment = StringAlignment.Center;
            gp2.AddString("Richpeace", ff, fontStyle, 72, new Point(500, this.Height / 2), sf);
            gp2.Flatten();
            PointF[] p = gp2.PathPoints;
            byte[] t = gp2.PathTypes;
            gp3 = new GraphicsPath();
            //gp3.AddLines(p);
            List<PointF> pt = new List<PointF>();

            for (int i = 0; i < t.Length; i++)
            {
                pt.Add(p[i]); 
                if (t[i] == 129)
                {
                    gp3.AddLines(pt.ToArray());
                    gp3.CloseFigure();
                    pt.Clear();
                }
            }
            //Matrix m = new Matrix();
            //m.Rotate(-(float)(Math.PI / 2));
            //m.Rotate(-90);
            //gp.Transform(m);
            //gp4 = new GraphicsPath();
            //PointF[] pts = new PointF[5];

            //pts[0] = new PointF(6, 5);
            //pts[1] = new PointF(651, 5);
            //pts[2] = new PointF(651, 650);
            //pts[3] = new PointF(6, 650);
            //pts[4] = new PointF(6, 5);
            //pts[5] = new PointF(12, 5);


            //PointF[] pts1 = new PointF[2];

            //pts1[0] = new PointF(200, 200);
            //pts1[1] = new PointF(300, 300);

            //gp4.AddLines(pts);
            //gp.AddLines(pts1);
        }

        private void Form2_Paint(object sender, PaintEventArgs e)
        {
            if (!isMouseDown)
            { 
                ConvertPoint();
            }

            if (gp != null)
            {
                e.Graphics.InterpolationMode = InterpolationMode.HighQualityBilinear;
                e.Graphics.SmoothingMode = SmoothingMode.HighQuality;
                e.Graphics.DrawPath(Pens.Black, gp);
            }

            if (rect != RectangleF.Empty && isMouseDown)
            {
                e.Graphics.DrawRectangle(Pens.Red, rect.X, rect.Y, rect.Width, rect.Height);
            }
        }

        
        private void Form2_MouseMove(object sender, MouseEventArgs e)
        {
            this.label1.Text = "X:" + e.X;
            this.label2.Text = "Y:" + e.Y;
            if(isMouseDown)
            {
                mouseMove = e.Location;
                if (mouseDown.X != mouseMove.X || mouseDown.Y != mouseMove.Y)
                {
                    rect.X = mouseDown.X < mouseMove.X ? mouseDown.X : mouseMove.X;
                    rect.Y = mouseDown.Y < mouseMove.Y ? mouseDown.Y : mouseMove.Y;
                    rect.Width = Math.Abs(mouseDown.X - mouseMove.X);
                    rect.Height = Math.Abs(mouseDown.Y - mouseMove.Y);
                }
                else
                    rect = RectangleF.Empty;
                this.Refresh();
            }
        }

        private void Form2_MouseDown(object sender, MouseEventArgs e)
        {
            if(e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                isMouseDown = true;
                mouseDown = e.Location;
            }
        }
        Matrix m = new Matrix();
        private void Form2_MouseUp(object sender, MouseEventArgs e)
        {
            if (isMouseDown)
            {
                isMouseDown = false;
                mouseMove = e.Location;
                if (mouseDown.X != mouseMove.X || mouseDown.Y != mouseMove.Y)
                {
                    rect.X = mouseDown.X < mouseMove.X ? mouseDown.X : mouseMove.X;
                    rect.Y = mouseDown.Y < mouseMove.Y ? mouseDown.Y : mouseMove.Y;
                    rect.Width = Math.Abs(mouseDown.X - mouseMove.X);
                    rect.Height = Math.Abs(mouseDown.Y - mouseMove.Y);
                }
                else
                    rect = RectangleF.Empty;
                this.Refresh();
            }
            if (e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                //if (gp != null)
                //{
                //    gp = (GraphicsPath)gpClone.Clone();
                //}
                //rect = RectangleF.Empty;
                rect = RectangleF.Empty;
                mouseDown = PointF.Empty;
                mouseMove = PointF.Empty;
                m_ptStartPos = PointF.Empty;
                this.Refresh();
            }
        }

        private void Transform()
        {
            //if (mouseDown.X != mouseMove.X || mouseDown.Y != mouseMove.Y)
            //{
            //    rect.X = mouseDown.X < mouseMove.X ? mouseDown.X : mouseMove.X;
            //    rect.Y = mouseDown.Y < mouseMove.Y ? mouseDown.Y : mouseMove.Y;
            //    rect.Width = Math.Abs(mouseDown.X - mouseMove.X);
            //    rect.Height = Math.Abs(mouseDown.Y - mouseMove.Y);
            //}
            if (gp != null && rect!= null &&rect != RectangleF.Empty && !isMouseDown)
            {
                float dx = this.Width / rect.Width;
                float dy = this.Height / rect.Height;
                if (dx > dy)
                    dx = dy;
                m.Reset();
                m.Translate(-(rect.X + rect.Width / 2), -(rect.Y + rect.Height / 2));
                gp.Transform(m);
                m.Reset();
                m.Scale(dx, dx);
                gp.Transform(m);
                m.Reset();
                m.Translate(this.Width / 2, this.Height / 2);
                gp.Transform(m);
            }
        }

        public PointF DocToClient(PointF pt)
        {
            double d;
            d = m_dProportion * pt.X;
            pt.X = ftoi(d);
            d = m_dProportion * pt.Y;
            pt.Y = ftoi(d);

            pt.X = pt.X + m_ptStartPos.X;
            pt.Y = pt.Y + m_ptStartPos.Y;
            return pt;
        }

        public PointF DocToClient(PointF pt, double dProportion)
        {
            double d;
            d = dProportion * pt.X;
            pt.X = ftoi(d);
            d = dProportion * pt.Y;
            pt.Y = ftoi(d);
            return pt;
        }
        public int ftoi(double f)
        {
            int k;
            if (f > 5e+75 || f < -5e+75) return 1;
            if (f >= 0)
            {
                f += 0.5;
                k = (int)f;
            }
            else
            {
                f -= 0.5;
                k = (int)f;
                if (k == f) k++;
            }
            return k;
        }

        public void ConvertPoint()
        {
            if (mouseDown == PointF.Empty && mouseMove == PointF.Empty)
            {
                m_dProportion = Width / maxX;
                double d = Height / maxY;
                if (m_dProportion > d)
                    m_dProportion = d;
            }
            Scale();
            PointT[] pt = pointList.ToArray();
            List<PointF> pointList1 = new List<PointF>();
            gp = new GraphicsPath();
            int j = 0;
            int k = 0;
            for (int i = 0; i < pt.Length; i++)
            {
                
                if (pt[i].name == "PU")
                {
                    j++;
                }
                if (j > k)
                {
                    k = j;
                    if (pointList1.Count > 1)
                    {
                        gp.AddLines(pointList1.ToArray());
                        gp.StartFigure();
                    }
                    pointList1.Clear();
                }
                PointF p = pt[i].ToPointF();
                p.Y = maxY - p.Y;
                p = DocToClient(p);
                //p = ZoomIn(p.X, p.Y);
                pointList1.Add(p);
            }
            //Transform();
            //gpClone = (Graphicsp)gp.Clone();
        }

        public void Scale()
        {
            if (mouseDown != PointF.Empty && mouseMove != PointF.Empty)
            {
                double dProportion, dProportion1, dProportion2;

                dProportion1 = Math.Abs(Width) / Math.Abs(mouseDown.X - mouseMove.X);
                dProportion2 = Math.Abs(Height) / Math.Abs(mouseDown.Y - mouseMove.Y);
                if (dProportion1 > dProportion2)
                {
                    dProportion = dProportion2;
                    dProportion1 = Math.Abs(Width) / dProportion2;
                    dProportion1 = (dProportion1 - Math.Abs(mouseDown.X - mouseMove.X)) / 2;
                    dProportion2 = 0;
                }
                else
                {
                    dProportion = dProportion1;
                    dProportion2 = Math.Abs(Height) / dProportion1;
                    dProportion2 = (dProportion2 - Math.Abs(mouseDown.Y - mouseMove.Y)) / 2;
                    dProportion1 = 0;
                }
                m_dProportion *= dProportion;
                if (mouseDown.X > mouseMove.X)
                    m_ptStartPos.X = -(long)((mouseMove.X - dProportion1 - m_ptStartPos.X) * dProportion);
                else
                    m_ptStartPos.X = -(long)((mouseDown.X - dProportion1 - m_ptStartPos.X) * dProportion);
                if (mouseDown.Y > mouseMove.Y)
                    m_ptStartPos.Y = -(long)((mouseMove.Y - dProportion2 - m_ptStartPos.Y) * dProportion);
                else
                    m_ptStartPos.Y = -(long)((mouseDown.Y - dProportion2 - m_ptStartPos.Y) * dProportion);
            }
        }
    }
}