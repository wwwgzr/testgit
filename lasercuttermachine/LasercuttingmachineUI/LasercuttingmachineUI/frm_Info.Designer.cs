namespace LasercuttingmachineUI
{
    partial class frm_Info
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
            this.lbl_FileX = new System.Windows.Forms.Label();
            this.lbl_FileY = new System.Windows.Forms.Label();
            this.lbl_CutNum = new System.Windows.Forms.Label();
            this.lbl_WorkTime = new System.Windows.Forms.Label();
            this.lbl_ConnectState = new System.Windows.Forms.Label();
            this.lbl_WorkState = new System.Windows.Forms.Label();
            this.lbl_FileName = new System.Windows.Forms.Label();
            this.lbl_DownClothState = new System.Windows.Forms.Label();
            this.lbl_Auto = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // lbl_FileX
            // 
            this.lbl_FileX.AutoSize = true;
            this.lbl_FileX.BackColor = System.Drawing.Color.Transparent;
            this.lbl_FileX.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_FileX.ForeColor = System.Drawing.SystemColors.Control;
            this.lbl_FileX.Location = new System.Drawing.Point(366, 28);
            this.lbl_FileX.Name = "lbl_FileX";
            this.lbl_FileX.Size = new System.Drawing.Size(88, 21);
            this.lbl_FileX.TabIndex = 6;
            this.lbl_FileX.Text = "文件X轴宽:";
            // 
            // lbl_FileY
            // 
            this.lbl_FileY.AutoSize = true;
            this.lbl_FileY.BackColor = System.Drawing.Color.Transparent;
            this.lbl_FileY.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_FileY.ForeColor = System.Drawing.SystemColors.Control;
            this.lbl_FileY.Location = new System.Drawing.Point(366, 49);
            this.lbl_FileY.Name = "lbl_FileY";
            this.lbl_FileY.Size = new System.Drawing.Size(88, 21);
            this.lbl_FileY.TabIndex = 7;
            this.lbl_FileY.Text = "文件Y轴宽:";
            // 
            // lbl_CutNum
            // 
            this.lbl_CutNum.AutoSize = true;
            this.lbl_CutNum.BackColor = System.Drawing.Color.Transparent;
            this.lbl_CutNum.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_CutNum.ForeColor = System.Drawing.SystemColors.Control;
            this.lbl_CutNum.Location = new System.Drawing.Point(7, 28);
            this.lbl_CutNum.Name = "lbl_CutNum";
            this.lbl_CutNum.Size = new System.Drawing.Size(87, 21);
            this.lbl_CutNum.TabIndex = 8;
            this.lbl_CutNum.Text = "完成数量:0";
            // 
            // lbl_WorkTime
            // 
            this.lbl_WorkTime.AutoSize = true;
            this.lbl_WorkTime.BackColor = System.Drawing.Color.Transparent;
            this.lbl_WorkTime.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_WorkTime.ForeColor = System.Drawing.SystemColors.Control;
            this.lbl_WorkTime.Location = new System.Drawing.Point(7, 49);
            this.lbl_WorkTime.Name = "lbl_WorkTime";
            this.lbl_WorkTime.Size = new System.Drawing.Size(87, 21);
            this.lbl_WorkTime.TabIndex = 9;
            this.lbl_WorkTime.Text = "工作时间:0";
            // 
            // lbl_ConnectState
            // 
            this.lbl_ConnectState.AutoSize = true;
            this.lbl_ConnectState.BackColor = System.Drawing.Color.Transparent;
            this.lbl_ConnectState.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_ConnectState.ForeColor = System.Drawing.SystemColors.Control;
            this.lbl_ConnectState.Location = new System.Drawing.Point(188, 28);
            this.lbl_ConnectState.Name = "lbl_ConnectState";
            this.lbl_ConnectState.Size = new System.Drawing.Size(78, 21);
            this.lbl_ConnectState.TabIndex = 10;
            this.lbl_ConnectState.Text = "连接状态:";
            // 
            // lbl_WorkState
            // 
            this.lbl_WorkState.AutoSize = true;
            this.lbl_WorkState.BackColor = System.Drawing.Color.Transparent;
            this.lbl_WorkState.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_WorkState.ForeColor = System.Drawing.SystemColors.Control;
            this.lbl_WorkState.Location = new System.Drawing.Point(188, 49);
            this.lbl_WorkState.Name = "lbl_WorkState";
            this.lbl_WorkState.Size = new System.Drawing.Size(126, 21);
            this.lbl_WorkState.TabIndex = 12;
            this.lbl_WorkState.Text = "设备状态:已停止";
            // 
            // lbl_FileName
            // 
            this.lbl_FileName.BackColor = System.Drawing.Color.Transparent;
            this.lbl_FileName.Dock = System.Windows.Forms.DockStyle.Top;
            this.lbl_FileName.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_FileName.ForeColor = System.Drawing.SystemColors.Control;
            this.lbl_FileName.Location = new System.Drawing.Point(0, 0);
            this.lbl_FileName.Name = "lbl_FileName";
            this.lbl_FileName.Size = new System.Drawing.Size(724, 28);
            this.lbl_FileName.TabIndex = 14;
            this.lbl_FileName.Text = "文件名称:";
            this.lbl_FileName.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lbl_DownClothState
            // 
            this.lbl_DownClothState.AutoSize = true;
            this.lbl_DownClothState.BackColor = System.Drawing.Color.Transparent;
            this.lbl_DownClothState.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_DownClothState.ForeColor = System.Drawing.SystemColors.Control;
            this.lbl_DownClothState.Location = new System.Drawing.Point(580, 28);
            this.lbl_DownClothState.Name = "lbl_DownClothState";
            this.lbl_DownClothState.Size = new System.Drawing.Size(90, 21);
            this.lbl_DownClothState.TabIndex = 15;
            this.lbl_DownClothState.Text = "压布辊落下";
            // 
            // lbl_Auto
            // 
            this.lbl_Auto.AutoSize = true;
            this.lbl_Auto.BackColor = System.Drawing.Color.Transparent;
            this.lbl_Auto.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_Auto.ForeColor = System.Drawing.SystemColors.Control;
            this.lbl_Auto.Location = new System.Drawing.Point(580, 49);
            this.lbl_Auto.Name = "lbl_Auto";
            this.lbl_Auto.Size = new System.Drawing.Size(74, 21);
            this.lbl_Auto.TabIndex = 16;
            this.lbl_Auto.Text = "手动状态";
            // 
            // frm_Info
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Black;
            this.ClientSize = new System.Drawing.Size(724, 80);
            this.Controls.Add(this.lbl_Auto);
            this.Controls.Add(this.lbl_DownClothState);
            this.Controls.Add(this.lbl_FileName);
            this.Controls.Add(this.lbl_WorkState);
            this.Controls.Add(this.lbl_ConnectState);
            this.Controls.Add(this.lbl_WorkTime);
            this.Controls.Add(this.lbl_CutNum);
            this.Controls.Add(this.lbl_FileY);
            this.Controls.Add(this.lbl_FileX);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "frm_Info";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "frm_Info";
            this.TransparencyKey = System.Drawing.Color.Black;
            this.Load += new System.EventHandler(this.frm_Info_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lbl_FileX;
        private System.Windows.Forms.Label lbl_FileY;
        private System.Windows.Forms.Label lbl_CutNum;
        private System.Windows.Forms.Label lbl_WorkTime;
        private System.Windows.Forms.Label lbl_ConnectState;
        private System.Windows.Forms.Label lbl_WorkState;
        private System.Windows.Forms.Label lbl_FileName;
        private System.Windows.Forms.Label lbl_DownClothState;
        private System.Windows.Forms.Label lbl_Auto;
    }
}