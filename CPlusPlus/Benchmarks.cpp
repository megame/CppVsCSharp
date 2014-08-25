#include "stdafx.h"
#include "Benchmarker.h"
#include <time.h>
#include <fstream>
#include <algorithm>
#include "FixedPoint.h"
#include "Paths.h"
using namespace std;
using namespace Math;

#if _SECURE_SCL || _HAS_ITERATOR_DEBUGGING
#error Just making sure it's off
#endif

#ifdef UNDER_CE
const int Iterations = 1000000;
#else
const int Iterations = 10000000;
#endif
Benchmarker _b;

typedef FixedPoint<8> FPI8;
typedef FixedPoint<16> FPI16;
typedef FixedPoint<16,int64> FPL16;
typedef FixedPoint<32,int64> FPL32;

// Define maximum map/dictionary size
#ifdef UNDER_CE
const int MapSizeLimit = Iterations / 4; // CE has per-process heap size limit, preventing a huge dictionary
#else
const int MapSizeLimit = Iterations / 2; // Desktop can do more, but 50% misses is still a reasonable test
#endif

// Standard hashtable test
#define HASHTABLE_NAMESPACE HashtableTest
// http://stackoverflow.com/questions/724465/how-to-check-for-tr1-while-compiling
// Apparently, there is no standard way to detect whether unordered_map is
// available, or whether it is located in "tr1" or "std" namespace. Typical.
#if _MSC_VER / 100 == 15 || _MSC_VER / 100 == 14 /* VC9 = version 15 */
#include <hash_map>
#define HASHTABLE stdext::hash_map
#else
#include <unordered_map>
#define HASHTABLE unordered_map
#endif
#include "hashtable_inc.cxx"

// Custom hash_map test
#include "hash_map.h"
#undef HASHTABLE_NAMESPACE
#undef HASHTABLE
#define HASHTABLE_NAMESPACE CustomHashtableTest
#define HASHTABLE hash_map
#include "hashtable_inc.cxx"

namespace IntMapTest
{
	map<int, int> _dict;
	const int MapSizeLimit = Iterations / 2; // limits the number of items

	string TestAdding()
	{
		map<int, int>& dict = _dict;
		for (int i = 0; i < Iterations; i++) {
			if ((int)dict.size() >= MapSizeLimit)
				dict.clear();
			dict[i ^ 314159] = i;
		}
		return string();
	}
	string TestQueries()
	{
		map<int, int>& dict = _dict;
		int misses = 0;
		for (int i = 0; i < Iterations; i++)
		{
			map<int,int>::iterator it = dict.find(i ^ 314159);
			if (it == dict.end())
				misses++;
		}
		return printstring("%d%% misses", misses * 100 / Iterations);
	}
	string TestRemoval()
	{
		map<int, int>& dict = _dict;
		int removed = 0;
		for (int i = 0; i < Iterations; i++)
			if (dict.erase(i ^ 314159))
				removed++;
		return printstring("%d removed", removed);
	}
	string Tests()
	{
		_dict.clear();
		_b.MeasureAndRecord("1 Adding items", TestAdding);
		_b.MeasureAndRecord("2 Running queries", TestQueries);
		_b.MeasureAndRecord("3 Removing items", TestRemoval);
		return Benchmarker::DiscardResult;
	}
}

namespace SquareRootTest
{
	int64 totalI = 0, totalL = 0;
	double totalD = 0;
	FPL16 totalFP;
	const int Base = 999999;

	string SqrtDouble()
	{
		double total = 0;
		for (double i = Base; i < Base + Iterations; i++)
			total += sqrt(i);
		totalD = total;
		return string();
	}
	string SqrtU32()
	{
		int64 total = 0;
		for (int i = Base; i < Base + Iterations; i++)
			total += Sqrt((uint)i);
		totalI = total;
		return string();
	}
	string SqrtU64()
	{
		int64 total = 0;
		for (int i = Base; i < Base + Iterations; i++)
			total += Sqrt((uint64)i);
		totalL = total;
		return string();
	}
	string SqrtFPL16()
	{
		FPL16 total = 0;
		for (FPL16 i = Base; i < (FPL16)(Base + Iterations); i++)
			total += i.Sqrt();
		totalFP = total;
		return string();
	}
	string Tests()
	{
		_b.MeasureAndRecord("double", SqrtDouble);
		_b.MeasureAndRecord("uint32", SqrtU32);
		_b.MeasureAndRecord("uint64", SqrtU64);
		_b.MeasureAndRecord("FPL16", SqrtFPL16);
		assert(totalI == totalL);
		assert(totalD > (double)totalI && totalD < (double)(totalI + Iterations));
		assert((totalFP - (FPL16)totalD).Abs().N < Iterations);
		return Benchmarker::DiscardResult;
	}
}

