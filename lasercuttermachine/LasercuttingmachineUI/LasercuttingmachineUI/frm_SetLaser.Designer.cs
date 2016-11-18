namespace LasercuttingmachineUI
{
    partial class frm_SetLaser
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
            this.txt_LaserPointLight = new LasercuttingmachineUI.Rich_TextBox();
            this.txt_LaserPointTime = new LasercuttingmachineUI.Rich_TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.btn_Exit = new LasercuttingmachineUI.Rich_Button();
            this.btn_Save = new LasercuttingmachineUI.Rich_Button();
            this.rab_LaserA = new System.Windows.Forms.RadioButton();
            this.rab_LaserB = new System.Windows.Forms.RadioButton();
            this.SuspendLayout();
            // 
            // txt_LaserPointLight
            // 
            this.txt_LaserPointLight.Font = new System.Drawing.Font("宋体", 21.5F);
            this.txt_LaserPointLight.Location = new System.Drawing.Point(21, 128);
            this.txt_LaserPointLight.MaxValue = 100D;
            this.txt_LaserPointLight.MinValue = 0D;
            this.txt_LaserPointLight.Name = "txt_LaserPointLight";
            this.txt_LaserPointLight.Size = new System.Drawing.Size(179, 40);
            this.txt_LaserPointLight.TabIndex = 19;
            this.txt_LaserPointLight.Text = "0";
            // 
            // txt_LaserPointTime
            // 
            this.txt_LaserPointTime.Font = new System.Drawing.Font("宋体", 21.5F);
            this.txt_LaserPointTime.Location = new System.Drawing.Point(21, 61);
            this.txt_LaserPointTime.MaxValue = 500D;
            this.txt_LaserPointTime.MinValue = 10D;
            this.txt_LaserPointTime.Name = "txt_LaserPointTime";
            this.txt_LaserPointTime.Size = new System.Drawing.Size(179, 40);
            this.txt_LaserPointTime.TabIndex = 18;
            this.txt_LaserPointTime.Text = "100";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(19, 46);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(77, 12);
            this.label6.TabIndex = 16;
            this.label6.Text = "激光点射延时";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(19, 113);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(131, 12);
            this.label7.TabIndex = 17;
            this.label7.Text = "激光点射光强（0-100）";
            // 
            // btn_Exit
            // 
            this.btn_Exit.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Exit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Exit.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Exit.Location = new System.Drawing.Point(119, 183);
            this.btn_Exit.MouseDownImage = null;
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
            this.btn_Save.Location = new System.Drawing.Point(5, 182);
            this.btn_Save.MouseDownImage = null;
            this.btn_Save.Name = "btn_Save";
            this.btn_Save.Size = new System.Drawing.Size(98, 45);
            this.btn_Save.TabIndex = 21;
            this.btn_Save.Text = "点 射";
            this.btn_Save.UseVisualStyleBackColor = true;
            this.btn_Save.Click += new System.EventHandler(this.btn_Save_Click);
            // 
            // rab_LaserA
            // 
            this.rab_LaserA.AutoSize = true;
            this.rab_LaserA.Checked = true;
            this.rab_LaserA.Font = new System.Drawing.Font("宋体", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rab_LaserA.Location = new System.Drawing.Point(15, 12);
            this.rab_LaserA.Name = "rab_LaserA";
            this.rab_LaserA.Size = new System.Drawing.Size(94, 23);
            this.rab_LaserA.TabIndex = 23;
            this.rab_LaserA.TabStop = true;
            this.rab_LaserA.Text = "A激光头";
            this.rab_LaserA.UseVisualStyleBackColor = true;
            this.rab_LaserA.CheckedChanged += new System.EventHandler(this.rab_LaserA_CheckedChanged);
            // 
            // rab_LaserB
            // 
            this.rab_LaserB.AutoSize = true;
            this.rab_LaserB.Font = new System.Drawing.Font("宋体", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.rab_LaserB.Location = new System.Drawing.Point(114, 12);
            this.rab_LaserB.Name = "rab_LaserB";
            this.rab_LaserB.Size = new System.Drawing.Size(94, 23);
            this.rab_LaserB.TabIndex = 24;
            this.rab_LaserB.Text = "B激光头";
            this.rab_LaserB.UseVisualStyleBackColor = true;
            this.rab_LaserB.CheckedChanged += new System.EventHandler(this.rab_LaserB_CheckedChanged);
            // 
            // frm_SetLaser
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(223, 235);
            this.Controls.Add(this.rab_LaserB);
            this.Controls.Add(this.rab_LaserA);
            this.Controls.Add(this.btn_Exit);
            this.Controls.Add(this.btn_Save);
            this.Controls.Add(this.txt_LaserPointLight);
            this.Controls.Add(this.txt_LaserPointTime);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label7);
            this.Name = "frm_SetLaser";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "frm_SetLaser";
            this.VisibleChanged += new System.EventHandler(this.frm_SetLaser_VisibleChanged);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private Rich_TextBox txt_LaserPointLight;
        private Rich_TextBox txt_LaserPointTime;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private Rich_Button btn_Exit;
        private Rich_Button btn_Save;
        private System.Windows.Forms.RadioButton rab_LaserA;
        private System.Windows.Forms.RadioButton rab_LaserB;
    }
}