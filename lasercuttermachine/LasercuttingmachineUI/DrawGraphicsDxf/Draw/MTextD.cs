using netDxf.Entities;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace DrawGraphicsDxf.Draw
{
    /// <summary>
    /// 单行文字
    /// </summary>
    public class MTextD : Shape
    {
        public MTextD(EntityObject eo)
            : base(eo)
        {
            isCanReverse = false;
            isText = true;
        }
        public override void InitialShape()
        {
            MText text = (MText)shape;
            GraphicsPath gp = new GraphicsPath();
            FontFamily ff = new FontFamily(text.Style.FontNameWithoutExtension);
            string drawText = text.Value;
            //drawText = "我\\P们平\\P时使\\P用的字\\P体集，只有在系统中注册过才能直接调用，但当我们使用一个别人系统中可能不存在的字体集";

            int fontStyle = (int)FontStyle.Regular;
            int fontSetH = ff.GetCellAscent(FontStyle.Regular);
            int fontSetT = ff.GetEmHeight(FontStyle.Regular);
            int fontSetT1 = ff.GetLineSpacing(FontStyle.Regular);

            StringFormat sf = new StringFormat();
            sf.LineAlignment = StringAlignment.Center;
            //文字对齐
            switch (text.AttachmentPoint)
            {
                case MTextAttachmentPoint.TopLeft:
                case MTextAttachmentPoint.MiddleLeft:
                case MTextAttachmentPoint.BottomLeft:
                    sf.Alignment = StringAlignment.Near;
                    break;
                case MTextAttachmentPoint.TopCenter:
                case MTextAttachmentPoint.MiddleCenter:
                case MTextAttachmentPoint.BottomCenter:
                    sf.Alignment = StringAlignment.Center;
                    break;
                case MTextAttachmentPoint.TopRight:
                case MTextAttachmentPoint.MiddleRight:
                case MTextAttachmentPoint.BottomRight:
                    sf.Alignment = StringAlignment.Far;
                    break;
                default:
                    sf.Alignment = StringAlignment.Near;
                    break;
            }

            float fontH = (float)(text.Height * 1.36);

            GraphicsPath gptemp = new GraphicsPath();
            gptemp.AddString("T", ff, fontStyle, fontH, PointF.Empty, sf);
            RectangleF rect = gptemp.GetBounds();
            fontH = (float)((text.Height * fontH) / rect.Height);
            gptemp.Reset();

            double rowDis = text.LineSpacingFactor * 12.5;
            //2016/8/12 图形旋转 begin
            double x = -text.Position.Y;
            double y = -text.Position.X;
            //double x = text.Position.X;
            //double y = -text.Position.Y;
            //2016/8/12 图形旋转 end
            PointF textP = new PointF((float)x, (float)y);
            string[] drawTextA = drawText.Split(new string[] { "\\P" }, StringSplitOptions.None);
            //string[] drawTextA = drawText.Split(new string[] { "\\P" }, StringSplitOptions.RemoveEmptyEntries);
            
            List<GraphicsPath> drawMTextPath = new List<GraphicsPath>();
            List<string> drawMText = new List<string>();
            List<RectangleF> drawMTextBounds = new List<RectangleF>();
            #region test
            GraphicsPath gptest = new GraphicsPath();
            string drawText1 = drawText.Replace("\\P", "\r\n");
            //GraphicsPath gptemp1 = new GraphicsPath();
            rect = new RectangleF(0, 0, (float)text.RectangleWidth + fontH, drawText1.Length * fontH);
            //gptemp1.AddString(drawText1, ff, fontStyle, fontH, rect, sf);
            gptest.AddString(drawText1, ff, fontStyle, fontH, rect, sf);
            //drawText = "f\r\nf\r\nf\r\nf\r\nf";
            //drawText = "国\r\n国\r\n国\r\n国\r\n国\r\n国";
            gptemp.AddString(drawText1, ff, fontStyle, fontH, PointF.Empty, sf);
            //RectangleF gptempBround = gptemp.GetBounds();
            //RectangleF gptempBround1 = gptemp1.GetBounds();
            //List<GraphicsPath> drawMTextPointsPath = new List<GraphicsPath>();


            //for (int k = 0; k < drawTextA.Length; k++)
            //{
            //    string s = "";
            //    for (int i = 0; i < drawTextA[k].Length; i++)
            //    {
            //        GraphicsPath gpoints = new GraphicsPath();
            //        s = drawTextA[k].Substring(i, 1);
            //        gpoints.AddString(s, ff, fontStyle, fontH, PointF.Empty, sf);
            //        drawMTextPointsPath.Add(gpoints);
            //    }
            //}
            List<float> hList = new List<float>();
            int j1 = 0;
            float h1 = 0;
            string s1 = "";

            for (int i = 1; i <= drawText1.Length; i++)
            {
                s1 = drawText1.Substring(j1, i - j1);
                gptemp.Reset();
                gptemp.AddString(s1, ff, fontStyle, fontH, rect, sf);
                RectangleF r = gptemp.GetBounds();
                if ((r.Height > fontH + h1))
                {
                    hList.Add(h1);
                }
                h1 = r.Height;
            }
            hList.Add(h1);
            gptemp.Reset();
            List<int> pointsNum = new List<int>();
            RectangleF rectf = gptest.GetBounds();
            j1 = 0;
            for (int i = 0; i < hList.Count; i++)
            {
                for (; j1 < gptest.PointCount; j1++)
                {
                    if (gptest.PathTypes[j1] == 0 && gptest.PathPoints[j1].Y > hList[i] + rectf.Y)
                    {
                        j1 -= 1;
                        pointsNum.Add(j1);
                        break;
                    }
                }
            }
            
            // drawText.Replace("\\P", "");
            //List<string> str = new List<string>();
            j1 = 0;
            s1 = "";
            int j2 = j1;
            for (int i = 0; i < pointsNum.Count; i++)
            {
                for (; j1 < drawText1.Length; j1++)
                {
                    s1 = drawText1.Substring(0, j1);
                    GraphicsPath gp1 = new GraphicsPath();
                    gp1.AddString(s1, ff, fontStyle, fontH, PointF.Empty, sf);
                    if(gp1.PointCount - 1 == pointsNum[i])
                    {
                        if (drawMText.Count > 0)
                        {
                            string ss = s1.Substring(j2, j1 - j2);
                            string[] strlist = ss.Split(new string[] { "\r\n" }, StringSplitOptions.None);
                            if (strlist.Length > 2)
                            {
                                for (int k = 2; k < strlist.Length; k++)
                                {
                                    drawMText.Add("");
                                }
                            }
                            if (ss.StartsWith(" "))
                            {
                                string ss1 = ss.TrimStart();
                                string ss2 = ss.Substring(0, ss.Length - ss1.Length);
                                drawMText[drawMText.Count - 1] += ss2;
                                drawMText.Add(ss1);
                            }
                            else
                                drawMText.Add(ss);
                        }
                        else
                            drawMText.Add(s1);
                        j2 = j1;
                        break;
                    }
                }
            }
            if (drawText1.Length > j1)
            {
                string ss = drawText1.Substring(j1, drawText1.Length - j1);
                string[] strlist = ss.Split(new string[] { "\r\n" }, StringSplitOptions.None);
                if (strlist.Length > 2)
                {
                    for (int k = 2; k < strlist.Length; k++)
                    {
                        drawMText.Add("");
                    }
                }

                if (ss.StartsWith(" "))
                {
                    string ss1 = ss.TrimStart();
                    string ss2 = ss.Substring(0, ss.Length - ss1.Length);
                    drawMText[drawMText.Count - 1] += ss2;
                    drawMText.Add(ss1);
                }
                else
                    drawMText.Add(ss);
            }

            for (int i = 0; i < drawMText.Count; i++)
            {
                GraphicsPath gpath = new GraphicsPath();
                gpath.AddString(drawMText[i], ff, fontStyle, fontH, PointF.Empty, sf);
                drawMTextBounds.Add(gpath.GetBounds());
                drawMTextPath.Add(gpath);
            }
            #endregion test

            //List<GraphicsPath> drawMTextPath = new List<GraphicsPath>();
            //List<string> drawMText = new List<string>();
            //List<RectangleF> drawMTextBounds = new List<RectangleF>();

            //for (int k = 0; k < drawTextA.Length; k++)
            //{
            //    int j = 0;
            //    string s = "";
            //    for (int i = 1; i <= drawTextA[k].Length; i++)
            //    {
            //        s = drawTextA[k].Substring(j, i - j);
            //        gptemp.AddString(s, ff, fontStyle, fontH, PointF.Empty, sf);
            //        if (gptemp.GetBounds().Width > text.RectangleWidth)
            //        {
            //            drawMText.Add(s.Substring(0, s.Length - 1));
            //            GraphicsPath gpath = new GraphicsPath();
            //            gpath.AddString(drawMText[drawMText.Count - 1], ff, fontStyle, fontH, PointF.Empty, sf);
            //            drawMTextBounds.Add(gpath.GetBounds());
            //            drawMTextPath.Add(gpath);
            //            gptemp.Reset();

            //            i = i - 1;
            //            j = i;
            //        }
            //    }
            //    if (s.Length > 0)
            //    {
            //        drawMText.Add(s);
            //        GraphicsPath gpath = new GraphicsPath();
            //        gpath.AddString(s, ff, fontStyle, fontH, PointF.Empty, sf);
            //        drawMTextBounds.Add(gpath.GetBounds());
            //        drawMTextPath.Add(gpath);
            //    }
            //}

            Matrix m = new Matrix();
            //sf.Alignment = StringAlignment.Center;
            sf.Trimming = StringTrimming.Word;
            for (int i = 0; i < drawMTextPath.Count; i++)
            {
                if (drawMTextPath[i].PointCount == 0)
                    continue;
                double offsetX = 0;
                switch (sf.Alignment)
                {
                    case StringAlignment.Center:
                        offsetX = (text.RectangleWidth - drawMTextBounds[i].Width) * 0.5;
                        break;
                    case StringAlignment.Far:
                        //右
                        offsetX = text.RectangleWidth - drawMTextBounds[i].Width;
                        break;
                    case StringAlignment.Near:
                        //左
                        offsetX = -drawMTextBounds[i].X;
                        break;
                }
                m.Translate((float)(x + offsetX), (float)(y - drawMTextBounds[i].Y + i * rowDis));
                drawMTextPath[i].Transform(m);
                m.Reset();
                gp.AddPath(drawMTextPath[i], true);
            }
            //2016/8/12 图形旋转 begin
            Matrix m1 = new Matrix();
            m1.Translate(-(float)x, -(float)y);
            gp.Transform(m1);
            m1.Reset();
            m1.Rotate(-90);
            gp.Transform(m1);
            m1.Reset();
            m1.Translate((float)x, (float)y);
            gp.Transform(m1);
            //2016/8/12 图形旋转 end

            //text.Rotation;
            if (text.Rotation != 0)
            {
                m.Reset();
                m.RotateAt((float)text.Rotation, textP);
                gp.Transform(m);
            }
            graphicsPath = gp;
            shapeType = "多行文字";
            InitialPenStyle();
        }

        public override void InitialPenStyle()
        {
            //base.InitialPenStyle();
            handle = shape.Handle;
            //样式
            brush = new SolidBrush[1];
            Color color = shape.Color.ToColor();
            if (shape.Color.IsByLayer)
            {
                color = shape.Layer.Color.ToColor();
            }
            brush[0] = new SolidBrush(color);
        }
        public override void Draw(Graphics g,Pen p)
        {
            g.FillPath(brush[0], this.graphicsPath);
        }

        public override Color GetColor()
        {
            return ((SolidBrush)brush[0]).Color;
        }



        private static bool IsHanZi(string ch)
        {
            byte[] byte_len = System.Text.Encoding.Default.GetBytes(ch);
            if (byte_len.Length == 2) { return true; }

            return false;
        }

        //public override void Transform(Matrix matrix)
        //{
        //    graphicsPath.Transform(matrix);
        //    //if (pen[0].DashStyle == DashStyle.Custom)
        //    //{
        //    //    float[] pattern = pen[0].DashPattern;
        //    //    for (int i = 0; i < pattern.Length; i++)
        //    //    {
        //    //        pattern[i] = pattern[i] * matrix.Elements[0];
        //    //    }
        //    //    pen[0].DashPattern = pattern;
        //    //}
        //}
    }
}