namespace SimpleArithmeticTest
{
	int64 totalI = 0, totalL = 0;
	double totalD = 0;
	float totalF = 0;
	FPI8 totalF8 = 0;
	FPL16 totalFL16 = 0;
	
	// This benchmark is designed so that all tests produce similar totals.
	// FPI8 has limited range (max 8 million); Modulus should be low to avoid overflow.
	const int Modulus = 100000;
	const int IterationsX10 = Iterations * 10;

	string TestDouble()
	{
		double total = 0;
		for (int i = 0; i < IterationsX10; i++)
			total = total - total / 4.0 + fmod((double)i, (double)Modulus) * 3.0;
		totalD = total;
		return string();
	}
	string TestFloat()
	{
		float total = 0;
		for (int i = 0; i < IterationsX10; i++)
			total = total - total / 4.0f + (float)(i % Modulus) * 3.0f;
		totalF = total;
		return string();
	}
	string TestFPI8()
	{
		FPI8 total = 0;
		for (int i = 0; i < IterationsX10; i++)
			total = total - (total >> 2) + (FPI8)(i % Modulus) * 3;
		totalF8 = total;
		return string();
	}
	string TestFPL16()
	{
		FPL16 total = 0;
		FPL16 Modulus2 = (FPL16)Modulus;
		for (int i = 0; i < IterationsX10; i++)
			total = total - (total >> 2) + (FPL16)i % Modulus2 * (int64)3;
		totalFL16 = total;
		return string();
	}
	string TestInt()
	{
		int total = 0;
		for (int i = 0; i < IterationsX10; i++)
			total = total - (total >> 2) + i % Modulus * 3;
		totalI = total;
		return string();
	}
	string TestInt64()
	{
		int64 total = 0;
		for (int i = 0; i < IterationsX10; i++)
			total = total - (total >> 2) + (int64)i % Modulus * 3;
		totalL = total;
		return string();
	}
	string Tests()
	{
		_b.MeasureAndRecord("double", TestDouble);
		_b.MeasureAndRecord("float", TestFloat);
		_b.MeasureAndRecord("FPI8", TestFPI8);
		_b.MeasureAndRecord("FPL16", TestFPL16);
		_b.MeasureAndRecord("Int", TestInt);
		_b.MeasureAndRecord("Int64", TestInt64);

		assert(totalI == totalL);
		assert(fabs(totalD - (double)totalI) < 5);
		assert(fabs(totalD - (double)totalF8) < 1);
		assert(fabs(totalD - (double)totalFL16) < 1);
		return Benchmarker::DiscardResult;
	}
}

namespace GenericSumTest
{
	const int ListSize = 500000;
	// Use 10x as many total iterations as usual because the inner loops are so fast
	const int OuterIterations = Iterations / ListSize * 10;
	
	vector<int> listI;
	vector<double> listD;
	vector<FPI8> listF8;
	volatile int64 sum;
	
	template<typename T>
	T GenericSum(const vector<T>& list)
	{
		T sum = (T)0;
		for(int i = 0; i < (int)list.size(); i++)
			sum += list[i];
		return sum; 
	} 
	// NOTE: It's important actually compute the sum and return it! If the code 
	// ignores the sum, AND checked iterators are off, the VC9 optimizer eliminates
	// the entire loop and doesn't call GenericSum, so the total time is 0.000!
	template<class T>
	string TestGeneric(vector<T>& list)
	{
		sum = 0;
		for (int i = 0; i < OuterIterations; i++)
			sum += (int64)GenericSum(listI);
		return printstring("%I64d", sum);
	}

