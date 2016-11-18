namespace LasercuttingmachineUI
{
    partial class frm_AddFile
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
            this.panel1 = new System.Windows.Forms.Panel();
            this.btn_Quit = new LasercuttingmachineUI.Rich_Button();
            this.btn_ExportParam = new LasercuttingmachineUI.Rich_Button();
            this.btn_SetParam = new LasercuttingmachineUI.Rich_Button();
            this.btn_SetOrigin = new LasercuttingmachineUI.Rich_Button();
            this.btn_Add = new LasercuttingmachineUI.Rich_Button();
            this.btn_Save = new LasercuttingmachineUI.Rich_Button();
            this.paintingPanel1 = new LasercuttingmachineUI.PaintingPanel();
            this.label1 = new System.Windows.Forms.Label();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.btn_Quit);
            this.panel1.Controls.Add(this.btn_ExportParam);
            this.panel1.Controls.Add(this.btn_SetParam);
            this.panel1.Controls.Add(this.btn_SetOrigin);
            this.panel1.Controls.Add(this.btn_Add);
            this.panel1.Controls.Add(this.btn_Save);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 400);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(760, 60);
            this.panel1.TabIndex = 7;
            // 
            // btn_Quit
            // 
            this.btn_Quit.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Quit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Quit.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Quit.Location = new System.Drawing.Point(612, 6);
            this.btn_Quit.MouseDownImage = null;
            this.btn_Quit.Name = "btn_Quit";
            this.btn_Quit.Size = new System.Drawing.Size(98, 45);
            this.btn_Quit.TabIndex = 44;
            this.btn_Quit.Text = "退 出";
            this.btn_Quit.UseVisualStyleBackColor = true;
            this.btn_Quit.Click += new System.EventHandler(this.btn_Quit_Click);
            // 
            // btn_ExportParam
            // 
            this.btn_ExportParam.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_ExportParam.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_ExportParam.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_ExportParam.Location = new System.Drawing.Point(329, 6);
            this.btn_ExportParam.MouseDownImage = null;
            this.btn_ExportParam.Name = "btn_ExportParam";
            this.btn_ExportParam.Size = new System.Drawing.Size(90, 45);
            this.btn_ExportParam.TabIndex = 43;
            this.btn_ExportParam.Text = "导入参数";
            this.btn_ExportParam.UseVisualStyleBackColor = true;
            this.btn_ExportParam.Click += new System.EventHandler(this.btn_ExportParam_Click);
            // 
            // btn_SetParam
            // 
            this.btn_SetParam.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_SetParam.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_SetParam.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_SetParam.Location = new System.Drawing.Point(151, 6);
            this.btn_SetParam.MouseDownImage = null;
            this.btn_SetParam.Name = "btn_SetParam";
            this.btn_SetParam.Size = new System.Drawing.Size(90, 45);
            this.btn_SetParam.TabIndex = 42;
            this.btn_SetParam.Text = "参数设置";
            this.btn_SetParam.UseVisualStyleBackColor = true;
            this.btn_SetParam.Click += new System.EventHandler(this.btn_SetParam_Click);
            // 
            // btn_SetOrigin
            // 
            this.btn_SetOrigin.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_SetOrigin.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_SetOrigin.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_SetOrigin.Location = new System.Drawing.Point(240, 6);
            this.btn_SetOrigin.MouseDownImage = null;
            this.btn_SetOrigin.Name = "btn_SetOrigin";
            this.btn_SetOrigin.Size = new System.Drawing.Size(90, 45);
            this.btn_SetOrigin.TabIndex = 41;
            this.btn_SetOrigin.Text = "定位点设置";
            this.btn_SetOrigin.UseVisualStyleBackColor = true;
            this.btn_SetOrigin.Click += new System.EventHandler(this.btn_SetOrigin_Click);
            // 
            // btn_Add
            // 
            this.btn_Add.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Add.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Add.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Add.Location = new System.Drawing.Point(54, 6);
            this.btn_Add.MouseDownImage = null;
            this.btn_Add.Name = "btn_Add";
            this.btn_Add.Size = new System.Drawing.Size(98, 45);
            this.btn_Add.TabIndex = 39;
            this.btn_Add.Text = "选择文件";
            this.btn_Add.UseVisualStyleBackColor = true;
            this.btn_Add.Click += new System.EventHandler(this.btn_Add_Click);
            // 
            // btn_Save
            // 
            this.btn_Save.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Save.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Save.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Save.Location = new System.Drawing.Point(515, 6);
            this.btn_Save.MouseDownImage = null;
            this.btn_Save.Name = "btn_Save";
            this.btn_Save.Size = new System.Drawing.Size(98, 45);
            this.btn_Save.TabIndex = 38;
            this.btn_Save.Text = "保 存";
            this.btn_Save.UseVisualStyleBackColor = true;
            this.btn_Save.Click += new System.EventHandler(this.btn_Save_Click);
            // 
            // paintingPanel1
            // 
            this.paintingPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.paintingPanel1.FilePath = "";
            this.paintingPanel1.Location = new System.Drawing.Point(0, 0);
            this.paintingPanel1.Name = "paintingPanel1";
            this.paintingPanel1.Size = new System.Drawing.Size(760, 400);
            this.paintingPanel1.TabIndex = 8;
            // 
            // label1
            // 
            this.label1.BackColor = System.Drawing.Color.Transparent;
            this.label1.Dock = System.Windows.Forms.DockStyle.Top;
            this.label1.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label1.Location = new System.Drawing.Point(0, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(760, 22);
            this.label1.TabIndex = 9;
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // frm_AddFile
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(760, 460);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.paintingPanel1);
            this.Controls.Add(this.panel1);
            this.Name = "frm_AddFile";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "frm_AddFile";
            this.VisibleChanged += new System.EventHandler(this.frm_AddFile_VisibleChanged);
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private Rich_Button btn_Add;
        private Rich_Button btn_Save;
        private PaintingPanel paintingPanel1;
        private Rich_Button btn_ExportParam;
        private Rich_Button btn_SetParam;
        private Rich_Button btn_SetOrigin;
        private Rich_Button btn_Quit;
        private System.Windows.Forms.Label label1;
    }
}