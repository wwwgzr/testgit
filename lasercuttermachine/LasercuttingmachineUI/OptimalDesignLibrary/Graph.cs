using System;
using System.Collections.Generic;
using System.Text;

namespace OptimalDesignLibrary
{
    class Graph
    {
        private int count;    //图的城市节点的个数
        private int[,] data;   //图的所有节点之间的长度距离矩阵
        private int[] node;    //图的所有节点组成的数组
        private int short_value;    //当前图的简约值

        public Graph (int n,int state=0)     //构造函数，构造一个n个节点的有向图，节点与节点之间的距离（1~100）,随机产生
        {
            this.count =n; 
            data = new int[n, n];
            node = new int[n];
            Random rdm = new Random();
            for (int i = 0; i < n; i++)
            {
                node[i] = i ;
            }
             for (int i = 0; i < n; i++)
                {
                    for (int j = 0; j < n; j++)
                    {
                        if (i == j)
                        {
                            if (state == 0)
                                data[i, j] = 0;
                            else if (state == 1)
                                data[i, j] = 10000;
                        }
                        else
                        {

                            data[i, j] = rdm.Next(1, 100);
                        }
                    }
                }
        }
        public Graph()
        {
            //data=new int []
        }
        public Graph(Graph grp)
        {
            int count= grp.GetCount();
            int[] n = new int[count];
            int[,] p = new int[count, count];
            for (int i = 0; i < count; i++)
            {
                n[i] = grp.GetNode()[i];
                for (int j = 0; j < count; j++)
                {
                    p[i, j] = grp.GetData()[i, j];
                }
            }
            this.count = count;
            this.data = p;
            this.node = n;
            this.short_value = grp.short_value;
        }
        public int GetCount()              //获取当前图的城市节点个数
        {
            return this.count;
        }
        public void SetCount(int i)            //设置图的城市节点个数
        {
            this.count = i;
        }
        public int[,] GetData()               //获取图的距离矩阵
        {
            return this.data;
        }
        public void SetData(int[,] p)            //   设置图的距离矩阵
        {
            this.data = p;
        }
        public int[] GetNode() //      获取图的城市节点数组
        {
            return this.node;
        }
        public Graph DelNode(int a)                
        {
           
            int temp = this.count;
            if (this.count > 0)
            {
               
                int n = this.count-1;
                int[,] m = new int[n, n];            //新矩阵
                int[] arr = new int[count];
                int p = 0;
                int q = 0;
                if (a < this.GetNode()[temp-1])
                {
                    for (int i = 0; i < temp; i++)      //重新构造删除了一个节点过后的矩阵
                    {

                        if (this.GetNode()[i] == a)
                        {
                            i++;
                            p = 1;
                        }
                        q = 0;
                        for (int j = 0; j < temp; j++)
                        {
                            if (this.GetNode()[j] == a)
                            {
                                j++;
                                q = 1;
                            }
                            if (p == 0 && q == 0)
                                m[i, j] = data[i, j];
                            if (p == 1 && q == 0)
                                m[i - 1, j] = data[i, j];
                            if (p == 0 && q == 1)
                                m[i, j - 1] = data[i, j];
                            if (p == 1 && q == 1)
                                m[i - 1, j - 1] = data[i, j];
                        }
                    }

                    p = 0;
                    for (int i = 0; i < temp; i++)
                    {
                        if (this.GetNode()[i] == a)
                        {
                            i++;
                            p = 1;
                        }
                        if (p == 0)
                            arr[i] = node[i];
                        if (p == 1)
                            arr[i - 1] = node[i];

                    }
                }
                if (this.GetNode()[temp-1] == a)
                {
                    for (int i = 0; i < n; i++)
                        for (int j = 0; j < n; j++)
                        {
                            m[i, j] = data[i, j];
                        }
                    for (int i = 0; i < n; i++)
                    {
                        arr[i] = node[i];
                    }
                }
                Graph grp = new Graph();
                grp.count = n;
                grp.data = m;
                grp.node = arr;


                return grp;
            }
            else
                return null;
        }             //删除一个节点，返回一个图
       
        public void Short()
        {
           // int[,] data = this.data;

            int min = 0;
            int total = 0;
            for (int i = 0; i < count; i++)
            {
                min = data[i, 0];
                for (int j = 1; j < count; j++)
                {
                    if (data[i, j] < min)
                        min = data[i, j];
                }
                for (int j = 0; j < count; j++)
                {
                    if(min<100)
                    data[i, j] = data[i, j] - min;
                }
                if(min<100)
                total = total + min;
            }
            for (int j = 0; j < count; j++)
            {
                min = data[0, j];
                for (int i = 1; i < count; i++)
                {
                    if (data[i, j] < min)
                        min = data[i, j];
                }
                for (int i = 0; i < count; i++)
                {
                    if(min<100)
                    data[i, j] = data[i, j] - min;
                }
                if(min<100)
                total = total + min;
            }
                this.short_value = total;
          //  Graph temp = new Graph();
           // temp.count = this.count;
           // temp.data = data;

        }             //对当前图进行简约
        public int Get_short_value()
        {
            return this.short_value;
        }       //获取当前的简约值
       
        

    }
}
