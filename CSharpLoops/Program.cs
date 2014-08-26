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
            Console.WriteLine("C#:");
            Console.WriteLine("Generating buffer...");
            var memory = GenerateBuffer();

            var stopwatch = new Stopwatch();

            Console.WriteLine("Starting...");

            stopwatch.Start();

            for (int i = 0; i < 10; i++)
            {
                ProcessBuffer(memory);
            }

            stopwatch.Stop();

            Console.WriteLine("Completed in: {0}", stopwatch.Elapsed);
        }

        static byte[] GenerateBuffer()
        {
            var size = 1024 * 100000;

            Console.WriteLine("Buffer size: {0}", size);

            var buffer = new byte[size];

            for (int i = 0; i < buffer.Length; i++)
                buffer[i] = (byte)(i % byte.MaxValue);

            return buffer;
        }

        static void ProcessBuffer(byte[] memory)
        {
            for(int i=0; i<(memory.Length - 8); i+=4)
            {
                var rr = memory[i + 0] * memory[i + 4];
                var rg = memory[i + 1] * memory[i + 5];
                var rb = memory[i + 2] * memory[i + 6];

                memory[i + 0] = (byte) (rr * 0.393 + rg * 0.769 + rb * 0.189);
                memory[i + 1] = (byte) (rr * 0.349 + rg * 0.686 + rb * 0.168);
                memory[i + 2] = (byte) (rr * 0.272 + rg * 0.534 + rb * 0.131);
            }
        }

    }
}
