namespace LasercuttingmachineUI
{
    partial class frm_SetManualFeed
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
            this.btn_Exit = new LasercuttingmachineUI.Rich_Button();
            this.btn_Save = new LasercuttingmachineUI.Rich_Button();
            this.txt_FeedLen = new LasercuttingmachineUI.Rich_TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.txt_FeedOffset = new LasercuttingmachineUI.Rich_TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.txt_SectionLen = new LasercuttingmachineUI.Rich_TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btn_Exit
            // 
            this.btn_Exit.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Exit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Exit.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Exit.Location = new System.Drawing.Point(116, 213);
            this.btn_Exit.MouseDownImage = null;
            this.btn_Exit.Name = "btn_Exit";
            this.btn_Exit.Size = new System.Drawing.Size(98, 45);
            this.btn_Exit.TabIndex = 26;
            this.btn_Exit.Text = "退 出";
            this.btn_Exit.UseVisualStyleBackColor = true;
            this.btn_Exit.Click += new System.EventHandler(this.btn_Exit_Click);
            // 
            // btn_Save
            // 
            this.btn_Save.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Save.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Save.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Save.Location = new System.Drawing.Point(12, 213);
            this.btn_Save.MouseDownImage = null;
            this.btn_Save.Name = "btn_Save";
            this.btn_Save.Size = new System.Drawing.Size(98, 45);
            this.btn_Save.TabIndex = 25;
            this.btn_Save.Text = "保 存";
            this.btn_Save.UseVisualStyleBackColor = true;
            this.btn_Save.Click += new System.EventHandler(this.btn_Save_Click);
            // 
            // txt_FeedLen
            // 
            this.txt_FeedLen.Font = new System.Drawing.Font("宋体", 21.5F);
            this.txt_FeedLen.Location = new System.Drawing.Point(24, 27);
            this.txt_FeedLen.MaxValue = 32767D;
            this.txt_FeedLen.MinValue = 0D;
            this.txt_FeedLen.Name = "txt_FeedLen";
            this.txt_FeedLen.Size = new System.Drawing.Size(179, 40);
            this.txt_FeedLen.TabIndex = 24;
            this.txt_FeedLen.Text = "10";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(22, 12);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(101, 12);
            this.label6.TabIndex = 23;
            this.label6.Text = "手动进料长度(mm)";
            // 
            // txt_FeedOffset
            // 
            this.txt_FeedOffset.Font = new System.Drawing.Font("宋体", 21.5F);
            this.txt_FeedOffset.Location = new System.Drawing.Point(24, 92);
            this.txt_FeedOffset.MaxValue = 32767D;
            this.txt_FeedOffset.MinValue = -32768D;
            this.txt_FeedOffset.Name = "txt_FeedOffset";
            this.txt_FeedOffset.Size = new System.Drawing.Size(179, 40);
            this.txt_FeedOffset.TabIndex = 28;
            this.txt_FeedOffset.Text = "300";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(22, 77);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(101, 12);
            this.label1.TabIndex = 27;
            this.label1.Text = "手动上料偏移(mm)";
            // 
            // txt_SectionLen
            // 
            this.txt_SectionLen.Font = new System.Drawing.Font("宋体", 21.5F);
            this.txt_SectionLen.Location = new System.Drawing.Point(23, 155);
            this.txt_SectionLen.MaxValue = 32767D;
            this.txt_SectionLen.MinValue = -32768D;
            this.txt_SectionLen.Name = "txt_SectionLen";
            this.txt_SectionLen.Size = new System.Drawing.Size(179, 40);
            this.txt_SectionLen.TabIndex = 30;
            this.txt_SectionLen.Text = "100";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(21, 140);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(125, 12);
            this.label2.TabIndex = 29;
            this.label2.Text = "手动上料分段长度(mm)";
            // 
            // frm_SetManualFeed
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.ClientSize = new System.Drawing.Size(227, 270);
            this.Controls.Add(this.txt_SectionLen);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txt_FeedOffset);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btn_Exit);
            this.Controls.Add(this.btn_Save);
            this.Controls.Add(this.txt_FeedLen);
            this.Controls.Add(this.label6);
            this.Name = "frm_SetManualFeed";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.VisibleChanged += new System.EventHandler(this.frm_SetManualFeed_VisibleChanged);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private Rich_Button btn_Exit;
        private Rich_Button btn_Save;
        private Rich_TextBox txt_FeedLen;
        private System.Windows.Forms.Label label6;
        private Rich_TextBox txt_FeedOffset;
        private System.Windows.Forms.Label label1;
        private Rich_TextBox txt_SectionLen;
        private System.Windows.Forms.Label label2;

    }
}

