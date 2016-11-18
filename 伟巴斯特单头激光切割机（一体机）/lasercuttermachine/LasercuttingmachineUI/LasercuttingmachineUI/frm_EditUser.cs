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
    public partial class frm_EditUser : frm_BaseForm
    {
        string currentUser;
        bool oldPass = false;
        bool newPass = false;
        bool newPass1 = false;
        Process p;
        public Form loadform;
        //string currentPassword;
        public frm_EditUser()
        {
            InitializeComponent();
            //AppSettingsReader reader = new AppSettingsReader();
            //string com = reader.GetValue("user", typeof(string)).ToString().Trim();
            //string[] fileinfo = com.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
            //userInfo = new Dictionary<string, string>();
            //foreach (string item in fileinfo)
            //{
            //    string[] str = item.Split(new char[] { ':' }, StringSplitOptions.RemoveEmptyEntries);
            //    if (str.Length == 2)
            //    {
            //        userInfo.Add(str[0], str[1]);
            //    }
            //    else if (str.Length == 1)
            //        userInfo.Add(str[0], "");
            //}
            currentUser = "admin";
        }

        private void btn_Save_Click(object sender, EventArgs e)
        {
            txt_OldPassword_Validated(null, null);
            if (oldPass)
            {
                txt_NewPassword_Validated(null, null);
                if(newPass)
                {
                    txt_Password_Validated(null, null);
                }
            }
            if(oldPass && newPass && newPass1)
            {
                string pwd = FormsAuthentication.HashPasswordForStoringInConfigFile(this.txt_Password.Text, "md5");
                ParseCommunicationCommand.userInfo[currentUser] = pwd;
                string value = "";
                foreach (string item in ParseCommunicationCommand.userInfo.Keys)
                {
                    value += item + ":" + ParseCommunicationCommand.userInfo[item] + ";";
                }
                ParseCommunicationCommand.SaveConfig(value, "user");
                this.DialogResult = System.Windows.Forms.DialogResult.OK;
                MessageBox.Show(this,"密码修改成功！","提示");
                this.Visible = false;
                //if(loadform != null)
                //{
                //    if(loadform.ShowDialog() == System.Windows.Forms.DialogResult.Cancel)
                //    {
                //        this.Owner.Close();
                //    }
                //}
            }
        }

        private void btn_Exit_Click(object sender, EventArgs e)
        {
            this.Visible = false;
        }

        //private void txt_OldPassword_Validating(object sender, CancelEventArgs e)
        private void txt_OldPassword_Validated(object sender, EventArgs e)
        {
            oldPass = false;
            if (!Visible)
                return;
            if (txt_OldPassword.Text== "")
            {
                ShowTooltip("提示", "请输入管理员密码！", txt_OldPassword);

                //MessageBox.Show(this, "请输入管理员密码！", "提示");
                this.txt_OldPassword.SelectAll();
                this.txt_OldPassword.Focus();
                return;
            }
            if (ParseCommunicationCommand.userInfo != null)
            {
                string pwd = FormsAuthentication.HashPasswordForStoringInConfigFile(this.txt_OldPassword.Text, "md5");
                if (ParseCommunicationCommand.userInfo["admin"] != pwd)
                {
                    ShowTooltip("提示", "管理员密码错误！", txt_OldPassword);
                    this.txt_OldPassword.SelectAll();
                    this.txt_OldPassword.Focus();
                    //this.lbl_tip.Text = "密码错误！";
                }
                else
                {
                    oldPass = true;
                }
            }
        }

        private void ShowTooltip(string title,string content,TextBox txt)
        {
            this.toolTip1.ToolTipTitle = title;
            this.toolTip1.IsBalloon = false;
            this.toolTip1.UseFading = true;
            this.toolTip1.Show(content, txt);
        }
        
        //private void txt_Password_Validating(object sender, CancelEventArgs e)
        private void txt_Password_Validated(object sender, EventArgs e)
        {
            newPass1 = false;
            if (!Visible)
                return;
            if (!oldPass)
            {
                this.txt_OldPassword.SelectAll();
                this.txt_OldPassword.Focus();
                return;
            }
            if (!newPass)
            {
                this.txt_NewPassword.SelectAll();
                this.txt_NewPassword.Focus();
                return;
            }
            if (this.txt_Password.Text == "")
                return;
            if (this.txt_NewPassword.Text != this.txt_Password.Text)
            {
                ShowTooltip("提示", "两次输入的密码不一致！", txt_Password);
                this.txt_Password.Text = "";
                this.txt_Password.Focus();
            }
            else
            {
                newPass1 = true;
            }

        }

        private void frm_EditUser_VisibleChanged(object sender, EventArgs e)
        {
            if(Visible)
            {
                //if (ParseCommunicationCommand.userType == 1)
                //{
                //    currentUser = "admin";
                //}
                //else if (ParseCommunicationCommand.userType == 2)
                //{
                //    currentUser = "guest";
                //}
                //this.label1.Focus();
                //this.txt_OldPassword.SelectAll();
                this.txt_OldPassword.Focus();
            }
            else
            {
                this.txt_Password.Text = "";
                this.txt_OldPassword.Text = "";
                this.txt_NewPassword.Text = "";
                Process[] pres = Process.GetProcessesByName("osk");
                foreach (Process item in pres)
                {
                    item.Kill();
                }
                //this.label1.Focus();
                this.txt_OldPassword.Focus();
            }
        }
        //private void txt_NewPassword_Validating(object sender, CancelEventArgs e)
        private void txt_NewPassword_Validated(object sender, EventArgs e)
        {
            newPass = false;
            if (!Visible)
                return;
            if(!oldPass)
            {
                this.txt_OldPassword.SelectAll();
                this.txt_OldPassword.Focus();
                return;
            }
            if (this.txt_NewPassword.Text == "")
            {
                ShowTooltip("提示", "新密码不能为空！", txt_NewPassword);
                this.txt_NewPassword.SelectAll();
                this.txt_NewPassword.Focus();
                return;
            }
            string pwd = FormsAuthentication.HashPasswordForStoringInConfigFile(this.txt_NewPassword.Text, "md5");

            if (pwd == ParseCommunicationCommand.userInfo[currentUser])
            {
                ShowTooltip("提示", "新旧密码一样，请重新输入！", txt_NewPassword);
                this.txt_NewPassword.SelectAll();
                this.txt_NewPassword.Focus();
            }
            else
            {
                newPass = true;
            }

        }

        private void txt_Password_MouseClick(object sender, MouseEventArgs e)
        {
            if (p == null)
            {
                p = Process.Start(Application.StartupPath + "\\osk.exe");
            }
            Process[] pres = Process.GetProcessesByName("osk");
            if (pres.Length == 0)
                p.Start();
            TextBox txt = (TextBox)sender;
            txt.Focus();
        }

        private void btn_Reset_Click(object sender, EventArgs e)
        {
            if(currentUser == "guest")
            {
                txt_OldPassword_Validated(null, null);
                if (!oldPass)
                    return;
            }
            if(MessageBox.Show("确定需要重置密码吗？", "提示", MessageBoxButtons.YesNo) == System.Windows.Forms.DialogResult.Yes)
            {
                string pwd = FormsAuthentication.HashPasswordForStoringInConfigFile("000000", "md5");
                ParseCommunicationCommand.userInfo[currentUser] = pwd;
                string value = "";
                foreach (string item in ParseCommunicationCommand.userInfo.Keys)
                {
                    value += item + ":" + ParseCommunicationCommand.userInfo[item] + ";";
                }
                ParseCommunicationCommand.SaveConfig(value, "user");
                this.DialogResult = System.Windows.Forms.DialogResult.OK;
                MessageBox.Show(this, "密码已重置为000000！", "提示");
                this.Visible = false;
                //if (loadform != null)
                //{
                //    if (loadform.ShowDialog() == System.Windows.Forms.DialogResult.Cancel)
                //    {
                //        this.Owner.Close();
                //    }
                //}
            }
        }

        private void rab_Guest_CheckedChanged(object sender, EventArgs e)
        {
            if (rab_Guest.Checked)
            {
                currentUser = "guest";
                this.txt_OldPassword.Focus();
            }
        }

        private void rab_Admin_CheckedChanged(object sender, EventArgs e)
        {
            if (rab_Admin.Checked)
            {
                currentUser = "admin";
                this.txt_OldPassword.Focus();
            }
        }

        private void txt_OldPassword_TextChanged(object sender, EventArgs e)
        {
            this.toolTip1.Hide(this);
        }
    }
}
