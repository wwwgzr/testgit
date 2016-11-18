namespace LasercuttingmachineUI
{
    partial class frm_Project
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frm_Project));
            this.pic_ProductView = new System.Windows.Forms.PictureBox();
            this.btn_Save = new LasercuttingmachineUI.Rich_Button();
            this.btn_Quit = new LasercuttingmachineUI.Rich_Button();
            this.btn_SelectFile = new LasercuttingmachineUI.Rich_Button();
            this.pic_ProjectView = new System.Windows.Forms.PictureBox();
            this.txt_ProductName = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.txt_ProjectName = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.pic_ProductView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pic_ProjectView)).BeginInit();
            this.SuspendLayout();
            // 
            // pic_ProductView
            // 
            this.pic_ProductView.Image = ((System.Drawing.Image)(resources.GetObject("pic_ProductView.Image")));
            this.pic_ProductView.Location = new System.Drawing.Point(223, 83);
            this.pic_ProductView.Name = "pic_ProductView";
            this.pic_ProductView.Size = new System.Drawing.Size(64, 64);
            this.pic_ProductView.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pic_ProductView.TabIndex = 38;
            this.pic_ProductView.TabStop = false;
            this.pic_ProductView.Click += new System.EventHandler(this.pic_ProductView_Click);
            // 
            // btn_Save
            // 
            this.btn_Save.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Save.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Save.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Save.Location = new System.Drawing.Point(83, 218);
            this.btn_Save.MouseDownImage = null;
            this.btn_Save.Name = "btn_Save";
            this.btn_Save.Size = new System.Drawing.Size(98, 45);
            this.btn_Save.TabIndex = 37;
            this.btn_Save.Text = "保 存";
            this.btn_Save.UseVisualStyleBackColor = true;
            this.btn_Save.Click += new System.EventHandler(this.btn_Save_Click);
            // 
            // btn_Quit
            // 
            this.btn_Quit.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Quit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Quit.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Quit.Location = new System.Drawing.Point(187, 218);
            this.btn_Quit.MouseDownImage = null;
            this.btn_Quit.Name = "btn_Quit";
            this.btn_Quit.Size = new System.Drawing.Size(98, 45);
            this.btn_Quit.TabIndex = 36;
            this.btn_Quit.Text = "退 出";
            this.btn_Quit.UseVisualStyleBackColor = true;
            this.btn_Quit.Click += new System.EventHandler(this.btn_Quit_Click);
            // 
            // btn_SelectFile
            // 
            this.btn_SelectFile.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_SelectFile.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_SelectFile.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_SelectFile.Location = new System.Drawing.Point(17, 153);
            this.btn_SelectFile.MouseDownImage = null;
            this.btn_SelectFile.Name = "btn_SelectFile";
            this.btn_SelectFile.Size = new System.Drawing.Size(270, 48);
            this.btn_SelectFile.TabIndex = 33;
            this.btn_SelectFile.Text = "添加切割文件";
            this.btn_SelectFile.UseVisualStyleBackColor = true;
            this.btn_SelectFile.Click += new System.EventHandler(this.btn_SelectFile_Click);
            // 
            // pic_ProjectView
            // 
            this.pic_ProjectView.Image = ((System.Drawing.Image)(resources.GetObject("pic_ProjectView.Image")));
            this.pic_ProjectView.Location = new System.Drawing.Point(223, 11);
            this.pic_ProjectView.Name = "pic_ProjectView";
            this.pic_ProjectView.Size = new System.Drawing.Size(64, 64);
            this.pic_ProjectView.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pic_ProjectView.TabIndex = 7;
            this.pic_ProjectView.TabStop = false;
            this.pic_ProjectView.Click += new System.EventHandler(this.pic_ProjectView_Click);
            // 
            // txt_ProductName
            // 
            this.txt_ProductName.Font = new System.Drawing.Font("宋体", 20.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.txt_ProductName.Location = new System.Drawing.Point(19, 96);
            this.txt_ProductName.Name = "txt_ProductName";
            this.txt_ProductName.Size = new System.Drawing.Size(194, 38);
            this.txt_ProductName.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(17, 81);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 12);
            this.label2.TabIndex = 3;
            this.label2.Text = "产品名称";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(17, 6);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "项目名称";
            // 
            // txt_ProjectName
            // 
            this.txt_ProjectName.Font = new System.Drawing.Font("宋体", 20.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.txt_ProjectName.Location = new System.Drawing.Point(17, 24);
            this.txt_ProjectName.Name = "txt_ProjectName";
            this.txt_ProjectName.Size = new System.Drawing.Size(194, 38);
            this.txt_ProjectName.TabIndex = 0;
            // 
            // frm_Project
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(299, 275);
            this.Controls.Add(this.pic_ProductView);
            this.Controls.Add(this.btn_Save);
            this.Controls.Add(this.btn_Quit);
            this.Controls.Add(this.btn_SelectFile);
            this.Controls.Add(this.pic_ProjectView);
            this.Controls.Add(this.txt_ProductName);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txt_ProjectName);
            this.Name = "frm_Project";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "frm_Project";
            this.Load += new System.EventHandler(this.frm_Project_Load);
            this.VisibleChanged += new System.EventHandler(this.frm_Project_VisibleChanged);
            ((System.ComponentModel.ISupportInitialize)(this.pic_ProductView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pic_ProjectView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txt_ProjectName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txt_ProductName;
        private System.Windows.Forms.PictureBox pic_ProjectView;
        private Rich_Button btn_SelectFile;
        private Rich_Button btn_Save;
        private Rich_Button btn_Quit;
        private System.Windows.Forms.PictureBox pic_ProductView;
    }
}