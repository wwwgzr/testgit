
using System;
using System.IO;
namespace CommonModule
{
    public class ConvertType
    {
        //Convert.ToInt32("0x" + handle, 16);
        //b.ToString("x")
        #region string 转 基本数据类型
        /// <summary>
        /// string转byte
        /// </summary>
        /// <param name="s"></param>
        /// <returns></returns>
        public static byte ConvertByte(string s)
        {
            byte value;
            if (byte.TryParse(s, out value))
            {
                return value;
            }
            else
                return byte.MinValue;
        }
        /// <summary>
        /// string转short
        /// </summary>
        /// <param name="s"></param>
        /// <returns></returns>
        public static short ConvertShort(string s)
        {
            short value;
            if (short.TryParse(s, out value))
            {
                return value;
            }
            else
                return short.MinValue;
        }
        /// <summary>
        /// string转int
        /// </summary>
        /// <param name="s"></param>
        /// <returns></returns>
        public static int ConvertInt(string s)
        {
            int value;
            if (int.TryParse(s, out value))
            {
                return value;
            }
            else
                return int.MinValue;
        }
        /// <summary>
        /// string转float
        /// </summary>
        /// <param name="s"></param>
        /// <returns></returns>
        public static float ConvertFloat(string s)
        {
            float value;
            if (float.TryParse(s, out value))
            {
                return value;
            }
            else
                return float.MinValue;
        }
        #endregion

        #region byte数组转基本类型
        /// <summary>
        /// byte数组转short
        /// </summary>
        /// <param name="data"></param>
        /// <param name="index"></param>
        /// <returns></returns>
        public static short ToShort(byte[] data, ref int index)
        {
            byte[] data1 = CopyToArray(data,ref index, 2);
            return BitConverter.ToInt16(Reverse(data1), 0);
        }
        /// <summary>
        /// byte数组转int
        /// </summary>
        /// <param name="data"></param>
        /// <param name="index"></param>
        /// <returns></returns>
        public static int ToInt32(byte[] data, ref int index)
        {
            byte[] data1 = CopyToArray(data,ref index, 4);
            return BitConverter.ToInt32(Reverse(data1), 0);
        }

        /// <summary>
        /// byte数组转int
        /// </summary>
        /// <param name="data"></param>
        /// <param name="index"></param>
        /// <returns></returns>
        public static int ToInt32R(byte[] data, ref int index)
        {
            byte[] data1 = CopyToArray(data, ref index, 4);
            return BitConverter.ToInt32(data1, 0);
        }

        /// <summary>
        /// byte数组转float
        /// </summary>
        /// <param name="data"></param>
        /// <param name="index"></param>
        /// <returns></returns>
        public static float ToFloat(byte[] data, ref int index)
        {
            byte[] data1 = CopyToArray(data,ref index, 4);

            return BitConverter.ToSingle(Reverse(data1), 0);
        }
        /// <summary>
        /// byte数组转DateTime
        /// </summary>
        /// <param name="data"></param>
        /// <param name="index"></param>
        /// <returns></returns>
        public static DateTime ToDateTime(byte[] data, ref int index)
        {
            byte[] data1 = CopyToArray(data,ref index, 8);
            return DateTime.FromBinary(BitConverter.ToInt64(data1, 0));
        }
        /// <summary>
        /// j截取byte数组
        /// </summary>
        /// <param name="data"></param>
        /// <param name="index"></param>
        /// <returns></returns>
        public static byte[] CopyToArray(byte[] data,ref int index, int len)
        {
            try
            {
                byte[] data1 = new byte[len];
                for (int i = 0; i < data1.Length; i++)
                {
                    if (index + i >= data.Length)
                        break;
                    data1[i] = data[index + i];
                }
                index = index + len;

                //string str = "";
                //for (int i = 0; i < data1.Length; i++)
                //{
                //    str += "[" + data1[i].ToString("x2") + "]";
                //}
                //Console.WriteLine(str);
                return data1;
            }
            catch (Exception ex)
            {
                WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "\r\n");
                return null;
            }
        }
        /// <summary>
        /// byte转位字符数组
        /// </summary>
        /// <param name="b"></param>
        /// <returns></returns>
        public static Char[] ToBit(byte b)
        {
            string Str_Value = "00000000" + Convert.ToString(b, 2);
            return Str_Value.ToCharArray(Str_Value.Length - 8, 8);
        }
        /// <summary>
        /// byte数组转IP
        /// </summary>
        /// <param name="data"></param>
        /// <param name="index"></param>
        /// <returns></returns>
        public static string ToIpAddress(byte[] data)
        {
            if (data != null && data.Length >= 4)
            {
                return data[0].ToString() + "." + data[1].ToString() + "." + data[2].ToString() + "." + data[3].ToString();
                //return data[3].ToString() + "." + data[2].ToString() + "." + data[1].ToString() + "." + data[0].ToString();
            }
            else
                return "";
        }

