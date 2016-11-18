using CommunicationModule;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class frm_AlertInfo : Form
    {
        public frm_AlertInfo()
        {
            InitializeComponent();
            this.lbl_DownClothState.Text = "";
        }

        private void frm_AlertInfo_Load(object sender, EventArgs e)
        {
            //timer1.Enabled = true;
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        public void SetAlertInfo(DeviceState ds)
        {
            if (ds != null)
            {
                string info = "";
                if (ds.airPressure)
                    info += "气压报警";
                if (ds.clothInductor)
                {
                    if(info.Length > 0)
                    {
                        info += "\n";
                    }
                    info += "布料感应器报警";
                }
                if (ds.openBox)
                {
                    if (info.Length > 0)
                    {
                        info += "\n";
                    }
                    info += "开盖报警";
                }
                if (ds.originInductor)
                {
                    if (info.Length > 0)
                    {
                        info += "\n";
                    }
                    info += "原点感应器报警";

                }
                if (ds.securityLightCurtains)
                {
                    if (info.Length > 0)
                    {
                        info += "\n";
                    }
                    info += "防护光幕报警";
                }
                if (ds.waterAlert)
                {
                    if (info.Length > 0)
                    {
                        info += "\n";
                    }
                    info += "水箱报警";
                }
                if (ds.xLimit)
                {
                    if (info.Length > 0)
                    {
                        info += "\n";
                    }
                    info += "X限位报警";
                }
                if (ds.yLimit)
                {
                    if (info.Length > 0)
                    {
                        info += "\n";
                    }
                    info += "Y限位报警";
                }

                if (ds.manualFeed)
                {
                    if (info.Length > 0)
                    {
                        info += "\n";
                    }
                    info += "手动报警";
                }
                if (ds.feedAlert)
                {
                    if (info.Length > 0)
                    {
                        info += "\n";
                    }
                    info += "缺料报警";
                }
                this.lbl_DownClothState.Text = info;
                //if (info.Length > 0)
                //{
                //    this.Visible = true;
                //    timer1.Enabled = true;
                //}
                //else
                //{
                //    this.Visible = false;
                //    timer1.Enabled = false;
                //}
                lbl_DownClothState.Location = new Point((this.Width - lbl_DownClothState.Width) / 2, lbl_DownClothState.Location.Y);
                this.Height = lbl_DownClothState.Location.Y + lbl_DownClothState.Height + 10;
            }
            else
            {
                this.Visible = false;
                timer1.Enabled = false;
                lbl_DownClothState.Text = "";
            }
        }

        private void frm_AlertInfo_VisibleChanged(object sender, EventArgs e)
        {
            timer1.Enabled = Visible;
        }

        private void lbl_DownClothState_TextChanged(object sender, EventArgs e)
        {

        }

        float add = -0.1f;
        private void timer1_Tick(object sender, EventArgs e)
        {
            if(this.Opacity >= 1)
            {
                this.Opacity = 1;
                add = -0.1f;
            }
            else if(this.Opacity <= 0)
            {
                this.Opacity = 0;
                add = 0.1f;
            }
            this.Opacity += add;
        }
    }
}
