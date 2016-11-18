using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;

namespace LasercuttingmachineUI
{
    public partial class MenuButton : Button
    {
        public MenuButton()
        {
            InitializeComponent();
        }
    
    }
    public enum ControlState
    {
        Hover,Pressed,Normal
    }

    public enum RoundStyle
    {
        All,None,Top
    }
    public class ButtonEx : Button
    {
        private Color _baseColor = Color.FromArgb(51, 161, 224);//基颜色
        private ControlState _controlState;//控件状态
        private int _imageWidth = 18;
        private RoundStyle _roundStyle = RoundStyle.All;//圆角
        private int _radius = 8;                        //圆角半径

        public ButtonEx()
            : base()
        {
            this.SetStyle(
                ControlStyles.UserPaint |  //控件自行绘制，而不使用操作系统的绘制
                ControlStyles.AllPaintingInWmPaint | //忽略擦出的消息，减少闪烁。
                ControlStyles.OptimizedDoubleBuffer |//在缓冲区上绘制，不直接绘制到屏幕上，减少闪烁。
                ControlStyles.ResizeRedraw | //控件大小发生变化时，重绘。                  
                ControlStyles.SupportsTransparentBackColor, true);//支持透明背景颜色
        }

        [DefaultValue(typeof(Color), "51, 161, 224")]
        public Color BaseColor
        {
            get { return _baseColor; }
            set
            {
                _baseColor = value;
                base.Invalidate();
            }
        }
        [DefaultValue(18)]//默认值为18px，最小12px
        public int ImageWidth
        {
            get { return _imageWidth; }
            set
            {
                if (value != _imageWidth)
                {

                    _imageWidth = value < 12 ? 12 : value;
                    base.Invalidate();
                }
            }
        }
        [DefaultValue(typeof(RoundStyle), "1")]//默认全部都是圆角
        public RoundStyle RoundStyle
        {
            get { return _roundStyle; }
            set
            {
                if (_roundStyle != value)
                {
                    _roundStyle = value;
                    base.Invalidate();
                }
            }
        }
        [DefaultValue(8)]//设置圆角半径，默认值为8，最小值为4px
        public int Radius
        {
            get { return _radius; }
            set
            {
                if (_radius != value)
                {
                    _radius = value < 4 ? 4 : value;
                    base.Invalidate();
                }
            }
        }
        internal ControlState ControlState  //控件的状态
        {
            get { return _controlState; }
            set
            {
                if (_controlState != value)
                {
                    _controlState = value;
                    base.Invalidate();
                }
            }
        }
        protected override void OnMouseEnter(EventArgs e)//鼠标进入时
        {
            base.OnMouseEnter(e);
            ControlState = ControlState.Hover;//正常
        }
        protected override void OnMouseLeave(EventArgs e)//鼠标离开
        {
            base.OnMouseLeave(e);
            ControlState = ControlState.Normal;//正常
        }
        protected override void OnMouseDown(MouseEventArgs e)//鼠标按下
        {
            base.OnMouseDown(e);
            if (e.Button == MouseButtons.Left && e.Clicks == 1)//鼠标左键且点击次数为1
            {
                ControlState = ControlState.Pressed;//按下的状态
            }
        }
        protected override void OnMouseUp(MouseEventArgs e)//鼠标弹起
        {
            base.OnMouseUp(e);
            if (e.Button == MouseButtons.Left && e.Clicks == 1)
            {
                if (ClientRectangle.Contains(e.Location))//控件区域包含鼠标的位置
                {
                    ControlState = ControlState.Hover;
                }
                else
                {
                    ControlState = ControlState.Normal;
                }
            }
        }
        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            base.OnPaintBackground(e);

            Graphics g = e.Graphics;
            Rectangle imageRect;//图像区域
            Rectangle textRect;//文字区域

            //this.CalculateRect(out imageRect, out textRect);

            g.SmoothingMode = SmoothingMode.AntiAlias;

            Color baseColor;
            Color borderColor;
            Color innerBorderColor = this._baseColor;//Color.FromArgb(200, 255, 255, 255); ;

