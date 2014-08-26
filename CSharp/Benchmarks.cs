using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using Loyc.Math;
using System.IO;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Runtime.CompilerServices;

namespace Benchmark
{
	public class Benchmarks : INoOp
	{
		public void INoOp() { }
		
		#if CompactFramework
		const int Iterations = 1000000;
		#else
		const int Iterations = 10000000;
		#endif
		const int IterationsX10 = Iterations * 10; // use more iterations for the simplest, fastest tests
		Benchmarker _b;
		Random _r = new Random();

		public Benchmarks(Benchmarker b) { _b = b; }

        //#region Hashtables

        //#if CompactFramework
        //const int MapSizeLimit = Iterations / 4; // CE has per-process heap size limit, preventing a huge dictionary
        //#else
        //const int MapSizeLimit = Iterations / 2; // Desktop can do more, but 50% misses is still a reasonable test
        //#endif

        //[Benchmark("Big int Dictionary", 8)]
        //public object IntDictionaryTests()
        //{
        //    var _dict = new Dictionary<int, int>();
			
        //    _b.MeasureAndRecord("1 Adding items", () =>
        //    {
        //        var dict = _dict;
        //        for (int i = 0; i < Iterations; i++) {
        //            if (dict.Count >= MapSizeLimit)
        //                dict.Clear();
        //            dict[i ^ 314159] = i;
        //        }
        //    });
        //    _b.MeasureAndRecord("2 Running queries", () =>
        //    {
        //        var dict = _dict;
        //        int misses = 0, value;
        //        for (int i = 0; i < Iterations; i++)
        //            if (!dict.TryGetValue(i ^ 314159, out value))
        //                misses++;
        //        return string.Format("{0}% misses", misses * 100 / Iterations);
        //    });
        //    _b.MeasureAndRecord("3 Removing items", () =>
        //    {
        //        var dict = _dict;
        //        int removed = 0;
        //        for (int i = 0; i < Iterations; i++)
        //            if (dict.Remove(i ^ 314159))
        //                removed++;
        //        return string.Format("{0} removed", removed);
        //    });
			
        //    return Benchmarker.DiscardResult;
        //}

        //[Benchmark("Big string Dictionary", 3)]
        //public object StringDictionaryTests()
        //{
        //    var _dict = new Dictionary<string, string>();

        //    _b.MeasureAndRecord("0 Ints to strings", () =>
        //    {
        //        // This just measures how long it takes to generate the strings used 
        //        // in the rest of the tests, in case you would like to mentally 
        //        // subtract this time from the results.
        //        for (int i = 0; i < Iterations; i++)
        //            i.ToString();
        //    });
        //    _b.MeasureAndRecord("1 Adding/setting", () =>
        //    {
        //        var dict = _dict;
        //        for (int i = 0; i < Iterations; i++)
        //        {
        //            if (dict.Count >= MapSizeLimit)
        //                dict.Clear();
        //            string s = (i ^ 314159).ToString();
        //            dict[s] = s;
        //        }
        //    });
        //    _b.MeasureAndRecord("2 Running queries", () =>
        //    {
        //        var dict = _dict;
        //        int misses = 0;
        //        for (int i = 0; i < Iterations; i++)
        //        {
        //            string s = (i ^ 314159).ToString();
        //            if (!dict.TryGetValue(s, out s))
        //                misses++;
        //        }
        //        return string.Format("{0}% misses", misses * 100 / Iterations);
        //    });
        //    _b.MeasureAndRecord("3 Removing items", () =>
        //    {
        //        var dict = _dict;
        //        int removed = 0;
        //        for (int i = 0; i < Iterations; i++)
        //        {
        //            string s = (i ^ 314159).ToString();
        //            if (dict.Remove(s))
        //                removed++;
        //        }
        //        return string.Format("{0} removed", removed);
        //    });

        //    return Benchmarker.DiscardResult;
        //}

        //#endregion

        //#region Map (SortedDictionary)
        //#if !CompactFramework // Lacks SortedDictionary

        //[Benchmark("Big int sorted map")]
        //public object IntMapTests()
        //{
        //    var _dict = new SortedDictionary<int, int>();

        //    _b.MeasureAndRecord("1 Adding items", () =>
        //    {
        //        var dict = _dict;
        //        for (int i = 0; i < Iterations; i++)
        //        {
        //            if (dict.Count >= MapSizeLimit)
        //                dict.Clear();
        //            dict[i ^ 314159] = i;
        //        }
        //    });
        //    _b.MeasureAndRecord("2 Running queries", () =>
        //    {
        //        var dict = _dict;
        //        int misses = 0, value;
        //        for (int i = 0; i < Iterations; i++)
        //            if (!dict.TryGetValue(i ^ 314159, out value))
        //                misses++;
        //        return string.Format("{0}% misses", misses * 100 / Iterations);
        //    });
        //    _b.MeasureAndRecord("3 Removing items", () =>
        //    {
        //        var dict = _dict;
        //        int removed = 0;
        //        for (int i = 0; i < Iterations; i++)
        //            if (dict.Remove(i ^ 314159))
        //                removed++;
        //        return string.Format("{0} removed", removed);
        //    });

        //    return Benchmarker.DiscardResult;
        //}

        //#endif
        //#endregion

        //#region Square roots

        //[Benchmark("Square root", 6)]
        //public object Roots()
        //{
        //    long totalI = 0, totalL = 0;
        //    double totalD = 0;
        //    const int Base = 999999;

        //    _b.MeasureAndRecord("double", () =>
        //    {
        //        double total = 0;
        //        for (double i = Base; i < Base + Iterations; i++)
        //            total += Math.Sqrt(i);
        //        totalD = total;
        //    });
        //    _b.MeasureAndRecord("uint", () =>
        //    {
        //        long total = 0;
        //        for (int i = Base; i < Base + Iterations; i++)
        //            total += MathEx.Sqrt((uint)i);
        //        totalI = total; // avoid JIT overoptimization
        //    });
        //    _b.MeasureAndRecord("ulong", () =>
        //    {
        //        long total = 0;
        //        for (int i = Base; i < Base + Iterations; i++)
        //            total += MathEx.Sqrt((ulong)i);
        //        totalL = total;
        //    });
        //    FPL16 totalFP = 0;
        //    _b.MeasureAndRecord("FPL16", () =>
        //    {
        //        FPL16 total = 0;
        //        for (FPL16 i = Base; i < (FPL16)(Base + Iterations); i++)
        //            total += i.Sqrt();
        //        totalFP = total;
        //    });
        //    Debug.Assert(totalI == totalL);
        //    Debug.Assert(totalD > (double)totalI && totalD < (double)(totalI + Iterations));
        //    Debug.Assert((totalFP - (FPL16)totalD).Abs().N < Iterations);
        //    return Benchmarker.DiscardResult;
        //}

