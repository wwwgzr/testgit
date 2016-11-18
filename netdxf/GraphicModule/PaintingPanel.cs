using DrawGraphicsDxf.Draw;
using netDxf;
using netDxf.Blocks;
using netDxf.Entities;
using netDxf.Header;
using netDxf.Objects;
using netDxf.Tables;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Windows.Forms;
using EntitiesImage = netDxf.Entities.Image;
using EntitiesPoint = netDxf.Entities.Point;

namespace GraphicModule
{
    public partial class PaintingPanel : UserControl
    {
        DxfDocument dxfDoc;
        public MouseOperator mouseOperator;
        public float maxX = float.MinValue;
        public float maxY = float.MinValue;
        public double m_dProportion;
        bool isMouseDown = false;
        PointF mouseDown = PointF.Empty;
        PointF mouseMove = PointF.Empty;
        PointF m_ptStartPos = PointF.Empty;
        Dictionary<string, GraphicsPath> gpList = new Dictionary<string, GraphicsPath>();
        public GraphicsPath graphicsP = new GraphicsPath();
        PointF pt1 = new PointF(0, 0);
        List<Shape> dLineList = new List<Shape>();
        PointF pt2 = new PointF(100, 100);

        public PaintingPanel()
        {
            InitializeComponent();
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint, true);
            this.UpdateStyles();
            mouseOperator = MouseOperator.Pan;
        }


