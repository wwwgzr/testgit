using System;
using System.Drawing;
using System.Collections.Generic;

namespace CommonModule
{
    public class HamiltonianPath
    {
        static int V = 5;
        /* A utility function to check if the vertex v can be added at
index 'pos' in the Hamiltonian Cycle constructed so far (stored
in 'path[]') */
        private static bool IsSafe(int v, float[,] graph, int[] path, int pos)
        {
            /* Check if this vertex is an adjacent vertex of the previously
	added vertex. */
            if (graph[path[pos - 1], v] == 0)
                return false;

            /* Check if the vertex has already been included.
            This step can be optimized by creating an array of size V */
            for (int i = 0; i < pos; i++)
                if (path[i] == v)
                    return false;

            return true;
        }


        /* A recursive utility function to solve hamiltonian cycle problem */
        static bool hamCycleUtil(float[,] graph, int[] path, int pos)
        {
            /* base case: If all vertices are included in Hamiltonian Cycle */
            if (pos == V)
            {
                // And if there is an edge from the last included vertex to the
                // first vertex
                if (graph[path[pos - 1], path[0]] > 0)
                    return true;
                else
                    return false;
            }

            // Try different vertices as a next candidate in Hamiltonian Cycle.
            // We don't try for 0 as we included 0 as starting point in in hamCycle()
            for (int v = 1; v < V; v++)
            {
                /* Check if this vertex can be added to Hamiltonian Cycle */
                if (IsSafe(v, graph, path, pos))
                {
                    path[pos] = v;

                    /* recur to construct rest of the path */
                    if (hamCycleUtil(graph, path, pos + 1) == true)
                        return true;

                    /* If adding vertex v doesn't lead to a solution,
                    then remove it */
                    path[pos] = -1;
                }
            }

            /* If no vertex can be added to Hamiltonian Cycle constructed so far,
            then return false */
            return false;
        }


        /* This function solves the Hamiltonian Cycle problem using Backtracking.
        It mainly uses hamCycleUtil() to solve the problem. It returns false
        if there is no Hamiltonian Cycle possible, otherwise return true and
        prints the path. Please note that there may be more than one solutions,
        this function prints one of the feasible solutions. */
        static bool hamCycle(float[,] graph)
        {
            int[] path = new int[V];
            for (int i = 0; i < V; i++)
                path[i] = -1;

            /* Let us put vertex 0 as the first vertex in the path. If there is
            a Hamiltonian Cycle, then the path can be started from any point
            of the cycle as the graph is undirected */
            path[0] = 0;
            if (hamCycleUtil(graph, path, 1) == false)
            {
                Console.WriteLine("Solution does not exist");
                return false;
            }

            printSolution(path);
            return true;
        }

        /* A utility function to print solution */
        static void printSolution(int[] path)
        {
            Console.WriteLine("Solution Exists: Following is one Hamiltonian Cycle \n");
            for (int i = 0; i < V; i++)
                Console.WriteLine(path[i]);

            // Let us print the first vertex again to show the complete cycle
            Console.WriteLine(path[0]);
            Console.WriteLine("");
        }


        public static void GetGraph(PointCut[] pointCut)
        {
            if (pointCut == null || pointCut.Length < 2)
                return;
            V = pointCut.Length * 2;
            float[,] graph1 = new float[V,V];
            //{ 0, 2, 0, 3, 0 },
            //{ 2, 0, 2, 3, 3 },
            //{ 0, 2, 0, 0, 3 },
            //{ 3, 3, 0, 0, 4},
            //{ 0, 3, 3, 4, 0 },
            for (int i = 0; i < V; i += 2)
            {
                for (int j = i; j < V; j += 2)
                {
                    if(i == j)
                    {
                        graph1[i, j] = 0;
                        graph1[i + 1, j + 1] = 0;
                        graph1[i, j + 1] = 0.01f;
                        graph1[j + 1, i] = 0.01f;
                    }
                    else
                    {
                        float d1 = (float)GraphicsMath.ComputeDistance(pointCut[i / 2].FiristPoint, pointCut[j / 2].FiristPoint);
                        float d2 = (float)GraphicsMath.ComputeDistance(pointCut[i / 2].FiristPoint, pointCut[j / 2].LastPoint);
                        float d3 = (float)GraphicsMath.ComputeDistance(pointCut[i / 2].LastPoint, pointCut[j / 2].FiristPoint);
                        float d4 = (float)GraphicsMath.ComputeDistance(pointCut[i / 2].LastPoint, pointCut[j / 2].LastPoint);

                        //1F -- 2F
                        graph1[i, j] = d1;
                        graph1[j, i] = d1;
                        //1F -- 2L
                        graph1[i, j + 1] = d2;
                        graph1[j + 1, i] = d2;
                        //1L -- 2F
                        graph1[i + 1, j] = d3;
                        graph1[j, i + 1] = d3;
                        //1L -- 2L
                        graph1[i + 1, j + 1] = d4;
                        graph1[j + 1, i + 1] = d4;
                    }
                }
            }
            string str = "";
            for (int i = 0; i < V; i++)
            {
                for (int j = 0; j < V; j++)
                {
                    str += ((int)graph1[i,j]).ToString("00000") + " ";
                }
                str += "\r\n";
            }
            Console.WriteLine(str);
            hamCycle(graph1);
        }