        //#endregion

		#region Simple arithmetic

		[Benchmark("Simple arithmetic")]
		public object RelativeArithmeticSpeed()
		{
			long totalI = 0, totalL = 0;
			double totalD = 0;
			float totalF = 0;
			FPI8 totalF8 = 0;
			FPL16 totalFL16 = 0;
			
			// This benchmark is designed so that all tests produce similar totals.
			// FPI8 has limited range (max 8 million); Modulus should be low to avoid overflow.
			const int Modulus = 100000;

			_b.MeasureAndRecord("double", () =>
			{
				double total = 0;
				for (int i = 0; i < IterationsX10; i++)
					total = total - total / 4.0 + (double)i % Modulus * 3.0;
				totalD = total;
			});
			_b.MeasureAndRecord("float", () =>
			{
				float total = 0;
				for (int i = 0; i < IterationsX10; i++)
					total = total - total / 4.0f + (float)(i % Modulus) * 3.0f;
				totalF = total;
			});
			_b.MeasureAndRecord("FPI8", () =>
			{
				FPI8 total = 0;
				for (int i = 0; i < IterationsX10; i++)
					total = total - (total >> 2) + (FPI8)(i % Modulus) * 3;
				totalF8 = total;
			});
			_b.MeasureAndRecord("FPL16", () =>
			{
				FPL16 total = 0;
				FPL16 Modulus2 = (FPL16)Modulus;
				for (int i = 0; i < IterationsX10; i++)
					total = total - (total >> 2) + (FPL16)i % Modulus2 * 3L;
				totalFL16 = total;
			});
			_b.MeasureAndRecord("int", () =>
			{
				int total = 0;
				for (int i = 0; i < IterationsX10; i++)
					total = total - (total >> 2) + i % Modulus * 3;
				totalI = total;
			});
			_b.MeasureAndRecord("long", () =>
			{
				long total = 0;
				for (int i = 0; i < IterationsX10; i++)
					total = total - (total >> 2) + (long)i % Modulus * 3;
				totalL = total;
			});
			Debug.Assert(totalI == totalL);
			Debug.Assert(Math.Abs(totalD - (double)totalI) < 5);
			Debug.Assert(Math.Abs(totalD - (double)totalF8) < 1);
			Debug.Assert(Math.Abs(totalD - (double)totalFL16) < 1);
			return Benchmarker.DiscardResult;
		}

		#endregion

		#region Generic sum

		[Benchmark("Generic sum", 8)]
		public object GenericSum()
		{
			const int ListSize = 500000;
			// Use 10x as many total iterations as usual because the inner loops are so fast
			const int OuterIterations = Iterations / ListSize * 10;
			
			List<int> listI = new List<int>(Enumerable.Range(1, ListSize));
			_b.MeasureAndRecord("int", () =>
			{
				for (int i = 0; i < OuterIterations; i++)
					GenericSum(listI, new MathI());
			});

			_b.MeasureAndRecord("int via IMath", () =>
			{
				for (int i = 0; i < OuterIterations; i++)
					GenericSum<int, IMath<int>>(listI, new MathI());
			});

			List<double> listD = new List<double>(Enumerable.Range(1, ListSize).Select(i => (double)i));
			_b.MeasureAndRecord("double", () =>
			{
				for (int i = 0; i < OuterIterations; i++)
					GenericSum(listD, new MathD());
			});

			List<FPI8> listF8 = new List<FPI8>(Enumerable.Range(1, ListSize).Select(i => (FPI8)i));
			_b.MeasureAndRecord("FPI8", () =>
			{
				for (int i = 0; i < OuterIterations; i++)
					GenericSum(listF8, new MathF8());
			});

			_b.MeasureAndRecord("int without generics", () =>
			{
				for (int i = 0; i < OuterIterations; i++)
					NonGenericSum(listI);
			});
			
			return Benchmarker.DiscardResult;
		}

		public static T GenericSum<T, Math>(List<T> list, Math M) where Math : IMath<T>
		{
			T sum = M.Zero;
			for (int i = 0; i < list.Count; i++)
				sum = M.Add(sum, list[i]);
			return sum;
		}
		public static int NonGenericSum(List<int> list)
		{
			int sum = 0;
			for (int i = 0; i < list.Count; i++)
				sum += list[i];
			return sum;
		}

		#endregion

        //#region Simple parsing

        //TextReader OpenTestDictFile()
        //{
        //    string home = Program.HomePath;
        //    string filename;
        //    while (!File.Exists(filename = Path.Combine(home, "TestDict.txt")))
        //    {
        //        int i = Math.Max(home.LastIndexOf('\\'), home.LastIndexOf('/'));
        //        if (i == -1)
        //            throw new FileNotFoundException("Missing TestDict.txt");
        //        home = home.Substring(0, i);
        //    }
        //    return File.OpenText(filename);
        //}

        //[Benchmark("Simple parsing")]
        //public object SimpleParsing()
        //{
        //    const int Repetitions = 
        //    #if CompactFramework
        //        2;
        //    #else
        //        20;
        //    #endif

        //    var lines = new List<string>();

        //    _b.MeasureAndRecord(string.Format("1 Read to end (x{0})", Repetitions.ToString()), () =>
        //    {
        //        for (int i = 0; i < Repetitions; i++)
        //            using (TextReader reader = OpenTestDictFile())
        //                reader.ReadToEnd();
        //    });

        //    _b.MeasureAndRecord(string.Format("2 Read lines (x{0})", Repetitions.ToString()), () =>
        //    {
        //        for (int i = 0; i < Repetitions; i++) {
        //            lines.Clear();
        //            using (TextReader reader = OpenTestDictFile())
        //            {
        //                string line;
        //                while ((line = reader.ReadLine()) != null)
        //                    lines.Add(line);
        //            }
        //        }
        //    });

