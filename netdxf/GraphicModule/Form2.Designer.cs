namespace GraphicModule
{
    partial class Form2
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
            this.paintingPanel1 = new GraphicModule.PaintingPanel();
            this.SuspendLayout();
            // 
            // paintingPanel1
            // 
            this.paintingPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.paintingPanel1.Location = new System.Drawing.Point(0, 0);
            this.paintingPanel1.Name = "paintingPanel1";
            this.paintingPanel1.Size = new System.Drawing.Size(603, 411);
            this.paintingPanel1.TabIndex = 0;
            // 
            // Form2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(603, 411);
            this.Controls.Add(this.paintingPanel1);
            this.Name = "Form2";
            this.Text = "Form2";
            this.ResumeLayout(false);

        }

        #endregion

        private PaintingPanel paintingPanel1;
    }
}