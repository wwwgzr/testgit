using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.IO;

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
                        //Bitmap bmp = new Bitmap(buttonImage);
                        FileStream fileStream = new FileStream(buttonImage, FileMode.Open, FileAccess.Read);
                        int byteLength = (int)fileStream.Length;
                        byte[] fileBytes = new byte[byteLength];
                        fileStream.Read(fileBytes, 0, byteLength);
                        //文件流关闭,文件解除锁定
                        fileStream.Close();
                        btn_Icon.BackgroundImage = Image.FromStream(new MemoryStream(fileBytes));
                        
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