        //    var entries = new List<KeyValuePair<string, string>>();

        //    _b.MeasureAndRecord(string.Format("3 Parse (x{0})", Repetitions.ToString()), () =>
        //    {
        //        for (int r = 0; r < Repetitions; r++)
        //        {
        //            entries.Clear();
        //            for (int i = 1; i < lines.Count; i++)
        //            {
        //                string line = lines[i];
        //                if (line == "" || line[0] == ';')
        //                    continue;

        //                string key;
        //                string value;
        //                int index = line.IndexOf(":=");
        //                if (index > 0)
        //                {
        //                    key = line.Substring(0, index);
        //                    value = line.Substring(index + 2);
        //                    while (i + 1 < lines.Count && !lines[i + 1].Contains(":="))
        //                        value += "\n" + lines[++i];
        //                    entries.Add(new KeyValuePair<string, string>(key, value));
        //                }
        //            }
        //        }
        //    });

        //    _b.MeasureAndRecord(string.Format("4 Sort (x{0})", Repetitions.ToString()), () =>
        //    {
        //        SimpleTimer timer = new SimpleTimer();
        //        int waste = 0;

        //        for (int r = 0; r < Repetitions; r++)
        //        {
        //            // Mess up some of the entries, to ensure sorting requires some work.
        //            // Time spent randomizing is tiny, but will be subtracted from the total.
        //            timer.Restart();
        //            for (int i = 0; i < entries.Count; i += 4) {
        //                var j = _r.Next(entries.Count);
        //                var tmp = entries[i];
        //                entries[j] = entries[i];
        //                entries[i] = tmp;
        //            }
        //            waste += timer.Millisec;
						 
        //            entries.Sort((a, b) => string.Compare(a.Key, b.Key, true));
        //        }
        //        return Benchmarker.SubtractOverhead(waste);
        //    });

        //    return Benchmarker.DiscardResult;
        //}

        //#endregion

        //#region P/Invoke

        //// Variables that guarantee that the return value marshaling is not optimized
        //// away (although I suspect that it's never optimized away, anyway).
        //static string _returnedString;
        //static double _returnedDouble;
        //static char _returnedChar;
        //static Point _returnedPoint;

        //[Benchmark("P/Invoke", Trials=4)]
        //public object TestPInvoke()
        //{
        //    _b.MeasureAndRecord("NextInt", () =>
        //    {
        //        SimpleDll.SetNextInt(1);
        //        int sum = 0;
        //        for (int i = 0; i < Iterations; i++)
        //            sum += SimpleDll.NextInt();
        //    });
        //    _b.MeasureAndRecord("AddInts", () =>
        //    {
        //        for (int i = 0; i < Iterations; i++)
        //            SimpleDll.AddInts(i, i);
        //    });
        //    _b.MeasureAndRecord("AddFourInts", () =>
        //    {
        //        for (int i = 0; i < Iterations; i++)
        //            SimpleDll.AddFourInts(i, i, i, i);
        //    });
        //    _b.MeasureAndRecord("AddDoubles", () =>
        //    {
        //        for (double i = 0; i < Iterations; i++)
        //            SimpleDll.AddDoubles(i, i);
        //    });
        //    _b.MeasureAndRecord("AddDoublesIndirect w/o return", () =>
        //    {
        //        for (double i = 0; i < Iterations; i++)
        //            SimpleDll.AddDoublesIndirect(ref i, ref i);
        //    });
        //    _b.MeasureAndRecord("AddDoublesIndirect", () =>
        //    {
        //        for (double i = 0; i < Iterations; i++)
        //        {
        //            IntPtr ptr = SimpleDll.AddDoublesIndirect(ref i, ref i);
        //            _returnedDouble = (double)Marshal.PtrToStructure(ptr, typeof(double));
        //        }
        //    });
        //    _b.MeasureAndRecord("CharStringArgument", () =>
        //    {
        //        for (int i = 0; i < Iterations; i++)
        //            _returnedChar = SimpleDll.CharStringArgument("char string argument");
        //    });
        //    _b.MeasureAndRecord("WCharStringArgument", () =>
        //    {
        //        for (int i = 0; i < Iterations; i++)
        //            _returnedChar = SimpleDll.WCharStringArgument("wchar string argument");
        //    });
        //    _b.MeasureAndRecord("ReturnString", () =>
        //    {
        //        #if CompactFramework
        //       throw new NotSupportedException("Compact Framework does not support char* marshaling");
        //        #else
        //        for (int i = 0; i < Iterations; i++)
        //            _returnedString = Marshal.PtrToStringAnsi(SimpleDll.ReturnString());
        //        #endif
        //    });
        //    _b.MeasureAndRecord("ReturnWString", () =>
        //    {
        //        #if CompactFramework
        //        throw new NotSupportedException("ReturnWString sometimes causes a crash after the benchmark");
        //        #else
        //        if (Environment.Version.Major == 4 && IntPtr.Size == 8)
        //        {
        //            // Calling ReturnWString stops the process on the very first 
        //            // call. When running in the debugger, no exception occurs; the
        //            // debugger stops as though the program terminated normally.
        //            throw new NotSupportedException("ReturnWString crashes x64 .NET 4 on the test PC");
        //        }
        //        for (int i = 0; i < Iterations; i++)
        //            _returnedString = SimpleDll.ReturnWString();
        //        #endif
        //    });
        //    _b.MeasureAndRecord("ReturnBSTR", () =>
        //    {
        //        #if CompactFramework
        //        throw new NotSupportedException("ReturnBSTR crashes the CF Garbage Collector (EE.GC_Collect(int))");
        //        #else
        //        for (int i = 0; i < Iterations; i++)
        //            _returnedString = SimpleDll.ReturnBSTR();
        //        #endif
        //    });
        //    _b.MeasureAndRecord("MakePoint", () =>
        //    {
        //        for (int i = 0; i < Iterations; i++)
        //            _returnedPoint = SimpleDll.MakePoint(i, -i);
        //    });
        //    _b.MeasureAndRecord("MakePointIndirect", () =>
        //    {
        //        for (int i = 0; i < Iterations; i++)
        //        {
        //            IntPtr ptr = SimpleDll.MakePointIndirect(i, -i);
        //            _returnedPoint = (Point)Marshal.PtrToStructure(ptr, typeof(Point));
        //        }
        //    });
        //    _b.MeasureAndRecord("GetPointY", () =>
        //    {
        //        Point p = new Point(24, 25);
        //        for (int i = 0; i < Iterations; i++)
        //            SimpleDll.GetPointY(p);
        //    });
        //    _b.MeasureAndRecord("GetPointYIndirect", () =>
        //    {
        //        Point p = new Point(24, 25);
        //        for (int i = 0; i < Iterations; i++)
        //            SimpleDll.GetPointYIndirect(ref p);
        //    });

