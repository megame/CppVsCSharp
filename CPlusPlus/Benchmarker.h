#ifndef _BENCHMARKER_H
#define _BENCHMARKER_H

#include <set>
#include <vector>
#include <string>
#include "EasyMap.h"
#include "FastDelegate.h"
#include "Misc.h"
using namespace fastdelegate;

class BenchmarkStatistic : public Statistic {
public:
	int Errors; // Number of trials that ended in an exception
	double First;
	double Last;
	std::set<std::string> UserData;
	
	BenchmarkStatistic();
	void Clear();
	void Add(double nextValue) { Add(nextValue, NULL); }
	void Add(double nextValue, const std::string& userDatum);
};

struct BenchmarkInfo
{
	BenchmarkInfo() : NumTrials(-1) { } // -1 for default # of trials
	BenchmarkInfo(const std::string& name, FastDelegate0<std::string> method, int numTrials = -1)
		: Name(name), Method(method), NumTrials(numTrials) { }
	std::string Name;
	FastDelegate0<std::string> Method;
	int NumTrials;
};

/// <summary>
/// A simple benchmarking framework that produces a fairly nice result table.
/// </summary>
/// <remarks>
/// This framework is designed mainly to measure the time that benchmarks
/// require, but it can also report information supplied by the benchmarks
/// themselves (see below).
/// <para/>
/// The main methods of interest are <see cref="RunAllBenchmarks"/>,
/// <see cref="RunAllBenchmarksInConsole"/>, <see cref="MeasureAndRecord"/>
/// and <see cref="PrintResults"/> (if you want to print the results to a file).
/// <para/>
/// Benchmarker is designed for relatively long-running benchmarks (100+
/// milliseconds) because Environment.TickCount is used for measurements, which
/// typically has a resolution of 15.6 ms under Windows. Also, benchmark methods 
/// are invoked via reflection. Therefore, if you are testing a small operation,
/// you should write a loop with many iterations (up to millions).
/// <para/>
/// Benchmarker normally runs several trials, in order to detect any "jitter" in
/// the results. It normally shows the first-run time, average time, 
/// minimum running time, maximum time, and standard deviation, but the min, max
/// and standard deviation columns are not shown when running less than 3 trials.
/// If the result table shows "Min" and "Max" values that are quite different, 
/// and the "Std.Dev" (standard deviation) is high, there's probably something 
/// weird going on. It is not unusual, however, that the first trial takes 
/// slightly longer (as the .NET Framework JITs it on the first run).
/// <para/>
/// Benchmarker can run benchmarks either in collated (alphabetical) order or in
/// a random order that mixes different trials together.
/// <para/>
/// Benchmark trials can return comments or other information. Benchmarker
/// builds a list of unique objects returned from benchmarks (duplicate datums
/// from the same benchmark, and null datums, are ignored). By default, 
/// PrintResults() converts this user-defined data to strings, concatenates the 
/// data for each benchmark, and prints it out as the last column. You can 
/// customize the way that the data are aggregated into a std::string via the 
/// userDataFormatter parameter. The <see cref="UserDataColumnName"/> attribute 
/// controls the name of this final column. The final column will not be shown 
/// if none of the benchmarks returned any (non-null) data.
/// </remarks>
class Benchmarker {
public:
	Benchmarker();
	
	/// <summary>Number of times that RunAllBenchmarksInConsole or
	/// RunAllBenchmarks should run each benchmark method that doesn't specify
	/// how many trials to run.</summary>
	int DefaultNumTrials;
	
	/// <summary>If benchmarks return data, it will be displayed in the last
	/// column, which is given this column heading.</summary>
	std::string UserDataColumnName;

	static const std::string DiscardResult;
	static std::string SubtractOverhead(int millisec) { return printstring("o:%d", millisec); }

protected:
	std::string _activeBenchmark;
	EasyMap<std::string, BenchmarkStatistic> _results;
	EasyMap<std::string, int> _errors;

	// Stuff used within PrintResults()
	struct ColInfo;
	typedef FastDelegate2<const std::string&, const BenchmarkStatistic&, std::string> GetColumn;
	typedef FastDelegate3<const std::string*, const BenchmarkStatistic*, ColInfo&, std::string> GetColumn2;

	struct ColInfo 
	{
		ColInfo(const std::string& name, GetColumn getter) { ColName = name; Getter = getter; }
		std::string ColName;
		GetColumn Getter;
		int Width;
	};

public:
	const EasyMap<std::string, BenchmarkStatistic>& Results() const { return _results; }

	/// <summary>
	/// Measures and records the time required for a given piece of code to run.
	/// Supports nested measurements.
	/// </summary>
	/// <param name="name">Name of a benchmark or sub-benchmark.</param>
	/// <param name="code">Code to run.</param>
	/// <remarks>A benchmark method called by RunAllBenchmarks can call this
	/// method to run a sub-benchmark. A row on the results table will be created
	/// with both the name of the outer benchmark and the sub-benchmark. The
	/// outer benchmark's time will include time used by the sub-benchmark.</remarks>
	void MeasureAndRecord(std::string name, FastDelegate0<std::string> code);

