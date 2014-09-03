using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CSharpLoops
{
    class MatrixMultiplication
    {
        const int MatrixSize = 1024;

        public static float[,] GenerateMatrixV1(int n)
        {
            float[,] a = new float[n, n];

            float tmp = 1.0f / n / n;
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    a[i, j] = tmp * (i - j) * (i + j);
            return a;
        }

        static float[,] MultiplyMatrix(float[,] a, float[,] b)
        {
            int m = a.GetLength(0), n = b.GetLength(1), p = a.GetLength(1);
            float[,] x = new float[m, n]; // result
            float[,] c = new float[n, p];

            for (int i = 0; i < p; ++i) // transpose
                for (int j = 0; j < n; ++j)
                    c[j, i] = b[i, j];

            for (int i = 0; i < m; ++i)
                for (int j = 0; j < n; ++j)
                {
                    float s = 0.0f;
                    for (int k = 0; k < p; ++k)
                        s += a[i, k] * c[j, k];
                    x[i, j] = s;
                }
            return x;
        }

        public static object Test()
        {
            float[,] a, b, x;
            a = GenerateMatrixV1(MatrixSize);
            b = GenerateMatrixV1(MatrixSize);
            x = MultiplyMatrix(a, b);
            return x[MatrixSize / 2, MatrixSize / 2];
        }
    }
}
