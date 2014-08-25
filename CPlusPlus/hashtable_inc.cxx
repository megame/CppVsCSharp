//
// DO NOT COMPILE ALONE.
//
// This is included from Benchmarks.cpp
//

#define PASSTHRU(_x_) _x_
#define CONCAT(_x_, _y_) PASSTHRU(_x_) ## PASSTHRU(_y_)

namespace CONCAT(Int, HASHTABLE_NAMESPACE)
{
	HASHTABLE<int, int> _dict;

	string TestAdding()
	{
		HASHTABLE<int, int>& dict = _dict;
		for (int i = 0; i < Iterations; i++) {
			if ((int)dict.size() >= MapSizeLimit)
				dict.clear();
			dict[i ^ 314159] = i;
		}
		return string();
	}
	string TestQueries()
	{
		HASHTABLE<int, int>& dict = _dict;
		int misses = 0;
		for (int i = 0; i < Iterations; i++)
		{
			HASHTABLE<int,int>::iterator it = dict.find(i ^ 314159);
			if (it == dict.end())
				misses++;
		}
		return printstring("%d%% misses", misses * 100 / Iterations);
	}
	string TestRemoval()
	{
		HASHTABLE<int, int>& dict = _dict;
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

namespace CONCAT(String, HASHTABLE_NAMESPACE)
{
	HASHTABLE<string, string> _dict;

	char* ToString(int i)
	{
		static char temp[20];
		return _itoa(i, temp, 10);
	}
	string TestGenerateStrings()
	{
		// This just measures how long it takes to generate the strings used 
		// in the rest of the tests, in case you would like to mentally 
		// subtract this time from the results.
		for (int i = 0; i < Iterations; i++) {
			string s = ToString(i);
		}
		return string();
	}
	string TestAddSet()
	{
		HASHTABLE<string, string>& dict = _dict;
		for (int i = 0; i < Iterations; i++) {
			if ((int)dict.size() >= MapSizeLimit)
				dict.clear();
			string s = ToString(i ^ 314159);
			dict[s] = s;
		}
		return string();
	}
	string TestQueries()
	{
		HASHTABLE<string, string>& dict = _dict;
		int misses = 0;
		for (int i = 0; i < Iterations; i++)
		{
			string s = ToString(i ^ 314159);
			HASHTABLE<string,string>::iterator it = dict.find(s);
			if (it == dict.end())
				misses++;
		}
		return printstring("%d%% misses", misses * 100 / Iterations);
	}
	string TestRemoval()
	{
		HASHTABLE<string, string>& dict = _dict;
		int removed = 0;
		for (int i = 0; i < Iterations; i++)
		{
			string s = ToString(i ^ 314159);
			if (dict.erase(s))
				removed++;
		}
		return printstring("%d removed", removed);
	}
	string Tests()
	{
		_dict.clear();
		_b.MeasureAndRecord("0 Ints to strings", TestGenerateStrings);
		_b.MeasureAndRecord("1 Adding/setting", TestAddSet);
		_b.MeasureAndRecord("2 Running queries", TestQueries);
		_b.MeasureAndRecord("3 Removing items", TestRemoval);
		return Benchmarker::DiscardResult;
	}
}

