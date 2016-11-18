using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class Rich_TextBox : TextBox
    {
        private double minValue;
        public bool isPanel = false;
        public double MinValue
        {
            get { return minValue; }
            set { minValue = value; }
        }
        private double maxValue;

        public double MaxValue
        {
            get { return maxValue; }
            set { maxValue = value; }
        }
        public Rich_TextBox()
        {
            InitializeComponent();
            this.Validating += Rich_TextBox_Validating;
            //this.GotFocus += Rich_TextBox_GotFocus;
            //this.LostFocus += Rich_TextBox_LostFocus;
            if (!isPanel)
                this.MouseClick += Rich_TextBox_MouseClick;
        }

       

        void Rich_TextBox_MouseClick(object sender, MouseEventArgs e)
        {
            MenuController.ShowPanel(true, this,null);
        }

        void Rich_TextBox_LostFocus(object sender, EventArgs e)
        {
            //MenuController.ShowPanel(false, this);
        }

        void Rich_TextBox_GotFocus(object sender, EventArgs e)
        {
            //MenuController.ShowPanel(true, this);
            //MenuController.operatPanel.focusForm = this.FindForm();
        }
        

        protected override void OnGotFocus(EventArgs e)
        {
            base.OnGotFocus(e);
        }
        private void Rich_TextBox_Validating(object sender, CancelEventArgs e)
        {
            if (this.Visible)
            {
                string str = "0" + this.Text.Trim();
                double value;
                if (double.TryParse(str, out value))
                {
                    if (value > maxValue || value < minValue)
                    {
                        this.Focus();
                        this.SelectAll();
                        MessageBox.Show("请输入" + minValue + "至" + maxValue + "的数字");
                    }
                    this.Text = value.ToString();
                }
                else
                {
                    this.Focus();
                    this.SelectAll();
                    MessageBox.Show("请输入数字");
                }
            }
        }
    }
}
