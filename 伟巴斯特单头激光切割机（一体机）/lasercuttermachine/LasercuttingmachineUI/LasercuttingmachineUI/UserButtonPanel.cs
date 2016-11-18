using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using CommunicationModule;
using CommonModule;
using System.IO;

namespace LasercuttingmachineUI
{
    public partial class UserButtonPanel : UserControl
    {
        frm_NewButton newButtonForm;
        //public frm_SetOrigin setOriginForm;
        //public frm_SettingParam settingParamForm;
        public frm_AddFile addFileForm;
        public bool isOpen = true;
        public UserButtonPanel()
        {
            InitializeComponent();
            this.paintingPanel2.isView = true;
            if (addFileForm == null)
            {
                addFileForm = new frm_AddFile();
                addFileForm.Owner = MenuController.owner;
                addFileForm.Tag = "Visble-1";
            }
            //MenuController.owner.userSetParam1.addCutFileForm = addFileForm;
        }

        ///// <summary>
        ///// 添加按钮
        ///// </summary>
        ///// <param name="name">按钮名称</param>
        ///// <param name="text">按钮显示文字</param>
        ///// <param name="tag"></param>
        ///// <returns></returns>
        //public Button AddButton(string name, string text, object tag,Panel owner,string iconfile,bool visible = true)
        //{
        //    Button mbtn_test = new Button();
        //    mbtn_test.Name = name;
        //    mbtn_test.Size = new System.Drawing.Size(160, 160);
        //    mbtn_test.Text = text;
        //    mbtn_test.UseVisualStyleBackColor = false;
        //    if(iconfile.Length > 0)
        //    {
        //        Bitmap bmp = new Bitmap(iconfile);
        //        mbtn_test.Image = bmp;
        //    }
        //    mbtn_test.Tag = tag;
        //    mbtn_test.Click += mbtn_test_Click;
        //    mbtn_test.Visible = visible;
        //    owner.Controls.Add(mbtn_test);
        //    return mbtn_test;
        //}

