using CommonModule;
using System;
using System.Drawing;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class frm_BaseForm : Form
    {
        /// <summary>
        /// 鼠标按下标识
        /// </summary>
        private bool isMouseDown = false;
        /// <summary>
        /// 鼠标按下的坐标点
        /// </summary>
        private Point mouseDownPoint;
        /// <summary>
        /// 窗体是否正在显示，显示时不发查询命令
        /// </summary>
        public static bool IsVisible = false;
        /// <summary>
        /// 窗体是否能拖动
        /// </summary>
        public bool IsCanMove = false;

        public frm_BaseForm()
        {
            InitializeComponent();
            
        }
        /// <summary>
        /// 鼠标移动
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void frm_BaseForm_MouseMove(object sender, MouseEventArgs e)
        {
            if (isMouseDown)
            {
                int x, y;
                if (Math.Abs(e.X - mouseDownPoint.X) > 0)
                {
                    x = e.X - mouseDownPoint.X;
                }
                else
                    x = 0;
                if (Math.Abs(e.Y - mouseDownPoint.Y) > 0)
                {
                    y = e.Y - mouseDownPoint.Y;
                }
                else
                    y = 0;

                this.Location = new Point(this.Location.X + x, this.Location.Y + y);
            }
        }
        /// <summary>
        /// 鼠标按下
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void frm_BaseForm_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                isMouseDown = true;
                if (mouseDownPoint == null)
                    mouseDownPoint = new Point(e.X, e.Y);
                else
                {
                    mouseDownPoint.X = e.X;
                    mouseDownPoint.Y = e.Y;
                }
            }

        }
        /// <summary>
        /// 鼠标抬起
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void frm_BaseForm_MouseUp(object sender, MouseEventArgs e)
        {
            isMouseDown = false;
        }


        private void frm_BaseForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            //AniWindowClass.AniWindow(this.Handle, 100, 0, this); 
        }

        private void frm_BaseForm_VisibleChanged(object sender, EventArgs e)
        {
            IsVisible = this.Visible;
            //if(this.Visible)
            //    AniWindowClass.AniWindow(this.Handle, 100, 1, this); 
            //else
            //    AniWindowClass.AniWindow(this.Handle, 100, 0, this); 

        }

        private void frm_BaseForm_Load(object sender, EventArgs e)
        {
            if (IsCanMove)
            {
                this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.frm_BaseForm_MouseDown);
                this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.frm_BaseForm_MouseMove);
                this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.frm_BaseForm_MouseUp);

                SetMouseHandler(this.Controls);
            }
        }

        private void SetMouseHandler(Control.ControlCollection controls)
        {
            foreach (Control item in controls)
            {
                item.MouseDown += new MouseEventHandler(frm_BaseForm_MouseDown);
                item.MouseUp += new MouseEventHandler(frm_BaseForm_MouseUp);
                item.MouseMove += new MouseEventHandler(frm_BaseForm_MouseMove);
                if (item.Controls.Count > 0)
                    SetMouseHandler(item.Controls);
            }
        }
    }
}