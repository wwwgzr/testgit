using System;
using System.Collections.Generic;
using System.Text;

namespace OptimalDesignLibrary
{
    class Generation
    {
        private Bee[] data=new Bee[4];                   //当前一代的个体集合
        private int[] live_num = new int[4];            //当前一代的个体生存的个数

        private int sum_adapt_value=0;                 
        private float sum_adapt = 0F;
        
        private int aver_adapt_value=0;
        private float aver_adapt = 0F;

        public Generation()
        {
            data = new Bee[4];
            live_num = new int[4];
        }
        public float Get_sum_adapt()
        {
            return this.sum_adapt;
        }
        public void Set_sum_adapt(float n)
        {
            this.sum_adapt = n;
        }
        public int Get_sum_adapt_value()
        {
            return this.sum_adapt_value;
        }
        public void Set_sum_adapt_value(Bee[] test)
        {
            int total = 0;
            for (int i = 0; i < 4; i++)
            {
                total += test[i].Get_Adapt_value();
            }
            this.sum_adapt_value = total;
            
        }
        public void Set_sum_adapt(Bee[] test)
        {
            float total = 0;
            for (int i = 0; i < 4; i++)
            {
                total += test[i].Get_Adapt();
            }
            this.sum_adapt = total;
        }
       
        public Generation(Bee[] bee)
        {
            data = new Bee[4];
            for (int i = 0; i < 4; i++)
            {
                //data[i] = bee[i];
                data[i] = new Bee(bee[i]);
                this.sum_adapt_value +=data[i].Get_Adapt_value();
                this.sum_adapt += data[i].Get_Adapt();
            }
            this.aver_adapt_value = sum_adapt_value / 4;
            this.aver_adapt = sum_adapt / 4;
        }
        public Bee[] Get_Bee()
        {
            return this.data;
        }
        public Generation Select_NextGeneration()
        {
            Generation temp = new Generation();
            float[] live = new float[4];
            for (int i = 0; i < 4; i++)
            {
                live[i] = 4 * Convert.ToSingle( this.data[i].Get_Adapt()) / Convert.ToSingle( this.sum_adapt);
                
                //live_num[i] = Convert.ToInt32(live[i]);
                if (live[i] < 0.5)
                    live_num[i] = 0;
                if (0.5 <= live[i] && live[i] < 1.5)
                    live_num[i] = 1;
                if (1.5 <= live[i])
                    live_num[i] = 2;
            }
            for (int j = 0; j < 4; j++)
            {
                Console.WriteLine(live[j].ToString());
            }
           for (int i = 0; i < 4; i++)
            {
                Console.WriteLine(live_num[i].ToString());
            }
             int num= 0;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < live_num[i]; j++)
                {
                    temp.data[num] = new Bee(this.data[i]);
                  //  MessageBox.Show(temp.data[num].Get_solu()[4].ToString());
                    num++;
                }
            }
            temp.Set_sum_adapt(temp.Get_Bee());
            temp.Set_sum_adapt_value(temp.Get_Bee());
         return temp;
            /*
           int n = temp.Get_Bee()[0].GetN();
            int n1 = 5;
            int n2 = 7;
            int temp1 = 0;
           
            temp.data[0].Change(temp.data[1]);

            temp.Set_sum_adapt(temp.Get_Bee());
            temp.Set_sum_adapt_value(temp.Get_Bee());
                return temp;
             * */
            
        }
      
       /* public void exchange()
        {
           // Bee[] temp = new Bee[4];
            int n = this.data[0].GetN();
            MessageBox.Show("n:" + n.ToString());
            int n1 = 3;
            int n2 = 6;
            int temp1 = 0;
           
                for (int j =  n- 1; j > n - 1 - n1; j--)
                {
                    temp1 = this.data[0].Get_solu()[j];
                    this.data[0].Get_solu()[j] = this.data[3].Get_solu()[j];
                    this.data[3].Get_solu()[j] = temp1;
                }

                for (int j = n - 1; j > n - 1 - n2; j--)
                {
                    temp1 = this.data[1].Get_solu()[j];
                    this.data[1].Get_solu()[j] = this.data[2].Get_solu()[j];
                    this.data[2].Get_solu()[j] = temp1;
                }
        }*/
    } 
}
 