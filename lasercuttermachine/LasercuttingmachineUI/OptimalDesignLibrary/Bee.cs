using System;
using System.Collections.Generic;
using System.Text;

namespace OptimalDesignLibrary
{
    class Bee
    {
        private  int[] solu ;
        private int adapt_value=0;
        private float adapt = 0F;

        private int N;

        public Bee(int i)
        {
            solu = new int [i];
           // int i = N;
            this.N = i;
            solu[i- 1] = 0;
        }
        public Bee(Bee bee)
        {
            this.N = bee.N;
            this.solu = bee.solu;
            this.adapt = bee.adapt;
            this.adapt_value = bee.adapt_value;
        }
        public Bee(int[] str,Graph grp,int n)
        { 
            solu = new int [n];
            Graph temp=new Graph(grp);
            this.N = n;
            for (int i = 0; i < N; i++)
            {
                solu[i] = str[i];
            }
            int[] length = new int[n];
            int p=0;
            int q=0;
            temp=temp.DelNode(0);
            int total = 0;
            for (int i = 0; i < N-1; i++)
            {
                    q= str[i];
                    q = temp.GetNode()[q];

                    temp = temp.DelNode(q);
                    length[i] = grp.GetData()[p, q];
                    total += length[i];
                    p = q;                
            }
          //  int data = grp.GetData()[p, 0];

            length[N - 1] = grp.GetData()[p, 0];
            total += length[N - 1];
            this.adapt_value = total;
            this.adapt = (Convert.ToSingle(n) / total) * (Convert.ToSingle(n) / total) * (Convert.ToSingle(n) / total);
        }

        public void Set_Adatpt(float n)
        {
            this.adapt = n;
        }
        public float Get_Adapt()
        {
            return this.adapt;
        }
        public void Set_Adapt_value(int i)
        {
            this.adapt_value = i;
           
        }
        public int Get_Adapt_value()
        {
            return this.adapt_value;
        }
        public int GetN()
        {
            return this.N;
        }
        public void SetN(int i)
        {
            this.N = i;
        }
        public int[] Get_solu()
        {
            int[] p = new int[this.N];
            for (int i = 0; i < N; i++)
                p[i] = solu[i];
            return p;
        }
        public void Set_solu(int[] p, int n)
        {
            for (int i = 0; i < n; i++)
            {
                this.solu[i] = p[i];
            }
        }
        public void Change(Bee bee)
        {
            Random rdm = new Random();
            int i = rdm.Next(bee.GetN()-1);
            int temp = this.solu[i];
            this.solu[i] = bee.solu[i];
            bee.solu[i] = temp;
        }
    }
}