        /// <summary>
        /// 添加按钮
        /// </summary>
        /// <param name="name">按钮名称</param>
        /// <param name="text">按钮显示文字</param>
        /// <param name="tag"></param>
        /// <returns></returns>
        public ButtonText AddButton(string name, string text, object tag, Panel owner, string iconfile, string cutFile = "", bool visible = true)
        {
            ButtonText mbtn_test = new ButtonText();
            mbtn_test.Name = name;
            mbtn_test.Text = text;
            mbtn_test.ButtonText1 = text;
            if (iconfile != null && iconfile.Length > 0)
            {
                mbtn_test.ButtonImage = iconfile;
            }
            if (cutFile.Length > 0)
            {
                if (cutFile.EndsWith(".dxf"))
                {
                    paintingPanel2.OpenDxfFile(cutFile);
                }
                else
                {
                    paintingPanel2.ReadPltFile(cutFile);
                }

                Bitmap bmp = new Bitmap(164, 164);
                paintingPanel2.DrawToBitmap(bmp, paintingPanel2.ClientRectangle);
                mbtn_test.btn_Icon.BackgroundImage = bmp;
            }
            mbtn_test.btn_Icon.Tag = tag;
            mbtn_test.lbl_Text.Tag = tag;
            mbtn_test.lbl_Text.Click += mbtn_test_Click;
            mbtn_test.btn_Icon.Click += mbtn_test_Click;
            mbtn_test.Visible = visible;
            owner.Controls.Add(mbtn_test);
            return mbtn_test;
        }
        private void mbtn_test_Click(object sender, EventArgs e)
        {
            Control btn = (Control)sender;
            selectButton = btn;
            if (this.flowLayoutPanel1.Visible)
            {
                flowLayoutPanel2.Controls.Clear();
                ProjectClass projectClass = (ProjectClass)btn.Tag;
                selectProject = projectClass;
                lbl_Navi.Text = selectProject.name;
                for (int i = 0; i < projectClass.products.Count; i++)
                {
                    AddButton(projectClass.products[i].name,
                    projectClass.products[i].name,
                    projectClass.products[i], flowLayoutPanel2,
                    projectClass.products[i].iconName);
                }

                if (ParseCommunicationCommand.userType == 2 || isOpen)
                {
                    flowLayoutPanel2.Visible = true;
                    flowLayoutPanel1.Visible = false;
                    btn_Next.Enabled = false;
                    btn_Edit.Enabled = false;
                    btn_Front.Enabled = true;
                }
                else
                {
                    btn_Next.Enabled = true;
                    btn_Edit.Enabled = true;
                }
            }
            else if (this.flowLayoutPanel2.Visible)
            {
                flowLayoutPanel3.Controls.Clear();
                ProductClass productClass = (ProductClass)btn.Tag;
                selectProduct = productClass;
                lbl_Navi.Text = selectProject.name + "->" + selectProduct.name;
                for (int i = 0; i < productClass.cutFiles.Count; i++)
                {
                    //);
                    AddButton(Path.GetFileName(productClass.cutFiles[i].cutFileName),
                    Path.GetFileName(productClass.cutFiles[i].cutFileName),
                    productClass.cutFiles[i], flowLayoutPanel3, "", productClass.cutFiles[i].cutFileName);
                }

                btn_Edit.Enabled = true;
                btn_Next.Enabled = true;
                if (ParseCommunicationCommand.userType == 2 || isOpen)
                {
                    flowLayoutPanel3.Visible = true;
                    flowLayoutPanel2.Visible = false;
                    btn_Next.Enabled = false;
                    btn_Edit.Enabled = false;
                    btn_Front.Enabled = true;
                }
                else
                {
                    btn_Next.Enabled = true;
                    btn_Edit.Enabled = true;
                }
            }
            else if (this.flowLayoutPanel3.Visible)
            {
                selectCutFile = (CutFileClass)btn.Tag;
                lbl_Navi.Text = selectProject.name + "->" + selectProduct.name + "->" + Path.GetFileName(selectCutFile.cutFileName);
                btn_Ok.Visible = true;
                btn_Edit.Enabled = true;
                btn_Front.Enabled = true;
            }
        }
        public ProductClass selectProduct;
        public ProjectClass selectProject;
        public CutFileClass selectCutFile;
        public Control selectButton;
        private void UserButtonPanel_Load(object sender, EventArgs e)
        {

        }

        private void btn_Front_Click(object sender, EventArgs e)
        {
            btn_Ok.Visible = false;
            selectCutFile = null;
            selectButton = null;
            if (flowLayoutPanel3.Visible)
            {
                flowLayoutPanel1.Visible = false;
                flowLayoutPanel2.Visible = true;
                flowLayoutPanel3.Visible = false;
                btn_Front.Enabled = true;
                btn_Next.Enabled = false;
                btn_Edit.Enabled = false;
                lbl_Navi.Text = selectProject.name + "->" + selectProduct.name;
            }
            else if (flowLayoutPanel2.Visible)
            {
                flowLayoutPanel1.Visible = true;
                flowLayoutPanel2.Visible = false;
                flowLayoutPanel3.Visible = false;
                btn_Front.Enabled = false;
                btn_Next.Enabled = false;
                btn_Edit.Enabled = false;
                lbl_Navi.Text = selectProject.name;
            }
            else
            {
                btn_Front.Enabled = false;
                btn_Next.Enabled = false;
                btn_Edit.Enabled = false;
            }
        }

        private void btn_Next_Click(object sender, EventArgs e)
        {
            btn_Ok.Visible = false;
            selectCutFile = null;
            selectButton = null;
            if (flowLayoutPanel1.Visible)
            {
                flowLayoutPanel1.Visible = false;
                flowLayoutPanel2.Visible = true;
                flowLayoutPanel3.Visible = false;
                btn_Front.Enabled = true;
                btn_Next.Enabled = false;
                btn_Edit.Enabled = false;
            }
            else if (flowLayoutPanel2.Visible)
            {
                flowLayoutPanel1.Visible = false;
                flowLayoutPanel2.Visible = false;
                flowLayoutPanel3.Visible = true;
                btn_Front.Enabled = true;
                btn_Next.Enabled = false;
                btn_Edit.Enabled = false;
            }
            else
            {
                btn_Front.Enabled = true;
                btn_Next.Enabled = false;
                btn_Edit.Enabled = false;
            }
        }

