using CommonModule;
using CommunicationModule;
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

namespace LasercuttingmachineUI
{
    public enum MouseOperator
    {
        /// <summary>
        /// 平移
        /// </summary>
        Pan,
        /// <summary>
        /// 放大
        /// </summary>
        ZoomIn,
        /// <summary>
        /// 缩小
        /// </summary>
        ZoomOut,
        /// <summary>
        /// 编辑
        /// </summary>
        Edit,
        Defuat
    }

    public partial class PaintingPanel : UserControl
    {
        public MouseOperator mouseOperator;
        //public float minX = float.MaxValue;
        //public float minY = float.MaxValue;
        //public float maxX = float.MinValue;
        //public float maxY = float.MinValue;
        //public double m_dProportion;
        bool isMouseDown = false;
        PointF mouseDown = PointF.Empty;
        PointF mouseMove = PointF.Empty;
        //PointF m_ptStartPos = PointF.Empty;
        //Dictionary<string, GraphicsPath> gpList = new Dictionary<string, GraphicsPath>();
        public GraphicsPath graphicsP = new GraphicsPath();
        PointF pt1 = new PointF(0, 0);
        public List<Shape> dLineList = new List<Shape>();
        PointF pt2 = new PointF(100, 100);
        public float heightG;
        public float widthG;
        private string filePath1 = "";

        public bool isView = false;
        Pen redPen = new Pen(Color.Red, 1);

        public string FilePath
        {
            get { return filePath1; }
            set
            { 
                filePath1 = value;
                ParseCommunicationCommand.currentFile = Path.GetFileNameWithoutExtension(filePath1);
                if(filePath1 != "")
                    saveFilePath = Application.StartupPath + "\\cutfile\\" + Path.GetFileNameWithoutExtension(filePath1) + ".plt";
            }
        }
        public int scalePlt = 1;
        public string saveFilePath = Application.StartupPath + "\\cutfile\\";
        public PaintingPanel()
        {
            InitializeComponent();
            // | ControlStyles.DoubleBuffer  OptimizedDoubleBuffer
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint, true);
            this.UpdateStyles();
            mouseOperator = MouseOperator.Pan;
            saveFilePath = Application.StartupPath + "\\cutfile\\";
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
                case Keys.E:
                    mouseOperator = MouseOperator.Edit;
                    break;
                case Keys.W:
                    //GetGraphicFromDXF();
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
            FilePath = filepath;
            DxfDocument dxfDoc;
            try
            {
                dxfDoc = DxfDocument.Load(filepath);
            }
            catch (Exception ex)
            {
                MessageBox.Show(filepath + "文件读取错误，该文件可能正在被其他程序使用。", "提示");
                FilePath = "";
                return false;
            }
            heightG = 0;
            widthG = 0;
            //maxX = float.MinValue;
            //maxY = float.MinValue;
            //minY = float.MaxValue;
            //minX = float.MaxValue;
            mouseDown = PointF.Empty;
            mouseMove = PointF.Empty;
            isMouseDown = false;
            pltFileContent = "";

            GetGraphicFromDXF(dxfDoc);
            scalePlt = 1;

            //for (int i = 0; i < dLineList.Count; i++)
            //{
            //    Console.WriteLine(dLineList[i].pen[0].Color.ToString());
            //}
            GetDxfBound();
            GetImageRect();
            //foreach (GraphicsPath item in gpList.Values)
            //{
            //    TranformGraphics(item, MouseOperator.ZoomIn);
            //}
            Tranform(0, 0, MouseOperator.ZoomIn);
            TranformGraphics(graphicsP, MouseOperator.ZoomIn);
            if (movePath != null && movePath.PointCount > 0)
                TranformGraphics(movePath, MouseOperator.ZoomIn);
            this.Invalidate(true);
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

            float x = mouseDown.X - mouseMove.X;
            float y = mouseDown.Y - mouseMove.Y;
            PointF pt1 = new PointF(mouseDown.X + x * 0.1f, mouseDown.Y + y * 0.1f);
            PointF pt2 = new PointF(mouseDown.X - x * 1.1f, mouseDown.Y - y * 1.1f);
            mouseDown = pt1;