        /// <summary>
        /// byte数组转Version 版本号V1.00
        /// </summary>
        /// <param name="data"></param>
        /// <param name="index"></param>
        /// <returns></returns>
        public static string ToVersion(byte[] data)
        {
            if (data != null && data.Length >= 4)
            {
                return "V" + data[0].ToString() + (char)data[1] + data[2].ToString() + data[3].ToString();
                //return data[3].ToString() + "." + data[2].ToString() + "." + data[1].ToString() + "." + data[0].ToString();
            }
            else
                return "";
        }
        #endregion

        /// <summary>
        /// ip地址转byte数组
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public static byte[] IpAddressToByteArray(string value)
        {
            if (value == null)
                return null;
            string[] str = value.Split('.');
            byte[] data = new byte[4];
            for (int i = 0; i < 4; i++)
            {
                if (str.Length > i)
                {
                    data[i] = ConvertByte(str[i]);
                }
                else
                    data[i] = 0;
            }
            return data;
        }

        public static string DeviceTimeToString(int value)
        {
            string str = value.ToString();
            if (str.Length == 8)
                return str.Substring(0, 4) + "年" + str.Substring(4, 2) + "月" + str.Substring(6, 2) + "日";
            return str;
        }

        public static DateTime DeviceTimeToDateTime(int value)
        {
            string str = value.ToString();
            if (str.Length == 8)
            {
                int y = 0;
                int m = 0;
                int d = 0;
                if(int.TryParse(str.Substring(0, 4),out y) && int.TryParse(str.Substring(4,2),out m) && int.TryParse(str.Substring(6,2),out d))
                {
                    DateTime dt = new DateTime(y,m,d);
                    return dt;
                }
            }
            return DateTime.Now;
        }

        public static int DateTimeToDeviceTime(DateTime value)
        {
            string str = value.Year.ToString("0000") + value.Month.ToString("00") + value.Day.ToString("00");
            int date = 0;
            if (str.Length == 8)
            {
                if (int.TryParse(str,out date))
                {
                    return date;
                }
            }
            return date;
        }

        /// <summary>
        /// 反转数据
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public static byte[] Reverse(byte[] data)
        {
            byte[] dataR = new byte[data.Length];
            for (int i = 0; i < data.Length; i++)
            {
                dataR[i] = data[data.Length - i - 1];
            }
            return dataR;
        }

        public static void WriteFileLog(string log)
        {
            try
            {
                File.AppendAllText(AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "\\log\\log"+DateTime.Now.ToString("yyyy-MM-dd")+".txt", log);
            }
            catch (Exception ex)
            {
            }
        }

        public static void DeleteLogFile()
        {
            DateTime dt1 = DateTime.Now;
            //log2016-08-16
            string[] files = Directory.GetFiles(AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "\\log");
            foreach (string item in files)
            {
                try
                {
                    DateTime dt2 = File.GetLastWriteTime(item);
                    if (dt1.Subtract(dt2).TotalDays > 30)
                    {
                        File.Delete(item);
                    }
                }
                catch (Exception ex)
                {
                    WriteFileLog(ex.Message);
                    WriteFileLog(item + "删除失败");
                }
            }
        }


        public static void CopyDir(string sourceDirName, string destDirName)
        {
            DirectoryInfo destdir = new DirectoryInfo(destDirName);
            DirectoryInfo sourcedir = new DirectoryInfo(sourceDirName);
            if (!destdir.Exists)
            {
                destdir.Create();
            }
            FileInfo[] files = sourcedir.GetFiles();
            foreach (FileInfo item in files)
            {
                File.Copy(item.FullName, destDirName + item.Name);
            }

            DirectoryInfo[] dirs = sourcedir.GetDirectories();
            foreach (DirectoryInfo item in dirs)
            {
                CopyDir(item.FullName + "\\", destdir.FullName + item.Name + "\\");
            }
        }
    }
}