        private void UserButtonPanel_VisibleChanged(object sender, EventArgs e)
        {
            flowLayoutPanel1.Visible = true;
            flowLayoutPanel2.Visible = false;
            flowLayoutPanel3.Visible = false;
            btn_Front.Enabled = false;
            btn_Next.Enabled = false;
            btn_Ok.Visible = false;
            btn_Edit.Enabled = false;
            this.lbl_Navi.Text = "";
            //if (ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
            //{
            //    this.Enabled = false; 
            //    return;
            //}
            //this.Enabled = true;

            if (Visible)
            {
                if (ParseCommunicationCommand.userType == 1 && !isOpen)
                {
                    this.btn_New.Visible = true;
                    this.btn_Edit.Visible = true;
                    this.btn_Delete.Visible = true;
                    this.btn_Next.Visible = true;
                }
                else
                {
                    this.btn_New.Visible = false;
                    this.btn_Edit.Visible = false;
                    this.btn_Delete.Visible = false;
                    this.btn_Next.Visible = false;
                }
                selectProject = null;
                selectProduct = null;
                selectCutFile = null;
                selectButton = null;
                flowLayoutPanel1.Controls.Clear();
                flowLayoutPanel2.Controls.Clear();
                flowLayoutPanel3.Controls.Clear();
                if (ParseCommunicationCommand.projectClassList != null)
                {
                    for (int i = 0; i < ParseCommunicationCommand.projectClassList.Count; i++)
                    {
                        AddButton(ParseCommunicationCommand.projectClassList[i].name, ParseCommunicationCommand.projectClassList[i].name,
                            ParseCommunicationCommand.projectClassList[i], flowLayoutPanel1,
                            ParseCommunicationCommand.projectClassList[i].iconName);
                    }
                }
            }
        }

