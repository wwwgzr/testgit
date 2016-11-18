using CommonModule;
//////////////////////////////////////////////////////////////////////////////////////////////////
//2016-7-18 Version 1.0.0.0
//////////////////////////////////////////////////////////////////////////////////////////////////
using CommunicationModule;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Threading;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public delegate void SetFormControlValue();
    public delegate void SetButtonControlValue(string text);
    /// <summary>
    /// 1024*768
    /// </summary>
    public partial class Form1 : Form
    {
        public Panel SplitPanel
        {
            get { return this.panel2; }
        }
        /// <summary>
        /// 界面菜单控制对象
        /// </summary>
        public MenuController MenuController;
        System.Windows.Forms.Timer timer;
        int time;
        public frm_Load loadForm;
        System.Threading.Timer timer1;
        System.Threading.Timer timerMove;
        System.Threading.Timer timerPreMove;
        SetFormControlValue setReadParamValue;
        SetFormControlValue setDeviceValue;
        SetButtonControlValue setButtonValue;
        DateTime dateNow;
        public frm_Info info_Form;
        public frm_AlertInfo alertInfo_Form;
        bool isWriteS = false;
        /// <summary>
        /// 程序启动写文件尝试次数
        /// </summary>
        int t = 0;
        /// <summary>
        /// 程序启动连接成功后查询次数
        /// </summary>
        int t1 = 0;
        public Form1()
        {
            InitializeComponent();
            paintingPanel1.precent = 0.5f;
            timer = new System.Windows.Forms.Timer();
            timer.Interval = 100;
            time = 0;
            timer.Tick += timer_Tick;
            this.Opacity = 0;
            //this.btn_ConnectState.Text = "未连接";
            loadForm = new frm_Load();
            loadForm.Owner = this;
            Rectangle rect = Screen.PrimaryScreen.WorkingArea;
            ParseCommunicationCommand.userType = 2;
            loadForm.Location = new Point(rect.Width / 2 - loadForm.Width / 2, rect.Height / 2 - loadForm.Height / 2 - 30);
            //loadForm.TopMost = true;
            loadForm.Show(this);
            //if(loadForm.ShowDialog(this) == System.Windows.Forms.DialogResult.OK)
            {

                timer.Start();
                this.KeyPreview = true;
                
                ParseCommunicationCommand.InitialSearchData();
                ParseCommunicationCommand.InitialProjectClass();
                ParseCommunicationCommand.executablePath = Application.ExecutablePath;
                ParseCommunicationCommand.OpenDevice();
                MenuController = new MenuController(this);
                this.panel2.Controls.Add(this.paintingPanel1);
                info_Form = new frm_Info();
                info_Form.Owner = this;
                info_Form.TopMost = true;
                info_Form.Location = new Point((this.SplitPanel.Width - info_Form.Width) / 2, 0);
                alertInfo_Form = new frm_AlertInfo();
                alertInfo_Form.Owner = this;
                alertInfo_Form.TopMost = true;
                alertInfo_Form.Location = new Point((this.SplitPanel.Width - alertInfo_Form.Width) / 2, (this.SplitPanel.Height - alertInfo_Form.Height) / 2);

                btn_ConnectState_TextChanged(null, null);

                
                timer1 = new System.Threading.Timer(new TimerCallback(timer1_Tick));
                timer1.Change(0, ParseCommunicationCommand.ConnectHz);

                timerMove = new System.Threading.Timer(new TimerCallback(timerMove_Tick));
                //timerPreMove
                timerPreMove = new System.Threading.Timer(new TimerCallback(timerPreMove_Tick));
                previewMove = new SetFormControlValue(PreviewMove);

                setReadParamValue = new SetFormControlValue(SetReadParamValue);
                setDeviceValue = new SetFormControlValue(SetDeviceValue);
                setButtonValue = new SetButtonControlValue(SetButtonValue);
                refreshMove = new SetFormControlValue(RefreshMove);
                this.backgroundWorker1.RunWorkerAsync();
                if (ParseCommunicationCommand.lastPltFile.Length > 0)
                {
                    string fileExt = Path.GetExtension(ParseCommunicationCommand.lastPltFile);
                    if (fileExt.ToLower() == ".dxf" && paintingPanel1.OpenDxfFile(ParseCommunicationCommand.lastPltFile))
                    {
                        SetFileName(ParseCommunicationCommand.lastPltFile);
                    }
                    else if (fileExt.ToLower() == ".plt" && paintingPanel1.ReadPltFile(ParseCommunicationCommand.lastPltFile))
                    {
                        SetFileName(ParseCommunicationCommand.lastPltFile);
                    }
                }
                this.userSetParam1.Enabled = false;
                MenuController.SetButtonStyle(btn_MainMenu, 0);
                //paintingPanel1.Invalidate();
            }

            //else
            //{
            //    this.Close();
            //}
            
            //暂时注释
            //ConnectMainBoard();
        }

        #region 路径跟踪测试
        List<PointF> ptList = new List<PointF>();
        SetFormControlValue refreshMove;
        int indexPt = 0;
        WorkState ws = WorkState.Stop;
        DateTime startWorkTime;
        double workTime = 0;
        int moveHz = 500;
        int oldCutNum = 0;
        public WorkState Ws
        {
            get { return ws; }
            set 
            { 
                if(ws != value)
                {
                    WorkState_ValueChange(ws, value);
                    ws = value;
                }
            }
        }

        /// <summary>
        /// 工作状态改变
        /// </summary>
        public void WorkState_ValueChange(WorkState oldWs, WorkState newWs)
        {
            //if ((oldWs == WorkState.Feed || oldWs == WorkState.Start) && (newWs == WorkState.Stop || newWs == WorkState.Pause))
            //    ParseCommunicationCommand.isAlert = false;
            //else
            //    ParseCommunicationCommand.isAlert = false;
            switch (newWs)
            {
                case WorkState.Start:
                    MenuController.isAutoFeed = false;
                    this.userSetParam1.Enabled = false;

                    CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "开始切割" + "\r\n");
                    if (oldWs == WorkState.Pause)
                    {
                        timerMove.Change(0, moveHz);
                    }
                    else
                    {
                        //if (paintingPanel1.FilePath == null || paintingPanel1.FilePath == "" || paintingPanel1.FilePath != ParseCommunicationCommand.lastPltFile)
                        //{
                        //    paintingPanel1.ReadPltFile(ParseCommunicationCommand.lastPltFile);
                        //    paintingPanel1.GetFinishWorkPoint();
                        //}
                        if (ptList != null)
                            ptList.Clear();
                        else
                            ptList = new List<PointF>();
                        #region lasercuttermachine实时路径跟踪未按主板发送数据跟踪
                        //ptList = paintingPanel1.GetAllPathPoints();
                        //indexPt = 0;
                        #endregion
                        paintingPanel1.pathIndex = 0;
                        paintingPanel1.pointIndex = 0;
                        timerMove.Change(0, moveHz);
                    }
                    break;
                case WorkState.Pause:
                    this.userSetParam1.Enabled = false;
                    timerMove.Change(-1, moveHz);
                    CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "暂停工作" + "\r\n");
                    break;
                case WorkState.Stop:
                    if(oldWs == WorkState.Start)
                    {
                        if (MenuController.isMainStop)
                        {
                        }
                        else
                        {
                            paintingPanel1.GetMoveGraphicsPath2();
                            paintingPanel1.Invalidate(true);
                        }
                    }
                    if(ParseCommunicationCommand.userType == 1)
                        this.userSetParam1.Enabled = true;
                    else
                        this.userSetParam1.Enabled = false;

                    MenuController.isAutoFeed = false;
                    timerMove.Change(-1, moveHz);
                    CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "停止工作" + "\r\n");
                    MenuController.isMainStop = false;
                    break;
                case WorkState.Feed:
                    this.userSetParam1.Enabled = false;
                    timerMove.Change(-1, moveHz);
                    CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "正在上料" + "\r\n");
                    break;
                default:
                    //ptList.Clear();
                    break;
            }
        }

        /// <summary>
        /// 主动停止工作
        /// </summary>
        public void StopWork()
        {
            ptList.Clear();
            timerMove.Change(-1, moveHz);
            indexPt = -1;
            paintingPanel1.pathIndex = -1;
            paintingPanel1.pointIndex = -1;
            if (this.paintingPanel1.InvokeRequired)
            {
                paintingPanel1.Invoke(refreshMove);
            }
            else
                paintingPanel1.Invalidate(true);
            MenuController.isMainStop = true;
        }
        public void ReadTxt()
        {
            string filepath = Path.GetDirectoryName(Application.ExecutablePath) + "\\40.txt";
            FileInfo pltFile = new FileInfo(filepath);		//the sourceFile is set.
            StreamReader reader = null;
            reader = pltFile.OpenText();
            if (reader == null)
            {
                return;
            }
            ptList.Clear();
            string strContent = reader.ReadToEnd();
            string[] strCells = strContent.Split(new char[] { ';','\r','\n' }, System.StringSplitOptions.RemoveEmptyEntries);
            float x = 0;
            float y = 0;
            foreach (string item in strCells)
            {
                string[] strlist = item.Split(new char[] { ',' }, System.StringSplitOptions.RemoveEmptyEntries);
                //string[] strlist = str.Split(new string[] { "\r\n" }, System.StringSplitOptions.RemoveEmptyEntries);
                x = ConvertType.ConvertFloat(strlist[0]);
                y = ConvertType.ConvertFloat(strlist[1]);
                if (ptList.Count > 0)
                {
                    if(ptList[ptList.Count-1].X == x && ptList[ptList.Count-1].Y == y)
                    { }
                    else
                        ptList.Add(new PointF(x, y));
                }
                else
                    ptList.Add(new PointF(x, y));
            }

            //string str;
            //while (!reader.EndOfStream)
            //{
            //    str = reader.ReadLine();
            //    string[] strlist = str.Split(new char[] { ',' }, System.StringSplitOptions.RemoveEmptyEntries);
            //    //string[] strlist = str.Split(new string[] { "\r\n" }, System.StringSplitOptions.RemoveEmptyEntries);
            //    x = ConvertType.ConvertFloat(strlist[0]);
            //    y = ConvertType.ConvertFloat(strlist[1]);
            //    ptList.Add(new PointF(x, y));
            //}
            reader.Close();
            reader.Dispose();
        }
        public void WritePathTxt()
        {
            string t = paintingPanel1.GetAllPathPoint();
            File.WriteAllText(Path.GetDirectoryName(Application.ExecutablePath) + "\\40.txt", t);
        }
        void timerMove_Tick(object obj)
        {
            try
            {
                if (ParseCommunicationCommand.deviceState != null )
                if (oldCutNum < ParseCommunicationCommand.cutNum)
                {
                    indexPt = ptList.Count - 1;
                    timer1.Change(-1, moveHz);
                    return;
                }
                if (ParseCommunicationCommand.deviceState != null)
                {
                    if (ptList[ptList.Count - 1].X == -ParseCommunicationCommand.deviceState.Y && ptList[ptList.Count - 1].Y == -ParseCommunicationCommand.deviceState.X)
                    { }
                    else
                        ptList.Add(new PointF(-ParseCommunicationCommand.deviceState.Y, -ParseCommunicationCommand.deviceState.X));
                }
                indexPt = ptList.Count - 1;

                if (this.paintingPanel1.InvokeRequired)
                {
                    paintingPanel1.Invoke(refreshMove);
                }
                else
                    RefreshMove();
                oldCutNum = ParseCommunicationCommand.cutNum;

                #region lasercuttermachine实时路径跟踪未按主板发送数据跟踪
                //if (indexPt >= ptList.Count || oldCutNum < ParseCommunicationCommand.cutNum)
                //{
                //    indexPt = ptList.Count - 1;
                //    timer1.Change(-1, moveHz);
                //    return;
                //}
                //if (this.paintingPanel1.InvokeRequired)
                //{
                //    paintingPanel1.Invoke(refreshMove);
                //}
                //else
                //    RefreshMove();
                //indexPt += 5;
                //oldCutNum = ParseCommunicationCommand.cutNum;
                #endregion

            }
            catch (Exception ex)
            {
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "timerMove_Tick\r\n");
            }
        }

        public void RefreshMove()
        {
            if (ptList.Count == 0 || indexPt == -1)
            {
                return;
            }
           
            if (indexPt >= ptList.Count)
            {
                indexPt = ptList.Count - 1;
            }
            this.paintingPanel1.HandleData1(ptList[indexPt], ref indexPt);
            
            //if (Ws != WorkState.Stop)
            //    this.paintingPanel1.HandleData(ptList[indexPt], ref indexPt);
            //if (indexPt < 0)
            //    ptList.Clear();
            if (indexPt == -1)
                timerMove.Change(-1, moveHz);
            this.paintingPanel1.Invalidate(true);
        }

        #region 工作预览
        List<PointFlat> previewPtList = new List<PointFlat>();
        SetFormControlValue previewMove;
        int previewHz = 100;
        int indexPreview = 0;
        /// <summary>
        /// 工作预览状态 0 停止  1 开始
        /// </summary>
        private int previewStart = 0;
        /// <summary>
        /// 工作预览状态 0 停止  1 开始
        /// </summary>
        public int PreviewStart
        {
            get { return previewStart; }
            set 
            {
                PreviewStart_ValueChange(previewStart, value);
                previewStart = value; 
            }
        }

        private void PreviewStart_ValueChange(int oldPreviewStart, int newPreviewStart)
        {
            switch (newPreviewStart)
            {
                case 1:
                    MenuController.isAutoFeed = false;
                    if (paintingPanel1.FilePath == null || paintingPanel1.FilePath == "")
                        {
                            paintingPanel1.ReadPltFile(paintingPanel1.saveFilePath);
                            paintingPanel1.GetFinishWorkPoint();
                        }
                        else
                        {
                            string fileExt = Path.GetExtension(paintingPanel1.FilePath);
                            if (fileExt.ToLower() == ".dxf")
                            {
                                paintingPanel1.ConvertToPLT(true);
                                paintingPanel1.ReadPltFile(paintingPanel1.saveFilePath);
                                paintingPanel1.GetFinishWorkPoint();
                            }
                        }
                        if (previewPtList != null)
                            previewPtList.Clear();
                        else
                            previewPtList = new List<PointFlat>();
                        #region lasercuttermachine实时路径跟踪未按主板发送数据跟踪

       
                        previewPtList = paintingPanel1.GetAllPathPoints();
                        indexPreview = 0;
                        #endregion
                        paintingPanel1.pathIndex = 0;
                        paintingPanel1.pointIndex = 0;
                        timerPreMove.Change(0, previewHz);
                    break;
                case 0:
                    timerPreMove.Change(-1, previewHz);
                    if (previewPtList != null)
                        previewPtList.Clear();
                    else
                        previewPtList = new List<PointFlat>();

                    paintingPanel1.Invalidate(true);
                    CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "停止工作" + "\r\n");
                    break;
            }
        }
        private void timerPreMove_Tick(object obj)
        {
            if (indexPreview >= previewPtList.Count)
            {
                indexPreview = 0;
                paintingPanel1.pathIndex = 0;
                paintingPanel1.pointIndex = 0;
            }
            if (this.paintingPanel1.InvokeRequired)
            {
                paintingPanel1.Invoke(previewMove);
            }
            else
                PreviewMove();
            indexPreview += 3;
        }

        private void PreviewMove()
        {
            if (previewPtList.Count == 0 || indexPreview == -1)
            {
                return;
            }

            if (indexPreview >= previewPtList.Count)
            {
                indexPreview = previewPtList.Count - 1;
            }
            this.paintingPanel1.HandleData(previewPtList[indexPreview]);

            this.paintingPanel1.Invalidate();
        }
        #endregion

        #endregion


        public void SetReadParamValue()
        {
            SetDeviceParam(ParseCommunicationCommand.readParam);

        }
        public void SetDeviceValue()
        {
            SetDeviceState(ParseCommunicationCommand.deviceState);
        }
        public void SetButtonValue(string text)
        {
            this.btn_ConnectState.Text = text;
        }

        void timer_Tick(object sender, System.EventArgs e)
        {
            if (time > 10)
            {
                if (MenuController.openProject.Opacity < 1)
                {
                    MenuController.openProject.Opacity += 0.1;
                    this.loadForm.Opacity -= 0.1;
                }
                else
                {
                    timer.Stop();
                    this.Opacity = 1;
                    
                    //ShowInfoForm();
                    //alertInfo_Form.Show();
                    //MenuController.ShowMenu();
                    loadForm.Visible = false;
                    loadForm.Opacity = 1;
                }
            }
            if (loadForm.LabelTxt.Contains("..."))
            {
                loadForm.LabelTxt = "初始化";
            }
            else
            {
                if (time % 5 == 0)
                    loadForm.LabelTxt += ".";
            }
            time++;
        }

        void timer1_Tick(object obj)
        {
            try
            {
                dateNow = DateTime.Now;
                if (ParseCommunicationCommand.GetDeviceIndex() == 0 || ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
                {
                    ParseCommunicationCommand.OpenDevice();
                    //ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "尝试连接\r\n");
                    if (ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
                    {
                        //btn_ConnectState.Invoke(setButtonValue, new object[] { "未连接", "" });
                        if (btn_ConnectState.InvokeRequired)
                            btn_ConnectState.Invoke(setButtonValue, "未连接");
                        else
                            this.btn_ConnectState.Text = "未连接";
                    }
                    else
                    {
                        //btn_ConnectState.Invoke(setButtonValue, new object[] { "已连接", "" });
                        ParseCommunicationCommand.GetDeviceIndex();
                        if (btn_ConnectState.InvokeRequired)
                            btn_ConnectState.Invoke(setButtonValue, "已连接");
                        else
                            this.btn_ConnectState.Text = "已连接";
                    }
                }
                else
                {
                    //btn_ConnectState.Invoke(setButtonValue, new object[] { "已连接", "" });
                    if (btn_ConnectState.InvokeRequired)
                        btn_ConnectState.Invoke(setButtonValue, "已连接");
                    else
                        this.btn_ConnectState.Text = "已连接";
                }
            }
            catch (Exception ex)
            {
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "timer1_Tick\r\n");
            }
        }
        private void Form1_KeyUp(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Escape:
                    this.Close();
                    break;
                case Keys.Space:
                    ReadTxt();
                    break;
                case Keys.A:
                    Ws = WorkState.Start;
                    
                    break;
                case Keys.S:
                    Ws = WorkState.Stop;
                    break;
                case Keys.D:
                    if (Ws == WorkState.Start)
                    {
                        Ws = WorkState.Pause;
                        timerMove.Change(-1, moveHz);
                    }
                    else if (Ws == WorkState.Pause)
                    {
                        Ws = WorkState.Start;
                        timerMove.Change(0, moveHz);
                    }
                    else if (Ws == WorkState.Stop)
                    {

                    }
                    break;
                case Keys.Q:
                    WritePathTxt();
                    break;
                default:
                    break;
            }
        }

        private void panel2_Paint(object sender, PaintEventArgs e)
        {
            //System.Drawing.Graphics g = e.Graphics;
            //g.DrawBeziers(Pens.Red, new Point[] { });
            //System.Windows.Forms.SystemInformation.KeyboardDelay;
        }

        private void backgroundWorker1_DoWork(object sender, System.ComponentModel.DoWorkEventArgs e)
        {
            while (true)
            {
                if (MenuController.isClose)
                    break;

                if(DateTime.Now.Subtract(dateNow).TotalMilliseconds> 10000)
                {
                    timer1.Change(-1, ParseCommunicationCommand.ConnectHz);
                    timer1.Change(0, ParseCommunicationCommand.ConnectHz);
                }
                if (btn_ConnectState.Text == "未连接")
                {
                    continue;
                }
                //if (ParseCommunicationCommand.GetDeviceIndex() == 0)
                //{
                //    ConnectMainBoard();
                //    if (btn_ConnectState.Text == "未连接")
                //        continue;
                //}
                if (ParseCommunicationCommand.IsReading)
                    continue;
                if (ParseCommunicationCommand.isAlert)
                    continue;
                try
                {
                    if (ParseCommunicationCommand.usbIntPtr.ToInt32() != -1)
                    {
                        UpdateState();
                        if (ParseCommunicationCommand.readParam == null || !ParseCommunicationCommand.readParam.isInital)
                            ReadDeviceParam();
                        else
                            SetContorlValue();
                    }
                }
                catch (Exception ex)
                {
                    CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + ex.Message + "backgroundWorker1_DoWork\r\n");
                }
                Thread.Sleep(ParseCommunicationCommand.BaseQueryHz);
            }
        }

        ///// <summary>
        ///// 连接
        ///// </summary>
        //public void ConnectMainBoard()
        //{
        //    ParseCommunicationCommand.OpenDevice();
        //    if (ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
        //    {
        //        this.btn_ConnectState.Text = "未连接";
        //        MenuController.SetButtonStyle(btn_ConnectState, 1);
        //        foreach (Control item in this.panel1.Controls)
        //        {
        //            if (item is Rich_Button && item != btn_ConnectState)
        //            {
        //                MenuController.SetButtonStyle((Button)item, 2);
        //            }
        //        }
        //    }
        //    else
        //    {
        //        ParseCommunicationCommand.GetDeviceIndex();
        //        this.btn_ConnectState.Text = "已连接";
        //        MenuController.SetButtonStyle(btn_ConnectState, 0);
        //        UpdateState();
        //    }
        //}

        public void UpdateState()
        {
            if (!frm_BaseForm.IsVisible)
            {
                string workstate = "";
                switch (Ws)
                {
                    case WorkState.Start:
                        workstate = "切割";
                        break;
                    case WorkState.Pause:
                        workstate = "暂停";
                        break;
                    case WorkState.Stop:
                        workstate = "停止";
                        break;
                    case WorkState.Feed:
                        workstate = "送料";
                        break;
                    default:
                        break;
                }
                bool isSuccess1 = ParseCommunicationCommand.ExecuteQueryDeviceState(ParseCommunicationCommand.GetQueryState(), "查询设备状态" + workstate, 120);
                if (isSuccess1)
                {
                    //if (Ws == WorkState.Start && ParseCommunicationCommand.deviceState != null)
                    //    ptList.Add(new PointF(ParseCommunicationCommand.deviceState.X, ParseCommunicationCommand.deviceState.Y));

                    if (InvokeRequired)
                    {
                        this.Invoke(this.setDeviceValue);
                    }
                    else
                        SetDeviceState(ParseCommunicationCommand.deviceState);
                }

                if (t1 <= ParseCommunicationCommand.WriteFileWait)
                {
                    t1++;
                }
                if (ParseCommunicationCommand.IsWriteFile && t1 > ParseCommunicationCommand.WriteFileWait)
                {
                    if (!isWriteS && t < 6)
                    {
                        InitialWriteFile();
                    }
                    t++;
                }
            }
            //ReadDeviceParam();
        }

        public void ReadDeviceParam()
        {
            bool isSuccess = ParseCommunicationCommand.ExecuteQueryDeviceParam(ParseCommunicationCommand.writeDataList["读取参数"], "读取参数", 64);
            if (isSuccess)
            {
                SetContorlValue();
            }
        }

        private void SetContorlValue()
        {
            if (InvokeRequired)
            {
                this.Invoke(this.setReadParamValue);
            }
            else
                SetDeviceParam(ParseCommunicationCommand.readParam);
        }

        ///// <summary>
        ///// 读取设备参数
        ///// </summary>
        //public void ReadDeviceParam()
        //{
        //    CommuncationData cd1 = new CommuncationData();
        //    cd1.writeData = ParseCommunicationCommand.writeDataList["读取参数"];
        //    cd1.dataType = "读取参数";
        //    cd1.isWrite = ParseCommunicationCommand.WriteDeviceData(cd1.writeData);
        //    if (cd1.isWrite)
        //    {
        //        cd1.readData = ParseCommunicationCommand.ReadDeviceData(120);
        //        if (cd1.readData != null)
        //        {
        //            ParseCommunicationCommand.GetReadParamData(cd1.readData);
        //            if (ParseCommunicationCommand.readParam.isInital)
        //            {
        //                SetDeviceParam(ParseCommunicationCommand.readParam);
        //            }
        //            else
        //                cd1.endRead = false;
        //            cd1.isRead = true;
        //        }
        //    }
        //}

        /// <summary>
        /// 查询设备状态
        /// </summary>
        //private void QueryDeviceState()
        //{
        //    CommuncationData cd = new CommuncationData();
        //    cd.writeData = ParseCommunicationCommand.writeDataList["查询设备状态"];
        //    cd.dataType = "查询设备状态";
        //    cd.isWrite = ParseCommunicationCommand.WriteDeviceData(cd.writeData);
        //    if (cd.isWrite)
        //    {
        //        cd.readData = ParseCommunicationCommand.ReadDeviceData(120);
        //        if (cd.readData != null)
        //        {
        //            ParseCommunicationCommand.GetSearchReadData(cd.readData);
        //            SetDeviceState(ParseCommunicationCommand.deviceState);
        //            cd.isRead = true;
        //        }
        //    }
        //}

        /// <summary>
        /// 设置设备参数
        /// </summary>
        private void SetDeviceParam(ReadParam rp)
        {
            if(rp != null)
            {
                if(Ws == WorkState.Stop)
                    workTime = 0;
                if (Ws == WorkState.Feed || Ws == WorkState.Start)
                {
                    workTime += DateTime.Now.Subtract(startWorkTime).TotalSeconds;
                    this.lbl_CutMaxSpeed.Text = "工作时间：" + workTime.ToString("f2");
                    info_Form.lbl_WorkTimeText = workTime.ToString("f2");
                }
                

                //this.lbl_Time.Text = "系统时间：" + CommonModule.ConvertType.DeviceTimeToString(rp.deviceTime);
                this.lbl_MaxLight.Text = "最大光强：" + rp.maxLightIntensity;
                this.lbl_MinLight.Text = "最小光强：" + rp.minLightIntensity;
                //bool oldSingle = PaintingPanel.IsSingleLaser;
                //if(rp.laserBKey == 0)
                //    PaintingPanel.IsSingleLaser = false;
                //else
                //    PaintingPanel.IsSingleLaser = true;

                //if (PaintingPanel.IsSingleLaser != oldSingle)
                //    paintingPanel1.Invalidate();

                if (userSetParam1.newParam == null)
                    userSetParam1.InitialControl(rp);

                //this.lbl_Verson.Text = "版本号：" + CommonModule.ConvertType.ToVersion(rp.versionNum);
                startWorkTime = DateTime.Now;
            }
        }

        /// <summary>
        /// 设置设备状态
        /// </summary>
        private void SetDeviceState(DeviceState ds)
        {
            MenuController.SetButtonStyle(this.btn_ClothInductor, ds.clothInductor);
            this.lbl_FinishCount.Text = "完成的切割的数量：" + ds.cutCount;
            info_Form.lbl_CutNumText = ds.cutCount.ToString();
            MenuController.SetButtonStyle(this.btn_FeedAlert, ds.feedAlert);
  
            MenuController.SetButtonStyle(this.btn_FolderClothSolenoidValve, ds.folderClothSolenoidValve);
            MenuController.SetButtonStyle(this.btn_OriginInductor, ds.originInductor);
            MenuController.SetButtonStyle(this.btn_SecurityLightCurtains, ds.securityLightCurtains);
            MenuController.SetButtonStyle(this.btn_TopClothSolenoidValve, ds.topClothSolenoidValve);
            MenuController.SetButtonStyle(this.btn_WaterState, ds.waterAlert);

            MenuController.SetButtonStyle(this.btn_OpenBox, ds.openBox);
            MenuController.SetButtonStyle(this.btn_ManualFeed, ds.manualFeed);
            MenuController.SetButtonStyle(this.btn_AirPressure, ds.airPressure);
            
            switch (ds.workState)
            {
                case WorkState.Start:
                    MenuController.SetButtonStyle(this.btn_WorkState, 0);
                    btn_WorkState.Text = "切割中";
                    break;
                case WorkState.Pause:
                    MenuController.SetButtonStyle(this.btn_WorkState, 3);
                    btn_WorkState.Text = "已暂停";
                    break;
                case WorkState.Stop:
                    MenuController.SetButtonStyle(this.btn_WorkState, 1);
                    btn_WorkState.Text = "已停止";
                    break;
                case WorkState.Feed:
                    MenuController.SetButtonStyle(this.btn_WorkState, 4);
                    btn_WorkState.Text = "进料中";
                    break;
            }
            info_Form.lbl_WorkStateText = btn_WorkState.Text;
            if (ds.isManual)
            {
                MenuController.SetButtonStyle(this.btn_FeedMode, 0);
                btn_FeedMode.Text = "自动";
            }
            else
            {
                MenuController.SetButtonStyle(this.btn_FeedMode, 3);
                btn_FeedMode.Text = "手动";
            }
            info_Form.lbl_AutoText = btn_FeedMode.Text;
            if(ds.downObjectCloth)
            {
                MenuController.SetButtonStyle(this.btn_DownObjectcloth, 0);
                btn_DownObjectcloth.Text = "压布辊抬起";
            }
            else
            {
                MenuController.SetButtonStyle(this.btn_DownObjectcloth, 3);
                btn_DownObjectcloth.Text = "压布辊落下";
            }
            info_Form.lbl_DownClothStateText = btn_DownObjectcloth.Text;
            alertInfo_Form.SetAlertInfo(ds);
            Ws = ParseCommunicationCommand.deviceState.workState;
            
            MenuController.SetButtonStyle(this.btn_LimitX, ds.xLimit);
            MenuController.SetButtonStyle(this.btn_LimitY, ds.yLimit);
            if(Ws != WorkState.Start)
            {
                this.lbl_LaserX.Text = "激光头X坐标：" + (ds.X / 90);
                this.lbl_LaserY.Text = "激光头Y坐标：" + (ds.Y / 90);
            }
            else
            {
                this.lbl_LaserX.Text = "激光头X坐标：" + (ds.X / 90);
                this.lbl_LaserY.Text = "激光头Y坐标：" + (ds.Y / 90);
            }

        }

        private void btn_ConnectState_Click(object sender, EventArgs e)
        {
            //timer1.Change(-1, 2000);
            //timer1.Change(0, 2000);
            //ConnectMainBoard();
            //timer1_Tick(null);
            //QueryState();
        }

        private void paintingPanel1_MouseMove(object sender, MouseEventArgs e)
        {
            if (paintingPanel1.FilePath != "")
            {
                //PointF p = paintingPanel1.ScreenToDxfCoord(e.X, e.Y);
                this.lbl_ImageX.Text = "图形文件X轴宽：" + paintingPanel1.widthG + "mm";
                this.lbl_ImageY.Text = "图形文件Y轴宽：" + paintingPanel1.heightG + "mm";
            }
        }

        public void SetFileName(string filename)
        {
            this.lbl_ImageFileName.Text = "文件名称：" + Path.GetFileNameWithoutExtension(filename);
            info_Form.lbl_FileNameText = Path.GetFileNameWithoutExtension(filename);
        }

        private void btn_ConnectState_TextChanged(object sender, EventArgs e)
        {
            if (btn_ConnectState.Text == "未连接")
            {
                isWriteS = false;
                t = 0;
                t1 = 0;
                MenuController.SetButtonStyle(btn_ConnectState, 1);
                foreach (Control item in this.panel1.Controls)
                {
                    if (item is Rich_Button && (item != btn_ConnectState && item != btn_MainMenu))
                    {
                        MenuController.SetButtonStyle((Button)item, 2);
                    }
                }
                this.userSetParam1.Enabled = false;
                info_Form.lbl_ConnectStateText = "未连接";
                info_Form.lbl_WorkStateText = "已停止";
                alertInfo_Form.SetAlertInfo(null);
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "断开连接" + "\r\n");
            }
            else if (btn_ConnectState.Text == "已连接")
            {
                MenuController.SetButtonStyle(btn_ConnectState, 0);
                //UpdateState();
                //ParseCommunicationCommand.ExecuteManualFeedParam();
                //InitialWriteFile();
                //ParseCommunicationCommand.lastPltFile
                //if (ParseCommunicationCommand.isCloseLaserB)
                //{
                //    ParseCommunicationCommand.ExecuteCommand("打开激光头B");
                //}
                //else
                //{
                //    ParseCommunicationCommand.ExecuteCommand("关闭激光头B");
                //}
                info_Form.lbl_ConnectStateText = "已连接";
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "连接成功" + "\r\n");
            }
        }

        private void InitialWriteFile()
        {
            Thread.Sleep(ParseCommunicationCommand.WriteFileWaitTime);
            isWriteS = MenuController.WritePltFile(null, true);
            if (isWriteS)
            {
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "初始化写入数据成功（" + paintingPanel1.FilePath + "）\r\n");
            }
            else
            {
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "初始化写入数据失败（" + paintingPanel1.FilePath + "）\r\n");
            }
        }

        public void OpenCutFile()
        {

        }

        public bool WriteProjectCutFile(string path)
        {
            string fileExt = Path.GetExtension(path);
            bool writeSuccess = false;
            if (fileExt.ToLower() == ".dxf" && paintingPanel1.OpenDxfFile(path))
            {
                SetFileName(path);
                writeSuccess = true;
            }
            else if (fileExt.ToLower() == ".plt" && paintingPanel1.ReadPltFile(path))
            {
                SetFileName(path);
                writeSuccess = true;
            }
            Thread.Sleep(ParseCommunicationCommand.WriteFileWaitTime);
            if(writeSuccess)
            {
                bool isWriteP = MenuController.WritePltFile(null, true);
                if (isWriteP)
                {
                    CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "项目写入切割数据成功（" + paintingPanel1.FilePath + "）\r\n");
                    //MessageBox.Show("写入切割数据成功");
                }
                else
                {
                    CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "项目写入切割数据失败（" + paintingPanel1.FilePath + "）\r\n");
                    //MessageBox.Show("写入切割数据失败");
                }
                return isWriteP;
            }
            else
            {
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "项目打开切割数据失败（" + path + "）\r\n");
            }
            return false;
        }
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (MenuController != null)
                MenuController.isClose = true;
            if (timer1 != null)
                timer1.Change(-1, ParseCommunicationCommand.ConnectHz);
            if (backgroundWorker1 != null)
            this.backgroundWorker1.CancelAsync();
            //Thread.Sleep(ParseCommunicationCommand.BaseQueryHz);
        }

        private void backgroundWorker1_RunWorkerCompleted(object sender, System.ComponentModel.RunWorkerCompletedEventArgs e)
        {
            ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "执行查询命令线程终止\r\n");
            if (!backgroundWorker1.IsBusy)
                backgroundWorker1.RunWorkerAsync();
        }

        private void btn_MainMenu_Click(object sender, EventArgs e)
        {
            bool menuVisible = MenuController.SwitchMenuForm();
            if (menuVisible)
            {
                MenuController.SetButtonStyle(btn_MainMenu, 0);
            }
            else
            {
                MenuController.SetButtonStyle(btn_MainMenu, 3);
            }
        }

        private void paintingPanel1_MouseDown(object sender, MouseEventArgs e)
        {
            //ShowPanel(false, null, null);
            MenuController.ShowPanel(false, null, null);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(!alertInfo_Form.Visible)
                alertInfo_Form.Show();
            else
                alertInfo_Form.Hide();
        }
    }
}