using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CSharpLoops
{
    class MatrixMultiplication
    {
        const int MatrixSize = 1000;

        public double[,] GenerateMatrixV1(int n)
        {
            double[,] a = new double[n, n];

            double tmp = 1.0 / n / n;
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    a[i, j] = tmp * (i - j) * (i + j);
            return a;
        }

        double[,] MultiplyMatrix(double[,] a, double[,] b)
        {
            int m = a.GetLength(0), n = b.GetLength(1), p = a.GetLength(1);
            double[,] x = new double[m, n]; // result
            double[,] c = new double[n, p];

            for (int i = 0; i < p; ++i) // transpose
                for (int j = 0; j < n; ++j)
                    c[j, i] = b[i, j];

            for (int i = 0; i < m; ++i)
                for (int j = 0; j < n; ++j)
                {
                    double s = 0.0;
                    for (int k = 0; k < p; ++k)
                        s += a[i, k] * c[j, k];
                    x[i, j] = s;
                }
            return x;
        }

        public object Run()
        {
            double[,] a, b, x;
            a = GenerateMatrixV1(MatrixSize);
            b = GenerateMatrixV1(MatrixSize);
            x = MultiplyMatrix(a, b);
            return x[MatrixSize / 2, MatrixSize / 2];
        }
    }
}