        //    return Benchmarker.DiscardResult;
        //}

        //#endregion

        //#region Trivial method calls

        //[Benchmark("Trivial method calls", 
        //#if !CompactFramework
        //    Trials = 1)] // Printing the results is very slow in WinCE so use less trials
        //#else
        //    Trials = 20)] // Desktop Windows has imprecise timer measurements so use lots of trials
        //#endif
        //public object TestFunctionCalls()
        //{
        //    _b.MeasureAndRecord("Static NoOp", () =>
        //    {
        //        for (int i = 0; i < IterationsX10; i++)
        //            NoOp();
        //    });
        //    _b.MeasureAndRecord("No-inline NoOp", () =>
        //    {
        //        for (int i = 0; i < IterationsX10; i++)
        //            NonInlineNoOp();
        //    });
        //    _b.MeasureAndRecord("Virtual NoOp", () =>
        //    {
        //        for (int i = 0; i < IterationsX10; i++)
        //            VirtualNoOp();
        //    });
        //    _b.MeasureAndRecord("Interface NoOp", () =>
        //    {
        //        Benchmark.INoOp @interface = this;
        //        for (int i = 0; i < IterationsX10; i++)
        //            @interface.INoOp();
        //    });

        //    return Benchmarker.DiscardResult;
        //}

        //static void NoOp() { }

        //[MethodImplAttribute(MethodImplOptions.NoInlining)]
        //static void NonInlineNoOp() { }

        //[MethodImplAttribute(MethodImplOptions.NoInlining)]
        //protected virtual void VirtualNoOp() { }

        //static int AddInts(int x, int y) { return x + y; }

        //[MethodImplAttribute(MethodImplOptions.NoInlining)]
        //static int NonInlineAddInts(int x, int y) { return x + y; }

        //[MethodImplAttribute(MethodImplOptions.NoInlining)]
        //protected virtual int VirtualAddInts(int x, int y) { return x + y; }

        //#endregion

		#region Matrix multiply

		#if CompactFramework
		const int MatrixSize = 464; // 464.1 on a side is about 1/10 the workload of 1000
		#else
		const int MatrixSize = 1000;
		#endif

		[Benchmark("Matrix multiply"
		#if CompactFramework
			, Trials=1 /* Because it's ridiculously slow, > 12 minutes, due to the double math */
		#endif
		)]
		public object MatrixMultiply()
		{
			// The csv printer and chart maker don't support quoted-comma notation.
			// So instead of double[n,n], use something with no comma.
			_b.MeasureAndRecord("double[n¸n]", () =>
			{
				double[,] a, b, x;
				a = GenerateMatrixV1(MatrixSize);
				b = GenerateMatrixV1(MatrixSize);
				x = MultiplyMatrix(a, b);
				return x[MatrixSize / 2, MatrixSize / 2];
			});
			_b.MeasureAndRecord("Array2D<double>", () =>
			{
				Array2D<double> a, b, x;
				a = GenerateMatrixV2(MatrixSize);
				b = GenerateMatrixV2(MatrixSize);
				x = MultiplyMatrix(a, b);
				return x[MatrixSize / 2, MatrixSize / 2];
			});
			_b.MeasureAndRecord("double[n*n]", () =>
			{
				Array2D<double> a, b;
				a = GenerateMatrixV2(MatrixSize);
				b = GenerateMatrixV2(MatrixSize);
				double[] x = MultiplyMatrix(a.InternalArray, b.InternalArray, a.Rows, a.Cols, b.Cols);
				return x[MatrixSize / 2 * MatrixSize + MatrixSize / 2];
			});
			_b.MeasureAndRecord("double[n][n]", () =>
			{
				double[][] a, b, x;
				a = GenerateMatrixV3(MatrixSize);
				b = GenerateMatrixV3(MatrixSize);
				x = MultiplyMatrix(a, b);
				return x[MatrixSize / 2][MatrixSize / 2];
			});
			_b.MeasureAndRecord("int[n][n]", () =>
			{
				int[][] a, b, x;
				a = GenerateMatrixI<int, MathI>(MatrixSize);
				b = GenerateMatrixI<int, MathI>(MatrixSize);
				x = MultiplyMatrix(a, b);
				return x[MatrixSize / 2][MatrixSize / 2];
			});
			_b.MeasureAndRecord("<double>[n][n]", new Func<object>(TestMatrix<double, MathD>));
			_b.MeasureAndRecord("<int>[n][n]",    new Func<object>(TestMatrix<int, MathI>));
			_b.MeasureAndRecord("<float>[n][n]",  new Func<object>(TestMatrix<float, MathF>));

			return Benchmarker.DiscardResult;
		}

		object TestMatrix<T, Math>() where Math:IMath<T>, new()
		{
			T[][] a, b, x;
			a = GenerateMatrixI<T, Math>(MatrixSize);
			b = GenerateMatrixI<T, Math>(MatrixSize);
			x = MultiplyMatrix(a, b, new Math());
			return x[MatrixSize / 2][MatrixSize / 2];
		}

		// Creates a rectangular jagged 2D array
		public T[][] NewJagged<T>(int rows, int cols)
		{
			T[][] a = new T[rows][];
			for (int i = 0; i < a.Length; i++)
				a[i] = new T[cols];
			return a;
		}