        private void btn_Ok_Click(object sender, EventArgs e)
        {
            try
            {
                this.btn_Ok.BackgroundImage = Image.FromFile(Application.StartupPath + "\\icon\\gggg1.png");
            }
            catch (Exception)
            {

            }

            if (!ParseCommunicationCommand.saveTest && ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
            {
                try
                {
                    this.btn_Ok.BackgroundImage = Image.FromFile(Application.StartupPath + "\\icon\\5756a1d774ff04.png");
                }
                catch (Exception)
                {
                    return;
                }
                return;
            }
            WriteParam();
            try
            {
                this.btn_Ok.BackgroundImage = Image.FromFile(Application.StartupPath + "\\icon\\5756a1d774ff04.png");
            }
            catch (Exception)
            {

            }
        }


        private void WriteParam()
        {
            ParseCommunicationCommand.IsReading = true;
            List<byte> data = new List<byte>();

            if (selectCutFile.cutFileName.EndsWith(".dxf"))
                MenuController.owner.paintingPanel1.OpenDxfFile(selectCutFile.cutFileName);
            else if (selectCutFile.cutFileName.EndsWith(".plt"))
                MenuController.owner.paintingPanel1.ReadPltFile(selectCutFile.cutFileName);
            CutFileClass readFile = ParseCommunicationCommand.ReadConfigCutFile(selectCutFile.cutFileName);
            selectCutFile.readParam = readFile.readParam;
            selectCutFile.manualFeedBreakLength = readFile.manualFeedBreakLength;
            selectCutFile.manualFeedLength = readFile.manualFeedLength;
            selectCutFile.manualFeedOffsetLength = readFile.manualFeedOffsetLength;
            selectCutFile.x = readFile.x;
            selectCutFile.y = readFile.y;

            bool isSuccess = false;
            bool writeSuccess = false;
            if (selectCutFile != null && selectCutFile.readParam != null && ParseCommunicationCommand.readParam != null)
            {
                selectCutFile.readParam.deviceTime = ParseCommunicationCommand.readParam.deviceTime;
                selectCutFile.readParam.deviceType = ParseCommunicationCommand.readParam.deviceType;
                selectCutFile.readParam.gateWay = ParseCommunicationCommand.readParam.gateWay;
                selectCutFile.readParam.ipAddress = ParseCommunicationCommand.readParam.ipAddress;
                selectCutFile.readParam.subNetMask = ParseCommunicationCommand.readParam.subNetMask;
                selectCutFile.readParam.versionNum = ParseCommunicationCommand.readParam.versionNum;

                if (ParseCommunicationCommand.sendCommandFlag != null)
                    data.AddRange(ParseCommunicationCommand.sendCommandFlag);
                data.AddRange(selectCutFile.readParam.GetByteData());
                isSuccess = ParseCommunicationCommand.ExecuteSetParamCommand(data.ToArray(), "设置项目参数", 120);

                //if(isSuccess)
                //    MessageBox.Show("设置项目参数成功");
                //else
                //    MessageBox.Show("设置项目参数失败");

            }
            if (ParseCommunicationCommand.saveTest && selectCutFile.readParam != null)
            {
                ParseCommunicationCommand.readParam = (ReadParam)selectCutFile.readParam.Clone();
                MenuController.owner.SetDeviceParam(ParseCommunicationCommand.readParam);
            }

            ParseCommunicationCommand.manualFeedLen = selectCutFile.manualFeedLength;
            ParseCommunicationCommand.feedOffset = selectCutFile.manualFeedOffsetLength;
            ParseCommunicationCommand.sectionLen = selectCutFile.manualFeedBreakLength;
            bool b = ParseCommunicationCommand.ExecuteManualFeedParam();
            //ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.manualFeedLen.ToString(), "ManualFeedLen");
            //ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.feedOffset.ToString(), "FeedOffset");
            //ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.sectionLen.ToString(), "SectionLen");

            data.Clear();
            if (ParseCommunicationCommand.sendCommandFlag != null)
                data.AddRange(ParseCommunicationCommand.sendCommandFlag);
            data.Add(0x14);
            //data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(selectCutFile.x)));
            //data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(selectCutFile.y)));
            data.AddRange((BitConverter.GetBytes(selectCutFile.x)));
            data.AddRange((BitConverter.GetBytes(selectCutFile.y)));
            bool isSuccess1 = ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "定位点坐标发送", 120);

            if (isSuccess1)
            {
                ParseCommunicationCommand.SaveOriginX = selectCutFile.x;
                ParseCommunicationCommand.SaveOriginY = selectCutFile.y;
            }
            //if (isSuccess1)
            //    MessageBox.Show("定位点坐标发送成功");
            //else
            //    MessageBox.Show("定位点坐标发送失败");


            writeSuccess = MenuController.owner.WriteProjectCutFile(selectCutFile.cutFileName);
            if (isSuccess)
            {
                if (selectCutFile.readParam != null)
                    ParseCommunicationCommand.readParam = (ReadParam)selectCutFile.readParam.Clone();
                //MenuController.owner.userSetParam1.InitialControl(ParseCommunicationCommand.readParam);
                MenuController.owner.SetDeviceParam(ParseCommunicationCommand.readParam);

                //ParseCommunicationCommand.cutNum = -1;
                MenuController.owner.UpdateState();
            }
            if (addFileForm != null)
                addFileForm.InitialSelectFile(selectCutFile);
            if (isSuccess && isSuccess1 && writeSuccess)
            {
                MessageBox.Show("操作成功");

                Form myParent = this.FindForm();
                myParent.Visible = false;
                MenuController.owner.MenuController.ShowMenu();
                //MenuController.owner.info_Form.Show();
            }
            else
            {
                string info = "";
                if (!isSuccess)
                {
                    info += "参数设置失败！";
                }
                if (!isSuccess1)
                {
                    info += "定位点设置失败！";
                }
                if (!writeSuccess)
                {
                    info += "文件写入失败！";
                }
                if (info.Length > 0)
                    MessageBox.Show(info);
            }
            ParseCommunicationCommand.IsReading = false;
        }