	int NonGenericSum(const vector<int>& list)
	{
		int sum = 0;
		for(int i = 0; i < (int)list.size(); i++)
			sum += list[i]; 
		return sum;
	} 
	string TestNonGeneric(vector<int>& listI)
	{
		int64 sum = 0;
		for (int i = 0; i < OuterIterations; i++)
			sum += NonGenericSum(listI);
		return printstring("%I64d", sum);
	}

	string TestInt() { return TestGeneric(listI); }
	string TestNonGeneric() { return TestNonGeneric(listI); }
	string TestDouble() { return TestGeneric(listD); }
	string TestFPI8() { return TestGeneric(listF8); }

	string Tests()
	{
		const int ListSize = 500000;
		
		listI.resize(ListSize);
		listD.resize(ListSize);
		listF8.resize(ListSize);
		for (int i = 0; i < ListSize; i++)
			listF8[i] = listD[i] = listI[i] = i+1;

		_b.MeasureAndRecord("int", TestInt);
		_b.MeasureAndRecord("int without template", TestNonGeneric);
		_b.MeasureAndRecord("double", TestDouble);
		_b.MeasureAndRecord("FPI8", TestFPI8);
		return Benchmarker::DiscardResult;
	}
}

namespace SimpleParsingTest
{
	const int Repetitions = 
	#ifdef UNDER_CE
		2;
	#else
		20;
	#endif

	vector<string> lines;
	string fileContents;

	void OpenTestDictFile(ifstream& file)
	{
		string home = GetHomeFolder();
		for(;;) {
			file.open(CombinePaths(home, "TestDict.txt").c_str(), ios::in);
			if (file.is_open()) {
				assert(file.good());
				return;
			} else {
				// We must clear the 'failbit', or the stream will refuse to read
				// from the file after it is opened; counterintuitively, the 
				// "clear" function doesn't clear the specified bits; it actually 
				// SETS the specified bits and clears all others.
				file.clear(ios::goodbit);
			}
			
			int i = max((int)home.find_last_of('\\'), (int)home.find_last_of('/'));
			if (i == -1)
				throw exception("Missing TestDict.txt");
			home.resize(i);
		}
	}

	string TestReadToEnd()
	{
		for (int i = 0; i < Repetitions; i++)
		{
			ifstream file;
			OpenTestDictFile(file);
			
			// Make buffer to hold the file contents
			file.seekg(0, ios::end);
			int fileSize = (int)file.tellg();
			file.seekg(0, ios::beg);
			fileContents.resize(fileSize);
			
			// Read the file into a string
			int textSize = (int)file.read(&fileContents[0], fileSize).gcount();
			fileContents.resize(textSize);
		}
		return string();
	}

	string TestReadLines()
	{
		for (int i = 0; i < Repetitions; i++)
		{
			lines.clear();
			
			ifstream file;
			OpenTestDictFile(file);

			do {
				// Initialize string in-place to avoid making a copy
				lines.push_back(string());
			} while(getline(file, lines.back()));
			lines.pop_back();
		}
		return string();
	}

	FILE* OpenTestDictFile2()
	{
		string home = GetHomeFolder();
		string filename;
		for(;;) {
			filename = CombinePaths(home, "TestDict.txt");
			if (FILE* fp = fopen(filename.c_str(), "rt"))
				return fp;
			
			int i = max((int)home.find_last_of('\\'), (int)home.find_last_of('/'));
			if (i == -1)
				throw exception("Missing TestDict.txt");
			home.resize(i);
		}
	}

	string TestReadToEnd2()
	{
		for (int i = 0; i < Repetitions; i++)
		{
			FILE* fp = OpenTestDictFile2();
			
			// Make buffer to hold the file contents
			fseek(fp, 0L, SEEK_END);
			int fileSize = ftell(fp);
			fseek(fp, 0L, SEEK_SET);
			fileContents.resize(fileSize);
			
			// Read the file into a string
			int textSize = (int)fread(&fileContents[0], 1, fileSize, fp);
			fileContents.resize(textSize);
		}
		return string();
	}

