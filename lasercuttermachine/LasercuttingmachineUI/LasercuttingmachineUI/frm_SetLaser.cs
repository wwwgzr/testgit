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
        public byte laserPointLightB = 0;
        public short laserPointTimeB = -1;
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

            List<byte> data = new List<byte>();
            bool isSuccess = false;

            data.AddRange(ParseCommunicationCommand.GetCommandByte(ParseCommunicationCommand.sendCommandFlag, 0x11, byte.MinValue, false));
            if(rab_LaserA.Checked)
            {
                data.Add(((byte)0x01));
            }
            else
                data.Add(((byte)0x02));
            data.Add(laserPointLight);
            data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(laserPointTime)));
            isSuccess = ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "激光点射测试", 120);
            if (isSuccess)
            {
                if(rab_LaserA.Checked)
                {
                    ParseCommunicationCommand.laserPointLight = laserPointLight;
                    ParseCommunicationCommand.laserPointTime = laserPointTime;
                }
                else
                {
                    ParseCommunicationCommand.laserPointLightB = laserPointLight;
                    ParseCommunicationCommand.laserPointTimeB = laserPointTime;
                }
                
            }

            //14	0-100		24
            //15	100-20000		25
            //List<byte> data = new List<byte>();
            //bool isSuccess = false;
            //bool isSuccess1 = false;
            
            //data.AddRange(ParseCommunicationCommand.GetCommandByte(ParseCommunicationCommand.sendCommandFlag, 0x12, laserPointLight, true));
            //isSuccess1 = ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "激光点射光强", 120);
            
            //data.Clear();
            //data.AddRange(ParseCommunicationCommand.GetCommandByte(ParseCommunicationCommand.sendCommandFlag, 0x13, byte.MinValue, false));
            //data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(laserPointTime)));
            //isSuccess = ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "激光点射延时", 120);


            //ParseCommunicationCommand.ExecuteCommand("激光点射");

            //if (isSuccess && isSuccess1)
            //{
            //    ParseCommunicationCommand.laserPointLight = laserPointLight;
            //    ParseCommunicationCommand.laserPointTime = laserPointTime;
            //}
            //else if (isSuccess1 && !isSuccess)
            //{
            //    ParseCommunicationCommand.laserPointLight = laserPointLight;
            //}
            //else if (!isSuccess1 && isSuccess)
            //{
            //    ParseCommunicationCommand.laserPointTime = laserPointTime;
            //    //MessageBox.Show("激光点射延时保存成功，激光点射光强保存失败");
            //}
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
                if(rab_LaserA.Checked)
                {
                    laserPointLight = ParseCommunicationCommand.laserPointLight;
                    laserPointTime = ParseCommunicationCommand.laserPointTime;
                }
                else
                {
                    laserPointLight = ParseCommunicationCommand.laserPointLightB;
                    laserPointTime = ParseCommunicationCommand.laserPointTimeB;
                }
                
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
                    ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.laserPointLightB.ToString(), "LaserPointLightB");
                }
                //if (laserPointTime != ParseCommunicationCommand.laserPointTime)
                {
                    ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.laserPointTime.ToString(), "LaserPointTime");
                    ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.laserPointTimeB.ToString(), "LaserPointTimeB");
                }
            }
        }

        /// <summary>
        /// 激光头A
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void rab_LaserA_CheckedChanged(object sender, EventArgs e)
        {
            if (rab_LaserA.Checked)
            {
                laserPointLight = ParseCommunicationCommand.laserPointLight;
                laserPointTime = ParseCommunicationCommand.laserPointTime;
            }
            else
            {
                laserPointLight = ParseCommunicationCommand.laserPointLightB;
                laserPointTime = ParseCommunicationCommand.laserPointTimeB;
            }

            this.txt_LaserPointLight.Text = laserPointLight.ToString();
            this.txt_LaserPointTime.Text = laserPointTime.ToString();
        }
        /// <summary>
        /// 激光头B
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void rab_LaserB_CheckedChanged(object sender, EventArgs e)
        {
            if (rab_LaserA.Checked)
            {
                laserPointLight = ParseCommunicationCommand.laserPointLight;
                laserPointTime = ParseCommunicationCommand.laserPointTime;
            }
            else
            {
                laserPointLight = ParseCommunicationCommand.laserPointLightB;
                laserPointTime = ParseCommunicationCommand.laserPointTimeB;
            }

            this.txt_LaserPointLight.Text = laserPointLight.ToString();
            this.txt_LaserPointTime.Text = laserPointTime.ToString();
        }

    }
}