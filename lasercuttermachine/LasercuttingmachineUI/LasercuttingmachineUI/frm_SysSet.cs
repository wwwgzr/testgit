using CommonModule;
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
    public partial class frm_SysSet : frm_BaseForm
    {
        ReadParam newRp;
        public frm_SysSet()
        {
            InitializeComponent();
        }

        private void btn_Save_Click(object sender, EventArgs e)
        {
            if (ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
            {
                MessageBox.Show("未连接");
                return;
            }
            newRp.ipAddress = ConvertType.IpAddressToByteArray(this.ipt_Adress.IP);
            newRp.subNetMask = ConvertType.IpAddressToByteArray(this.ipt_SubNetMask.IP);
            newRp.gateWay = ConvertType.IpAddressToByteArray(this.ipt_GateWay.IP);
            //newRp.versionNum = ConvertType.IpAddressToByteArray(this.ipt_Version.IP);
            //newRp.deviceTime = ConvertType.DateTimeToDeviceTime(this.dateTimePicker1.Value);
            
            //if (ParseCommunicationCommand.readParam != null && ParseCommunicationCommand.readParam.Equals(newRp))
            //{
            //    MessageBox.Show("参数保存成功");
            //    return;
            //}
            List<byte> data = new List<byte>();
            if (ParseCommunicationCommand.sendCommandFlag != null)
                data.AddRange(ParseCommunicationCommand.sendCommandFlag);
            data.AddRange(newRp.GetByteData());

            bool isSuccess = ParseCommunicationCommand.ExecuteSetParamCommand(data.ToArray(), "设置参数", 120);

            //bool isSuccess = ParseCommunicationCommand.ExecuteSetParamCommand(newRp.GetByteData(), "设置参数", 120);
            if (isSuccess)
            {
                MessageBox.Show("参数保存成功");
                ParseCommunicationCommand.readParam = (ReadParam)newRp.Clone();
            }
            else
                MessageBox.Show("参数保存失败");
        }

        private void btn_Exit_Click(object sender, EventArgs e)
        {
            this.Visible = false;
        }

        private void frm_SysSet_VisibleChanged(object sender, EventArgs e)
        {
            if (Visible)
            {
                if (ParseCommunicationCommand.readParam != null && ParseCommunicationCommand.readParam.isInital)
                    newRp = (ReadParam)ParseCommunicationCommand.readParam.Clone();
                else
                    newRp = new ReadParam();
                //终止工作时才能设置 20160818
                if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                {
                    foreach (Control item in this.Controls)
                    {
                        if (item != btn_Exit && item != txt_Version && item != dateTimePicker1)
                            item.Enabled = true;
                    }
                }
                else
                {
                    foreach (Control item in this.Controls)
                    {
                        if (item != btn_Exit && item != txt_Version && item != dateTimePicker1)
                            item.Enabled = false;
                    }
                }
                InitialControl(newRp);
            }
        }

        public void InitialControl(ReadParam rp)
        {
            if (rp == null && !rp.isInital)
                return;
            this.ipt_Adress.IP = ConvertType.ToIpAddress(rp.ipAddress);
            this.ipt_SubNetMask.IP = ConvertType.ToIpAddress(rp.subNetMask);
            this.ipt_GateWay.IP = ConvertType.ToIpAddress(rp.gateWay);
            this.txt_Version.Text = ConvertType.ToVersion(rp.versionNum);
            try
            {
                this.dateTimePicker1.Value = ConvertType.DeviceTimeToDateTime(rp.deviceTime);
            }
            catch (Exception ex)
            {
                this.dateTimePicker1.Value = DateTime.Now;
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "InitialControl\r\n");
            }
        }
    }
}
