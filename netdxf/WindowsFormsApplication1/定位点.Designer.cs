namespace WindowsFormsApplication1
{
    partial class 定位点
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
            this.txt_X = new System.Windows.Forms.TextBox();
            this.txt_Y = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.ipAddressBox1 = new WindowsFormsApplication1.IPAddressBox();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.AutoScroll = true;
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.ipAddressBox1);
            this.panel1.Location = new System.Drawing.Point(14, 61);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(502, 373);
            this.panel1.TabIndex = 0;
            this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
            this.panel1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.定位点_MouseMove);
            // 
            // txt_X
            // 
            this.txt_X.Location = new System.Drawing.Point(29, 16);
            this.txt_X.Name = "txt_X";
            this.txt_X.Size = new System.Drawing.Size(100, 21);
            this.txt_X.TabIndex = 1;
            // 
            // txt_Y
            // 
            this.txt_Y.Location = new System.Drawing.Point(166, 16);
            this.txt_Y.Name = "txt_Y";
            this.txt_Y.Size = new System.Drawing.Size(100, 21);
            this.txt_Y.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 19);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(11, 12);
            this.label1.TabIndex = 3;
            this.label1.Text = "X";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(149, 19);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(11, 12);
            this.label2.TabIndex = 4;
            this.label2.Text = "Y";
            // 
            // ipAddressBox1
            // 
            this.ipAddressBox1.Location = new System.Drawing.Point(88, 23);
            this.ipAddressBox1.Name = "ipAddressBox1";
            this.ipAddressBox1.Size = new System.Drawing.Size(355, 156);
            this.ipAddressBox1.TabIndex = 5;
            // 
            // 定位点
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(540, 446);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txt_Y);
            this.Controls.Add(this.txt_X);
            this.Controls.Add(this.panel1);
            this.Name = "定位点";
            this.Text = "定位点";
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TextBox txt_X;
        private System.Windows.Forms.TextBox txt_Y;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private IPAddressBox ipAddressBox1;

    }
}