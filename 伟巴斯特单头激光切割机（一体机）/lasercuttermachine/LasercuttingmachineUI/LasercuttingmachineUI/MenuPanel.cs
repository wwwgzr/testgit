using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class MenuPanel : UserControl
    {
        private int level;
        Bitmap bitmap1;
        Bitmap bitmap2;
        public MenuPanel(int level1)
        {
            InitializeComponent();
            this.Visible = false;
            level = level1;
            bitmap1 = new Bitmap(Application.StartupPath + "\\icon\\rrrr.png");
            bitmap2 = new Bitmap(Application.StartupPath + "\\icon\\sss.png");
        }
        public MenuPanel()
        {
            InitializeComponent();
            this.Visible = false;
            level = 1;
            bitmap1 = new Bitmap(Application.StartupPath + "\\icon\\rrrr.png");
            bitmap2 = new Bitmap(Application.StartupPath + "\\icon\\sss.png");
        }

        public void Show()
        {
            this.Visible = true;
        }

        public void Hide()
        {
            this.Visible = false;
        }
        /// <summary>
        /// 添加按钮
        /// </summary>
        /// <param name="name">按钮名称</param>
        /// <param name="text">按钮显示文字</param>
        /// <param name="tag"></param>
        /// <returns></returns>
        public MenuButton AddButton1(string name, string text, object tag, bool visible = true)
        {
            this.Width += 64 + 8;
            MenuButton mbtn_test = new MenuButton();

            mbtn_test.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            mbtn_test.FlatAppearance.BorderSize = 0;
            mbtn_test.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Transparent;
            mbtn_test.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Transparent;
            mbtn_test.FlatStyle = System.Windows.Forms.FlatStyle.Flat;

            mbtn_test.Location = new System.Drawing.Point(this.Width - 64 - 8, 4);
            mbtn_test.Name = name;
            mbtn_test.Size = new System.Drawing.Size(64, 64);
            mbtn_test.Text = text;
            mbtn_test.UseVisualStyleBackColor = false;
            mbtn_test.Click += mbtn_test_Click;
            mbtn_test.Visible = visible;
            //Bitmap bitmap3;
            //switch(name)
            //{
            //    case "btn_CutProject":
            //        mbtn_test.Text = "";
            //        bitmap3 = new Bitmap(Application.StartupPath + "\\icon\\切割项目.png");
            //        mbtn_test.BackgroundImage = bitmap3;
            //        break;
            //    case "btn_SelectFile":
            //        mbtn_test.Text = "";
            //        bitmap3 = new Bitmap(Application.StartupPath + "\\icon\\切割文件.png");
            //        mbtn_test.BackgroundImage = bitmap3;
            //        break;
            //    case "btn_MoveFrame":
            //        mbtn_test.Text = "";
            //        bitmap3 = new Bitmap(Application.StartupPath + "\\icon\\移框.png");
            //        mbtn_test.BackgroundImage = bitmap3;
            //        break;
            //    case "btn_SysTest":
            //        mbtn_test.Text = "";
            //        bitmap3 = new Bitmap(Application.StartupPath + "\\icon\\上料调试.png");
            //        mbtn_test.BackgroundImage = bitmap3;
            //        break;
            //    case "btn_FeedMode":
            //        mbtn_test.Text = "";
            //        bitmap3 = new Bitmap(Application.StartupPath + "\\icon\\系统测试.png");
            //        mbtn_test.BackgroundImage = bitmap3;
            //        break;
            //    case "btn_WorkMenu":
            //        mbtn_test.Text = "";
            //        bitmap3 = new Bitmap(Application.StartupPath + "\\icon\\工作菜单.png");
            //        mbtn_test.BackgroundImage = bitmap3;
            //        break;
            //    case "btn_Exit":
            //        mbtn_test.Text = "";
            //        bitmap3 = new Bitmap(Application.StartupPath + "\\icon\\退出系统.png");
            //        mbtn_test.BackgroundImage = bitmap3;
            //        break;
            //    default:
            //        mbtn_test.BackgroundImage = bitmap1;
            //        break;
            //}
            mbtn_test.BackgroundImage = bitmap1;
            mbtn_test.BackgroundImageLayout = ImageLayout.Stretch;
            this.Controls.Add(mbtn_test);
            return mbtn_test;
        }

        void mbtn_test_Click(object sender, EventArgs e)
        {
            MenuButton btn = (MenuButton)sender;
            btn.BackgroundImage = bitmap2;

            for (int i = 0; i < this.Controls.Count; i++)
            {
                if (this.Controls[i] is MenuButton && btn != this.Controls[i])
                {
                    this.Controls[i].BackgroundImage = bitmap1;
                }
            }
        }
    }
}
