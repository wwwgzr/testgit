namespace LasercuttingmachineUI
{
    partial class frm_NewButton
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frm_NewButton));
            this.pic_ProjectView = new System.Windows.Forms.PictureBox();
            this.label1 = new System.Windows.Forms.Label();
            this.txt_ProjectName = new System.Windows.Forms.TextBox();
            this.btn_Save = new LasercuttingmachineUI.Rich_Button();
            this.btn_Quit = new LasercuttingmachineUI.Rich_Button();
            ((System.ComponentModel.ISupportInitialize)(this.pic_ProjectView)).BeginInit();
            this.SuspendLayout();
            // 
            // pic_ProjectView
            // 
            this.pic_ProjectView.Image = ((System.Drawing.Image)(resources.GetObject("pic_ProjectView.Image")));
            this.pic_ProjectView.Location = new System.Drawing.Point(213, 13);
            this.pic_ProjectView.Name = "pic_ProjectView";
            this.pic_ProjectView.Size = new System.Drawing.Size(64, 64);
            this.pic_ProjectView.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pic_ProjectView.TabIndex = 10;
            this.pic_ProjectView.TabStop = false;
            this.pic_ProjectView.Click += new System.EventHandler(this.pic_ProjectView_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 12);
            this.label1.TabIndex = 9;
            this.label1.Text = "名称";
            // 
            // txt_ProjectName
            // 
            this.txt_ProjectName.Font = new System.Drawing.Font("宋体", 20.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.txt_ProjectName.Location = new System.Drawing.Point(7, 26);
            this.txt_ProjectName.Name = "txt_ProjectName";
            this.txt_ProjectName.Size = new System.Drawing.Size(194, 38);
            this.txt_ProjectName.TabIndex = 8;
            this.txt_ProjectName.MouseClick += new System.Windows.Forms.MouseEventHandler(this.txt_ProjectName_MouseClick);
            // 
            // btn_Save
            // 
            this.btn_Save.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Save.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Save.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Save.Location = new System.Drawing.Point(19, 94);
            this.btn_Save.MouseDownImage = null;
            this.btn_Save.Name = "btn_Save";
            this.btn_Save.Size = new System.Drawing.Size(98, 45);
            this.btn_Save.TabIndex = 39;
            this.btn_Save.Text = "保 存";
            this.btn_Save.UseVisualStyleBackColor = true;
            this.btn_Save.Click += new System.EventHandler(this.btn_Save_Click);
            // 
            // btn_Quit
            // 
            this.btn_Quit.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Quit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Quit.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Quit.Location = new System.Drawing.Point(169, 94);
            this.btn_Quit.MouseDownImage = null;
            this.btn_Quit.Name = "btn_Quit";
            this.btn_Quit.Size = new System.Drawing.Size(98, 45);
            this.btn_Quit.TabIndex = 38;
            this.btn_Quit.Text = "退 出";
            this.btn_Quit.UseVisualStyleBackColor = true;
            this.btn_Quit.Click += new System.EventHandler(this.btn_Quit_Click);
            // 
            // frm_NewButton
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 149);
            this.Controls.Add(this.btn_Save);
            this.Controls.Add(this.btn_Quit);
            this.Controls.Add(this.pic_ProjectView);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txt_ProjectName);
            this.Name = "frm_NewButton";
            this.Text = "frm_NewButton";
            this.VisibleChanged += new System.EventHandler(this.frm_NewButton_VisibleChanged);
            ((System.ComponentModel.ISupportInitialize)(this.pic_ProjectView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pic_ProjectView;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txt_ProjectName;
        private Rich_Button btn_Save;
        private Rich_Button btn_Quit;
    }
}