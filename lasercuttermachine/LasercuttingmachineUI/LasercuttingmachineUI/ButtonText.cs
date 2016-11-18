using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class ButtonText : UserControl
    {
        string buttonImage = "";
        public string ButtonImage
        {
            get { return buttonImage; }
            set
            {
                buttonImage = value;
                if (buttonImage.Length > 0)
                {
                    try
                    {
                        Bitmap bmp = new Bitmap(buttonImage);
                        btn_Icon.BackgroundImage = bmp;
                    }
                    catch (Exception ex)
                    {

                    }
                }
            }
        }

        public string ButtonText1
        {
            get { return lbl_Text.Text; }
            set { lbl_Text.Text = value; }
        }
        public ButtonText()
        {
            InitializeComponent();
        }

        private void btn_Icon_Click(object sender, EventArgs e)
        {
            
        }

        private void lbl_Text_Click(object sender, EventArgs e)
        {

        }
    }
}