	/// <summary>Runs a piece of code and returns the number of seconds it required.</summary>
	/// <remarks>Garbage-collects before the test if DoGC is true.</remarks>
	double Measure(FastDelegate0<std::string> code, OUT std::string& message);

protected:
	void TallyError(const std::string& name, const std::string& excType);
	void Tally(const std::string& name, double seconds, const std::string& userData);

public:
	/// <summary>
	/// Runs a series of benchmarks the number of times specified by DefaultNumTrials,
	/// and records the results. Reflection is impossible in C++ so the caller must 
	/// supply a list of methods of type FastDelegate0<std::string> in an STL collection.
	/// </summary>
	/// <param name="randomOrder">If true, the order in which the methods are run
	/// is randomized, different trials are even mixed together. If false, the
	/// methods are run in order, collated, sorted by method name.</param>
	/// <param name="postprocess">A method to run after each trial, if desired, 
	/// or null.</param>
	/// <remarks>Existing results are clear()ed before running the benchmarks.</remarks>
	void RunAllBenchmarks(const std::vector<BenchmarkInfo> &methods, bool randomOrder, FastDelegate0<> postprocess);

	/// <summary>Runs all public benchmark methods (static and nonstatic) in the
	/// specified object ('set') and prints results to the console. Each method 
	/// is run the number of times indicated by the Trials attribute of
	/// BenchmarkAttribute (if provided) or DefaultNumTrials.</summary>
	void RunAllBenchmarksInConsole(const std::vector<BenchmarkInfo> &methods, bool randomOrder);

private:
	int _rabicEnd;
	void RabicHelper();
public:	

	/// <summary>Deletes all benchmark results within this object.</summary>
	void Clear();

	/// <summary>Prints all recorded results to the specified stream.</summary>
	/// <param name="writer">An output stream.</param>
	void PrintResults(FILE* writer)
	{
		PrintResults(writer, "|", true);
	}
	/// <summary>Prints all recorded results to the specified stream.</summary>
	/// <param name="writer">An output stream.</param>
	/// <param name="separator">Separator between table columns.</param>
	/// <param name="addPadding">If true, columns are space-padded to equal width.</param>
	/// <param name="userDataFormatter">A function that converts a list of
	/// user-defined data to a std::string, or null for the default behavior, which
	/// converts the data to strings and concatenates them.</param>
	void PrintResults(FILE* writer, const std::string& separator, bool addPadding);

	std::string _printSeparator;
	std::string JoinStrings(std::vector<std::string>& list);

	void PrintResults(FILE* writer, const std::string& separator, bool addPadding, FastDelegate1<std::vector<std::string>&, std::string> userDataFormatter);

	/// <summary>Prints a list of results to the specified stream.</summary>
	/// <param name="writer">An output stream.</param>
	/// <param name="results">A list of benchmark results.</param>
	/// <param name="separator">Separator between table columns.</param>
	/// <param name="addPadding">If true, columns are space-padded to equal width.</param>
	/// <param name="userDataFormatter">A function that converts a list of
	/// user-defined data to a std::string, or null for the default behavior, which
	/// converts the data to strings and concatenates them.</param>
	/// <param name="userDataColumnName">Name of user-defined data column.</param>
	void PrintResults(FILE* writer, const EasyMap<std::string, BenchmarkStatistic>& results, const std::string& separator, 
		bool addPadding, FastDelegate1<std::vector<std::string>&, std::string> userDataFormatter, const std::string& userDataColumnName);

private:
	// Helper functions for PrintResults(), which were lambdas in the original C#
	static std::string PrTestName(const std::string& name, const BenchmarkStatistic&) { return name; }
	static std::string PrCount   (const std::string&, const BenchmarkStatistic& s) { return printstring("%d", s.Count); }
	static std::string PrAverage (const std::string&, const BenchmarkStatistic& s) { return printstring("%0.3f", s.Avg()); }
	static std::string PrFirst   (const std::string&, const BenchmarkStatistic& s) { return printstring("%0.3f", s.First); }
	static std::string PrMax     (const std::string&, const BenchmarkStatistic& s) { return printstring("%0.3f", s.Max); }
	static std::string PrMin     (const std::string&, const BenchmarkStatistic& s) { return printstring("%0.3f", s.Min); }
	static std::string PrStdDev  (const std::string&, const BenchmarkStatistic& s) { return printstring("%0.3f", s.StdDeviation()); }

	// Helper functions for PrintResults() that take additional information
	FastDelegate1<std::vector<std::string>&, std::string> _userDataFormatter;
	std::string PrUserData(const std::string&, const BenchmarkStatistic& s);
	static std::string PrColName(const std::string* benchmarkName, const BenchmarkStatistic* stats, ColInfo& col) { return col.ColName; }
	static std::string PrColGetter(const std::string* benchmarkName, const BenchmarkStatistic* stats, ColInfo& col) { return col.Getter(*benchmarkName, *stats); }
	
	static void PrintRow(FILE* writer, std::vector<ColInfo>& columns, const std::string& separator, const std::string* benchmarkName, const BenchmarkStatistic* stats, GetColumn2 getter);

public:
	template<class V>
	static bool IsKeyLessCaseInsensitive(const std::pair<std::string, V>& a, const std::pair<std::string, V>& b)
	{
		const std::string& a1 = a.first;
		const std::string& b1 = b.first;

		return _stricmp(a1.c_str(), b1.c_str()) < 0;
	}
};

#endif