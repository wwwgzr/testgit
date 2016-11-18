namespace LasercuttingmachineUI
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.lbl_ImageFileName = new System.Windows.Forms.Label();
            this.lbl_CutMaxSpeed = new System.Windows.Forms.Label();
            this.lbl_MaxLight = new System.Windows.Forms.Label();
            this.lbl_MinLight = new System.Windows.Forms.Label();
            this.lbl_LaserY = new System.Windows.Forms.Label();
            this.lbl_LaserX = new System.Windows.Forms.Label();
            this.lbl_ImageY = new System.Windows.Forms.Label();
            this.lbl_ImageX = new System.Windows.Forms.Label();
            this.lbl_FinishCount = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.paintingPanel1 = new LasercuttingmachineUI.PaintingPanel();
            this.userSetParam1 = new LasercuttingmachineUI.UserSetParam();
            this.btn_AirPressure = new LasercuttingmachineUI.Rich_Button();
            this.btn_DownObjectcloth = new LasercuttingmachineUI.Rich_Button();
            this.btn_ManualFeed = new LasercuttingmachineUI.Rich_Button();
            this.btn_OpenBox = new LasercuttingmachineUI.Rich_Button();
            this.btn_MainMenu = new LasercuttingmachineUI.Rich_Button();
            this.btn_ConnectState = new LasercuttingmachineUI.Rich_Button();
            this.btn_FeedMode = new LasercuttingmachineUI.Rich_Button();
            this.btn_ClothInductor = new LasercuttingmachineUI.Rich_Button();
            this.btn_LimitX = new LasercuttingmachineUI.Rich_Button();
            this.btn_FolderClothSolenoidValve = new LasercuttingmachineUI.Rich_Button();
            this.btn_WorkState = new LasercuttingmachineUI.Rich_Button();
            this.btn_SecurityLightCurtains = new LasercuttingmachineUI.Rich_Button();
            this.btn_FeedAlert = new LasercuttingmachineUI.Rich_Button();
            this.btn_LimitY = new LasercuttingmachineUI.Rich_Button();
            this.btn_WaterState = new LasercuttingmachineUI.Rich_Button();
            this.btn_OriginInductor = new LasercuttingmachineUI.Rich_Button();
            this.btn_TopClothSolenoidValve = new LasercuttingmachineUI.Rich_Button();
            this.groupBox1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.BackColor = System.Drawing.Color.Transparent;
            this.groupBox1.Controls.Add(this.lbl_ImageFileName);
            this.groupBox1.Controls.Add(this.lbl_CutMaxSpeed);
            this.groupBox1.Controls.Add(this.lbl_MaxLight);
            this.groupBox1.Controls.Add(this.lbl_MinLight);
            this.groupBox1.Controls.Add(this.lbl_LaserY);
            this.groupBox1.Controls.Add(this.lbl_LaserX);
            this.groupBox1.Controls.Add(this.lbl_ImageY);
            this.groupBox1.Controls.Add(this.lbl_ImageX);
            this.groupBox1.Controls.Add(this.lbl_FinishCount);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(293, 276);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Visible = false;
            // 
            // lbl_ImageFileName
            // 
            this.lbl_ImageFileName.AutoSize = true;
            this.lbl_ImageFileName.Location = new System.Drawing.Point(6, 245);
            this.lbl_ImageFileName.Name = "lbl_ImageFileName";
            this.lbl_ImageFileName.Size = new System.Drawing.Size(65, 12);
            this.lbl_ImageFileName.TabIndex = 13;
            this.lbl_ImageFileName.Text = "文件名称：";
            // 
            // lbl_CutMaxSpeed
            // 
            this.lbl_CutMaxSpeed.AutoSize = true;
            this.lbl_CutMaxSpeed.Location = new System.Drawing.Point(6, 188);
            this.lbl_CutMaxSpeed.Name = "lbl_CutMaxSpeed";
            this.lbl_CutMaxSpeed.Size = new System.Drawing.Size(65, 12);
            this.lbl_CutMaxSpeed.TabIndex = 12;
            this.lbl_CutMaxSpeed.Text = "工作时间：";
            // 
            // lbl_MaxLight
            // 
            this.lbl_MaxLight.AutoSize = true;
            this.lbl_MaxLight.Location = new System.Drawing.Point(6, 159);
            this.lbl_MaxLight.Name = "lbl_MaxLight";
            this.lbl_MaxLight.Size = new System.Drawing.Size(65, 12);
            this.lbl_MaxLight.TabIndex = 11;
            this.lbl_MaxLight.Text = "最大光强：";
            // 
            // lbl_MinLight
            // 
            this.lbl_MinLight.AutoSize = true;
            this.lbl_MinLight.Location = new System.Drawing.Point(6, 130);
            this.lbl_MinLight.Name = "lbl_MinLight";
            this.lbl_MinLight.Size = new System.Drawing.Size(65, 12);
            this.lbl_MinLight.TabIndex = 10;
            this.lbl_MinLight.Text = "最小光强：";
            // 
            // lbl_LaserY
            // 
            this.lbl_LaserY.AutoSize = true;
            this.lbl_LaserY.Location = new System.Drawing.Point(6, 101);
            this.lbl_LaserY.Name = "lbl_LaserY";
            this.lbl_LaserY.Size = new System.Drawing.Size(83, 12);
            this.lbl_LaserY.TabIndex = 9;
            this.lbl_LaserY.Text = "激光头Y坐标：";
            // 
            // lbl_LaserX
            // 
            this.lbl_LaserX.AutoSize = true;
            this.lbl_LaserX.Location = new System.Drawing.Point(6, 73);
            this.lbl_LaserX.Name = "lbl_LaserX";
            this.lbl_LaserX.Size = new System.Drawing.Size(83, 12);
            this.lbl_LaserX.TabIndex = 8;
            this.lbl_LaserX.Text = "激光头X坐标：";
            // 
            // lbl_ImageY
            // 
            this.lbl_ImageY.AutoSize = true;
            this.lbl_ImageY.Location = new System.Drawing.Point(6, 45);
            this.lbl_ImageY.Name = "lbl_ImageY";
            this.lbl_ImageY.Size = new System.Drawing.Size(95, 12);
            this.lbl_ImageY.TabIndex = 7;
            this.lbl_ImageY.Text = "图形文件Y轴宽：";
            // 
            // lbl_ImageX
            // 
            this.lbl_ImageX.AutoSize = true;
            this.lbl_ImageX.Location = new System.Drawing.Point(6, 17);
            this.lbl_ImageX.Name = "lbl_ImageX";
            this.lbl_ImageX.Size = new System.Drawing.Size(95, 12);
            this.lbl_ImageX.TabIndex = 6;
            this.lbl_ImageX.Text = "图形文件X轴宽：";
            // 
            // lbl_FinishCount
            // 
            this.lbl_FinishCount.AutoSize = true;
            this.lbl_FinishCount.Location = new System.Drawing.Point(6, 217);
            this.lbl_FinishCount.Name = "lbl_FinishCount";
            this.lbl_FinishCount.Size = new System.Drawing.Size(113, 12);
            this.lbl_FinishCount.TabIndex = 2;
            this.lbl_FinishCount.Text = "完成的切割的数量：";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.userSetParam1);
            this.panel1.Controls.Add(this.btn_AirPressure);
            this.panel1.Controls.Add(this.btn_DownObjectcloth);
            this.panel1.Controls.Add(this.btn_ManualFeed);
            this.panel1.Controls.Add(this.btn_OpenBox);
            this.panel1.Controls.Add(this.btn_MainMenu);
            this.panel1.Controls.Add(this.btn_ConnectState);
            this.panel1.Controls.Add(this.btn_FeedMode);
            this.panel1.Controls.Add(this.btn_ClothInductor);
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Controls.Add(this.btn_LimitX);
            this.panel1.Controls.Add(this.btn_FolderClothSolenoidValve);
            this.panel1.Controls.Add(this.btn_WorkState);
            this.panel1.Controls.Add(this.btn_SecurityLightCurtains);
            this.panel1.Controls.Add(this.btn_FeedAlert);
            this.panel1.Controls.Add(this.btn_LimitY);
            this.panel1.Controls.Add(this.btn_WaterState);
            this.panel1.Controls.Add(this.btn_OriginInductor);
            this.panel1.Controls.Add(this.btn_TopClothSolenoidValve);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Right;
            this.panel1.Location = new System.Drawing.Point(743, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(293, 768);
            this.panel1.TabIndex = 6;
            // 
            // panel2
            // 
            this.panel2.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.panel2.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            //this.panel2.Controls.Add(this.paintingPanel1);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(743, 768);
            this.panel2.TabIndex = 7;
            this.panel2.Paint += new System.Windows.Forms.PaintEventHandler(this.panel2_Paint);
            // 
            // backgroundWorker1
            // 
            this.backgroundWorker1.WorkerSupportsCancellation = true;
            this.backgroundWorker1.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker1_DoWork);
            this.backgroundWorker1.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroundWorker1_RunWorkerCompleted);
            // 
            // paintingPanel1
            // 
            this.paintingPanel1.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.paintingPanel1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.paintingPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.paintingPanel1.FilePath = "";
            this.paintingPanel1.Location = new System.Drawing.Point(0, 0);
            this.paintingPanel1.Name = "paintingPanel1";
            this.paintingPanel1.Size = new System.Drawing.Size(743, 768);
            this.paintingPanel1.TabIndex = 4;
            this.paintingPanel1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.paintingPanel1_MouseDown);
            // 
            // userSetParam1
            // 
            this.userSetParam1.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.userSetParam1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.userSetParam1.Location = new System.Drawing.Point(0, 276);
            this.userSetParam1.Name = "userSetParam1";
            this.userSetParam1.Size = new System.Drawing.Size(293, 492);
            this.userSetParam1.TabIndex = 39;
            // 
            // btn_AirPressure
            // 
            this.btn_AirPressure.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_AirPressure.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_AirPressure.Location = new System.Drawing.Point(103, 286);
            this.btn_AirPressure.MouseDownImage = null;
            this.btn_AirPressure.Name = "btn_AirPressure";
            this.btn_AirPressure.Size = new System.Drawing.Size(96, 54);
            this.btn_AirPressure.TabIndex = 38;
            this.btn_AirPressure.Text = "气压报警";
            this.btn_AirPressure.UseVisualStyleBackColor = true;
            this.btn_AirPressure.Visible = false;
            // 
            // btn_DownObjectcloth
            // 
            this.btn_DownObjectcloth.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_DownObjectcloth.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_DownObjectcloth.ForeColor = System.Drawing.Color.Black;
            this.btn_DownObjectcloth.Location = new System.Drawing.Point(3, 286);
            this.btn_DownObjectcloth.MouseDownImage = null;
            this.btn_DownObjectcloth.Name = "btn_DownObjectcloth";
            this.btn_DownObjectcloth.Size = new System.Drawing.Size(96, 54);
            this.btn_DownObjectcloth.TabIndex = 37;
            this.btn_DownObjectcloth.Text = "压布辊落下";
            this.btn_DownObjectcloth.UseVisualStyleBackColor = false;
            this.btn_DownObjectcloth.Visible = false;
            // 
            // btn_ManualFeed
            // 
            this.btn_ManualFeed.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_ManualFeed.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_ManualFeed.Location = new System.Drawing.Point(103, 347);
            this.btn_ManualFeed.MouseDownImage = null;
            this.btn_ManualFeed.Name = "btn_ManualFeed";
            this.btn_ManualFeed.Size = new System.Drawing.Size(96, 54);
            this.btn_ManualFeed.TabIndex = 35;
            this.btn_ManualFeed.Text = "手动报警";
            this.btn_ManualFeed.UseVisualStyleBackColor = true;
            this.btn_ManualFeed.Visible = false;
            // 
            // btn_OpenBox
            // 
            this.btn_OpenBox.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_OpenBox.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_OpenBox.ForeColor = System.Drawing.Color.Black;
            this.btn_OpenBox.Location = new System.Drawing.Point(3, 347);
            this.btn_OpenBox.MouseDownImage = null;
            this.btn_OpenBox.Name = "btn_OpenBox";
            this.btn_OpenBox.Size = new System.Drawing.Size(96, 54);
            this.btn_OpenBox.TabIndex = 34;
            this.btn_OpenBox.Text = "开盖报警";
            this.btn_OpenBox.UseVisualStyleBackColor = false;
            this.btn_OpenBox.Visible = false;
            // 
            // btn_MainMenu
            // 
            this.btn_MainMenu.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_MainMenu.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_MainMenu.Location = new System.Drawing.Point(3, 707);
            this.btn_MainMenu.MouseDownImage = null;
            this.btn_MainMenu.Name = "btn_MainMenu";
            this.btn_MainMenu.Size = new System.Drawing.Size(194, 54);
            this.btn_MainMenu.TabIndex = 33;
            this.btn_MainMenu.Text = "主菜单";
            this.btn_MainMenu.UseVisualStyleBackColor = true;
            this.btn_MainMenu.Visible = false;
            this.btn_MainMenu.Click += new System.EventHandler(this.btn_MainMenu_Click);
            // 
            // btn_ConnectState
            // 
            this.btn_ConnectState.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_ConnectState.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_ConnectState.Location = new System.Drawing.Point(102, 647);
            this.btn_ConnectState.MouseDownImage = null;
            this.btn_ConnectState.Name = "btn_ConnectState";
            this.btn_ConnectState.Size = new System.Drawing.Size(96, 54);
            this.btn_ConnectState.TabIndex = 22;
            this.btn_ConnectState.Text = "未连接";
            this.btn_ConnectState.UseVisualStyleBackColor = true;
            this.btn_ConnectState.Visible = false;
            this.btn_ConnectState.TextChanged += new System.EventHandler(this.btn_ConnectState_TextChanged);
            this.btn_ConnectState.Click += new System.EventHandler(this.btn_ConnectState_Click);
            // 
            // btn_FeedMode
            // 
            this.btn_FeedMode.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_FeedMode.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_FeedMode.Location = new System.Drawing.Point(102, 407);
            this.btn_FeedMode.MouseDownImage = null;
            this.btn_FeedMode.Name = "btn_FeedMode";
            this.btn_FeedMode.Size = new System.Drawing.Size(96, 54);
            this.btn_FeedMode.TabIndex = 31;
            this.btn_FeedMode.Text = "手动自动";
            this.btn_FeedMode.UseVisualStyleBackColor = true;
            this.btn_FeedMode.Visible = false;
            // 
            // btn_ClothInductor
            // 
            this.btn_ClothInductor.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_ClothInductor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_ClothInductor.Location = new System.Drawing.Point(102, 587);
            this.btn_ClothInductor.MouseDownImage = null;
            this.btn_ClothInductor.Name = "btn_ClothInductor";
            this.btn_ClothInductor.Size = new System.Drawing.Size(96, 54);
            this.btn_ClothInductor.TabIndex = 32;
            this.btn_ClothInductor.Text = "布料感应器";
            this.btn_ClothInductor.UseVisualStyleBackColor = true;
            this.btn_ClothInductor.Visible = false;
            // 
            // btn_LimitX
            // 
            this.btn_LimitX.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_LimitX.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_LimitX.ForeColor = System.Drawing.Color.Black;
            this.btn_LimitX.Location = new System.Drawing.Point(2, 407);
            this.btn_LimitX.MouseDownImage = null;
            this.btn_LimitX.Name = "btn_LimitX";
            this.btn_LimitX.Size = new System.Drawing.Size(96, 54);
            this.btn_LimitX.TabIndex = 21;
            this.btn_LimitX.Text = "X限位";
            this.btn_LimitX.UseVisualStyleBackColor = false;
            this.btn_LimitX.Visible = false;
            // 
            // btn_FolderClothSolenoidValve
            // 
            this.btn_FolderClothSolenoidValve.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_FolderClothSolenoidValve.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_FolderClothSolenoidValve.Location = new System.Drawing.Point(102, 587);
            this.btn_FolderClothSolenoidValve.MouseDownImage = null;
            this.btn_FolderClothSolenoidValve.Name = "btn_FolderClothSolenoidValve";
            this.btn_FolderClothSolenoidValve.Size = new System.Drawing.Size(96, 54);
            this.btn_FolderClothSolenoidValve.TabIndex = 26;
            this.btn_FolderClothSolenoidValve.Text = "夹布电磁阀";
            this.btn_FolderClothSolenoidValve.UseVisualStyleBackColor = true;
            this.btn_FolderClothSolenoidValve.Visible = false;
            // 
            // btn_WorkState
            // 
            this.btn_WorkState.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_WorkState.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_WorkState.Location = new System.Drawing.Point(2, 647);
            this.btn_WorkState.MouseDownImage = null;
            this.btn_WorkState.Name = "btn_WorkState";
            this.btn_WorkState.Size = new System.Drawing.Size(96, 54);
            this.btn_WorkState.TabIndex = 29;
            this.btn_WorkState.Text = "机器停止";
            this.btn_WorkState.UseVisualStyleBackColor = true;
            this.btn_WorkState.Visible = false;
            // 
            // btn_SecurityLightCurtains
            // 
            this.btn_SecurityLightCurtains.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_SecurityLightCurtains.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_SecurityLightCurtains.Location = new System.Drawing.Point(2, 587);
            this.btn_SecurityLightCurtains.MouseDownImage = null;
            this.btn_SecurityLightCurtains.Name = "btn_SecurityLightCurtains";
            this.btn_SecurityLightCurtains.Size = new System.Drawing.Size(96, 54);
            this.btn_SecurityLightCurtains.TabIndex = 27;
            this.btn_SecurityLightCurtains.Text = "防护光幕报警";
            this.btn_SecurityLightCurtains.UseVisualStyleBackColor = true;
            this.btn_SecurityLightCurtains.Visible = false;
            // 
            // btn_FeedAlert
            // 
            this.btn_FeedAlert.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_FeedAlert.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_FeedAlert.Location = new System.Drawing.Point(102, 527);
            this.btn_FeedAlert.MouseDownImage = null;
            this.btn_FeedAlert.Name = "btn_FeedAlert";
            this.btn_FeedAlert.Size = new System.Drawing.Size(96, 54);
            this.btn_FeedAlert.TabIndex = 30;
            this.btn_FeedAlert.Text = "缺料报警";
            this.btn_FeedAlert.UseVisualStyleBackColor = true;
            this.btn_FeedAlert.Visible = false;
            // 
            // btn_LimitY
            // 
            this.btn_LimitY.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_LimitY.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_LimitY.Location = new System.Drawing.Point(2, 527);
            this.btn_LimitY.MouseDownImage = null;
            this.btn_LimitY.Name = "btn_LimitY";
            this.btn_LimitY.Size = new System.Drawing.Size(96, 54);
            this.btn_LimitY.TabIndex = 25;
            this.btn_LimitY.Text = "Y限位";
            this.btn_LimitY.UseVisualStyleBackColor = true;
            this.btn_LimitY.Visible = false;
            // 
            // btn_WaterState
            // 
            this.btn_WaterState.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_WaterState.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_WaterState.Location = new System.Drawing.Point(2, 467);
            this.btn_WaterState.MouseDownImage = null;
            this.btn_WaterState.Name = "btn_WaterState";
            this.btn_WaterState.Size = new System.Drawing.Size(96, 54);
            this.btn_WaterState.TabIndex = 23;
            this.btn_WaterState.Text = "水箱报警状态";
            this.btn_WaterState.UseVisualStyleBackColor = true;
            this.btn_WaterState.Visible = false;
            // 
            // btn_OriginInductor
            // 
            this.btn_OriginInductor.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_OriginInductor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_OriginInductor.Location = new System.Drawing.Point(102, 467);
            this.btn_OriginInductor.MouseDownImage = null;
            this.btn_OriginInductor.Name = "btn_OriginInductor";
            this.btn_OriginInductor.Size = new System.Drawing.Size(96, 54);
            this.btn_OriginInductor.TabIndex = 28;
            this.btn_OriginInductor.Text = "原点感应器";
            this.btn_OriginInductor.UseVisualStyleBackColor = true;
            this.btn_OriginInductor.Visible = false;
            // 
            // btn_TopClothSolenoidValve
            // 
            this.btn_TopClothSolenoidValve.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_TopClothSolenoidValve.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_TopClothSolenoidValve.Location = new System.Drawing.Point(102, 467);
            this.btn_TopClothSolenoidValve.MouseDownImage = null;
            this.btn_TopClothSolenoidValve.Name = "btn_TopClothSolenoidValve";
            this.btn_TopClothSolenoidValve.Size = new System.Drawing.Size(96, 54);
            this.btn_TopClothSolenoidValve.TabIndex = 24;
            this.btn_TopClothSolenoidValve.Text = "顶布电磁阀";
            this.btn_TopClothSolenoidValve.UseVisualStyleBackColor = true;
            this.btn_TopClothSolenoidValve.Visible = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.ClientSize = new System.Drawing.Size(1036, 768);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "激光切割机(一体机部分)";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Form1_KeyUp);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label lbl_FinishCount;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.Label lbl_ImageY;
        private System.Windows.Forms.Label lbl_ImageX;
        public Rich_Button btn_ConnectState;
        private Rich_Button btn_FeedMode;
        private Rich_Button btn_ClothInductor;
        private Rich_Button btn_LimitX;
        private Rich_Button btn_FolderClothSolenoidValve;
        private Rich_Button btn_WorkState;
        private Rich_Button btn_SecurityLightCurtains;
        private Rich_Button btn_FeedAlert;
        private Rich_Button btn_LimitY;
        private Rich_Button btn_WaterState;
        private Rich_Button btn_OriginInductor;
        private Rich_Button btn_TopClothSolenoidValve;
        private System.Windows.Forms.Label lbl_ImageFileName;
        private System.Windows.Forms.Label lbl_MaxLight;
        private System.Windows.Forms.Label lbl_MinLight;
        private System.Windows.Forms.Label lbl_LaserY;
        private System.Windows.Forms.Label lbl_LaserX;
        private Rich_Button btn_MainMenu;
        private Rich_Button btn_ManualFeed;
        private Rich_Button btn_OpenBox;
        private System.Windows.Forms.Label lbl_CutMaxSpeed;
        private Rich_Button btn_AirPressure;
        private Rich_Button btn_DownObjectcloth;
        public UserSetParam userSetParam1;
        public PaintingPanel paintingPanel1;
    }
}