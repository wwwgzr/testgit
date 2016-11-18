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
    public partial class frm_SetManualFeed : frm_BaseForm
    {
        short feedLen;
        short offset;
        short sectionLenM;
        public frm_SetManualFeed()
        {
            InitializeComponent();
        }

        private void btn_Save_Click(object sender, EventArgs e)
        {
            if (!ParseCommunicationCommand.IsReading)
            {
                if (!ParseCommunicationCommand.IsReading)
                {
                    if (short.TryParse(this.txt_FeedLen.Text.Trim(), out feedLen))
                    {
                        ParseCommunicationCommand.manualFeedLen = feedLen;
                    }
                    if (short.TryParse(this.txt_FeedOffset.Text.Trim(), out offset))
                    {
                        ParseCommunicationCommand.feedOffset = offset;
                    }
                    if (short.TryParse(this.txt_SectionLen.Text.Trim(), out sectionLenM))
                    {
                        ParseCommunicationCommand.sectionLen = sectionLenM;
                    }

                    bool b = ParseCommunicationCommand.ExecuteManualFeedParam();
                    ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.manualFeedLen.ToString(), "ManualFeedLen");
                    ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.feedOffset.ToString(), "FeedOffset");
                    ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.sectionLen.ToString(), "SectionLen");
                    if (b)
                    {
                        MessageBox.Show("保存成功！", "提示");
                        ParseCommunicationCommand.SaveConfig(MenuController.owner.paintingPanel1.FilePath);
                    }
                    if(ParseCommunicationCommand.saveTest)
                    {
                        ParseCommunicationCommand.SaveConfig(MenuController.owner.paintingPanel1.FilePath);
                    }
                    Visible = false;
                }
            }
        }

        private void btn_Exit_Click(object sender, EventArgs e)
        {
            Visible = false;
        }

        private void frm_SetManualFeed_VisibleChanged(object sender, EventArgs e)
        {
            if(Visible)
            {
                //终止工作时才能设置 20160818
                if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                {
                    foreach (Control item in this.Controls)
                    {
                        item.Enabled = true;
                    }
                }
                else if(ParseCommunicationCommand.saveTest)
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
                feedLen = ParseCommunicationCommand.manualFeedLen;
                offset = ParseCommunicationCommand.feedOffset;
                sectionLenM = ParseCommunicationCommand.sectionLen;
                this.txt_FeedLen.Text = feedLen.ToString();
                this.txt_FeedOffset.Text = offset.ToString();
                this.txt_SectionLen.Text = sectionLenM.ToString();

            }
        }
    }
}