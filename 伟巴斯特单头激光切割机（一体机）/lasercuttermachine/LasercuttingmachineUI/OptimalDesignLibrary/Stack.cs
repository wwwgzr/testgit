using System;
using System.Collections.Generic;
using System.Text;

namespace OptimalDesignLibrary
{
    class Stack
    {
        private int top;                 //栈顶指针
        private int num;            //栈的大小
        private GNode []data ;            //栈中数据为节点对象的集合
        
        public Stack()
        {
            top = -1;
            num = 0;
            data = new GNode [10000000];
        }
        public int GetTop()
        {
            return this.top;
        }
      
        public int GetNum()
        {
            return this.num;
        }
        public void Push(GNode i)
        {
            top++;
            num++;
            data[top] = i;
        }
        public GNode Pop()
        {
            int temp=this.top;
            num--;
            top--;
            return data[temp]; 
        }
        public bool Empty()
        {
            if (this.num == 0)
                return true;
            else
                return false;
        }
        public bool Check_Do()
        {
            for (int i = 0; i < this.num; i++)
            {
                if (data[i].Can_Do() == false)
                    return false;
            }
           
             return true;
        }                    //判断该堆栈内的所以元素是否都可以扩展
        public void SetEmpty()
        {
            this.top = -1;
            this.num = 0;
        }
    }
}
