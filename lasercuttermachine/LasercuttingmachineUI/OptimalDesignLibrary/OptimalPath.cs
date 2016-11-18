using System;
using System.Collections.Generic;
using System.Text;

namespace OptimalDesignLibrary
{
    public class OptimalPath
    {
        private Graph grp;            //初始化生成的图
        private Graph chang_grp;           // 对初始的图进行不断删除节点的生成图
        private Stack solution = new Stack();  // 在贪心算法中存放节点路径的堆栈
        //最后的所以扩展完的对象均在closed堆栈中
        private int[] parent;

        public void InitialGraph(int n)
        {
            solution.SetEmpty();
            grp = new Graph(n, 1);               //随机生成一个N个节点组成的图
            ShowGraph(grp);
            chang_grp = new Graph(grp);
        }

        private void 贪心算法_Click(int tx_Count)
        {
            solution.SetEmpty();
            Graph temp = new Graph();
            temp = chang_grp.DelNode(0);
            int count = temp.GetCount();
            int[,] data = grp.GetData();
            Stack stc = new Stack();
            GNode node = new GNode(0, temp);
            int s = 0;
            int min = 0;
            for (int i = 0; i < count; i++)                     //计算距离前一个点的最近点的数值
            {
                //并将路径上的点进入到栈stc中
                int num = count - i;
                int[] p = temp.GetNode();

                int[] m = new int[num];
                for (int j = 0; j < num; j++)
                {
                    m[j] = data[s, p[j]];
                }

                min = GetMin(m, num);
                s = GetMinLocation(m, min, num);
                s = p[s];
                Graph q = new Graph(temp.DelNode(s));
                GNode new_node = new GNode(s, q);

                stc.Push(new_node);
                temp = q;
            }

            while (stc.Empty() != true)        //将stc 中的点压入solutiong栈中，以便顺序输出路径上的点
            {
                solution.Push(stc.Pop());
            }
            int n = solution.GetNum();
            int[] length = new int[n + 1];      //存放路径上相邻两点中的距离
            int[] arr = new int[n + 2];       //存放路径上的点，包括第一个和最后一个点0
            arr[0] = arr[n + 1] = 0;
            int j1 = 0;
            int total = 0;
            for (int i = 0; i < n; i++)
            {
                GNode sol = new GNode();
                sol = solution.Pop();
                length[i] = data[j1, sol.GetI()];
                //rtx_Solution.Text += j1.ToString() + "到" + sol.GetI().ToString() + "的距离为：" + length[i] + "    " + '\n';
                j1 = sol.GetI();
                arr[i + 1] = j1;
                total += length[i];
            }
            length[n] = data[j1, 0];
            //rtx_Solution.Text += j1.ToString() + "到" + "0" + "的距离为：" + length[n] + "    " + '\n';
            total += length[n];
            //rtx_Solution.Text += "路径路线为：";
            for (int j = 0; j < n + 2; j++)
            {
                //rtx_Solution.Text += arr[j] + "   ";

            }
            //rtx_Solution.Text += '\n' + "总路径长度为：" + total.ToString();
            parent = new int[tx_Count];
            for (int j = 0; j < n + 1; j++)
            {
                parent[j] = arr[j + 1];
            }
        }

        private void ShowGraph(Graph grp)
        {
            int count = grp.GetCount();
            int[] arr = new int[count];
            arr = grp.GetNode();
            int[,] temp = new int[count, count];
            temp = grp.GetData();

            //rtx_Graph.Text += "    ";
            //for (int i = 0; i < count; i++)
            //{
            //    rtx_Graph.Text = rtx_Graph.Text + Convert.ToString(arr[i]) + "    ";
            //}
            //rtx_Graph.Text += '\n';
            //for (int i = 0; i < count; i++)
            //{
            //    rtx_Graph.Text += arr[i].ToString() + "   ";
            //    for (int j = 0; j < count; j++)
            //    {
            //        if (temp[i, j] >= 10)
            //            rtx_Graph.Text += temp[i, j].ToString() + "   ";
            //        if (temp[i, j] < 10)
            //            rtx_Graph.Text += temp[i, j].ToString() + "    ";

            //    }
            //    rtx_Graph.Text += '\n';
            //}
        }       //显示一个图

        //显示一个节点对象
        private int GetMin(int[] p, int n)
        {
            int temp = 10000;
            for (int i = 0; i < n; i++)
            {
                if (p[i] < temp)
                    temp = p[i];

            }
            return temp;
        }        //在一个数组中返回最小的值
        private int GetMinLocation(int[] p, int data, int n)
        {
            int j = -2;
            for (int i = 0; i < n; i++)
            {
                if (p[i] == data)
                    j = i;
            }
            return j;
        }       //在一个数组中返回指定数据所在的位置
    }
}