#ifndef STATISTIC_H
#define STATISTIC_H

#include <math.h>
#include <float.h>

/// <summary>
/// A lightweight class to help you compute the minimum, maximum, average
/// and standard deviation of a set of values. Call Clear(), then Add(each
/// value); you can compute the average and standard deviation at any time by 
/// calling Avg() and StdDeviation(). Also tracks the first and last value.
/// </summary>
class Statistic {
public:
	double Min;
	double Max;
	double SumTotal;
	double SumOfSquares;
	int Count;

	void Clear()
	{
		SumOfSquares = Min = Max = SumTotal = 0;
		Count = 0;
	}
	void Add(double nextValue)
	{
		assert(_finite(nextValue));
		if (Count > 0)
		{
			if (Min > nextValue)
				Min = nextValue;
			if (Max < nextValue)
				Max = nextValue;
			SumTotal += nextValue;
			SumOfSquares += nextValue * nextValue;
			Count++;
		}
		else
		{
			Min = Max = SumTotal = nextValue;
			SumOfSquares = nextValue * nextValue;
			Count = 1;
		}
	}
	double Avg() const
	{
		return SumTotal / Count;
	}
	double Variance() const
	{
		return (SumOfSquares * Count - SumTotal * SumTotal) / (Count * (Count - 1));
	}
	double StdDeviation() const
	{
		double v = Variance();
		assert(_isnan(v) || v > -0.00001); // can be negative due to a rounding error
		if (v < 0)
			return 0;
		return sqrt(v);
	}
};

#endif
