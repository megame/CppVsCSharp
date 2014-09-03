using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CSharpLoops
{
    class Polynomial
    {
        const int Iterations = 10000000;

        static float dopoly(float x, float[] pol)
        {
            float mu = 10.0f;
            float s;
            int j;

            for (j = 0; j < 100; j++)
                pol[j] = mu = (mu + 2.0f) / 2.0f;
            s = 0.0f;
            for (j = 0; j < 100; j++)
                s = x * s + pol[j];
            return s;
        }

        public static object Test()
        {
            float x = 0.2f;
            float pu = 0.0f;
            float[] pol = new float[100];

            for (int i = 0; i < Iterations; i++)
                pu += dopoly(x, pol);
            return pu;
        }
    }
}
