using CommunicationModule;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Text;
using System.Web.Security;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class frm_Load : Form
    {
        public string LabelTxt
        {
            get { return this.label2.Text; }
            set { this.label2.Text = value; }
        }

        public bool isInitial = true;
        Process p;
        public frm_Load()
        {
            InitializeComponent();
            AppSettingsReader reader = new AppSettingsReader();
            string com = reader.GetValue("user", typeof(string)).ToString().Trim();
            string[] fileinfo = com.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
            ParseCommunicationCommand.userInfo = new Dictionary<string, string>();
            foreach (string item in fileinfo)
            {
                string[] str = item.Split(new char[] { ':' }, StringSplitOptions.RemoveEmptyEntries);
                if (str.Length == 2)
                {
                    ParseCommunicationCommand.userInfo.Add(str[0], str[1]);
                }
                else if (str.Length == 1)
                    ParseCommunicationCommand.userInfo.Add(str[0], "");
            }
            this.VisibleChanged += frm_Load_VisibleChanged;
            ParseCommunicationCommand.WriteFileCommand(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "程序启动" + "\r\n");

//            System.Resources.ResourceWriter writer =

//new System.Resources.ResourceWriter("dongpad.com.resources");

//            writer.AddResource("bitmap", Image.FromFile(@"dongpad.bitmap"));

//            writer.AddResource("cursor", Image.FromFile(@"dongpad.cursor"));

//            writer.AddResource("ico", Image.FromFile(@"dongpad.ico"));

//            writer.AddResource("str", Image.FromFile(@"dongpad.str"));

//            //在这里资源的名称记得要唯一。

//            writer.Generate();

//            writer.Close();
        }

        void frm_Load_VisibleChanged(object sender, EventArgs e)
        {
            //if (ParseCommunicationCommand.userType == 1 && !isInitial)
            //{ 
            //    //管理员
            //    panel1.Visible = false;
            //}
            //else if (ParseCommunicationCommand.userType == 1)
            //{
            //    panel1.Visible = false;
            //}
            //else
            //    panel1.Visible = true;
            if (!isInitial)
                panel1.Visible = false;
            if (Visible)
            {
                this.txt_Password.Text = "";
                this.txt_Password.Focus();
                isInitial = false;
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            Process[] pres = Process.GetProcessesByName("osk");
            foreach (Process item in pres)
            {
                item.Kill();
            }
            this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
        }

        private void btn_OK_Click(object sender, EventArgs e)
        {
            if (ParseCommunicationCommand.userInfo != null)
            {
                if (ParseCommunicationCommand.userType == 1)
                    ParseCommunicationCommand.userType = 2;
                else
                    ParseCommunicationCommand.userType = 1;



                string pwd = "";
                try
                {
                    pwd = FormsAuthentication.HashPasswordForStoringInConfigFile(this.txt_Password.Text, "md5");
                }
                catch (Exception ex)
                {
                    pwd = ParseCommunicationCommand.userInfo["admin"];
                    CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "Load_Ok\r\n");
                }
                //if (rab_Guest.Checked)
                //{
                //    if (ParseCommunicationCommand.userInfo["guest"] == pwd)
                //    {
                //        ParseCommunicationCommand.userType = 2;
                //        this.lbl_tip.Text = "";
                //        this.DialogResult = System.Windows.Forms.DialogResult.OK;
                //        Process[] pres = Process.GetProcessesByName("osk");
                //        foreach (Process item in pres)
                //        {
                //            item.Kill();
                //        }
                //    }
                //    else
                //    {
                //        //this.lbl_tip.Text = "密码错误！";
                //        ShowTooltip("提示", "密码错误！", txt_Password);
                //        this.txt_Password.SelectAll();
                //        this.txt_Password.Focus();
                //    }
                //}
                //else 
                if (rad_Admin.Checked)
                {
                    if (ParseCommunicationCommand.userInfo["admin"] == pwd)
                    {
                        this.lbl_tip.Text = "";
                        ParseCommunicationCommand.userType = 1;
                        
                        this.DialogResult = System.Windows.Forms.DialogResult.OK;
                        Process[] pres = Process.GetProcessesByName("osk");
                        foreach (Process item in pres)
                        {
                            item.Kill();
                        }
                    }
                    else
                    {
                        ShowTooltip("提示", "密码错误！", txt_Password);
                        //this.lbl_tip.Text = "密码错误！";
                        this.txt_Password.SelectAll();
                        this.txt_Password.Focus();
                    }
                }
            }
            this.txt_Password.Focus();
        }

        private void txt_Password_MouseClick(object sender, MouseEventArgs e)
        {
            if (p == null)
            {
                //p = Process.Start("C:\\Windows\\System32\\osk.exe");
                p = Process.Start(Application.StartupPath + "\\osk.exe");

            }
            Process[] pres = Process.GetProcessesByName("osk");
            if (pres.Length == 0)
                p.Start();
            

            this.txt_Password.Focus();
        }

        private void rab_Guest_Click(object sender, EventArgs e)
        {
            this.txt_Password.Focus();
        }

        private void ShowTooltip(string title, string content, TextBox txt)
        {
            this.toolTip1.ToolTipTitle = title;
            this.toolTip1.IsBalloon = false;
            this.toolTip1.UseFading = true;
            this.toolTip1.Show(content, txt);
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {

        }
    }
}
