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
            //string drawText = text.Value.Replace("\\P", "\r\n");

            //drawText = "我们\\P平时使用的字体集，只有在系统中注册过才能直接调用，但当我们使用一个别人系统中可能不存在的字体集";
            drawText = drawText.Replace("\\P", "\r\n");
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

            RectangleF rectttt = new RectangleF((float)x, (float)y, (float)(text.RectangleWidth + fontH), fontH * drawText.Length);
            gptemp.Reset();
            gptemp.AddString(drawText, ff, fontStyle, fontH, rectttt, sf);
            RectangleF rect1 = gptemp.GetBounds();
            RectangleF rect = new RectangleF();
            rect.X = rectttt.X * 2 - rect1.X;
            rect.Y = rectttt.Y * 2 - rect1.Y;
            rect.Width = rectttt.Width;
            rect.Height = rect1.Height + fontH;
            gp.AddString(drawText, ff, fontStyle, fontH, rect, sf);

            //text.Rotation;
            if (text.Rotation != 0)
            {
                Matrix m = new Matrix();
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