        /// <summary>
        /// 按图形最后一点到其他图形首尾点坐标最短距离 返回路径
        /// </summary>
        /// <param name="pointCut"></param>
        public static void GetPathArrayByDistance(PointCut[] pointCut, int index, List<int> orderList)
        {
            double minDis = double.MaxValue;
            int minIndex = -1;
            for (int j = 0; j < pointCut.Length; j++)
            {
                if (j != index && !pointCut[j].IsVisit)
                {
                    double d3 = GraphicsMath.ComputeDistance(pointCut[index].LastPoint, pointCut[j].FiristPoint);
                    double d4 = GraphicsMath.ComputeDistance(pointCut[index].LastPoint, pointCut[j].LastPoint);
                    if (minDis > d3)
                    {
                        minDis = d3;
                        minIndex = j;
                        pointCut[j].IsReserve = false;
                        if (d3 <= float.Epsilon)
                            break;
                    }
                    if (minDis > d4)
                    {
                        minDis = d4;
                        minIndex = j;
                        pointCut[j].IsReserve = true;
                        if (d4 <= float.Epsilon)
                            break;
                    }
                }
            }
            if (minIndex >= 0)
            {
                pointCut[minIndex].IsVisit = true;
                if (pointCut[minIndex].IsReserve)
                    pointCut[minIndex].Reserve();
                orderList.Add(minIndex);
                GetPathArrayByDistance(pointCut, minIndex, orderList);
            }
            else
            {

            }
        }
        public static int[] GetPath(float[,] graph)
        {
            hamCycle(graph);
            return null;
        }
        // driver program to test above function
        int main()
        {
            /* Let us create the following graph
            (0)--(1)--(2)
            |   / \   |
            |  /   \  |
            | /     \ |
            (3)-------(4)    */
            float[,] graph1 = { 
	{ 0, 2, 0, 3, 0 },
	{ 2, 0, 2, 3, 3 },
	{ 0, 2, 0, 0, 3 },
	{ 3, 3, 0, 0, 4},
	{ 0, 3, 3, 4, 0 },
	};

            // Print the solution
            hamCycle(graph1);

            /* Let us create the following graph
            (0)--(1)--(2)
            |   / \   |
            |  /   \  |
            | /     \ |
            (3)       (4)    */
            float[,] graph2 = {
	{ 0, 1, 0, 1, 0 },
	{ 1, 0, 1, 1, 1 },
	{ 0, 1, 0, 0, 1 },
	{ 1, 1, 0, 0, 0 },
	{ 0, 1, 1, 0, 0 },
	};

            // Print the solution
            hamCycle(graph2);
            return 0;
        }
    }


    public class PointCut
    {
        public PointF[] ptList;

        public object parent;
        private bool isReserve = false;

        public bool IsReserve
        {
            get { return isReserve; }
            set { isReserve = value; }
        }
        private bool isVisit = false;

        public bool IsVisit
        {
            get { return isVisit; }
            set { isVisit = value; }
        }
        public PointF FiristPoint
        {
            get
            {
                if (ptList != null && ptList.Length > 0)
                    return ptList[0];
                else
                    return PointF.Empty;
            }
        }
        public PointF LastPoint
        {
            get
            {
                if (ptList != null && ptList.Length > 0)
                    return ptList[ptList.Length - 1];
                else
                    return PointF.Empty;
            }
        }
        public string id = Guid.NewGuid().ToString();

        public void Reserve()
        {
            if(ptList != null && ptList.Length > 1)
            {
                if (!ptList[0].Equals(ptList[ptList.Length - 1]))
                {
                    PointF[] ptemp = new PointF[ptList.Length];
                    for (int i = 0; i < ptemp.Length; i++)
                    {
                        ptemp[i] = ptList[ptemp.Length - i - 1];
                    }
                    ptList = ptemp;
                }
            }
        }
    }
}
