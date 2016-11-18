namespace LasercuttingmachineUI
{
    partial class frm_OperationPanel
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
            this.txt_Value = new LasercuttingmachineUI.Rich_TextBox();
            this.btn_ClearAll = new System.Windows.Forms.Button();
            this.btn_Clear = new System.Windows.Forms.Button();
            this.btn_9 = new System.Windows.Forms.Button();
            this.btn_7 = new System.Windows.Forms.Button();
            this.btn_P = new System.Windows.Forms.Button();
            this.btn_4 = new System.Windows.Forms.Button();
            this.btn_2 = new System.Windows.Forms.Button();
            this.btn_0 = new System.Windows.Forms.Button();
            this.btn_8 = new System.Windows.Forms.Button();
            this.btn_6 = new System.Windows.Forms.Button();
            this.btn_5 = new System.Windows.Forms.Button();
            this.btn_3 = new System.Windows.Forms.Button();
            this.btn_1 = new System.Windows.Forms.Button();
            this.btn_Enter = new System.Windows.Forms.Button();
            this.btn_Return = new System.Windows.Forms.Button();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.SuspendLayout();
            // 
            // txt_Value
            // 
            this.txt_Value.Font = new System.Drawing.Font("宋体", 15F);
            this.txt_Value.Location = new System.Drawing.Point(12, 305);
            this.txt_Value.MaxValue = 0D;
            this.txt_Value.MinValue = 0D;
            this.txt_Value.Name = "txt_Value";
            this.txt_Value.Size = new System.Drawing.Size(215, 30);
            this.txt_Value.TabIndex = 24;
            this.txt_Value.Visible = false;
            // 
            // btn_ClearAll
            // 
            this.btn_ClearAll.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_ClearAll.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_ClearAll.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_ClearAll.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_ClearAll.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_ClearAll.Location = new System.Drawing.Point(171, 60);
            this.btn_ClearAll.Name = "btn_ClearAll";
            this.btn_ClearAll.Size = new System.Drawing.Size(50, 50);
            this.btn_ClearAll.TabIndex = 23;
            this.btn_ClearAll.Text = "<-";
            this.btn_ClearAll.UseVisualStyleBackColor = false;
            this.btn_ClearAll.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btn_ClearAll_MouseDown);
            this.btn_ClearAll.MouseUp += new System.Windows.Forms.MouseEventHandler(this.btn_ClearAll_MouseUp);
            // 
            // btn_Clear
            // 
            this.btn_Clear.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Clear.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_Clear.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_Clear.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Clear.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Clear.Location = new System.Drawing.Point(171, 4);
            this.btn_Clear.Name = "btn_Clear";
            this.btn_Clear.Size = new System.Drawing.Size(50, 50);
            this.btn_Clear.TabIndex = 22;
            this.btn_Clear.Tag = "Clear";
            this.btn_Clear.Text = "C";
            this.btn_Clear.UseVisualStyleBackColor = false;
            // 
            // btn_9
            // 
            this.btn_9.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_9.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_9.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_9.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_9.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_9.Location = new System.Drawing.Point(115, 116);
            this.btn_9.Name = "btn_9";
            this.btn_9.Size = new System.Drawing.Size(50, 50);
            this.btn_9.TabIndex = 22;
            this.btn_9.Tag = "9";
            this.btn_9.Text = "9";
            this.btn_9.UseVisualStyleBackColor = false;
            // 
            // btn_7
            // 
            this.btn_7.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_7.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_7.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_7.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_7.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_7.Location = new System.Drawing.Point(3, 116);
            this.btn_7.Name = "btn_7";
            this.btn_7.Size = new System.Drawing.Size(50, 50);
            this.btn_7.TabIndex = 22;
            this.btn_7.Tag = "7";
            this.btn_7.Text = "7";
            this.btn_7.UseVisualStyleBackColor = false;
            // 
            // btn_P
            // 
            this.btn_P.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_P.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_P.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_P.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_P.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_P.Font = new System.Drawing.Font("宋体", 10.5F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btn_P.Location = new System.Drawing.Point(59, 172);
            this.btn_P.Name = "btn_P";
            this.btn_P.Size = new System.Drawing.Size(50, 50);
            this.btn_P.TabIndex = 21;
            this.btn_P.Tag = "P";
            this.btn_P.Text = ".";
            this.btn_P.UseVisualStyleBackColor = false;
            // 
            // btn_4
            // 
            this.btn_4.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_4.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_4.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_4.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_4.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_4.Location = new System.Drawing.Point(3, 60);
            this.btn_4.Name = "btn_4";
            this.btn_4.Size = new System.Drawing.Size(50, 50);
            this.btn_4.TabIndex = 21;
            this.btn_4.Tag = "4";
            this.btn_4.Text = "4";
            this.btn_4.UseVisualStyleBackColor = false;
            // 
            // btn_2
            // 
            this.btn_2.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_2.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_2.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_2.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_2.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_2.Location = new System.Drawing.Point(59, 4);
            this.btn_2.Name = "btn_2";
            this.btn_2.Size = new System.Drawing.Size(50, 50);
            this.btn_2.TabIndex = 21;
            this.btn_2.Tag = "2";
            this.btn_2.Text = "2";
            this.btn_2.UseVisualStyleBackColor = false;
            // 
            // btn_0
            // 
            this.btn_0.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_0.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_0.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_0.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_0.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_0.Location = new System.Drawing.Point(3, 172);
            this.btn_0.Name = "btn_0";
            this.btn_0.Size = new System.Drawing.Size(50, 50);
            this.btn_0.TabIndex = 20;
            this.btn_0.Tag = "0";
            this.btn_0.Text = "0";
            this.btn_0.UseVisualStyleBackColor = false;
            // 
            // btn_8
            // 
            this.btn_8.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_8.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_8.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_8.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_8.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_8.Location = new System.Drawing.Point(59, 116);
            this.btn_8.Name = "btn_8";
            this.btn_8.Size = new System.Drawing.Size(50, 50);
            this.btn_8.TabIndex = 20;
            this.btn_8.Tag = "8";
            this.btn_8.Text = "8";
            this.btn_8.UseVisualStyleBackColor = false;
            // 
            // btn_6
            // 
            this.btn_6.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_6.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_6.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_6.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_6.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_6.Location = new System.Drawing.Point(115, 60);
            this.btn_6.Name = "btn_6";
            this.btn_6.Size = new System.Drawing.Size(50, 50);
            this.btn_6.TabIndex = 20;
            this.btn_6.Tag = "6";
            this.btn_6.Text = "6";
            this.btn_6.UseVisualStyleBackColor = false;
            // 
            // btn_5
            // 
            this.btn_5.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_5.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_5.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_5.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_5.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_5.Location = new System.Drawing.Point(59, 60);
            this.btn_5.Name = "btn_5";
            this.btn_5.Size = new System.Drawing.Size(50, 50);
            this.btn_5.TabIndex = 19;
            this.btn_5.Tag = "5";
            this.btn_5.Text = "5";
            this.btn_5.UseVisualStyleBackColor = false;
            // 
            // btn_3
            // 
            this.btn_3.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_3.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_3.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_3.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_3.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_3.Location = new System.Drawing.Point(115, 5);
            this.btn_3.Name = "btn_3";
            this.btn_3.Size = new System.Drawing.Size(50, 50);
            this.btn_3.TabIndex = 19;
            this.btn_3.Tag = "3";
            this.btn_3.Text = "3";
            this.btn_3.UseVisualStyleBackColor = false;
            // 
            // btn_1
            // 
            this.btn_1.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_1.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_1.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_1.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_1.Location = new System.Drawing.Point(3, 4);
            this.btn_1.Name = "btn_1";
            this.btn_1.Size = new System.Drawing.Size(50, 50);
            this.btn_1.TabIndex = 19;
            this.btn_1.Tag = "1";
            this.btn_1.Text = "1";
            this.btn_1.UseVisualStyleBackColor = false;
            // 
            // btn_Enter
            // 
            this.btn_Enter.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Enter.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_Enter.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_Enter.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Enter.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Enter.Location = new System.Drawing.Point(115, 173);
            this.btn_Enter.Name = "btn_Enter";
            this.btn_Enter.Size = new System.Drawing.Size(106, 50);
            this.btn_Enter.TabIndex = 8;
            this.btn_Enter.Text = "Enter";
            this.btn_Enter.UseVisualStyleBackColor = false;
            // 
            // btn_Return
            // 
            this.btn_Return.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Return.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.btn_Return.FlatAppearance.MouseDownBackColor = System.Drawing.Color.Lime;
            this.btn_Return.FlatAppearance.MouseOverBackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Return.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Return.Location = new System.Drawing.Point(171, 116);
            this.btn_Return.Name = "btn_Return";
            this.btn_Return.Size = new System.Drawing.Size(50, 50);
            this.btn_Return.TabIndex = 5;
            this.btn_Return.Text = "Esc";
            this.btn_Return.UseVisualStyleBackColor = false;
            // 
            // backgroundWorker1
            // 
            this.backgroundWorker1.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker1_DoWork);
            // 
            // frm_OperationPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(225, 229);
            this.Controls.Add(this.txt_Value);
            this.Controls.Add(this.btn_ClearAll);
            this.Controls.Add(this.btn_Clear);
            this.Controls.Add(this.btn_9);
            this.Controls.Add(this.btn_7);
            this.Controls.Add(this.btn_P);
            this.Controls.Add(this.btn_4);
            this.Controls.Add(this.btn_2);
            this.Controls.Add(this.btn_0);
            this.Controls.Add(this.btn_8);
            this.Controls.Add(this.btn_6);
            this.Controls.Add(this.btn_5);
            this.Controls.Add(this.btn_3);
            this.Controls.Add(this.btn_1);
            this.Controls.Add(this.btn_Enter);
            this.Controls.Add(this.btn_Return);
            this.Name = "frm_OperationPanel";
            this.ShowInTaskbar = true;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "frm_OperationPanel";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btn_Return;
        private System.Windows.Forms.Button btn_Enter;
        private System.Windows.Forms.Button btn_1;
        private System.Windows.Forms.Button btn_6;
        private System.Windows.Forms.Button btn_7;
        private System.Windows.Forms.Button btn_2;
        private System.Windows.Forms.Button btn_3;
        private System.Windows.Forms.Button btn_8;
        private System.Windows.Forms.Button btn_4;
        private System.Windows.Forms.Button btn_9;
        private System.Windows.Forms.Button btn_5;
        private System.Windows.Forms.Button btn_0;
        private System.Windows.Forms.Button btn_P;
        private System.Windows.Forms.Button btn_Clear;
        private System.Windows.Forms.Button btn_ClearAll;
        private Rich_TextBox txt_Value;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
    }
}