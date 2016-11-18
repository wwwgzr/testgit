namespace LasercuttingmachineUI
{
    partial class frm_Load
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frm_Load));
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.rab_Guest = new System.Windows.Forms.RadioButton();
            this.txt_Password = new System.Windows.Forms.TextBox();
            this.rad_Admin = new System.Windows.Forms.RadioButton();
            this.btn_OK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.lbl_tip = new System.Windows.Forms.Label();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.panel1 = new System.Windows.Forms.Panel();
            this.label5 = new System.Windows.Forms.Label();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Font = new System.Drawing.Font("楷体", 26.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label1.Location = new System.Drawing.Point(105, 102);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(411, 35);
            this.label1.TabIndex = 0;
            this.label1.Text = "欢迎使用激光切割机系统";
            this.label1.UseWaitCursor = true;
            this.label1.Click += new System.EventHandler(this.label1_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.Color.Transparent;
            this.label2.Font = new System.Drawing.Font("楷体", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label2.Location = new System.Drawing.Point(42, 185);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(69, 19);
            this.label2.TabIndex = 1;
            this.label2.Text = "初始化";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.BackColor = System.Drawing.Color.Transparent;
            this.label3.Font = new System.Drawing.Font("楷体", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label3.Location = new System.Drawing.Point(170, 166);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(109, 19);
            this.label3.TabIndex = 2;
            this.label3.Text = "用户类型：";
            this.label3.Visible = false;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.BackColor = System.Drawing.Color.Transparent;
            this.label4.Font = new System.Drawing.Font("楷体", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label4.Location = new System.Drawing.Point(170, 209);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(109, 19);
            this.label4.TabIndex = 3;
            this.label4.Text = "用户密码：";
            // 
            // rab_Guest
            // 
            this.rab_Guest.AutoSize = true;
            this.rab_Guest.BackColor = System.Drawing.Color.Transparent;
            this.rab_Guest.Font = new System.Drawing.Font("楷体", 12.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rab_Guest.Location = new System.Drawing.Point(285, 165);
            this.rab_Guest.Name = "rab_Guest";
            this.rab_Guest.Size = new System.Drawing.Size(62, 21);
            this.rab_Guest.TabIndex = 0;
            this.rab_Guest.Text = "职工";
            this.rab_Guest.UseVisualStyleBackColor = false;
            this.rab_Guest.Visible = false;
            this.rab_Guest.Click += new System.EventHandler(this.rab_Guest_Click);
            // 
            // txt_Password
            // 
            this.txt_Password.Font = new System.Drawing.Font("楷体", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.txt_Password.Location = new System.Drawing.Point(267, 204);
            this.txt_Password.Name = "txt_Password";
            this.txt_Password.PasswordChar = '*';
            this.txt_Password.Size = new System.Drawing.Size(178, 29);
            this.txt_Password.TabIndex = 2;
            this.txt_Password.MouseClick += new System.Windows.Forms.MouseEventHandler(this.txt_Password_MouseClick);
            // 
            // rad_Admin
            // 
            this.rad_Admin.AutoSize = true;
            this.rad_Admin.BackColor = System.Drawing.Color.Transparent;
            this.rad_Admin.Checked = true;
            this.rad_Admin.Font = new System.Drawing.Font("楷体", 12.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rad_Admin.Location = new System.Drawing.Point(353, 165);
            this.rad_Admin.Name = "rad_Admin";
            this.rad_Admin.Size = new System.Drawing.Size(80, 21);
            this.rad_Admin.TabIndex = 1;
            this.rad_Admin.TabStop = true;
            this.rad_Admin.Text = "管理员";
            this.rad_Admin.UseVisualStyleBackColor = false;
            this.rad_Admin.Visible = false;
            this.rad_Admin.Click += new System.EventHandler(this.rab_Guest_Click);
            // 
            // btn_OK
            // 
            this.btn_OK.BackColor = System.Drawing.Color.Transparent;
            this.btn_OK.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_OK.Font = new System.Drawing.Font("楷体", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btn_OK.Location = new System.Drawing.Point(261, 264);
            this.btn_OK.Name = "btn_OK";
            this.btn_OK.Size = new System.Drawing.Size(86, 40);
            this.btn_OK.TabIndex = 3;
            this.btn_OK.Text = "确 定";
            this.btn_OK.UseVisualStyleBackColor = false;
            this.btn_OK.Click += new System.EventHandler(this.btn_OK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.BackColor = System.Drawing.Color.Transparent;
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnCancel.Font = new System.Drawing.Font("楷体", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btnCancel.Location = new System.Drawing.Point(359, 264);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(86, 40);
            this.btnCancel.TabIndex = 4;
            this.btnCancel.Text = "取消";
            this.btnCancel.UseVisualStyleBackColor = false;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // lbl_tip
            // 
            this.lbl_tip.AutoSize = true;
            this.lbl_tip.BackColor = System.Drawing.Color.Transparent;
            this.lbl_tip.Font = new System.Drawing.Font("楷体", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_tip.Location = new System.Drawing.Point(270, 237);
            this.lbl_tip.Name = "lbl_tip";
            this.lbl_tip.Size = new System.Drawing.Size(0, 19);
            this.lbl_tip.TabIndex = 9;
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.Color.Transparent;
            this.panel1.Controls.Add(this.label5);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Location = new System.Drawing.Point(75, 34);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(488, 361);
            this.panel1.TabIndex = 10;
            this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.BackColor = System.Drawing.Color.Transparent;
            this.label5.Font = new System.Drawing.Font("楷体", 26.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label5.Location = new System.Drawing.Point(31, 131);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(411, 35);
            this.label5.TabIndex = 1;
            this.label5.Text = "欢迎使用激光切割机系统";
            this.label5.UseWaitCursor = true;
            // 
            // frm_Load
            // 
            this.AcceptButton = this.btn_OK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = global::LasercuttingmachineUI.Properties.Resources.Loading;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(650, 443);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.lbl_tip);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btn_OK);
            this.Controls.Add(this.rad_Admin);
            this.Controls.Add(this.txt_Password);
            this.Controls.Add(this.rab_Guest);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label1);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "frm_Load";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "激光切割机(一体机部分)";
            this.TransparencyKey = System.Drawing.SystemColors.Control;
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.RadioButton rab_Guest;
        private System.Windows.Forms.TextBox txt_Password;
        private System.Windows.Forms.RadioButton rad_Admin;
        private System.Windows.Forms.Button btn_OK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Label lbl_tip;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label5;
    }
}