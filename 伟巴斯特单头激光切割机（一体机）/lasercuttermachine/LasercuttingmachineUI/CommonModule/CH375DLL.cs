using System;
using System.Runtime.InteropServices;

namespace CommonModule
{
    public static class CH375DLL
    {
        public static Int32[] ReadLength = new Int32[1];
        //1．CH375OpenDevice()
        //功能：打开CH375设备。
        //格式：HANDLE WINAPI CH375OpenDevice (ULONG iIndex );
        //参数：iIndex指定CH375设备序号,0对应第一个设备,-1则自动搜索一个可以被打开的设备并返回序号。
        //传回值：成功 - 返回句柄。
        //        失败 – 无效数据。
        //说明：此函数用来打开相应的USB设备，如果打开成功就可以与之USB设备进行通讯了。
        [DllImport("CH375DLL.DLL", EntryPoint = "CH375OpenDevice", ExactSpelling = false, SetLastError = true)]
        public static extern IntPtr CH375OpenDevice(Int16 iIndex);
        //2．CH375GetUsbID()
        //功能：获取USB设备ID。
        //格式：ULONG	WINAPI CH375GetUsbID (ULONG iIndex );
        //参数：iIndex指定CH375设备序号。
        //传回值：成功 - 设备ID号，低16位为厂商ID,高16位为产品ID。
        //        失败 – 0无效ID。
        //说明：此函数用来获取设备的ID号，获取ID号是用来是否连接了正确的设备。
        [DllImport("CH375DLL.DLL", EntryPoint = "CH375GetUsbID", ExactSpelling = false, SetLastError = true)]
        public static extern Int16 CH375GetUsbID(Int16 iIndex);
        //3．CH375WriteData()
        //功能：写出数据块。
        //格式：BOOL	WINAPI CH375WriteData(ULONG iIndex,PVOID iBuffer,	PULONG ioLength );
        //参数：iIndex指定CH375设备序号。
        //iBuffer指向一个缓冲区,放置准备写出的数据。
        //ioLength指向长度单元,输入时为准备写出的长度,返回后为实际写出的长度。
        //传回值：成功 – true 此时ioLength中为实际写出的数据长度。
        //        失败 – false。
        //说明：调用此函数时一定要注意实际写出的数据长度，以免数据没有写完。
        [DllImport("CH375DLL.DLL", EntryPoint = "CH375WriteData", ExactSpelling = false, SetLastError = true)]
        public static extern bool CH375WriteData(
        Int16 iIndex, [MarshalAs(UnmanagedType.LPArray)]  byte[] iBuffer, [MarshalAs(UnmanagedType.LPArray)]  Int32[] ioLength);   //写单片机缓存

        //4. CH375ReadData()
        //功能：读取数据块。
        //格式：BOOL WINAPI CH375ReadData(ULONG	iIndex, PVOID oBuffer, PULONG ioLength );
        //参数：iIndex指定CH375设备序号。
        //    oBuffer指向一个足够大的缓冲区,用于保存读取的数据。
        //    ioLength指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度。
        //传回值：成功 – true 此时ioLength中为实际读取的数据长度。
        //        失败 – false。
        //说明：调用此函数时一定要注意实际读取的数据长度，以免出错。
        [DllImport("CH375DLL.DLL", EntryPoint = "CH375ReadData", ExactSpelling = false, SetLastError = true)]
        public static extern bool CH375ReadData(
        Int16 iIndex, [MarshalAs(UnmanagedType.LPArray)] byte[] oBuffer, [MarshalAs(UnmanagedType.LPArray)]  Int32[] ioLength);//读单片机缓存

        //5．CH375CloseDevice()
        //功能：关闭CH375设备。
        //格式：VOID WINAPI CH375CloseDevice(ULONG	iIndex );
        //参数：iIndex指定CH375设备序号。
        //传回值：无。
        //说明：此函数用来关闭一个USB设备。
        [DllImport("CH375DLL.DLL", EntryPoint = "CH375CloseDevice", ExactSpelling = false, SetLastError = true)]
        public static extern void CH375CloseDevice(Int16 iIndex);

        public static IntPtr OpenDevice(Int16 iIndex)
        {
            return CH375OpenDevice(iIndex);
        }
        public static Int16 GetUsbID(Int16 iIndex)
        {
            return CH375GetUsbID(iIndex);
        }
        public static void CloseDevice(Int16 iIndex)
        {
            CH375CloseDevice(iIndex);
        }
        public static bool ReadData(short index, byte[] buffer, ref int length)
        {
            ReadLength[0] = length;
            bool r = CH375ReadData(0, buffer, ReadLength);
            length = ReadLength[0];
            return r;
        }

        //写入数据
        public static bool WriteData(short index, byte[] buffer, ref int length)
        {
            ReadLength[0] = length;
            bool r = CH375WriteData(0, buffer, ReadLength);
            length = ReadLength[0];
            return r;
        }
    }
}