	string TestReadLines2()
	{
		for (int i = 0; i < Repetitions; i++)
		{
			lines.clear();
			
			FILE* fp = OpenTestDictFile2();

			// Note that fgets requires a fixed-size buffer and a line length limit,
			// unlike  C++ getline() and C# ReadLine(). I imagine lines in text
			// files don't usually get longer than 64 KB, right?
			vector<char> buf;
			buf.resize(64*1024);
			char* buf2;
			
			while ((buf2 = fgets(&buf[0], 64*1024-1, fp)) != NULL)
			{
				// Exclude '\n' from the string we add to 'lines'. Note: I'm being lazy
				// by not trying to detect if there was a '\0' character in the file 
				// itself.
				int len = strlen(buf2);
				buf2[max(len-1, 0)] = '\0';
				
				// Initialize string in-place to avoid an unnecessary copy
				lines.push_back(string());
				lines.back() = buf2;
			}
		}
		return string();
	}

	bool IsKeyLessCaseInsensitive(const pair<string, string>& a, const pair<string, string>& b)
	{
		const string& a1 = a.first;
		const string& b1 = b.first;

		return _stricmp(a1.c_str(), b1.c_str()) < 0;
	}

	typedef pair<string, string> KeyValue;
	vector<KeyValue> entries;

	string TestParse()
	{
		for (int r = 0; r < Repetitions; r++)
		{
			entries.clear();

			for (int i = 1; i < (int)lines.size(); i++)
			{
				const string& line = lines[i];
				if (line.empty() || line[0] == ';')
					continue;

				string key;
				string value;
				int index = (int)line.find(":=");
				if (index > 0)
				{
					entries.push_back(KeyValue());
					KeyValue& back = entries.back();

					back.first = line.substr(0, index);
					back.second = line.substr(index + 2);
					while (i + 1 < (int)lines.size() && lines[i + 1].find(":=") == string::npos)
						back.second += "\n" + lines[++i];
				}
			}
		}
		return string();
	}

	string TestSort()
	{
		SimpleTimer timer;
		int waste = 0;

		for (int r = 0; r < Repetitions; r++)
		{
			// Mess up some of the entries, to ensure sorting requires some work.
			// Time spent randomizing is tiny, but will be subtracted from the total.
			timer.Reset();
			for (int i = 0; i < (int)entries.size(); i += 4)
				std::swap(entries[i], entries[rand() % entries.size()]);
			waste += timer.Millisec();
				 
			std::sort(entries.begin(), entries.end(), &Benchmarker::IsKeyLessCaseInsensitive<string>);
		}

		return Benchmarker::SubtractOverhead(waste);
	}

	string Tests()
	{
		_b.MeasureAndRecord(printstring("1 Read to end (ifstr.x%d)", Repetitions), TestReadToEnd);
		_b.MeasureAndRecord(printstring("1 Read to end (FILE* x%d)", Repetitions), TestReadToEnd2);
		_b.MeasureAndRecord(printstring("2 Read lines (ifstr.x%d)", Repetitions), TestReadLines);
		int lines1 = lines.size();
		_b.MeasureAndRecord(printstring("2 Read lines (FILE* x%d)", Repetitions), TestReadLines2);
		int lines2 = lines.size();
		assert (lines1 == lines2);
		_b.MeasureAndRecord(printstring("3 Parse (x%d)", Repetitions), TestParse);
		_b.MeasureAndRecord(printstring("4 Sort (x%d)", Repetitions), TestSort);
		return Benchmarker::DiscardResult;
	}
}

namespace MatrixMultiplyTest
{
	#ifdef UNDER_CE
	const int MatrixSize = 464; // 464.1 on a side is about 1/10 the workload of 1000
	#else
	const int MatrixSize = 1000;
	#endif

	double* GenerateMatrix(int n)
	{
		double* a = new double[n * n];
		
		double tmp = 1.0 / n / n;
		for (int i = 0; i < n; ++i) {
			double* row = &a[i * n];
			for (int j = 0; j < n; ++j)
				row[j] = tmp * (i - j) * (i + j);
		}
		return a;
	}
	template<typename T>
	T* GenerateMatrix(int n)
	{
		T* a = new T[n * n];
		
		for (int i = 0; i < n; ++i) {
			T* row = &a[i * n];
			for (int j = 0; j < n; ++j)
				row[j] = (T)((i - j) * (i + j));
		}
		return a;
	}