        private void Form1_KeyUp(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.A:
                    //if (this.openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                    //{
                    //    OpenDxfFile();
                    //}
                    break;
                case Keys.I:
                    mouseOperator = MouseOperator.ZoomIn;
                    break;
                case Keys.O:
                    mouseOperator = MouseOperator.ZoomOut;
                    break;
                case Keys.P:
                    mouseOperator = MouseOperator.Pan;
                    break;
                case Keys.D:
                    mouseOperator = MouseOperator.Defuat;
                    break;
                case Keys.W:
                    GetGraphicFromDXF();
                    ConvertToPLT();
                    break;
                default:
                    break;
            }
        }

        /// <summary>
        /// 打开dxf文件
        /// </summary>
        /// <param name="filepath">文件路径</param>
        /// <returns></returns>
        public bool OpenDxfFile(string filepath)
        {
            if (!File.Exists(filepath))
                return false;
            dxfDoc = DxfDocument.Load(filepath);
            GetGraphicFromDXF();
            //for (int i = 0; i < dLineList.Count; i++)
            //{
            //    Console.WriteLine(dLineList[i].pen[0].Color.ToString());
            //}
            GetDxfBound();
            foreach (GraphicsPath item in gpList.Values)
            {
                TranformGraphics(item, MouseOperator.ZoomIn);
            }
            this.Refresh();
            return true;
        }

        private void GetDxfBound()
        {
            mouseDown.X = float.MaxValue;
            mouseDown.Y = float.MaxValue;
            mouseMove.X = float.MinValue;
            mouseMove.Y = float.MinValue;
            foreach (Shape item in dLineList)
            {
                RectangleF rect = item.graphicsPath.GetBounds();
                if (mouseDown.X > rect.Left)
                {
                    mouseDown.X = rect.Left;
                }
                if (mouseDown.Y > rect.Top)
                {
                    mouseDown.Y = rect.Top;
                }
                if (mouseMove.X < rect.Right)
                {
                    mouseMove.X = rect.Right;
                }
                if (mouseMove.Y < rect.Bottom)
                {
                    mouseMove.Y = rect.Bottom;
                }
            }
            foreach (GraphicsPath item in gpList.Values)
            {
                RectangleF rect = item.GetBounds();
                if (mouseDown.X > rect.Left)
                {
                    mouseDown.X = rect.Left;
                }
                if (mouseDown.Y > rect.Top)
                {
                    mouseDown.Y = rect.Top;
                }
                if (mouseMove.X < rect.Right)
                {
                    mouseMove.X = rect.Right;
                }
                if (mouseMove.Y < rect.Bottom)
                {
                    mouseMove.Y = rect.Bottom;
                }
            }
        }

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            Graphics g = e.Graphics;
            //g.DrawString("tlfds\r\nf\r\n宋体\r\n宋体\r\n宋体\r\n宋体\r\n宋体", new Font("宋体", 20), Brushes.Red, new PointF(0, 0));

            foreach (Shape item in dLineList)
            {
                item.Draw(g);
            }

            if ((mouseOperator == MouseOperator.ZoomIn || mouseOperator == MouseOperator.ZoomOut) && isMouseDown && mouseMove.X != mouseDown.X && mouseDown.Y != mouseMove.Y)
            {
                g.DrawRectangle(Pens.Red, Math.Min(mouseMove.X, mouseDown.X), Math.Min(mouseMove.Y, mouseDown.Y), Math.Abs(mouseMove.X - mouseDown.X), Math.Abs(mouseMove.Y - mouseDown.Y));
            }
        }

        private void GetGraphicFromDXF()
        {
            gpList.Clear();
            graphicsP.Reset();
            //gptest.Reset();
            graphicsP.AddLine(pt1, pt2);
            dLineList.Clear();
            if (dxfDoc == null)
                return;
            //public ReadOnlyCollection<Arc> Arcs
            for (int i = 0; i < dxfDoc.Arcs.Count; i++)
            {
                ArcD arc = new ArcD(dxfDoc.Arcs[i]);
                dLineList.Add(arc);
            }
            //public ReadOnlyCollection<Ellipse> Ellipses
            for (int i = 0; i < dxfDoc.Ellipses.Count; i++)
            {
                EllipseD elp = new EllipseD(dxfDoc.Ellipses[i]);
                dLineList.Add(elp);
            }

            //public ReadOnlyCollection<Circle> Circles
            for (int i = 0; i < dxfDoc.Circles.Count; i++)
            {
                CircleD cir = new CircleD(dxfDoc.Circles[i]);
                dLineList.Add(cir);
            }

            //public ReadOnlyCollection<Face3d> Faces3d 三维面
            for (int i = 0; i < dxfDoc.Faces3d.Count; i++)
            {

            }

            //public ReadOnlyCollection<Solid> Solids 多边形 
            for (int i = 0; i < dxfDoc.Solids.Count; i++)
            {

            }
            //public ReadOnlyCollection<Insert> Inserts 插入块（有样式的点 有文字）
            for (int i = 0; i < dxfDoc.Inserts.Count; i++)
            {

            }
            //public ReadOnlyCollection<Line> Lines
            for (int i = 0; i < dxfDoc.Lines.Count; i++)
            {
                dLineList.Add(new LineD(dxfDoc.Lines[i]));
            }

            //public ReadOnlyCollection<Polyline> Polylines
            for (int i = 0; i < dxfDoc.Polylines.Count; i++)
            {
                PolylineD line = new PolylineD(dxfDoc.Polylines[i]);
                dLineList.Add(line);
            }
            //public ReadOnlyCollection<LwPolyline> LwPolylines
            for (int i = 0; i < dxfDoc.LwPolylines.Count; i++)
            {
                //LwPolyline line = dxfDoc.LwPolylines[i];
                LwPolylineD line = new LwPolylineD(dxfDoc.LwPolylines[i]);
                dLineList.Add(line);
            }
            //public ReadOnlyCollection<PolyfaceMesh> PolyfaceMeshes
            for (int i = 0; i < dxfDoc.PolyfaceMeshes.Count; i++)
            {

            }
            //public ReadOnlyCollection<Point> Points 点
            for (int i = 0; i < dxfDoc.Points.Count; i++)
            {
            }
            //public ReadOnlyCollection<Text> Texts
            for (int i = 0; i < dxfDoc.Texts.Count; i++)
            {
                TextD text = new TextD(dxfDoc.Texts[i]);
                dLineList.Add(text);
                //g.FillPath(Brushes.Red, gp);
            }
            //public ReadOnlyCollection<MText> MTexts
            for (int i = 0; i < dxfDoc.MTexts.Count; i++)
            {
                MTextD text = new MTextD(dxfDoc.MTexts[i]);
                dLineList.Add(text);
            }
            //public ReadOnlyCollection<Hatch> Hatches 图案填充
            for (int i = 0; i < dxfDoc.Hatches.Count; i++)
            {

            }
            //public ReadOnlyCollection<Image> Images
            for (int i = 0; i < dxfDoc.Images.Count; i++)
            {

            }
            //public ReadOnlyCollection<MLine> MLines 平行线
            for (int i = 0; i < dxfDoc.MLines.Count; i++)
            {

            }
            //public ReadOnlyCollection<Dimension> Dimensions标注
            for (int i = 0; i < dxfDoc.Dimensions.Count; i++)
            {

            }
            //public ReadOnlyCollection<Spline> Splines 样条曲线
            for (int i = 0; i < dxfDoc.Splines.Count; i++)
            {
                SplineD Spline = new SplineD(dxfDoc.Splines[i]);
                dLineList.Add(Spline);
            }
            //public ReadOnlyCollection<Ray> Rays 射线
            for (int i = 0; i < dxfDoc.Rays.Count; i++)
            {

            }
            //public ReadOnlyCollection<XLine> XLines 双向射线
            for (int i = 0; i < dxfDoc.XLines.Count; i++)
            {

            }
        }

        private void TranformGraphics(GraphicsPath gp, MouseOperator mouse)
        {
            Matrix m = new Matrix();
            if (!isMouseDown && mouseMove != PointF.Empty && mouseDown != PointF.Empty)
            {
                float x = Math.Min(mouseMove.X, mouseDown.X);
                float y = Math.Min(mouseMove.Y, mouseDown.Y);
                float w = Math.Abs(mouseMove.X - mouseDown.X);
                float h = Math.Abs(mouseMove.Y - mouseDown.Y);

                double dx = 1;
                double dy = 1;
                switch (mouse)
                {
                    case MouseOperator.Pan:
                        break;
                    case MouseOperator.ZoomIn:
                        dx = this.Width / w;
                        dy = this.Height / h;
                        if (dx > dy)
                            dx = dy;
                        if (w < 3 && h < 3)
                        {
                            dx = 1.1f;
                        }
                        break;
                    case MouseOperator.ZoomOut:
                        dx = w / this.Width;
                        dy = h / this.Height;
                        if (dx < dy)
                            dx = dy;
                        if (w < 3 && h < 3)
                        {
                            dx = 0.9f;
                        }
                        break;
                    case MouseOperator.Defuat:
                        break;
                    default:
                        break;
                }

                m.Translate(-(x + w * 0.5f), -(y + h * 0.5f));
                gp.Transform(m);
                m.Reset();
                m.Scale((float)dx, (float)dx);

                gp.Transform(m);
                m.Reset();
                m.Translate(Width * 0.5f, Height * 0.5f);
                gp.Transform(m);
            }
        }
        private void TranformGraphics(Shape gp, MouseOperator mouse)
        {
            Matrix m = new Matrix();
            if (!isMouseDown && mouseMove != PointF.Empty && mouseDown != PointF.Empty)
            {
                float x = Math.Min(mouseMove.X, mouseDown.X);
                float y = Math.Min(mouseMove.Y, mouseDown.Y);
                float w = Math.Abs(mouseMove.X - mouseDown.X);
                float h = Math.Abs(mouseMove.Y - mouseDown.Y);

                double dx = 1;
                double dy = 1;
                switch (mouse)
                {
                    case MouseOperator.Pan:
                        break;
                    case MouseOperator.ZoomIn:
                        dx = this.Width / w;
                        dy = this.Height / h;
                        if (dx > dy)
                            dx = dy;
                        if (w < 3 && h < 3)
                        {
                            dx = 1.1f;
                        }
                        break;
                    case MouseOperator.ZoomOut:
                        dx = w / this.Width;
                        dy = h / this.Height;
                        if (dx < dy)
                            dx = dy;
                        if (w < 3 && h < 3)
                        {
                            dx = 0.9f;
                        }
                        break;
                    case MouseOperator.Defuat:
                        break;
                    default:
                        break;
                }

                m.Translate(-(x + w * 0.5f), -(y + h * 0.5f));
                gp.Transform(m);
                m.Reset();
                m.Scale((float)dx, (float)dx);

                gp.Transform(m);
                m.Reset();
                m.Translate(Width * 0.5f, Height * 0.5f);
                gp.Transform(m);
            }
        }
        private void Pan(GraphicsPath gp, float x, float y)
        {
            if (isMouseDown && mouseOperator == MouseOperator.Pan)
            {
                Matrix m = new Matrix();
                m.Translate(x, y);
                gp.Transform(m);
            }
        }

        private void Tranform(float x, float y, MouseOperator mouse)
        {
            for (int i = 0; i < dLineList.Count; i++)
            {
                switch (mouse)
                {
                    case MouseOperator.Pan:
                        Pan(dLineList[i].graphicsPath, x, y);
                        break;
                    case MouseOperator.ZoomIn:
                        //TranformGraphics(dLineList[i].graphicsPath, MouseOperator.ZoomIn);
                        TranformGraphics(dLineList[i], MouseOperator.ZoomIn);
                        break;
                    case MouseOperator.ZoomOut:
                        //TranformGraphics(dLineList[i].graphicsPath, MouseOperator.ZoomOut);
                        TranformGraphics(dLineList[i], MouseOperator.ZoomOut);
                        break;
                    case MouseOperator.Defuat:
                        break;
                    default:
                        break;
                }
            }

        }

        private void Form1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                isMouseDown = true;
                mouseDown = e.Location;
                mouseMove = e.Location;
            }
        }

        private void Form1_MouseMove(object sender, MouseEventArgs e)
        {
            this.Text = "X:" + e.X + ",Y:" + e.Y;
            if (graphicsP.PointCount > 0)
            {
                PointF p = ScreenToDxfCoord(e.X, e.Y);
                this.Text += ";TOX:" + p.X + ",TOY:" + p.Y;
            }
            if (isMouseDown)
            {
                float x = e.X - mouseMove.X;
                float y = e.Y - mouseMove.Y;
                foreach (GraphicsPath item in gpList.Values)
                {
                    Pan(item, x, y);
                }
                Tranform(x, y, MouseOperator.Pan);
                Pan(graphicsP, x, y);
                mouseMove = e.Location;
                this.Refresh();
            }
        }

        private void Form1_MouseUp(object sender, MouseEventArgs e)
        {
            if (isMouseDown)
            {
                mouseMove = e.Location;
                isMouseDown = false;
                float x = e.X - mouseMove.X;
                float y = e.Y - mouseMove.Y;

                switch (mouseOperator)
                {
                    case MouseOperator.Pan:
                        foreach (GraphicsPath item in gpList.Values)
                        {
                            Pan(item, x, y);
                        }
                        Tranform(x, y, MouseOperator.Pan);
                        Pan(graphicsP, x, y);
                        break;
                    case MouseOperator.ZoomIn:
                        foreach (GraphicsPath item in gpList.Values)
                        {
                            TranformGraphics(item, MouseOperator.ZoomIn);
                        }
                        Tranform(x, y, MouseOperator.ZoomIn);
                        TranformGraphics(graphicsP, MouseOperator.ZoomIn);
                        //dLineList[1].pen[0].ScaleTransform(2, 2);

                        //Brush b;
                        //SolidBrush s;
                        //PathGradientBrush;
                        //LinearGradientBrush;
                        //TextureBrush;
                        //HatchBrush;
                        //Pen
                        break;
                    case MouseOperator.ZoomOut:
                        foreach (GraphicsPath item in gpList.Values)
                        {
                            TranformGraphics(item, MouseOperator.ZoomOut);
                        }
                        Tranform(x, y, MouseOperator.ZoomOut);
                        TranformGraphics(graphicsP, MouseOperator.ZoomOut);
                        break;
                    case MouseOperator.Defuat:
                        break;
                    default:
                        break;
                }
                mouseMove = PointF.Empty;
                mouseDown = PointF.Empty;
                this.Refresh();
            }
            if (e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                GetGraphicFromDXF();
                GetDxfBound();

                Tranform(0, 0, MouseOperator.ZoomIn);
                TranformGraphics(graphicsP, MouseOperator.ZoomIn);
                this.Refresh();
            }
        }

        public PointF ScreenToDxfCoord(float x, float y)
        {
            PointF[] pt = graphicsP.PathPoints;
            float x1 = 100 / (pt[1].X - pt[0].X);
            float y1 = 100 / (pt[1].Y - pt[0].Y);

            PointF p = new PointF();
            p.X = (x - pt[0].X) * x1;
            p.Y = (y - pt[0].Y) * y1;
            return p;
            //double d = ComputeDistance(pt[0], pt[1]) / ComputeDistance(pt1, pt2);
            //float x1 = (pt[0].X + pt[1].X - pt1.X - pt2.X) * 0.5f;
            //float y1 = (pt[0].Y + pt[1].Y - pt1.Y - pt2.Y) * 0.5f;
            //return new PointF((float)(x / d + x1), (float)(y / d + y1));
        }

        public double ComputeDistance(PointF p1, PointF p2)
        {
            return Math.Sqrt((p1.X - p2.X) * (p1.X - p2.X) + (p1.Y - p2.Y) * (p1.Y - p2.Y));
        }

        public void ConvertToPLT()
        {
            string str = "IN;DF;PA;PU;SP1;PU;SP1;";
            Matrix m = new Matrix();
            m.Scale(40, 40);

            foreach (Shape item in dLineList)
            {
                item.graphicsPath.Transform(m);
                item.graphicsPath.Flatten();
                PointF[] ptList = item.graphicsPath.PathPoints;
                byte[] typeList = item.graphicsPath.PathTypes;

                Color color = item.GetColor();
                str += "PC1," + color.R + "," + color.G + "," + color.B + ";";
                bool isText = false;
                bool isPM = false;

                if (item.shapeType == "Text" || item.shapeType == "MText")
                {
                    isText = true;
                }
                for (int i = 0; i < ptList.Length; i++)
                {
                    if (typeList[i] == 0)
                    {
                        if (isText)
                        {

                            str += "PU" + ftoi(ptList[i].X) + "," + ftoi((-ptList[i].Y)) + ";";
                            if (!isPM)
                            {
                                isPM = true;
                                str += "PM0;";
                            }
                        }
                        else
                        {
                            str += "PU" + ftoi(ptList[i].X) + "," + ftoi((-ptList[i].Y)) + ";";
                        }
                    }
                    else
                    {
                        str += "PD" + ftoi(ptList[i].X) + "," + ftoi((-ptList[i].Y)) + ";";
                        if (typeList[i] != 1 && isText && isPM)
                            str += "PU;PM1;";
                    }
                }
                if (isText)
                {
                    str += "PM2;FP;";
                }
            }
            str += "PU0,0;SP1;PG;";
            File.WriteAllText(Application.StartupPath + "1.txt", str);
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

    }

}
