using System;
using System.Collections.Generic;
using System.Text;


namespace CommonModule
{
    /// <summary>
    /// 调用api窗口动画
    /// </summary>
    public static class AniWindowClass
    {
        /// <summary>
        /// 窗口动画
        /// </summary>
        /// <param name="hwnd">窗体指针</param>
        /// <param name="dwTime">时间 ms</param>
        /// <param name="dwFlags">动画标识</param>
        /// <returns></returns>
        [System.Runtime.InteropServices.DllImport("user32")]
        private static extern bool AnimateWindow(IntPtr hwnd, int dwTime, int dwFlags);
        private const int AW_HOR_POSITIVE = 0x0001;
        private const int AW_HOR_NEGATIVE = 0x0002;
        private const int AW_VER_POSITIVE = 0x0004;
        private const int AW_VER_NEGATIVE = 0x0008;
        private const int AW_CENTER = 0x0010;
        private const int AW_HIDE = 0x10000;
        private const int AW_ACTIVATE = 0x20000;
        private const int AW_SLIDE = 0x40000;
        private const int AW_BLEND = 0x80000;
        private static int CloseOpen = 0x20000;
        /// <summary>
        /// 窗口动画
        /// </summary>
        /// <param name="hwnd">窗体指针</param>
        /// <param name="dwFlags">动画标识</param>
        //0:普通显示
        //1:从左向右显示
        //2:从右向左显示
        //3:从上到下显示
        //4:从下到上显示
        //5:透明渐变显示
        //6:从中间向四周
        //7:左上角伸展
        //8:左下角伸展
        //9:右上角伸展
        //10:右下角伸展
        //100:随机
        /// <param name="CloseOrOpen">0是关闭  1是打开</param>
        /// <param name="myform">窗体对象</param>
        public static void AniWindow(IntPtr hwnd, int dwFlags, int CloseOrOpen, System.Windows.Forms.Form myform,int time = 200)
        {
            try
            {
                if (CloseOrOpen == 1)
                {
                    foreach (System.Windows.Forms.Control mycontrol in myform.Controls)
                    {
                        string m = mycontrol.GetType().ToString();
                        m = m.Substring(m.Length - 5);
                        if (m == "Label")
                        {
                            mycontrol.Visible = false;
                        }
                    }
                }
                //打开or关闭 0是关闭  1是打开
                if (CloseOrOpen == 0) { CloseOpen = 0x10000; }
                if (dwFlags == 100)
                {
                    int zz = 6;
                    Random a = new Random();
                    dwFlags = (int)a.Next(zz);
                }


                switch (dwFlags)
                {
                    case 0://普通显示
                        AnimateWindow(hwnd, time, AW_ACTIVATE);
                        break;
                    case 1://从左向右显示
                        AnimateWindow(hwnd, time, AW_HOR_POSITIVE | CloseOpen);
                        break;
                    case 2://从右向左显示
                        AnimateWindow(hwnd, time, AW_HOR_NEGATIVE | CloseOpen);
                        break;
                    case 3://从上到下显示
                        AnimateWindow(hwnd, time, AW_VER_POSITIVE | CloseOpen);
                        break;
                    case 4://从下到上显示
                        AnimateWindow(hwnd, time, AW_VER_NEGATIVE | CloseOpen);
                        break;
                    case 5://透明渐变显示
                        AnimateWindow(hwnd, time, AW_BLEND | CloseOpen);
                        break;
                    case 6://从中间向四周
                        AnimateWindow(hwnd, time, AW_CENTER | CloseOpen);
                        break;
                    //case 7://左上角伸展
                    //    AnimateWindow(hwnd, time, AW_SLIDE | AW_HOR_POSITIVE | AW_VER_POSITIVE | CloseOpen);
                    //    break;
                    //case 8://左下角伸展
                    //    AnimateWindow(hwnd, time, AW_SLIDE | AW_HOR_POSITIVE | AW_VER_NEGATIVE | CloseOpen);
                    //    break;
                    //case 9://右上角伸展
                    //    AnimateWindow(hwnd, time, AW_SLIDE | AW_HOR_NEGATIVE | AW_VER_POSITIVE | CloseOpen);
                    //    break;
                    //case 10://右下角伸展
                    //    AnimateWindow(hwnd, time, AW_SLIDE | AW_HOR_NEGATIVE | AW_VER_NEGATIVE | CloseOpen);
                    //    break;
                }
                if (CloseOrOpen == 1)
                {
                    foreach (System.Windows.Forms.Control mycontrol in myform.Controls)
                    {
                        string m = mycontrol.GetType().ToString();
                        m = m.Substring(m.Length - 5);
                        if (m == "Label")
                        {
                            mycontrol.Visible = true;
                        }
                    }
                }
            }
            catch { }
        }
    }
}