        private void btn_New_Click(object sender, EventArgs e)
        {
            if (newButtonForm == null)
            {
                newButtonForm = new frm_NewButton();
                newButtonForm.Owner = MenuController.owner;
                newButtonForm.Tag = "Visble-1";
                newButtonForm.userButtonPanel = this;
            }
            if (addFileForm == null)
            {
                addFileForm = new frm_AddFile();
                addFileForm.Owner = MenuController.owner;
                addFileForm.Tag = "Visble-1";
            }
            if (this.flowLayoutPanel1.Visible)
            {
                newButtonForm.InitialControl();
                newButtonForm.level = 1;
                newButtonForm.flowLayoutPanel = flowLayoutPanel1;
                newButtonForm.Location = new System.Drawing.Point(MenuController.owner.SplitPanel.Location.X + MenuController.owner.SplitPanel.Width / 2 - newButtonForm.Width / 2, (MenuController.owner.SplitPanel.Height - newButtonForm.Height) / 2 - 45);
                newButtonForm.ShowDialog();
            }
            else if (this.flowLayoutPanel2.Visible)
            {
                newButtonForm.InitialControl();
                newButtonForm.level = 2;
                newButtonForm.flowLayoutPanel = flowLayoutPanel2;
                newButtonForm.Location = new System.Drawing.Point(MenuController.owner.SplitPanel.Location.X + MenuController.owner.SplitPanel.Width / 2 - newButtonForm.Width / 2, (MenuController.owner.SplitPanel.Height - newButtonForm.Height) / 2 - 45);
                newButtonForm.ShowDialog();
            }
            else if (this.flowLayoutPanel3.Visible)
            {
                addFileForm.userButtonPanel = this;
                addFileForm.flowLayoutPanel = flowLayoutPanel3;

                addFileForm.productClass = selectProduct;
                addFileForm.InitialSelectFile(null);
                addFileForm.Location = new Point(MenuController.owner.SplitPanel.Location.X + MenuController.owner.SplitPanel.Width / 2 - addFileForm.Width / 2,
                    (MenuController.owner.SplitPanel.Height - addFileForm.Height) / 2 - 45);
                addFileForm.ShowDialog();
            }
        }

