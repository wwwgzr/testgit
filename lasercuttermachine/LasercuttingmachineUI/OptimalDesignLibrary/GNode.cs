using System;
using System.Collections.Generic;
using System.Text;

namespace OptimalDesignLibrary
{
    class GNode
    {
        private int i;                           //   当前所在的城市位置
        private Graph grp;                      //   剩余城市节点组成的图
        private int value;               //       当前对象的距离
        private int min_j;               //       使当前对象值最小的下一个城市节点位置
        private int near;                  //
        private GNode[] son;            //      当前对象发展的子对象，个数为当前对象的图中的城市节点的个数

        public GNode()    
        {
            
            grp = new Graph();
            //son=new GNode[];
        }
        public GNode(int i,Graph grp)
        {
            this.i = i;                   
            this.grp = new Graph(grp);
            this.value = 0;
            this.min_j = 0;
            this.near = 0;
            this.son = new GNode[this.grp.GetCount()];

        }               //构造函数，通过当前所在的城市节点位置以及剩下的节点组成的图生成一个对象
        public Graph GetGraph()             //获取当前对象的图属性
        {
            return this.grp;
        }
        public void GiveSon()               //设置当前对象的子对象
        {
            this.son = new GNode[this.grp.GetCount()];
        }
        public int GetI()               //获取当前对象的所在城市的位置
        {
            return this.i;
        }
        public int GetNear()
        {
            return this.near;
        }
        public GNode[] GetSon()
        {
            int count=this.grp.GetCount();
            GNode []node=new GNode[count];
            for (int i = 0; i < grp.GetCount(); i++)
            {
                node[i] = this.son[i];
            }
            return node;
        }                //   获取当前对象生成的子对象集合
        public int GetValue()
        {
            return this.value;
        }                   //获取当前对象的值
        public void SetValue(int i)
        {
            this.value = i;
        }             //设置当前对象的值
        public void SetMin_J(int i)
        {
            this.min_j = i;
        }                //设置min_j值
        public int GetMin_J()
        {
            return this.min_j;
        }                        //获取min_j值
        public GNode [] GNode_Do()
        {
            int count=this.grp.GetCount();
            GNode [] p=new GNode[count];
            int [] arr=this.grp.GetNode();
            for(int i=0;i<count;i++)
            {
                GNode node=new GNode();
                node.i=arr[i];
                node.grp=this.grp.DelNode(arr[i]);
                node.near = this.i;
                p[i]=node;
                this.son[i] = node;
            }
            return p;
        }                                      //对当前对象进行扩展，并返回值为子对象集合
        public bool Can_Do()
        {
            if (this.GetGraph().GetCount() == 0)
                return false;
            else
                return true;
        }             //判断当前对象可否进行扩展
       

    }
}
