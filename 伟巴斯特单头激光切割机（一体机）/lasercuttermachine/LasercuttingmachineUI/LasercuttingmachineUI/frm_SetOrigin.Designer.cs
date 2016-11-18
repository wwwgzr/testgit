namespace LasercuttingmachineUI
{
    partial class frm_SetOrigin
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.rab_700 = new System.Windows.Forms.RadioButton();
            this.rad_02 = new System.Windows.Forms.RadioButton();
            this.rab_50 = new System.Windows.Forms.RadioButton();
            this.rab_250 = new System.Windows.Forms.RadioButton();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.rab_NoLight = new System.Windows.Forms.RadioButton();
            this.rab_Light = new System.Windows.Forms.RadioButton();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.btn_YPlus = new LasercuttingmachineUI.Rich_Button();
            this.btn_YMinus = new LasercuttingmachineUI.Rich_Button();
            this.btn_XMinus = new LasercuttingmachineUI.Rich_Button();
            this.btn_XPlus = new LasercuttingmachineUI.Rich_Button();
            this.btn_Quit = new LasercuttingmachineUI.Rich_Button();
            this.btn_Save = new LasercuttingmachineUI.Rich_Button();
            this.nud_CroodX = new System.Windows.Forms.NumericUpDown();
            this.nud_CroodY = new System.Windows.Forms.NumericUpDown();
            this.btn_Left = new LasercuttingmachineUI.Rich_Button();
            this.btn_Down = new LasercuttingmachineUI.Rich_Button();
            this.btn_Right = new LasercuttingmachineUI.Rich_Button();
            this.btn_Up = new LasercuttingmachineUI.Rich_Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.lbl_CroodY = new System.Windows.Forms.Label();
            this.lbl_CroodX = new System.Windows.Forms.Label();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.groupBox4.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_CroodX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_CroodY)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.rab_700);
            this.groupBox4.Controls.Add(this.rad_02);
            this.groupBox4.Controls.Add(this.rab_50);
            this.groupBox4.Controls.Add(this.rab_250);
            this.groupBox4.Location = new System.Drawing.Point(5, 74);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(312, 60);
            this.groupBox4.TabIndex = 21;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "移动步长";
            // 
            // rab_700
            // 
            this.rab_700.AutoSize = true;
            this.rab_700.Location = new System.Drawing.Point(7, 14);
            this.rab_700.Name = "rab_700";
            this.rab_700.Size = new System.Drawing.Size(71, 40);
            this.rab_700.TabIndex = 4;
            this.rab_700.Text = "\r\n700mm   \r\n\r\n";
            this.rab_700.UseVisualStyleBackColor = true;
            this.rab_700.CheckedChanged += new System.EventHandler(this.rad_CheckedChanged);
            // 
            // rad_02
            // 
            this.rad_02.AutoSize = true;
            this.rad_02.Location = new System.Drawing.Point(238, 14);
            this.rad_02.Name = "rad_02";
            this.rad_02.Size = new System.Drawing.Size(71, 40);
            this.rad_02.TabIndex = 3;
            this.rad_02.Text = "\r\n0.2mm   \r\n\r\n";
            this.rad_02.UseVisualStyleBackColor = true;
            this.rad_02.CheckedChanged += new System.EventHandler(this.rad_CheckedChanged);
            // 
            // rab_50
            // 
            this.rab_50.AutoSize = true;
            this.rab_50.Location = new System.Drawing.Point(165, 14);
            this.rab_50.Name = "rab_50";
            this.rab_50.Size = new System.Drawing.Size(65, 40);
            this.rab_50.TabIndex = 2;
            this.rab_50.Text = "\r\n50mm   \r\n\r\n";
            this.rab_50.UseVisualStyleBackColor = true;
            this.rab_50.CheckedChanged += new System.EventHandler(this.rad_CheckedChanged);
            // 
            // rab_250
            // 
            this.rab_250.AutoSize = true;
            this.rab_250.Checked = true;
            this.rab_250.Location = new System.Drawing.Point(86, 14);
            this.rab_250.Name = "rab_250";
            this.rab_250.Size = new System.Drawing.Size(71, 40);
            this.rab_250.TabIndex = 1;
            this.rab_250.TabStop = true;
            this.rab_250.Text = "\r\n250mm   \r\n\r\n";
            this.rab_250.UseVisualStyleBackColor = true;
            this.rab_250.CheckedChanged += new System.EventHandler(this.rad_CheckedChanged);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.rab_NoLight);
            this.groupBox3.Controls.Add(this.rab_Light);
            this.groupBox3.Location = new System.Drawing.Point(5, 8);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(312, 60);
            this.groupBox3.TabIndex = 20;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "移动出光";
            // 
            // rab_NoLight
            // 
            this.rab_NoLight.AutoSize = true;
            this.rab_NoLight.Checked = true;
            this.rab_NoLight.Location = new System.Drawing.Point(192, 14);
            this.rab_NoLight.Name = "rab_NoLight";
            this.rab_NoLight.Size = new System.Drawing.Size(107, 40);
            this.rab_NoLight.TabIndex = 2;
            this.rab_NoLight.TabStop = true;
            this.rab_NoLight.Text = "\r\n不出光        \r\n\r\n";
            this.rab_NoLight.UseVisualStyleBackColor = true;
            this.rab_NoLight.CheckedChanged += new System.EventHandler(this.rab_Light_CheckedChanged);
            // 
            // rab_Light
            // 
            this.rab_Light.AutoSize = true;
            this.rab_Light.Location = new System.Drawing.Point(24, 14);
            this.rab_Light.Name = "rab_Light";
            this.rab_Light.Size = new System.Drawing.Size(107, 40);
            this.rab_Light.TabIndex = 1;
            this.rab_Light.Text = "\r\n出光          \r\n\r\n";
            this.rab_Light.UseVisualStyleBackColor = true;
            this.rab_Light.CheckedChanged += new System.EventHandler(this.rab_Light_CheckedChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("宋体", 9F);
            this.label1.Location = new System.Drawing.Point(26, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 12);
            this.label1.TabIndex = 23;
            this.label1.Text = "当前X坐标：";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("宋体", 9F);
            this.label2.Location = new System.Drawing.Point(26, 83);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(71, 12);
            this.label2.TabIndex = 25;
            this.label2.Text = "当前Y坐标：";
            // 
            // btn_YPlus
            // 
            this.btn_YPlus.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_YPlus.FlatAppearance.BorderSize = 0;
            this.btn_YPlus.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_YPlus.Font = new System.Drawing.Font("宋体", 20F);
            this.btn_YPlus.Location = new System.Drawing.Point(583, 205);
            this.btn_YPlus.MouseDownImage = null;
            this.btn_YPlus.Name = "btn_YPlus";
            this.btn_YPlus.Size = new System.Drawing.Size(40, 40);
            this.btn_YPlus.TabIndex = 26;
            this.btn_YPlus.Text = "+";
            this.btn_YPlus.UseVisualStyleBackColor = false;
            this.btn_YPlus.Click += new System.EventHandler(this.btn_Right_Click);
            // 
            // btn_YMinus
            // 
            this.btn_YMinus.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_YMinus.FlatAppearance.BorderSize = 0;
            this.btn_YMinus.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_YMinus.Font = new System.Drawing.Font("宋体", 20F);
            this.btn_YMinus.Location = new System.Drawing.Point(402, 205);
            this.btn_YMinus.MouseDownImage = null;
            this.btn_YMinus.Name = "btn_YMinus";
            this.btn_YMinus.Size = new System.Drawing.Size(40, 40);
            this.btn_YMinus.TabIndex = 27;
            this.btn_YMinus.Text = "-";
            this.btn_YMinus.UseVisualStyleBackColor = false;
            this.btn_YMinus.Click += new System.EventHandler(this.btn_Left_Click);
            // 
            // btn_XMinus
            // 
            this.btn_XMinus.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_XMinus.FlatAppearance.BorderSize = 0;
            this.btn_XMinus.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_XMinus.Font = new System.Drawing.Font("宋体", 20F);
            this.btn_XMinus.Location = new System.Drawing.Point(402, 154);
            this.btn_XMinus.MouseDownImage = null;
            this.btn_XMinus.Name = "btn_XMinus";
            this.btn_XMinus.Size = new System.Drawing.Size(40, 40);
            this.btn_XMinus.TabIndex = 29;
            this.btn_XMinus.Text = "-";
            this.btn_XMinus.UseVisualStyleBackColor = false;
            this.btn_XMinus.Click += new System.EventHandler(this.btn_Up_Click);
            // 
            // btn_XPlus
            // 
            this.btn_XPlus.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_XPlus.FlatAppearance.BorderSize = 0;
            this.btn_XPlus.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_XPlus.Font = new System.Drawing.Font("宋体", 20F);
            this.btn_XPlus.Location = new System.Drawing.Point(583, 154);
            this.btn_XPlus.MouseDownImage = null;
            this.btn_XPlus.Name = "btn_XPlus";
            this.btn_XPlus.Size = new System.Drawing.Size(40, 40);
            this.btn_XPlus.TabIndex = 28;
            this.btn_XPlus.Text = "+";
            this.btn_XPlus.UseVisualStyleBackColor = false;
            this.btn_XPlus.Click += new System.EventHandler(this.btn_Down_Click);
            // 
            // btn_Quit
            // 
            this.btn_Quit.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Quit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Quit.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Quit.Location = new System.Drawing.Point(206, 288);
            this.btn_Quit.MouseDownImage = null;
            this.btn_Quit.Name = "btn_Quit";
            this.btn_Quit.Size = new System.Drawing.Size(98, 45);
            this.btn_Quit.TabIndex = 31;
            this.btn_Quit.Text = "退 出";
            this.btn_Quit.UseVisualStyleBackColor = true;
            this.btn_Quit.Click += new System.EventHandler(this.btn_Quit_Click);
            // 
            // btn_Save
            // 
            this.btn_Save.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Save.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Save.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Save.Location = new System.Drawing.Point(100, 288);
            this.btn_Save.MouseDownImage = null;
            this.btn_Save.Name = "btn_Save";
            this.btn_Save.Size = new System.Drawing.Size(98, 45);
            this.btn_Save.TabIndex = 30;
            this.btn_Save.Text = "保 存";
            this.btn_Save.UseVisualStyleBackColor = true;
            this.btn_Save.Click += new System.EventHandler(this.btn_Save_Click);
            // 
            // nud_CroodX
            // 
            this.nud_CroodX.DecimalPlaces = 1;
            this.nud_CroodX.Font = new System.Drawing.Font("宋体", 21.5F);
            this.nud_CroodX.Increment = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nud_CroodX.Location = new System.Drawing.Point(445, 154);
            this.nud_CroodX.Maximum = new decimal(new int[] {
            1800,
            0,
            0,
            0});
            this.nud_CroodX.Name = "nud_CroodX";
            this.nud_CroodX.Size = new System.Drawing.Size(135, 40);
            this.nud_CroodX.TabIndex = 32;
            this.nud_CroodX.ValueChanged += new System.EventHandler(this.nud_CroodX_ValueChanged);
            // 
            // nud_CroodY
            // 
            this.nud_CroodY.DecimalPlaces = 1;
            this.nud_CroodY.Font = new System.Drawing.Font("宋体", 21.5F);
            this.nud_CroodY.Increment = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.nud_CroodY.Location = new System.Drawing.Point(445, 205);
            this.nud_CroodY.Maximum = new decimal(new int[] {
            1800,
            0,
            0,
            0});
            this.nud_CroodY.Name = "nud_CroodY";
            this.nud_CroodY.Size = new System.Drawing.Size(135, 40);
            this.nud_CroodY.TabIndex = 33;
            this.nud_CroodY.ValueChanged += new System.EventHandler(this.nud_CroodY_ValueChanged);
            // 
            // btn_Left
            // 
            this.btn_Left.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Left.FlatAppearance.BorderSize = 0;
            this.btn_Left.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Left.Font = new System.Drawing.Font("宋体", 20F);
            this.btn_Left.Location = new System.Drawing.Point(124, 49);
            this.btn_Left.MouseDownImage = null;
            this.btn_Left.Name = "btn_Left";
            this.btn_Left.Size = new System.Drawing.Size(50, 50);
            this.btn_Left.TabIndex = 34;
            this.btn_Left.Text = "←";
            this.btn_Left.UseVisualStyleBackColor = false;
            this.btn_Left.Click += new System.EventHandler(this.btn_Left_Click);
            this.btn_Left.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btn_MouseDown);
            this.btn_Left.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btn_MouseUp);
            // 
            // btn_Down
            // 
            this.btn_Down.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Down.FlatAppearance.BorderSize = 0;
            this.btn_Down.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Down.Font = new System.Drawing.Font("宋体", 20F);
            this.btn_Down.Location = new System.Drawing.Point(179, 82);
            this.btn_Down.MouseDownImage = null;
            this.btn_Down.Name = "btn_Down";
            this.btn_Down.Size = new System.Drawing.Size(50, 50);
            this.btn_Down.TabIndex = 35;
            this.btn_Down.Text = "↓";
            this.btn_Down.UseVisualStyleBackColor = false;
            this.btn_Down.Click += new System.EventHandler(this.btn_Down_Click);
            this.btn_Down.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btn_MouseDown);
            this.btn_Down.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btn_MouseUp);
            // 
            // btn_Right
            // 
            this.btn_Right.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Right.FlatAppearance.BorderSize = 0;
            this.btn_Right.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Right.Font = new System.Drawing.Font("宋体", 20F);
            this.btn_Right.Location = new System.Drawing.Point(234, 50);
            this.btn_Right.MouseDownImage = null;
            this.btn_Right.Name = "btn_Right";
            this.btn_Right.Size = new System.Drawing.Size(50, 50);
            this.btn_Right.TabIndex = 36;
            this.btn_Right.Text = "→";
            this.btn_Right.UseVisualStyleBackColor = false;
            this.btn_Right.Click += new System.EventHandler(this.btn_Right_Click);
            this.btn_Right.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btn_MouseDown);
            this.btn_Right.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btn_MouseUp);
            // 
            // btn_Up
            // 
            this.btn_Up.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Up.FlatAppearance.BorderSize = 0;
            this.btn_Up.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Up.Font = new System.Drawing.Font("宋体", 20F);
            this.btn_Up.Location = new System.Drawing.Point(179, 16);
            this.btn_Up.MouseDownImage = null;
            this.btn_Up.Name = "btn_Up";
            this.btn_Up.Size = new System.Drawing.Size(50, 50);
            this.btn_Up.TabIndex = 37;
            this.btn_Up.Text = "↑";
            this.btn_Up.UseVisualStyleBackColor = false;
            this.btn_Up.Click += new System.EventHandler(this.btn_Up_Click);
            this.btn_Up.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btn_MouseDown);
            this.btn_Up.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btn_MouseUp);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.lbl_CroodY);
            this.groupBox1.Controls.Add(this.lbl_CroodX);
            this.groupBox1.Controls.Add(this.btn_Left);
            this.groupBox1.Controls.Add(this.btn_Up);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.btn_Right);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.btn_Down);
            this.groupBox1.Location = new System.Drawing.Point(5, 138);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(312, 144);
            this.groupBox1.TabIndex = 22;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "坐标设置";
            // 
            // lbl_CroodY
            // 
            this.lbl_CroodY.AutoSize = true;
            this.lbl_CroodY.Font = new System.Drawing.Font("宋体", 9F);
            this.lbl_CroodY.Location = new System.Drawing.Point(26, 103);
            this.lbl_CroodY.Name = "lbl_CroodY";
            this.lbl_CroodY.Size = new System.Drawing.Size(0, 12);
            this.lbl_CroodY.TabIndex = 39;
            // 
            // lbl_CroodX
            // 
            this.lbl_CroodX.AutoSize = true;
            this.lbl_CroodX.Font = new System.Drawing.Font("宋体", 9F);
            this.lbl_CroodX.Location = new System.Drawing.Point(26, 53);
            this.lbl_CroodX.Name = "lbl_CroodX";
            this.lbl_CroodX.Size = new System.Drawing.Size(0, 12);
            this.lbl_CroodX.TabIndex = 38;
            // 
            // backgroundWorker1
            // 
            this.backgroundWorker1.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker1_DoWork);
            // 
            // frm_SetOrigin
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(329, 339);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.nud_CroodY);
            this.Controls.Add(this.nud_CroodX);
            this.Controls.Add(this.btn_Quit);
            this.Controls.Add(this.btn_Save);
            this.Controls.Add(this.btn_XMinus);
            this.Controls.Add(this.btn_XPlus);
            this.Controls.Add(this.btn_YMinus);
            this.Controls.Add(this.btn_YPlus);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Name = "frm_SetOrigin";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "frm_SetOrigin";
            this.VisibleChanged += new System.EventHandler(this.frm_SetOrigin_VisibleChanged);
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nud_CroodX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nud_CroodY)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.RadioButton rad_02;
        private System.Windows.Forms.RadioButton rab_50;
        private System.Windows.Forms.RadioButton rab_250;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.RadioButton rab_NoLight;
        private System.Windows.Forms.RadioButton rab_Light;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private Rich_Button btn_YPlus;
        private Rich_Button btn_YMinus;
        private Rich_Button btn_XMinus;
        private Rich_Button btn_XPlus;
        private Rich_Button btn_Quit;
        private Rich_Button btn_Save;
        private System.Windows.Forms.NumericUpDown nud_CroodX;
        private System.Windows.Forms.NumericUpDown nud_CroodY;
        private Rich_Button btn_Left;
        private Rich_Button btn_Down;
        private Rich_Button btn_Right;
        private Rich_Button btn_Up;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label lbl_CroodY;
        private System.Windows.Forms.Label lbl_CroodX;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.RadioButton rab_700;
    }
}