        private void btn_Delete_Click(object sender, EventArgs e)
        {
            try
            {
                if (selectButton != null)
                {
                    if (flowLayoutPanel1.Visible)
                    {
                        if (selectButton.Tag != null)
                        {
                            ProjectClass pc = (ProjectClass)selectButton.Tag;
                            flowLayoutPanel1.Controls.Remove(selectButton.Parent);
                            selectButton.Parent.Dispose();
                            ParseCommunicationCommand.projectClassList.Remove(pc);
                            string path1 = Application.StartupPath + "\\project\\" + pc.name;
                            if (Directory.Exists(path1))
                            {
                                Directory.Delete(path1, true);
                            }
                        }
                    }
                    else if (flowLayoutPanel2.Visible)
                    {
                        if (selectButton.Tag != null)
                        {
                            ProductClass pc = (ProductClass)selectButton.Tag;
                            flowLayoutPanel2.Controls.Remove(selectButton.Parent);
                            selectButton.Parent.Dispose();
                            pc.parent.products.Remove(pc);

                            string path1 = Application.StartupPath + "\\project\\" + pc.parent.name + "\\" + pc.name;
                            if (Directory.Exists(path1))
                            {
                                Directory.Delete(path1, true);
                            }
                        }
                    }
                    else if (flowLayoutPanel3.Visible)
                    {
                        if (selectButton.Tag != null)
                        {
                            CutFileClass cFile = (CutFileClass)selectButton.Tag;
                            flowLayoutPanel3.Controls.Remove(selectButton.Parent);
                            selectButton.Parent.Dispose();
                            cFile.parent.cutFiles.Remove(cFile);

                            string path1 = cFile.cutFileName.Substring(0, cFile.cutFileName.Length - 3) + "txt";
                            if (File.Exists(cFile.cutFileName))
                            {
                                File.Delete(cFile.cutFileName);
                            }
                            if (File.Exists(path1))
                            {
                                File.Delete(path1);
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "\r\n");
            }
        }

        private void btn_Edit_Click(object sender, EventArgs e)
        {
            if (newButtonForm == null)
            {
                newButtonForm = new frm_NewButton();
                newButtonForm.Owner = MenuController.owner;
                newButtonForm.Tag = "Visble-1";
                newButtonForm.userButtonPanel = this;
            }
            if (addFileForm == null)
            {
                addFileForm = new frm_AddFile();
                addFileForm.Owner = MenuController.owner;
                addFileForm.Tag = "Visble-1";
            }
            if (this.flowLayoutPanel1.Visible)
            {
                newButtonForm.InitialControl(selectProject);
                newButtonForm.level = 1;
                newButtonForm.flowLayoutPanel = flowLayoutPanel1;
                newButtonForm.Location = new System.Drawing.Point(MenuController.owner.SplitPanel.Location.X + MenuController.owner.SplitPanel.Width / 2 - newButtonForm.Width / 2, (MenuController.owner.SplitPanel.Height - newButtonForm.Height) / 2 - 45);
                newButtonForm.ShowDialog();
            }
            else if (this.flowLayoutPanel2.Visible)
            {
                newButtonForm.InitialControl(selectProduct);
                newButtonForm.level = 2;
                newButtonForm.flowLayoutPanel = flowLayoutPanel2;
                newButtonForm.Location = new System.Drawing.Point(MenuController.owner.SplitPanel.Location.X + MenuController.owner.SplitPanel.Width / 2 - newButtonForm.Width / 2, (MenuController.owner.SplitPanel.Height - newButtonForm.Height) / 2 - 45);
                newButtonForm.ShowDialog();
            }
            else if (this.flowLayoutPanel3.Visible)
            {
                addFileForm.userButtonPanel = this;
                addFileForm.flowLayoutPanel = flowLayoutPanel3;
                addFileForm.productClass = selectProduct;
                addFileForm.InitialSelectFile(selectCutFile);
                addFileForm.Location = new Point(MenuController.owner.SplitPanel.Location.X + MenuController.owner.SplitPanel.Width / 2 - addFileForm.Width / 2,
                    (MenuController.owner.SplitPanel.Height - addFileForm.Height) / 2 - 45);
                addFileForm.ShowDialog();
            }
        }

        private void btn_Next_EnabledChanged(object sender, EventArgs e)
        {
            //if (btn_Next.Enabled)
            //{
            //    btn_Next.BackColor = System.Drawing.SystemColors.ActiveCaption;
            //}
            //else
            //{
            //    btn_Next.BackColor = Color.Gray;
            //}
        }

        private void btn_Front_EnabledChanged(object sender, EventArgs e)
        {
            //if (btn_Front.Enabled)
            //{
            //    btn_Front.BackColor = System.Drawing.SystemColors.ActiveCaption;
            //}
            //else
            //{
            //    btn_Front.BackColor = Color.Gray;
            //}
        }

        private void pictureBox2_Click(object sender, EventArgs e)
        {
            this.FindForm().Visible = false;
            MenuController.owner.MenuController.ShowMenu();
            //MenuController.owner.info_Form.Show();
        }

        private void btn_UpPage_Click(object sender, EventArgs e)
        {

        }

        private void btn_DownPage_Click(object sender, EventArgs e)
        {

        }

        private void btn_Ok_MouseDown(object sender, MouseEventArgs e)
        {
            this.btn_Ok.BackgroundImage = Image.FromFile(Application.StartupPath + "\\icon\\gggg1.png");
        }

        private void btn_Ok_MouseUp(object sender, MouseEventArgs e)
        {
            this.btn_Ok.BackgroundImage = Image.FromFile(Application.StartupPath + "\\icon\\5756a1d774ff04.png");
        }

    }
}