            if (Enabled)
            {
                switch (ControlState)
                {
                    case ControlState.Hover:
                        baseColor = GetColor(_baseColor, 0, -35, -24, -30);
                        borderColor = _baseColor;
                        break;
                    case ControlState.Pressed:
                        baseColor = GetColor(_baseColor, 0, -35, -24, -9);
                        borderColor = _baseColor;
                        break;
                    default:
                        baseColor = _baseColor;
                        borderColor = _baseColor;
                        break;
                }
            }
            else
            {
                baseColor = SystemColors.ControlDark;
                borderColor = SystemColors.ControlDark;
            }

            //this.RenderBackgroundInternal(
            //    g,
            //    ClientRectangle,
            //    baseColor,
            //    borderColor,
            //    innerBorderColor,
            //    RoundStyle,
            //    Radius,
            //    0.35f,
            //    false,
            //    true,
            //    LinearGradientMode.Vertical);

            if (Image != null)
            {
                g.InterpolationMode = InterpolationMode.HighQualityBilinear;
                //g.DrawImage(Image, imageRect, 0, 0, Image.Width, Image.Height, GraphicsUnit.Pixel);
            }
            //TextRenderer.DrawText(g, Text, Font, textRect, ForeColor, GetTextFormatFlags(TextAlign, RightToLeft == RightToLeft.Yes));
        }
        private Color GetColor(Color colorBase, int a, int r, int g, int b)
        {
            int a0 = colorBase.A;
            int r0 = colorBase.R;
            int g0 = colorBase.G;
            int b0 = colorBase.B;
            if (a + a0 > 255) { a = 255; } else { a = Math.Max(a + a0, 0); }
            if (r + r0 > 255) { r = 255; } else { r = Math.Max(r + r0, 0); }
            if (g + g0 > 255) { g = 255; } else { g = Math.Max(g + g0, 0); }
            if (b + b0 > 255) { b = 255; } else { b = Math.Max(b + b0, 0); }

            return Color.FromArgb(a, r, g, b);
        }
        internal void RenderBackgroundInternal(
               Graphics g,
               Rectangle rect,
               Color baseColor,
               Color borderColor,
               Color innerBorderColor,
               RoundStyle style,
               int roundWidth,//圆角半径
               float basePosition,
               bool drawBorder,
               bool drawGlass,
               LinearGradientMode mode)
        {
            if (drawBorder)//是否画边框
            {
                rect.Width--;
                rect.Height--;
            }

            using (LinearGradientBrush brush = new LinearGradientBrush(rect, Color.Transparent, Color.Transparent, mode))
            {
                Color[] colors = new Color[4];
                colors[0] = GetColor(baseColor, 0, 35, 24, 9);
                colors[1] = GetColor(baseColor, 0, 0, 0, 0);
                colors[2] = baseColor;
                colors[3] = GetColor(baseColor, 0, 0, 0, 0);

                ColorBlend blend = new ColorBlend();
                blend.Positions = new float[] { 0.0f, basePosition, basePosition, 1.0f };
                blend.Colors = colors;
                brush.InterpolationColors = blend;
                if (style != RoundStyle.None)
                {
                    //using (GraphicsPath path =
                    //    GraphicsPathHelper.CreatePath(rect, roundWidth, style, false))
                    //{
                    //    g.FillPath(brush, path);
                    //}

                    if (baseColor.A > 80)
                    {
                        Rectangle rectTop = rect;

                        if (mode == LinearGradientMode.Vertical)
                        {
                            // rectTop.Height = (int)(rectTop.Height * basePosition);
                        }
                        else
                        {
                            // rectTop.Width = (int)(rect.Width * basePosition);
                        }
                        //using (GraphicsPath pathTop = GraphicsPathHelper.CreatePath(
                        //    rectTop, roundWidth, RoundStyle.Top, false))
                        //{
                        //    using (SolidBrush brushAlpha =
                        //        new SolidBrush(Color.FromArgb(80, 255, 255, 255)))
                        //    {
                        //        g.FillPath(brushAlpha, pathTop);
                        //    }
                        //}
                    }

                }
            }
        }
    }
}
