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
    public partial class frm_SetLaser : frm_BaseForm
    {
        public byte laserPointLight=0;
        public short laserPointTime=-1;
        public frm_SetLaser()
        {
            InitializeComponent();
        }

        private void btn_Save_Click(object sender, EventArgs e)
        {
            laserPointLight = ConvertType.ConvertByte(this.txt_LaserPointLight.Text);
            laserPointTime = ConvertType.ConvertShort(this.txt_LaserPointTime.Text);
            if (ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
            {
                MessageBox.Show("未连接");
                return;
            }

            //14	0-100		24
            //15	100-20000		25
            List<byte> data = new List<byte>();
            bool isSuccess = false;
            bool isSuccess1 = false;
            //if (laserPointLight != ParseCommunicationCommand.laserPointLight)
            //{
                data.AddRange(ParseCommunicationCommand.GetCommandByte(ParseCommunicationCommand.sendCommandFlag, 0x12, laserPointLight, true));
                isSuccess1 = ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "激光点射光强", 120);
            //}
            //else
            //    isSuccess1 = true;
            //if (laserPointTime != ParseCommunicationCommand.laserPointTime)
            //{
                data.Clear();
                data.AddRange(ParseCommunicationCommand.GetCommandByte(ParseCommunicationCommand.sendCommandFlag, 0x13, byte.MinValue, false));
                data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(laserPointTime)));
                isSuccess = ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "激光点射延时", 120);
            //}
            //else
            //    isSuccess = true;

            ParseCommunicationCommand.ExecuteCommand("激光点射");

            if (isSuccess && isSuccess1)
            {
                ParseCommunicationCommand.laserPointLight = laserPointLight;
                ParseCommunicationCommand.laserPointTime = laserPointTime;
                //MessageBox.Show("参数保存成功");
            }
            else if (isSuccess1 && !isSuccess)
            {
                ParseCommunicationCommand.laserPointLight = laserPointLight;
                //MessageBox.Show("激光点射光强保存成功，激光点射延时保存失败");
            }
            else if (!isSuccess1 && isSuccess)
            {
                ParseCommunicationCommand.laserPointTime = laserPointTime;
                //MessageBox.Show("激光点射延时保存成功，激光点射光强保存失败");
            }
            //else
            //    MessageBox.Show("参数保存失败");LaserPointLight
            
        }

        private void btn_Exit_Click(object sender, EventArgs e)
        {
            this.Visible = false;
        }

        private void frm_SetLaser_VisibleChanged(object sender, EventArgs e)
        {
            if (Visible)
            {
                //if (laserPointLight == 0)
                //{
                //    laserPointLight = ParseCommunicationCommand.laserPointLight;
                //    List<byte> data = new List<byte>();
                //    data.AddRange(ParseCommunicationCommand.GetCommandByte(ParseCommunicationCommand.sendCommandFlag, 0x12, laserPointLight, true));
                //    ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "激光点射光强", 120);
                //}
                //if(laserPointTime == -1)
                //{
                //    laserPointTime = ParseCommunicationCommand.laserPointTime;
                //    List<byte> data = new List<byte>();
                //    data.AddRange(ParseCommunicationCommand.GetCommandByte(ParseCommunicationCommand.sendCommandFlag, 0x13, byte.MinValue, false));
                //    data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(laserPointTime)));
                //    ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "激光点射延时", 120);
                //}

                //终止工作时才能设置 20160818
                if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                {
                    foreach (Control item in this.Controls)
                    {
                        item.Enabled = true;
                    }
                }
                else
                {
                    foreach (Control item in this.Controls)
                    {
                        if (item != btn_Exit)
                            item.Enabled = false;
                    }
                }
                laserPointLight = ParseCommunicationCommand.laserPointLight;
                laserPointTime = ParseCommunicationCommand.laserPointTime;
                this.txt_LaserPointLight.Text = laserPointLight.ToString();
                this.txt_LaserPointTime.Text = laserPointTime.ToString();
            }
            else
            {
                //laserPointLight = ConvertType.ConvertByte(this.txt_LaserPointLight.Text);
                //laserPointTime = ConvertType.ConvertShort(this.txt_LaserPointLight.Text);

                //if (laserPointLight != ParseCommunicationCommand.laserPointLight)
                {
                    ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.laserPointLight.ToString(), "LaserPointLight");
                }
                //if (laserPointTime != ParseCommunicationCommand.laserPointTime)
                {
                    ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.laserPointTime.ToString(), "LaserPointTime");
                }
            }
        }

    }
}