            mouseMove = pt2;
            //foreach (GraphicsPath item in gpList.Values)
            //{
            //    RectangleF rect = item.GetBounds();
            //    if (mouseDown.X > rect.Left)
            //    {
            //        mouseDown.X = rect.Left;
            //    }
            //    if (mouseDown.Y > rect.Top)
            //    {
            //        mouseDown.Y = rect.Top;
            //    }
            //    if (mouseMove.X < rect.Right)
            //    {
            //        mouseMove.X = rect.Right;
            //    }
            //    if (mouseMove.Y < rect.Bottom)
            //    {
            //        mouseMove.Y = rect.Bottom;
            //    }
            //}
        }

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            try
            {
                Graphics g = e.Graphics;
                //g.DrawString("tlfds\r\nf\r\n宋体\r\n宋体\r\n宋体\r\n宋体\r\n宋体", new Font("宋体", 20), Brushes.Red, new PointF(0, 0));
                g.SmoothingMode = SmoothingMode.HighSpeed;
                g.CompositingQuality = CompositingQuality.HighSpeed;
                for (int i = 0; i < dLineList.Count; i++)
                {
                    //dLineList[i].DrawBounds(g);
                    //dLineList[i].Draw(g);
                    if (isView)
                        dLineList[i].Draw(g, redPen);
                    else
                        dLineList[i].Draw(g);
                }

                if ((mouseOperator == MouseOperator.ZoomIn || mouseOperator == MouseOperator.ZoomOut) && isMouseDown && mouseMove.X != mouseDown.X && mouseDown.Y != mouseMove.Y)
                {
                    g.DrawRectangle(Pens.Red, Math.Min(mouseMove.X, mouseDown.X), Math.Min(mouseMove.Y, mouseDown.Y), Math.Abs(mouseMove.X - mouseDown.X), Math.Abs(mouseMove.Y - mouseDown.Y));
                }

                DrawMovePath(g);
            }
            catch (Exception ex)
            {

            }
        }

        private void GetGraphicFromDXF(DxfDocument dxfDoc,bool isReset = false)
        {
            //gpList.Clear();
            graphicsP.Reset();
            //gptest.Reset();
            graphicsP.AddLine(pt1, pt2);
            if (isReset)
            {
                foreach (Shape item in dLineList)
                {
                    item.graphicsPath = (GraphicsPath)item.outCutPath.Clone();
                }
                return;
            }

            if (dxfDoc == null)
                return;
            

            dLineList.Clear();
            //foreach (string item in dxfDoc.AddedEntity.Keys)
            //{
            //    Console.WriteLine(item);
            //}
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
            dLineList.Sort(CompareDinosByLength);
            RectangleF rect1 = GetPathBounds();
            if(rect1.Width/40 > 1798 || rect1.Height / 40 > 1598)
            {
                MessageBox.Show("图形范围已超出该机型切割范围！","警告！");
            }
        }
        #region 平移放大缩小
        private void TranformGraphics(GraphicsPath gp, MouseOperator mouse)
        {
            Matrix m = new Matrix();
            if (!isMouseDown && mouseMove.X != mouseDown.X && mouseDown.Y != mouseMove.Y)
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
            if (!isMouseDown && mouseMove.X != mouseDown.X && mouseDown.Y != mouseMove.Y)
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
        #endregion

        #region 控件鼠标事件
        private void Form1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                isMouseDown = true;
                mouseDown = e.Location;
                mouseMove = e.Location;
                if(mouseOperator == MouseOperator.Edit)
                {
                    foreach (Shape item in dLineList)
                    {
                        item.isSelect = item.PickedShape(e.Location);
                    }
                }
            }
        }

        private void Form1_MouseMove(object sender, MouseEventArgs e)
        {
            //this.Text = "X:" + e.X + ",Y:" + e.Y;
            //if (graphicsP.PointCount > 0)
            //{
            //    PointF p = ScreenToDxfCoord(e.X, e.Y);
            //    this.Text += ";TOX:" + p.X + ",TOY:" + p.Y;
            //}
            if (isMouseDown)
            {
                float x = e.X - mouseMove.X;
                float y = e.Y - mouseMove.Y;
                //foreach (GraphicsPath item in gpList.Values)
                //{
                //    Pan(item, x, y);
                //}
                Tranform(x, y, MouseOperator.Pan);
                Pan(graphicsP, x, y);
                if(movePath != null && movePath.PointCount > 0)
                    Pan(movePath, x, y);

                mouseMove = e.Location;
                this.Invalidate(true);
            }
        }

        private void Form1_MouseUp(object sender, MouseEventArgs e)
        {
            if (isMouseDown && e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                mouseMove = e.Location;
                isMouseDown = false;
                float x = e.X - mouseMove.X;
                float y = e.Y - mouseMove.Y;

                switch (mouseOperator)
                {
                    case MouseOperator.Pan:
                        //foreach (GraphicsPath item in gpList.Values)
                        //{
                        //    Pan(item, x, y);
                        //}
                        Tranform(x, y, MouseOperator.Pan);
                        Pan(graphicsP, x, y);
                        if (movePath != null && movePath.PointCount > 0)
                            Pan(movePath, x, y);
                        break;
                    case MouseOperator.ZoomIn:
                        //foreach (GraphicsPath item in gpList.Values)
                        //{
                        //    TranformGraphics(item, MouseOperator.ZoomIn);
                        //}
                        Tranform(x, y, MouseOperator.ZoomIn);
                        TranformGraphics(graphicsP, MouseOperator.ZoomIn);
                        if (movePath != null && movePath.PointCount > 0)
                            TranformGraphics(movePath, MouseOperator.ZoomIn);
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
                        //foreach (GraphicsPath item in gpList.Values)
                        //{
                        //    TranformGraphics(item, MouseOperator.ZoomOut);
                        //}
                        Tranform(x, y, MouseOperator.ZoomOut);
                        TranformGraphics(graphicsP, MouseOperator.ZoomOut);
                        if (movePath != null && movePath.PointCount > 0)
                            TranformGraphics(movePath, MouseOperator.ZoomOut);
                        break;
                    case MouseOperator.Defuat:
                        break;
                    case MouseOperator.Edit:
                        foreach (Shape item in dLineList)
                        {
                            if(item.isSelect)
                            {
                                x = mouseMove.X - mouseDown.X;
                                y = mouseMove.Y - mouseDown.Y;
                                item.OffSet(x, y, x, y);
                            }
                            //item.isSelect = item.PickedShape(e.Location);
                        }
                        break;
                    default:
                        break;
                }
                mouseMove = PointF.Empty;
                mouseDown = PointF.Empty;
                this.Invalidate();
            }
            if (e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                FullDisplay();
            }
        }
        #endregion

        public void FullDisplay()
        {
            GetGraphicFromDXF(null, true);
            GetDxfBound();

            Tranform(0, 0, MouseOperator.ZoomIn);
            TranformGraphics(graphicsP, MouseOperator.ZoomIn);
            if (movePath != null && movePath.PointCount > 0)
                TranformGraphics(movePath, MouseOperator.ZoomIn);
            this.Invalidate(true);
        }

        public PointF ScreenToDxfCoord(float x, float y)
        {
            PointF[] pt = graphicsP.PathPoints;
            //2016/8/12 图形旋转 begin
            float x1 = 100 / ((pt[1].X - pt[0].X) * scalePlt);
            float y1 = 100 / ((pt[1].Y - pt[0].Y) * scalePlt);

            PointF p = new PointF();
            p.Y = (x - pt[0].X) * x1;
            p.X= (y - pt[0].Y) * y1;

            //float x1 = 100 / ((pt[1].X - pt[0].X) * scalePlt);
            //float y1 = 100 / ((pt[1].Y - pt[0].Y) * scalePlt);

            //PointF p = new PointF();
            //p.X = (x - pt[0].X) * x1;
            //p.Y = (y - pt[0].Y) * y1;
            //2016/8/12 图形旋转 end

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

        public string ConvertToPLT(bool isWrite = false)
        {
            if (FilePath == "")
                return "";
            string str = "IN;DF;PA;PU;SP1;PU;SP1;";
            int indexPS = 0;
            float minx = float.MaxValue;
            float miny = float.MaxValue;
            GetImageRect(out minx, out miny);
            #region 路径优化
            PointCut[] orderPoints = GetOptimizePath(minx, miny);
            #endregion 路径优化
            if (orderPoints == null)
                return "";
            for (int j = 0; j < orderPoints.Length; j++)
            {
                PointCut pointCut = orderPoints[j];
                bool isContinePD = false;
                Shape item = (Shape)pointCut.parent;
                if (item == null)
                    continue;
                isContinePD = item.isContinePD;
                if (!isWrite && !item.IsCut)
                {
                    continue;
                }
                indexPS = 0;

                if (pointCut.ptList == null || pointCut.ptList.Length == 0)
                    continue;
                
                PointF[] ptList = pointCut.ptList;
               
                Color color = item.GetColor();

                //if (item.IsCut)
                //{
                //    if (isWrite)
                //    {
                //        //str += "SP4;";
                //    }
                //    else
                //        str += "SP4;PC1," + color.R + "," + color.G + "," + color.B + ";";
                //}
                //else
                //{
                //    if (isWrite)
                //    {
                //        str += "SP1;";
                //    }
                //    else
                //        str += "SP1;PC1," + color.R + "," + color.G + "," + color.B + ";";
                //}

                bool isText = false;
                bool isPM = false;

                if (item.shapeType == "文字" || item.shapeType == "多行文字")
                {
                    isText = true;
                }
                string pDText = "";
                string pDTextC = "PD";

                for (int i = 0; i < ptList.Length; i++)
                {
                    if (i == 0)
                    {
                        if (isText)
                        {
                            str += "PU" + ftoi(ptList[i].X) + "," + ftoi((ptList[i].Y)) + ";";
                            if (!isPM)
                            {
                                isPM = true;
                                str += "PM0;";
                            }
                        }
                        else
                        {
                            //int xi = ftoi(ptList[i].X);
                            //int yi = ftoi(ptList[i].Y);
                            if (j > 0 && ((ftoi(orderPoints[j - 1].LastPoint.X) == ftoi(ptList[i].X) && ftoi(orderPoints[j - 1].LastPoint.Y) == ftoi(ptList[i].Y))))
                            {
                                str = str.Substring(0, str.Length - 1) + ",";
                                pDText = "";
                            }
                            else
                            {
                                str += "PU" + ftoi(ptList[i].X) + "," + ftoi((ptList[i].Y)) + ";";
                                pDText = "PD";
                            }
                        }
                    }
                    else
                    {
                        if (i + 1 >= ptList.Length)
                        {
                            pDText += ftoi(ptList[i].X) + "," + ftoi((ptList[i].Y)) + ";";
                            str += pDText;
                            pDText = "";
                        }
                        else
                            pDText += ftoi(ptList[i].X) + "," + ftoi((ptList[i].Y)) + ",";

                        if (isText && isPM)
                        {
                            if (i + 1 == ptList.Length)
                            {
                                str += "PD" + ftoi(ptList[indexPS].X) + "," + ftoi((ptList[indexPS].Y)) + ";";
                                indexPS = i + 1;
                            }
                            str += "PU;PM1;";
                        }
                    }
                }
                if (isText)
                {
                    str += "PM2;FP;";
                }
            }

            str += "PU0,0;SP1;PG;";
            File.WriteAllText(saveFilePath, str);
            if (!isWrite)
            {
                if (Path.GetExtension(FilePath).ToLower() == ".dxf")
                    MessageBox.Show(saveFilePath + "文件转换完成", "提示");
                else
                    MessageBox.Show(saveFilePath + "文件保存完成", "提示");
            }
            pltFileContent = str;
            return str;
        }

        private void GetImageRect()
        {
            float minx = float.MaxValue;
            float miny = float.MaxValue;
            #region 求负值坐标
            RectangleF rect1 = GetPathBounds();
            minx = Math.Min((-rect1.Left), (-rect1.Right)) - 1;
            if (minx <= (int)minx)
                minx = (int)minx;
            miny = Math.Min((-rect1.Top), (-rect1.Bottom)) - 1;
            if (miny <= (int)miny)
                miny = (int)miny;
            #endregion 求负值坐标

            //this.widthG = Math.Abs(p1.X - p2.X);
            //this.heightG = Math.Abs(p1.Y - p2.Y);
            this.widthG = rect1.Height / 40;
            this.heightG = rect1.Width / 40;
        }

        private void GetImageRect(out float minx,out float miny)
        {
            #region 求负值坐标
            RectangleF rect1 = GetPathBounds();
            minx = Math.Min((-rect1.Left), (-rect1.Right)) - 1;
            if (minx <= (int)minx)
                minx = (int)minx;
            miny = Math.Min((-rect1.Top), (-rect1.Bottom)) - 1;
            if (miny <= (int)miny)
                miny = (int)miny;
            #endregion 求负值坐标

            //this.widthG = Math.Abs(p1.X - p2.X);
            //this.heightG = Math.Abs(p1.Y - p2.Y);
            this.widthG = rect1.Height / 40;
            this.heightG = rect1.Width / 40;
        }

        private RectangleF GetPathBounds()
        {
            GraphicsPath gptest = new GraphicsPath();
            for (int i = 0; i < dLineList.Count; i++)
            {
                if (dLineList[i].outCutPath != null)
                {
                    RectangleF pathBound = dLineList[i].outCutPath.GetBounds();
                    if (pathBound.Height > 0 && pathBound.Width > 0)
                    {
                        gptest.AddRectangle(pathBound);
                    }
                    else
                        gptest.AddPath(dLineList[i].outCutPath, false);

                    gptest.StartFigure();
                }
            }
            return gptest.GetBounds();
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

        private static int CompareDinosByLength(Shape x, Shape y)
        {
            if (x == null)
            {
                if (y == null)
                {
                    // If x is null and y is null, they're
                    // equal. 
                    return 0;
                }
                else
                {
                    // If x is null and y is not null, y
                    // is greater. 
                    return -1;
                }
            }
            else
            {
                // If x is not null...
                //
                if (y == null)
                // ...and y is null, x is greater.
                {
                    return 1;
                }
                else
                {
                    // ...and y is not null, compare the 
                    // lengths of the two strings.
                    //
                    if (x.id > y.id)
                    {
                        // If the strings are not of equal length,
                        // the longer string is greater.
                        //
                        return 1;
                    }
                    else if (x.id == y.id)
                    {
                        // If the strings are of equal length,
                        // sort them with ordinary string comparison.
                        //
                        //return x.CompareTo(y);
                        return 0;
                    }
                    else
                        return -1;
                }
            }
        }

        public void LocationPath(RectangleF rect)
        {
            PointF pt1 = new PointF(rect.X - rect.Width * 0.5f, rect.Y - rect.Height  * 0.5f);
            PointF pt2 = new PointF(rect.X + rect.Width * 1.5f, rect.Y + rect.Height * 1.5f);
            mouseDown = pt1;
            mouseMove = pt2;

            Tranform(0, 0, MouseOperator.ZoomIn);
            TranformGraphics(graphicsP, MouseOperator.ZoomIn);
            if (movePath != null && movePath.PointCount > 0)
                TranformGraphics(movePath, MouseOperator.ZoomIn);
            this.Invalidate(true);
        }
        #region plt
        public void GetGraphicsPathPlt(PointT[] pointList)
        {
            PointT[] pt = pointList;
            List<PointF> pointList1 = new List<PointF>();
            List<PointT> pointTArray = new List<PointT>();
           
            int j = 0;
            int k = 0;
            Color spColor = Color.White;
            Color pcColor = Color.Empty;
            bool IsFill = false;
            int fillIndex = -1;
            bool isSP = false;
            bool isCut = true;
            try
            {
                for (int i = 0; i < pt.Length; i++)
                {
                    switch (pt[i].name)
                    {
                        case "FP":
                            break;
                        case "PM":
                            if (pt[i].value == "0" && fillIndex == -1)
                            {
                                if (!IsFill && fillIndex == -1)
                                {
                                    LinesPlt line = GetPltLineObject(isSP, true, 0, false,isCut);

                                    //if(dLineList[dLineList.Count - 1].graphicsPath == null)
                                    //{
                                    //    LinesPlt line = (LinesPlt)dLineList[dLineList.Count - 1];
                                    //    line.isText = true;
                                    //}
                                    //else
                                    //{
                                    //    LinesPlt line = new LinesPlt(null);
                                    //    dLineList.Add(line);
                                    //    line.isText = true;
                                    //}
                                }
                                fillIndex = i;
                                IsFill = true;
                            }
                            else if (pt[i].value == "1" && fillIndex == -1)
                            {
                                LinesPlt line = GetPltLineObject(isSP, true, 1, false, isCut);

                                //if (!IsFill && fillIndex == -1)
                                //{
                                //    if (dLineList[dLineList.Count - 1].graphicsPath == null)
                                //    {
                                //        LinesPlt line = (LinesPlt)dLineList[dLineList.Count - 1];
                                //        line.isText = true;
                                //    }
                                //    else
                                //    {
                                //        LinesPlt line = new LinesPlt(null);
                                //        dLineList.Add(line);
                                //        line.isText = true;
                                //    }
                                //}
                                fillIndex = i;
                                IsFill = true;
                            }
                            else if (pt[i].value == "2" && IsFill && fillIndex != -1)
                            {
                                LinesPlt line = GetPltLineObject(isSP, true, 2, false, isCut);
                                //LinesPlt line = (LinesPlt)dLineList[dLineList.Count - 1];
                                if (line.graphicsPath == null)
                                    line.graphicsPath = new GraphicsPath();
                                line.graphicsPath.AddLines(pointList1.ToArray());
                                line.graphicsPath.StartFigure();
                                line.SetLineOut();
                                if (pointTArray.Count > 2 && pointTArray[1].index == pointTArray[pointTArray.Count - 1].index)
                                    line.isContinePD = true;
                                if (line.brush == null)
                                    line.brush = new Brush[1];
                                if (pcColor != Color.Empty)
                                {
                                    line.brush[0] = new SolidBrush(pcColor);
                                }
                                else
                                    line.brush[0] = new SolidBrush(spColor);

                                IsFill = false;
                                fillIndex = -1;
                                pointList1.Clear();
                                pointTArray.Clear();
                            }
                            break;
                        case "SP":
                            if (pointList1.Count > 0)
                            {
                                if (IsFill && dLineList.Count > 0)
                                {
                                    LinesPlt line = GetPltLineObject(isSP, IsFill, -1, false, isCut);
                                    if (line.graphicsPath == null)
                                        line.graphicsPath = new GraphicsPath();
                                    line.graphicsPath.AddLines(pointList1.ToArray());
                                    line.graphicsPath.StartFigure();
                                    if (pointTArray.Count > 2 && pointTArray[1].index == pointTArray[pointTArray.Count - 1].index)
                                        line.isContinePD = true;
                                }
                                else
                                {
                                    if(pointList1.Count > 1)
                                    {
                                        LinesPlt line = GetPltLineObject(isSP, false, -1, false, isCut);
                                        if (line.graphicsPath == null)
                                            line.graphicsPath = new GraphicsPath();

                                        line.graphicsPath.AddLines(pointList1.ToArray());
                                        line.graphicsPath.StartFigure();
                                        line.isCanReverse = true;
                                        line.SetLineOut();
                                        if (pointTArray.Count > 2 && pointTArray[1].index == pointTArray[pointTArray.Count - 1].index)
                                            line.isContinePD = true;
                                        line.pen = new Pen[1];
                                        if (pcColor != Color.Empty)
                                        {
                                            line.pen[0] = new Pen(pcColor);
                                        }
                                        else
                                            line.pen[0] = new Pen(spColor);
                                    }
                                }
                                pointList1.Clear();
                                pointTArray.Clear();
                            }
                            if (pt[i].value == "1")
                            {
                                spColor = Color.White;
                                isCut = false;
                            }
                            else if (pt[i].value == "4")
                            {
                                spColor = Color.Yellow;
                                isCut = true;
                            }
                            else
                                isCut = false;

                            isSP = true;
                            //GetPltLineObject(true, false, -1, false);
                            break;
                        case "PC":
                            //1,138,181,74
                            Color c = GetPointColor(pt[i].value);
                            if (c != Color.Empty)
                                pcColor = c;
                            break;
                    }
                    if (!pt[i].isPoint)
                        continue;
                    if (pt[i].name == "PU")
                    {
                        j++;
                    }
                    if (j > k)
                    {
                        k = j;
                        if (pointList1.Count > 1)
                        {
                            if (IsFill && dLineList.Count > 0)
                            {
                                LinesPlt line = GetPltLineObject(isSP, IsFill, -1, false, isCut);
                                //LinesPlt line = (LinesPlt)dLineList[dLineList.Count - 1];
                                if (line.graphicsPath == null)
                                    line.graphicsPath = new GraphicsPath();
                                line.graphicsPath.AddLines(pointList1.ToArray());
                                line.graphicsPath.StartFigure();
                                if (pointTArray.Count > 2 && pointTArray[1].index == pointTArray[pointTArray.Count - 1].index)
                                    line.isContinePD = true;
                            }
                            else
                            {
                                LinesPlt line = GetPltLineObject(isSP, false, -1, false, isCut);
                                //LinesPlt line;
                                //if (dLineList[dLineList.Count - 1].isText)
                                //{
                                //    line = new LinesPlt(null);
                                //    dLineList.Add(line);
                                //}
                                //else
                                //{
                                //    line = (LinesPlt)dLineList[dLineList.Count - 1];
                                //}
                                if (line.graphicsPath == null)
                                    line.graphicsPath = new GraphicsPath();

                                line.graphicsPath.AddLines(pointList1.ToArray());
                                line.graphicsPath.StartFigure();
                                line.isCanReverse = true;
                                line.SetLineOut();
                                if (pointTArray.Count > 2 && pointTArray[1].index == pointTArray[pointTArray.Count - 1].index)
                                    line.isContinePD = true;
                                line.pen = new Pen[1];
                                if (pcColor != Color.Empty)
                                {
                                    line.pen[0] = new Pen(pcColor);
                                }
                                else
                                    line.pen[0] = new Pen(spColor);
                                //dLineList.Add(line);
                            }
                            //gp.AddLines(pointList1.ToArray());
                            //gp.StartFigure();
                        }
                        pointList1.Clear();
                        pointTArray.Clear();
                    }
                    PointF p = pt[i].ToPointF();
                    //2016/8/12 图形旋转 begin
                    //p.X = maxX - p.X;
                    //p.Y = maxY - p.Y;
                    p.X = -p.X;
                    p.Y = -p.Y;
                    //p.X = maxX - p.X;
                    //2016/8/12 图形旋转 end
                    if (pointList1.Count == 0)
                    {
                        pointList1.Add(p);
                        pointTArray.Add(pt[i]);
                    }
                    else
                    {
                        if (pointList1[pointList1.Count - 1].X != p.X || pointList1[pointList1.Count - 1].Y != p.Y)
                        {
                            pointList1.Add(p);
                            pointTArray.Add(pt[i]);
                        }
                    }
                }

                if (pointList1.Count > 1)
                {
                    LinesPlt line = GetPltLineObject(isSP, false, -1, true, isCut);
                    if (line.graphicsPath == null)
                        line.graphicsPath = new GraphicsPath();

                    //LinesPlt line = new LinesPlt(null);
                    //line.graphicsPath = new GraphicsPath();
                    line.graphicsPath.AddLines(pointList1.ToArray());
                    line.graphicsPath.StartFigure();
                    line.isCanReverse = true;
                    line.SetLineOut();
                    if (pointTArray.Count > 2 && pointTArray[1].index == pointTArray[pointTArray.Count - 1].index)
                        line.isContinePD = true;
                    line.pen = new Pen[1];
                    if (pcColor != Color.Empty)
                    {
                        line.pen[0] = new Pen(pcColor);
                    }
                    else
                        line.pen[0] = new Pen(spColor);
                }
            }
            catch (Exception ex)
            {
                
            }
        }

        public LinesPlt GetPltLineObject(bool isSp,bool isText,int textN,bool isend,bool cut)
        {
            if (dLineList.Count == 0)
            {
                LinesPlt line1 = new LinesPlt(null);
                line1.isText = isText;
                line1.IsCut = cut;
                dLineList.Add(line1);
                return line1;
            }
            LinesPlt lastLine = (LinesPlt)dLineList[dLineList.Count - 1];
            if (isText && lastLine.isText)
            {
                lastLine.IsCut = cut;
                isSp = false;
                return lastLine;
            }
            if (isSp)
            {
                isSp = false;
                if (lastLine.graphicsPath == null)
                {
                    lastLine.IsCut = cut;
                    lastLine.isText = isText;
                    return lastLine;
                }
                LinesPlt line1 = new LinesPlt(null);
                line1.isText = isText;
                dLineList.Add(line1);
                line1.IsCut = cut;
                return line1;
            }
            if(isText)
            {
                if (lastLine.graphicsPath == null)
                {
                    lastLine.isText = isText;
                    lastLine.IsCut = cut;
                    return lastLine;
                }
                

                if(textN == 0 || textN == 1)
                {
                    LinesPlt line1 = new LinesPlt(null);
                    line1.isText = isText;
                    dLineList.Add(line1);
                }
                else if(textN == 2)
                {
                    if (lastLine.isText)
                    {
                        lastLine.IsCut = cut;
                        return lastLine;
                    }
                    else
                    {
                        LinesPlt line1 = new LinesPlt(null);
                        line1.isText = isText;
                        line1.IsCut = cut;
                        dLineList.Add(line1);
                        return line1;
                    }
                }
            }
            if(isend)
            {
                if(lastLine.isText && !isText)
                {
                    LinesPlt line1 = new LinesPlt(null);
                    line1.isText = isText;
                    line1.IsCut = cut;
                    dLineList.Add(line1);
                    return line1;
                }
            }
            lastLine.IsCut = cut;
            return lastLine;
        }

        public string pltFileContent = "";
        public bool ReadPltFile(string filepath)
        {
            try
            {
                if (!File.Exists(filepath))
                    return false;
                FilePath = filepath;
                List<string> operators2 = new List<string>();
                FileInfo pltFile = new FileInfo(filepath);		//the sourceFile is set.
                StreamReader reader = null;
                reader = pltFile.OpenText();
                if (reader == null)
                {
                    return false;
                }
                pltFileContent = "";
                string str;
                //while (!reader.EndOfStream)
                //{
                //    str = reader.ReadLine();

                //    string[] strlist = str.Split(new char[] { ';' }, System.StringSplitOptions.RemoveEmptyEntries);
                //    pltFileContent += str;
                //    foreach (string item1 in strlist)
                //    {
                //        if (item1.Length == 0)
                //            continue;
                //        //if (item1.Length > 2)
                //            operators2.Add(item1);
                //    }
                //}
                str = reader.ReadToEnd();
                string[] strlist = str.Split(new string[] { ";","\r\n" }, System.StringSplitOptions.RemoveEmptyEntries);
                pltFileContent = str;
                foreach (string item1 in strlist)
                {
                    if (item1.Length == 0)
                        continue;
                    operators2.Add(item1);
                    //Console.WriteLine(item1);
                }

                reader.Close();
                reader.Dispose();
                heightG = 0;
                widthG = 0;
                //maxX = float.MinValue;
                //maxY = float.MinValue;
                //minY = float.MaxValue;
                //minX = float.MaxValue;
                mouseDown = PointF.Empty;
                mouseMove = PointF.Empty;
                isMouseDown = false;
                dLineList.Clear();
                graphicsP.Reset();
                graphicsP.AddLine(pt1, pt2);
                scalePlt = 40;
                GraphicsPath gp = new GraphicsPath();
                List<PointT> pointList = new List<PointT>();
                List<PointF> pointList1 = new List<PointF>();

                for (int i = 0; i < operators2.Count; i++)
                {
                    if (operators2[i].Length < 2)
                        continue;
                    switch (operators2[i].Substring(0, 2))
                    {
                        case "PU":
                            GetPoints(pointList, operators2[i],i);
                            break;
                        case "PD":
                            GetPoints(pointList, operators2[i],i);
                            break;
                        //case "SP":
                        //    break;
                        //case "PM":
                        //    break;
                        //case "PC":
                        //    break;
                        default:
                            GetPointC(pointList, operators2[i].Substring(2, operators2[i].Length - 2), operators2[i].Substring(0, 2), i);
                            break;
                    }
                }
                GetGraphicsPathPlt(pointList.ToArray());

                for (int i = 0; i < dLineList.Count; )
                {
                    if(dLineList[i].graphicsPath == null)
                    {
                        dLineList.RemoveAt(i);
                        continue;
                    }
                    i++;
                }
                RectangleF rect1 = GetPathBounds();
                if (rect1.Width / scalePlt > 1800 || rect1.Height / scalePlt > 1600)
                {
                    MessageBox.Show("图形范围已超出该机型切割范围！", "警告！");
                }
                GetDxfBound();
                GetImageRect();
                Tranform(0, 0, MouseOperator.ZoomIn);
                TranformGraphics(graphicsP, MouseOperator.ZoomIn);
                if (movePath != null && movePath.PointCount > 0)
                    TranformGraphics(movePath, MouseOperator.ZoomIn);
                this.Invalidate(true);
                return true;
            }
            catch (Exception ex)
            {
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "\r\n");
                FilePath = "";
                return false;
            }

            //Form2 frm2 = new Form2(pointList, maxX, maxY);
            //frm2.Show();
        }

        private void GetPoints(List<PointT> pointList, string item,int index)
        {
            string[] strlist = item.Substring(2, item.Length - 2).Split(new char[] { ',',' ' }, System.StringSplitOptions.RemoveEmptyEntries);
            float x = 0;
            float y = 0;
            for (int i = 0; i < strlist.Length; )
            {
                //2016/8/12 图形旋转 begin
                x = float.Parse(strlist[i + 1]);
                y = float.Parse(strlist[i]);
                //x = float.Parse(strlist[i]);
                //y = float.Parse(strlist[i + 1]);
                //2016/8/12 图形旋转 end
                //if (x > maxX)
                //    maxX = x;
                //if (y > maxY)
                //    maxY = y;

                //if (y < minY)
                //    minY = y;
                //if (x < minX)
                //    minX = x;
                PointT p = new PointT(x, y, item.Substring(0, 2));
                p.index = index;
                pointList.Add(p);
                i += 2;
            }
        }

        private void GetPointC(List<PointT> pointList, string value,string name, int index)
        {
            PointT p = new PointT(name,value);
            p.index = index;
            pointList.Add(p);
        }

        private Color GetPointColor(string value)
        {
            string[] strA = value.Split(',');
            int R = 0;
            int G = 0;
            int B = 0;
            if(strA.Length == 4 && int.TryParse(strA[1],out R) && int.TryParse(strA[2],out G) && int.TryParse(strA[3],out B))
            {
                return Color.FromArgb(R, G, B);
            }
            return Color.Empty;
        }
        #endregion

        #region 跟踪路径
        //public List<PointF> pointfs = new List<PointF>();
        public int pathIndex = -1;
        public int pointIndex = -1;
        public PointF currentPoint;
        public PointF endPoint;
        public int endPathPointCount;
        #region 判断当前点位置
        public void HandleData(PointF p, ref int indexPt)
        {
            if (dLineList != null && dLineList.Count > 0)
            {
                for (int i = pathIndex; i < dLineList.Count; i++)
                {
                    if (dLineList[i].IsCut)
                    {
                        Pen pen = (Pen)dLineList[i].pen[0].Clone();
                        if (dLineList[i].outCutPath.IsVisible(p) || dLineList[i].outCutPath.IsOutlineVisible(p, pen))
                        {
                            if (pathIndex != i)
                            {
                                pathIndex = i;
                                pointIndex = 0;
                            }
                            int r = GetSelectPointIndex(dLineList[i].outCutPath, p, pen);
                            if (r == 1)
                            {
                                currentPoint = GetDrawCrood(p);
                                GetMoveGraphicsPath();
                                if (pathIndex + 1 == dLineList.Count && pointIndex + 2 >= endPathPointCount && p.X == endPoint.X && p.Y == endPoint.Y)
                                {
                                    indexPt = -1;
                                    pathIndex = 0;
                                    pointIndex = 0;
                                }
                                return;
                            }
                        }
                    }
                }
                if (pathIndex + 1 == dLineList.Count && pointIndex + 2 >= endPathPointCount)
                {
                    indexPt = -1;
                    pathIndex = 0;
                    pointIndex = 0;
                }
                currentPoint = GetDrawCrood(p);
                GetMoveGraphicsPath();
            }
        }

        public void HandleData(PointFlat p)
        {
            if (dLineList != null && dLineList.Count > 0)
            {
                pathIndex = p.pathIndex;
                pointIndex = p.pointIndex - 1;
                currentPoint = GetDrawCrood(p.position);
                GetMoveGraphicsPath();
            }
        }
        private void GetMoveGraphicsPath3()
        {
            if (pointIndex > 0)
            {
                PointF[] temp = new PointF[pointIndex +1];
                for (int i = 0; i < pointIndex + 1; i++)
                {
                    temp[i] = dLineList[pathIndex].graphicsPath.PathPoints[i];
                }
                if (movePath == null)
                {
                    movePath = new GraphicsPath();
                }
                else
                    movePath.Reset();
                movePath.AddLines(temp);
                movePath.AddLine(temp[temp.Length - 1], currentPoint);
                movePath.StartFigure();
                movePath.AddRectangle(new RectangleF(currentPoint.X - 4, currentPoint.Y - 4, 8, 8));
                movePath.StartFigure();
            }
        }

        public int GetSelectPointIndex(GraphicsPath gpathPick, PointF p, Pen pen)
        {
            PointF[] pt = gpathPick.PathPoints;
            byte[] ptype = gpathPick.PathTypes;
            GraphicsPath gp = new GraphicsPath();

            for (int i = pointIndex; i < pt.Length - 1; i++)
            {
                if (ptype[i + 1] == 0)
                    continue;
                if (pt[i].X == p.X && pt[i].Y == p.Y)
                {
                    pointIndex = i;
                    return 1;
                }
                else
                {
                    gp.AddLine(pt[i], pt[i + 1]);
                    if (gp.IsVisible(p) || gp.IsOutlineVisible(p, pen))
                    {
                        pointIndex = i;
                        return 1;
                    }
                }
            }
            return 0;
        }
        private GraphicsPath GetMoveGraphicsPath()
        {
            PointF[] temp = new PointF[pointIndex + 1];
            byte[] btype = new byte[pointIndex + 1];
            for (int i = 0; i < pointIndex + 1; i++)
            {
                temp[i] = dLineList[pathIndex].graphicsPath.PathPoints[i];
                btype[i] = dLineList[pathIndex].graphicsPath.PathTypes[i];
            }
            movePath = new GraphicsPath();
            if (pointIndex == 0)
            {
                movePath = new GraphicsPath();
            }
            else
                movePath = new GraphicsPath(temp, btype);

            movePath.AddLine(temp[temp.Length - 1], currentPoint);
            movePath.StartFigure();
            movePath.AddRectangle(new RectangleF(currentPoint.X - 4, currentPoint.Y - 4, 8, 8));
            movePath.StartFigure();
            for (int i = 0; i < pathIndex; i++)
            {
                if (dLineList[i].IsCut)
                    movePath.AddPath((GraphicsPath)dLineList[i].graphicsPath.Clone(), false);
            }
            return movePath;
        }
        #endregion 判断当前点位置


        #region 注释
        public void HandleData1(PointF p, ref int indexPt)
        {
            if (dLineList != null && dLineList.Count > 0)
            {
                for (int i = pathIndex; i < dLineList.Count; i++)
                {
                    if (dLineList[i].IsCut)
                    {
                        PointF[] ptLists = dLineList[i].outCutPath.PathPoints;
                        for (int j = pointIndex; j < ptLists.Length; j++)
                        {
                            if (p.X == ptLists[j].X && p.Y == ptLists[j].Y)
                            {
                                pathIndex = i;
                                pointIndex = j;
                                GetMoveGraphicsPath1();

                                if (pathIndex + 1 == dLineList.Count && pointIndex + 2 >= endPathPointCount && p.X == endPoint.X && p.Y == endPoint.Y)
                                {
                                    indexPt = -1;
                                }
                                File.AppendAllText(AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "\\log1" + DateTime.Now.ToString("yyyy-MM-dd") + ".txt", DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " :" + p.X + "," + p.Y + "," + "true \r\n");

                                return;
                            }
                        }
                    }
                }
                if (pathIndex + 1 == dLineList.Count && pointIndex + 2 >= endPathPointCount && p.X == endPoint.X && p.Y == endPoint.Y)
                {
                    indexPt = -1;
                }
                File.AppendAllText(AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "\\log1" + DateTime.Now.ToString("yyyy-MM-dd") + ".txt", DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + " :" + p.X + "," + p.Y + "," + "false \r\n");

                pointIndex++;
                if (pointIndex + 1 >= dLineList[pathIndex].graphicsPath.PointCount)
                {
                    pathIndex++;
                    pointIndex = 0;
                }
                GetMoveGraphicsPath1();
            }
        }
        private GraphicsPath GetMoveGraphicsPath1()
        {
            if (dLineList.Count > 0 && pathIndex >= 0 && pointIndex >= 0)
            {
                if (pointIndex + 1 >= dLineList[pathIndex].graphicsPath.PointCount)
                {
                    pointIndex = dLineList[pathIndex].graphicsPath.PointCount - 1;
                }
                PointF[] temp = new PointF[pointIndex + 1];
                byte[] btype = new byte[pointIndex + 1];
                for (int i = 0; i < pointIndex + 1; i++)
                {
                    temp[i] = dLineList[pathIndex].graphicsPath.PathPoints[i];
                    btype[i] = dLineList[pathIndex].graphicsPath.PathTypes[i];
                }
                if (pointIndex == 0)
                {
                    movePath = new GraphicsPath();
                }
                else
                    movePath = new GraphicsPath(temp, btype);

                movePath.AddRectangle(new RectangleF(temp[temp.Length - 1].X - 4, temp[temp.Length - 1].Y - 4, 8, 8));
                movePath.StartFigure();
                for (int i = 0; i < pathIndex; i++)
                {
                    if (dLineList[i].IsCut)
                        movePath.AddPath((GraphicsPath)dLineList[i].graphicsPath.Clone(), false);
                }
                return movePath;
            }
            else
                return null;
        }

        public GraphicsPath GetMoveGraphicsPath2()
        {
            movePath = new GraphicsPath();
            for (int i = 0; i < dLineList.Count; i++)
            {
                if (dLineList[i].IsCut)
                    movePath.AddPath((GraphicsPath)dLineList[i].graphicsPath.Clone(), false);
            }
            return movePath;
        }
        #endregion 注释

        public void DrawMovePath(Graphics g)
        {
            if (dLineList.Count > 0 && pathIndex >= 0 && pointIndex >= 0 && movePath != null)
            {
                //GraphicsPath movePath = GetMoveGraphicsPath();
                g.DrawPath(Pens.Red, movePath);
                //GraphicsMath.isPointOnSegment()
            }
        }
        GraphicsPath movePath;

        public PointF GetDrawCrood(PointF p)
        {
            PointF[] ptTemp = graphicsP.PathPoints;
            float x = ptTemp[1].X - ptTemp[0].X;
            float y = ptTemp[1].Y - ptTemp[0].Y;
            PointF ptR = new PointF(p.X * x / 100 + ptTemp[0].X, p.Y * y / 100 + ptTemp[0].Y);
            return ptR;
        }

        public string GetAllPathPoint()
        {
            string allPointsStr = "";
            for (int i = 0; i < dLineList.Count; i++)
            {
                Shape item = dLineList[i];
                if (item.outCutPath == null)
                    continue;
                //item.graphicsPath.Transform(m);
                GraphicsPath path = (GraphicsPath)item.outCutPath.Clone();
                if (item.isClose && !path.PathPoints[0].Equals(path.PathPoints[path.PointCount - 1]))
                {
                    path.AddLine(path.PathPoints[path.PointCount - 1], path.PathPoints[0]);
                }
                //PointF[] ptList = path.PathPoints;
                List<PointF> pathPoints = new List<PointF>();
                pathPoints.AddRange(path.PathPoints);

                List<CommonModule.PointFlat> pointList3 = new List<CommonModule.PointFlat>();
                pointList3.AddRange(CommonModule.GraphicsMath.FlattenLine(pathPoints.ToArray(), 40, i));

                for (int j = 0; j < pointList3.Count; j++)
                {
                    //Console.WriteLine(pointList3[j].position.X + "," + pointList3[j].position.Y+";");
                    allPointsStr += pointList3[j].position.X + "," + pointList3[j].position.Y + ";";
                }
                allPointsStr += "\r\n";
            }
            return allPointsStr;
            //for (int i = 0; i < dLineList.Count; i++)
            //{
            //    if (dLineList[i] is LinesPlt)
            //    {
            //        LinesPlt lineplt = (LinesPlt)dLineList[i];
            //        List<PointF> pathPoints = new List<PointF>();
            //        pathPoints.AddRange(lineplt.graphicsPath.PathPoints);

            //        List<CommonModule.PointFlat> pointList3 = new List<CommonModule.PointFlat>();
            //        pointList3.AddRange(CommonModule.GraphicsMath.FlattenLine(pathPoints.ToArray(), 40));

            //        for (int j = 0; j < pointList3.Count; j++)
            //        {
            //            Console.WriteLine(pointList3[j].position.X+","+pointList3[j].position.Y);
            //        }
            //        //pointFlatList = pointList.ToArray();
            //    }
            //}
        }

        public List<PointFlat> GetAllPathPoints()
        {
            List<PointFlat> allPathPoints = new List<PointFlat>();
            for (int j = 0; j < dLineList.Count;j++)
            {
                Shape item = dLineList[j];
                if (item.outCutPath == null)
                    continue;

                GraphicsPath path = (GraphicsPath)item.outCutPath.Clone();
                if (item.isClose && !path.PathPoints[0].Equals(path.PathPoints[path.PointCount - 1]))
                {
                    path.AddLine(path.PathPoints[path.PointCount - 1], path.PathPoints[0]);
                }
                List<PointF> pathPoints = new List<PointF>();
                pathPoints.AddRange(path.PathPoints);

                PointFlat[] flatPoint = CommonModule.GraphicsMath.FlattenLine(pathPoints.ToArray(), 1000,j);

                for (int i = 0; i < flatPoint.Length; i++)
                {
                    if (flatPoint[i].isMovePoint || i + 1 >= flatPoint.Length)
                        allPathPoints.Add(flatPoint[i]);
                }
            }
            
            return allPathPoints;
        }
        /// <summary>
        /// 获取完成一次切割时 最后一个点
        /// </summary>
        public void GetFinishWorkPoint()
        {
            for (int i = dLineList.Count - 1; i >= 0; i--)
            {
                if (dLineList[i].IsCut)
                {
                    endPoint = dLineList[i].outCutPath.PathPoints[dLineList[i].outCutPath.PointCount - 1];
                    endPathPointCount = dLineList[i].outCutPath.PointCount;
                }
            }
        }

        #endregion

        #region 实时路径跟踪判断
        //20160817
        //public string ConvertToPLT(bool isWrite = false)
        //{
        //    if (FilePath == "")
        //        return "";
        //    string str = "IN;DF;PA;PU;SP1;PU;";
        //    //pltFileContent = "IN;DF;PA;PU;SP1;PU;";
        //    //Matrix m = new Matrix();
        //    //if (Path.GetExtension(filePath).ToLower() == ".dxf")
        //    //{
        //    //    m.Scale(40, 40);
        //    //}
        //    PointF ptAngle1 = PointF.Empty;
        //    PointF ptAngle2 = PointF.Empty;
        //    PointF ptAngle3 = PointF.Empty;
        //    int indexPS = 0;
        //    float minx = float.MaxValue;
        //    float miny = float.MaxValue;
        //    GetImageRect(out minx, out miny);
        //    #region 路径优化
        //    PointCut[] orderPoints = GetOptimizePath(minx, miny);
        //    #endregion 路径优化
        //    if (orderPoints == null)
        //        return "";
        //    foreach (PointCut pointCut in orderPoints)
        //    {
        //        bool isContinePD = false;
        //        Shape item = (Shape)pointCut.parent;
        //        if (item == null)
        //            continue;
        //        isContinePD = item.isContinePD;
        //        if (!isWrite && !item.IsCut)
        //        {
        //            continue;
        //        }
        //        indexPS = 0;

        //        if (pointCut.ptList == null || pointCut.ptList.Length == 0)
        //            continue;
        //        //if (item.isClose && !pointCut.ptList[0].Equals(pointCut.ptList[pointCut.ptList.Length - 1]))
        //        //{
        //        //    path.AddLine(path.PathPoints[path.PointCount - 1],path.PathPoints[0]);
        //        //}
        //        PointF[] ptList = pointCut.ptList;
        //        //byte[] typeList = path.PathTypes;
        //        //RectangleF rect = path.GetBounds();

        //        //if (ptMax.X < rect.Right)
        //        //    ptMax.X = rect.Right;
        //        //if (ptMax.Y < rect.Bottom)
        //        //    ptMax.Y = rect.Bottom;

        //        //if (ptMin.X > rect.Left)
        //        //    ptMin.X = rect.Left;
        //        //if (ptMin.Y > rect.Top)
        //        //    ptMin.Y = rect.Top;

        //        Color color = item.GetColor();

        //        if (item.IsCut)
        //        {
        //            if (isWrite)
        //            {
        //                //str += "SP4;";
        //            }
        //            else
        //                str += "SP4;PC1," + color.R + "," + color.G + "," + color.B + ";";
        //        }
        //        else
        //        {
        //            if (isWrite)
        //            {
        //                str += "SP1;";
        //            }
        //            else
        //                str += "SP1;PC1," + color.R + "," + color.G + "," + color.B + ";";
        //        }

        //        bool isText = false;
        //        bool isPM = false;

        //        if (item.shapeType == "文字" || item.shapeType == "多行文字")
        //        {
        //            isText = true;
        //        }
        //        string pDText = "PD";
        //        string pDTextC = "PD";

        //        for (int i = 0; i < ptList.Length; i++)
        //        {
        //            if (i == 0)
        //            {
        //                if (isText)
        //                {
        //                    str += "PU" + ftoi(ptList[i].X - miny) + "," + ftoi((ptList[i].Y - minx)) + ";";
        //                    if (!isPM)
        //                    {
        //                        isPM = true;
        //                        str += "PM0;";
        //                    }
        //                }
        //                else
        //                {
        //                    str += "PU" + ftoi(ptList[i].X - miny) + "," + ftoi((ptList[i].Y - minx)) + ";";
        //                }
        //            }
        //            else
        //            {
        //                if (isContinePD)
        //                {
        //                    //if (i > 0 && ptList.Length > 2 && i + 1 < ptList.Length)
        //                    //{
        //                    //    ptAngle1 = ptList[i - 1];
        //                    //    ptAngle2 = ptList[i];
        //                    //    ptAngle3 = ptList[i + 1];
        //                    //    double angle = 180 - GraphicsMath.Angle(ptAngle2, ptAngle1, ptAngle3);
        //                    //    if (angle > 14.4 && pDText.Length > 2)
        //                    //    {
        //                    //        pDText += ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //        str += pDText;
        //                    //        str += "PU" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //        pDText = "PD";
        //                    //        continue;
        //                    //    }
        //                    //}

        //                    if (i + 1 >= ptList.Length)
        //                    {
        //                        pDText += ftoi(ptList[i].X - miny) + "," + ftoi((ptList[i].Y - minx)) + ";";
        //                        str += pDText;
        //                        pDText = "PD";
        //                    }
        //                    else
        //                        pDText += ftoi(ptList[i].X - miny) + "," + ftoi((ptList[i].Y - minx)) + ",";
        //                }
        //                else
        //                {
        //                    str += "PD" + ftoi(ptList[i].X - miny) + "," + ftoi((ptList[i].Y - minx)) + ";";
        //                    //if (i > 0 && ptList.Length > 2 && i + 1 < ptList.Length)
        //                    //{
        //                    //    ptAngle1 = ptList[i - 1];
        //                    //    ptAngle2 = ptList[i];
        //                    //    ptAngle3 = ptList[i + 1];
        //                    //    double angle = 180 - GraphicsMath.Angle(ptAngle2, ptAngle1, ptAngle3);
        //                    //    //Console.WriteLine(i + ":" + angle);
        //                    //    if (angle < 14.4)
        //                    //    {
        //                    //        if (i == 1)
        //                    //        {
        //                    //            pDTextC += ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ",";
        //                    //            pDTextC += ftoi(-ptList[i + 1].Y - miny) + "," + ftoi((-ptList[i + 1].X - minx)) + ",";
        //                    //        }
        //                    //        else
        //                    //        {
        //                    //            if(pDTextC == "PD")
        //                    //            {
        //                    //                pDTextC += ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ",";
        //                    //                pDTextC += ftoi(-ptList[i + 1].Y - miny) + "," + ftoi((-ptList[i + 1].X - minx)) + ",";
        //                    //            }
        //                    //            else
        //                    //            {
        //                    //                pDTextC += ftoi(-ptList[i + 1].Y - miny) + "," + ftoi((-ptList[i + 1].X - minx)) + ",";
        //                    //            }
        //                    //        }
        //                    //        continue;
        //                    //    }
        //                    //    else
        //                    //    {
        //                    //        if(pDTextC != "PD")
        //                    //        {
        //                    //            str += pDTextC.Substring(0, pDTextC.Length - 1) + ";";
        //                    //            str += "PU" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //            pDTextC = "PD";
        //                    //        }
        //                    //        else
        //                    //            str += "PD" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //    }
        //                    //}
        //                    //else
        //                    //{
        //                    //    if(pDTextC != "PD")
        //                    //    {
        //                    //        str += pDTextC.Substring(0, pDTextC.Length - 1) + ";";
        //                    //        if (i + 1 < ptList.Length)
        //                    //        {
        //                    //            str += "PU" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //        }
        //                    //        pDTextC = "PD";
        //                    //    }
        //                    //    else
        //                    //        str += "PD" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //}
        //                }

        //                if (isText && isPM)
        //                {
        //                    //if (i + 1 < typeList.Length && typeList[i + 1] == 0)
        //                    if (i + 1 == ptList.Length)
        //                    {
        //                        str += "PD" + ftoi(ptList[indexPS].X - miny) + "," + ftoi((ptList[indexPS].Y - minx)) + ";";
        //                        indexPS = i + 1;
        //                    }
        //                    str += "PU;PM1;";
        //                }
        //            }
        //        }
        //        if (isText)
        //        {
        //            str += "PM2;FP;";
        //        }
        //    }
        //    str += "PU0,0;SP1;PG;";

        //    File.WriteAllText(saveFilePath, str);
        //    if (!isWrite)
        //    {
        //        if (Path.GetExtension(FilePath).ToLower() == ".dxf")
        //            MessageBox.Show(saveFilePath + "文件转换完成", "提示");
        //        else
        //            MessageBox.Show(saveFilePath + "文件保存完成", "提示");
        //    }
        //    pltFileContent = str;
        //    return str;
        //}
        //20160816
        //public string ConvertToPLT(bool isWrite = false)
        //{
        //    if (FilePath == "")
        //        return "";
        //    string str = "IN;DF;PA;PU;SP1;PU;";
        //    //pltFileContent = "IN;DF;PA;PU;SP1;PU;";
        //    //Matrix m = new Matrix();
        //    //if (Path.GetExtension(filePath).ToLower() == ".dxf")
        //    //{
        //    //    m.Scale(40, 40);
        //    //}
        //    PointF ptAngle1 = PointF.Empty;
        //    PointF ptAngle2 = PointF.Empty;
        //    PointF ptAngle3 = PointF.Empty;
        //    int indexPS = 0;
        //    float minx = float.MaxValue;
        //    float miny = float.MaxValue;
        //    GetImageRect(out minx, out miny);
        //    #region 路径优化
        //    PointCut[] orderPoints = GetOptimizePath();
        //    #endregion 路径优化
        //    if (orderPoints == null)
        //        return "";
        //    foreach (PointCut pointCut in orderPoints)
        //    {
        //        bool isContinePD = false;
        //        Shape item = (Shape)pointCut.parent;
        //        if (item == null)
        //            continue;
        //        isContinePD = item.isContinePD;
        //        if (!isWrite && !item.IsCut)
        //        {
        //            continue;
        //        }
        //        indexPS = 0;

        //        if (pointCut.ptList == null || pointCut.ptList.Length == 0)
        //            continue;
        //        //if (item.isClose && !pointCut.ptList[0].Equals(pointCut.ptList[pointCut.ptList.Length - 1]))
        //        //{
        //        //    path.AddLine(path.PathPoints[path.PointCount - 1],path.PathPoints[0]);
        //        //}
        //        PointF[] ptList = pointCut.ptList;
        //        //byte[] typeList = path.PathTypes;
        //        //RectangleF rect = path.GetBounds();

        //        //if (ptMax.X < rect.Right)
        //        //    ptMax.X = rect.Right;
        //        //if (ptMax.Y < rect.Bottom)
        //        //    ptMax.Y = rect.Bottom;

        //        //if (ptMin.X > rect.Left)
        //        //    ptMin.X = rect.Left;
        //        //if (ptMin.Y > rect.Top)
        //        //    ptMin.Y = rect.Top;

        //        Color color = item.GetColor();

        //        if (item.IsCut)
        //        {
        //            if (isWrite)
        //            {
        //                //str += "SP4;";
        //            }
        //            else
        //                str += "SP4;PC1," + color.R + "," + color.G + "," + color.B + ";";
        //        }
        //        else
        //        {
        //            if (isWrite)
        //            {
        //                str += "SP1;";
        //            }
        //            else
        //                str += "SP1;PC1," + color.R + "," + color.G + "," + color.B + ";";
        //        }

        //        bool isText = false;
        //        bool isPM = false;

        //        if (item.shapeType == "文字" || item.shapeType == "多行文字")
        //        {
        //            isText = true;
        //        }
        //        string pDText = "PD";
        //        string pDTextC = "PD";

        //        for (int i = 0; i < ptList.Length; i++)
        //        {
        //            if (i == 0)
        //            {
        //                if (isText)
        //                {
        //                    str += "PU" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    if (!isPM)
        //                    {
        //                        isPM = true;
        //                        str += "PM0;";
        //                    }
        //                }
        //                else
        //                {
        //                    str += "PU" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                }
        //            }
        //            else
        //            {
        //                if (isContinePD)
        //                {
        //                    //if (i > 0 && ptList.Length > 2 && i + 1 < ptList.Length)
        //                    //{
        //                    //    ptAngle1 = ptList[i - 1];
        //                    //    ptAngle2 = ptList[i];
        //                    //    ptAngle3 = ptList[i + 1];
        //                    //    double angle = 180 - GraphicsMath.Angle(ptAngle2, ptAngle1, ptAngle3);
        //                    //    if (angle > 14.4 && pDText.Length > 2)
        //                    //    {
        //                    //        pDText += ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //        str += pDText;
        //                    //        str += "PU" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //        pDText = "PD";
        //                    //        continue;
        //                    //    }
        //                    //}

        //                    if (i + 1 >= ptList.Length)
        //                    {
        //                        pDText += ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                        str += pDText;
        //                        pDText = "PD";
        //                    }
        //                    else
        //                        pDText += ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ",";
        //                }
        //                else
        //                {
        //                    str += "PD" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //if (i > 0 && ptList.Length > 2 && i + 1 < ptList.Length)
        //                    //{
        //                    //    ptAngle1 = ptList[i - 1];
        //                    //    ptAngle2 = ptList[i];
        //                    //    ptAngle3 = ptList[i + 1];
        //                    //    double angle = 180 - GraphicsMath.Angle(ptAngle2, ptAngle1, ptAngle3);
        //                    //    //Console.WriteLine(i + ":" + angle);
        //                    //    if (angle < 14.4)
        //                    //    {
        //                    //        if (i == 1)
        //                    //        {
        //                    //            pDTextC += ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ",";
        //                    //            pDTextC += ftoi(-ptList[i + 1].Y - miny) + "," + ftoi((-ptList[i + 1].X - minx)) + ",";
        //                    //        }
        //                    //        else
        //                    //        {
        //                    //            if(pDTextC == "PD")
        //                    //            {
        //                    //                pDTextC += ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ",";
        //                    //                pDTextC += ftoi(-ptList[i + 1].Y - miny) + "," + ftoi((-ptList[i + 1].X - minx)) + ",";
        //                    //            }
        //                    //            else
        //                    //            {
        //                    //                pDTextC += ftoi(-ptList[i + 1].Y - miny) + "," + ftoi((-ptList[i + 1].X - minx)) + ",";
        //                    //            }
        //                    //        }
        //                    //        continue;
        //                    //    }
        //                    //    else
        //                    //    {
        //                    //        if(pDTextC != "PD")
        //                    //        {
        //                    //            str += pDTextC.Substring(0, pDTextC.Length - 1) + ";";
        //                    //            str += "PU" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //            pDTextC = "PD";
        //                    //        }
        //                    //        else
        //                    //            str += "PD" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //    }
        //                    //}
        //                    //else
        //                    //{
        //                    //    if(pDTextC != "PD")
        //                    //    {
        //                    //        str += pDTextC.Substring(0, pDTextC.Length - 1) + ";";
        //                    //        if (i + 1 < ptList.Length)
        //                    //        {
        //                    //            str += "PU" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //        }
        //                    //        pDTextC = "PD";
        //                    //    }
        //                    //    else
        //                    //        str += "PD" + ftoi(-ptList[i].Y - miny) + "," + ftoi((-ptList[i].X - minx)) + ";";
        //                    //}
        //                }

        //                if (isText && isPM)
        //                {
        //                    //if (i + 1 < typeList.Length && typeList[i + 1] == 0)
        //                    if (i + 1 == ptList.Length)
        //                    {
        //                        str += "PD" + ftoi(-ptList[indexPS].Y - miny) + "," + ftoi((-ptList[indexPS].X - minx)) + ";";
        //                        indexPS = i + 1;
        //                    }
        //                    str += "PU;PM1;";
        //                }
        //            }
        //        }
        //        if (isText)
        //        {
        //            str += "PM2;FP;";
        //        }
        //    }
        //    str += "PU0,0;SP1;PG;";

        //    File.WriteAllText(saveFilePath, str);
        //    if (!isWrite)
        //    {
        //        if (Path.GetExtension(FilePath).ToLower() == ".dxf")
        //            MessageBox.Show(saveFilePath + "文件转换完成", "提示");
        //        else
        //            MessageBox.Show(saveFilePath + "文件保存完成", "提示");
        //    }
        //    pltFileContent = str;
        //    return str;
        //}
        //20160811
        //public string ConvertToPLT(bool isWrite = false)
        //{
        //    if (filePath == "")
        //        return "";
        //    string str = "IN;DF;PA;PU;SP1;PU;";
        //    //pltFileContent = "IN;DF;PA;PU;SP1;PU;";
        //    Matrix m = new Matrix();
        //    //if (Path.GetExtension(filePath).ToLower() == ".dxf")
        //    //{
        //    //    m.Scale(40, 40);
        //    //}
        //    PointF ptMax = new PointF(float.MinValue, float.MinValue);
        //    PointF ptMin = new PointF(float.MaxValue, float.MaxValue);
        //    int indexPS = 0;
        //    foreach (Shape item in dLineList)
        //    {
        //        bool isContinePD = false;
        //        if (item is ArcD || item is CircleD || item is EllipseD || item is SplineD)
        //        {
        //            isContinePD = true;
        //        }
        //        if (!isWrite && !item.IsCut)
        //        {
        //            continue;
        //        }
        //        indexPS = 0;

        //        if (item.outCutPath == null)
        //            continue;
        //        //item.graphicsPath.Transform(m);
        //        GraphicsPath path = (GraphicsPath)item.outCutPath.Clone();
        //        if (item.isClose && !path.PathPoints[0].Equals(path.PathPoints[path.PointCount - 1]))
        //        {
        //            path.AddLine(path.PathPoints[path.PointCount - 1], path.PathPoints[0]);
        //        }

        //        path.Flatten(m, 0.1f);
        //        PointF[] ptList = path.PathPoints;
        //        byte[] typeList = path.PathTypes;
        //        RectangleF rect = path.GetBounds();

        //        if (ptMax.X < rect.Right)
        //            ptMax.X = rect.Right;
        //        if (ptMax.Y < rect.Bottom)
        //            ptMax.Y = rect.Bottom;

        //        if (ptMin.X > rect.Left)
        //            ptMin.X = rect.Left;
        //        if (ptMin.Y > rect.Top)
        //            ptMin.Y = rect.Top;

        //        Color color = item.GetColor();

        //        if (item.IsCut)
        //        {
        //            if (isWrite)
        //                str += "SP4;";
        //            else
        //                str += "SP4;PC1," + color.R + "," + color.G + "," + color.B + ";";
        //        }
        //        else
        //        {
        //            if (isWrite)
        //                str += "SP1;";
        //            else
        //                str += "SP1;PC1," + color.R + "," + color.G + "," + color.B + ";";
        //        }

        //        bool isText = false;
        //        bool isPM = false;

        //        if (item.shapeType == "Text" || item.shapeType == "MText")
        //        {
        //            isText = true;
        //        }
        //        string pDText = "";
        //        if (isContinePD)
        //        {
        //            pDText = "PD";
        //        }
        //        for (int i = 0; i < ptList.Length; i++)
        //        {
        //            if (typeList[i] == 0)
        //            {
        //                if (isText)
        //                {
        //                    str += "PU" + ftoi(ptList[i].X) + "," + ftoi((-ptList[i].Y)) + ";";
        //                    if (!isPM)
        //                    {
        //                        isPM = true;
        //                        str += "PM0;";
        //                    }
        //                }
        //                else
        //                {
        //                    str += "PU" + ftoi(ptList[i].X) + "," + ftoi((-ptList[i].Y)) + ";";
        //                }
        //            }
        //            else
        //            {
        //                if (isContinePD)
        //                {
        //                    if ((i + 1 >= typeList.Length) || (i + 1 < typeList.Length && typeList[i + 1] == 0))
        //                    {
        //                        pDText += ftoi(ptList[i].X) + "," + ftoi((-ptList[i].Y)) + ";";
        //                        str += pDText;
        //                        pDText = "";
        //                    }
        //                    else
        //                        pDText += ftoi(ptList[i].X) + "," + ftoi((-ptList[i].Y)) + ",";
        //                }
        //                else
        //                    str += "PD" + ftoi(ptList[i].X) + "," + ftoi((-ptList[i].Y)) + ";";

        //                if (typeList[i] != 1 && isText && isPM)
        //                {
        //                    if (i + 1 < typeList.Length && typeList[i + 1] == 0)
        //                    {
        //                        str += "PD" + ftoi(ptList[indexPS].X) + "," + ftoi((-ptList[indexPS].Y)) + ";";
        //                        indexPS = i + 1;
        //                    }
        //                    str += "PU;PM1;";
        //                }
        //            }
        //        }
        //        if (isText)
        //        {
        //            str += "PM2;FP;";
        //        }
        //    }
        //    str += "PU0,0;SP1;PG;";

        //    PointF p1 = ScreenToDxfCoord(ptMax.X, ptMax.Y);
        //    PointF p2 = ScreenToDxfCoord(ptMin.X, ptMin.Y);

        //    this.widthG = Math.Abs(p1.X - p2.X);
        //    this.heightG = Math.Abs(p1.Y - p2.Y);
        //    File.WriteAllText(filePath.Replace(".dxf", ".plt"), str);
        //    if (!isWrite)
        //    {
        //        if (Path.GetExtension(filePath).ToLower() == ".dxf")
        //            MessageBox.Show(filePath.Replace(".dxf", ".plt") + "文件转换完成", "提示");
        //        else
        //            MessageBox.Show(filePath + "文件保存完成", "提示");
        //    }
        //    pltFileContent = str;
        //    return str;
        //}
        
        #endregion

        #region 切割路径优化
        /// <summary>
        /// 获取切割文件中路径的起点终点
        /// </summary>
        /// <returns></returns>
        public PointCut[] GetPathStartEndPoint(float minx, float miny)
        {
            if (dLineList == null && dLineList.Count == 0)
                return null;
            List<PointCut> septList = new List<PointCut>();
            septList.Add(GetOriginPointCut());
            for (int i = 0; i < dLineList.Count; i++)
            {
                if (dLineList[i].outCutPath == null || dLineList[i].outCutPath.PointCount == 0 || !dLineList[i].IsCut)//
                    continue;
                List<PointF> points = new List<PointF>();
                PointF[] ptArray = dLineList[i].outCutPath.PathPoints;
                byte[] ptType = dLineList[i].outCutPath.PathTypes;
                for (int j = 0; j < ptArray.Length; j++)
                {
                    points.Add(new PointF(-ptArray[j].Y - miny, -ptArray[j].X - minx));
                    if (j + 1 < ptType.Length && ptType[j + 1] == 0)
                    {
                        septList.Add(GetPointCut(points.ToArray(), dLineList[i]));
                        points.Clear();
                    }
                }

                if(points.Count > 1)
                {
                    septList.Add(GetPointCut(points.ToArray(), dLineList[i]));
                    points.Clear();
                }
            }

            //foreach (PointCut item in septList)
            //{
            //    item.Reserve();
            //}
            return septList.ToArray();
        }

        private PointCut GetPointCut(PointF[] points,Shape shape)
        {
            PointCut pt = new PointCut();
            pt.ptList = points;
            pt.parent = shape;
            return pt;
        }

        private PointCut GetOriginPointCut()
        {
            PointCut pt = new PointCut();
            pt.ptList = new PointF[2];
            for (int i = 0; i < pt.ptList.Length; i++)
            {
                pt.ptList[i] = new PointF(0, 0);
            }
            return pt;
        }

        public void DrawPathOrder(PointCut[] points,int[] order)
        {
            dLineList.Clear();
            PointCut[] orderPoints = new PointCut[points.Length - 1];
            int j = 0;
            for (int i = 0; i < order.Length; i++)
            {
                if (order[i] == 0)
                    continue;
                orderPoints[j] = points[order[i]];
                j++;
            }
            LinesPlt plt = new LinesPlt(null);
            plt.graphicsPath = new GraphicsPath();
            plt.pen = new Pen[1];
            plt.pen[0] = new Pen(Color.Red, 2);
            for (int i = 0; i < orderPoints.Length; i++)
            {
                plt.graphicsPath.AddLines(orderPoints[i].ptList);
            }
            plt.SetLineOut();
            dLineList.Add(plt);
            this.Invalidate(true);
        }
        /// <summary>
        /// 路径优化
        /// </summary>
        /// <returns></returns>
        private PointCut[] GetOptimizePath(float minx, float miny)
        {
            PointCut[] pointCutArray = GetPathStartEndPoint(minx,miny);
            if (Path.GetExtension(FilePath).ToLower() == ".dxf")
            {
                List<int> orderList = new List<int>();
                orderList.Add(0);
                HamiltonianPath.GetPathArrayByDistance(pointCutArray, 0, orderList);
                PointCut[] orderPoints = new PointCut[pointCutArray.Length - 1];
                int j = 0;
                for (int i = 0; i < orderList.Count; i++)
                {
                    if (orderList[i] == 0)
                        continue;
                    orderPoints[j] = pointCutArray[orderList[i]];
                    j++;
                }
                return orderPoints;
            }
            return pointCutArray;
        }
        #endregion

        public void RemoveAll()
        {
            FilePath = "";
            dLineList.Clear();
            Invalidate();
        }
    }

    public class PointT
    {
        public string name;
        public float X;
        public float Y;
        public int index;
        public string value;
        public bool isPoint = false;
        public PointT(float x, float y, string n)
        {
            X = x;
            Y = y;
            name = n;
            isPoint = true;
        }

        public PointT(string n,string v)
        {
            name = n;
            value = v;
        }
        public PointF ToPointF()
        {
            return new PointF(X, Y);
        }
        public System.Drawing.Point ToPoint()
        {
            return new System.Drawing.Point((int)X, (int)Y);
        }
    }
}