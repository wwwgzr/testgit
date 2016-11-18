using System;
using System.Collections.Generic;
using System.Text;

namespace CommunicationModule
{
    public class ProjectClass
    {
        public List<ProductClass> products = new List<ProductClass>();
        public string name;
        public string iconName;
    }

    public class ProductClass
    {
        public string name;
        public string iconName;
        public List<CutFileClass> cutFiles = new List<CutFileClass>();
        public ProjectClass parent;
    }

    public class CutFileClass
    {
        public string cutFileName;
        public ReadParam readParam;
        public int x;
        public int y;
        /// <summary>
        /// 打开文件 0  设置参数 1  设置定位点 2
        /// </summary>
        public int step = 0;
        /// <summary>
        /// 手动进料长度
        /// </summary>
        public short manualFeedLength = 10;
        /// <summary>
        /// 手动上料分段长度
        /// </summary>
        public short manualFeedBreakLength = 30;
        /// <summary>
        /// 手动上偏移长度
        /// </summary>
        public short manualFeedOffsetLength = 600;
        public ProductClass parent;
    }
}