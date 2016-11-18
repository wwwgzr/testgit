using CommonModule;
using CommunicationModule;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public class MenuController
    {
        public static Form1 owner;
        //public frm_MenuPanel mainMenu;
        public MenuPanel workMenu;
        public MenuPanel materialMenu;
        public MenuPanel feedMenu;
        public MenuPanel testMenu;
        public MenuPanel fileMenu;
        public MenuPanel projectMenu;
        public List<MenuButton> mianMenuButtons = new List<MenuButton>();
        public List<MenuButton> materialMenuButtons = new List<MenuButton>();
        public List<MenuButton> feedMenuButtons = new List<MenuButton>();
        public List<MenuButton> testMenuButtons = new List<MenuButton>();
        public List<MenuButton> fileMenuButtons = new List<MenuButton>();
        public List<MenuButton> projectMenuButtons = new List<MenuButton>();
        public List<MenuButton> workMenuButtons = new List<MenuButton>();
        /// <summary>
        /// 是否关闭
        /// </summary>
        public bool isClose = false;
        /// <summary>
        /// 是否正在自动送料测试
        /// </summary>
        public bool isAutoFeed = false;
        /// <summary>
        /// 是否按下终止工作按钮
        /// </summary>
        public bool isMainStop = false;

        /// <summary>
        /// 上料部分参数设置
        /// </summary>
        public frm_SettingParam ps;
        /// <summary>
        /// 页面监控
        /// </summary>
        PageMonitor pm;
        /// <summary>
        /// 手动功能
        /// </summary>
        public frm_SetOrigin mf;

        frm_EditCutPath editCutPath;
        //frm_SelectFile selectFile;
        frm_SysSet sysSet;
        frm_SetLaser setLaser;
        public frm_EditUser editUser;
        frm_SetManualFeed setManualFeed;
        //frm_Project project;
        public frm_OpenProject openProject;
        public MenuButton btn_openProject;

        public MenuController(Form Owner)
        {
            owner = (Form1)Owner;
            //CreateMenuPanel();
            CreateMaterialMenuPanel();
            //ShowPanel(false);
            foreach (MenuButton item in materialMenuButtons)
            {
                item.Click += new EventHandler(ButtonClick);
            }
            CreateFeedMenuPanel();

            foreach (MenuButton item in feedMenuButtons)
            {
                item.Click += new EventHandler(ButtonClick);
            }
            CreateTestMenuPanel();
            foreach (MenuButton item in testMenuButtons)
            {
                item.Click += new EventHandler(ButtonClick);
            }
            CreateFileMenuPanel();
            foreach (MenuButton item in fileMenuButtons)
            {
                item.Click += new EventHandler(ButtonClick);
            }
            CreateWorkMenuPanel();
            foreach (MenuButton item in workMenuButtons)
            {
                item.Click += new EventHandler(ButtonClick);
            }
            CreateProjectMenuPanel();
            foreach (MenuButton item in projectMenuButtons)
            {
                item.Click += new EventHandler(ButtonClick);
            }
            owner.SplitPanel.Controls.Add(workMenu);
            owner.SplitPanel.Controls.Add(feedMenu);
            owner.SplitPanel.Controls.Add(testMenu);
            owner.SplitPanel.Controls.Add(fileMenu);
            owner.SplitPanel.Controls.Add(projectMenu);
            owner.SplitPanel.Controls.Add(materialMenu);
            materialMenu.Visible = true;
            ResetButtonLocation();
            owner.FormClosed += owner_FormClosed;
        }

        void owner_FormClosed(object sender, FormClosedEventArgs e)
        {
            mianMenuButtons.Clear();
            materialMenuButtons.Clear();
            foreach (Form item in owner.OwnedForms)
            {
                item.Close();
                item.Dispose();
            }

            ParseCommunicationCommand.CloseDevice(0);
            ConvertType.DeleteLogFile();
        }

        //public void CreateMenuPanel()
        //{
        //    mainMenu = new frm_MenuPanel(1);
        //    mainMenu.Owner = owner;
        //    mainMenu.TopMost = true;
        //    mianMenuButtons.Add(mainMenu.AddButton("btn_UDisk", "U 盘\r\n\r\n文 件", "U盘文件"));
        //    mianMenuButtons.Add(mainMenu.AddButton("btn_SysFile", "内 存\r\n\r\n文 件", "内存文件"));
        //    mianMenuButtons.Add(mainMenu.AddButton("btn_SysInfo", "系 统\r\n\r\n信 息", "系统信息"));
        //    mianMenuButtons.Add(mainMenu.AddButton("btn_SetFile", "文 件\r\n\r\n设 置", "文件设置"));
        //    mianMenuButtons.Add(mainMenu.AddButton("btn_Setting", "综 合\r\n\r\n设 置", "综合设置"));
        //    mianMenuButtons.Add(mainMenu.AddButton("btn_SysTest", "系 统\r\n\r\n测 试", "系统测试"));
        //}
        /// <summary>
        /// 操作面板
        /// </summary>
        public static frm_OperationPanel operatPanel;
        public static void ShowPanel(bool visible, Rich_TextBox txt,IpInputTextbox ip)
        {
            if (operatPanel == null || operatPanel.IsDisposed)
            {
                operatPanel = new frm_OperationPanel();
                operatPanel.Owner = owner;
            }
            if (visible)
            {
                if (txt != null)
                {
                    operatPanel.FocusText = txt;
                    operatPanel.FocusForm = txt.FindForm();
                    if (!operatPanel.Visible)
                    {
                        Point local = new Point(operatPanel.FocusForm.Right, operatPanel.FocusForm.Location.Y + txt.Location.Y);
                        if (local.X + operatPanel.Width > Screen.PrimaryScreen.Bounds.Width)
                        {
                            local.X = operatPanel.FocusForm.Right - txt.Parent.Width - operatPanel.Width;
                        }
                        
                        operatPanel.Location = local;
                    }
                }
                if(ip != null)
                {
                    operatPanel.FocusIpText = ip;
                    operatPanel.FocusForm = ip.FindForm();
                    if (!operatPanel.Visible)
                    {
                        Point local = new Point(operatPanel.FocusForm.Right, operatPanel.FocusForm.Location.Y + ip.Location.Y);
                        operatPanel.Location = local;
                    }
                }
            }
            operatPanel.TopMost = true;
            operatPanel.Visible = visible;
        }
        public void CreateMaterialMenuPanel()
        {
            materialMenu = new MenuPanel(1);
            //materialMenu.Owner = owner;
            //materialMenu.TopMost = true;
            //Parameter setting 参数设置
            //SelectCutterFile 切割文件选择
            //手动功能
            //页面监控
            //materialMenuButtons.Add(materialMenu.AddButton1("btn_SetParam", "参 数\r\n\r\n设 置", "参数设置"));
            //materialMenuButtons.Add(materialMenu.AddButton1("btn_SelectFile", "切割文\r\n\r\n件选择", "切割文件选择"));
            //materialMenuButtons.Add(materialMenu.AddButton1("btn_Manualfunction", "手 动\r\n\r\n功 能", "手动功能"));
            //materialMenuButtons.Add(materialMenu.AddButton1("btn_PageMonitor", "页 面\r\n\r\n监 控", "页面监控"));
            //materialMenuButtons.Add(materialMenu.AddButton1("btn_Stop", "终 止\r\n\r\n工 作", "终止工作"));

            btn_openProject = materialMenu.AddButton1("btn_CutProject", "切 割\r\n\r\n项 目", "切割项目");
            materialMenuButtons.Add(btn_openProject);
            btn_OpenProject(btn_openProject);

            materialMenuButtons.Add(materialMenu.AddButton1("btn_SetParam", "参 数\r\n\r\n设 置", "参数设置"));
            materialMenuButtons.Add(materialMenu.AddButton1("btn_SelectFile", "切 割\r\n\r\n文 件", "切割文件"));
            materialMenuButtons.Add(materialMenu.AddButton1("btn_SetOrigin", "定位点\r\n\r\n设  置", "定位点设置"));
            materialMenuButtons.Add(materialMenu.AddButton1("btn_MoveFrame", "移 框", "移框"));
            materialMenuButtons.Add(materialMenu.AddButton1("btn_SysTest", "系 统\r\n\r\n测 试", "系统测试"));
            materialMenuButtons.Add(materialMenu.AddButton1("btn_SysSet", "系 统\r\n\r\n设 置", "系统设置"));

            materialMenuButtons.Add(materialMenu.AddButton1("btn_EditUser", "密 码\r\n\r\n修 改", "密码修改"));

            materialMenuButtons.Add(materialMenu.AddButton1("btn_FeedMode", "上 料\r\n\r\n调 试", "上料调试"));

            materialMenuButtons.Add(materialMenu.AddButton1("btn_WorkMenu", "工 作\r\n\r\n菜 单", "工作菜单"));
            
            //materialMenuButtons.Add(materialMenu.AddButton1("btn_ClearProduct", "清 除\r\n\r\n产 量", "清除产量"));
            ////materialMenuButtons.Add(materialMenu.AddButton1("btn_Start", "开 始\r\n\r\n工 作", "开始工作"));
            //materialMenuButtons.Add(materialMenu.AddButton1("btn_Pause", "暂 停\r\n\r\n工 作", "暂停工作"));
            //materialMenuButtons.Add(materialMenu.AddButton1("btn_Stop", "终 止\r\n\r\n工 作", "终止工作"));
            //materialMenuButtons.Add(materialMenu.AddButton1("btn_Clear", "清 洗\r\n\r\n功 能", "清洗功能"));

            materialMenuButtons.Add(materialMenu.AddButton1("btn_Exit", "退 出\r\n\r\n系 统", "退出系统"));
            //materialMenuButtons.Add(materialMenu.AddButton1("btn_ChangeUser", "切 换\r\n\r\n用 户", "切换用户"));
        }

        public void ResetButtonLocation()
        {
            int j = 0;
            bool visble = true;
            if (ParseCommunicationCommand.userType == 2)
            {
                visble = false;
            }
            for (int i = 0; i < materialMenuButtons.Count; i++)
            {
                if (visble)
                {
                    //btn_MoveFrame
                    if (materialMenuButtons[i].Name != "btn_MoveFrame")
                    {
                        materialMenuButtons[i].Visible = true;
                        materialMenu.Width = 8 + (64 + 8) * (j + 1);
                        materialMenuButtons[i].Location = new System.Drawing.Point(materialMenu.Width - 64 - 8, 4);
                        j++;
                    }
                    else
                    {
                        materialMenuButtons[i].Visible = false;
                    }
                    
                }
                else
                {
                    //materialMenuButtons[i].Name != "btn_SetParam" &&
                    if (materialMenuButtons[i].Name != "btn_SetOrigin" &&
                        materialMenuButtons[i].Name != "btn_SysSet" &&
                        materialMenuButtons[i].Name != "btn_EditUser" &&
                        materialMenuButtons[i].Name != "btn_SetParam")
                    {
                        materialMenuButtons[i].Visible = true;
                        materialMenu.Width = 8 + (64 + 8) * (j + 1);
                        materialMenuButtons[i].Location = new System.Drawing.Point(materialMenu.Width - 64 - 8, 4);
                        j++;
                        //this.Controls[i].BackColor = Color.DarkGray;
                    }
                    else
                    {
                        materialMenuButtons[i].Visible = false;
                    }
                }
            }

            j = 0;
            for (int i = 0; i < fileMenuButtons.Count; i++)
            {
                if (visble)
                {
                    //btn_MoveFrame
                    //if (fileMenuButtons[i].Name != "btn_MoveFrame")
                    {
                        fileMenuButtons[i].Visible = true;
                        fileMenu.Width = 8 + (64 + 8) * (j + 1);
                        fileMenuButtons[i].Location = new System.Drawing.Point(fileMenu.Width - 64 - 8, 4);
                        j++;
                    }
                }
                else
                {
                    //materialMenuButtons[i].Name != "btn_SetParam" &&
                    if (fileMenuButtons[i].Name != "btn_SelectImageFile" &&
                        fileMenuButtons[i].Name != "btn_EditFile" &&
                        fileMenuButtons[i].Name != "btn_TurnFile" &&
                        fileMenuButtons[i].Name != "btn_WriteFile")
                    {
                        fileMenuButtons[i].Visible = true;
                        fileMenu.Width = 8 + (64 + 8) * (j + 1);
                        fileMenuButtons[i].Location = new System.Drawing.Point(fileMenu.Width - 64 - 8, 4);
                        j++;
                        //this.Controls[i].BackColor = Color.DarkGray;
                    }
                    else
                    {
                        fileMenuButtons[i].Visible = false;
                    }
                }
            }
        }

        public void CreateFeedMenuPanel()
        {
            feedMenu = new MenuPanel(2);
            //feedMenu.Owner = owner;
            //feedMenu.TopMost = true;
            feedMenu.Tag = "Visble-1";
            feedMenuButtons.Add(feedMenu.AddButton1("btn_ManualFeedStart", "手动上\r\n\r\n料开始", "手动上料开始"));
            feedMenuButtons.Add(feedMenu.AddButton1("btn_ManualFeedFinal", "手动上\r\n\r\n料结束", "手动上料结束"));

            //btn_ManualInFeed,btn_ManualInFeedSet,btn_ManualOutFeed,btn_ManualCutFeed
            feedMenuButtons.Add(feedMenu.AddButton1("btn_ManualInFeed", "手 动\r\n\r\n进 料", "手动进料"));
            feedMenuButtons.Add(feedMenu.AddButton1("btn_ManualInFeedSet", "手动参数\r\n\r\n设置", "手动参数设置"));
            feedMenuButtons.Add(feedMenu.AddButton1("btn_ManualOutFeed", "手 动\r\n\r\n退 料", "手动退料"));
            feedMenuButtons.Add(feedMenu.AddButton1("btn_ManualCutFeed", "手 动\r\n\r\n切 断", "手动切断"));

            feedMenuButtons.Add(feedMenu.AddButton1("btn_ClothPressUp", "压布棍\r\n\r\n升", "压布棍升"));
            feedMenuButtons.Add(feedMenu.AddButton1("btn_ClothPressDown", "压布棍\r\n\r\n降", "压布棍降"));

            feedMenuButtons.Add(feedMenu.AddButton1("btn_AutoFeed", "自动送\r\n\r\n料测试", "自动送料测试"));
            feedMenuButtons.Add(feedMenu.AddButton1("btn_CancelAutoFeed", "取消自\r\n\r\n动进料", "取消自动进料"));
             //feedMenuButtons.Add(feedMenu.AddButton1("btn_ManualInFeedStart", "手动低速\r\n\r\n送料开始", "手动低速送料开始"));
            //feedMenuButtons.Add(feedMenu.AddButton1("btn_ManualInFeedFinal", "手动低速\r\n\r\n送料结束", "手动低速送料结束"));
            //feedMenuButtons.Add(feedMenu.AddButton1("btn_ManualOutFeedStart", "手动低速\r\n\r\n退料开始", "手动低速退料开始"));
            //feedMenuButtons.Add(feedMenu.AddButton1("btn_ManualOutFeedEnd", "手动低速\r\n\r\n退料结束", "手动低速退料结束"));
            //feedMenuButtons.Add(feedMenu.AddButton1("btn_CancelAutoInFeed", "取消自\r\n\r\n动送料", "取消自动送料"));
            //feedMenuButtons.Add(feedMenu.AddButton1("btn_ClothPressUp", "压布棍\r\n\r\n升", "压布棍升"));
            //feedMenuButtons.Add(feedMenu.AddButton1("btn_ClothPressDown", "压布棍\r\n\r\n降", "压布棍降"));
        }

        public void CreateTestMenuPanel()
        {
            testMenu = new MenuPanel(2);
            //testMenu.Owner = owner;
            //testMenu.TopMost = true;
            testMenu.Tag = "Visble-1";

            //testMenuButtons.Add(testMenu.AddButton1("btn_TopCloth-1", "顶布测试\r\n\r\n顶布", "顶布测试"));
            //testMenuButtons.Add(testMenu.AddButton1("btn_TopCloth-2", "顶布测试\r\n\r\n落下", "顶布测试"));

            testMenuButtons.Add(testMenu.AddButton1("btn_Gassing-1", "吹气测试\r\n\r\n吹气", "吹气测试"));
            testMenuButtons.Add(testMenu.AddButton1("btn_Gassing-2", "吹气测试\r\n\r\n关闭", "吹气测试"));

            //testMenuButtons.Add(testMenu.AddButton1("btn_AutoFeed", "自动送\r\n\r\n料测试", "自动送料测试"));
            testMenuButtons.Add(testMenu.AddButton1("btn_LaserPoint", "激光\r\n\r\n点射", "激光点射"));
            //testMenuButtons.Add(testMenu.AddButton1("btn_SetLaserPoint", "激光\r\n\r\n点射设置", "激光点射设置"));
            testMenuButtons.Add(testMenu.AddButton1("btn_IdlingBraking", "空走\r\n\r\n测试", "空走测试"));
            testMenuButtons.Add(testMenu.AddButton1("btn_ClipClothUp", "夹布测试\r\n\r\n升", "夹布测试"));
            testMenuButtons.Add(testMenu.AddButton1("btn_ClipClothDown", "夹布测试\r\n\r\n降", "夹布测试"));
            testMenuButtons.Add(testMenu.AddButton1("btn_ClothPressUp", "压布棍\r\n\r\n升", "压布棍升"));
            testMenuButtons.Add(testMenu.AddButton1("btn_ClothPressDown", "压布棍\r\n\r\n降", "压布棍降"));
        }

        public void CreateFileMenuPanel()
        {
            fileMenu = new MenuPanel(2);
            //fileMenu.Owner = owner;
            //fileMenu.TopMost = true;
            fileMenu.Tag = "Visble-1";

            fileMenuButtons.Add(fileMenu.AddButton1("btn_SelectImageFile", "选 择\r\n\r\n文 件", "选择文件"));
            fileMenuButtons.Add(fileMenu.AddButton1("btn_EditFile", "编 辑\r\n\r\n文 件", "编辑文件"));
            fileMenuButtons.Add(fileMenu.AddButton1("btn_TurnFile", "转 换\r\n\r\n文 件", "转换文件"));
            fileMenuButtons.Add(fileMenu.AddButton1("btn_WriteFile", "写 入\r\n\r\n文 件", "写入内存"));
            fileMenuButtons.Add(fileMenu.AddButton1("btn_PreviewFile", "切割预览\r\n\r\n开始", "切割预览"));
            fileMenuButtons.Add(fileMenu.AddButton1("btn_ZoomIn", "放 大", "放大"));
            fileMenuButtons.Add(fileMenu.AddButton1("btn_ZoomOut", "缩 小", "缩小"));
            fileMenuButtons.Add(fileMenu.AddButton1("btn_Pan", "平 移", "平移"));
            fileMenuButtons.Add(fileMenu.AddButton1("btn_Full", "全 图", "全图"));
            //fileMenuButtons.Add(fileMenu.AddButton1("btn_DeleteFile", "删 除\r\n\r\n文 件", "删除文件"));
        }

        public void CreateWorkMenuPanel()
        {
            workMenu = new MenuPanel(2);
            //workMenu.Owner = owner;
            //fileMenu.TopMost = true;
            workMenu.Tag = "Visble-1";

            //materialMenuButtons.Add(materialMenu.AddButton1("btn_Start", "开 始\r\n\r\n工 作", "开始工作"));
            workMenuButtons.Add(workMenu.AddButton1("btn_ClearProduct", "清 除\r\n\r\n产 量", "清除产量"));
            //if (ParseCommunicationCommand.isCloseLaserB)
            //{
            //    workMenuButtons.Add(workMenu.AddButton1("btn_CloseLaserB", "打开激光\r\n\r\n头 B", "打开激光头B"));
            //}
            //else
            //    workMenuButtons.Add(workMenu.AddButton1("btn_CloseLaserB", "关闭激光\r\n\r\n头 B", "关闭激光头B"));
            //workMenuButtons.Add(workMenu.AddButton1("btn_Pause", "暂 停\r\n\r\n工 作", "暂停工作"));
            workMenuButtons.Add(workMenu.AddButton1("btn_Stop", "终 止\r\n\r\n工 作", "终止工作"));
            workMenuButtons.Add(workMenu.AddButton1("btn_Clear", "清 洗\r\n\r\n功 能", "清洗功能"));
            workMenuButtons.Add(workMenu.AddButton1("btn_ChangeUser", "切 换\r\n\r\n用 户", "切换用户"));
            //workMenuButtons.Add(workMenu.AddButton1("btn_CancelAutoFeed", "取消自\r\n\r\n动进料", "取消自动进料"));
        }

        /// <summary>
        /// 创建项目菜单
        /// </summary>
        public void CreateProjectMenuPanel()
        {
            projectMenu = new MenuPanel(2);
            //projectMenu.Owner = owner;
            //fileMenu.TopMost = true;
            projectMenu.Tag = "Visble-1";

            //projectMenuButtons.Add(projectMenu.AddButton1("btn_NewProject", "新 建\r\n\r\n项 目", "新建项目"));
            projectMenuButtons.Add(projectMenu.AddButton1("btn_EditProject", "编 辑\r\n\r\n项 目", "编辑项目"));
            projectMenuButtons.Add(projectMenu.AddButton1("btn_OpenProject", "打 开\r\n\r\n项 目", "打开项目"));

            //projectMenuButtons.Add(projectMenu.AddButton1("btn_WriteFile", "写 入\r\n\r\n文 件", "写入内存"));
            //projectMenuButtons.Add(projectMenu.AddButton1("btn_PreviewFile", "切割预览\r\n\r\n开始", "切割预览"));
            //projectMenuButtons.Add(projectMenu.AddButton1("btn_ZoomIn", "放 大", "放大"));
            //projectMenuButtons.Add(projectMenu.AddButton1("btn_ZoomOut", "缩 小", "缩小"));
            //projectMenuButtons.Add(projectMenu.AddButton1("btn_Pan", "平 移", "平移"));
            //projectMenuButtons.Add(projectMenu.AddButton1("btn_Full", "全 图", "全图"));
            //fileMenuButtons.Add(fileMenu.AddButton1("btn_DeleteFile", "删 除\r\n\r\n文 件", "删除文件"));
        }

        public void ShowMenu()
        {
            if (owner != null)
            {
                materialMenu.Location = new System.Drawing.Point((owner.SplitPanel.Width - materialMenu.Width) / 2, owner.SplitPanel.Height - materialMenu.Height);
                materialMenu.Show();
            }
        }

        /// <summary>
        /// 按钮点击事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void ButtonClick(object sender, EventArgs e)
        {
            ShowPanel(false, null, null);
            if (ParseCommunicationCommand.IsReading)
                return;
            MenuButton mbtn = (MenuButton)sender;
            if (mbtn.Name == "btn_ZoomIn" || mbtn.Name == "btn_ZoomOut" ||
                mbtn.Name == "btn_Pan" || mbtn.Name == "btn_Full")
            { }
            else
            {
                foreach (Form item in owner.OwnedForms)
                {
                    if (item.Tag == null || item.Tag.ToString() != "Visble-1")
                        continue;
                    if (mbtn.Tag == item)
                        continue;
                    //if (item != mbtn.FindForm())
                    item.Hide();
                }

                foreach (Control item in owner.SplitPanel.Controls)
                {
                    if (item is MenuPanel)
                    {
                        if (item.Tag == null || item.Tag.ToString() != "Visble-1")
                            continue;
                        if (mbtn.Tag == item)
                            continue;
                        if (item != mbtn.Parent)
                            item.Hide();
                    }
                }
            }
            //终止工作时才能设置 20160818
            //if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
            //{
            //    foreach (Control item in this.Controls)
            //    {
            //        item.Enabled = true;
            //    }
            //}
            //else
            //{
            //    foreach (Control item in this.Controls)
            //    {
            //        if (item != btn_Exit)
            //            item.Enabled = false;
            //    }
            //}

            switch (mbtn.Name)
            {
                #region 主菜单
                case "btn_SetParam":
                    btn_SetParam(mbtn);
                    break;
                case "btn_SelectFile":
                    btn_SelectFile(mbtn);
                    break;
                case "btn_CutProject":
                    btn_CutProject(mbtn);
                    break;
                case "btn_SetOrigin":
                    btn_SetOrigin(mbtn);
                    break;
                case "btn_MoveFrame":
                    btn_SetOrigin(mbtn);
                    break;
                case "btn_SysTest":
                    btn_SysTest(mbtn);
                    break;
                case "btn_Start":
                    //开始工作
                    ParseCommunicationCommand.ExecuteCommand("开始工作");
                    break;
                case "btn_Pause":
                    //暂停工作
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Start)
                        ParseCommunicationCommand.ExecuteCommand("暂停工作");
                    break;
                case "btn_Stop":
                    //终止工作
                    ParseCommunicationCommand.ExecuteCommand("终止工作");
                    owner.StopWork();
                    break;
                case "btn_Exit":
                    owner.Close();
                    //退出系统
                    break;
                case "btn_FeedMode":
                    btn_FeedMode(mbtn);
                    break;
                case "btn_SysSet":
                    btn_SysSet(mbtn);
                    break;
                case "btn_ClearProduct":
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        btn_ClearProduct(mbtn);
                    break;
                case "btn_EditUser":
                    btn_EditUser(mbtn);
                    break;
                case "btn_WorkMenu":
                    btn_WorkMenu(mbtn);
                    break;
                case "btn_ChangeUser":
                    //切换用户
                    if (ParseCommunicationCommand.userType == 2)
                    {
                        if (owner.loadForm.ShowDialog() == DialogResult.OK)
                        {
                            ResetButtonLocation();
                            ShowMenu();
                            workMenu.Visible = false;
                            owner.userSetParam1.Enabled = true;
                        }
                    }
                    else
                    {
                        ParseCommunicationCommand.userType = 2;
                        owner.userSetParam1.Enabled = false;
                        ResetButtonLocation();
                        ShowMenu();
                        workMenu.Visible = false;
                    }
                    break;
                #endregion
                #region 上料调试
                case "btn_ManualFeedStart":
                    //手动上料开始
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                    {
                        ParseCommunicationCommand.ExecuteManualFeedParam();
                        ParseCommunicationCommand.ExecuteCommand("手动上料开始");
                    }
                    break;
                case "btn_ManualFeedFinal":
                    //手动上料结束
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        btn_ManualFeedEnd(mbtn);
                    //ParseCommunicationCommand.ExecuteCommand("手动上料结束");
                    break;
                case "btn_CancelAutoFeed":
                    //取消自动进料
                    if (isAutoFeed && ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Feed)
                        ParseCommunicationCommand.ExecuteCommand("取消自动进料");
                    break;
                case "btn_ManualInFeedStart":
                    //手动低速送料开始
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("手动低速送料-开始");
                    break;
                case "btn_ManualInFeedFinal":
                    //手动低速送料结束
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("手动低速送料-结束");
                    break;
                case "btn_ManualOutFeedStart":
                    //手动低速退料开始
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("手动低速退料-开始");
                    break;
                case "btn_ManualOutFeedEnd":
                    //手动低速退料结束
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("手动低速退料-结束");
                    break;
                case "btn_CancelAutoInFeed":
                    //取消自动送料
                    ParseCommunicationCommand.ExecuteCommand("取消自动送料");
                    break;
                case "btn_ClothPressUp":
                    //压布棍升
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("压布棍-升");
                    break;
                case "btn_ClothPressDown":
                    //压布棍降
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("压布棍-降");
                    break;
                //btn_ManualInFeed,btn_ManualInFeedSet,btn_ManualOutFeed,btn_ManualCutFeed
                case "btn_ManualInFeed":
                    //手动进料
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        btn_ManualInFeed();
                    break;
                case "btn_ManualInFeedSet":
                    //手动进料设置
                    btn_SetManualFeed(mbtn);
                    break;
                case "btn_ManualOutFeed":
                    //手动退料
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        btn_ManualOutFeed();
                    break;
                case "btn_ManualCutFeed":
                    //手动切断
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("手动切断");
                    break;
                #endregion
                #region 系统测试
                case "btn_TopCloth-1":
                    //顶布测试-顶布       writeDataList["顶布测试-顶布];
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("顶布测试-顶布");
                    break;
                case "btn_TopCloth-2":
                    //顶布测试-落下 
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("顶布测试-落下");
                    break;
                case "btn_ClipClothUp":
                    //夹布测试-升
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("夹布测试-升");
                    break;
                case "btn_ClipClothDown":
                    //夹布测试-降
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("夹布测试-降");
                    break;
                case "btn_Gassing-1":
                    //吹气测试-吹气
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("吹气测试-吹气");
                    break;
                case "btn_Gassing-2":
                    //吹气测试-关闭
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("吹气测试-关闭");
                    break;
                case "btn_AutoFeed":
                    //自动送料测试
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                    {
                        ParseCommunicationCommand.ExecuteCommand("自动送料测试");
                        isAutoFeed = true;
                    }
                    break;
                case "btn_LaserPoint":
                    //激光点射
                    btn_SetLaser(mbtn);
                    //ParseCommunicationCommand.ExecuteCommand("激光点射");
                    break;
                case "btn_SetLaserPoint":
                    //激光点射设置
                    //btn_SetLaser(mbtn);
                    break;
                case "btn_IdlingBraking":
                    //空走测试
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("空走测试");
                    break;
                #endregion
                #region 文件
                case "btn_SelectImageFile":
                    //选择文件
                    btn_SelectImageFile(mbtn);
                    break;
                case "btn_EditFile":
                    //编辑文件
                    btn_EditCutPath(mbtn);
                    break;
                case "btn_TurnFile":
                    btn_TurnFile(mbtn);
                    //转换文件
                    break;
                case "btn_WriteFile":
                    //写入内存
                    btn_WriteFile(mbtn);
                    break;
                case "btn_DeleteFile":
                    //删除文件
                    break;
                case "btn_ZoomIn":
                    //放大
                    owner.paintingPanel1.mouseOperator = MouseOperator.ZoomIn;
                    break;
                case "btn_ZoomOut":
                    owner.paintingPanel1.mouseOperator = MouseOperator.ZoomOut;
                    //缩小
                    break;
                case "btn_Pan":
                    owner.paintingPanel1.mouseOperator = MouseOperator.Pan;
                    //平移
                    break;
                case "btn_Full":
                    owner.paintingPanel1.FullDisplay();
                    //全图
                    break;
                #endregion
                #region 切割项目
                case "btn_NewProject":
                    break;
                case "btn_EditProject":
                    btn_EditProject(mbtn);
                    break;
                case "btn_OpenProject":
                    btn_OpenProject(mbtn);
                    break;
                #endregion
                case "btn_CloseLaserB":
                    ////if (ParseCommunicationCommand.deviceState != null || (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop))
                    //{
                    //    ParseCommunicationCommand.isCloseLaserB = !ParseCommunicationCommand.isCloseLaserB;
                    //}                    
                    ////关闭激光头B
                    //if (ParseCommunicationCommand.isCloseLaserB)
                    //{
                    //    mbtn.Text = "打开激光\r\n\r\n头 B";
                    //    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                    //        ParseCommunicationCommand.ExecuteCommand("打开激光头B");
                    //    PaintingPanel.IsSingleLaser = true;
                    //}
                    //else
                    //{
                    //    mbtn.Text = "关闭激光\r\n\r\n头 B"; 
                    //    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                    //        ParseCommunicationCommand.ExecuteCommand("关闭激光头B");
                    //    PaintingPanel.IsSingleLaser = false;
                    //}
                    //owner.paintingPanel1.Invalidate();
                    //ParseCommunicationCommand.SaveConfig(ParseCommunicationCommand.isCloseLaserB.ToString(), "IsCloseLaserB");
                    break;
                case "btn_Clear":
                    if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
                        ParseCommunicationCommand.ExecuteCommand("清洗功能");
                    break;
                case "btn_PreviewFile":
                    //切割预览
                    if (ParseCommunicationCommand.deviceState == null  || (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop))
                    {
                        if (owner.PreviewStart == 0)
                        {
                            owner.PreviewStart = 1;
                            mbtn.Text = "切割预览\r\n\r\n停止";
                        }
                        else
                        {
                            owner.PreviewStart = 0;
                            mbtn.Text = "切割预览\r\n\r\n开始";
                        }
                    }
                    else
                    {
                        owner.PreviewStart = 0;
                        mbtn.Text = "切割预览\r\n\r\n开始";
                    }
                    break;
                default:
                    break;
            }
        }

        /// <summary>
        /// 手动上料结束
        /// </summary>
        private void btn_ManualFeedEnd(MenuButton mbtn)
        {
            List<byte> data = new List<byte>();
            data.AddRange(ParseCommunicationCommand.GetCommandByte(ParseCommunicationCommand.sendCommandFlag, 0x08, byte.MinValue, false));
            data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(ParseCommunicationCommand.feedOffset)));
            data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(ParseCommunicationCommand.sectionLen)));
            ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "手动上料结束", 120);
            //ParseCommunicationCommand.ExecuteCommand("手动进料");
        }

        /// <summary>
        /// 手动进料设置
        /// </summary>
        public void btn_SetManualFeed(MenuButton mbtn)
        {
            if (setManualFeed == null)
            {
                setManualFeed = new frm_SetManualFeed();
                setManualFeed.Owner = owner;
                //setLaser.TopMost = true;
                setManualFeed.Tag = "Visble-1";
                mbtn.Tag = setManualFeed;
            }
            if (setManualFeed.Visible)
            {
                setManualFeed.Hide();
            }
            else
            {
                setManualFeed.Location = new System.Drawing.Point(owner.SplitPanel.Location.X + owner.SplitPanel.Width / 2 - setManualFeed.Width / 2, (owner.SplitPanel.Height - setManualFeed.Height) / 2);
                setManualFeed.Show();
            }
        }
        /// <summary>
        /// 手动进料
        /// </summary>
        private void btn_ManualInFeed()
        {
            List<byte> data = new List<byte>();
            data.AddRange(ParseCommunicationCommand.GetCommandByte(ParseCommunicationCommand.sendCommandFlag, 0x0B, byte.MinValue, false));
            data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(ParseCommunicationCommand.manualFeedLen)));
            ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "手动进料", 120);
            //ParseCommunicationCommand.ExecuteCommand("手动进料");
        }
        /// <summary>
        /// 手动退料
        /// </summary>
        private void btn_ManualOutFeed()
        {
            List<byte> data = new List<byte>();
            data.AddRange(ParseCommunicationCommand.GetCommandByte(ParseCommunicationCommand.sendCommandFlag, 0x0D, byte.MinValue, false));
            data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(ParseCommunicationCommand.manualFeedLen)));
            ParseCommunicationCommand.ExecuteLaserPointCommand(data.ToArray(), "手动退料", 120);
            //ParseCommunicationCommand.ExecuteCommand("手动退料");
        }
        public void btn_TurnFile(MenuButton mbtn)
        {
            if (owner.paintingPanel1.FilePath == "")
            {
                btn_SelectImageFile(mbtn);
            }
            owner.paintingPanel1.ConvertToPLT();
        }
        public void btn_WriteFile(MenuButton mbtn)
        {
            if (ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
            {
                MessageBox.Show("连接异常！","提示");
                return;
            }
            if(ParseCommunicationCommand.deviceState.workState == WorkState.Start) 
            {
                MessageBox.Show("设备正在工作，请停止工作后重试！", "提示");
                return;
            }
            WritePltFile(mbtn,false);
        }

        public bool WritePltFile(MenuButton mbtn, bool isInitial)
        {
            if (ParseCommunicationCommand.deviceState != null && ParseCommunicationCommand.deviceState.workState == WorkState.Stop)
            {
                ParseCommunicationCommand.IsReading = true;
                if (owner.paintingPanel1.FilePath == "" && !isInitial)
                    btn_SelectImageFile(mbtn);
                else if (owner.paintingPanel1.FilePath != "")
                {
                    string str = "";
                    string filePath = "";
                    if (!PaintingPanel.IsSingleLaser && ParseCommunicationCommand.readParam.distanceAB == 0)
                    {
                        str = owner.paintingPanel1.ConvertToPLT(true, true);
                        filePath = owner.paintingPanel1.saveFilePath;
                    }
                    else
                    {
                        if (Path.GetExtension(owner.paintingPanel1.FilePath).ToLower() == ".dxf")
                        {
                            str = owner.paintingPanel1.ConvertToPLT(true);
                            filePath = owner.paintingPanel1.saveFilePath;
                        }
                        else
                        {
                            filePath = owner.paintingPanel1.FilePath;
                            str = owner.paintingPanel1.pltFileContent;
                        }
                    }

                    owner.paintingPanel1.GetFinishWorkPoint();
                    FileInfo fileInfo = new FileInfo(filePath);
                    if (!fileInfo.Exists)
                    {
                        if (!isInitial)
                            MessageBox.Show(fileInfo.FullName + "文件不存在!", "提示！");
                        ParseCommunicationCommand.IsReading = false;
                        CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "写入数据失败；" + fileInfo.FullName + "文件不存在!" + "\r\n");

                        return false;
                    }
                    List<byte> data = new List<byte>();
                    if (ParseCommunicationCommand.sendCommandFlag != null)
                        data.AddRange(ParseCommunicationCommand.sendCommandFlag);
                    data.Add(0x15);
                    int w = (int)(owner.paintingPanel1.widthG);
                    if (w < owner.paintingPanel1.widthG)
                        w += 1;
                    int h = (int)(owner.paintingPanel1.heightG);
                    if (h < owner.paintingPanel1.heightG)
                        h += 1;
                    data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(w)));
                    data.AddRange(ConvertType.Reverse(BitConverter.GetBytes(h)));
                    data.Add(0x01);
                    data.AddRange(ConvertType.Reverse(BitConverter.GetBytes((int)fileInfo.Length)));
                    //ParseCommunicationCommand.ExecuteOriginPointCommand(data.ToArray(), "文件信息传输", 120);
                    byte[] readData = ParseCommunicationCommand.ExecuteOriginPointCommand(data.ToArray(), "文件信息传输", 120, true);

                    if (readData != null && readData[0] == 0x25)
                    {
                        List<byte> data1 = new List<byte>();
                        try
                        {
                            System.Threading.Thread.Sleep(1000);
                            int length = 64;
                            int filelength = (int)fileInfo.Length;
                            byte[] sendBuf = new byte[4];
                            while (true)
                            {
                                data1.AddRange(System.Text.Encoding.Default.GetBytes("Buf_Long_Start"));
                                CommuncationData cd = new CommuncationData();
                                cd.writeData = data1.ToArray();
                                cd.dataType = "Buf_Long_Start";
                                cd.isWrite = ParseCommunicationCommand.WriteDeviceData(cd.writeData);
                                if (cd.isWrite)
                                {
                                    length = 64;

                                    cd.readData = ParseCommunicationCommand.ReadDeviceData1(length);
                                    //cd.readData = ParseCommunicationCommand.ReadDeviceData(lengt);
                                    ParseCommunicationCommand.WriteFileCommand(cd);
                                    if (cd.readData != null)
                                    {
                                        ParseCommunicationCommand.WriteFileCommand(cd);
                                        sendBuf = cd.readData;
                                        //sendBuf[3] = cd.readData[0];
                                        //sendBuf[2] = cd.readData[1];
                                        //sendBuf[1] = cd.readData[2];
                                        //sendBuf[0] = cd.readData[3];
                                        int ibflen = 0;
                                        int tets = ConvertType.ToInt32(sendBuf, ref ibflen);
                                        if (tets > filelength)
                                            break;
                                        //else
                                        //    System.Threading.Thread.Sleep(10);
                                    }
                                }
                                if (ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
                                {
                                    break;
                                }
                            }
                            data1.Clear();
                            data1.AddRange(System.Text.Encoding.Default.GetBytes("Snd_Long"));
                            data1.AddRange(ConvertType.Reverse(BitConverter.GetBytes(filelength)));
                            length = 12;
                            CommuncationData cd1 = new CommuncationData();
                            cd1.writeData = data1.ToArray();
                            cd1.dataType = "Snd_Long";
                            cd1.isWrite = ParseCommunicationCommand.WriteDeviceData(cd1.writeData);
                            if (cd1.isWrite)
                            {
                                length = 64;
                                cd1.readData = ParseCommunicationCommand.ReadDeviceData(length);
                                ParseCommunicationCommand.WriteFileCommand(cd1);
                                if (cd1.readData != null)
                                {
                                    ParseCommunicationCommand.WriteFileCommand(cd1);
                                }
                            }
                            string msg_ok = System.Text.Encoding.Default.GetString(cd1.readData);
                            data1.Clear();
                            data1.AddRange(System.Text.Encoding.Default.GetBytes(str));

                            if (msg_ok == "OkS ")
                            {
                                while (true)
                                {
                                    length = 64;
                                    if (filelength < 64)
                                        length = filelength;
                                    if (CommonModule.CH375DLL.WriteData(0, data1.ToArray(), ref length))
                                    {
                                        if (filelength > length)
                                        {
                                            filelength -= length;
                                            data1.RemoveRange(0, length);
                                        }
                                        else
                                            break;
                                    }
                                    if (ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
                                    {
                                        break;
                                    }
                                }
                            }
                            while (true)
                            {
                                length = 64;
                                byte[] btyA = new byte[64];
                                CommonModule.CH375DLL.ReadData(0, btyA, ref length);
                                byte[] btyAR = new byte[8];
                                for (int i = 0; i < btyAR.Length; i++)
                                {
                                    btyAR[i] = btyA[i];
                                }
                                string endstr = System.Text.Encoding.Default.GetString(btyAR);

                                if (endstr == "Msg_End " || endstr == "Msg_OkPG")
                                {
                                    if (!isInitial)
                                    {
                                        btn_ClearProduct(null);
                                        MessageBox.Show("文件写入成功！", "提示");
                                    }
                                    ParseCommunicationCommand.SaveLastFile(owner.paintingPanel1.FilePath);
                                    break;
                                }
                                if (ParseCommunicationCommand.usbIntPtr.ToInt32() == -1)
                                {
                                    break;
                                }
                            }

                            while (true)
                            {
                                data1.Clear();
                                if (ParseCommunicationCommand.sendCommandFlag != null)
                                    data1.AddRange(ParseCommunicationCommand.sendCommandFlag);
                                data1.Add(0x16);
                                //data1.AddRange(System.Text.Encoding.Default.GetBytes(str));
                                if (ParseCommunicationCommand.ExecuteOriginPointCommand(data1.ToArray(), "文件数据传输", 120))
                                    break;
                            }

                            //ParseCommunicationCommand.ReadConfig(owner.paintingPanel1.FilePath);

                            ParseCommunicationCommand.IsReading = false;
                            return true;
                        }
                        catch (Exception ex)
                        {
                            if (!isInitial)
                                MessageBox.Show("文件写入失败！", "提示");
                            ParseCommunicationCommand.IsReading = false;
                            CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "写入数据失败报异常；"+ ex.Message + "\r\n");
                            return false;
                        }
                        //Buf_Long_Start
                        //Snd_Long
                        //Msg_OkS
                        //data1.Clear();
                        //ParseCommunicationCommand.ExecuteOriginPointCommand(data1.ToArray(), "文件数据传输", 120);
                    }
                    else if (readData != null && readData[0] == 0xE5)
                    {
                        if (!isInitial)
                            MessageBox.Show("设备正在工作，请停止工作后重试！", "提示");
                        CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "写入数据失败;设备正在工作，请停止工作后重试！\r\n");
                    }
                    else if (readData != null && readData[0] == 0xF5)
                    {
                        if (!isInitial)
                            MessageBox.Show("图形在此定位点上超出切割范围，请重新设置定位点！", "提示");
                        CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "写入数据失败;图形在此定位点上超出切割范围，请重新设置定位点！\r\n");
                    }
                    else
                    {
                        if (!isInitial)
                            MessageBox.Show("文件写入失败！", "提示");
                        CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "写入数据失败2\r\n");
                    }
                }
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "写入数据失败3\r\n");
                ParseCommunicationCommand.IsReading = false;
            }
            CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + "写入数据失败1\r\n");
            return false;
        }

        public void btn_SelectImageFile(MenuButton mbtn)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Multiselect = false;
            openFile.Filter = "dxf文件;plt文件|*.dxf;*.plt";
            if (openFile.ShowDialog(owner) == DialogResult.OK)
            {
                string fileExt = Path.GetExtension(openFile.FileName);
                if (fileExt.ToLower() == ".dxf" && owner.paintingPanel1.OpenDxfFile(openFile.FileName))
                {
                    owner.SetFileName(openFile.FileName);
                }
                else if (fileExt.ToLower() == ".plt" && owner.paintingPanel1.ReadPltFile(openFile.FileName))
                {
                    owner.SetFileName(openFile.FileName);
                }
                //ParseCommunicationCommand.SaveConfig();
                //ParseCommunicationCommand.ReadConfig();
            }
        }

        /// <summary>
        /// 编辑切割路径
        /// </summary>
        public void btn_EditCutPath(MenuButton mbtn)
        {
            if (editCutPath == null)
            {
                editCutPath = new frm_EditCutPath();
                editCutPath.Owner = owner;
                editCutPath.paintingPanel = owner.paintingPanel1;
                editCutPath.Tag = "Visble-1";
                mbtn.Tag = editCutPath;
            }
            if (editCutPath.Visible)
            {
                editCutPath.Hide();
            }
            else
            {
                editCutPath.shapeList = owner.paintingPanel1.dLineList;
                editCutPath.Location = new System.Drawing.Point(owner.SplitPanel.Location.X + owner.SplitPanel.Width / 2 - editCutPath.Width / 2, mbtn.FindForm().Location.Y - editCutPath.Height - 10);
                editCutPath.Show();
            }
        }
        public void btn_SetParam(MenuButton mbtn)
        {
            if (ps == null)
            {
                ps = new frm_SettingParam();
                ps.Owner = owner;
                //ps.TopMost = true;
                ps.Tag = "Visble-1";
                mbtn.Tag = ps;
                ps.addCutFileForm = owner.userSetParam1.addCutFileForm;
            }
            if (ps.Visible)
            {
                ps.Hide();
            }
            else
            {
                ps.path = owner.paintingPanel1.FilePath;
                frm_Project.isProject = false;
                owner.ReadDeviceParam();
                //ps.Location = new System.Drawing.Point(owner.SplitPanel.Location.X + owner.SplitPanel.Width / 2 - ps.Width / 2, mbtn.FindForm().Location.Y - ps.Height);
                ps.Location = new System.Drawing.Point(owner.SplitPanel.Location.X + owner.SplitPanel.Width / 2 - ps.Width / 2, materialMenu.Location.Y - ps.Height);
                ps.Show();
            }
            //foreach (Form item in owner.OwnedForms)
            //{
            //    if (item.Tag == null || item.Tag.ToString() != "Visble-1")
            //        continue;
            //    if (item == ps)
            //    {
            //        if (ps.Visible)
            //        {
            //            ps.Hide();
            //        }
            //        else
            //        {
            //            ParseCommunicationCommand.ExecuteQueryDeviceParam(ParseCommunicationCommand.writeDataList["读取参数"], "读取参数", 120);
            //            ps.Location = new System.Drawing.Point(owner.SplitPanel.Location.X + owner.SplitPanel.Width / 2 - ps.Width / 2, mbtn.FindForm().Location.Y - ps.Height);
            //            ps.Show();
            //        }
            //    }
            //    else
            //        item.Hide();
            //}
        }

        public void btn_SelectFile(MenuButton mbtn)
        {
            mbtn.Tag = fileMenu;
            if (fileMenu.Visible)
            {
                fileMenu.Hide();
            }
            else
            {
                //int x = mbtn.FindForm().Location.X + mbtn.Location.X + mbtn.Width / 2 - fileMenu.Width / 2;
                int x = materialMenu.Location.X + mbtn.Location.X + mbtn.Width / 2 - fileMenu.Width / 2;
                if (x < owner.SplitPanel.Left)
                {
                    x = 10;
                }
                //fileMenu.Location = new System.Drawing.Point(x, mbtn.FindForm().Location.Y + mbtn.Location.Y - fileMenu.Height - 10);
                fileMenu.Location = new System.Drawing.Point(x, materialMenu.Location.Y + mbtn.Location.Y - fileMenu.Height - 10); 
                fileMenu.Show();
            }
        }
        /// <summary>
        /// 切割项目
        /// </summary>
        /// <param name="mbtn"></param>
        public void btn_CutProject(MenuButton mbtn)
        {
            if (ParseCommunicationCommand.userType == 2)
            {
                btn_OpenProject(mbtn);
            }
            else
            {
                mbtn.Tag = projectMenu;
                if (projectMenu.Visible)
                {
                    projectMenu.Hide();
                }
                else
                {
                    int x = materialMenu.Location.X + mbtn.Location.X + mbtn.Width / 2 - projectMenu.Width / 2;
                    if (x < owner.SplitPanel.Left)
                    {
                        x = 10;
                    }
                    projectMenu.Location = new System.Drawing.Point(x, materialMenu.Location.Y + mbtn.Location.Y - projectMenu.Height - 10);
                    projectMenu.Show();
                }
            }
        }
        //public void btn_SelectFile(MenuButton mbtn)
        //{
        //    if (selectFile == null)
        //    {
        //        selectFile = new frm_SelectFile();
        //        selectFile.Owner = owner;
        //        selectFile.TopMost = true;
        //        selectFile.Tag = "Visble-1";
        //    }
        //    foreach (Form item in owner.OwnedForms)
        //    {
        //        if (item.Tag == null || item.Tag.ToString() != "Visble-1")
        //            continue;
        //        if (item == selectFile)
        //        {
        //            if (selectFile.Visible)
        //            {
        //                selectFile.Hide();
        //            }
        //            else
        //                selectFile.Show();
        //        }
        //        else
        //            item.Hide();
        //    }
        //}

        /// <summary>
        /// 定位点设置
        /// </summary>
        public void btn_SetOrigin(MenuButton mbtn)
        {
            if (mf == null)
            {
                mf = new frm_SetOrigin();
                mf.Owner = owner;
                //mf.TopMost = true;
                mf.Tag = "Visble-1";
                mbtn.Tag = mf;
            }
            if (mf.Visible)
            {
                mf.Hide();
            }
            else
            {
                mf.addCutFileForm = owner.userSetParam1.addCutFileForm;
                frm_Project.isProject = false;
                mf.Location = new System.Drawing.Point(owner.SplitPanel.Location.X + owner.SplitPanel.Width / 2 - mf.Width / 2, (owner.SplitPanel.Height - mf.Height)/2);
                mf.path = owner.paintingPanel1.FilePath;
                mf.Show();
            }
        }
        /// <summary>
        /// 上料调试
        /// </summary>
        public void btn_FeedMode(MenuButton mbtn)
        {
            mbtn.Tag = feedMenu;

            if (feedMenu.Visible)
            {
                feedMenu.Hide();
            }
            else
            {
                int x = materialMenu.Location.X + mbtn.Location.X + mbtn.Width / 2 - feedMenu.Width / 2;
                if (x + feedMenu.Width > owner.SplitPanel.Right)
                {
                    int t = x + feedMenu.Width - owner.SplitPanel.Right;
                    x = x - t - 10;
                }
                feedMenu.Location = new System.Drawing.Point(x, materialMenu.Location.Y + mbtn.Location.Y - feedMenu.Height - 10);
                feedMenu.Show();
            }
        }

        /// <summary>
        /// 系统测试
        /// </summary>
        public void btn_SysTest(MenuButton mbtn)
        {
            mbtn.Tag = testMenu;
            if (testMenu.Visible)
            {
                testMenu.Hide();
            }
            else
            {
                int x = materialMenu.Location.X + mbtn.Location.X + mbtn.Width / 2 - testMenu.Width / 2;
                if (x < 0)
                {
                    x = 10;
                }
                testMenu.Location = new System.Drawing.Point(x, materialMenu.Location.Y + mbtn.Location.Y - testMenu.Height - 10);
                testMenu.Show();
            }
        }

        /// <summary>
        /// 系统设置
        /// </summary>
        public void btn_SysSet(MenuButton mbtn)
        {
            if (sysSet == null)
            {
                sysSet = new frm_SysSet();
                sysSet.Owner = owner;
                //sysSet.TopMost = true;
                sysSet.Tag = "Visble-1";
                mbtn.Tag = sysSet;
            }
            if (sysSet.Visible)
            {
                sysSet.Hide();
            }
            else
            {
                owner.ReadDeviceParam();
                sysSet.Location = new System.Drawing.Point(owner.SplitPanel.Location.X + owner.SplitPanel.Width / 2 - sysSet.Width / 2, (owner.SplitPanel.Height - sysSet.Height) / 2);
                sysSet.Show();
            }
        }

        /// <summary>
        /// 点射设置
        /// </summary>
        public void btn_SetLaser(MenuButton mbtn)
        {
            if (setLaser == null)
            {
                setLaser = new frm_SetLaser();
                setLaser.Owner = owner;
                //setLaser.TopMost = true;
                setLaser.Tag = "Visble-1";
                mbtn.Tag = setLaser;
            }
            if (setLaser.Visible)
            {
                setLaser.Hide();
            }
            else
            {
                setLaser.Location = new System.Drawing.Point(owner.SplitPanel.Location.X + owner.SplitPanel.Width / 2 - setLaser.Width / 2, (owner.SplitPanel.Height - setLaser.Height)/2);
                setLaser.Show();
            }
        }

        /// <summary>
        /// 清除产量
        /// </summary>
        /// <param name="mbtn"></param>
        public void btn_ClearProduct(MenuButton mbtn)
        {
            ParseCommunicationCommand.cutNum = -1;
            owner.UpdateState();
        }

        /// <summary>
        /// 密码设置
        /// </summary>
        public void btn_EditUser(MenuButton mbtn)
        {
            if (editUser == null)
            {
                editUser = new frm_EditUser();
                editUser.Owner = owner;
                editUser.loadform = owner.loadForm;
                //sysSet.TopMost = true;
                editUser.Tag = "Visble-1";
                mbtn.Tag = editUser;
            }
            if (editUser.Visible)
            {
                editUser.Hide();
            }
            else
            {
                editUser.Location = new System.Drawing.Point(owner.SplitPanel.Location.X + owner.SplitPanel.Width / 2 - editUser.Width / 2, (owner.SplitPanel.Height - editUser.Height) / 2-45);
                editUser.Show();
            }
        }

        /// <summary>
        /// 新建项目
        /// </summary>
        /// <summary>
        /// 编辑项目
        /// </summary>
        public void btn_EditProject(MenuButton mbtn)
        {
            if (openProject == null)
            {
                openProject = new frm_OpenProject();
                openProject.Owner = owner;

                openProject.Tag = "Visble-1";
                mbtn.Tag = openProject;
                //openProject.VisibleChanged += openProject_VisibleChanged;
            }
            if (openProject.Visible)
            {
                openProject.Hide();
            }
            else
            {
                if (owner.info_Form != null)
                    owner.info_Form.Hide();
                openProject.userButtonPanel1.isOpen = false;
                openProject.Location = new System.Drawing.Point(owner.SplitPanel.Location.X + owner.SplitPanel.Width / 2 - openProject.Width / 2, (owner.SplitPanel.Height - openProject.Height) / 2);
                openProject.Show();
            }
        }
        /// <summary>
        /// 打开项目
        /// </summary>
        public void btn_OpenProject(MenuButton mbtn)
        {
            if (openProject == null)
            {
                openProject = new frm_OpenProject();
                openProject.Owner = owner;
               
                openProject.Tag = "Visble-1";
                mbtn.Tag = openProject;
                //openProject.VisibleChanged += openProject_VisibleChanged;
            }
            if (openProject.Visible)
            {
                openProject.Hide();
            }
            else
            {
                if (owner.info_Form != null)
                    owner.info_Form.Hide();
                openProject.userButtonPanel1.isOpen = true;
                openProject.Location = new System.Drawing.Point(owner.SplitPanel.Location.X + owner.SplitPanel.Width / 2 - openProject.Width / 2, (owner.SplitPanel.Height - openProject.Height) / 2);
                openProject.Show();
            }
        }

        public void btn_WorkMenu(MenuButton mbtn)
        {
            mbtn.Tag = workMenu;
            if (workMenu.Visible)
            {
                workMenu.Hide();
            }
            else
            {
                int x = materialMenu.Location.X + mbtn.Location.X + mbtn.Width / 2 - workMenu.Width / 2;
                if (x < owner.SplitPanel.Left)
                {
                    x = 10;
                }
                else if (x + workMenu.Width > owner.SplitPanel.Right)
                {
                    int t = x + workMenu.Width - owner.SplitPanel.Right;
                    x = x - t - 10;
                }
                workMenu.Location = new System.Drawing.Point(x, materialMenu.Location.Y + mbtn.Location.Y - workMenu.Height - 10);
                workMenu.Show();
            }
        }
        /// <summary>
        /// 设置按钮颜色样式
        /// </summary>
        /// <param name="btn"></param>
        /// <param name="state"></param>
        public void SetButtonStyle(Button btn, byte state)
        {
            switch (state)
            {
                case 0:
                    //正常
                    btn.BackColor = Color.Lime;
                    btn.ForeColor = Color.Black;
                    break;
                case 1:
                    //报警
                    btn.BackColor = Color.Red;
                    btn.ForeColor = Color.WhiteSmoke;
                    break;
                case 2:
                    //不可用
                    btn.BackColor = Color.Gray;
                    btn.ForeColor = Color.Black;
                    break;
                case 3:
                    //正常
                    btn.BackColor = Color.Yellow;
                    btn.ForeColor = Color.Black;
                    break;
                case 4:
                    //正常
                    btn.BackColor = Color.Blue;
                    btn.ForeColor = Color.WhiteSmoke;
                    break;
                default:
                    //默认
                    btn.BackColor = SystemColors.ActiveCaption;
                    btn.ForeColor = Color.Black;
                    break;
            }
        }
        /// <summary>
        /// 设置按钮颜色样式
        /// </summary>
        /// <param name="btn"></param>
        /// <param name="flag"></param>
        public void SetButtonStyle(Button btn, bool flag)
        {
            if (flag)
            {
                CommonModule.ConvertType.WriteFileLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss ： ") + btn.Text + "" + "\r\n");
                SetButtonStyle(btn, 1);
            }
            else
                SetButtonStyle(btn, 0);
        }
        public bool SwitchMenuForm()
        {
            foreach (Form item in owner.OwnedForms)
            {
                if (item.Tag == null || item.Tag.ToString() != "Visble-1")
                    continue;
                item.Hide();
            }

            foreach (Control item in owner.SplitPanel.Controls)
            {
                if (item is MenuPanel)
                {
                    if (item != materialMenu && item.Visible)
                        item.Hide();
                }
            }
            materialMenu.Visible = !materialMenu.Visible;
            return materialMenu.Visible;
        }
    }
}