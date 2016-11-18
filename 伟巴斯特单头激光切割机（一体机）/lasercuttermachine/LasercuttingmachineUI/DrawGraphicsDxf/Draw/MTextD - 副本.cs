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

        }
        public override void InitialShape()
        {
            MText text = (MText)shape;
            GraphicsPath gp = new GraphicsPath();
            FontFamily ff = new FontFamily(text.Style.FontNameWithoutExtension);
            string drawText = text.Value;
            //drawText = "我\\P们平\\P时使\\P用的字\\P体集，只有在系统中注册过才能直接调用，但当我们使用一个别人系统中可能不存在的字体集";

            int fontStyle = (int)FontStyle.Regular;
            //int fontSetH = ff.GetCellAscent(FontStyle.Regular);
            //int fontSetT = ff.GetEmHeight(FontStyle.Regular);

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
            double rowDis = text.LineSpacingFactor * 12.5;
            double x = text.Position.X;
            double y = -text.Position.Y;
            PointF textP = new PointF((float)x, (float)y);
            GraphicsPath gptemp = new GraphicsPath();
            List<string> drawMText = new List<string>();
            List<RectangleF> drawMTextBounds = new List<RectangleF>();
            List<GraphicsPath> drawMTextPath = new List<GraphicsPath>();
            string[] drawTextA = drawText.Split(new string[] { "\\P" }, StringSplitOptions.RemoveEmptyEntries);
            for (int k = 0; k < drawTextA.Length; k++)
            {
                int j = 0;
                string s = "";
                for (int i = 1; i <= drawTextA[k].Length; i++)
                {
                    s = drawTextA[k].Substring(j, i - j);
                    gptemp.AddString(s, ff, fontStyle, fontH, PointF.Empty, sf);
                    if (gptemp.GetBounds().Width > text.RectangleWidth)
                    {
                        drawMText.Add(s.Substring(0, s.Length - 1));
                        GraphicsPath gpath = new GraphicsPath();
                        gpath.AddString(drawMText[drawMText.Count - 1], ff, fontStyle, fontH, PointF.Empty, sf);
                        drawMTextBounds.Add(gpath.GetBounds());
                        drawMTextPath.Add(gpath);
                        gptemp.Reset();

                        i = i - 1;
                        j = i;
                    }
                }
                if (s.Length > 0)
                {
                    drawMText.Add(s);
                    GraphicsPath gpath = new GraphicsPath();
                    gpath.AddString(s, ff, fontStyle, fontH, PointF.Empty, sf);
                    drawMTextBounds.Add(gpath.GetBounds());
                    drawMTextPath.Add(gpath);
                }
            }

            Matrix m = new Matrix();
            //sf.Alignment = StringAlignment.Center;
            sf.Trimming = StringTrimming.Word;
            for (int i = 0; i < drawMTextPath.Count; i++)
            {
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
                gp.AddPath(drawMTextPath[i],true);
            }

            //text.Rotation;
            if (text.Rotation != 0)
            {
                m.Reset();
                m.RotateAt((float)text.Rotation, textP);
                gp.Transform(m);
            }
            graphicsPath = gp;
            shapeType = "MText";
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
        public override void Draw(Graphics g)
        {
            //g.DrawPath(pen[0], this.graphicsPath);
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