namespace LasercuttingmachineUI
{
    partial class frm_EditUser
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
            this.btn_Exit = new LasercuttingmachineUI.Rich_Button();
            this.btn_Save = new LasercuttingmachineUI.Rich_Button();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.txt_OldPassword = new System.Windows.Forms.TextBox();
            this.txt_NewPassword = new System.Windows.Forms.TextBox();
            this.txt_Password = new System.Windows.Forms.TextBox();
            this.btn_Reset = new LasercuttingmachineUI.Rich_Button();
            this.label2 = new System.Windows.Forms.Label();
            this.rab_Guest = new System.Windows.Forms.RadioButton();
            this.rab_Admin = new System.Windows.Forms.RadioButton();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.SuspendLayout();
            // 
            // btn_Exit
            // 
            this.btn_Exit.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Exit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btn_Exit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Exit.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Exit.Location = new System.Drawing.Point(263, 221);
            this.btn_Exit.MouseDownImage = null;
            this.btn_Exit.Name = "btn_Exit";
            this.btn_Exit.Size = new System.Drawing.Size(98, 45);
            this.btn_Exit.TabIndex = 7;
            this.btn_Exit.Text = "退 出";
            this.btn_Exit.UseVisualStyleBackColor = true;
            this.btn_Exit.Click += new System.EventHandler(this.btn_Exit_Click);
            // 
            // btn_Save
            // 
            this.btn_Save.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Save.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Save.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Save.Location = new System.Drawing.Point(149, 221);
            this.btn_Save.MouseDownImage = null;
            this.btn_Save.Name = "btn_Save";
            this.btn_Save.Size = new System.Drawing.Size(98, 45);
            this.btn_Save.TabIndex = 5;
            this.btn_Save.Text = "保 存";
            this.btn_Save.UseVisualStyleBackColor = true;
            this.btn_Save.Click += new System.EventHandler(this.btn_Save_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("宋体", 9F);
            this.label6.Location = new System.Drawing.Point(12, 75);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(89, 12);
            this.label6.TabIndex = 25;
            this.label6.Text = "管 理 员 密 码";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("宋体", 9F);
            this.label7.Location = new System.Drawing.Point(12, 129);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(89, 12);
            this.label7.TabIndex = 26;
            this.label7.Text = "新    密    码";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("宋体", 9F);
            this.label1.Location = new System.Drawing.Point(12, 183);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(89, 12);
            this.label1.TabIndex = 29;
            this.label1.Text = "再次输入新密码";
            // 
            // txt_OldPassword
            // 
            this.txt_OldPassword.Font = new System.Drawing.Font("宋体", 22F);
            this.txt_OldPassword.Location = new System.Drawing.Point(116, 57);
            this.txt_OldPassword.Name = "txt_OldPassword";
            this.txt_OldPassword.PasswordChar = '*';
            this.txt_OldPassword.Size = new System.Drawing.Size(245, 41);
            this.txt_OldPassword.TabIndex = 2;
            this.txt_OldPassword.MouseClick += new System.Windows.Forms.MouseEventHandler(this.txt_Password_MouseClick);
            this.txt_OldPassword.TextChanged += new System.EventHandler(this.txt_OldPassword_TextChanged);
            this.txt_OldPassword.Validated += new System.EventHandler(this.txt_OldPassword_Validated);
            // 
            // txt_NewPassword
            // 
            this.txt_NewPassword.Font = new System.Drawing.Font("宋体", 22F);
            this.txt_NewPassword.Location = new System.Drawing.Point(116, 111);
            this.txt_NewPassword.Name = "txt_NewPassword";
            this.txt_NewPassword.PasswordChar = '*';
            this.txt_NewPassword.Size = new System.Drawing.Size(245, 41);
            this.txt_NewPassword.TabIndex = 3;
            this.txt_NewPassword.MouseClick += new System.Windows.Forms.MouseEventHandler(this.txt_Password_MouseClick);
            this.txt_NewPassword.TextChanged += new System.EventHandler(this.txt_OldPassword_TextChanged);
            this.txt_NewPassword.Validated += new System.EventHandler(this.txt_NewPassword_Validated);
            // 
            // txt_Password
            // 
            this.txt_Password.Font = new System.Drawing.Font("宋体", 22F);
            this.txt_Password.Location = new System.Drawing.Point(116, 165);
            this.txt_Password.Name = "txt_Password";
            this.txt_Password.PasswordChar = '*';
            this.txt_Password.Size = new System.Drawing.Size(245, 41);
            this.txt_Password.TabIndex = 4;
            this.txt_Password.MouseClick += new System.Windows.Forms.MouseEventHandler(this.txt_Password_MouseClick);
            this.txt_Password.TextChanged += new System.EventHandler(this.txt_OldPassword_TextChanged);
            this.txt_Password.Validated += new System.EventHandler(this.txt_Password_Validated);
            // 
            // btn_Reset
            // 
            this.btn_Reset.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Reset.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btn_Reset.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Reset.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Reset.Location = new System.Drawing.Point(33, 221);
            this.btn_Reset.MouseDownImage = null;
            this.btn_Reset.Name = "btn_Reset";
            this.btn_Reset.Size = new System.Drawing.Size(98, 45);
            this.btn_Reset.TabIndex = 6;
            this.btn_Reset.Text = "重 置";
            this.btn_Reset.UseVisualStyleBackColor = true;
            this.btn_Reset.Click += new System.EventHandler(this.btn_Reset_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("宋体", 9F);
            this.label2.Location = new System.Drawing.Point(12, 22);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(89, 12);
            this.label2.TabIndex = 31;
            this.label2.Text = "用  户  类  型";
            // 
            // rab_Guest
            // 
            this.rab_Guest.AutoSize = true;
            this.rab_Guest.Location = new System.Drawing.Point(120, 12);
            this.rab_Guest.Name = "rab_Guest";
            this.rab_Guest.Size = new System.Drawing.Size(107, 40);
            this.rab_Guest.TabIndex = 0;
            this.rab_Guest.Text = "\r\n职工          \r\n\r\n";
            this.rab_Guest.UseVisualStyleBackColor = true;
            this.rab_Guest.CheckedChanged += new System.EventHandler(this.rab_Guest_CheckedChanged);
            // 
            // rab_Admin
            // 
            this.rab_Admin.AutoSize = true;
            this.rab_Admin.Checked = true;
            this.rab_Admin.Location = new System.Drawing.Point(246, 12);
            this.rab_Admin.Name = "rab_Admin";
            this.rab_Admin.Size = new System.Drawing.Size(119, 40);
            this.rab_Admin.TabIndex = 1;
            this.rab_Admin.TabStop = true;
            this.rab_Admin.Text = "\r\n管理员          \r\n\r\n";
            this.rab_Admin.UseVisualStyleBackColor = true;
            this.rab_Admin.CheckedChanged += new System.EventHandler(this.rab_Admin_CheckedChanged);
            // 
            // frm_EditUser
            // 
            this.AcceptButton = this.btn_Save;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btn_Exit;
            this.ClientSize = new System.Drawing.Size(383, 290);
            this.Controls.Add(this.rab_Admin);
            this.Controls.Add(this.rab_Guest);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.btn_Reset);
            this.Controls.Add(this.txt_Password);
            this.Controls.Add(this.txt_NewPassword);
            this.Controls.Add(this.txt_OldPassword);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.btn_Exit);
            this.Controls.Add(this.btn_Save);
            this.Name = "frm_EditUser";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "frm_EditUser";
            this.VisibleChanged += new System.EventHandler(this.frm_EditUser_VisibleChanged);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private Rich_Button btn_Exit;
        private Rich_Button btn_Save;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txt_OldPassword;
        private System.Windows.Forms.TextBox txt_NewPassword;
        private System.Windows.Forms.TextBox txt_Password;
        private Rich_Button btn_Reset;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.RadioButton rab_Guest;
        private System.Windows.Forms.RadioButton rab_Admin;
        private System.Windows.Forms.ToolTip toolTip1;
    }
}