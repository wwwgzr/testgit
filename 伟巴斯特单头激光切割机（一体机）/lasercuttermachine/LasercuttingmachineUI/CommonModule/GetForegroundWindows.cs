using System;
using System.Runtime.InteropServices;

namespace CommonModule
{
    public struct GUITHREADINFO
    {
        public int cbSize;
        public int flags;
        public int hwndActive;
        public int hwndFocus;
        public int hwndCapture;
        public int hwndMenuOwner;
        public int hwndMoveSize;
        public int hwndCaret;
        public System.Drawing.Rectangle rcCaret;
    }
    public static  class GetForegroundWindows
    {
        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetGUIThreadInfo(uint idThread, ref GUITHREADINFO lpgui);
        [DllImport("user32.dll")]
        public static extern IntPtr GetForegroundWindow();
        [DllImport("user32.dll", SetLastError = true)]
        public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);

        [DllImport("user32.dll", EntryPoint = "keybd_event", SetLastError = true)]
        public static extern void keybd_event(System.Windows.Forms.Keys bVk, byte bScan, uint dwFlags, uint dwExtraInfo);

        // 申明要使用的dll和api
        [DllImport("User32.dll", EntryPoint = "FindWindow")]
        public extern static IntPtr FindWindow(string lpClassName, string lpWindowName);
        [System.Runtime.InteropServices.DllImportAttribute("user32.dll", EntryPoint = "MoveWindow")]
        public static extern bool MoveWindow(System.IntPtr hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);

        [DllImport("user32.dll")]
        static extern bool SetForegroundWindow(IntPtr hWnd);
    }
}
