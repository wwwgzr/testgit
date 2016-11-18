namespace LasercuttingmachineUI
{
    partial class frm_OpenProject
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
            this.userButtonPanel1 = new LasercuttingmachineUI.UserButtonPanel();
            this.SuspendLayout();
            // 
            // userButtonPanel1
            // 
            this.userButtonPanel1.AutoScroll = true;
            this.userButtonPanel1.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.userButtonPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.userButtonPanel1.Location = new System.Drawing.Point(0, 0);
            this.userButtonPanel1.Name = "userButtonPanel1";
            this.userButtonPanel1.Size = new System.Drawing.Size(670, 430);
            this.userButtonPanel1.TabIndex = 0;
            // 
            // frm_OpenProject
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(670, 430);
            this.Controls.Add(this.userButtonPanel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "frm_OpenProject";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "frm_OpenProject";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.ResumeLayout(false);

        }

        #endregion

        public UserButtonPanel userButtonPanel1;
    }
}