using CommonModule;
using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class frm_OperationPanel : frm_BaseForm
    {
        private const int WS_EX_TOOLWINDOW = 0x00000080;
        private const int WS_EX_NOACTIVATE = 0x08000000;
        private Form focusForm;

        public Form FocusForm
        {
            get { return focusForm; }
            set
            {
                if (focusForm != null)
                {
                    focusForm.VisibleChanged -= focusForm_VisibleChanged;
                }
                focusForm = value;
                if(focusForm != null)
                {
                    focusForm.VisibleChanged += focusForm_VisibleChanged;
                }
                IsMouseDown = false;
            }
        }

        private IpInputTextbox focusIpText;

        public IpInputTextbox FocusIpText
        {
            get { return focusIpText; }
            set 
            { 
                focusIpText = value;
                this.txt_Value.Visible = false;
                //this.ip_Value.Visible = true;
                IsMouseDown = false;
            }
        }
        private Rich_TextBox focusText;

        public Rich_TextBox FocusText
        {
            get { return focusText; }
            set 
            {
                focusText = value;
                this.txt_Value.MaxValue = focusText.MaxValue;
                this.txt_Value.MinValue = focusText.MinValue;
                this.txt_Value.Text = "";
                //this.ip_Value.Visible = false;
                this.txt_Value.Visible = true;
                IsMouseDown = false;
            }
        }

        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams cp = base.CreateParams;
                //cp.Style = (int)WindowStyles.WS_THICKFRAME | (int)WindowStyles.WS_CHILD;
                cp.ExStyle |= (WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
                cp.Parent = IntPtr.Zero; // Keep this line only if you used UserControl
                return cp;
            }
        }
        SetFormControlValue setFormControlValue1;
        public frm_OperationPanel()
        {
            IsCanMove = true;
            InitializeComponent();
            this.txt_Value.isPanel = true;
            //this.ip_Value.isPanel = true;
            foreach (Control item in this.Controls)
            {
                if(item is Button)
                {
                    item.Click += button11_Click;
                }
            }
            this.Paint += frm_OperationPanel_Paint;
            setFormControlValue1 = new SetFormControlValue(SetFormControlValue1);
            this.VisibleChanged += frm_OperationPanel_VisibleChanged;
        }

        void frm_OperationPanel_VisibleChanged(object sender, EventArgs e)
        {
            IsMouseDown = false;
        }

        void frm_OperationPanel_Paint(object sender, PaintEventArgs e)
        {
            if (Visible && focusForm != null)
                focusForm.Focus(); 
        }
        void focusForm_VisibleChanged(object sender, EventArgs e)
        {
            if (focusForm != null && !focusForm.Visible)
            {
                Visible = false;
            }
        }

        //private void button11_Click(object sender, EventArgs e)
        //{
        //    if (focusForm != null)
        //        focusForm.Focus();

        //    Button btn = (Button)sender;
        //    if (txt_Value.Visible)
        //        SetRichTextBox(btn);
        //    else
        //        SetIpTextBox(btn);
        //}

        //private void SetRichTextBox(Button btn)
        //{
        //    switch (btn.Name)
        //    {
        //        case "btn_0":
        //            this.txt_Value.Text += "0";
        //            break;
        //        case "btn_1":
        //            this.txt_Value.Text += "1";
        //            break;
        //        case "btn_2":
        //            this.txt_Value.Text += "2";
        //            break;
        //        case "btn_3":
        //            this.txt_Value.Text += "3";
        //            break;
        //        case "btn_4":
        //            this.txt_Value.Text += "4";
        //            break;
        //        case "btn_5":
        //            this.txt_Value.Text += "5";
        //            break;
        //        case "btn_6":
        //            this.txt_Value.Text += "6";
        //            break;
        //        case "btn_7":
        //            this.txt_Value.Text += "7";
        //            break;
        //        case "btn_8":
        //            this.txt_Value.Text += "8";
        //            break;
        //        case "btn_9":
        //            this.txt_Value.Text += "9";
        //            break;
        //        case "btn_Clear":
        //            this.txt_Value.Text = "0";
        //            break;
        //        case "btn_P":
        //            this.txt_Value.Text += ".";
        //            break;
        //        case "btn_Enter":
        //            this.focusText.Text = this.txt_Value.Text;
        //            this.Visible = false;
        //            break;
        //        case "btn_Return":
        //            Visible = false;
        //            break;
        //        case "btn_ClearAll":
        //            if (txt_Value.Text.Length > 0)
        //                this.txt_Value.Text = this.txt_Value.Text.Substring(0, txt_Value.Text.Length - 1);
        //            break;
        //    }
        //}
        //private void SetIpTextBox(Button btn)
        //{
        //    Keys keyValue = Keys.None;
        //    switch (btn.Name)
        //    {
        //        case "btn_0":
        //            keyValue = Keys.NumPad0;
        //            break;
        //        case "btn_1":
        //            keyValue = Keys.NumPad1;
        //            break;
        //        case "btn_2":
        //            keyValue = Keys.NumPad2;
        //            break;
        //        case "btn_3":
        //            keyValue = Keys.NumPad3;
        //            break;
        //        case "btn_4":
        //            keyValue = Keys.NumPad4;
        //            break;
        //        case "btn_5":
        //            keyValue = Keys.NumPad5;
        //            break;
        //        case "btn_6":
        //            keyValue = Keys.NumPad6;
        //            break;
        //        case "btn_7":
        //            keyValue = Keys.NumPad7;
        //            break;
        //        case "btn_8":
        //            keyValue = Keys.NumPad8;
        //            break;
        //        case "btn_9":
        //            keyValue = Keys.NumPad9;
        //            break;
        //        case "btn_Clear":
        //            keyValue = Keys.Back;
        //            break;
        //        case "btn_P":
        //            keyValue = Keys.Decimal;
        //            break;
        //        case "btn_Enter":
        //            this.Visible = false;
        //            break;
        //        case "btn_Return":
        //            this.Visible = false;
        //            break;
        //        case "btn_ClearAll":
        //            keyValue = Keys.Clear;
        //            break;
        //    }
        //    if (keyValue != Keys.None)
        //        GetForegroundWindows.keybd_event(keyValue, 0, 0, 0);
        //}
        private void button11_Click(object sender, EventArgs e)
        {
            if (focusForm != null)
                focusForm.Focus();

            Button btn = (Button)sender;
            Keys keyValue = Keys.None;
            switch (btn.Name)
            {
                case "btn_0":
                    keyValue = Keys.NumPad0;
                    break;
                case "btn_1":
                    keyValue = Keys.NumPad1;
                    break;
                case "btn_2":
                    keyValue = Keys.NumPad2;
                    break;
                case "btn_3":
                    keyValue = Keys.NumPad3;
                    break;
                case "btn_4":
                    keyValue = Keys.NumPad4;
                    break;
                case "btn_5":
                    keyValue = Keys.NumPad5;
                    break;
                case "btn_6":
                    keyValue = Keys.NumPad6;
                    break;
                case "btn_7":
                    keyValue = Keys.NumPad7;
                    break;
                case "btn_8":
                    keyValue = Keys.NumPad8;
                    break;
                case "btn_9":
                    keyValue = Keys.NumPad9;
                    break;
                case "btn_Clear":
                    if (FocusIpText != null)
                        focusIpText.IP = "";
                    if (focusText != null)
                        focusText.Text = "";
                    break;
                case "btn_P":
                    keyValue = Keys.Decimal;
                    break;
                case "btn_Enter":
                    keyValue = Keys.Tab;
                    this.Visible = false;
                    break;
                case "btn_Return":
                    this.Visible = false;
                    break;
                case "btn_ClearAll":
                    keyValue = Keys.Back;
                    break;
            }
            if (keyValue != Keys.None)
                GetForegroundWindows.keybd_event(keyValue, 0, 0, 0);
        }

        private IntPtr GetForegroundWindow()
        {
            IntPtr hWnd = GetForegroundWindows.GetForegroundWindow();

            uint processId;
            uint threadid = GetForegroundWindows.GetWindowThreadProcessId(hWnd, out processId);

            GUITHREADINFO lpgui = new GUITHREADINFO();
            lpgui.cbSize = Marshal.SizeOf(lpgui);

            if (GetForegroundWindows.GetGUIThreadInfo(threadid, ref lpgui))
            {
                if (lpgui.hwndCaret != 0)
                {

                    //textBox1.Focus();
                    //GetForegroundWindows.keybd_event(Keys.A, 0, 0, 0);
                    return hWnd;
                }
            }
            return IntPtr.Zero;
        }

        bool IsMouseDown = false;
        DateTime mouseDownTime;
        int delayTime = 1000;
        private void btn_ClearAll_MouseDown(object sender, MouseEventArgs e)
        {
            IsMouseDown = true;
            mouseDownTime = DateTime.Now;
            if (!backgroundWorker1.IsBusy)
                backgroundWorker1.RunWorkerAsync();
        }

        private void btn_ClearAll_MouseUp(object sender, MouseEventArgs e)
        {
            IsMouseDown = false;
        }

        private void backgroundWorker1_DoWork(object sender, System.ComponentModel.DoWorkEventArgs e)
        {
            
            while(true)
            {
                //try
                //{

                //}
                //catch (Exception)
                //{
                    
                //}
                if (IsMouseDown && DateTime.Now.Subtract(mouseDownTime).TotalMilliseconds > 1500)
                {
                    delayTime = 150;
                    if (focusForm != null && focusForm.InvokeRequired)
                        focusForm.Invoke(setFormControlValue1);
                }
                else if(!IsMouseDown)
                {
                    delayTime = 1000;
                    break;
                }
                System.Threading.Thread.Sleep(delayTime);
            }
        }

        public void SetFormControlValue1()
        {
            button11_Click(btn_ClearAll, null);
        }
    }
}
