using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GraphicModule
{
    public partial class Form2 : Form
    {
        public Form2()
        {
            InitializeComponent();
            //C:\Users\Administrator\Desktop\dxf\sample.dxf
            paintingPanel1.OpenDxfFile(@"C:\Users\Administrator\Desktop\dxf\sample.dxf");
        }
    }
}
