using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CSharpLoops
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("C# Tests");

            var stopwatch = new Stopwatch();

            Console.WriteLine("Starting Matrix Multiplication...");

            stopwatch.Start();

            MatrixMultiplication.Test();

            stopwatch.Stop();

            Console.WriteLine("Matrix Multiplication Completed in: {0}", stopwatch.ElapsedMilliseconds / 1000.0);

            Console.WriteLine("Starting Polynomial...");

            stopwatch.Restart();

            Polynomial.Test();

            stopwatch.Stop();

            Console.WriteLine("Polynomial Completed in: {0}", stopwatch.ElapsedMilliseconds / 1000.0);

        }
    }
}
