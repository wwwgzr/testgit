using CommonModule;
using System;
using System.Drawing;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class frm_MenuPanel : Form
    {
        private int level;
        public frm_MenuPanel(int level1)
        {
            InitializeComponent();
            this.Width = 8;
            level = level1;
        }

        ///// <summary>
        ///// 添加按钮
        ///// </summary>
        ///// <param name="name">按钮名称</param>
        ///// <param name="text">按钮显示文字</param>
        ///// <param name="tag"></param>
        ///// <returns></returns>
        //public MenuButton AddButton(string name, string text, object tag)
        //{
        //    this.Width += 68;
        //    MenuButton mbtn_test = new MenuButton();
        //    mbtn_test.BackColor = System.Drawing.SystemColors.ButtonShadow;
        //    mbtn_test.FlatAppearance.BorderSize = 0;
        //    mbtn_test.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
        //    mbtn_test.Image = global::LasercuttingmachineUI.Properties.Resources.timg;
        //    mbtn_test.Location = new System.Drawing.Point(this.Width - 68, 4);
        //    mbtn_test.Name = name;
        //    mbtn_test.Size = new System.Drawing.Size(64, 64);
        //    mbtn_test.Text = text;
        //    mbtn_test.UseVisualStyleBackColor = false;
        //    this.Controls.Add(mbtn_test);
        //    return mbtn_test;
        //}

        /// <summary>
        /// 添加按钮
        /// </summary>
        /// <param name="name">按钮名称</param>
        /// <param name="text">按钮显示文字</param>
        /// <param name="tag"></param>
        /// <returns></returns>
        public MenuButton AddButton1(string name, string text, object tag,bool visible = true)
        {
            this.Width += 64 + 8;
            MenuButton mbtn_test = new MenuButton();
            mbtn_test.BackColor = Color.DarkGray;
            mbtn_test.FlatAppearance.BorderSize = 0;
            mbtn_test.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            mbtn_test.Location = new System.Drawing.Point(this.Width - 64 - 8, 4);
            mbtn_test.Name = name;
            mbtn_test.Size = new System.Drawing.Size(64, 64);
            mbtn_test.Text = text;
            mbtn_test.UseVisualStyleBackColor = false;
            mbtn_test.Click += mbtn_test_Click;
            mbtn_test.Visible = visible;
            this.Controls.Add(mbtn_test);
            return mbtn_test;
        }

        void mbtn_test_Click(object sender, EventArgs e)
        {
            MenuButton btn = (MenuButton)sender;
            btn.BackColor = Color.Lime;
            for (int i = 0; i < this.Controls.Count; i++)
            {
                if (this.Controls[i] is MenuButton && btn != this.Controls[i])
                {
                    this.Controls[i].BackColor = Color.DarkGray;
                }
            }
        }
        private void frm_MenuPanel_VisibleChanged(object sender, EventArgs e)
        {
            if(this.Visible)
            {

            }
            else
            {
                for (int i = 0; i < this.Controls.Count; i++)
                {
                    if (this.Controls[i] is MenuButton)
                    {
                        this.Controls[i].BackColor = Color.DarkGray;
                    }
                }
            }
            //if (this.Visible)
            //    AniWindowClass.AniWindow(this.Handle, 5, 1, this, 5000);
            //else
            //    AniWindowClass.AniWindow(this.Handle, 5, 0, this, 5000); 
        }

        private void frm_MenuPanel_FormClosed(object sender, FormClosedEventArgs e)
        {
            //AniWindowClass.AniWindow(this.Handle, 5, 0, this, 5000); 
        }

        public void ResetButtonLocation()
        {
            for (int i = 0; i < this.Controls.Count; i++)
            {
                if (this.Controls[i] is MenuButton && this.Controls[i].Visible)
                {
                    this.Width = 8+ (64 + 8) * (i + 1);
                    this.Controls[i].Location = new System.Drawing.Point(this.Width - 64 - 8, 4);
                    //this.Controls[i].BackColor = Color.DarkGray;
                }
            }

        }
    }
}