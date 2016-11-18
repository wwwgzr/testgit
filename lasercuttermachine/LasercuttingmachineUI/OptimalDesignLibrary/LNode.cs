using System;
using System.Collections.Generic;
using System.Text;

namespace OptimalDesignLibrary
{
    class LNode
    {
        private int near;               //离当前节点位置最近的节点
        private int value;              //  当前节点的权值
        private Graph grp;              //  当前节点的图属性
        private int now;                // 当前节点所在的位置
        private int[] next;             // 剩下节点组成的数组
        private int next_num;            //剩下节点的大小
        private LNode parent;              //当前节点的父节点

        public LNode()              //构造函数
        {
            near = 0;
            value = 0;
            grp = new Graph();
            now = 0;
        }
        public LNode(LNode node)                //拷贝构造函数
        {
            this.near = node.near;
            this.value = node.value;
            this.now = node.now;
            this.next = node.next;
            this.next_num = node.next_num;
           
            this.grp = new Graph(node.grp);

           // this.parent = node.parent;
        }
        public LNode(Graph grp)                 
        {
            this.grp = new Graph();
            this.grp = grp;
        } //  通过一个图构造一个节点
        public void Set_Parent(LNode node)
        {
            this.parent = node;
        }            //设置父节点
        public LNode Get_Parent()
        {
            return this.parent;
        }                    //获取父节点
        public void Set_next(int []p,int n)
        {
            next = new int[n];
            for (int i = 0; i < n; i++)
            {
                next[i] = p[i];
            }
            this.next_num = n;
        }               //设置next数组
        public int[] Get_next()
        {
            return this.next;
        }                         //获取next数组
        public int Get_next_num()
        {
            return this.next_num;
        }                       //获取next_num
        public void ArrDel(int data)
        {
            int n = this.next_num;
            int[] temp = new int[n-1];
            if (next[n - 1] != data)
            {
                int p = 0;

                for (int i = 0; i < n; i++)
                {
                    if (next[i] == data)
                    {
                        i++;
                        p = 1;
                    }
                    if (p == 0)
                        temp[i] = next[i];
                    if (p == 1)
                        temp[i - 1] = next[i];
                }
            }
            else if (next[n - 1] == data)
            {
                for (int i = 0; i < n - 1; i++)
                {
                    temp[i] = next[i];
                }
            }
            this.next = temp;
            
            this.next_num = next_num - 1;
          //  return temp;
        }                     //在next数组中删除一个指定的数
        public void Set_now(int i)
        {
            this.now = i;
        }                           //设置now属性
        public int Get_now()
        {
            return this.now;
        }                                 //获取now
        
        public void Set_near(int i)
        {
            this.near = i;
        }                             //设置near
        public int Get_near()
        {
            return this.near;
        }                              //获取near
        public LNode Get_parent()
        {
            return this.parent;
        }                             
        public void Set_value(int i)
        {
            this.value = i;
        }                      //设置当前的权值
        public int Get_value()
        {
            return this.value;
        }                              //获取权值
        public void Set_graph(Graph grp)
        {
            this.grp = grp;
        }                     //设置当前的图属性
        public Graph Get_graph()
        {
            return this.grp;
        }                              //获取当前的图属性
        public LNode Get_nextnode(int  n)
        {
                LNode node = new LNode();
           
                int temp = this.grp.GetData()[now, n];
                Graph temp_grp = new Graph(this.grp);
               
                for (int i = 0; i < grp.GetCount(); i++)
                {
                    temp_grp.GetData()[now, i] = 10000;
                    temp_grp.GetData()[i,n] = 10000;
                }
                temp_grp.GetData()[n, 0] = 10000;
                temp_grp.Short();
                temp = temp + temp_grp.Get_short_value();
                temp = temp + this.value;

                node.parent = this;
                node.now = n;
                node.near = this.now;
                node.grp = new Graph(temp_grp);
                node.value = temp;
                
                 node.next_num = this.next_num ;
              //   MessageBox.Show(node.next_num.ToString());
                node.next = this.next;
                node.ArrDel(n);
                   

                return node;
        }                      //获取下一个指定节点，返回一个节点
        public bool Can_Do()
        {
            if(this.next_num!=0)
            {
                return true;
            }
            else 
                return false;
        }                                   //判断当前节点是否还可以扩展



    }
}
