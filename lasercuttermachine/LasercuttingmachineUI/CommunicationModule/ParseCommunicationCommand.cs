using CommonModule;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Text;
using System.Xml;

namespace CommunicationModule
{
    public class ParseCommunicationCommand
    {
        public static bool ParseCommand(string code)
        {
            //double o = 0;
            //Convert.ToByte(o);
            //BitConverter.GetBytes(o);


            //BitConverter.GetBytes();
            
            //Encoding.Default.GetBytes(code);
            return false;
        }
        public static IntPtr usbIntPtr = (IntPtr)(-1);
        public static short usbIndex = 0;
        public static Dictionary<string, byte[]> writeDataList = new Dictionary<string, byte[]>();
        //public static List<string> commandList = new List<string>();
        public static DeviceState deviceState;
        public static ReadParam readParam;
        //public static List<CommuncationData> commandList = new List<CommuncationData>();
        public static byte[] sendCommandFlag = null;
        public static byte[] receiveCommandFlag = null;

        public static int BaseQueryHz = 1000;
        public static int ConnectHz = 1000;
        public static int cutNum;
        public static byte laserPointLight;
        public static short laserPointTime = 100;

        public static byte laserPointLightB;
        public static short laserPointTimeB = 100;

        public static float OriginX = -1;
        public static float OriginY = -1;
        public static bool IsReading = false;
        public static string executablePath;
        public static string lastPltFile = "";
        public static string currentFile = "";

        public static Dictionary<string, string> userInfo;
        public static bool isAlert = false;
        public static int userType;
        public static short manualFeedLen = 10;
        public static short feedOffset = 600;
        public static short sectionLen = 30;
        public static bool isCloseLaserB = false;
        public static bool IsWriteFile = false;
        public static int WriteFileWaitTime = 2000;
        public static int WriteFileWait = 10;
        public static int SaveOriginX = 0;
        public static int SaveOriginY = 0;

