namespace LasercuttingmachineUI
{
    partial class ButtonText
    {
        /// <summary> 
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region 组件设计器生成的代码

        /// <summary> 
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.btn_Icon = new System.Windows.Forms.Button();
            this.lbl_Text = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btn_Icon
            // 
            this.btn_Icon.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.btn_Icon.Dock = System.Windows.Forms.DockStyle.Top;
            this.btn_Icon.Location = new System.Drawing.Point(0, 0);
            this.btn_Icon.Margin = new System.Windows.Forms.Padding(0);
            this.btn_Icon.Name = "btn_Icon";
            this.btn_Icon.Size = new System.Drawing.Size(160, 160);
            this.btn_Icon.TabIndex = 0;
            this.btn_Icon.UseVisualStyleBackColor = true;
            this.btn_Icon.Click += new System.EventHandler(this.btn_Icon_Click);
            // 
            // lbl_Text
            // 
            this.lbl_Text.BackColor = System.Drawing.Color.Transparent;
            this.lbl_Text.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.lbl_Text.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.lbl_Text.Location = new System.Drawing.Point(0, 158);
            this.lbl_Text.Margin = new System.Windows.Forms.Padding(0);
            this.lbl_Text.Name = "lbl_Text";
            this.lbl_Text.Size = new System.Drawing.Size(160, 20);
            this.lbl_Text.TabIndex = 1;
            this.lbl_Text.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            this.lbl_Text.Click += new System.EventHandler(this.lbl_Text_Click);
            // 
            // ButtonText
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btn_Icon);
            this.Controls.Add(this.lbl_Text);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "ButtonText";
            this.Size = new System.Drawing.Size(160, 180);
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.Button btn_Icon;
        public System.Windows.Forms.Label lbl_Text;
    }
}