	double* MultiplyMatrix(double* a, double* b, int aRows, int aCols, int bCols)
	{
		// int m = aRows, n = bCols, p = aCols; 
		// Note: as is conventional in C#/C/C++, a and b are in row-major order.
		// Note: bRows (the number of rows in b) must equal aCols.
		int bRows = aCols;
		double* x = new double[aRows * bCols]; // result
		double* c = new double[bRows * bCols];

		for (int i = 0; i < aCols; ++i) // transpose (large-matrix optimization)
			for (int j = 0; j < bCols; ++j)
				c[j*bRows + i] = b[i*bCols + j];

		for (int i = 0; i < aRows; ++i) {
			double* a_i = &a[i*aCols];
			for (int j = 0; j < bCols; ++j)
			{
				double* c_j = &c[j*bRows];
				double s = 0.0;
				for (int k = 0; k < aCols; ++k)
					s += a_i[k] * c_j[k];
				x[i*bCols + j] = s;
			}
		}
		delete[] c;
		return x;
	}
	template<typename T>
	T* MultiplyMatrix(T* a, T* b, int aRows, int aCols, int bCols)
	{
		// int m = aRows, n = bCols, p = aCols; 
		// Note: as is conventional in C#/C/C++, a and b are in row-major order.
		// Note: bRows (the number of rows in b) must equal aCols.
		int bRows = aCols;
		T* x = new T[aRows * bCols]; // result
		T* c = new T[bRows * bCols];

		for (int i = 0; i < aCols; ++i) // transpose (large-matrix optimization)
			for (int j = 0; j < bCols; ++j)
				c[j*bRows + i] = b[i*bCols + j];

		for (int i = 0; i < aRows; ++i) {
			T* a_i = &a[i*aCols];
			for (int j = 0; j < bCols; ++j)
			{
				T* c_j = &c[j*bRows];
				T s = 0;
				for (int k = 0; k < aCols; ++k)
					s += a_i[k] * c_j[k];
				x[i*bCols + j] = s;
			}
		}
		delete[] c;
		return x;
	}
	
	string TestDoubleMatrix()
	{
		double* a, * b, * x;
		a = GenerateMatrix(MatrixSize);
		b = GenerateMatrix(MatrixSize);
		x = MultiplyMatrix(a, b, MatrixSize, MatrixSize, MatrixSize);
		double result = x[MatrixSize / 2 * MatrixSize + MatrixSize / 2];
		delete[] a;
		delete[] b;
		delete[] x;
		return printstring("%f", result);
	}
	template<typename T>
	string TestMatrix()
	{
		T* a, * b, * x;
		a = GenerateMatrix<T>(MatrixSize);
		b = GenerateMatrix<T>(MatrixSize);
		x = MultiplyMatrix<T>(a, b, MatrixSize, MatrixSize, MatrixSize);
		T result = x[MatrixSize / 2 * MatrixSize + MatrixSize / 2];
		delete[] a;
		delete[] b;
		delete[] x;
		return printstring("%0.0f", (double)result);
	}

	string Tests()
	{
		_b.MeasureAndRecord("double[n*n]",   TestDoubleMatrix);
		_b.MeasureAndRecord("<double>[n*n]", TestMatrix<double>);
		_b.MeasureAndRecord("<float>[n*n]",  TestMatrix<float>);
		_b.MeasureAndRecord("<int>[n*n]",    TestMatrix<int>);
		return Benchmarker::DiscardResult;
	}
}

namespace Sudoku
{
	class SudokuSolver {
	private:
		int R[324][9];
		int C[729][4];

		int Update(int* sr, int* sc, int r, int v)
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

	public:
		SudokuSolver()
		{
			int nr[324];
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

		// Convert 81-character dot/number representation to an array of integers
		static int* PuzzleToArray(const char* s)
		{
			int* result = new int[81];
			for (int i = 0; i < 81; i++)
				result[i] = s[i] >= '1' && s[i] <= '9' ? s[i] - '1' : -1; // number from -1 to 8
			return result;
		}

		void Solve(int* puzzle, OUT vector<int*>& solutions)
		{
			int i, j, r, c, r2, dir, cand, min, hints = 0; // dir=1: forward; dir=-1: backtrack
			int sr[729];
			int cr[81];
			int sc[324];
			int cc[81];
			int o[81];
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

				int* solution = new int[81];
				memcpy(solution, o, sizeof(o));
				for (j = 0; j < i; ++j)
				{
					r = R[cc[j]][cr[j]];
					solution[r / 9] = r % 9;
				}
				solutions.push_back(solution);
				--i; dir = -1; // backtrack
			}
		}

