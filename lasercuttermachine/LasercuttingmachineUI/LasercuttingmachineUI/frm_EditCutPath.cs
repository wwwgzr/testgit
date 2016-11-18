using CommonModule;
using DrawGraphicsDxf.Draw;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Text;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public delegate void PanelRefreshDelegate(int i);
    public partial class frm_EditCutPath : frm_BaseForm
    {
        public List<Shape> shapeList;
        System.Threading.Timer timer1;
        GraphicsPath selectPath;
        GraphicsPath movePath;
        Pen movePathPen;
        Shape selectShape;
        PointFlat[] pointFlatList;
        PointFlat[] pointFlatListO;
        PointFlat[] pointFlatListR;
        int selectPointIndex = 0;
        int splitLength = 10;
        public PaintingPanel paintingPanel;
        List<PointF> linePoint = new List<PointF>();

        int index = 0;
        public frm_EditCutPath()
        {
            InitializeComponent();
            this.IsCanMove = true;
            timer1 = new System.Threading.Timer(new System.Threading.TimerCallback(timer1_Tick));
        }

        void timer1_Tick(object obj)
        {
            try
            {
                if (pointFlatList == null || pointFlatList.Length < 2)
                {
                    timer1.Change(System.Threading.Timeout.Infinite, 100);
                    return;
                }
                index++;
                if (index >= pointFlatList.Length)
                {
                    index = 1;
                    movePath.Reset();
                }
                linePoint.Clear();
                double length1 = 0;
                linePoint.Add(pointFlatList[index - 1].position);
                for (int i = index; i < pointFlatList.Length; i++)
                {
                    length1 += pointFlatList[i].len;
                    linePoint.Add(pointFlatList[i].position);
                    if (pointFlatList[i].isMovePoint || i + 1 >= pointFlatList.Length)
                    {
                        index = i;
                        break;
                    }
                }

                movePath.AddLines(linePoint.ToArray());
                if (ptb_Paint.InvokeRequired)
                    ptb_Paint.Invoke(new PanelRefreshDelegate(PanelRefresh), new object[] { index });
                else
                    ptb_Paint.Invalidate();
            }
            catch (Exception ex)
            {
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "timer1_Tick77777\r\n");
            }
        }
        private void PanelRefresh(int i)
        {
            ptb_Paint.Invalidate(true);
        }

        public void InitialControl()
        {
            this.trv_GraphicShape.Nodes.Clear();
            pointFlatList = null;
            pointFlatListR = null;
            pointFlatListO = null;
            if (shapeList != null && shapeList.Count>0)
            {
                trv_GraphicShape.Tag = shapeList;
                int i = 1;
                foreach (Shape item in shapeList)
                {
                    TreeNode node = new TreeNode();
                    if(item.handle == null)
                    {
                        item.handle = i.ToString();
                        item.id = i;
                        i++;
                    }
                    node.Name = item.handle;
                    node.Text = item.shapeType + item.handle;
                    node.Tag = item;
                    trv_GraphicShape.Nodes.Add(node);
                }
                trv_GraphicShape_NodeMouseClick(trv_GraphicShape, new TreeNodeMouseClickEventArgs(trv_GraphicShape.Nodes[0], System.Windows.Forms.MouseButtons.Left, 1, 0, 0));
                if(shapeList.Count > 14)
                {
                    TreeNode node1 = new TreeNode();
                    trv_GraphicShape.Nodes.Add(node1);
                }
            }
        }

        private void frm_EditCutPath_VisibleChanged(object sender, EventArgs e)
        {
            if(Visible)
            {
                InitialControl();
            }
            else
                timer1.Change(System.Threading.Timeout.Infinite, 100);
        }

        private void btn_Save_Click(object sender, EventArgs e)
        {
            foreach (Shape item in shapeList)
            {
                if(item.outCutPathClone != null)
                {
                    GraphicsPath gp = (GraphicsPath)item.outCutPathClone.Clone();
                    Matrix m = new Matrix();
                    m.Translate(-item.pathMatrix[2, 0], -item.pathMatrix[2, 1]);
                    gp.Transform(m);
                    m.Reset();
                    m.Scale(1 / item.pathMatrix[1, 0], 1 / item.pathMatrix[1, 1]);
                    gp.Transform(m);
                    m.Reset();
                    m.Translate(-item.pathMatrix[0, 0], -item.pathMatrix[0, 1]);
                    gp.Transform(m);

                    item.outCutPath = gp;
                    item.outCutPathClone.Dispose();
                    item.outCutPathClone = null;
                }
            }
            MessageBox.Show("保存成功！", "提示");
        }

        private void btn_Exit_Click(object sender, EventArgs e)
        {
            foreach (Shape item in shapeList)
            {
                //if(item.outCutPathClone != null)
                //{
                //    if (MessageBox.Show("是否保存修改的切割路径？", "提示", MessageBoxButtons.YesNo) == System.Windows.Forms.DialogResult.Yes)
                //    {
                //        btn_Save_Click(null, null);
                //    }
                //}
                item.isSelect = false;
                item.outCutPathClone = null;
            }

            if (paintingPanel != null)
                paintingPanel.FullDisplay();
            this.Visible = false;
        }

        private void trv_GraphicShape_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if(selectShape != null && selectPath != null)
            {
                selectShape.outCutPathClone = (GraphicsPath)selectPath.Clone();
            }
            if (e.Node.Tag == null)
                return;
            timer1.Change(System.Threading.Timeout.Infinite, 100);
            trv_GraphicShape.SelectedNode = e.Node;
            index = 0;
            if (selectShape != null)
                selectShape.isSelect = false;
            selectPointIndex = 0;
            selectShape = null;
            selectPath = null;
            movePathPen = null;
            pointFlatList = null;
            chb_Reverse.Checked = false;
            chb_PickPoint.Checked = false;
            GetSelectGraphicsPath();
            if (paintingPanel != null)
            {
                RectangleF rect = selectShape.graphicsPath.GetBounds();
                if(rect.Height == 0)
                {
                    rect.Height = rect.Width;
                    rect.Y = rect.Y - rect.Height / 2;
                }
                if (rect.Width == 0)
                {
                    rect.Width = rect.Height;
                    rect.X = rect.X - rect.Width / 2;
                }
                paintingPanel.LocationPath(rect);
            }
            //ptb_Paint.Invalidate(true);
            if (selectShape != null)
            {
                if (selectShape.isCanReverse)
                {
                    timer1.Change(0, 100);
                    this.chb_Reverse.Visible = true;
                }
                else
                {
                    this.chb_Reverse.Visible = false;
                    ptb_Paint.Invalidate(true);
                }
                if (selectShape.isClose)
                {
                    this.chb_PickPoint.Visible = true;
                }
                else
                {
                    this.chb_PickPoint.Visible = false;
                    this.rab_PickKeyPoint.Visible = false;
                    this.rab_PickRandom.Visible = false;
                }
            }
        }
        private void ptb_Paint_Paint(object sender, PaintEventArgs e)
        {
            if (selectShape != null && selectPath != null)
            {
                Graphics g = e.Graphics;
                if (movePath != null && movePathPen != null && !chb_PickPoint.Checked)
                {
                    g.FillRectangle(Brushes.Lime, pointFlatList[0].position.X - 5, pointFlatList[0].position.Y - 5, 10, 10);
                    g.DrawPath(movePathPen, movePath);
                }

                if (chb_PickPoint.Checked)
                {
                    using (SolidBrush brush = new SolidBrush(Color.FromArgb(150, 0, 255, 0)))
                    {
                        g.FillEllipse(brush, pointFlatList[selectPointIndex].position.X - 15, pointFlatList[selectPointIndex].position.Y - 15, 30, 30);
                    }
                }
                if (selectShape.pen != null && selectShape.pen.Length > 0)
                    g.DrawPath(selectShape.pen[0], selectPath);
                else if (selectShape.brush != null && selectShape.brush.Length > 0)
                    g.FillPath(selectShape.brush[0], selectPath);
            }
        }

        private void GetSelectGraphicsPath()
        {
            if (trv_GraphicShape.SelectedNode.Tag == null)
                return;
            selectShape = (Shape)trv_GraphicShape.SelectedNode.Tag;

            if (selectShape.outCutPathClone != null)
                selectPath = (GraphicsPath)selectShape.outCutPathClone.Clone();
            else
            {
                selectPath = (GraphicsPath)selectShape.outCutPath.Clone();

                RectangleF rect = selectPath.GetBounds();
                Matrix m = new Matrix();
                selectShape.pathMatrix[0, 0] = -rect.X - rect.Width / 2;
                selectShape.pathMatrix[0, 1] = -rect.Y - rect.Height / 2;
                m.Translate(selectShape.pathMatrix[0, 0], selectShape.pathMatrix[0, 1]);
                selectPath.Transform(m);

                //selectShape.pathMatrix[0] = m;
                float dx = ptb_Paint.Width / (rect.Width + rect.Width * 0.05f);
                float dy = ptb_Paint.Height / (rect.Height + rect.Height * 0.05f);
                if (dx > dy)
                    dx = dy;
                m.Reset();
                m.Scale(dx, dx);
                selectPath.Transform(m);
                selectShape.pathMatrix[1, 0] = dx;
                selectShape.pathMatrix[1, 1] = dx;
                //selectShape.pathMatrix[1] = m;
                m.Reset();
                selectShape.pathMatrix[2, 0] = ptb_Paint.Width / 2;
                selectShape.pathMatrix[2, 1] = ptb_Paint.Height / 2;
                m.Translate(selectShape.pathMatrix[2, 0], selectShape.pathMatrix[2, 1]);
                selectPath.Flatten(m,0.25f);
            }

            selectShape.isSelect = true;

            //selectShape.pathMatrix[2] =m;
            if (selectShape.isCanReverse)
            {
                List<PointF> pathPoints = new List<PointF>();
                pathPoints.AddRange(selectPath.PathPoints);
                if (selectShape.isClose && !pathPoints[0].Equals(pathPoints[pathPoints.Count - 1]))
                {
                    pathPoints.Add(pathPoints[0]);
                }
                List<PointFlat> pointList = new List<PointFlat>();
                pointList.AddRange(GraphicsMath.FlattenLine(pathPoints.ToArray(), splitLength, 0));
                pointFlatList = pointList.ToArray();
                pointFlatListO = pointList.ToArray();
                pointList.Reverse();
                pointFlatListR = pointList.ToArray();
                GetMovePath();
            }
        }

        private void GetMovePath()
        {
            if (pointFlatList != null)
            {
                movePath = new GraphicsPath();
                if (selectShape.pen != null && selectShape.pen.Length > 0 && movePathPen == null)
                {
                    movePathPen = (Pen)selectShape.pen[0].Clone();
                    movePathPen.Width += 2;
                    movePathPen.Color = Color.Red;
                }
            }
        }
        private void chb_Reverse_CheckedChanged(object sender, EventArgs e)
        {
            if (pointFlatList != null)
            {
                timer1.Change(System.Threading.Timeout.Infinite, 100);
                index = 0;
                if (chb_Reverse.Checked)
                    pointFlatList = pointFlatListR;
                else
                    pointFlatList = pointFlatListO;
              
                GetMovePath();

                SaveSelectPath();

                timer1.Change(0, 100);
            }
        }

        //private void SaveSelectPath()
        //{
        //    selectPath.Reset();
        //    PointF[] ptArray = new PointF[pointFlatList.Length];
        //    for (int j = 0; j < ptArray.Length; j++)
        //    {
        //        if (pointFlatList[j].isKeyPoint || j == 0 || j == ptArray.Length - 1)
        //            ptArray[j] = pointFlatList[j].position;
        //    }
        //    selectPath.AddLines(ptArray);
        //}

        private void SaveSelectPath()
        {
            selectPath.Reset();
            List<PointF> ptArray = new List<PointF>();
            for (int j = 0; j < pointFlatList.Length; j++)
            {
                if (pointFlatList[j].isKeyPoint || j == 0 || j == pointFlatList.Length - 1)
                    ptArray.Add(pointFlatList[j].position);
            }
            selectPath.AddLines(ptArray.ToArray());
        }
        private void ptb_Paint_MouseDown(object sender, MouseEventArgs e)
        {
            if (selectPath != null && chb_PickPoint.Checked)
            {
                int w = 10;
                RectangleF rect = new RectangleF(e.X - w / 2, e.Y - w / 2, w, w);

                for (int i = 0; i < pointFlatList.Length; i++)
                {
                    if (rab_PickKeyPoint.Checked && !pointFlatList[i].isKeyPoint)
                    {
                        continue;
                    }
                    RectangleF rect1 = new RectangleF(pointFlatList[i].position.X - w / 2, pointFlatList[i].position.Y - w / 2, w, w);
                    if (rect.Contains(rect1) || rect.IntersectsWith(rect1))
                    {
                        selectPointIndex = i;
                        ptb_Paint.Invalidate(true);
                        pointFlatList = GetTurnPoint(pointFlatList, selectPointIndex);
                        GetPointReverse();
                        selectPointIndex = 0;
                        timer1.Change(System.Threading.Timeout.Infinite, 100);
                        index = 0;
                        GetMovePath();
                        SaveSelectPath();
                        timer1.Change(0, 100);
                        break;
                    }
                }
            }
        }

        private void ptb_Paint_MouseMove(object sender, MouseEventArgs e)
        {

        }

        private void ptb_Paint_MouseUp(object sender, MouseEventArgs e)
        {

        }

        private void chb_PickPoint_CheckedChanged(object sender, EventArgs e)
        {
            if (chb_PickPoint.Checked)
            {
                this.rab_PickKeyPoint.Visible = true;
                this.rab_PickRandom.Visible = true;
            }
            else
            {
                this.rab_PickKeyPoint.Visible = false;
                this.rab_PickRandom.Visible = false;
            }
        }

        private PointFlat[] GetTurnPoint(PointFlat[] pArray, int startIndex)
        {
            PointFlat[] ptList = new PointFlat[pArray.Length];
            int j = startIndex;
            for (int i = 0; i < pArray.Length - 1; i++)
            {
                if (j == pArray.Length - 1)
                {
                    j++;
                }
                if (j >= pArray.Length)
                {
                    j = j - pArray.Length;
                }
               
                ptList[i] = pArray[j];
                j++;
            }
            //ptList[0].isKeyPoint = true;
            ptList[ptList.Length - 1] = ptList[0];
            return ptList;
        }

        private void GetPointReverse()
        {
            List<PointFlat> pt = new List<PointFlat>();
            pt.AddRange(pointFlatList);
            if(chb_Reverse.Checked)
            {
                pointFlatListR = pt.ToArray();
                pt.Reverse();
                pointFlatListO = pt.ToArray();
            }
            else
            {
                pointFlatListO = pt.ToArray();
                pt.Reverse();
                pointFlatListR = pt.ToArray();
            }
            
        }
    }
}