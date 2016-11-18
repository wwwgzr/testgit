namespace LasercuttingmachineUI
{
    partial class frm_EditCutPath
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
            this.trv_GraphicShape = new System.Windows.Forms.TreeView();
            this.btn_Exit = new LasercuttingmachineUI.Rich_Button();
            this.btn_Save = new LasercuttingmachineUI.Rich_Button();
            this.ptb_Paint = new System.Windows.Forms.PictureBox();
            this.chb_Reverse = new System.Windows.Forms.CheckBox();
            this.rab_PickKeyPoint = new System.Windows.Forms.RadioButton();
            this.rab_PickRandom = new System.Windows.Forms.RadioButton();
            this.chb_PickPoint = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.ptb_Paint)).BeginInit();
            this.SuspendLayout();
            // 
            // trv_GraphicShape
            // 
            this.trv_GraphicShape.Font = new System.Drawing.Font("宋体", 21F);
            this.trv_GraphicShape.FullRowSelect = true;
            this.trv_GraphicShape.Location = new System.Drawing.Point(4, 3);
            this.trv_GraphicShape.Name = "trv_GraphicShape";
            this.trv_GraphicShape.Size = new System.Drawing.Size(199, 429);
            this.trv_GraphicShape.TabIndex = 1;
            this.trv_GraphicShape.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.trv_GraphicShape_NodeMouseClick);
            // 
            // btn_Exit
            // 
            this.btn_Exit.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Exit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Exit.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Exit.Location = new System.Drawing.Point(631, 444);
            this.btn_Exit.Name = "btn_Exit";
            this.btn_Exit.Size = new System.Drawing.Size(98, 45);
            this.btn_Exit.TabIndex = 22;
            this.btn_Exit.Text = "退 出";
            this.btn_Exit.UseVisualStyleBackColor = true;
            this.btn_Exit.Click += new System.EventHandler(this.btn_Exit_Click);
            // 
            // btn_Save
            // 
            this.btn_Save.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Save.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btn_Save.Font = new System.Drawing.Font("宋体", 10F);
            this.btn_Save.Location = new System.Drawing.Point(517, 443);
            this.btn_Save.Name = "btn_Save";
            this.btn_Save.Size = new System.Drawing.Size(98, 45);
            this.btn_Save.TabIndex = 21;
            this.btn_Save.Text = "保 存";
            this.btn_Save.UseVisualStyleBackColor = true;
            this.btn_Save.Click += new System.EventHandler(this.btn_Save_Click);
            // 
            // ptb_Paint
            // 
            this.ptb_Paint.BackColor = System.Drawing.Color.Black;
            this.ptb_Paint.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.ptb_Paint.Location = new System.Drawing.Point(202, 3);
            this.ptb_Paint.Name = "ptb_Paint";
            this.ptb_Paint.Size = new System.Drawing.Size(542, 429);
            this.ptb_Paint.TabIndex = 23;
            this.ptb_Paint.TabStop = false;
            this.ptb_Paint.Paint += new System.Windows.Forms.PaintEventHandler(this.ptb_Paint_Paint);
            this.ptb_Paint.MouseDown += new System.Windows.Forms.MouseEventHandler(this.ptb_Paint_MouseDown);
            this.ptb_Paint.MouseMove += new System.Windows.Forms.MouseEventHandler(this.ptb_Paint_MouseMove);
            this.ptb_Paint.MouseUp += new System.Windows.Forms.MouseEventHandler(this.ptb_Paint_MouseUp);
            // 
            // chb_Reverse
            // 
            this.chb_Reverse.AutoSize = true;
            this.chb_Reverse.Font = new System.Drawing.Font("宋体", 22F);
            this.chb_Reverse.Location = new System.Drawing.Point(407, 454);
            this.chb_Reverse.Name = "chb_Reverse";
            this.chb_Reverse.Size = new System.Drawing.Size(92, 34);
            this.chb_Reverse.TabIndex = 24;
            this.chb_Reverse.Text = "反向";
            this.chb_Reverse.UseVisualStyleBackColor = true;
            this.chb_Reverse.CheckedChanged += new System.EventHandler(this.chb_Reverse_CheckedChanged);
            // 
            // rab_PickKeyPoint
            // 
            this.rab_PickKeyPoint.AutoSize = true;
            this.rab_PickKeyPoint.Checked = true;
            this.rab_PickKeyPoint.Font = new System.Drawing.Font("宋体", 22F);
            this.rab_PickKeyPoint.Location = new System.Drawing.Point(22, 453);
            this.rab_PickKeyPoint.Name = "rab_PickKeyPoint";
            this.rab_PickKeyPoint.Size = new System.Drawing.Size(121, 34);
            this.rab_PickKeyPoint.TabIndex = 25;
            this.rab_PickKeyPoint.TabStop = true;
            this.rab_PickKeyPoint.Text = "取端点";
            this.rab_PickKeyPoint.UseVisualStyleBackColor = true;
            this.rab_PickKeyPoint.Visible = false;
            // 
            // rab_PickRandom
            // 
            this.rab_PickRandom.AutoSize = true;
            this.rab_PickRandom.Font = new System.Drawing.Font("宋体", 22F);
            this.rab_PickRandom.Location = new System.Drawing.Point(149, 453);
            this.rab_PickRandom.Name = "rab_PickRandom";
            this.rab_PickRandom.Size = new System.Drawing.Size(151, 34);
            this.rab_PickRandom.TabIndex = 26;
            this.rab_PickRandom.Text = "取任意点";
            this.rab_PickRandom.UseVisualStyleBackColor = true;
            this.rab_PickRandom.Visible = false;
            // 
            // chb_PickPoint
            // 
            this.chb_PickPoint.AutoSize = true;
            this.chb_PickPoint.Font = new System.Drawing.Font("宋体", 22F);
            this.chb_PickPoint.Location = new System.Drawing.Point(304, 453);
            this.chb_PickPoint.Name = "chb_PickPoint";
            this.chb_PickPoint.Size = new System.Drawing.Size(92, 34);
            this.chb_PickPoint.TabIndex = 27;
            this.chb_PickPoint.Text = "取点";
            this.chb_PickPoint.UseVisualStyleBackColor = true;
            this.chb_PickPoint.CheckedChanged += new System.EventHandler(this.chb_PickPoint_CheckedChanged);
            // 
            // frm_EditCutPath
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(748, 501);
            this.Controls.Add(this.chb_PickPoint);
            this.Controls.Add(this.rab_PickRandom);
            this.Controls.Add(this.rab_PickKeyPoint);
            this.Controls.Add(this.chb_Reverse);
            this.Controls.Add(this.trv_GraphicShape);
            this.Controls.Add(this.ptb_Paint);
            this.Controls.Add(this.btn_Exit);
            this.Controls.Add(this.btn_Save);
            this.Name = "frm_EditCutPath";
            this.Text = "frm_EditCutPath";
            this.VisibleChanged += new System.EventHandler(this.frm_EditCutPath_VisibleChanged);
            ((System.ComponentModel.ISupportInitialize)(this.ptb_Paint)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TreeView trv_GraphicShape;
        private Rich_Button btn_Exit;
        private Rich_Button btn_Save;
        private System.Windows.Forms.PictureBox ptb_Paint;
        private System.Windows.Forms.CheckBox chb_Reverse;
        private System.Windows.Forms.RadioButton rab_PickKeyPoint;
        private System.Windows.Forms.RadioButton rab_PickRandom;
        private System.Windows.Forms.CheckBox chb_PickPoint;


    }
}