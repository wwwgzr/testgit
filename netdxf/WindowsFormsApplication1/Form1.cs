using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        public float maxX = float.MinValue;
        public float maxY = float.MinValue;
        public Form1()
        {
            InitializeComponent();
        }

        private void button2_Click(object sender, System.EventArgs e)
        {
            Dictionary<string, string> operators = new Dictionary<string, string>();
            Dictionary<string, string> operators1 = new Dictionary<string, string>();
            List<string> operators2 = new List<string>();

            string[] files = Directory.GetFiles(Application.StartupPath,"*.plt");
            foreach (string item in files)
            {
                FileInfo pltFile = new FileInfo(item);		//the sourceFile is set.

                StreamReader reader = null;
                reader = pltFile.OpenText();
                if (reader == null)
                {
                    return;
                }
                string str;
                while (!reader.EndOfStream)
                {
                    str = reader.ReadLine();
                    
                    string[] strlist = str.Split(new char[] { ';' }, System.StringSplitOptions.RemoveEmptyEntries);
                    foreach (string item1 in strlist)
                    {
                        if (item1.Length == 0)
                            continue;

                        if (item1.Length > 2)
                            operators2.Add(item1);
                    }
                }
                reader.Close();
                reader.Dispose();
            }
            GraphicsPath gp = new GraphicsPath();
            List<PointT> pointList = new List<PointT>();
            List<PointF> pointList1 = new List<PointF>();

            List<int> ptIndex = new List<int>();
            foreach (string item in operators2)
            {
                switch (item.Substring(0, 2))
                {
                    case "PU":
                        GetPoints(pointList, item);
                        break;
                    case "PD":
                        GetPoints(pointList, item);
                        break;
                    default:
                        break;
                }
            }

            Form2 frm2 = new Form2(pointList, maxX, maxY);
            frm2.Show();
        }

        private void GetPoints(List<PointT> pointList, string item)
        {
            string[] strlist = item.Substring(2, item.Length - 2).Split(new char[] { ',' }, System.StringSplitOptions.RemoveEmptyEntries);
            float x = 0;
            float y = 0;
            for (int i = 0; i < strlist.Length; )
            {
                x = float.Parse(strlist[i]);
                y = float.Parse(strlist[i + 1]);
                if (x > maxX)
                    maxX = x;
                if (y > maxY)
                    maxY = y;
                pointList.Add(new PointT(x, y, item.Substring(0, 2)));
                i += 2;
            }
        }



        private void btn_Send_Click(object sender, EventArgs e)
        {
            try
            {

                //char[] charar = new char[2] { '0', '1' };
                //byte[] byteArray = new byte[2];
                //byteArray[0] = 0;
                //byteArray[1] = 1;


                //byte[] byteArray = new byte[1];
                //byteArray[0] = 0x01;

                string str = this.textBox1.Text.Trim();
                byte[] byteArray = System.Text.Encoding.Default.GetBytes(str);

                int length = byteArray.Length;

               
                //CommonModule.CH375DLL.CloseDevice(usbid);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        private void btn_Accept_Click(object sender, EventArgs e)
        {
            try
            {
                byte[] btyA = new byte[120];
                int length = 120;
                this.textBox2.Text = Encoding.Default.GetString(btyA);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            try
            {

            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }

    public class PointT
    {
        public string name;
        public float X;
        public float Y;

        public PointT(float x,float y,string n)
        {
            X = x;
            Y = y;
            name = n;
        }
        public PointF ToPointF()
        {
            return new PointF(X, Y);
        }
        public Point ToPoint()
        {
            return new Point((int)X, (int)Y);
        }
    }
}
