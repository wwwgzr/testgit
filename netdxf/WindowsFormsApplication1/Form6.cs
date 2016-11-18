using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Form6 : Form
    {
        public Form6()
        {
            InitializeComponent();
            InitializeComponent();
            this.panel1 = new System.Windows.Forms.Panel();
            this.SuspendLayout();
            //  
            // panel1 
            //  
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(292, 273);
            this.panel1.TabIndex = 0;
            this.Controls.Add(this.panel1);
            Load += new EventHandler(Form1_Load);
        }

        private System.Windows.Forms.Panel panel1;

        private void Form1_Load(object sender, EventArgs e)
        {
            
        }
    }
}
