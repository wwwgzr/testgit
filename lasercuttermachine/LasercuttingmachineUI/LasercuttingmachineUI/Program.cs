using CommunicationModule;
using System;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {
            try
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Form1 frm1 = new Form1();
                if(!frm1.IsDisposed)
                {
                    Application.Run(frm1);
                }
            }
            catch(Exception ex)
            {
                ParseCommunicationCommand.WriteFileCommand(DateTime.Now.ToString("Main yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "\r\n");

                Console.WriteLine(ex.Message);
            }
        }
    }
}