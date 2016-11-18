using CommonModule;
using CommunicationModule;
using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class frm_Project : frm_BaseForm
    {
        public Form1 mainForm;
        public frm_SettingParam frmSetParam;
        public frm_SetOrigin frmSetOrigin;
        public frm_AddCutFile frmAddCutFile;
        //public string projectName;
        //public string productName;
        //public string projectIconName;
        //public string productIconName;
        //public string imageFileName;
        public static bool isProject = false;


        public frm_Project(Form1 owner)
        {
            InitializeComponent();
            mainForm = owner;
        }
        /// <summary>
        /// 保存
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_Save_Click(object sender, EventArgs e)
        {
            if (this.txt_ProjectName.Text.Trim().Length <= 0)
            {
                MessageBox.Show("项目名称不能为空！请输入", "提示！");
                txt_ProjectName.Focus();
                return;
            }
            if (this.txt_ProductName.Text.Trim().Length <= 0)
            {
                MessageBox.Show("产品名称不能为空！请输入", "提示！");
                txt_ProductName.Focus();
                return;
            }

            string path = Application.StartupPath + "\\project\\";
            if(!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
            string path1 = path + txt_ProjectName.Text + "\\";
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
            string path2 = path1 + txt_ProductName.Text + "\\";
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
            File.Copy(pic_ProjectView.Text, path1 + txt_ProjectName.Text + Path.GetExtension(pic_ProjectView.Text));
            File.Copy(pic_ProductView.Text, path2 + txt_ProductName.Text + Path.GetExtension(pic_ProductView.Text));
            
            File.Copy(btn_SelectFile.Text, path2 + Path.GetFileName(btn_SelectFile.Text));
            
            ParseCommunicationCommand.SaveConfig(path2);
        }
        /// <summary>
        /// 退出
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_Quit_Click(object sender, EventArgs e)
        {
            Visible = false;
        }
        /// <summary>
        /// 选择切割文件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_SelectFile_Click(object sender, EventArgs e)
        {
            if (frmAddCutFile == null)
            {
                frmAddCutFile = new frm_AddCutFile(this);
                frmAddCutFile.Owner = mainForm;
                frmAddCutFile.Tag = "Visble-1";
            }

            {
                //mainForm.ReadDeviceParam();
                frmAddCutFile.Location = new System.Drawing.Point(mainForm.SplitPanel.Location.X + mainForm.SplitPanel.Width / 2 - frmAddCutFile.Width / 2, (mainForm.SplitPanel.Height - frmAddCutFile.Height) / 2 - 45);
                frmAddCutFile.Show();
            }
            
        }
        /// <summary>
        /// 选择项目图标
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void pic_ProjectView_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Multiselect = false;
            openFile.Filter = "图片文件|*.jpg;*.png;*.bmp;*.gif";
            if (openFile.ShowDialog(mainForm) == DialogResult.OK)
            {
                //<data name="pic_ProductView.Image" type="System.Drawing.Bitmap, System.Drawing" mimetype="application/x-microsoft.net.object.bytearray.base64">
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
        }
        /// <summary>
        /// 选择产品图标
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void pic_ProductView_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Multiselect = false;
            openFile.Filter = "图片文件|*.jpg;*.png;*.bmp;*.gif";
            if (openFile.ShowDialog(mainForm) == DialogResult.OK)
            {
                //<data name="pic_ProductView.Image" type="System.Drawing.Bitmap, System.Drawing" mimetype="application/x-microsoft.net.object.bytearray.base64">
                try
                {
                    Bitmap bmp = new Bitmap(openFile.FileName);
                    this.pic_ProductView.Image = bmp;
                    this.pic_ProductView.Text = openFile.FileName;
                }
                catch (Exception ex)
                {
                    ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "pic_ProjectView_Click\r\n");
                }
            }
        }

        private void frm_Project_Load(object sender, EventArgs e)
        {

        }

        private void frm_Project_VisibleChanged(object sender, EventArgs e)
        {
            if(Visible)
            {
                if (mainForm.paintingPanel1.FilePath.Length > 0)
                    this.btn_SelectFile.Text = mainForm.paintingPanel1.FilePath;

            }
            else
            {
                if (frmSetOrigin != null && !frmSetOrigin.IsDisposed)
                    frmSetOrigin.Visible = false;
                if (frmSetParam != null && !frmSetParam.IsDisposed)
                    frmSetParam.Visible = false;
            }
        }
    }
}
