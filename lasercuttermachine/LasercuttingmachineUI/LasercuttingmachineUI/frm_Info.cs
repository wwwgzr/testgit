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
    public partial class frm_Info : Form
    {
        public frm_Info()
        {
            //IsCanMove = true;
            InitializeComponent();
            //lbl_DownClothState.Text = "";
            lbl_FileName.Text = "";
        }

        public string lbl_FileXText
        {
            set
            {
                this.lbl_FileX.Text = "文件X轴宽:" + value;
            }
        }

        public string lbl_FileYText
        {
            set
            {
                this.lbl_FileY.Text = "文件Y轴宽:" + value;
            }
        }
        public string lbl_CutNumText
        {
            set
            {
                this.lbl_CutNum.Text = "完成数量:" + value;
            }
        }

        public string lbl_WorkTimeText
        {
            set
            {
                this.lbl_WorkTime.Text = "工作时间:" + value;
            }
        }

        public string lbl_ConnectStateText
        {
            set
            {
                this.lbl_ConnectState.Text = "连接状态:" + value;
            }
        }

        public string lbl_WorkStateText
        {
            set
            {
                this.lbl_WorkState.Text = "设备状态:" + value;
            }
        }

        public string lbl_FileNameText
        {
            set
            {
                this.lbl_FileName.Text = value;
            }
        }

        public string lbl_DownClothStateText
        {
            set
            {
                this.lbl_DownClothState.Text = value;
            }
        }


        public string lbl_AutoText
        {
            set
            {
                this.lbl_Auto.Text = value + "状态";
            }
        }


        private void frm_Info_Load(object sender, EventArgs e)
        {

        }
    }
}