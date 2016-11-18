using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Management;
using System.Net;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Form3 : Form
    {
        public Form3()
        {
            InitializeComponent();
            IPHostEntry myEntry = System.Net.Dns.GetHostEntry(System.Net.Dns.GetHostName());
//                System.Net.IPHostEntry myEntry = System.Net.Dns.GetHostEntry;(System.Net.Dns.GetHostName());
//string ipAddress = myEntry.AddressList[0].ToString(); 
//            (System.Net.Dns.GetHostName());
            string ipAddress = myEntry.AddressList[0].ToString();

            IPHostEntry ipe = Dns.GetHostEntry(Dns.GetHostName());
            IPAddress ipa = ipe.AddressList[0];
//连接特定的 DNS 后缀: 
//描述: Dell 无线 1704 802.11b/g/n (2.4GHz)
//物理地址: ‎18-4F-32-FB-F1-B5
//已启用 DHCP: 是
//IPv4 地址: 192.168.1.101
//IPv4 子网掩码: 255.255.255.0
//获得租约的时间: 2016年6月23日 星期四 9:32:21
//租约过期的时间: 2016年6月23日 星期四 13:32:21
//IPv4 默认网关: 192.168.1.1
//IPv4 DHCP 服务器: 192.168.1.1
//IPv4 DNS 服务器: 192.168.2.1, 8.8.8.8
//IPv4 WINS 服务器: 
//已启用 NetBIOS over Tcpip: 是
//连接-本地 IPv6 地址: fe80::a086:cb4d:fd29:df33%11
//IPv6 默认网关: 
//IPv6 DNS 服务器: 
            ManagementClass mc = new ManagementClass("Win32_NetworkAdapterConfiguration");
            ManagementObjectCollection nics = mc.GetInstances();
            foreach (ManagementObject nic in nics)
            {
                if (Convert.ToBoolean(nic["ipEnabled"]) == true)
                {
                    MessageBox.Show((nic["IPAddress"] as String[])[0]);
                    MessageBox.Show((nic["IPSubnet"] as String[])[0]);
                    MessageBox.Show((nic["DefaultIPGateway"] as String[])[0]);
                }
            } 
        }


        private void button1_Click(object sender, EventArgs e)
        {
            ManagementBaseObject inPar = null;
            ManagementBaseObject outPar = null;
            ManagementClass mc = new ManagementClass("Win32_NetworkAdapterConfiguration");
            ManagementObjectCollection moc = mc.GetInstances();
            foreach (ManagementObject mo in moc)
            {
                if (!(bool)mo["IPEnabled"])
                    continue;
                //设置ip地址和子网掩码
                inPar = mo.GetMethodParameters("EnableStatic");
                string ip = "";
                //ip = numericUpDown1.Value.ToString() + "." + numericUpDown2.Value.ToString() + "." + numericUpDown3.Value.ToString() + "." + numericUpDown4.Value.ToString();
                ip = "";
                inPar["IPAddress"] = new string[] { ip };// 1.备用 2.IP

                string ym = "";
                //ym = numericUpDown8.Value.ToString() + "." + numericUpDown7.Value.ToString() + "." + numericUpDown6.Value.ToString() + "." + numericUpDown5.Value.ToString();
                ym = "";
                inPar["SubnetMask"] = new string[] { ym };
                outPar = mo.InvokeMethod("EnableStatic", inPar, null);
                //设置网关地址
                inPar = mo.GetMethodParameters("SetGateways");
                string wg = "";
                //wg = numericUpDown12.Value.ToString() + "." + numericUpDown11.Value.ToString() + "." + numericUpDown10.Value.ToString() + "." + numericUpDown9.Value.ToString();
                wg = "";
                inPar["DefaultIPGateway"] = new string[] { wg }; // 1.网关;2.备用网关
                outPar = mo.InvokeMethod("SetGateways", inPar, null);
                //设置DNS
                inPar = mo.GetMethodParameters("SetDNSServerSearchOrder");
                string dns1 = "";
                string dns2 = "";
                inPar["DNSServerSearchOrder"] = new string[] { dns1, dns2 }; // 1.DNS 2.备用DNS
                outPar = mo.InvokeMethod("SetDNSServerSearchOrder", inPar, null);
                break;
            }
        }
        private void button2_Click(object sender, EventArgs e)
        {
            ManagementClass wmi = new ManagementClass("Win32_NetworkAdapterConfiguration");
            ManagementObjectCollection moc = wmi.GetInstances();
            foreach (ManagementObject mo in moc)
            {
                //如果没有启用IP设置的网络设备则跳过
                if (!(bool)mo["IPEnabled"])
                    continue;
                //重置DNS为空
                mo.InvokeMethod("SetDNSServerSearchOrder", null);
                //开启DHCP
                mo.InvokeMethod("EnableDHCP", null);
            }
        }
        private void button3_Click(object sender, EventArgs e)
        {
            this.Close();
            this.Dispose();
        }
        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.F2:
                    button1_Click(sender, e);
                    break;
                case Keys.F3:
                    button2_Click(sender, e);
                    break;
            }
        }
    }
}