		public double[,] GenerateMatrixV1(int n)
		{
			double[,] a = new double[n, n];
			
			double tmp = 1.0 / n / n;
			for (int i = 0; i < n; ++i)
				for (int j = 0; j < n; ++j)
					a[i, j] = tmp * (i - j) * (i + j);
			return a;
		}
		Array2D<double> GenerateMatrixV2(int n)
		{
			var a = new Array2D<double>(n, n);

			double tmp = 1.0 / n / n;
			for (int i = 0; i < n; ++i)
				for (int j = 0; j < n; ++j)
					a[i, j] = tmp * (i - j) * (i + j);
			return a;
		}
		double[][] GenerateMatrixV3(int n)
		{
			double[][] a = NewJagged<double>(n, n);
			
			double tmp = 1.0 / n / n;
			for (int i = 0; i < n; ++i)
				for (int j = 0; j < n; ++j)
					a[i][j] = tmp * (i - j) * (i + j);
			return a;
		}
		private T[][] GenerateMatrixI<T, Math>(int n) where Math : IMath<T>, new()
		{
			Math m = new Math();
			T[][] a = NewJagged<T>(n, n);

			for (int i = 0; i < n; ++i)
				for (int j = 0; j < n; ++j)
					a[i][j] = m.Multiply(m.Subtract(m.From(i), m.From(j)), m.Add(m.From(i), m.From(j)));
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
		Array2D<double> MultiplyMatrix(Array2D<double> a, Array2D<double> b)
		{
			int m = a.Rows, n = b.Cols, p = a.Cols;
			var x = new Array2D<double>(a.Rows, b.Cols); // result
			var c = new Array2D<double>(b.Cols, b.Rows);

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
		double[][] MultiplyMatrix(double[][] a, double[][] b)
		{
			//int m = a.Length, n = a[0].Length, p = b.Length;
			double[][] x = NewJagged<double>(a.Length, b[0].Length);
			double[][] c = NewJagged<double>(b[0].Length, b.Length);
			
			for (int i = 0; i < b.Length; ++i) // transpose
				for (int j = 0; j < b[0].Length; ++j)
					c[j][i] = b[i][j];
			
			for (int i = 0; i < a.Length; ++i)
			{
				var a_i = a[i];
				for (int j = 0; j < c.Length; ++j)
				{
					double s = 0.0;
					var c_j = c[j];
					for (int k = 0; k < a_i.Length; ++k)
						s += a_i[k] * c_j[k];
					x[i][j] = s;
				}
			}
			return x;
		}
		unsafe double[] MultiplyMatrix(double[] a, double[] b, int aRows, int aCols, int bCols)
		{
			// Note: as is conventional in C#/C/C++, a and b are in row-major order.
			// Note: bRows (the number of rows in b) must equal aCols.
			int bRows = aCols;
			Debug.Assert(a.Length == aRows * aCols);
			Debug.Assert(b.Length == bRows * bCols);
			double[] result = new double[aRows * bCols];
			fixed (double* c = new double[bCols * bRows], x = result)
			{
				for (int i = 0; i < bRows; ++i) // transpose (large-matrix optimization)
					for (int j = 0; j < bCols; ++j)
						c[j * bRows + i] = b[i * bCols + j];

				for (int i = 0; i < aRows; ++i)
				{
					fixed(double* a_i = &a[i * aCols])
					for (int j = 0; j < bCols; ++j)
					{
						double* c_j = c + j * bRows;
						double s = 0.0;
						for (int k = 0; k < aCols; ++k)
							s += a_i[k] * c_j[k];
						x[i * bCols + j] = s;
					}
				}
			}
			return result;
		}

		int[][] MultiplyMatrix(int[][] a, int[][] b)
		{
			//int m = a.Length, n = a[0].Length, p = b.Length;
			int[][] x = NewJagged<int>(a.Length, b.Length);
			int[][] c = NewJagged<int>(b.Length, a[0].Length);
			
			for (int i = 0; i < a[0].Length; ++i) // transpose
				for (int j = 0; j < b.Length; ++j)
					c[j][i] = b[i][j];
			
			for (int i = 0; i < a.Length; ++i)
			{
				var a_i = a[i];
				for (int j = 0; j < b.Length; ++j)
				{
					int s = 0;
					var c_j = c[j];
					for (int k = 0; k < a[i].Length; ++k)
						s += a_i[k] * c_j[k];
					x[i][j] = s;
				}
			}
			return x;
		}
		T[][] MultiplyMatrix<T,Math>(T[][] a, T[][] b, Math m) where Math:IRing<T>
		{
			//int m = a.Length, n = a[0].Length, p = b.Length;
			T[][] x = NewJagged<T>(a.Length, b.Length);
			T[][] c = NewJagged<T>(b.Length, a[0].Length);

			for (int i = 0; i < a[0].Length; ++i) // transpose
				for (int j = 0; j < b.Length; ++j)
					c[j][i] = b[i][j];

			for (int i = 0; i < a.Length; ++i)
			{
				var a_i = a[i];
				for (int j = 0; j < b.Length; ++j)
				{
					T s = m.Zero;
					var c_j = c[j];
					for (int k = 0; k < a[i].Length; ++k)
						s = m.Add(s, m.Multiply(a_i[k], c_j[k]));
					x[i][j] = s;
				}
			}
			return x;
		}

		#endregion

		#region Sudoku

		class SudokuSolver
		{
			readonly Array2D<int> R = new Array2D<int>(324, 9);
			readonly Array2D<int> C = new Array2D<int>(729, 4);
			
			public SudokuSolver()
			{
				int[] nr = new int[324];
				int i, j, k, r, c, c2;
				for (i = r = 0; i < 9; ++i) // generate c[729][4]
					for (j = 0; j < 9; ++j)
						for (k = 0; k < 9; ++k)
						{	// this "9" means each cell has 9 possible numbers
							C[r, 0] = 9 * i + j;                  // row-column constraint
							C[r, 1] = (i / 3 * 3 + j / 3) * 9 + k + 81; // box-number constraint
							C[r, 2] = 9 * i + k + 162;            // row-number constraint
							C[r, 3] = 9 * j + k + 243;            // col-number constraint
							++r;
						}
				for (c = 0; c < 324; ++c)
					nr[c] = 0;
				for (r = 0; r < 729; ++r) // generate r[][] from c[][]
					for (c2 = 0; c2 < 4; ++c2)
					{
						k = C[r, c2]; R[k, nr[k]] = r; nr[k]++;
					}
			}

			private int Update(int[] sr, int[] sc, int r, int v)
			{
				int c2, min = 10, min_c = 0;
				for (c2 = 0; c2 < 4; ++c2)
					sc[C[r, c2]] += v << 7;
				for (c2 = 0; c2 < 4; ++c2)
				{	// update # available choices
					int r2, rr, cc2, c = C[r, c2];
					if (v > 0)
					{	// move forward
						for (r2 = 0; r2 < 9; ++r2)
						{
							if (sr[rr = R[c, r2]]++ != 0) continue; // update the row status
							for (cc2 = 0; cc2 < 4; ++cc2)
							{
								int cc = C[rr, cc2];
								if (--sc[cc] < min)
								{	// update # allowed choices
									min = sc[cc]; min_c = cc; // register the minimum number
								}
							}
						}
					}
					else
					{	// revert
						for (r2 = 0; r2 < 9; ++r2)
						{
							if (--sr[rr = R[c, r2]] != 0) continue; // update the row status
							++sc[C[rr, 0]]; ++sc[C[rr, 1]]; ++sc[C[rr, 2]]; ++sc[C[rr, 3]]; // update the count array
						}
					}
				}
				return min << 16 | min_c; // return the col that has been modified and with the minimal available choices
			}

			/// <summary>Convert 81-character dot/number representation to an array of integers</summary>
			public static int[] PuzzleToArray(string s)
			{
				Debug.Assert(s.Length == 81);
				int[] result = new int[s.Length];
				for (int i = 0; i < s.Length; i++)
					result[i] = s[i] >= '1' && s[i] <= '9' ? s[i] - '1' : -1; // number from -1 to 8
				return result;
			}

			public List<int[]> Solve(int[] puzzle)
			{
				var solutions = new List<int[]>();

				int i, j, r, c, r2, dir, cand, n = 0, min, hints = 0; // dir=1: forward; dir=-1: backtrack
				int[] sr = new int[729];
				int[] cr = new int[81];
				int[] sc = new int[324];
				int[] cc = new int[81];
				int[] o = new int[81];
				for (r = 0; r < 729; ++r) sr[r] = 0; // no row is forbidden
				for (c = 0; c < 324; ++c) sc[c] = 0 << 7 | 9; // 9 allowed choices; no constraint has been used
				for (i = 0; i < 81; ++i)
				{
					int a = puzzle[i];
					if (a >= 0)
						Update(sr, sc, i * 9 + a, 1); // set the choice
					if (a >= 0)
						++hints; // count the number of hints
					cr[i] = cc[i] = -1; o[i] = a;
				}
				i = 0; dir = 1; cand = 10 << 16 | 0;
				for (;;)
				{
					while (i >= 0 && i < 81 - hints)
					{	// maximum 81-hints steps
						if (dir == 1)
						{
							min = cand >> 16; cc[i] = cand & 0xffff;
							if (min > 1)
							{
								for (c = 0; c < 324; ++c)
								{
									if (sc[c] < min)
									{
										min = sc[c]; cc[i] = c; // choose the top constraint
										if (min <= 1) break; // this is for acceleration; slower without this line
									}
								}
							}
							if (min == 0 || min == 10) cr[i--] = dir = -1; // backtrack
						}
						c = cc[i];
						if (dir == -1 && cr[i] >= 0)
							Update(sr, sc, R[c, cr[i]], -1); // revert the choice
						for (r2 = cr[i] + 1; r2 < 9; ++r2) // search for the choice to make
							if (sr[R[c, r2]] == 0) break; // found if the state equals 0
						if (r2 < 9)
						{
							cand = Update(sr, sc, R[c, r2], 1); // set the choice
							cr[i++] = r2; dir = 1; // moving forward
						}
						else cr[i--] = dir = -1; // backtrack
					}
					if (i < 0) break;

					int[] solution = (int[])o.Clone();
					for (j = 0; j < i; ++j)
					{
						r = R[cc[j], cr[j]];
						solution[r / 9] = r % 9;
					}
					solutions.Add(solution);
					++n; --i; dir = -1; // backtrack
				}
				return solutions;
			}

			public List<string> Solve(string puzzle)
			{
				int[] a = PuzzleToArray(puzzle);
				var solutions = Solve(a);
				var strings = new List<string>();

				foreach (var s in solutions)
					strings.Add(new string(s.Select(i => (char)(i + '1')).ToArray()));
				return strings;
			}
		}

		#if false // Same performance as the first version
		class SudokuSolverV2
		{
			unsafe struct Constraint
			{
				public fixed int c[4];
				public unsafe int this[int i]
				{
					get { fixed (int* p = c) return p[i]; }
					set { fixed (int* p = c) p[i] = value; }
				}
			}
			unsafe struct Row
			{
				public fixed int r[9];
				public unsafe int this[int i]
				{
					get { fixed (int* p = r) return p[i]; }
					set { fixed (int* p = r) p[i] = value; }
				}
			}
			readonly Row[] R = new Row[324];
			readonly Constraint[] C = new Constraint[729];

			public SudokuSolverV2()
			{
				int[] nr = new int[324];
				int i, j, k, r, c, c2;
				for (i = r = 0; i < 9; ++i) // generate c[729][4]
					for (j = 0; j < 9; ++j)
						for (k = 0; k < 9; ++k)
						{	// this "9" means each cell has 9 possible numbers
							C[r][0] = 9 * i + j;                  // row-column constraint
							C[r][1] = (i / 3 * 3 + j / 3) * 9 + k + 81; // box-number constraint
							C[r][2] = 9 * i + k + 162;            // row-number constraint
							C[r][3] = 9 * j + k + 243;            // col-number constraint
							++r;
						}
				for (c = 0; c < 324; ++c)
					nr[c] = 0;
				for (r = 0; r < 729; ++r) // generate r[][] from c[][]
					for (c2 = 0; c2 < 4; ++c2)
					{
						k = C[r][c2]; R[k][nr[k]] = r; nr[k]++;
					}
			}

			private int Update(int[] sr, int[] sc, int r, int v)
			{
				int c2, min = 10, min_c = 0;
				for (c2 = 0; c2 < 4; ++c2)
					sc[C[r][c2]] += v << 7;
				for (c2 = 0; c2 < 4; ++c2)
				{	// update # available choices
					int r2, rr, cc2, c = C[r][c2];
					if (v > 0)
					{	// move forward
						for (r2 = 0; r2 < 9; ++r2)
						{
							if (sr[rr = R[c][r2]]++ != 0) continue; // update the row status
							for (cc2 = 0; cc2 < 4; ++cc2)
							{
								int cc = C[rr][cc2];
								if (--sc[cc] < min)
								{	// update # allowed choices
									min = sc[cc]; min_c = cc; // register the minimum number
								}
							}
						}
					}
					else
					{	// revert
						for (r2 = 0; r2 < 9; ++r2)
						{
							if (--sr[rr = R[c][r2]] != 0) continue; // update the row status
							++sc[C[rr][0]]; ++sc[C[rr][1]]; ++sc[C[rr][2]]; ++sc[C[rr][3]]; // update the count array
						}
					}
				}
				return min << 16 | min_c; // return the col that has been modified and with the minimal available choices
			}

			/// <summary>Convert 81-character dot/number representation to an array of integers</summary>
			public static int[] PuzzleToArray(string s)
			{
				Debug.Assert(s.Length == 81);
				int[] result = new int[s.Length];
				for (int i = 0; i < s.Length; i++)
					result[i] = s[i] >= '1' && s[i] <= '9' ? s[i] - '1' : -1; // number from -1 to 8
				return result;
			}

			public List<int[]> Solve(int[] puzzle)
			{
				var solutions = new List<int[]>();

				int i, j, r, c, r2, dir, cand, min, hints = 0; // dir=1: forward; dir=-1: backtrack
				int[] sr = new int[729];
				int[] cr = new int[81];
				int[] sc = new int[324];
				int[] cc = new int[81];
				int[] o = new int[81];
				for (r = 0; r < 729; ++r)
					sr[r] = 0; // no row is forbidden
				for (c = 0; c < 324; ++c)
					sc[c] = 0 << 7 | 9; // 9 allowed choices; no constraint has been used
				for (i = 0; i < 81; ++i)
				{
					int a = puzzle[i];
					if (a >= 0)
						Update(sr, sc, i * 9 + a, 1); // set the choice
					if (a >= 0)
						++hints; // count the number of hints
					cr[i] = cc[i] = -1; o[i] = a;
				}
				i = 0; dir = 1; cand = 10 << 16 | 0;
				for (; ; )
				{
					while (i >= 0 && i < 81 - hints)
					{	// maximum 81-hints steps
						if (dir == 1)
						{
							min = cand >> 16; cc[i] = cand & 0xffff;
							if (min > 1)
							{
								for (c = 0; c < 324; ++c)
								{
									if (sc[c] < min)
									{
										min = sc[c]; cc[i] = c; // choose the top constraint
										if (min <= 1) break; // this is for acceleration; slower without this line
									}
								}
							}
							if (min == 0 || min == 10) cr[i--] = dir = -1; // backtrack
						}
						c = cc[i];
						if (dir == -1 && cr[i] >= 0)
							Update(sr, sc, R[c][cr[i]], -1); // revert the choice
						for (r2 = cr[i] + 1; r2 < 9; ++r2) // search for the choice to make
							if (sr[R[c][r2]] == 0) break; // found if the state equals 0
						if (r2 < 9)
						{
							cand = Update(sr, sc, R[c][r2], 1); // set the choice
							cr[i++] = r2; dir = 1; // moving forward
						}
						else cr[i--] = dir = -1; // backtrack
					}
					if (i < 0) break;

					int[] solution = (int[])o.Clone();
					for (j = 0; j < i; ++j)
					{
						r = R[cc[j]][cr[j]];
						solution[r / 9] = r % 9;
					}
					solutions.Add(solution);
					--i; dir = -1; // backtrack
				}
				return solutions;
			}

			public List<string> Solve(string puzzle)
			{
				int[] a = PuzzleToArray(puzzle);
				var solutions = Solve(a);
				var strings = new List<string>();

				foreach (var s in solutions)
					strings.Add(new string(s.Select(i => (char)(i + '1')).ToArray()));
				return strings;
			}
		}
		#endif

		string[] SudokuPuzzles = new string[] {
			"..............3.85..1.2.......5.7.....4...1...9.......5......73..2.1........4...9", // near worst case for brute-force solver (wiki)
			".......12........3..23..4....18....5.6..7.8.......9.....85.....9...4.5..47...6...", // gsf's sudoku q1 (Platinum Blonde)
			".2..5.7..4..1....68....3...2....8..3.4..2.5.....6...1...2.9.....9......57.4...9..", // (Cheese)
			"........3..1..56...9..4..7......9.5.7.......8.5.4.2....8..2..9...35..1..6........", // (Fata Morgana)
			"12.3....435....1....4........54..2..6...7.........8.9...31..5.......9.7.....6...8", // (Red Dwarf)
			"1.......2.9.4...5...6...7...5.9.3.......7.......85..4.7.....6...3...9.8...2.....1", // (Easter Monster)
			".......39.....1..5..3.5.8....8.9...6.7...2...1..4.......9.8..5..2....6..4..7.....", // Nicolas Juillerat's Sudoku explainer 1.2.1 (top 5)
			"12.3.....4.....3....3.5......42..5......8...9.6...5.7...15..2......9..6......7..8", 
			"..3..6.8....1..2......7...4..9..8.6..3..4...1.7.2.....3....5.....5...6..98.....5.",
			"1.......9..67...2..8....4......75.3...5..2....6.3......9....8..6...4...1..25...6.",
			"..9...4...7.3...2.8...6...71..8....6....1..7.....56...3....5..1.4.....9...2...7..",
			"....9..5..1.....3...23..7....45...7.8.....2.......64...9..1.....8..6......54....7", // dukuso's suexrat9 (top 1)
			"4...3.......6..8..........1....5..9..8....6...7.2........1.27..5.3....4.9........", // from http://magictour.free.fr/topn87 (top 3)
			"7.8...3.....2.1...5.........4.....263...8.......1...9..9.6....4....7.5...........",
			"3.7.4...........918........4.....7.....16.......25..........38..9....5...2.6.....",
			"........8..3...4...9..2..6.....79.......612...6.5.2.7...8...5...1.....2.4.5.....3", // dukuso's suexratt (top 1)
			".......1.4.........2...........5.4.7..8...3....1.9....3..4..2...5.1........8.6...", // first 2 from sudoku17
			".......12....35......6...7.7.....3.....4..8..1...........12.....8.....4..5....6..",
			"1.......2.9.4...5...6...7...5.3.4.......6........58.4...2...6...3...9.8.7.......1", // 2 from http://www.setbb.com/phpbb/viewtopic.php?p=10478
			".....1.2.3...4.5.....6....7..2.....1.8..9..3.4.....8..5....2....9..3.4....67.....",
		};

		[Benchmark("Sudoku")]
		public object Sudokus()
		{
			const int LessIterations = Iterations / 100000;
			for (int i = 0; i < LessIterations; i++)
			{
				SudokuSolver a = new SudokuSolver();
				for (int j = 0; j < SudokuPuzzles.Length; j++)
				{
					string puzzle = SudokuPuzzles[j];
					List<string> solutions = a.Solve(puzzle);
					
					#if DEBUG
					if (i == 0) {
						Console.WriteLine("{0} has {1} solution:", puzzle, solutions.Count);
						foreach (var s in solutions) {
							for (int r = 0; r < 9; r++)
								Console.WriteLine(s.Substring(r * 9, 9));
							Console.WriteLine("---------");
						}
					}
					#endif
				}
			}
			return string.Format("{0}x{1} puzzles", SudokuPuzzles.Length, LessIterations);
		}

		#if false // Same performance as the first version
		[Benchmark("Sudoku v2")]
		public object SudokusV2()
		{
			const int LessIterations = Iterations / 100000;
			for (int i = 0; i < LessIterations; i++)
			{
				SudokuSolver a = new SudokuSolver();
				for (int j = 0; j < SudokuPuzzles.Length; j++)
				{
					string puzzle = SudokuPuzzles[j];
					List<string> solutions = a.Solve(puzzle);
					
					#if DEBUG
					if (i == 0) {
						Console.WriteLine("{0} has {1} solution:", puzzle, solutions.Count);
						foreach (var s in solutions) {
							for (int r = 0; r < 9; r++)
								Console.WriteLine(s.Substring(r * 9, 9));
							Console.WriteLine("---------");
						}
					}
					#endif
				}
			}
			return string.Format("{0}x{1} puzzles", SudokuPuzzles.Length, LessIterations);
		}
		#endif

		#endregion

		#region Polynomial

		float dopoly(float x, float[] pol) {
			float mu = 10.0f;
			float s;
			int j;

			for (j=0; j<100; j++)
				pol[j] = mu = (mu + 2.0f) / 2.0f;
			s = 0.0f;
			for (j=0; j<100; j++)
				s = x*s + pol[j];
			return s;
		}

		[Benchmark("Polynomials"
		#if CompactFramework
			, Trials=1 /* Because it's ridiculously slow, > 4 minutes */
		#endif
		)]
		public object Polynomials()
		{
			float x = 0.2f;
			float pu = 0.0f;
			float[] pol = new float[100];

			for (int i = 0; i < Iterations; i++)
				pu += dopoly(x, pol);
			return pu;
		}

		#endregion
	}

	public interface INoOp { void INoOp(); }

	public static class SimpleDll
	{
		[DllImport("SimpleDll")] public static extern int NextInt();
		[DllImport("SimpleDll")] public static extern void SetNextInt(int x);
		[DllImport("SimpleDll")] public static extern int AddInts(int a, int b);
		[DllImport("SimpleDll")] public static extern int AddFourInts(int a, int b, int c, int d);
		[DllImport("SimpleDll")] public static extern double AddDoubles(double x, double y);
		[DllImport("SimpleDll")] public static extern IntPtr AddDoublesIndirect(ref double x, ref double y);
		[DllImport("SimpleDll")] [return: MarshalAs(UnmanagedType.U1)]
		public static extern char CharStringArgument([MarshalAs(UnmanagedType.LPStr)]string s);
		[DllImport("SimpleDll")] [return: MarshalAs(UnmanagedType.U2)]
		public static extern char WCharStringArgument([MarshalAs(UnmanagedType.LPWStr)]string s);

		// There is a problem which causes an AccessViolationException if the
		// DLL is compiled to a Release build and ReturnString() is declared to 
		// return a string directly, so instead we declare it to return IntPtr, 
		// and call Marshal.PtrToStringAnsi() on the returned pointer.
		[DllImport("SimpleDll")]
		public static extern IntPtr ReturnString();
		
		[DllImport("SimpleDll")] [return: MarshalAs(UnmanagedType.LPWStr)]
		public static extern string ReturnWString();
		[DllImport("SimpleDll")] [return: MarshalAs(UnmanagedType.BStr)]
		public static extern string ReturnBSTR();
		[DllImport("SimpleDll")] public static extern System.Drawing.Point MakePoint(int x, int y);
		[DllImport("SimpleDll")] public static extern IntPtr MakePointIndirect(int x, int y);
		[DllImport("SimpleDll")] public static extern int GetPointY(System.Drawing.Point p);
		[DllImport("SimpleDll")] public static extern int GetPointYIndirect(ref System.Drawing.Point pp);
		[DllImport("SimpleDll")] public static extern int SumIntegers(ref int firstElem, int size);
	}

	public struct Array2D<T> // use "class" if you prefer
	{
		readonly T[] _a;
		readonly int _cols, _rows;
		
		public Array2D(int rows, int cols)
		{
			_a = new T[rows * cols];
			_cols = cols;
			_rows = rows;
		}
		public Array2D(T[] array, int cols)
		{
			_a = array;
			_cols = cols;
			_rows = _a.Length / cols;
		}
		
		public T[] InternalArray { get { return _a; } }
		public int Cols { get { return _cols; } }
		public int Rows { get { return _rows; } }
		
		// Add a range check on "col" if you want to sacrifice performance for safety
		public T this[int row, int col]
		{
			get { return _a[row * _cols + col]; }
			set { _a[row * _cols + col] = value; }
		}
	};
}