		void Solve(const char* puzzle, OUT vector<string>& solutions)
		{
			int* a = PuzzleToArray(puzzle);
			vector<int*> solutions1;
			Solve(a, OUT solutions1);
			
			solutions.resize(solutions1.size());
			string s(81, '\0');
			for (int i = 0; i < (int)solutions1.size(); i++)
			{
				solutions[i].resize(81);
				for (int j = 0; j < 81; j++)
					solutions[i][j] = solutions1[i][j] + '1';
				delete[] solutions1[i];
			}
		}
	};

	char* SudokuPuzzles[] = {
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

	string Test()
	{
		const int PuzzleCount = sizeof(SudokuPuzzles) / sizeof(SudokuPuzzles[0]);
		const int LessIterations = Iterations / 100000;

		for (int i = 0; i < LessIterations; i++)
		{
			SudokuSolver a;
			for (int j = 0; j < PuzzleCount; j++)
			{
				vector<string> solutions;
				char* puzzle = SudokuPuzzles[j];
				a.Solve(puzzle, solutions);
				
				#if DEBUG
				if (i == 0) {
					printf("%s has %d solution:", puzzle, solutions.size());
					for (int i = 0; i < solutions.size(); i++) {
						for (int r = 0; r < 9; r++)
							puts(s.substr(r * 9, 9).c_str());
						puts("---------");
					}
				}
				#endif
			}
		}
		return printstring("%dx%d puzzles", PuzzleCount, LessIterations);
	}
}

namespace Polynomials
{
	float dopoly(float x, float pol[]) {
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

	string Test()
	{
		float x = 0.2f;
		float pu = 0.0f;
		float pol[100];

		for(int i=0; i<Iterations; i++)
			pu += dopoly(x, pol);
		return printstring("%f", pu);
	}
}

void Run()
{
	printf("C++ Benchmarks running...\n");
	#ifdef _DEBUG
	printf("DEBUG BUILD!\n");
	#endif

	vector<BenchmarkInfo> methods;

    #ifdef UNDER_CE
    #define ONE_TRIAL_UNDER_CE ,1 /* certain tests are ultra-slow under CE */
    #else
    #define ONE_TRIAL_UNDER_CE
    #endif

	srand(GetTickCount());
	methods.push_back(BenchmarkInfo("Big int hashtable",     IntHashtableTest::Tests));
	methods.push_back(BenchmarkInfo("Big int custom HT",     IntCustomHashtableTest::Tests, 8));
	methods.push_back(BenchmarkInfo("Big int sorted map",    IntMapTest::Tests, 8));
	methods.push_back(BenchmarkInfo("Big string hashtable",  StringHashtableTest::Tests,  4));
	methods.push_back(BenchmarkInfo("Big string custom HT",  StringCustomHashtableTest::Tests,  4));
	methods.push_back(BenchmarkInfo("Square root",           SquareRootTest::Tests));
	methods.push_back(BenchmarkInfo("Simple arithmetic",     SimpleArithmeticTest::Tests));
	methods.push_back(BenchmarkInfo("Generic sum",           GenericSumTest::Tests,  8));
	methods.push_back(BenchmarkInfo("Simple parsing",        SimpleParsingTest::Tests));
	methods.push_back(BenchmarkInfo("Matrix multiply",       MatrixMultiplyTest::Tests  ONE_TRIAL_UNDER_CE));
	methods.push_back(BenchmarkInfo("Sudoku",                Sudoku::Test));
	methods.push_back(BenchmarkInfo("Polynomials",           Polynomials::Test  ONE_TRIAL_UNDER_CE));

	_b.RunAllBenchmarksInConsole(methods, true);

	printf("\n");
	char* filename = "ResultsC++.csv";
	FILE* fp = fopen(filename, "wt");
	if (fp) {
		printf("Writing results to %s. ", filename);
		_b.PrintResults(fp, ",", true);
		fclose(fp);
	}
	printf("Press Enter to quit.");
	fgetc(stdin);
}

#ifdef UNDER_CE
int wmain(int argc, wchar_t* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	Run();
	return 0;
}
