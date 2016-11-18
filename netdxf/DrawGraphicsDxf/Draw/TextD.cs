using netDxf.Entities;
using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace DrawGraphicsDxf.Draw
{
    /// <summary>
    /// 单行文字
    /// </summary>
    public class TextD : Shape
    {
        public TextD(EntityObject eo)
            : base(eo)
        {

        }

        public override void InitialShape()
        {
            Text text = (Text)shape;
            GraphicsPath gp = new GraphicsPath();

            FontFamily ff = new FontFamily(text.Style.FontNameWithoutExtension);
            int fontStyle = (int)FontStyle.Regular;
            StringFormat sf = new StringFormat();
            sf.Alignment = StringAlignment.Center;
            sf.LineAlignment = StringAlignment.Center;
            float fontH = (float)(text.Height);
            GraphicsPath gptemp = new GraphicsPath();
            gptemp.AddString("T", ff, fontStyle, fontH, PointF.Empty, sf);
            RectangleF rect = gptemp.GetBounds();

            fontH = (float)((text.Height * fontH)/rect.Height);
            gptemp.Reset();
            gptemp.AddString(text.Value, ff, fontStyle, fontH, PointF.Empty, sf);
            rect = gptemp.GetBounds();
            rect.Width = rect.Width + fontH;
            rect.Height = rect.Height + 1;
            double x = text.Position.X;
            double y = -text.Position.Y;
            //文字对齐
            switch (text.Alignment)
            {
                case TextAlignment.TopLeft:
                    rect.X = (float)(x);
                    rect.Y = (float)(y);
                    break;
                case TextAlignment.TopCenter:
                    rect.X = (float)(x - rect.Width * 0.5);
                    rect.Y = (float)(y);
                    break;
                case TextAlignment.TopRight:
                    rect.X = (float)(x - rect.Width);
                    rect.Y = (float)(y);
                    break;
                case TextAlignment.MiddleLeft:
                    rect.X = (float)(x);
                    rect.Y = (float)(y - rect.Height * 0.5);
                    break;
                case TextAlignment.MiddleCenter:
                    rect.X = (float)(x - rect.Width * 0.5);
                    rect.Y = (float)(y - rect.Height * 0.5);
                    break;
                case TextAlignment.MiddleRight:
                    rect.X = (float)(x - rect.Width);
                    rect.Y = (float)(y - rect.Height * 0.5);
                    break;
                case TextAlignment.BottomLeft:
                    rect.X = (float)(x);
                    rect.Y = (float)(y - rect.Height);
                    break;
                case TextAlignment.BottomCenter:
                    rect.X = (float)(x - rect.Width * 0.5);
                    rect.Y = (float)(y - rect.Height);
                    break;
                case TextAlignment.BottomRight:
                    rect.X = (float)(x - rect.Width);
                    rect.Y = (float)(y - rect.Height);
                    break;
                case TextAlignment.Fit:
                    rect.X = (float)(x);
                    rect.Y = (float)(y - rect.Height);
                    break;
                default:
                    rect.X = (float)(x - rect.Width * 0.5);
                    rect.Y = (float)(y - rect.Height * 0.5);
                    break;
            }
            gp.AddString(text.Value, ff, fontStyle, fontH, rect, sf);
            if (!(text.MirrorXY == 0 && !text.Style.IsUpsideDown && !text.Style.IsBackward))
            {
                Matrix m = new Matrix();
                float offsetX = (float)x;
                float offsetY = (float)y;
                m.Translate(-offsetX, -offsetY);
                gp.Transform(m);
                m.Reset();
                //text.Style.IsBackward;
                if (text.MirrorXY == 2 || (text.MirrorXY == 0 && text.Style.IsBackward))
                {
                    m.Scale(-1, 1);
                    gp.Transform(m);
                }
                //text.Style.IsUpsideDown;
                else if (text.MirrorXY == 4 || (text.MirrorXY == 0 && text.Style.IsUpsideDown))
                {
                    m.Scale(1, -1);
                    gp.Transform(m);
                }
                m.Reset();
                m.Translate(offsetX, offsetY);
                gp.Transform(m);
            }
            //设置斜体文字text.ObliqueAngle为倾斜角度
            if (text.ObliqueAngle != 0)
            {
                PointF[] pt = new PointF[4];
                //text.ObliqueAngle
                double offX = rect.Height * Math.Tan((text.ObliqueAngle * Math.PI) / 180);
                pt[0] = new PointF((float)(rect.Left + offX), rect.Top);
                pt[1] = new PointF((float)(rect.Right + offX), rect.Top);
                pt[2] = new PointF(rect.Left, rect.Bottom);
                pt[3] = new PointF(rect.Right, rect.Bottom);

                gp.Warp(pt, rect);
            }
            //text.WidthFactor;
            if (text.WidthFactor != 1)
            {
                Matrix m = new Matrix();
                m.Scale((float)text.WidthFactor, 1);
                gp.Transform(m);
            }
            //text.Rotation;
            if (text.Rotation != 0)
            {
                Matrix m = new Matrix();
                m.RotateAt((float)text.Rotation, rect.Location);
                gp.Transform(m);
            }
            graphicsPath = gp;
            rect = graphicsPath.GetBounds();
            shapeType = "Text";
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