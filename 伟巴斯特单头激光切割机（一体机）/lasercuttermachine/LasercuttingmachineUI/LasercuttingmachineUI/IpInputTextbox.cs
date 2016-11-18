﻿using System;
using System.Windows.Forms;
using System.Net;

namespace LasercuttingmachineUI
{
    public partial class IpInputTextbox : UserControl
    {
        public bool isPanel = false;
        /// <summary>
        /// Ip地址
        /// </summary>
        public string IP
        {
            get
            {
                return this.ToString();
            }
            set
            {
                string ipStr = value;
                if (string.IsNullOrEmpty(ipStr))
                {
                    Ip1.Text = "";
                    Ip2.Text = "";
                    Ip3.Text = "";
                    Ip4.Text = "";
                    ip = "";
                }
                else
                {
                    try
                    {
                        IPAddress ipValue;
                        if(IPAddress.TryParse(ipStr,out ipValue))
                        {
                            string[] ips = ipStr.Split('.');
                            Ip1.Text = ips[0];
                            Ip2.Text = ips[1];
                            Ip3.Text = ips[2];
                            Ip4.Text = ips[3];
                            ip = ipStr;
                        }
                    }
                    catch(Exception ex)
                    {
                        CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "\r\n");
                        Ip1.Text = "";
                        Ip2.Text = "";
                        Ip3.Text = "";
                        Ip4.Text = "";
                        ip = "";
                    }
                }
            }
        }
        private string ip;

        public IpInputTextbox()
        {
            InitializeComponent();
            this.Ip1.Index = 1;
            this.Ip2.Index = 2;
            this.Ip3.Index = 3;
            this.Ip4.Index = 4;

            this.Ip1.OnPressBackspace += new NumberTextBoxExt.PressBackspaceHandle(Ip_OnPressBackspace);
            this.Ip2.OnPressBackspace += new NumberTextBoxExt.PressBackspaceHandle(Ip_OnPressBackspace);
            this.Ip3.OnPressBackspace += new NumberTextBoxExt.PressBackspaceHandle(Ip_OnPressBackspace);
            this.Ip4.OnPressBackspace += new NumberTextBoxExt.PressBackspaceHandle(Ip_OnPressBackspace);

            if (!isPanel)
            {
                this.Ip1.MouseClick += Ip_MouseClick;
                this.Ip2.MouseClick += Ip_MouseClick;
                this.Ip3.MouseClick += Ip_MouseClick;
                this.Ip4.MouseClick += Ip_MouseClick;
            }
        }

        void Ip_MouseClick(object sender, MouseEventArgs e)
        {
            MenuController.ShowPanel(true, null, this);
        }

       

        private void Ip_OnPressBackspace(int index)
        {
            switch (index)
            {
                case 4:
                    Ip3.Focus();
                    Ip3.SelectionStart = Ip3.Text.Length;
                    break;
                case 3:
                    Ip2.Focus();
                    Ip2.SelectionStart = Ip2.Text.Length;
                    break;
                case 2:
                    Ip1.Focus();
                    Ip1.SelectionStart = Ip1.Text.Length;
                    break;
                default:
                    break;
            }
        }

        private void Ip1_TextChanged(object sender, EventArgs e)
        {
            if (Ip1.Text.Length == 3 && Ip1.Text.Length > 0 && Ip1.SelectionLength == 0)
            {
                Ip2.Focus();
                Ip2.Select(0, Ip2.Text.Length);
            }
        }

        private void Ip2_TextChanged(object sender, EventArgs e)
        {
            if (Ip2.Text.Length == 3 && Ip2.Text.Length > 0 && Ip2.SelectionLength == 0)
            {
                Ip3.Focus();
                Ip3.Select(0, Ip3.Text.Length);
            }
        }

        private void Ip3_TextChanged(object sender, EventArgs e)
        {
            if (Ip3.Text.Length == 3 && Ip3.Text.Length > 0 && Ip3.SelectionLength == 0)
            {
                Ip4.Focus();
                Ip4.Select(0, Ip4.Text.Length);
            }
        }

        /// <summary>
        /// ToString重写
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            string Ipstr = Ip1.Text + "." + Ip2.Text + "." + Ip3.Text + "." + Ip4.Text;
            try
            {
                IPAddress.Parse(Ipstr);
            }
            catch
            {
                return "IP地址格式不正确";
            }
            this.ip = Ipstr;
            return this.ip;
        }

        private void Ip1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if ((Keys)e.KeyChar == Keys.Tab && Ip1.Text.Length > 0 && Ip1.SelectionLength == 0)
            {
                Ip2.Focus();
                Ip2.Select(0, Ip2.Text.Length);
            }
        }

        private void Ip2_KeyPress(object sender, KeyPressEventArgs e)
        {
            if ((Keys)e.KeyChar == Keys.Tab && Ip2.Text.Length > 0 && Ip2.SelectionLength == 0)
            {
                Ip3.Focus();
                Ip3.Select(0, Ip3.Text.Length);
            }
        }

        private void Ip3_KeyPress(object sender, KeyPressEventArgs e)
        {
            if ((Keys)e.KeyChar == Keys.Tab && Ip3.Text.Length > 0 && Ip3.SelectionLength == 0)
            {
                Ip4.Focus();
                Ip4.Select(0, Ip4.Text.Length);
            }
        }

    }
}
