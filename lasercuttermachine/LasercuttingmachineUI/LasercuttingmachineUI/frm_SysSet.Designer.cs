namespace LasercuttingmachineUI
{
    partial class frm_SysSet
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
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.dateTimePicker1 = new System.Windows.Forms.DateTimePicker();
            this.label5 = new System.Windows.Forms.Label();
            this.btn_Exit = new LasercuttingmachineUI.Rich_Button();
            this.btn_Save = new LasercuttingmachineUI.Rich_Button();
            this.ipt_Adress = new LasercuttingmachineUI.IpInputTextbox();
            this.ipt_SubNetMask = new LasercuttingmachineUI.IpInputTextbox();
            this.ipt_GateWay = new LasercuttingmachineUI.IpInputTextbox();
            this.txt_Version = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("宋体", 9F);
            this.label1.Location = new System.Drawing.Point(6, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "IP地址";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("宋体", 9F);
            this.label2.Location = new System.Drawing.Point(6, 64);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 24);
            this.label2.TabIndex = 4;
            this.label2.Text = "子 网\r\n掩 码";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("宋体", 9F);
            this.label3.Location = new System.Drawing.Point(6, 121);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(35, 12);
            this.label3.TabIndex = 6;
            this.label3.Text = "网 关";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("宋体", 9F);
            this.label4.Location = new System.Drawing.Point(6, 160);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(35, 12);
            this.label4.TabIndex = 9;
            this.label4.Text = "日 期";
            // 
            // dateTimePicker1
            // 
            this.dateTimePicker1.CalendarFont = new System.Drawing.Font("宋体", 20.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.dateTimePicker1.CalendarMonthBackground = System.Drawing.SystemColors.WindowText;
            this.dateTimePicker1.CustomFormat = "";
            this.dateTimePicker1.Enabled = false;
            this.dateTimePicker1.Font = new System.Drawing.Font("宋体", 20F);
            this.dateTimePicker1.Location = new System.Drawing.Point(51, 149);
            this.dateTimePicker1.Name = "dateTimePicker1";
            this.dateTimePicker1.Size = new System.Drawing.Size(321, 38);
            this.dateTimePicker1.TabIndex = 11;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("宋体", 9F);
            this.label5.Location = new System.Drawing.Point(6, 209);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(41, 12);
            this.label5.TabIndex = 13;
            this.label5.Text = "版本号";
            // 
            // btn_Exit
            // 
            this.btn_Exit.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Exit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Exit.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Exit.Location = new System.Drawing.Point(274, 242);
            this.btn_Exit.Name = "btn_Exit";
            this.btn_Exit.Size = new System.Drawing.Size(98, 45);
            this.btn_Exit.TabIndex = 22;
            this.btn_Exit.Text = "退 出";
            this.btn_Exit.UseVisualStyleBackColor = true;
            this.btn_Exit.Click += new System.EventHandler(this.btn_Exit_Click);
            // 
            // btn_Save
            // 
            this.btn_Save.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Save.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Save.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Save.Location = new System.Drawing.Point(159, 242);
            this.btn_Save.Name = "btn_Save";
            this.btn_Save.Size = new System.Drawing.Size(98, 45);
            this.btn_Save.TabIndex = 21;
            this.btn_Save.Text = "保 存";
            this.btn_Save.UseVisualStyleBackColor = true;
            this.btn_Save.Click += new System.EventHandler(this.btn_Save_Click);
            // 
            // ipt_Adress
            // 
            this.ipt_Adress.Font = new System.Drawing.Font("宋体", 18F);
            this.ipt_Adress.IP = "IP地址格式不正确";
            this.ipt_Adress.Location = new System.Drawing.Point(51, 10);
            this.ipt_Adress.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.ipt_Adress.Name = "ipt_Adress";
            this.ipt_Adress.Size = new System.Drawing.Size(320, 42);
            this.ipt_Adress.TabIndex = 23;
            // 
            // ipt_SubNetMask
            // 
            this.ipt_SubNetMask.Font = new System.Drawing.Font("宋体", 18F);
            this.ipt_SubNetMask.IP = "IP地址格式不正确";
            this.ipt_SubNetMask.Location = new System.Drawing.Point(51, 57);
            this.ipt_SubNetMask.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.ipt_SubNetMask.Name = "ipt_SubNetMask";
            this.ipt_SubNetMask.Size = new System.Drawing.Size(320, 42);
            this.ipt_SubNetMask.TabIndex = 24;
            // 
            // ipt_GateWay
            // 
            this.ipt_GateWay.Font = new System.Drawing.Font("宋体", 18F);
            this.ipt_GateWay.IP = "IP地址格式不正确";
            this.ipt_GateWay.Location = new System.Drawing.Point(51, 103);
            this.ipt_GateWay.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.ipt_GateWay.Name = "ipt_GateWay";
            this.ipt_GateWay.Size = new System.Drawing.Size(320, 42);
            this.ipt_GateWay.TabIndex = 25;
            // 
            // txt_Version
            // 
            this.txt_Version.Enabled = false;
            this.txt_Version.Font = new System.Drawing.Font("宋体", 20F);
            this.txt_Version.Location = new System.Drawing.Point(51, 193);
            this.txt_Version.Name = "txt_Version";
            this.txt_Version.Size = new System.Drawing.Size(321, 38);
            this.txt_Version.TabIndex = 26;
            // 
            // frm_SysSet
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(384, 292);
            this.Controls.Add(this.txt_Version);
            this.Controls.Add(this.ipt_GateWay);
            this.Controls.Add(this.ipt_SubNetMask);
            this.Controls.Add(this.ipt_Adress);
            this.Controls.Add(this.btn_Exit);
            this.Controls.Add(this.btn_Save);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.dateTimePicker1);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Name = "frm_SysSet";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "frm_SysSet";
            this.VisibleChanged += new System.EventHandler(this.frm_SysSet_VisibleChanged);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.DateTimePicker dateTimePicker1;
        private System.Windows.Forms.Label label5;
        private Rich_Button btn_Exit;
        private Rich_Button btn_Save;
        private IpInputTextbox ipt_Adress;
        private IpInputTextbox ipt_SubNetMask;
        private IpInputTextbox ipt_GateWay;
        private System.Windows.Forms.TextBox txt_Version;
    }
}