using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Net.Sockets;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class 定位点 : Form
    {
        public 定位点()
        {
            InitializeComponent();
        }

        private void 定位点_MouseMove(object sender, MouseEventArgs e)
        {
            this.txt_X.Text = e.X.ToString();
            this.txt_Y.Text = e.Y.ToString();
            
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {
            GetGraphicsPath(e.Graphics);
        }

        private void GetGraphicsPath(Graphics g)
        {
            GraphicsPath gp = new GraphicsPath();
            FontFamily ff = new FontFamily("Arial");
            int fontStyle = (int)FontStyle.Regular;
            StringFormat sf = new StringFormat();
            sf.Alignment = StringAlignment.Center;
            sf.LineAlignment = StringAlignment.Center;
            float fontH = 308;
            gp.AddString("T", ff, fontStyle, fontH, PointF.Empty, sf);
            RectangleF rect = gp.GetBounds();

            Matrix m = new Matrix();
            m.Translate(-rect.X, -rect.Y);
            gp.Transform(m);
            g.FillPath(Brushes.Red, gp);
        }
    }
}
