using CommonModule;
using CommunicationModule;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class frm_NewButton : frm_BaseForm
    {
        public int level = 0;
        public FlowLayoutPanel flowLayoutPanel;
        public UserButtonPanel userButtonPanel;
        Process p;

        public frm_NewButton()
        {
            InitializeComponent();
        }

        public void InitialControl()
        {
            this.txt_ProjectName.Text = "";
            isProject = false;
            isProduct = false;
        }
        ProjectClass projectClass;
        ProductClass productClass;
        bool isProject = false;
        bool isProduct = false;
        public void InitialControl(ProjectClass pc)
        {
            projectClass = pc;
            isProject = true;
            isProduct = false;
            this.txt_ProjectName.Text = pc.name;
            if(File.Exists(pc.iconName))
            {
                Bitmap bmp = new Bitmap(pc.iconName);
                this.pic_ProjectView.Image = bmp;
                this.pic_ProjectView.Text = pc.iconName;
            }
        }
        public void InitialControl(ProductClass pc)
        {
            productClass = pc;
            isProject = false;
            isProduct = true;
            this.txt_ProjectName.Text = pc.name;
            if (File.Exists(pc.iconName))
            {
                Bitmap bmp = new Bitmap(pc.iconName);
                this.pic_ProjectView.Image = bmp;
                this.pic_ProjectView.Text = pc.iconName;
            }
        }

        private void btn_Quit_Click(object sender, EventArgs e)
        {
            this.Visible = false;
        }

        private void pic_ProjectView_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Multiselect = false;
            openFile.Filter = "图片文件|*.jpg;*.png;*.bmp;*.gif";
            CloseOsk();
            if (openFile.ShowDialog(this) == DialogResult.OK)
            {
                try
                {
                    Bitmap bmp = new Bitmap(openFile.FileName);
                    this.pic_ProjectView.Image = bmp;
                    this.pic_ProjectView.Text = openFile.FileName;
                }
                catch (Exception ex)
                {
                    ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "pic_ProjectView_Click\r\n");
                }
            }
            //this.txt_ProjectName.Focus();
        }

        private void btn_Save_Click(object sender, EventArgs e)
        {
            string path = Application.StartupPath + "\\project\\";
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
            if (this.txt_ProjectName.Text.Trim().Length <= 0)
            {
                MessageBox.Show("项目名称不能为空！请输入", "提示！");
                txt_ProjectName.Focus();
                return;
            }
            if(level == 1)
            {
                if(isProject)
                {
                    string path2 = path + projectClass.name + "\\";
                    projectClass.name = txt_ProjectName.Text;
                    projectClass.iconName = pic_ProjectView.Text;
                    ButtonText btn = (ButtonText)userButtonPanel.selectButton.Parent;
                    btn.lbl_Text.Text = txt_ProjectName.Text;
                    btn.ButtonImage = pic_ProjectView.Text;
                    string path1 = path + txt_ProjectName.Text + "\\";
              
                    if (path2 != path1 && !Directory.Exists(path1))
                    {
                        try
                        {
                            ConvertType.CopyDir(path2, path1);
                            Directory.Delete(Directory.GetParent(path2).FullName,true);
                        }
                        catch (Exception ex)
                        {
                            
                        }
                    }
                    if (File.Exists(pic_ProjectView.Text))
                        File.Copy(pic_ProjectView.Text, path1 + Path.GetFileName(pic_ProjectView.Text), true);
                }
                else
                {
                    ProjectClass pc = new ProjectClass();
                    pc.name = txt_ProjectName.Text;
                    pc.iconName = pic_ProjectView.Text;
                    ParseCommunicationCommand.projectClassList.Add(pc);
                    userButtonPanel.AddButton(txt_ProjectName.Text, txt_ProjectName.Text, pc, flowLayoutPanel, this.pic_ProjectView.Text);
                    string path1 = path + txt_ProjectName.Text + "\\";
                    if (!Directory.Exists(path1))
                    {
                        Directory.CreateDirectory(path1);
                    }
                    if (pic_ProjectView.Text != null && pic_ProjectView.Text.Length > 0 && File.Exists(pic_ProjectView.Text))
                        File.Copy(pic_ProjectView.Text, path1 + Path.GetFileName(pic_ProjectView.Text), true);
                }
                
                
            }
            else if (level == 2)
            {
                if(isProduct)
                {
                    string path2 = path + productClass.parent.name + "\\" + productClass.name + "\\";
                    productClass.name = txt_ProjectName.Text;
                    productClass.iconName = pic_ProjectView.Text;
                    
                    ButtonText btn = (ButtonText)userButtonPanel.selectButton.Parent;
                    btn.lbl_Text.Text = txt_ProjectName.Text;
                    if (File.Exists(pic_ProjectView.Text))
                    {
                        btn.ButtonImage = pic_ProjectView.Text;
                    }
                    string path1 = path + productClass.parent.name + "\\" + txt_ProjectName.Text + "\\";

                    if (path2 != path1 && !Directory.Exists(path1))
                    {
                        try
                        {
                            ConvertType.CopyDir(path2, path1);
                            Directory.Delete(Directory.GetParent(path2).FullName, true);
                        }
                        catch (Exception ex)
                        {

                        }
                        //Directory.Move(path2, path1);
                    }
                    if (File.Exists(pic_ProjectView.Text))
                    {
                        File.Copy(pic_ProjectView.Text, path1 + Path.GetFileName(pic_ProjectView.Text), true);
                    }
                }
                else
                {
                    ProductClass pc = new ProductClass();
                    pc.name = txt_ProjectName.Text;
                    pc.iconName = pic_ProjectView.Text;
                    pc.parent = userButtonPanel.selectProject;
                    userButtonPanel.selectProject.products.Add(pc);
                    userButtonPanel.AddButton(txt_ProjectName.Text, txt_ProjectName.Text, pc, flowLayoutPanel, this.pic_ProjectView.Text);

                    string path1 = path + pc.parent.name + "\\";
                    if (!Directory.Exists(path1))
                    {
                        Directory.CreateDirectory(path);
                    }
                    string path2 = path1 + pc.name + "\\";
                    if (!Directory.Exists(path2))
                    {
                        Directory.CreateDirectory(path2);
                    }

                    if (pic_ProjectView.Text != null && pic_ProjectView.Text.Length > 0 && File.Exists(pic_ProjectView.Text)) 
                        File.Copy(pic_ProjectView.Text, path2 + Path.GetFileName(pic_ProjectView.Text), true);
                }
            }
            else if (level == 3)
            {

            }
            Visible = false;
            //userButtonPanel.AddButton(txt_ProjectName.Text, txt_ProjectName.Text, null, flowLayoutPanel, this.pic_ProjectView.Text);
        }

        private void txt_ProjectName_MouseClick(object sender, MouseEventArgs e)
        {
            if (p == null)
            {
                //p = Process.Start("C:\\Windows\\System32\\osk.exe");
                p = Process.Start(Application.StartupPath + "\\osk.exe");
            }
            Process[] pres = Process.GetProcessesByName("osk");
            if (pres.Length == 0)
                p.Start();

            this.txt_ProjectName.Focus();
        }

        private void frm_NewButton_VisibleChanged(object sender, EventArgs e)
        {
            if(!Visible)
            {
                CloseOsk();
            }
        }

        private static void CloseOsk()
        {
            Process[] pres = Process.GetProcessesByName("osk");
            foreach (Process item in pres)
            {
                item.Kill();
            }
        }
    }
}