        public static string projectName;
        public static string productName;
        public static string projectIconName;
        public static string productIconName;
        public static string imageFileName;
        public static List<ProjectClass> projectClassList = new List<ProjectClass>();
        //public static CutFileClass currentWorkFile;
        //public static List<CommuncationData> writeDataList = new List<CommuncationData>();
        /// <summary>
        /// 打开设备
        /// </summary>
        public static void OpenDevice()
        {
            usbIntPtr = CommonModule.CH375DLL.OpenDevice(0);
        }
        /// <summary>
        /// 获取设备ID号
        /// </summary>
        /// <returns></returns>
        public static short GetDeviceIndex()
        {
            usbIndex = CommonModule.CH375DLL.GetUsbID(0);
            return usbIndex;
        }
        /// <summary>
        /// 关闭设备
        /// </summary>
        /// <param name="usbid"></param>
        public static void CloseDevice(short usbid)
        {
            CommonModule.CH375DLL.CloseDevice(usbid);
            usbIndex = 0;
            usbIntPtr = IntPtr.Zero;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="dataBufferLen"></param>
        /// <returns></returns>
        public static byte[] ReadDeviceData(int dataBufferLen)
        {
            if (usbIntPtr.ToInt32() == -1)
                return null;
            byte[] btyA = new byte[dataBufferLen];
            int length = btyA.Length;
            bool b = CommonModule.CH375DLL.ReadData(usbIndex, btyA, ref length);
            if (b)
            {
                byte[] data = new byte[length - receiveCommandFlag.Length];
                bool isFlag = true;
                for (int i = 0; i < receiveCommandFlag.Length; i++)
                {
                    if (btyA[i] != receiveCommandFlag[i])
                    {
                        isFlag = false;
                        break;
                    }
                }
                if (isFlag)
                {
                    for (int i = 0; i < data.Length; i++)
                    {
                        data[i] = btyA[i + receiveCommandFlag.Length];
                    }
                    return data;
                }
            }
            return null;
        }

         /// <summary>
        /// 
        /// </summary>
        /// <param name="dataBufferLen"></param>
        /// <returns></returns>
        public static byte[] ReadDeviceData1(int dataBufferLen)
        {
            if (usbIntPtr.ToInt32() == -1)
                return null;
            byte[] btyA = new byte[dataBufferLen];
            int length = btyA.Length;
            bool b = CommonModule.CH375DLL.ReadData(usbIndex, btyA, ref length);
            if (b)
            {
                byte[] data = new byte[length - receiveCommandFlag.Length];
                bool isFlag = true;
                for (int i = 0; i < receiveCommandFlag.Length; i++)
                {
                    if (btyA[i] != receiveCommandFlag[i])
                    {
                        isFlag = false;
                        break;
                    }
                }
                if (isFlag)
                {
                    for (int i = 0; i < data.Length; i++)
                    {
                        data[i] = btyA[i + receiveCommandFlag.Length];
                    }
                    return data;
                }
            }
            return null;
        }
        /// <summary>
        /// 获得设置参数返回状态 13	0x1A设置成功
        /// 0x10设置失败
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public static bool GetSetParamReturn(byte[] data)
        {
            if (data.Length < 2)
                return false;
            if (data[0] == 0x13 && data[1] == 0x1A)
            {
                return true;
            }
            else if (data[0] == 0x13 && data[1] == 0x10)
            {
                return false;
            }
            else
                return false;
        }
        /// <summary>
        /// 写数据
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public static bool WriteDeviceData(byte[] data)
        {
            if (data == null)
                return false;
            
            int length = data.Length;
            //if (deviceState.workState == WorkState.Stop)
            {
                bool b = CommonModule.CH375DLL.WriteData(usbIndex, data, ref length);
                if (b && length == data.Length)
                {
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// 初始化查询数据
        /// </summary>
        public static void InitialSearchData()
        {
            if (writeDataList == null)
                writeDataList = new Dictionary<string, byte[]>();
            writeDataList.Clear();

            //Console.WriteLine(System.Configuration.ConfigurationManager.AppSettings["Command"]);
            #region 读配置文件
            AppSettingsReader reader = new AppSettingsReader();
            string com = reader.GetValue("SendCommand", typeof(string)).ToString().Trim();
            List<byte> commandData = new List<byte>();
            if (com.Length > 0)
            {
                commandData.AddRange(System.Text.Encoding.Default.GetBytes(com));
                commandData.Add(0);
                sendCommandFlag = commandData.ToArray();
                commandData.Clear();
            }
            com = reader.GetValue("ReceiveCommand", typeof(string)).ToString().Trim();
            if (com.Length > 0)
            {
                commandData.AddRange(System.Text.Encoding.Default.GetBytes(com));
                receiveCommandFlag = commandData.ToArray();
                commandData.Clear();
            }
           
            com = reader.GetValue("CutNum", typeof(string)).ToString().Trim();
            if(!int.TryParse(com,out cutNum))
            {
                cutNum = -1;
            }
            //string[] tempCom = reader.GetValue("Command", typeof(string)).ToString().Trim().Split(new char[]{','}, StringSplitOptions.RemoveEmptyEntries);
            //foreach (string item in tempCom)
            //{
            //    commandList.Add(item);
            //}

            com = reader.GetValue("BaseQueryHz", typeof(string)).ToString().Trim();
            if (!int.TryParse(com, out BaseQueryHz))
            {
                BaseQueryHz = 1000;
            }
            com = reader.GetValue("ConnectHz", typeof(string)).ToString().Trim();
            if (!int.TryParse(com, out ConnectHz))
            {
                ConnectHz = 1000;
            }

            com = reader.GetValue("LastPltFile", typeof(string)).ToString().Trim();
            if (File.Exists(com))
            {
                lastPltFile = com;
            }

            com = reader.GetValue("LaserPointTime", typeof(string)).ToString().Trim();
            if (!short.TryParse(com, out laserPointTime))
            {
                laserPointTime = 100;
            }
            com = reader.GetValue("LaserPointLight", typeof(string)).ToString().Trim();
            if (!byte.TryParse(com, out laserPointLight))
            {
                laserPointLight = 20;
            }

            com = reader.GetValue("LaserPointTimeB", typeof(string)).ToString().Trim();
            if (!short.TryParse(com, out laserPointTimeB))
            {
                laserPointTimeB = 100;
            }
            com = reader.GetValue("LaserPointLightB", typeof(string)).ToString().Trim();
            if (!byte.TryParse(com, out laserPointLightB))
            {
                laserPointLightB = 20;
            }
            //manualFeedLen
            com = reader.GetValue("ManualFeedLen", typeof(string)).ToString().Trim();
            if (!short.TryParse(com, out manualFeedLen))
            {
                manualFeedLen = 200;
            }
            //FeedOffset 
            com = reader.GetValue("FeedOffset", typeof(string)).ToString().Trim();
            if (!short.TryParse(com, out feedOffset))
            {
                feedOffset = 0;
            }
            //sectionLen
            com = reader.GetValue("SectionLen", typeof(string)).ToString().Trim();
            if (!short.TryParse(com, out sectionLen))
            {
                sectionLen = 30;
            }
            //isCloseLaserB
            com = reader.GetValue("IsCloseLaserB", typeof(string)).ToString().Trim();
            if (!bool.TryParse(com, out isCloseLaserB))
            {
                isCloseLaserB = false;
            }
            //IsWriteFile
            com = reader.GetValue("IsWriteFile", typeof(string)).ToString().Trim();
            if (!bool.TryParse(com, out IsWriteFile))
            {
                IsWriteFile = false;
            }
            //WriteFileWaitTime
            com = reader.GetValue("WriteFileWaitTime", typeof(string)).ToString().Trim();
            if (!int.TryParse(com, out WriteFileWaitTime))
            {
                WriteFileWaitTime = 2000;
            }
            //WriteFileWait
            com = reader.GetValue("WriteFileWait", typeof(string)).ToString().Trim();
            if (!int.TryParse(com, out WriteFileWait))
            {
                WriteFileWait = 10;
            }
            #endregion 读配置文件

            //if(!int.TryParse(com,out cutNum))
            //{
            //    cutNum = -1;
            //}
            //commandList

            //查询设备状态 01
            //writeDataList.Add("查询设备状态", GetCommandByte(sendCommandFlag, 0x01, byte.MinValue, false));
            //读取参数 02
            //byte[] temp = new byte[sendCommandFlag.Length - 1];
            //for (int i = 0; i < temp.Length; i++)
            //{
            //    temp[i] = sendCommandFlag[i];
            //}
            #region 配置通信协议命令
            writeDataList.Add("读取参数", GetCommandByte(sendCommandFlag, 0x02, byte.MinValue, false));
            
            //定位点设置 04 上 0x36
            writeDataList.Add("定位点设置-上", GetCommandByte(sendCommandFlag, 0x04, 0x36, true));
            //下040x37
            writeDataList.Add("定位点设置-下", GetCommandByte(sendCommandFlag, 0x04, 0x37, true));
            //左040x37
            writeDataList.Add("定位点设置-左", GetCommandByte(sendCommandFlag, 0x04, 0x38, true));
            //右040x39
            writeDataList.Add("定位点设置-右", GetCommandByte(sendCommandFlag, 0x04, 0x39, true));
            //保存定位点信息040x40
            writeDataList.Add("保存定位点信息", GetCommandByte(sendCommandFlag, 0x04, 0x40, true));
            //取消保存定位点信息040x41
            writeDataList.Add("取消保存定位点信息", GetCommandByte(sendCommandFlag, 0x04, 0x41, true));
            //移动步长设置 250mm 050x01
            writeDataList.Add("移动步长设置-250mm", GetCommandByte(sendCommandFlag, 0x05, 0x01, true));
            //50mm  050x02
            writeDataList.Add("移动步长设置-50mm", GetCommandByte(sendCommandFlag, 0x05, 0x02, true));
            //0.2mm 050x03
            writeDataList.Add("移动步长设置-0.2mm", GetCommandByte(sendCommandFlag, 0x05, 0x03, true));
            //0.2mm 050x03
            writeDataList.Add("移动步长设置-700mm", GetCommandByte(sendCommandFlag, 0x05, 0x04, true));

            //移动出光设置 出光  060x01
            writeDataList.Add("移动出光设置-出光", GetCommandByte(sendCommandFlag, 0x06, 0x01, true));
            //不出光 060x00
            writeDataList.Add("移动出光设置-不出光", GetCommandByte(sendCommandFlag, 0x06, 0x00, true));

            //手动上料开始 07
            writeDataList.Add("手动上料开始", GetCommandByte(sendCommandFlag, 0x07, byte.MinValue, false));
            //手动上料结束 08
            writeDataList.Add("手动上料结束", GetCommandByte(sendCommandFlag, 0x08, byte.MinValue, false));
            //取消自动进料 09
            writeDataList.Add("取消自动进料", GetCommandByte(sendCommandFlag, 0x09, byte.MinValue, false));

            //手动低速送料 进料开始，按键按下 0A0x01
            writeDataList.Add("手动低速送料-开始", GetCommandByte(sendCommandFlag, 0x0A, 0x01, true));
            //手动低速送料 进料结束，按键松开 0A0x00
            writeDataList.Add("手动低速送料-结束", GetCommandByte(sendCommandFlag, 0x0A, 0x00, true));

            //手动低速退料-退料开始按键按下 0B0x01
            writeDataList.Add("手动低速退料-开始", GetCommandByte(sendCommandFlag, 0x0B, 0x01, true));
            //手动低速退料-退料结束按键松开 0B0x00
            writeDataList.Add("手动低速退料-结束", GetCommandByte(sendCommandFlag, 0x0B, 0x00, true));

            //终止工作 0C
            writeDataList.Add("终止工作", GetCommandByte(sendCommandFlag, 0x0C, byte.MinValue, false));
            //取消自动送料 0D
            writeDataList.Add("取消自动送料", GetCommandByte(sendCommandFlag, 0x0D, byte.MinValue, false));
            ////顶布测试 顶布 0E0x01
            //writeDataList.Add("顶布测试-顶布", GetCommandByte(sendCommandFlag, 0x0E, 0x01, true));
            ////顶布测试 夹布 0E0x00
            //writeDataList.Add("顶布测试-落下", GetCommandByte(sendCommandFlag, 0x0E, 0x00, true));

            //压布棍升降   升 0E0x01
            writeDataList.Add("压布棍-升", GetCommandByte(sendCommandFlag, 0x0E, 0x01, true));
            //压布棍升降   降 0E0x00
            writeDataList.Add("压布棍-降", GetCommandByte(sendCommandFlag, 0x0E, 0x00, true));


            ////夹布测试 夹布 100x00
            //writeDataList.Add("夹布测试-升", GetCommandByte(sendCommandFlag, 0x1A, 0x00, true));
            ////夹布测试 松开 100x01
            //writeDataList.Add("夹布测试-降", GetCommandByte(sendCommandFlag, 0x1A, 0x01, true));


            //吹气测试 吹气 0x0F0x01
            writeDataList.Add("吹气测试-吹气", GetCommandByte(sendCommandFlag, 0x0F, 0x01, true));
            //吹气测试 关闭 0x0F0x00
            writeDataList.Add("吹气测试-关闭", GetCommandByte(sendCommandFlag, 0x0F, 0x00, true));

            //自动送料测试 0x10
            writeDataList.Add("自动送料测试", GetCommandByte(sendCommandFlag, 0x10, byte.MinValue, false));
            //激光点射 0x11
            writeDataList.Add("激光点射", GetCommandByte(sendCommandFlag, 0x11, byte.MinValue, false));

            //关闭激光头B 0x12 - 1
            writeDataList.Add("关闭激光头B", GetCommandByte(sendCommandFlag, 0x12, 0x00, true));
            //打开激光头B 0x12 - 1
            writeDataList.Add("打开激光头B", GetCommandByte(sendCommandFlag, 0x12, 0x01, true));

            //手动/自动  手动 14
            writeDataList.Add("手动/自动-手动", GetCommandByte(sendCommandFlag, 0x14, 0x00, true));
            //手动/自动  自动 141
            writeDataList.Add("手动/自动-自动", GetCommandByte(sendCommandFlag, 0x14, 0x01, true));

            //空走测试 0x17
            //writeDataList.Add("空走测试", GetCommandByte(commandFlag, 0x16, 0x01, true));
            writeDataList.Add("空走测试", GetCommandByte(sendCommandFlag, 0x17, byte.MinValue, false));
            //开始工作
            writeDataList.Add("开始工作", null);
            //暂停工作  0x18
            writeDataList.Add("暂停工作", GetCommandByte(sendCommandFlag, 0x18, byte.MinValue, false));
            //清洗功能  0x19
            writeDataList.Add("清洗功能", GetCommandByte(sendCommandFlag, 0x19, byte.MinValue, false));
            //夹布测试 夹布 1A0x00
            writeDataList.Add("夹布测试-升", GetCommandByte(sendCommandFlag, 0x1A, 0x00, true));
            //夹布测试 松开 1A0x01
            writeDataList.Add("夹布测试-降", GetCommandByte(sendCommandFlag, 0x1A, 0x01, true));
            
            //夹布测试 松开 1A0x01
            writeDataList.Add("手动进料", GetCommandByte(sendCommandFlag, 0x0B, byte.MinValue, false));
            //夹布测试 松开 1A0x01
            writeDataList.Add("手动退料", GetCommandByte(sendCommandFlag, 0x0D, byte.MinValue, false));
            //夹布测试 松开 1A0x01
            writeDataList.Add("手动切断", GetCommandByte(sendCommandFlag, 0x1B, byte.MinValue, false));
            #endregion 配置通信协议命令
        }

        /// <summary>
        /// 获取命令字节数组
        /// </summary>
        /// <param name="flag">命令标识前缀 如：Msg_Comm</param>
        /// <param name="command1">一级命令</param>
        /// <param name="command2">二级命令</param>
        /// <param name="isPart">是否有二级命令</param>
        /// <returns></returns>
        public static byte[] GetCommandByte(byte[] flag,byte command1,byte command2,bool isPart= false)
        {
            List<byte> data = new List<byte>();
            if (flag != null && flag.Length > 0)
                data.AddRange(flag);
            if (isPart)
            {
                data.Add(command1);
                data.Add(command2);
            }
            else
            {
                data.Add(command1);
            }
            
            return data.ToArray();
        }
        #region 注释暂时未用到的
        ///// <summary>
        ///// 获得设置点射光强、延时的写入数据
        ///// </summary>
        ///// <returns></returns>
        //public static byte[] GetSetLaserPointWriteData(SetLaserPoint sp,object value)
        //{
        //    List<byte> data = new List<byte>();
        //    switch (sp)
        //    {
        //        case SetLaserPoint.激光点射光强: //14
        //            //data.AddRange(GetCommandByte(commandFlag, 0x14, byte.MinValue, false));

        //            data.Add(0x14);
        //            data.Add((byte)value);
        //            break;
        //        case SetLaserPoint.激光点射延时: //15
        //            //data.AddRange(GetCommandByte(commandFlag, 0x15, byte.MinValue, false));
        //            data.Add(0x15);
        //            data.AddRange(BitConverter.GetBytes((short)value));
        //            break;
        //    }
            
        //    return data.ToArray();
        //}
        ///// <summary>
        ///// 获得文件信息的写入数据
        ///// </summary>
        ///// <returns></returns>
        //public static byte[] GetFileInfoWriteData(FileInfo fInfo)
        //{
        //    List<byte> data = new List<byte>();
        //    data.Add(0x17);

        //    //图形宽度4Byte
        //    //图形长度4Byte
        //    //片料/卷料1Byte
        //    //文件总字节数4Byte
        //    data.AddRange(BitConverter.GetBytes((int)fInfo.Length));

        //    ////文件信息传输
        //    //writeDataList.Add(System.Text.Encoding.Default.GetBytes("17"));
        //    return data.ToArray();
        //}
        ///// <summary>
        ///// 获得传输文件的写入数据
        ///// </summary>
        ///// <returns></returns>
        //public static byte[] GetTranfromFileWriteData(string data)
        //{
        //    //List<byte> data = new List<byte>();
        //    ////文件数据传输
        //    //writeDataList.Add();
        //    return System.Text.Encoding.Default.GetBytes(data);
        //}
        #endregion
        /// <summary>
        /// 查询设备状态数据解析
        /// </summary>
        /// <param name="data"></param>
        public static bool GetSearchReadData(byte[] data)
        {
            if (data== null || data.Length < 12)
                return false;
            if (deviceState == null)
                deviceState = new DeviceState();
            deviceState.InitialDeviceState(data);
            if (deviceState.cutCount != ParseCommunicationCommand.cutNum)
            {
                ParseCommunicationCommand.cutNum =deviceState.cutCount;
                SaveConfig(ParseCommunicationCommand.cutNum.ToString(), "CutNum");
            }

            return deviceState.isInitial;
        }

        /// <summary>
        /// 获取查询设备状态写入数据
        /// </summary>
        /// <returns></returns>
        public static byte[] GetQueryState()
        {
            //writeDataList.Add("查询设备状态", GetCommandByte(sendCommandFlag, 0x01, byte.MinValue, false));
            List<byte> data = new List<byte>();
            if (sendCommandFlag != null && sendCommandFlag.Length > 0)
            {
                data.AddRange(sendCommandFlag);
            }
            data.Add(0x01);
            data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(cutNum)));
            return data.ToArray();
        }

        /// <summary>
        /// 执行设置参数命令
        /// </summary>
        /// <param name="writeData"></param>
        /// <param name="dataType"></param>
        /// <param name="dataLen"></param>
        /// <returns></returns>
        public static bool ExecuteOriginPointCommand(byte[] writeData, string dataType, int dataLen)
        {
            IsReading = true;
            CommuncationData cd = new CommuncationData();
            cd.writeData = writeData;
            cd.dataType = dataType;
            cd.isWrite = WriteDeviceData(cd.writeData);
            if (cd.isWrite)
            {
                cd.readData = ReadDeviceData(dataLen);
                WriteFileCommand(cd);
                IsReading = false;
                if (cd.readData != null)
                {
                    WriteFileCommand(cd);
                    if ((cd.writeData[sendCommandFlag.Length] + 16) == cd.readData[0])
                    {
                        cd.isRead = true;
                        return true;
                    }
                }
            }
            WriteFileCommand(cd);
            IsReading = false;
            return false;
        }

        /// <summary>
        /// 执行设置参数命令
        /// </summary>
        /// <param name="writeData"></param>
        /// <param name="dataType"></param>
        /// <param name="dataLen"></param>
        /// <returns></returns>
        public static CommuncationData ExecuteOriginPointCommand1(byte[] writeData, string dataType, int dataLen)
        {
            IsReading = true;
            CommuncationData cd = new CommuncationData();
            cd.writeData = writeData;
            cd.dataType = dataType;
            cd.isWrite = WriteDeviceData(cd.writeData);
            if (cd.isWrite)
            {
                cd.readData = ReadDeviceData(dataLen);
                WriteFileCommand(cd);
                IsReading = false;
                if (cd.readData != null)
                {
                    WriteFileCommand(cd);
                    if ((cd.writeData[sendCommandFlag.Length] + 16) == cd.readData[0])
                    {
                        cd.isRead = true;
                        return cd;
                    }
                }
            }
            WriteFileCommand(cd);
            IsReading = false;
            return cd;
        }

        /// <summary>
        /// 执行设置参数命令
        /// </summary>
        /// <param name="writeData"></param>
        /// <param name="dataType"></param>
        /// <param name="dataLen"></param>
        /// <returns></returns>
        public static byte[] ExecuteOriginPointCommand(byte[] writeData, string dataType, int dataLen,bool b)
        {
            IsReading = true;
            CommuncationData cd = new CommuncationData();
            cd.writeData = writeData;
            cd.dataType = dataType;
            cd.isWrite = WriteDeviceData(cd.writeData);
            if (cd.isWrite)
            {
                cd.readData = ReadDeviceData(dataLen);
                IsReading = false;
                if (cd.readData != null)
                {
                    WriteFileCommand(cd);
                    if ((cd.writeData[sendCommandFlag.Length] + 16) == cd.readData[0])
                    {
                        cd.isRead = true;
                        return cd.readData;
                    }
                }
            }
            WriteFileCommand(cd);
            IsReading = false;
            return null;
        }
        /// <summary>
        /// 执行设置参数命令
        /// </summary>
        /// <param name="writeData"></param>
        /// <param name="dataType"></param>
        /// <param name="dataLen"></param>
        /// <returns></returns>
        public static bool ExecuteLaserPointCommand(byte[] writeData, string dataType, int dataLen)
        {
            IsReading = true;
            CommuncationData cd = new CommuncationData();
            cd.writeData = writeData;
            cd.dataType = dataType;
            cd.isWrite = WriteDeviceData(cd.writeData);
            WriteFileCommand(cd);
            if (cd.isWrite)
            {
                cd.readData = ReadDeviceData(dataLen);
                WriteFileCommand(cd);
                IsReading = false;
                if (cd.readData != null)
                {
                    if (cd.writeData[9] + 16 == cd.readData[0])
                    {
                        cd.isRead = true;
                        return true;
                    }
                }
            }
            IsReading = false;
            return false;
        }

        /// <summary>
        /// 执行设置参数命令
        /// </summary>
        /// <param name="writeData"></param>
        /// <param name="dataType"></param>
        /// <param name="dataLen"></param>
        /// <returns></returns>
        public static bool ExecuteSetParamCommand(byte[] writeData, string dataType, int dataLen)
        {
            IsReading = true;
            CommuncationData cd = new CommuncationData();
            cd.writeData = writeData;
            cd.dataType = dataType;
            cd.isWrite = WriteDeviceData(cd.writeData);
            if (cd.isWrite)
            {
                cd.readData = ReadDeviceData(dataLen);
                IsReading = false;
                WriteFileCommand(cd);
                if (cd.readData != null)
                {
                    cd.endRead = GetSetParamReturn(cd.readData);
                    if (cd.endRead)
                    {
                        return true;
                    }
                    cd.isRead = true;
                }
            }
            WriteFileCommand(cd);
            IsReading = false;
            return false;
        }

        /// <summary>
        /// 执行查询设备参数
        /// </summary>
        /// <param name="writeData"></param>
        /// <param name="dataType"></param>
        /// <param name="dataLen"></param>
        public static bool ExecuteQueryDeviceParam(byte[] writeData, string dataType, int dataLen)
        {
            IsReading = true;
            CommuncationData cd1 = new CommuncationData();
            cd1.writeData = writeData;
            cd1.dataType = dataType;
            cd1.isWrite = ParseCommunicationCommand.WriteDeviceData(cd1.writeData);
            if (cd1.isWrite)
            {
                cd1.readData = ParseCommunicationCommand.ReadDeviceData(dataLen);
                IsReading = false;
                WriteFileCommand(cd1);
                if (cd1.readData != null)
                {
                    ParseCommunicationCommand.GetReadParamData(cd1.readData);
                    if (ParseCommunicationCommand.readParam.isInital)
                    {
                        return true;
                    }
                    cd1.isRead = true;
                }
            }
            WriteFileCommand(cd1);
            IsReading = false;
            return false;
        }

        /// <summary>
        /// 执行查询设备状态
        /// </summary>
        /// <param name="writeData"></param>
        /// <param name="dataType"></param>
        /// <param name="dataLen"></param>
        public static bool ExecuteQueryDeviceState(byte[] writeData, string dataType, int dataLen)
        {
            IsReading = true;
            CommuncationData cd = new CommuncationData();
            cd.writeData = writeData;
            cd.dataType = dataType;
            cd.isWrite = WriteDeviceData(cd.writeData);
            if (cd.isWrite)
            {
                cd.readData = ReadDeviceData(dataLen);
                WriteFileCommand(cd);
                IsReading = false;
                if (cd.readData != null)
                {
                    return GetSearchReadData(cd.readData);
                }
            }
            IsReading = false;
            WriteFileCommand(cd);
            return false;
        }
        /// <summary>
        /// 读取参数 数据解析
        /// </summary>
        /// <param name="data"></param>
        public static void GetReadParamData(byte[] data)
        {
            if (data==null)
                return;
            if (readParam == null)
                readParam = new ReadParam();
            if (data[0] == 0x12)
            {
                readParam.InitialReadParamData(data);
            }
        }

        /// <summary>
        /// 执行手动进料参数命令
        /// </summary>
        /// <returns></returns>
        public static bool ExecuteManualFeedParam()
        {
            List<byte> data = new List<byte>();
            data.AddRange(GetCommandByte(sendCommandFlag, 0x0A, byte.MinValue, false));
            data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(manualFeedLen)));
            data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(feedOffset)));
            data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(sectionLen)));

            bool b = ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "手动参数设置", 120);
            return b;
        }
        public static bool ExecuteCommand(string command)
        {
            if (writeDataList.ContainsKey(command))
            {
                byte[] data = writeDataList[command];
                return ExecuteOriginPointCommand(data, command, 120);
            }
            return false;
        }

        public static CommuncationData ExecuteCommand1(string command)
        {
            if (writeDataList.ContainsKey(command))
            {
                byte[] data = writeDataList[command];
                CommuncationData cd = ExecuteOriginPointCommand1(data, command, 120);
                return cd;
            }
            return null;
        }

        public static void WriteFileCommand(CommuncationData cd)
        {
            try
            {
                string info = cd.ToString();
                string path = AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "log";
                if(!Directory.Exists(path))
                {
                    Directory.CreateDirectory(path);
                }
                File.AppendAllText(path + "\\commandlog" + DateTime.Now.ToString("yyyy-MM-dd") + ".txt", info);
            }
            catch (Exception ex)
            {
                WriteFileCommand(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "WriteFileCommandCommuncationData\r\n");
            }
        }

        public static void WriteFileCommand(string log)
        {
            try
            {
                string path = AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "log";
                if (!Directory.Exists(path))
                {
                    Directory.CreateDirectory(path);
                }
                File.AppendAllText(path + "\\log" + DateTime.Now.ToString("yyyy-MM-dd") + ".txt", log);
            }
            catch (Exception ex)
            {
                WriteFileCommand(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "WriteFileCommandstring\r\n");
            }
        }
        /// <summary>
        /// 保存配置文件
        /// </summary>
        /// <param name="value"></param>
        /// <param name="key"></param>
        public static void SaveConfig(string value, string key)
        {
            try
            {
                XmlDocument doc = new XmlDocument();
                //获得配置文件的全路径
                //string strFileName = AppDomain.CurrentDomain.BaseDirectory.ToString() + "Code.exe.config";
                string strFileName = executablePath + ".config";
                doc.Load(strFileName);
                //找出名称为“add”的所有元素
                XmlNodeList nodes = doc.GetElementsByTagName("add");
                for (int i = 0; i < nodes.Count; i++)
                {
                    //获得将当前元素的key属性
                    XmlAttribute att = nodes[i].Attributes["key"];
                    //根据元素的第一个属性来判断当前的元素是不是目标元素
                    if (att.Value == key)
                    {
                        //对目标元素中的第二个属性赋值
                        att = nodes[i].Attributes["value"];
                        att.Value = value;
                        break;
                    }
                }
                //保存上面的修改
                doc.Save(strFileName);
            }
            catch (Exception ex)
            {
                ParseCommunicationCommand.WriteFileCommand(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "SaveConfig\r\n");
            }
        }

        public static void SaveLastFile(string filePath)
        {
            lastPltFile = filePath;
            SaveConfig(filePath, "LastPltFile");
        }

        /// <summary>
        /// 保存配置数据
        /// </summary>
        public static void SaveConfig(string path)
        {
            if (readParam == null)
                return;
            List<string> values = new List<string>();
            values.Add("filename:" + path + ";");
            values.Add("autoFeed:" + readParam.autoFeed.ToString() + ";");
            values.Add("clothKey:" + readParam.clothKey.ToString() + ";");
            values.Add("clothTest:" + readParam.clothTest.ToString() + ";");
            values.Add("cutInitalSpeed1:" + readParam.cutInitalSpeed1.ToString() + ";");
            values.Add("cutMaxSpeed1:" + readParam.cutMaxSpeed1.ToString() + ";");
            values.Add("cutOffset:" + readParam.cutOffset.ToString() + ";");
            values.Add("cutSpeed1:" + readParam.cutSpeed1.ToString() + ";");
            values.Add("cutSpeedUpTime:" + readParam.cutSpeedUpTime.ToString() + ";");
            values.Add("deviceTime:" + readParam.deviceTime.ToString() + ";");
            values.Add("deviceType:" + readParam.deviceType.ToString() + ";");
            values.Add("distanceAB:" + readParam.distanceAB.ToString() + ";");
            values.Add("feedCompensate:" + readParam.feedCompensate.ToString() + ";");
            values.Add("feedSpeedU1:" + readParam.feedSpeedU1.ToString() + ";");
            values.Add("feedSpeedX1:" + readParam.feedSpeedX1.ToString() + ";");

            values.Add("gateWay:" + ConvertType.ToIpAddress(readParam.gateWay) + ";");
            values.Add("ipAddress:" + ConvertType.ToIpAddress(readParam.ipAddress) + ";");
            values.Add("laserBKey:" + readParam.laserBKey.ToString() + ";");
            values.Add("maxLightIntensity:" + readParam.maxLightIntensity.ToString() + ";");
            values.Add("maxLightIntensityB:" + readParam.maxLightIntensityB.ToString() + ";");
            values.Add("minLightIntensity:" + readParam.minLightIntensity.ToString() + ";");
            values.Add("minLightIntensityB:" + readParam.minLightIntensityB.ToString() + ";");
            values.Add("moveFrameSpeed1:" + readParam.moveFrameSpeed1.ToString() + ";");
            values.Add("needCut:" + readParam.needCut.ToString() + ";");
            values.Add("pushFeedKey:" + readParam.pushFeedKey.ToString() + ";");
            values.Add("subNetMask:" + ConvertType.ToIpAddress(readParam.subNetMask) + ";");
            values.Add("testWorkSpeed1:" + readParam.testWorkSpeed1.ToString() + ";");
            values.Add("versionNum:" + ConvertType.ToIpAddress(readParam.versionNum) + ";");
            values.Add("xLenCorrection1:" + readParam.xLenCorrection1.ToString() + ";");
            values.Add("yLenCorrection1:" + readParam.yLenCorrection1.ToString() + ";");
            values.Add("SaveOriginX:" + SaveOriginX.ToString() + ";");
            values.Add("SaveOriginY:" + SaveOriginY.ToString() + ";");
            values.Add("manualFeedLength:" + manualFeedLen.ToString() + ";");
            values.Add("manualFeedOffsetLength:" + feedOffset.ToString() + ";");
            values.Add("manualFeedBreakLength:" + sectionLen.ToString() + ";");
            //File.WriteAllLines(AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "\\config\\" + currentFile + ".txt", values.ToArray());
            if (path == null || path.Trim().Length == 0)
            {
                if (currentFile.Length == 0)
                    path = AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "\\config\\defuat.txt";
                else
                {
                    path = AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "\\config\\" + currentFile + ".txt";
                    //path = path.Substring(0, path.Length - 4) + ".txt";
                }
            }

            string cutFilePath = path.Substring(0, path.Length - 4) + ".txt";
            //if (File.Exists(cutFilePath))
            //{
            //    File.Delete(cutFilePath);
            //}
            File.WriteAllLines(cutFilePath, values.ToArray());
        }

        /// <summary>
        /// 读取配置数据
        /// </summary>
        public static void ReadConfig(string path)
        {
            FileInfo pltFile = new FileInfo(path.Substring(0, path.Length - 4) + ".txt");		//the sourceFile is set.
            if (!pltFile.Exists)
                return;
            StreamReader reader = pltFile.OpenText();
            if (reader == null)
            {
                return;
            }
            string str = reader.ReadToEnd();
            ReadParam rp = (ReadParam)readParam.Clone();
            int x = 0;
            int y = 0;
            string[] strlist = str.Split(new char[] { ';','\r','\n'}, System.StringSplitOptions.RemoveEmptyEntries);
            foreach (string item in strlist)
            {
                if (item.Length == 0)
                    continue;
                string[] line = item.Split(new char[] { ':' }, System.StringSplitOptions.RemoveEmptyEntries);

                switch(line[0])
                {
                    case "autoFeed":
                        rp.autoFeed = ConvertType.ConvertByte(line[1]);
                        break;
                    case "clothKey":
                        rp.clothKey = ConvertType.ConvertByte(line[1]);
                        break;
                    case "clothTest":
                        rp.clothKey = ConvertType.ConvertByte(line[1]);
                        break;
                    case "cutInitalSpeed1":
                        rp.cutInitalSpeed1 = ConvertType.ConvertByte(line[1]);
                        break;
                    case "cutMaxSpeed1":
                        rp.cutMaxSpeed1 = ConvertType.ConvertByte(line[1]);
                        break;
                    case "cutOffset":
                        rp.cutOffset = ConvertType.ConvertByte(line[1]);
                        break;
                    case "cutSpeed1":
                        rp.cutSpeed1 = ConvertType.ConvertByte(line[1]);
                        break;
                    case "cutSpeedUpTime":
                        rp.cutSpeedUpTime = ConvertType.ConvertShort(line[1]);
                        break;
                    case "deviceTime":
                        //rp.deviceTime = ConvertType.ConvertInt(line[1]);
                        break;
                    case "deviceType":
                        //rp.deviceType = ConvertType.ConvertShort(line[1]);
                        break;
                    case "distanceAB":
                        rp.distanceAB = ConvertType.ConvertShort(line[1]);
                        break;
                    case "feedCompensate":
                        rp.feedCompensate = ConvertType.ConvertByte(line[1]);
                        break;
                    case "feedSpeedU1":
                        rp.feedSpeedU1 = ConvertType.ConvertShort(line[1]);
                        break;
                    case "feedSpeedX1":
                        rp.feedSpeedX1 = ConvertType.ConvertShort(line[1]);
                        break;
                    case "gateWay": 
                        break;
                    case "ipAddress": 
                        break;
                    case "laserBKey":
                        rp.laserBKey = ConvertType.ConvertByte(line[1]);
                        break;
                    case "maxLightIntensity":
                        rp.maxLightIntensity = ConvertType.ConvertByte(line[1]);
                        break;
                    case "maxLightIntensityB":
                        rp.maxLightIntensityB = ConvertType.ConvertByte(line[1]);
                        break;
                    case "minLightIntensity":
                        rp.minLightIntensity = ConvertType.ConvertByte(line[1]);
                        break;
                    case "minLightIntensityB":
                        rp.minLightIntensityB = ConvertType.ConvertByte(line[1]);
                        break;
                    case "moveFrameSpeed1":
                        rp.moveFrameSpeed1 = ConvertType.ConvertByte(line[1]);
                        break;
                    case "needCut":
                        rp.needCut = ConvertType.ConvertByte(line[1]);
                        break;
                    case "pushFeedKey":
                        rp.pushFeedKey = ConvertType.ConvertByte(line[1]);
                        break;
                    case "subNetMask":
                        //rp.subNetMask = ConvertType.ConvertByte(line[1]);
                        break;
                    case "testWorkSpeed1":
                        rp.testWorkSpeed1 = ConvertType.ConvertByte(line[1]);
                        break;
                    case "versionNum": 
                        break;
                    case "xLenCorrection1":
                        rp.xLenCorrection1 = ConvertType.ConvertInt(line[1]);
                        break;
                    case "yLenCorrection1":
                        rp.yLenCorrection1 = ConvertType.ConvertInt(line[1]);
                        break;
                    case "SaveOriginX":
                        x = ConvertType.ConvertInt(line[1]);
                        break;
                    case "SaveOriginY":
                        y = ConvertType.ConvertInt(line[1]);
                        break;
                }
                //if (item1.Length > 2)
                //    operators2.Add(item1);
            }
            reader.Close();
            reader.Dispose();
            List<byte> data = new List<byte>();
            if (sendCommandFlag != null)
                data.AddRange(sendCommandFlag);

            bool isSuccess = ExecuteSetParamCommand(data.ToArray(), "设置参数", 120);
            if (isSuccess)
            {
                readParam = rp;
            }
            isSuccess = false;
            data.Clear();
            if (sendCommandFlag != null)
                data.AddRange(sendCommandFlag);
            data.Add(0x14);
            data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(SaveOriginX)));
            data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(SaveOriginY)));
            isSuccess = ExecuteLaserPointCommand(data.ToArray(), "定位点坐标发送", 120);
            if(isSuccess)
            {
                SaveOriginX = x;
                SaveOriginY = y;
            }
        }
        public static void InitialProjectClass()
        {
            if (projectClassList == null)
                projectClassList = new List<ProjectClass>();
            string path = AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "project\\";
            if(Directory.Exists(path))
            {
                string[] projects = Directory.GetDirectories(path);
                for (int i = 0; i < projects.Length; i++)
                {
                    ProjectClass projectClass = new ProjectClass();
                    projectClass.name = Path.GetFileNameWithoutExtension(projects[i]);
                    string[] projectIconFiles = Directory.GetFiles(projects[i]);//, "(*.png|*.jpg|*.bmp|*.gif)"

                    for (int m = 0; m < projectIconFiles.Length; m++)
                    {
                        if(projectIconFiles[m].EndsWith(".png") || projectIconFiles[m].EndsWith(".jpg") ||
                            projectIconFiles[m].EndsWith(".bmp") || projectIconFiles[m].EndsWith(".gif"))
                        {
                            projectClass.iconName = projectIconFiles[m];
                            break;
                        }
                    }
                    if (projectIconFiles.Length > 0)
                        projectClass.iconName = projectIconFiles[0];

                    string[] products = Directory.GetDirectories(projects[i]);
                    for (int j = 0; j < products.Length; j++)
                    {
                        ProductClass productClass = new ProductClass();
                        productClass.name = Path.GetFileNameWithoutExtension(products[j]);
                        productClass.parent = projectClass;
                        string[] productIconFiles = Directory.GetFiles(products[j]);
                        //if (productIconFiles.Length > 0)
                        //    productClass.iconName = productIconFiles[0];
                        for (int n = 0; n < productIconFiles.Length; n++)
                        {
                            if (productIconFiles[n].EndsWith(".png") || productIconFiles[n].EndsWith(".jpg") ||
                                productIconFiles[n].EndsWith(".bmp") || productIconFiles[n].EndsWith(".gif"))
                            {
                                productClass.iconName = productIconFiles[n];
                                break;
                            }
                        }

                        string[] cutFiles = Directory.GetFiles(products[j]);
                        for (int k = 0; k < cutFiles.Length; k++)
                        {
                            if (cutFiles[k].EndsWith(".dxf") || cutFiles[k].EndsWith(".plt"))
                            {
                                CutFileClass cutFileClass = ReadConfigCutFile(cutFiles[k]);
                                cutFileClass.parent = productClass;
                                productClass.cutFiles.Add(cutFileClass);
                            }
                        }
                        if (projectClass.products == null)
                            projectClass.products = new List<ProductClass>();
                        projectClass.products.Add(productClass);
                    }
                    projectClassList.Add(projectClass);
                }
            }
        }

        /// <summary>
        /// 保存配置数据
        /// </summary>
        public static void SaveConfigCutFile(CutFileClass file,string path)
        {
            if (file == null)
                return;

            List<string> values = new List<string>();
            values.Add("filename:" + path + ";");
            if (file.readParam != null)
            {
                values.Add("autoFeed:" + file.readParam.autoFeed.ToString() + ";");
                values.Add("clothKey:" + file.readParam.clothKey.ToString() + ";");
                values.Add("clothTest:" + file.readParam.clothTest.ToString() + ";");
                values.Add("cutInitalSpeed1:" + file.readParam.cutInitalSpeed1.ToString() + ";");
                values.Add("cutMaxSpeed1:" + file.readParam.cutMaxSpeed1.ToString() + ";");
                values.Add("cutOffset:" + file.readParam.cutOffset.ToString() + ";");
                values.Add("cutSpeed1:" + file.readParam.cutSpeed1.ToString() + ";");
                values.Add("cutSpeedUpTime:" + file.readParam.cutSpeedUpTime.ToString() + ";");
                values.Add("deviceTime:" + file.readParam.deviceTime.ToString() + ";");
                values.Add("deviceType:" + file.readParam.deviceType.ToString() + ";");
                values.Add("distanceAB:" + file.readParam.distanceAB.ToString() + ";");
                values.Add("feedCompensate:" + file.readParam.feedCompensate.ToString() + ";");
                values.Add("feedSpeedU1:" + file.readParam.feedSpeedU1.ToString() + ";");
                values.Add("feedSpeedX1:" + file.readParam.feedSpeedX1.ToString() + ";");

                values.Add("gateWay:" + ConvertType.ToIpAddress(file.readParam.gateWay) + ";");
                values.Add("ipAddress:" + ConvertType.ToIpAddress(file.readParam.ipAddress) + ";");
                values.Add("laserBKey:" + file.readParam.laserBKey.ToString() + ";");
                values.Add("maxLightIntensity:" + file.readParam.maxLightIntensity.ToString() + ";");
                values.Add("maxLightIntensityB:" + file.readParam.maxLightIntensityB.ToString() + ";");
                values.Add("minLightIntensity:" + file.readParam.minLightIntensity.ToString() + ";");
                values.Add("minLightIntensityB:" + file.readParam.minLightIntensityB.ToString() + ";");
                values.Add("moveFrameSpeed1:" + file.readParam.moveFrameSpeed1.ToString() + ";");
                values.Add("needCut:" + file.readParam.needCut.ToString() + ";");
                values.Add("pushFeedKey:" + file.readParam.pushFeedKey.ToString() + ";");
                values.Add("subNetMask:" + ConvertType.ToIpAddress(file.readParam.subNetMask) + ";");
                values.Add("testWorkSpeed1:" + file.readParam.testWorkSpeed1.ToString() + ";");
                values.Add("versionNum:" + ConvertType.ToIpAddress(file.readParam.versionNum) + ";");
                values.Add("xLenCorrection1:" + file.readParam.xLenCorrection1.ToString() + ";");
                values.Add("yLenCorrection1:" + file.readParam.yLenCorrection1.ToString() + ";");

                //case "manualFeedLength":
                //    case "manualFeedOffsetLength":
                //    case "manualFeedBreakLength":
                values.Add("manualFeedLength:" + file.manualFeedLength.ToString() + ";");
                values.Add("manualFeedOffsetLength:" + file.manualFeedOffsetLength.ToString() + ";");
                values.Add("manualFeedBreakLength:" + file.manualFeedBreakLength.ToString() + ";");
            }
            values.Add("SaveOriginX:" + file.x.ToString() + ";");
            values.Add("SaveOriginY:" + file.y.ToString() + ";");

            string cutFilePath = path.Substring(0, path.Length - 4) + ".txt";
            //if(File.Exists(cutFilePath))
            //{
            //    File.Delete(cutFilePath);
            //}
            //File.WriteAllLines(AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "\\config\\" + currentFile + ".txt", values.ToArray());
            File.WriteAllLines(cutFilePath, values.ToArray());
        }
        /// <summary>
        /// 读取配置数据
        /// </summary>
        public static CutFileClass ReadConfigCutFile(string path)
        {
            FileInfo pltFile = new FileInfo(path.Substring(0, path.Length - 4) + ".txt");		//the sourceFile is set.
            CutFileClass file = new CutFileClass();
            file.cutFileName = path;
            if (!pltFile.Exists)
            {
                return file;
            }
            StreamReader reader = pltFile.OpenText();
            if (reader == null)
            {
                return file;
            }
            string str = reader.ReadToEnd();
            ReadParam rp;
            if (readParam == null)
                rp = new ReadParam();
            else
                rp = (ReadParam)readParam.Clone();
            int x = 0;
            int y = 0;
            short feedLen1 = 10;
            short offsetLen1 = 600;
            short breakLen1 = 30;
            string[] strlist = str.Split(new char[] { ';', '\r', '\n' }, System.StringSplitOptions.RemoveEmptyEntries);
            foreach (string item in strlist)
            {
                if (item.Length == 0)
                    continue;
                string[] line = item.Split(new char[] { ':' }, System.StringSplitOptions.RemoveEmptyEntries);

                switch (line[0])
                {
                    case "autoFeed":
                        rp.autoFeed = ConvertType.ConvertByte(line[1]);
                        break;
                    case "clothKey":
                        rp.clothKey = ConvertType.ConvertByte(line[1]);
                        break;
                    case "clothTest":
                        rp.clothTest = ConvertType.ConvertByte(line[1]);
                        break;
                    case "cutInitalSpeed1":
                        rp.cutInitalSpeed1 = ConvertType.ConvertByte(line[1]);
                        break;
                    case "cutMaxSpeed1":
                        rp.cutMaxSpeed1 = ConvertType.ConvertByte(line[1]);
                        break;
                    case "cutOffset":
                        rp.cutOffset = ConvertType.ConvertByte(line[1]);
                        break;
                    case "cutSpeed1":
                        rp.cutSpeed1 = ConvertType.ConvertByte(line[1]);
                        break;
                    case "cutSpeedUpTime":
                        rp.cutSpeedUpTime = ConvertType.ConvertShort(line[1]);
                        break;
                    case "deviceTime":
                        //rp.deviceTime = ConvertType.ConvertInt(line[1]);
                        break;
                    case "deviceType":
                        //rp.deviceType = ConvertType.ConvertShort(line[1]);
                        break;
                    case "distanceAB":
                        rp.distanceAB = ConvertType.ConvertShort(line[1]);
                        break;
                    case "feedCompensate":
                        rp.feedCompensate = ConvertType.ConvertByte(line[1]);
                        break;
                    case "feedSpeedU1":
                        rp.feedSpeedU1 = ConvertType.ConvertShort(line[1]);
                        break;
                    case "feedSpeedX1":
                        rp.feedSpeedX1 = ConvertType.ConvertShort(line[1]);
                        break;
                    case "gateWay":
                        break;
                    case "ipAddress":
                        break;
                    case "laserBKey":
                        rp.laserBKey = ConvertType.ConvertByte(line[1]);
                        break;
                    case "maxLightIntensity":
                        rp.maxLightIntensity = ConvertType.ConvertByte(line[1]);
                        break;
                    case "maxLightIntensityB":
                        rp.maxLightIntensityB = ConvertType.ConvertByte(line[1]);
                        break;
                    case "minLightIntensity":
                        rp.minLightIntensity = ConvertType.ConvertByte(line[1]);
                        break;
                    case "minLightIntensityB":
                        rp.minLightIntensityB = ConvertType.ConvertByte(line[1]);
                        break;
                    case "moveFrameSpeed1":
                        rp.moveFrameSpeed1 = ConvertType.ConvertByte(line[1]);
                        break;
                    case "needCut":
                        rp.needCut = ConvertType.ConvertByte(line[1]);
                        break;
                    case "pushFeedKey":
                        rp.pushFeedKey = ConvertType.ConvertByte(line[1]);
                        break;
                    case "subNetMask":
                        //rp.subNetMask = ConvertType.ConvertByte(line[1]);
                        break;
                    case "testWorkSpeed1":
                        rp.testWorkSpeed1 = ConvertType.ConvertByte(line[1]);
                        break;
                    case "versionNum":
                        break;
                    case "xLenCorrection1":
                        rp.xLenCorrection1 = ConvertType.ConvertInt(line[1]);
                        break;
                    case "yLenCorrection1":
                        rp.yLenCorrection1 = ConvertType.ConvertInt(line[1]);
                        break;
                    case "SaveOriginX":
                        x = ConvertType.ConvertInt(line[1]);
                        break;
                    case "SaveOriginY":
                        y = ConvertType.ConvertInt(line[1]);
                        break;
            //            ParseCommunicationCommand.manualFeedLen = selectCutFile.manualFeedLength;
            //ParseCommunicationCommand.feedOffset = selectCutFile.manualFeedOffsetLength;
            //ParseCommunicationCommand.sectionLen = selectCutFile.manualFeedBreakLength;
                    case "manualFeedLength":
                        feedLen1 = ConvertType.ConvertShort(line[1]);
                        break;
                    case "manualFeedOffsetLength":
                        offsetLen1 = ConvertType.ConvertShort(line[1]);
                        break;
                    case "manualFeedBreakLength":
                        breakLen1 = ConvertType.ConvertShort(line[1]);
                        break;
                }
                //if (item1.Length > 2)
                //    operators2.Add(item1);
            }
            if(readParam != null)
            {
                rp.deviceTime = readParam.deviceTime;
                rp.gateWay = readParam.gateWay;
                rp.deviceType = readParam.deviceType;
                rp.ipAddress = readParam.ipAddress;
                rp.subNetMask = readParam.subNetMask;
                rp.versionNum = readParam.versionNum;
            }
            
            reader.Close();
            reader.Dispose();
            file.readParam = rp;
            file.cutFileName = path;
            file.x = x;
            file.y = y;
            file.manualFeedBreakLength = breakLen1;
            file.manualFeedLength = feedLen1;
            file.manualFeedOffsetLength = offsetLen1;
            file.step = 2;
            return file;
        }
        //public static bool IsFindCommand(byte command)
        //{
        //    string t = "0x" + command.ToString("x2");
        //    foreach (string item in commandList)
        //    {
        //        if(item == t)
        //        {
        //            return true;
        //        }
        //    }
        //    return false;
        //}
    }

    /// <summary>
    /// 设备状态
    /// </summary>
    public class DeviceState
    {
        //占用2Byte
        /// <summary>
        /// Bit0-X限位[ 1触发  0无效]
        /// </summary>
        public bool xLimit;
        /// <summary>
        /// Bit1-水箱报警 [0正常，1触发]
        /// </summary>
        public bool waterAlert;
        /// <summary>
        /// Bit2-Y限位 [1触发 0无效]
        /// </summary>
        public bool yLimit;
        /// <summary>
        /// Bit3-防护光幕[0正常 1触发]
        /// </summary>
        public bool securityLightCurtains;
        /// <summary>
        /// Bit4-工作状态 
        /// Bit5-工作状态[00 停止 01切割 10暂停]
        /// </summary>
        public WorkState workState;
        /// <summary>
        /// Bit6-原点感应器[1触发 0无效]
        /// </summary>
        public bool originInductor;

        //占用2Byte
        /// <summary>
        /// Bit0-顶布电磁阀[1启动 0停止]
        /// </summary>
        public bool topClothSolenoidValve;
        /// <summary>
        /// Bit3-夹布电磁阀[1启动 0停止]
        /// </summary>
        public bool folderClothSolenoidValve;
        /// <summary>
        /// Bit1-送料报警1触发 0无效]
        /// </summary>
        public bool feedAlert;
        /// <summary>
        /// Bit4-布料感应器1[ 1触发  0无效]
        /// </summary>
        public bool clothInductor;


        /// <summary>
        /// Bit5-气压报警[ 1触发  0无效] 2016-8-16
        /// </summary>
        public bool airPressure;

        //btn_DownObjectCloth
        /// <summary>
        /// Bit6-压布辊报警[ 1抬起  0落下] 2016-8-16
        /// </summary>
        public bool downObjectCloth;

        /// <summary>
        /// Bit0-开盖报警[ 1触发  0无效]
        /// </summary>
        public bool openBox;
        /// <summary>
        /// Bit3-手动上料报警[ 1触发  0无效]
        /// </summary>
        public bool manualFeed;
        /// <summary>
        /// Bit2   0手动/1自动
        /// </summary>
        public bool isManual;
        /// <summary>
        /// 完成切割数量 2
        /// </summary>
        public int cutCount;
        /// <summary>
        /// x坐标  4    / 90
        /// </summary>
        public int X;
        /// <summary>
        /// y坐标  4    / 90
        /// </summary>
        public int Y;
        /// <summary>
        /// 是否已经完成初始化
        /// </summary>
        public bool isInitial = false;
        /// <summary>
        /// 初始化设备状态
        /// </summary>
        /// <param name="data"></param>
        public void InitialDeviceState(byte[] data)
        {
            try
            {
                int index = 0;
                if (data[index] == 0x11)
                {
                    index += 1;
                    List<char> data1 = new List<char>();
                    data1.AddRange(ConvertType.ToBit(data[index]));
                    index += 1;
                    if (data1[7] == '0')
                        xLimit = false;
                    else
                        xLimit = true;
                    if (data1[6] == '0')
                        waterAlert = false;
                    else
                        waterAlert = true;
                    if (data1[5] == '0')
                        yLimit = false;
                    else
                        yLimit = true;
                    if (data1[4] == '0')
                        securityLightCurtains = false;
                    else
                        securityLightCurtains = true;

                    if (data1[3] == '0' && data1[2] == '0')
                        workState = WorkState.Stop;
                    else if (data1[3] == '1' && data1[2] == '0')
                        workState = WorkState.Start;
                    else if (data1[3] == '0' && data1[2] == '1')
                        workState = WorkState.Pause;
                    else if (data1[3] == '1' && data1[2] == '1')
                        workState = WorkState.Feed;

                    if (data1[1] == '0')
                        originInductor = false;
                    else
                        originInductor = true;

                    data1.AddRange(ConvertType.ToBit(data[index]));
                    index += 1;

                    List<char> data2 = new List<char>();
                    data2.AddRange(ConvertType.ToBit(data[index]));
                    index += 1;
                    //if (data2[7] == '0')
                    //    topClothSolenoidValve = false;
                    //else
                    //    topClothSolenoidValve = true;
                    //bit-0 开盖报警
                    if (data2[7] == '0')
                        openBox = false;
                    else
                        openBox = true;

                    //bit-1 送料报警
                    if (data2[6] == '0')
                        feedAlert = false;
                    else
                        feedAlert = true;

                    //bit-2 手动自动
                    if (data2[5] == '0')
                        isManual = false;
                    else
                        isManual = true;

                    //bit-3 手动上料  1报警 0正常 上料架报警manualFeed
                    if (data2[4] == '0')
                        manualFeed = false;
                    else
                        manualFeed = true;
                    //if (data2[4] == '0')
                    //    folderClothSolenoidValve = false;
                    //else
                    //    folderClothSolenoidValve = true;
                    //Bit4-布料感应器
                    if (data2[3] == '0')
                        clothInductor = false;
                    else
                        clothInductor = true;

                    //Bit5-气压报警
                    if (data2[2] == '0')
                        airPressure = false;
                    else
                        airPressure = true;
                    //Bit6-压布辊报警
                    if (data2[1] == '0')
                        downObjectCloth = false;
                    else
                        downObjectCloth = true;

                    data2.AddRange(ConvertType.ToBit(data[index]));
                    index += 1;


                    //cutCount = ConvertType.ToShort(data, 5);

                    cutCount = ConvertType.ToInt32(data, ref index);//5

                    X = ConvertType.ToInt32(data, ref index);//9
                    Y = ConvertType.ToInt32(data, ref index);//13
                    isInitial = true;
                }
            }
            catch (Exception ex)
            {
                ParseCommunicationCommand.WriteFileCommand(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "InitialDeviceState\r\n");

            }
        }
    }

    /// <summary>
    /// 读取参数
    /// </summary>
    public class ReadParam
    {
        ///// <summary>
        ///// 切割初始速度2Byte
        ///// </summary>
        //public short cutInitalSpeed;
        public byte cutInitalSpeed1;
        ///// <summary>
        ///// 切割最高速度2Byte
        ///// </summary>
        public byte cutMaxSpeed1;
        //public short cutMaxSpeed;
        /// <summary>
        /// 切割加速时间2Byte
        /// </summary>
        public short cutSpeedUpTime;

        ///// <summary>
        ///// 空走速度2Byte
        ///// </summary>
        public byte testWorkSpeed1;
        //public short testWorkSpeed;
        ///// <summary>
        ///// 移框速度2Byte
        ///// </summary>
        public byte moveFrameSpeed1;
        //public short moveFrameSpeed;
        ///// <summary>
        ///// 切断速度2Byte
        ///// </summary>
        public byte cutSpeed1;
        //public short cutSpeed;
        /// <summary>
        /// 最大光强1Byte
        /// </summary>
        public byte maxLightIntensity;
        /// <summary>
        /// 最小光强1Byte
        /// </summary>
        public byte minLightIntensity;

        /// <summary>
        /// B最大光强1Byte
        /// </summary>
        public byte maxLightIntensityB;
        /// <summary>
        /// B最小光强1Byte
        /// </summary>
        public byte minLightIntensityB;
        /// <summary>
        /// 自动送料1Byte 0否 手动，发1是 自动
        /// </summary>
        public byte autoFeed;
        /// <summary>
        /// 送料补偿1Byte
        /// </summary>
        public byte feedCompensate;
        ///// <summary>
        ///// 送料速度1Byte U轴
        ///// </summary>
        //public byte feedSpeedU;
        ///// <summary>
        ///// 送料速度1Byte X轴
        ///// </summary>
        //public byte feedSpeedX;
        /// <summary>
        /// 夹布开关
        /// </summary>
        public byte clothKey;
        /// <summary>
        /// 压料辊开关 
        /// </summary>
        public byte pushFeedKey;
        /// <summary>
        /// B激光头开关 
        /// </summary>
        public byte laserBKey;

        /// <summary>
        /// 送料速度2Byte U轴
        /// </summary>
        public short feedSpeedU1;
        /// <summary>
        /// 送料速度2Byte X轴
        /// </summary>
        public short feedSpeedX1;
        /// <summary>
        /// 布料检测1Byte 0不检测，1检测
        /// </summary>
        public byte clothTest;
    
        /// <summary>
        /// 工作结束是否切断 1Byte 0不切断，1切断 2016-8-16
        /// </summary>
        public byte needCut;
        /// <summary>
        /// 切断偏移
        /// </summary>
        public byte cutOffset;

        /// <summary>
        /// 机头间距 2Byte (单位：mm 毫米)
        /// </summary>
        public short distanceAB;

        /// <summary>
        /// 机器类型1Byte
        /// </summary>
        public short deviceType;
        /// <summary>
        /// X长度校正4Byte
        /// </summary>
        public int xLenCorrection1;
        //public float xLenCorrection;
        /// <summary>
        /// Y长度校正4Byte
        /// </summary>
        public int yLenCorrection1;
        //public float yLenCorrection;
        /// <summary>
        /// IP地址4Byte
        /// </summary>
        public byte[] ipAddress;
        /// <summary>
        /// 子网掩码4Byte
        /// </summary>
        public byte[] subNetMask;
        /// <summary>
        /// 网关4Byte
        /// </summary>
        public byte[] gateWay;
        /// <summary>
        /// 主板版本号4Byte
        /// </summary>
        public byte[] versionNum;
        /// <summary>
        /// 日期时间8Byte
        /// </summary>
        public int deviceTime;
        //public DateTime deviceTime = DateTime.MinValue;
        public bool isInital = false;
        /// <summary>
        /// 克隆
        /// </summary>
        /// <returns></returns>
        public object Clone()
        {
            ReadParam newRp = new ReadParam();
            newRp.autoFeed = autoFeed;
            newRp.clothTest = clothTest;
            newRp.cutInitalSpeed1 = cutInitalSpeed1;
            newRp.cutMaxSpeed1 = cutMaxSpeed1;
            newRp.cutSpeed1 = cutSpeed1;
            newRp.cutSpeedUpTime = cutSpeedUpTime;
            //newRp.deviceTime = deviceTime;
            newRp.deviceTime = deviceTime;
            newRp.deviceType = deviceType;
            
            newRp.feedCompensate = feedCompensate;
            newRp.feedSpeedU1 = feedSpeedU1;
            newRp.feedSpeedX1 = feedSpeedX1;
            newRp.clothKey = clothKey;
            newRp.pushFeedKey = pushFeedKey;
            //laserBKey
            newRp.laserBKey = laserBKey;

            newRp.cutOffset = cutOffset;
            newRp.needCut = needCut;
            newRp.gateWay = gateWay;
            newRp.ipAddress = ipAddress;
            newRp.maxLightIntensity = maxLightIntensity;
            newRp.minLightIntensity = minLightIntensity;

            //B激光头最大最小光强
            newRp.maxLightIntensityB = maxLightIntensityB;
            newRp.minLightIntensityB = minLightIntensityB;

            newRp.distanceAB = distanceAB;
            //

            newRp.moveFrameSpeed1 = moveFrameSpeed1;
            newRp.subNetMask = subNetMask;
            newRp.testWorkSpeed1 = testWorkSpeed1;
            newRp.versionNum = versionNum;
            newRp.xLenCorrection1 = xLenCorrection1;
            newRp.yLenCorrection1 = yLenCorrection1;
            newRp.isInital = isInital;
            return newRp;
        }

        public bool Equals(ReadParam rp)
        {
            try
            {
                if (rp == null)
                    return false;
                if (rp.autoFeed.Equals(autoFeed) &&
            rp.clothTest.Equals(clothTest) &&
            rp.cutInitalSpeed1.Equals(cutInitalSpeed1) &&
            rp.cutMaxSpeed1.Equals(cutMaxSpeed1) &&
            rp.cutSpeed1.Equals(cutSpeed1) &&
            rp.cutSpeedUpTime.Equals(cutSpeedUpTime) &&
            rp.deviceTime.Equals(deviceTime) &&
            rp.deviceType.Equals(deviceType) &&
            rp.feedCompensate.Equals(feedCompensate) &&
            rp.clothKey.Equals(clothKey) &&
            rp.pushFeedKey.Equals(pushFeedKey) &&
            rp.laserBKey.Equals(laserBKey) &&
            rp.cutOffset.Equals(cutOffset) &&
            rp.needCut.Equals(needCut) &&
            rp.feedSpeedU1.Equals(feedSpeedU1) &&
            rp.feedSpeedX1.Equals(feedSpeedX1) &&
            rp.gateWay.Equals(gateWay) &&
            rp.ipAddress.Equals(ipAddress) &&
            rp.maxLightIntensity.Equals(maxLightIntensity) &&
            rp.minLightIntensity.Equals(minLightIntensity) &&
            rp.maxLightIntensityB.Equals(maxLightIntensityB) &&
            rp.minLightIntensityB.Equals(minLightIntensityB) &&
            rp.distanceAB.Equals(distanceAB) &&
            rp.moveFrameSpeed1.Equals(moveFrameSpeed1) &&
            rp.subNetMask.Equals(subNetMask) &&
            rp.testWorkSpeed1.Equals(testWorkSpeed1) &&
            rp.versionNum.Equals(versionNum) &&
            rp.xLenCorrection1.Equals(xLenCorrection1) &&
            rp.yLenCorrection1.Equals(yLenCorrection1) &&
            rp.isInital.Equals(isInital))
                    return true;

                return false;
            }
            catch (Exception ex)
            {
                ParseCommunicationCommand.WriteFileCommand(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "Equals\r\n");
                return false;
            }
            
        }
        public byte[] GetByteData()
        {
            //if (isInital)
            {
                List<byte> data = new List<byte>();
                data.Add(0x03);
                //data.AddRange(BitConverter.GetBytes(cutInitalSpeed));
                //data.AddRange(BitConverter.GetBytes(cutMaxSpeed));
                data.Add(cutInitalSpeed1);
                data.Add(cutMaxSpeed1);
                data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(cutSpeedUpTime)));
                data.Add(testWorkSpeed1);
                data.Add(moveFrameSpeed1);
                data.Add(cutSpeed1);
                //data.AddRange(BitConverter.GetBytes(testWorkSpeed));
                //data.AddRange(BitConverter.GetBytes(moveFrameSpeed));
                //data.AddRange(BitConverter.GetBytes(cutSpeed));
                data.Add(maxLightIntensity);
                data.Add(minLightIntensity);

                //B头光强设置
                data.Add(maxLightIntensityB);
                data.Add(minLightIntensityB);

                data.Add(autoFeed);
                data.Add(feedCompensate);
                data.Add(clothKey);
                data.Add(pushFeedKey);
                data.Add(laserBKey);
                //完成工作后是否需要切断 2016-8-16
                data.Add(needCut);
                //完成工作后是否需要切断 2016-8-16
                data.Add(cutOffset);
                data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(feedSpeedU1)));
                data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(feedSpeedX1)));
                //data.Add(clothTest);
                //AB头间距
                data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(distanceAB)));
                
                data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(deviceType)));
                data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(xLenCorrection1)));
                data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(yLenCorrection1)));
                data.AddRange(ipAddress);
                data.AddRange(subNetMask);
                data.AddRange(gateWay);
                data.AddRange(versionNum);
                data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(deviceTime)));
                return data.ToArray();
            }
            return null;
        }

        //public byte[] GetByteData1()
        //{
        //    if (isInital)
        //    {
        //        List<byte> data = new List<byte>();
        //        data.Add(0x03);
        //        //data.AddRange(BitConverter.GetBytes(cutInitalSpeed));
        //        //data.AddRange(BitConverter.GetBytes(cutMaxSpeed));
        //        data.Add(cutInitalSpeed1);
        //        data.Add(cutMaxSpeed1);
        //        data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(cutSpeedUpTime)));
        //        data.Add(testWorkSpeed1);
        //        data.Add(moveFrameSpeed1);
        //        data.Add(cutSpeed1);
        //        //data.AddRange(BitConverter.GetBytes(testWorkSpeed));
        //        //data.AddRange(BitConverter.GetBytes(moveFrameSpeed));
        //        //data.AddRange(BitConverter.GetBytes(cutSpeed));
        //        data.Add(maxLightIntensity);
        //        data.Add(minLightIntensity);
        //        data.Add(autoFeed);
        //        data.Add(feedCompensate);
        //        data.Add(feedSpeed);
        //        data.Add(clothTest);
        //        data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(deviceType)));
        //        data.AddRange((BitConverter.GetBytes(xLenCorrection1)));
        //        data.AddRange((BitConverter.GetBytes(yLenCorrection1)));
        //        data.AddRange(ipAddress);
        //        data.AddRange(subNetMask);
        //        data.AddRange(gateWay);
        //        data.AddRange(versionNum);
        //        data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(deviceTime)));
        //        return data.ToArray();
        //    }
        //    return null;
        //}
        /// <summary>
        /// 读取参数 数据解析
        /// </summary>
        /// <param name="data"></param>
        public void InitialReadParamData(byte[] data)
        {
            //if (data == null || data.Length < 44)
            //for (int i = 0; i < data.Length; i++)
            //{
            //    Console.Write("[" + data[i].ToString("x") + "]");
            //}
            if (data == null)
            {
                return;
            }
            try
            {
                string ss = "";
                for (int i = 0; i < data.Length; i++)
                {
                    ss += "[" + data[i].ToString("x2") + "]";
                }
                //Console.WriteLine(ss);
                int index = 0;
                if (data[index] == 0x12)
                {
                    index += 1;
                    cutInitalSpeed1 = data[index];//1
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;
                    cutMaxSpeed1 = data[index];//2
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;

                    cutSpeedUpTime = ConvertType.ToShort(data, ref index);//3
                    testWorkSpeed1 = data[index];//5
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;
                    moveFrameSpeed1 = data[index];//6
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;
                    cutSpeed1 = data[index];//7
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;
                    maxLightIntensity = data[index];//8
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;
                    minLightIntensity = data[index];//9
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;

                    //B激光头光强
                    maxLightIntensityB = data[index];//8
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;
                    minLightIntensityB = data[index];//9
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;
                    //

                    autoFeed = data[index];//10
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;
                    feedCompensate = data[index];//11
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;
                    clothKey = data[index];//12
                    //index += 1;
                    //feedSpeedU = data[index];//12
                    //index += 1;
                    //feedSpeedX = data[index];//13
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    //布料检测
                    //index += 1;
                    //clothTest = data[index];//13
                    //end 布料检测
                    //Console.WriteLine("[" + data[index].ToString("x2") + "]");
                    index += 1;
                    pushFeedKey = data[index];//12
                    index += 1;
                    laserBKey = data[index];//12
                    index += 1;
                    //工作结束是否需要切断 2016-8-16
                    needCut = data[index];//12
                    index += 1;
                    //工作结束切断偏移 2016-8-16
                    cutOffset = data[index];//12
                    index += 1;

                    feedSpeedU1 = ConvertType.ToShort(data, ref index);//14
                    feedSpeedX1 = ConvertType.ToShort(data, ref index);//14

                    //AB头间距
                    distanceAB = ConvertType.ToShort(data, ref index);//14

                    deviceType = ConvertType.ToShort(data, ref index);//14
                    xLenCorrection1 = ConvertType.ToInt32(data, ref index);//16
                    yLenCorrection1 = ConvertType.ToInt32(data, ref index);//20
                    ipAddress = ConvertType.CopyToArray(data, ref index, 4);//24
                    subNetMask = ConvertType.CopyToArray(data, ref index, 4);//28
                    gateWay = ConvertType.CopyToArray(data, ref index, 4);//32
                    versionNum = ConvertType.CopyToArray(data, ref index, 4);//36
                    deviceTime = ConvertType.ToInt32(data, ref index);//40
                    //cutInitalSpeed = ConvertType.ToShort(data, 1);
                    //cutMaxSpeed = ConvertType.ToShort(data, 3);
                    //cutSpeedUpTime = ConvertType.ToShort(data, 5);
                    //testWorkSpeed = ConvertType.ToShort(data, 7);
                    //moveFrameSpeed = ConvertType.ToShort(data, 9);
                    //cutSpeed = ConvertType.ToShort(data, 11);
                    //maxLightIntensity = data[13];
                    //minLightIntensity = data[14];
                    //autoFeed = data[15];
                    //feedCompensate = data[16];
                    //feedSpeed = data[17];
                    //clothTest = data[18];
                    //deviceType = ConvertType.ToShort(data,19);
                    //xLenCorrection = ConvertType.ToFloat(data, 21);
                    //yLenCorrection = ConvertType.ToFloat(data, 25);
                    //ipAddress = ConvertType.CopyToArray(data, 29, 4);
                    //subNetMask = ConvertType.CopyToArray(data, 33, 4);
                    //gateWay = ConvertType.CopyToArray(data, 37, 4);
                    //versionNum = ConvertType.CopyToArray(data, 41, 4);
                    //deviceTime = ConvertType.ToInt32(data, 45);
                    isInital = true;
                }
            }
            catch (Exception ex)
            {
                ParseCommunicationCommand.WriteFileCommand(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "InitialReadParamData\r\n");
            }
        }
    }
    public class CommuncationData
    {
        public byte[] writeData = null;
        public byte[] readData = null;
        public bool isWrite = false;
        public bool isRead = false;
        public bool endRead = false;
        public Guid id = Guid.NewGuid();
        public string dataType;

        public string ToString()
        {
            string str = dataType + "\r\n" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss") + "\r\n";
            str += "Write:";
            if (writeData != null)
            {
                for (int i = 0; i < writeData.Length; i++)
                {
                    if (i == writeData.Length - 1)
                        str += "[" + writeData[i].ToString("x2") + "]";
                    else
                        str += "[" + writeData[i].ToString("x2") + "]";
                }
            }
            str += "\r\nRead:";
            if (readData != null)
            {
                for (int i = 0; i < readData.Length; i++)
                {
                    if (i == readData.Length - 1)
                        str += "[" + readData[i].ToString("x2") + "]";
                    else
                        str += "[" + readData[i].ToString("x2") + "]";
                }
            }
            str += "\r\n";
            return str;
        }
    }
    ///// <summary>
    ///// 工控机指令
    ///// </summary>
    //public enum ComputerCommand
    //{
    //    查询设备状态 = 0x01,
    //    读取参数 = 0x02,
    //    设置参数 = 0x03,
    //    定位点设置 = 0x04,
    //    移动步长设置 = 0x05,
    //    移动出光设置 = 0x06,
    //    手动上料开始 = 0x07,
    //    手动上料结束 = 0x08,
    //    取消自动进料 = 0x09,
    //    手动低速送料 = 0x0A,
    //    手动低速退料 = 0x0B,
    //    终止工作 = 0x0C,
    //    取消自动送料 = 0x0D,
    //    顶布测试 = 0x0E,
    //    压布棍升降 = 0x0F,
    //    夹布测试 = 0x10,
    //    吹气测试 = 0x11,
    //    自动送料测试 = 0x12,
    //    激光点射 = 0x13,
    //    激光点射光强 = 0x14,
    //    激光点射延时 = 0x15,
    //    手动自动 = 0x16,
    //    文件信息传输 = 0x17,
    //    文件数据传输 = 0x18,
    //}

    ///// <summary>
    ///// 主板应答指令
    ///// </summary>
    //public enum MainBoardCommand
    //{
    //    查询设备状态 = 0x11,
    //    读取参数 = 0x12,
    //    设置参数 = 0x13,
    //    定位点设置 = 0x14,
    //    移动步长设置 = 0x15,
    //    移动出光设置 = 0x16,
    //    手动上料开始 = 0x17,
    //    手动上料结束 = 0x18,
    //    取消自动进料 = 0x19,
    //    终止工作 = 0x1C,
    //    取消自动送料 = 0x1D,
    //    顶布测试 = 0x1E,
    //    压布棍升降 = 0x1B,
    //    夹布测试 = 0x20,
    //    吹气测试 = 0x21,
    //    自动送料测试 = 0x22,
    //    激光点射 = 0x23,
    //    激光点射光强 = 0x24,
    //    激光点射延时 = 0x25,
    //    手动自动 = 0x26,
    //    文件信息传输 = 0x27,
    //    文件数据传输 = 0x28
    //}

    ///// <summary>
    ///// 设置参数返回状态
    ///// </summary>
    //public enum SetParamReturnState
    //{
    //    设置成功=0x1A,
    //    设置失败 = 0x10
    //}

    //public enum SetParamItem
    //{
    //    切割初始速度2,
    //    切割最高速度2,
    //    切割加速时间2,
    //    空走速度2,
    //    移框速度2,
    //    切断速度2,
    //    最大光强1,
    //    最小光强1,
    //    自动送料1,
    //    送料补偿1,
    //    送料速度1,
    //    布料检测1,
    //    机器类型1,
    //    X长度校正4,
    //    Y长度校正4,
    //    IP地址4,
    //    子网掩码4,
    //    网关4,
    //    主板版本号4,
    //    日期时间8,
    //}
    //public enum SetLaserPoint
    //{
    //    激光点射光强,
    //    激光点射延时,
    //}

    /// <summary>
    /// 工作状态
    /// </summary>
    public enum WorkState
    {
        /// <summary>
        /// 开始
        /// </summary>
        Start,
        /// <summary>
        /// 暂停
        /// </summary>
        Pause,
        /// <summary>
        /// 停止
        /// </summary>
        Stop,
        /// <summary>
        /// 进料
        /// </summary>
        Feed,
    }
}