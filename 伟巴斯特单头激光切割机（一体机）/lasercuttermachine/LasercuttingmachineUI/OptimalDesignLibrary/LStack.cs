using System;
using System.Collections.Generic;
using System.Text;

namespace OptimalDesignLibrary
{
    class LStack
    {
        private int top = -1;
        private int num = 0;
        private LNode[] data;

        public LStack()
        {
            this.top = -1;
            num = 0;
           data = new LNode[100000000];

        }
        public int GetTop()
        {
            return this.top;
        }

        public int GetNum()
        {
            return this.num;
        }
        public void Push(LNode i)
        {
            top++;
            num++;
            data[top] = i;
        }
        public LNode Pop()
        {
            int temp = this.top;
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
        public LNode Pop_min(LStack stc)
        {
          //  LStack stc=new LStack;
         //   stc=this;
           
                LNode temp = new LNode();
                temp = new LNode(data[0]);
                int min = 0;
                for (int i = 1; i <= stc.top; i++)
                {
                    if (stc.data[i].Get_value() < temp.Get_value())
                    {

                        min = i;
                        temp =data[i];
                    }
                }
           //   MessageBox.Show("min_i:" + min.ToString());
                LNode node = new LNode(data[min]);
              node.Set_Parent(data[min].Get_parent());
              data[min].Set_value(1000);
              return node;
                
           
           
        }
        public void SetEmpty()
        {
            this.top = -1;
            this.num = 0;
        }
        public void Show()
        {
            for (int i = 0; i < num; i++)
            {
                Console.WriteLine(data[i].Get_value().ToString());
            }
        }
      
    }
}
