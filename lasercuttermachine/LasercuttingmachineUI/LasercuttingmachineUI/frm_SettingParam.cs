using CommonModule;
using CommunicationModule;
using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class frm_SettingParam : frm_BaseForm
    {
        public ReadParam newParam;
        public frm_AddFile addCutFileForm;
        public string path;
        public frm_SettingParam()
        {
            InitializeComponent();
            this.cob_ClothKey.SelectedIndex = 1;
            this.cob_PushFeedKey.SelectedIndex = 1;
            this.cob_LaserB.SelectedIndex = 1;
        }

        private void frm_SettingParam_VisibleChanged(object sender, System.EventArgs e)
        {
            if (this.Visible)
            {
                MenuController.owner.userSetParam1.Enabled = false;
                if (!frm_Project.isProject)
                {
                    if (ParseCommunicationCommand.readParam != null && ParseCommunicationCommand.readParam.isInital)
                        newParam = (ReadParam)ParseCommunicationCommand.readParam.Clone();
                    else
                        newParam = new ReadParam();

                    InitialControl(newParam);
                }
                
                //终止工作时才能设置 20160818
                if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                {
                    foreach (Control item in this.groupBox1.Controls)
                    {
                        item.Enabled = true;
                    }
                }
                else
                {
                    foreach (Control item in this.groupBox1.Controls)
                    {
                        if (item != btn_Exit)
                            item.Enabled = false;
                    }
                }
            }
            else
            {
                if(ParseCommunicationCommand.userType == 1)
                    MenuController.owner.userSetParam1.Enabled = true;
                else
                    MenuController.owner.userSetParam1.Enabled = false;
            }
        }

        public void InitialControl(ReadParam rp)
        {
            if (rp == null)
                return;
            this.txt_CutMaxSpeed.Text = rp.cutMaxSpeed1.ToString();
            this.txt_CutSpeed.Text = rp.cutSpeed1.ToString();
            this.txt_FeedCompensate.Text = rp.feedCompensate.ToString();
            //this.txt_FeedSpeed.Text = rp.feedSpeed.ToString();
            //if (rp.feedSpeed == 0)
            //    this.cob_FeedSpeed.Text = "低速";
            //else if (rp.feedSpeed == 1)
            //    this.cob_FeedSpeed.Text = "中速";
            //else if (rp.feedSpeed == 2)
            //    this.cob_FeedSpeed.Text = "高速";
            //if (rp.feedSpeedU >= 0 && rp.feedSpeedU <= 2)
            this.cob_ClothKey.SelectedIndex = rp.clothKey;
            this.cob_PushFeedKey.SelectedIndex = rp.pushFeedKey;
            this.cob_LaserB.SelectedIndex = rp.laserBKey;
            
            this.txt_FeedSpeedU.Text = rp.feedSpeedU1.ToString();
            this.txt_FeedSpeedX.Text = rp.feedSpeedX1.ToString();
            this.txt_CutInitSpeed.Text = rp.cutInitalSpeed1.ToString();
            this.txt_MaxLightIntensity.Text = rp.maxLightIntensity.ToString();
            this.txt_MinLightIntensity.Text = rp.minLightIntensity.ToString();

            this.txt_MaxLightIntensityB.Text = rp.maxLightIntensityB.ToString();
            this.txt_MinLightIntensityB.Text = rp.minLightIntensityB.ToString();

            this.txt_MoveFrameSpeed.Text = rp.moveFrameSpeed1.ToString();
            this.txt_SpeedUpTime.Text = rp.cutSpeedUpTime.ToString();
            this.txt_TestWorkSpeed.Text = rp.testWorkSpeed1.ToString();
            this.txt_XLenCorrection.Text = (rp.xLenCorrection1).ToString();
            this.txt_YLenCorrection.Text = (rp.yLenCorrection1).ToString();
            //AB头间距
            this.txt_laserDistance.Text = rp.distanceAB.ToString();


            if (rp.needCut == 1)
                this.rab_EndWorkCut.Checked = true;
            else
                this.rab_EndWorkNoCut.Checked = true;
            this.txt_CutOffset.Text = rp.cutOffset.ToString();
            if (rp.autoFeed == 1)
                this.rab_Auto.Checked = true;
            else
                this.rab_Manual.Checked = true;

            //设备类型
            //if(rp.deviceType)
            SetUserEditControl();
        }
        /// <summary>
        /// 根据用户设置控件是否可用
        /// </summary>
        public void SetUserEditControl()
        {
            if (ParseCommunicationCommand.userType == 2)
            {
                SetControlEnable(false);
            }
            else
            {
                SetControlEnable(true);
            }
        }
        /// <summary>
        /// 设置控件是否可用
        /// </summary>
        private void SetControlEnable(bool isEnable)
        {
            this.txt_CutMaxSpeed.Enabled = isEnable;
            this.txt_CutSpeed.Enabled = isEnable;
            this.txt_FeedCompensate.Enabled = isEnable;
            this.cob_ClothKey.Enabled = isEnable;
            this.cob_PushFeedKey.Enabled = isEnable;
            cob_LaserB.Enabled = isEnable;
            this.txt_FeedSpeedU.Enabled = isEnable;
            this.txt_FeedSpeedX.Enabled = isEnable;
            this.txt_CutInitSpeed.Enabled = isEnable;
            this.txt_MaxLightIntensity.Enabled = isEnable;
            this.txt_MinLightIntensity.Enabled = isEnable;
            this.txt_MoveFrameSpeed.Enabled = isEnable;
            this.txt_SpeedUpTime.Enabled = isEnable;
            this.txt_TestWorkSpeed.Enabled = isEnable;
            this.txt_XLenCorrection.Enabled = isEnable;
            this.txt_YLenCorrection.Enabled = isEnable;
            this.txt_CutOffset.Enabled = isEnable;
            this.rab_EndWorkCut.Enabled = isEnable;
            this.rab_EndWorkNoCut.Enabled = isEnable;
            //this.rab_Auto.Enabled = isEnable;
            //this.rab_Manual.Enabled = isEnable;
            rab_EndWorkCut.Enabled = isEnable;
        }
        private void btn_Save_Click(object sender, System.EventArgs e)
        {
            if (ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
            {
                MessageBox.Show("未连接");
                return;
            }
            newParam.cutMaxSpeed1 = ConvertType.ConvertByte(txt_CutMaxSpeed.Text);
            newParam.cutSpeed1 = ConvertType.ConvertByte(txt_CutSpeed.Text);
            newParam.feedCompensate = ConvertType.ConvertByte(txt_FeedCompensate.Text);
            //newParam.feedSpeed = ConvertType.ConvertByte(txt_FeedSpeed.Text);
            //newParam.feedSpeedU = (byte)cob_FeedSpeedU.SelectedIndex;
            newParam.feedSpeedU1 = ConvertType.ConvertShort(txt_FeedSpeedU.Text);
            newParam.feedSpeedX1 = ConvertType.ConvertShort(txt_FeedSpeedX.Text);
            newParam.clothKey = (byte)cob_ClothKey.SelectedIndex;
            newParam.pushFeedKey = (byte)cob_PushFeedKey.SelectedIndex;
            //
            newParam.laserBKey = (byte)cob_LaserB.SelectedIndex;

            //this.txt_FeedSpeedU.Text = rp.feedSpeedU.ToString();
            //this.txt_FeedSpeedX.Text = rp.feedSpeedX.ToString();

            newParam.cutInitalSpeed1 = ConvertType.ConvertByte(txt_CutInitSpeed.Text);
            newParam.maxLightIntensity = ConvertType.ConvertByte(txt_MaxLightIntensity.Text);
            newParam.minLightIntensity = ConvertType.ConvertByte(txt_MinLightIntensity.Text);

            //B激光头光强
            newParam.maxLightIntensityB = ConvertType.ConvertByte(txt_MaxLightIntensityB.Text);
            newParam.minLightIntensityB = ConvertType.ConvertByte(txt_MinLightIntensityB.Text);
            //

            newParam.moveFrameSpeed1 = ConvertType.ConvertByte(txt_MoveFrameSpeed.Text);
            newParam.cutSpeedUpTime = ConvertType.ConvertShort(txt_SpeedUpTime.Text);
            newParam.testWorkSpeed1 = ConvertType.ConvertByte(txt_TestWorkSpeed.Text);
            newParam.xLenCorrection1 = ConvertType.ConvertInt(txt_XLenCorrection.Text);
            newParam.yLenCorrection1 = ConvertType.ConvertInt(txt_YLenCorrection.Text);

            //AB头间距
            newParam.distanceAB = ConvertType.ConvertShort(txt_laserDistance.Text);

            if (this.rab_Auto.Checked)
                newParam.autoFeed = 1;
            else
                newParam.autoFeed = 0;
            if (this.rab_EndWorkNoCut.Checked)
                newParam.clothTest = 1;
            else
                newParam.clothTest = 0;

            if (this.rab_EndWorkCut.Checked)
                newParam.needCut = 1;
            else
                newParam.needCut = 0;
            newParam.cutOffset = ConvertType.ConvertByte(txt_CutOffset.Text);
            ////设备类型
            ////if(this.rab_Roller)
            ////newParam.deviceType = 1816;
            //if (ParseCommunicationCommand.readParam != null && ParseCommunicationCommand.readParam.Equals(newParam))
            //{
            //    MessageBox.Show("参数保存成功");
            //    Visible = false;
            //    return;
            //}

            //if (frm_Project.isProject)
            //{
            //    MessageBox.Show("参数保存成功");
            //    Visible = false;
            //}
            //else
                SaveConfig();
        }

        private void SaveConfig()
        {
            List<byte> data = new List<byte>();
            if (ParseCommunicationCommand.sendCommandFlag != null)
                data.AddRange(ParseCommunicationCommand.sendCommandFlag);
            data.AddRange(newParam.GetByteData());

            bool isSuccess = ParseCommunicationCommand.ExecuteSetParamCommand(data.ToArray(), "设置参数", 120);
            if (isSuccess)
            {
                MessageBox.Show("参数保存成功");
                Visible = false;
                ParseCommunicationCommand.readParam = (ReadParam)newParam.Clone();
                MenuController.owner.userSetParam1.InitialControl(ParseCommunicationCommand.readParam);
                ParseCommunicationCommand.SaveConfig(path);
                if (addCutFileForm != null && addCutFileForm.file != null)
                {
                    addCutFileForm.file.readParam = (ReadParam)newParam.Clone();
                }
            }
            else
            {
                MessageBox.Show("参数保存失败");
            }
        }

        //public void SaveConfigP()
        //{
        //    List<byte> data = new List<byte>();
        //    if (ParseCommunicationCommand.sendCommandFlag != null)
        //        data.AddRange(ParseCommunicationCommand.sendCommandFlag);
        //    data.AddRange(ParseCommunicationCommand.readParam.GetByteData());

        //    bool isSuccess = ParseCommunicationCommand.ExecuteSetParamCommand(data.ToArray(), "设置参数", 120);
        //    ParseCommunicationCommand.readParam = (ReadParam)newParam.Clone();
        //    ParseCommunicationCommand.SaveConfig(path);
        //}

        private void btn_Exit_Click(object sender, System.EventArgs e)
        {
            this.Visible = false;
        }

        private void rab_EndWorkCut_CheckedChanged(object sender, EventArgs e)
        {
            if (rab_EndWorkCut.Checked)
                this.txt_CutOffset.Enabled = true;
            else
                this.txt_CutOffset.Enabled = false;
        }

    }
}