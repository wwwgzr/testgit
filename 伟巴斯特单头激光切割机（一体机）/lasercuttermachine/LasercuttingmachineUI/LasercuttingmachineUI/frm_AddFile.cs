using CommunicationModule;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class frm_AddFile : frm_BaseForm
    {
        public CutFileClass file;
        public UserButtonPanel userButtonPanel;
        private frm_SetOrigin setOriginForm;
        private frm_SettingParam settingParamForm;
        public ProductClass productClass;
        public FlowLayoutPanel flowLayoutPanel;
        private string oldFile = "";
        public frm_AddFile()
        {
            InitializeComponent();
        }

        public void InitialSelectFile(CutFileClass selectFile)
        {
            if (selectFile != null)
            {
                oldFile = selectFile.cutFileName;
                file = selectFile;
                string fileExt = Path.GetExtension(selectFile.cutFileName);
                if (fileExt.ToLower() == ".dxf" && paintingPanel1.OpenDxfFile(selectFile.cutFileName))
                {
                    this.label1.Text = Path.GetFileName(selectFile.cutFileName);
                }
                else if (fileExt.ToLower() == ".plt" && paintingPanel1.ReadPltFile(selectFile.cutFileName))
                {
                    this.label1.Text = Path.GetFileName(selectFile.cutFileName);
                }
            }
            else
            {
                oldFile = "";
                paintingPanel1.RemoveAll();
                label1.Text = "";
                file = new CutFileClass();
            }
        }
        private void btn_Add_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Multiselect = false;
            openFile.Filter = "dxf文件;plt文件|*.dxf;*.plt";
            if (openFile.ShowDialog(this) == DialogResult.OK)
            {
                string fileExt = Path.GetExtension(openFile.FileName);
                if (fileExt.ToLower() == ".dxf" && paintingPanel1.OpenDxfFile(openFile.FileName))
                {
                    file.cutFileName = openFile.FileName;
                    MenuController.owner.paintingPanel1.OpenDxfFile(openFile.FileName);
                    this.label1.Text = Path.GetFileName(openFile.FileName);
                }
                else if (fileExt.ToLower() == ".plt" && paintingPanel1.ReadPltFile(openFile.FileName))
                {
                    file.cutFileName = openFile.FileName;
                    MenuController.owner.paintingPanel1.ReadPltFile(openFile.FileName);
                    this.label1.Text = Path.GetFileName(openFile.FileName);
                }
            }
        }

        /// <summary>
        /// 设置参数
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_SetParam_Click(object sender, EventArgs e)
        {
            if (settingParamForm == null)
            {
                settingParamForm = new frm_SettingParam();
                settingParamForm.Owner = MenuController.owner;
                settingParamForm.Tag = "Visble-1";
            }

            {
                if (file.readParam == null)
                    file.readParam = new ReadParam();
                //MenuController.owner.ReadDeviceParam();
                //settingParamForm.newParam = ParseCommunicationCommand.readParam;
                //settingParamForm.InitialControl(ParseCommunicationCommand.readParam);
                if (ParseCommunicationCommand.readParam != null)
                {
                    file.readParam.deviceTime = ParseCommunicationCommand.readParam.deviceTime;
                    file.readParam.gateWay = ParseCommunicationCommand.readParam.gateWay;
                    file.readParam.deviceType = ParseCommunicationCommand.readParam.deviceType;
                    file.readParam.ipAddress = ParseCommunicationCommand.readParam.ipAddress;
                    file.readParam.subNetMask = ParseCommunicationCommand.readParam.subNetMask;
                    file.readParam.versionNum = ParseCommunicationCommand.readParam.versionNum;
                }
                settingParamForm.newParam = file.readParam;

                settingParamForm.InitialControl(file.readParam);

                settingParamForm.addCutFileForm = this;
                settingParamForm.path = file.cutFileName;
                frm_Project.isProject = true;
                settingParamForm.Location = new System.Drawing.Point(
                    MenuController.owner.SplitPanel.Location.X + MenuController.owner.SplitPanel.Width / 2 - settingParamForm.Width / 2,
                    (MenuController.owner.SplitPanel.Height - settingParamForm.Height) / 2 - 45);
                settingParamForm.ShowDialog();
            }
        }
        /// <summary>
        /// 设置定位点
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_SetOrigin_Click(object sender, EventArgs e)
        {
            if (setOriginForm == null)
            {
                setOriginForm = new frm_SetOrigin();
                setOriginForm.Owner = MenuController.owner;
                setOriginForm.Tag = "Visble-1";
            }

            {
                setOriginForm.addCutFileForm = this;
                setOriginForm.path = file.cutFileName;
                frm_Project.isProject = true;
                setOriginForm.Location = new System.Drawing.Point(
                    MenuController.owner.SplitPanel.Location.X + MenuController.owner.SplitPanel.Width / 2 - setOriginForm.Width / 2,
                    (MenuController.owner.SplitPanel.Height - setOriginForm.Height) / 2 - 45);
                setOriginForm.ShowDialog();
            }
        }

        private void btn_ExportParam_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Multiselect = false;
            openFile.Filter = "txt文件|*.txt;";
            if (openFile.ShowDialog(this) == DialogResult.OK)
            {
                CutFileClass t = ParseCommunicationCommand.ReadConfigCutFile(openFile.FileName);
                file.readParam = (ReadParam)t.readParam.Clone();
                file.x = t.x;
                file.y = t.y;
            }
        }

        private void btn_Delete_Click(object sender, EventArgs e)
        {

        }

        private void btn_Save_Click(object sender, EventArgs e)
        {
            if (file.readParam == null)
            {
                MessageBox.Show("请设置切割文件参数", "提示！");
                return;
            }
            if (file.cutFileName == null || file.cutFileName.Trim().Length == 0)
            {
                MessageBox.Show("请选择切割文件", "提示！");
                return;
            }
            string path = Application.StartupPath + "\\project\\";
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
            string path1 = path + productClass.parent.name + "\\";
            if (!Directory.Exists(path1))
            {
                Directory.CreateDirectory(path1);
            }
            string path2 = path1 + productClass.name + "\\";
            if (!Directory.Exists(path2))
            {
                Directory.CreateDirectory(path2);
            }
            string path3 = path2 + Path.GetFileName(file.cutFileName);
            if (oldFile != file.cutFileName && oldFile.Length > 0)
            {
                if (File.Exists(oldFile))
                {
                    File.Delete(oldFile);
                }
                string oldConfig = oldFile.Substring(0, oldFile.Length - 3) + ".txt";
                if (File.Exists(oldConfig))
                {
                    File.Delete(oldConfig);
                }
            }
            if (file.cutFileName != path3)
                File.Copy(file.cutFileName, path3, true);

            ParseCommunicationCommand.SaveConfigCutFile(file, path3);
            CutFileClass t = new CutFileClass();
            t.cutFileName = path3;
            t.parent = productClass;
            t.readParam = (ReadParam)file.readParam.Clone();
            t.x = file.x;
            t.y = file.y;

            if (oldFile == "")
            {
                userButtonPanel.AddButton(Path.GetFileName(t.cutFileName), Path.GetFileName(t.cutFileName), t, flowLayoutPanel, "", t.cutFileName);
                productClass.cutFiles.Add(t);
            }
            else
            {
                userButtonPanel.selectCutFile = t;
                ButtonText bt = (ButtonText)userButtonPanel.selectButton.Parent;
                bt.lbl_Text.Text = Path.GetFileName(t.cutFileName);
                if (t.cutFileName.Length > 0)
                {
                    if (t.cutFileName.EndsWith(".dxf"))
                    {
                        userButtonPanel.paintingPanel2.OpenDxfFile(t.cutFileName);
                    }
                    else
                    {
                        userButtonPanel.paintingPanel2.ReadPltFile(t.cutFileName);
                    }

                    Bitmap bmp = new Bitmap(160, 160);
                    userButtonPanel.paintingPanel2.DrawToBitmap(bmp, userButtonPanel.paintingPanel2.ClientRectangle);
                    bt.btn_Icon.BackgroundImage = bmp;
                }
            }
            this.Visible = false;
        }

        private void btn_Quit_Click(object sender, EventArgs e)
        {
            this.Visible = false;
        }

        private void frm_AddFile_VisibleChanged(object sender, EventArgs e)
        {
            if (Visible)
            {
                //file = new CutFileClass();
            }
            else
            {
                file = null;
            }
        }
    }

    public class frm_Project
    {
        public static bool isProject = false;
    }
}
