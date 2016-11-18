using CommonModule;
using CommunicationModule;
using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class frm_SetOrigin : frm_BaseForm
    {
        float x = 0;
        float y = 0;
        bool isChange = false;
        SetFormControlValue setFormControlValue1;
        public frm_AddFile addCutFileForm;
        public string path;
        public frm_SetOrigin()
        {
            InitializeComponent();
            setFormControlValue1 = new SetFormControlValue(SetFormControlValue1);
        }

        ////定位点设置 04 上 0x36
        //   writeDataList.Add("定位点设置-上", GetCommandByte(commandFlag, 0x04, 0x36, true));
        //   writeDataList.Add("定位点设置-下", GetCommandByte(commandFlag, 0x04, 0x37, true));
        //   writeDataList.Add("定位点设置-左", GetCommandByte(commandFlag, 0x04, 0x38, true));
        //   writeDataList.Add("定位点设置-右", GetCommandByte(commandFlag, 0x04, 0x39, true));
        /// <summary>
        /// 上
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_Up_Click(object sender, EventArgs e)
        {
            if (ParseCommunicationCommand.IsReading)
                return;
            SetButtonEnable(false);
            if (this.nud_CroodX.Value == nud_CroodX.Maximum)
            {

            }
            else if (this.nud_CroodX.Value + this.nud_CroodX.Increment > nud_CroodX.Maximum)
            {
                if(ParseCommunicationCommand.ExecuteCommand("定位点设置-上"))
                {
                    this.nud_CroodX.Value = nud_CroodX.Maximum;
                }
            }
            else
            {
                if (ParseCommunicationCommand.ExecuteCommand("定位点设置-上"))
                {
                    this.nud_CroodX.Value += this.nud_CroodX.Increment;
                }
            }
            //ParseCommunicationCommand.ExecuteCommand("定位点设置-左");
             SetButtonEnable(true);
        }
        /// <summary>
        /// 左
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_Left_Click(object sender, EventArgs e)
        {
            if (ParseCommunicationCommand.IsReading)
                return;
            SetButtonEnable(false);
            if (this.nud_CroodY.Value == nud_CroodY.Maximum)
            {

            }
            else if (this.nud_CroodY.Value + this.nud_CroodY.Increment > nud_CroodY.Maximum)
            {
                if (ParseCommunicationCommand.ExecuteCommand("定位点设置-左"))
                {
                    this.nud_CroodY.Value = this.nud_CroodY.Maximum;
                }
            }
            else
            {
                if (ParseCommunicationCommand.ExecuteCommand("定位点设置-左"))
                {
                    this.nud_CroodY.Value += this.nud_CroodY.Increment;
                }
            }
            //ParseCommunicationCommand.ExecuteCommand("定位点设置-上");
            SetButtonEnable(true);
        }

        /// <summary>
        /// 下
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_Down_Click(object sender, EventArgs e)
        {
            if (ParseCommunicationCommand.IsReading)
                return;
            SetButtonEnable(false);
            if (this.nud_CroodX.Value == nud_CroodX.Minimum)
            {

            }
            else if (this.nud_CroodX.Value - this.nud_CroodX.Increment < nud_CroodX.Minimum)
            {
                if(ParseCommunicationCommand.ExecuteCommand("定位点设置-下"))
                {
                    this.nud_CroodX.Value = this.nud_CroodX.Minimum;
                }
            }
            else
            {
                if (ParseCommunicationCommand.ExecuteCommand("定位点设置-下"))
                {
                    this.nud_CroodX.Value -= this.nud_CroodX.Increment;
                }
            }
            //ParseCommunicationCommand.ExecuteCommand("定位点设置-右");
            SetButtonEnable(true);
        }

        /// <summary>
        /// 右
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_Right_Click(object sender, EventArgs e)
        {
            if (ParseCommunicationCommand.IsReading)
                return;
            SetButtonEnable(false);
            if (this.nud_CroodY.Value == nud_CroodY.Minimum)
            {

            }
            else if (this.nud_CroodY.Value - this.nud_CroodY.Increment < nud_CroodY.Minimum)
            {
                if (ParseCommunicationCommand.ExecuteCommand("定位点设置-右"))
                {
                    this.nud_CroodY.Value = this.nud_CroodY.Minimum;
                }
            }
            else
            {
                if (ParseCommunicationCommand.ExecuteCommand("定位点设置-右"))
                {
                    this.nud_CroodY.Value -= this.nud_CroodY.Increment;
                }
            }
            SetButtonEnable(true);
        }

        private void btn_Save_Click(object sender, EventArgs e)
        {
            if (ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
            {
                MessageBox.Show("未连接");
                return;
            }

            btn_Save.Enabled = false;
            ParseCommunicationCommand.OriginX = x;
            ParseCommunicationCommand.OriginY = y;
            CommuncationData cd = ParseCommunicationCommand.ExecuteCommand1("保存定位点信息");
            
            ParseCommunicationCommand.SaveConfig(x + "," + y, "LastOriginPoint");
            btn_Save.Enabled = true;
            if(cd.isRead)
            {
                int index = 2;
                ParseCommunicationCommand.SaveOriginX = ConvertType.ToInt32R(cd.readData,ref index);
                ParseCommunicationCommand.SaveOriginY = ConvertType.ToInt32R(cd.readData, ref index);
                ParseCommunicationCommand.SaveConfig(path);
                //Console.WriteLine(ParseCommunicationCommand.SaveOriginX + "," + ParseCommunicationCommand.SaveOriginY);
                if (addCutFileForm != null && addCutFileForm.file != null)
                {
                    addCutFileForm.file.x = ParseCommunicationCommand.SaveOriginX;
                    addCutFileForm.file.y = ParseCommunicationCommand.SaveOriginY;
                }

                //List<byte> data = new List<byte>();
                //data.Clear();
                //if (ParseCommunicationCommand.sendCommandFlag != null)
                //    data.AddRange(ParseCommunicationCommand.sendCommandFlag);
                //data.Add(0x14);
                //data.AddRange((BitConverter.GetBytes(ParseCommunicationCommand.SaveOriginX)));
                //data.AddRange((BitConverter.GetBytes(ParseCommunicationCommand.SaveOriginY)));
                //bool isSuccess1 = ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "定位点坐标发送", 120);
                //if (isSuccess1)
                //    MessageBox.Show("定位点坐标发送成功");
                //else
                //    MessageBox.Show("定位点坐标发送失败");

       
                MessageBox.Show("保存成功", "提示！");
            }

            //取消保存定位点信息
            this.Visible = false;
        }
        private void btn_Quit_Click(object sender, EventArgs e)
        {
            if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
            {
                btn_Quit.Enabled = false;
                ParseCommunicationCommand.ExecuteCommand("取消保存定位点信息");
                btn_Quit.Enabled = true;
            }
            //this.nud_CroodX.Value = (decimal)ParseCommunicationCommand.OriginX;
            //this.nud_CroodY.Value = (decimal)ParseCommunicationCommand.OriginY;
            this.Visible = false;
            //取消保存定位点信息
        }

        private void nud_CroodX_ValueChanged(object sender, EventArgs e)
        {
            if (isVisibleChange)
                return;
            //if (Math.Abs(((int)(nud_CroodX.Value * 10))) % 2 != 0)
            //{
            //    nud_CroodX.Value += (decimal)0.1;
            //    return;
            //}
            if (this.nud_CroodX.Value == this.nud_CroodX.Minimum)
                this.btn_XMinus.Enabled = false;
            else
                this.btn_XMinus.Enabled = true;


            //float changeValue = (float)nud_CroodX.Value - x;
            //if (changeValue != (float)nud_CroodX.Increment && changeValue != 0)
            //{
            //    int[] step = GetMoveStep(changeValue);
            //    ParseCommunicationCommand.ExecuteCommand("移动步长设置-250mm");
            //    string c = "";
            //    if (changeValue < 0)
            //        c = "定位点设置-左";
            //    else
            //        c = "定位点设置-右";
            //    for (int i = 0; i < step[0]; i++)
            //    {
            //        ParseCommunicationCommand.ExecuteCommand(c);
            //    }

            //    ParseCommunicationCommand.ExecuteCommand("移动步长设置-50mm");
            //    for (int i = 0; i < step[1]; i++)
            //    {
            //        ParseCommunicationCommand.ExecuteCommand(c);
            //    }

            //    ParseCommunicationCommand.ExecuteCommand("移动步长设置-0.2mm");
            //    for (int i = 0; i < step[2]; i++)
            //    {
            //        ParseCommunicationCommand.ExecuteCommand(c);
            //    }
            //    rad_CheckedChanged(null, null);
            //}
            x = (float)nud_CroodX.Value;
            this.lbl_CroodX.Text = x.ToString() + "mm";
        }

        private void nud_CroodY_ValueChanged(object sender, EventArgs e)
        {
            if (isVisibleChange)
                return;
            //if (((int)(nud_CroodY.Value * 10)) % 2 != 0)
            //{
            //    nud_CroodY.Value += (decimal)0.1;
            //    return;
            //}
            if (this.nud_CroodY.Value == this.nud_CroodX.Minimum)
                this.btn_YMinus.Enabled = false;
            else
                this.btn_YMinus.Enabled = true;


            //float changeValue = (float)nud_CroodY.Value - y;
            //if (changeValue != (float)nud_CroodY.Increment && changeValue != 0)
            //{
            //    int[] step = GetMoveStep(changeValue);
            //    ParseCommunicationCommand.ExecuteCommand("移动步长设置-250mm");
            //    string c = "";
            //    if (changeValue < 0)
            //        c = "定位点设置-上";
            //    else
            //        c = "定位点设置-下";
            //    for (int i = 0; i < step[0]; i++)
            //    {
            //        ParseCommunicationCommand.ExecuteCommand(c);
            //    }

            //    ParseCommunicationCommand.ExecuteCommand("移动步长设置-50mm");
            //    for (int i = 0; i < step[1]; i++)
            //    {
            //        ParseCommunicationCommand.ExecuteCommand(c);
            //    }

            //    ParseCommunicationCommand.ExecuteCommand("移动步长设置-0.2mm");
            //    for (int i = 0; i < step[2]; i++)
            //    {
            //        ParseCommunicationCommand.ExecuteCommand(c);
            //    }
            //    rad_CheckedChanged(null, null);
            //}
            y = (float)nud_CroodY.Value;
            this.lbl_CroodY.Text = y.ToString() + "mm";
        }

        //   //移动步长设置 250mm 050x01
        //   writeDataList.Add("移动步长设置-250mm", GetCommandByte(commandFlag, 0x05, 0x01, true));
        //   writeDataList.Add("移动步长设置-50mm", GetCommandByte(commandFlag, 0x05, 0x02, true));
        //   writeDataList.Add("移动步长设置-0.2mm", GetCommandByte(commandFlag, 0x05, 0x03, true));

        private void rad_CheckedChanged(object sender, EventArgs e)
        {
            //移动步长设置-700mm
            if (this.rab_250.Checked)
            {
                this.nud_CroodX.Increment = 250;
                this.nud_CroodY.Increment = 250;
                ParseCommunicationCommand.ExecuteCommand("移动步长设置-250mm");
            }
            else if (this.rab_50.Checked)
            {
                this.nud_CroodX.Increment = 50;
                this.nud_CroodY.Increment = 50;
                ParseCommunicationCommand.ExecuteCommand("移动步长设置-50mm");
            }
            //else if (this.rab_700.Checked)
            //{
            //    this.nud_CroodX.Increment = 700;
            //    this.nud_CroodY.Increment = 700;
            //    ParseCommunicationCommand.ExecuteCommand("移动步长设置-700mm");
            //}
            else
            {
                this.nud_CroodX.Increment = (decimal)0.2;
                this.nud_CroodY.Increment = (decimal)0.2;
                ParseCommunicationCommand.ExecuteCommand("移动步长设置-0.2mm");
            }
        }

        bool isVisibleChange = false;
        private void frm_SetOrigin_VisibleChanged(object sender, EventArgs e)
        {
            isVisibleChange = true;
            if (Visible)
            {
                if (ParseCommunicationCommand.userType == 1)
                    this.btn_Save.Visible = true;
                else
                    this.btn_Save.Visible = false;
                //终止工作时才能设置 20160818
                if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                {
                    //if (ParseCommunicationCommand.userType == 1)
                    //    this.btn_Save.Visible = true;
                    //else
                    //    this.btn_Save.Visible = false;
                    foreach (Control item in this.Controls)
                    {
                            item.Enabled = true;
                    }
                }
                else
                {
                    foreach (Control item in this.Controls)
                    {
                        if (item != btn_Quit)
                            item.Enabled = false;
                    }
                }


                if (ParseCommunicationCommand.deviceState != null)
                {
                    ParseCommunicationCommand.OriginX = ParseCommunicationCommand.deviceState.X / 90;
                    ParseCommunicationCommand.OriginY = ParseCommunicationCommand.deviceState.Y / 90;
                }

                x = ParseCommunicationCommand.OriginX;
                y = ParseCommunicationCommand.OriginY;
                if (x < (float)this.nud_CroodX.Minimum)
                    x = (float)this.nud_CroodX.Minimum;
                if (y < (float)this.nud_CroodY.Minimum)
                    y = (float)this.nud_CroodY.Minimum;
                this.nud_CroodX.Value = (decimal)x;
                this.nud_CroodY.Value = (decimal)y;
                this.lbl_CroodX.Text = x.ToString() + "mm";
                this.lbl_CroodY.Text = y.ToString() + "mm";

                rad_CheckedChanged(null, null);
                rab_Light_CheckedChanged(null, null);
            }
            isVisibleChange = false;
        }

        public int[] GetMoveStep(float value)
        {
            int[] step = new int[3];
            step[0] = (int)(value / 250);
            value = value - step[0] * 250;
            step[1] = (int)(value / 50);
            value = value - step[1] * 50;
            step[2] = (int)(value * 10) / 2;
            return step;
        }

        private void rab_Light_CheckedChanged(object sender, EventArgs e)
        {
            //   //移动出光设置 出光  060x01
            //   writeDataList.Add("移动出光设置-出光", GetCommandByte(commandFlag, 0x06, 0x01, true));
            //   writeDataList.Add("移动出光设置-不出光", GetCommandByte(commandFlag, 0x06, 0x00, true));
            if (this.rab_Light.Checked)
            {
                ParseCommunicationCommand.ExecuteCommand("移动出光设置-出光");
            }
            else
            {
                ParseCommunicationCommand.ExecuteCommand("移动出光设置-不出光");
            }
        }

        private void backgroundWorker1_DoWork(object sender, System.ComponentModel.DoWorkEventArgs e)
        {
            while (true)
            {
                if ((IsMouseDown && DateTime.Now.Subtract(mouseDownTime).TotalMilliseconds > 1000))
                {
                    //delayTime = 150;
                    Invoke(setFormControlValue1);
                }
                else if (!IsMouseDown)
                {
                    //delayTime = 500;
                    break;
                }
                System.Threading.Thread.Sleep(delayTime);
            }
        }

        bool IsMouseDown = false;
        DateTime mouseDownTime;
        int delayTime = 100;
        Rich_Button MouseDownButton;

        private void btn_MouseDown(object sender, MouseEventArgs e)
        {
            MouseDownButton = (Rich_Button)sender;
            MouseDownButton.BackColor = System.Drawing.Color.Lime;
            if(rad_02.Checked)
            {
                IsMouseDown = true;
                mouseDownTime = DateTime.Now;
                if (!backgroundWorker1.IsBusy)
                    backgroundWorker1.RunWorkerAsync();
            }
        }

        private void btn_MouseUp(object sender, MouseEventArgs e)
        {
            MouseDownButton = (Rich_Button)sender;
            MouseDownButton.BackColor = System.Drawing.SystemColors.ActiveCaption;
            IsMouseDown = false;
            MouseDownButton = null;
        }
        public void SetFormControlValue1()
        {
            if (ParseCommunicationCommand.IsReading || MouseDownButton == null)
                return;
            switch (MouseDownButton.Name)
            {
                case "btn_Up":
                    btn_Up_Click(MouseDownButton, null);
                    break;
                case "btn_Down":
                    btn_Down_Click(MouseDownButton, null);
                    break;
                case "btn_Left":
                    btn_Left_Click(MouseDownButton, null);
                    break;
                case "btn_Right":
                    btn_Right_Click(MouseDownButton, null);
                    break;
            }
            //button11_Click(btn_ClearAll, null);
        }

        //private static byte[] ExecuteCommand(string command)
        //{
        //    byte[] data = ParseCommunicationCommand.writeDataList[command];
        //    ParseCommunicationCommand.ExecuteOriginPointCommand(data, command, 120);
        //    return data;
        //}
        public void SetButtonEnable(bool b)
        {
            btn_Down.Enabled = b;
            btn_Up.Enabled = b;
            btn_Left.Enabled = b;
            btn_Right.Enabled = b;
        }
    }
}