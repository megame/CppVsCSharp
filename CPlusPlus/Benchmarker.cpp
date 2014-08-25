#include "stdafx.h"
#include <ctype.h> 
#include <limits>
#include <algorithm>
#include "Misc.h"
#include "Benchmarker.h"
using namespace std;

////////////////////////////////////////////////////////////////////////////////

BenchmarkStatistic::BenchmarkStatistic()
{
	Clear();
}

void BenchmarkStatistic::Clear()
{
	Statistic::Clear();
	First = Last = Errors = 0;
	UserData.clear();
}

void BenchmarkStatistic::Add(double nextValue, const string& userDatum)
{
	Last = nextValue;
	if (Count == 0)
		First = nextValue;

	if (!userDatum.empty())
		UserData.insert(userDatum);

	if (!_finite(nextValue))
		Errors++;
	else
		Statistic::Add(nextValue);
}

////////////////////////////////////////////////////////////////////////////////

const std::string Benchmarker::DiscardResult("(discard result)");

Benchmarker::Benchmarker()
{
	#ifdef UNDER_CE
	// CE has ms-resolution timer and consistent measurements; no need for many trials
	DefaultNumTrials = 2;
	#else
	// Desktop Windows' GetTickCount() has 15ms resolution
	DefaultNumTrials = 5;
	#endif
	UserDataColumnName = "Comment";
}

void Benchmarker::MeasureAndRecord(string name, FastDelegate0<string> code)
{
	string oldActive = _activeBenchmark;
	if (!_activeBenchmark.empty())
		_activeBenchmark += ": " + name;
	else
		_activeBenchmark = name;

	try {
		string userData;
		double time = Measure(code, OUT userData);
		if (userData != DiscardResult)
			Tally(_activeBenchmark, time, userData);
	}
	catch(exception& e)
	{
		++_errors.GetOrAdd(string(e.what()), 0);
		TallyError(_activeBenchmark, typeid(e).name());
	}
	_activeBenchmark = oldActive;
}

double Benchmarker::Measure(FastDelegate0<string> code, OUT string& message)
{
	SimpleTimer timer;
	message = code();
	double time = timer.Millisec() / 1000.0;
	
	if (message.size() > 2 && message[0] == 'o' && message[1] == ':')
	{
		int overhead = atoi(&message[2]);
		message.clear();
		return time - overhead / 1000.0;
	}
	return time;
}

void Benchmarker::TallyError(const string& name, const string& excType)
{
	Tally(name, numeric_limits<double>::quiet_NaN(), excType);
}
void Benchmarker::Tally(const string& name, double seconds, const string& userData)
{
	// For once, this is easier in C++ than C#
	_results[name].Add(seconds, userData);
}

void Benchmarker::RunAllBenchmarks(const vector<BenchmarkInfo> &methods, bool randomOrder, FastDelegate0<> postprocess)
{
	// Prepare the coallated order to do them in
	vector<BenchmarkInfo> order;
	bool done = false;
	for (int j = 0; !done; j++) {
		done = true;
		for (int i = 0; i < (int)methods.size(); i++)
		{
			int trials = methods[i].NumTrials < 0 ? DefaultNumTrials : methods[i].NumTrials;
			if (j < trials) {
				order.push_back(methods[i]);
				done = false;
			}
		}
	}

	// Randomize if requested
	if (randomOrder) {
		for (int i = 0; i < (int)order.size(); i++)
		{
			int j = rand() % order.size();
			std::swap(order[i], order[j]);
		}
	}

	Clear();

	// Finally, do the benchmarks
	for (int i = 0; i < (int)order.size(); i++)
	{
		MeasureAndRecord(order[i].Name, order[i].Method);
		postprocess();
	}
}

void Benchmarker::RunAllBenchmarksInConsole(const vector<BenchmarkInfo> &methods, bool randomOrder)
{
	_rabicEnd = GetCursorTop();
	FastDelegate0<> helper(this, &Benchmarker::RabicHelper);
	RunAllBenchmarks(methods, randomOrder, helper);
	SetCursorPosition(0, _rabicEnd);
}

void Benchmarker::RabicHelper()
{
	int start = GetCursorTop();
	PrintResults(stdout);
	_rabicEnd = GetCursorTop();
	SetCursorPosition(0, start);
}

void Benchmarker::Clear()
{
	_errors.clear();
	_results.clear();
}

