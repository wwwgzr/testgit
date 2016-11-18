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
    public partial class frm_AddCutFile : frm_BaseForm
    {
        public List<CutFileClass> cutFiles = new List<CutFileClass>();
        public CutFileClass selectFile;
        public frm_Project projectForm;
        public frm_AddCutFile(frm_Project parentFrom)
        {
            InitializeComponent();
            projectForm = parentFrom;
        }

        private void btn_Save_Click(object sender, EventArgs e)
        {

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
                    CutFileClass file = new CutFileClass();
                    file.cutFileName = openFile.FileName;
                    cutFiles.Add(file);
                    selectFile = file;
                    AddButton(Path.GetFileName(file.cutFileName), Path.GetFileName(file.cutFileName), file, flowLayoutPanel1, "");
                    //this.btn_SelectFile.Text = openFile.FileName;
                }
                else if (fileExt.ToLower() == ".plt" && paintingPanel1.ReadPltFile(openFile.FileName))
                {
                    CutFileClass file = new CutFileClass();
                    file.cutFileName = openFile.FileName;
                    cutFiles.Add(file);
                    selectFile = file;
                    AddButton(Path.GetFileName(file.cutFileName), Path.GetFileName(file.cutFileName), file, flowLayoutPanel1, "");
                    //this.btn_SelectFile.Text = openFile.FileName;
                }
            }
        }

        private void btn_Delete_Click(object sender, EventArgs e)
        {

        }
        /// <summary>
        /// 设置参数
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_SetParam_Click(object sender, EventArgs e)
        {
            if (projectForm.frmSetParam == null)
            {
                projectForm.frmSetParam = new frm_SettingParam();
                projectForm.frmSetParam.Owner = projectForm.mainForm;
                projectForm.frmSetParam.Tag = "Visble-1";
            }

            {
                frm_Project.isProject = true;
                if (selectFile != null)
                {
                    projectForm.frmSetParam.path = selectFile.cutFileName;
                    //projectForm.frmSetParam.addCutFileForm = this;
                }
                projectForm.mainForm.ReadDeviceParam();
                projectForm.frmSetParam.Location = new System.Drawing.Point(
                    projectForm.mainForm.SplitPanel.Location.X + projectForm.mainForm.SplitPanel.Width / 2 - projectForm.frmSetParam.Width / 2,
                    (projectForm.mainForm.SplitPanel.Height - projectForm.frmSetParam.Height) / 2 - 45);
                //frmSetParam.Location = new System.Drawing.Point(mainForm.SplitPanel.Location.X + mainForm.SplitPanel.Width / 2 - frmSetParam.Width / 2, 10);
                projectForm.frmSetParam.Show();
            }
        }
        /// <summary>
        /// 设置定位点
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_SetOrigin_Click(object sender, EventArgs e)
        {
            if (projectForm.frmSetOrigin == null)
            {
                projectForm.frmSetOrigin = new frm_SetOrigin();
                projectForm.frmSetOrigin.Owner = projectForm.mainForm;
                projectForm.frmSetOrigin.Tag = "Visble-1";
            }

            {
                //mainForm.ReadDeviceParam();
                if (selectFile != null)
                {
                    projectForm.frmSetOrigin.path = selectFile.cutFileName;
                    //projectForm.frmSetOrigin.addCutFileForm = this;
                }
                frm_Project.isProject = true;
                projectForm.frmSetOrigin.Location = new System.Drawing.Point(
                    projectForm.mainForm.SplitPanel.Location.X + projectForm.mainForm.SplitPanel.Width / 2 - projectForm.frmSetOrigin.Width / 2,
                    (projectForm.mainForm.SplitPanel.Height - projectForm.frmSetOrigin.Height) / 2 - 45);
                projectForm.frmSetOrigin.Show();
            }
        }
        
        private void btn_ExportParam_Click(object sender, EventArgs e)
        {

        }


        public Button AddButton(string name, string text, object tag, Panel owner, string iconfile, string cutFile = "", bool visible = true)
        {
            Button mbtn_test = new Button();
            mbtn_test.Name = name;
            mbtn_test.Text = text;
            if (iconfile.Length > 0)
            {
                try
                {
                    Bitmap bmp = new Bitmap(iconfile);
                    mbtn_test.Image = bmp;
                }
                catch (Exception ex)
                {

                }
            }
            mbtn_test.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            mbtn_test.Size = new System.Drawing.Size(95, 95);
            mbtn_test.Tag = tag;
            mbtn_test.Click += mbtn_test_Click;
            mbtn_test.Visible = visible;
            owner.Controls.Add(mbtn_test);
            return mbtn_test;
        }
        private void mbtn_test_Click(object sender, EventArgs e)
        {
            Button btn = (Button)sender;
            selectFile = (CutFileClass)btn.Tag;
            string fileExt = Path.GetExtension(selectFile.cutFileName);
            if (fileExt.ToLower() == ".dxf" && paintingPanel1.OpenDxfFile(selectFile.cutFileName))
            {
            }
            else if (fileExt.ToLower() == ".plt" && paintingPanel1.ReadPltFile(selectFile.cutFileName))
            {
            }
        }

        private void btn_Quit_Click_1(object sender, EventArgs e)
        {
            this.Visible = false;
        }

        private void frm_AddCutFile_VisibleChanged(object sender, EventArgs e)
        {
            selectFile = null;
            if (Visible)
            {
                
            }
            else
            {

            }
        }
    }
}