void Benchmarker::PrintResults(FILE* writer, const string& separator, bool addPadding)
{
	_printSeparator = separator;
	FastDelegate1<vector<string>&, string> joiner(this, &Benchmarker::JoinStrings);
	PrintResults(writer, separator, addPadding, joiner);
}

string Benchmarker::JoinStrings(vector<string>& list)
{
	string out;
	for (int i = 0; i < (int)list.size(); i++) {
		if (i > 0)
			out += _printSeparator;
		out += list[i];
	}
	return out;
}

void Benchmarker::PrintResults(FILE* writer, const string& separator, bool addPadding, FastDelegate1<vector<string>&, string> userDataFormatter)
{
	PrintResults(writer, Results(), separator, addPadding, userDataFormatter, UserDataColumnName);

	// Print errors
	if (_errors.size() > 0) {
		printf("Errors occurred:");
		map<string,int>::iterator it = _errors.begin();
		for (; it != _errors.end(); ++it)
			printf("%s x%d", it->first, it->second);
	}
}

void Benchmarker::PrintResults(FILE* writer, const EasyMap<string, BenchmarkStatistic>& results, const string& separator, 
	bool addPadding, FastDelegate1<vector<string>&, string> userDataFormatter, const string& userDataColumnName)
{
	map<string, BenchmarkStatistic>::const_iterator it;
	int maxCount = 0;
	bool haveUserData = false;
	for (it = results.begin(); it != results.end(); ++it) {
		maxCount = max(maxCount, it->second.Count);
		haveUserData |= it->second.UserData.size() != 0;
	}
			
	// Prepare a list of columns
	vector<ColInfo> columns;
	columns.push_back(ColInfo("Test name", GetColumn(&PrTestName)));
	columns.push_back(ColInfo("#", GetColumn(&PrCount)));
	if (maxCount >= 2)
		columns.push_back(ColInfo("Average", GetColumn(&PrAverage)));
	columns.push_back(ColInfo("First", GetColumn(&PrFirst)));
	if (maxCount >= 3) {
		columns.push_back(ColInfo("Max", GetColumn(&PrMax)));
		columns.push_back(ColInfo("Min", GetColumn(&PrMin)));
		columns.push_back(ColInfo("Std.Dev", GetColumn(&PrStdDev)));
	}
	if (haveUserData) {
		GetColumn gc(this, &Benchmarker::PrUserData);
		columns.push_back(ColInfo(userDataColumnName, gc));
	}

	_userDataFormatter = userDataFormatter;

	if (addPadding)
		for (int i = 0; i < (int)columns.size(); i++) {
			int max = (int)columns[i].ColName.length();
			for (it = results.begin(); it != results.end(); ++it) {
				string text = columns[i].Getter(it->first, it->second);
				if (max < (int)text.length())
					max = (int)text.length();
			}
			columns[i].Width = max;
		}

	// Now sort (case insensitively) and print the results
	PrintRow(writer, columns, separator, NULL, NULL, GetColumn2(&PrColName));
	vector< pair<string, BenchmarkStatistic> > results2(results.begin(), results.end());
	sort(results2.begin(), results2.end(), &IsKeyLessCaseInsensitive<BenchmarkStatistic>);
	for (size_t i = 0; i < results2.size(); i++)
		PrintRow(writer, columns, separator, &results2[i].first, &results2[i].second, GetColumn2(&PrColGetter));
}

string Benchmarker::PrUserData(const string&, const BenchmarkStatistic& s)
{
	vector<string> data;
	for (set<string>::const_iterator it = s.UserData.begin(); it != s.UserData.end(); ++it)
		data.push_back(*it);
	return _userDataFormatter(data);
}

void Benchmarker::PrintRow(FILE* writer, vector<ColInfo>& columns, const string& separator, const string* benchmarkName, const BenchmarkStatistic* stats, GetColumn2 getter)
{
	for (int i = 0; i < (int)columns.size(); i++)
	{
		if (i > 0)
			fprintf(writer, "%s", separator.c_str());
		
		string text = getter(benchmarkName, stats, columns[i]);
		
		bool hasLetters = false;
		for (int j = 0; j < (int)text.length(); j++)
			if (isalpha(text[j]))
				hasLetters = true;

		int w = columns[i].Width;
		fprintf(writer, "%*s", hasLetters ? -w : w, text.c_str());
	}
	fprintf(writer, "\n");
}
