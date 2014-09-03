#include "stdafx.h"
#include "MatrixMultiplication.h"
#include "helpers.h"

using namespace std;

const int MatrixSize = 1024;

static unique_ptr<float> GenerateMatrix(int n)
{
	auto ua = unique_ptr<float>(new float[n * n]);
	auto a = ua.get();

	float tmp = 1.0 / n / n;
	for (int i = 0; i < n; ++i) 
	{
		float* row = &a[i * n];
		for (int j = 0; j < n; ++j)
			row[j] = tmp * (i - j) * (i + j);
	}

	return ua;
}

static unique_ptr<float> MultiplyMatrix(float* a, float* b, int aRows, int aCols, int bCols)
{
	// int m = aRows, n = bCols, p = aCols; 
	// Note: as is conventional in C#/C/C++, a and b are in row-major order.
	// Note: bRows (the number of rows in b) must equal aCols.
	int bRows = aCols;
	auto ux = unique_ptr<float>(new float[aRows * bCols]); // result
	auto x = ux.get();
	auto uc = unique_ptr<float>(new float[bRows * bCols]);
	auto c = uc.get();

	for (int i = 0; i < aCols; ++i) // transpose (large-matrix optimization)
		for (int j = 0; j < bCols; ++j)
			c[j*bRows + i] = b[i*bCols + j];

	for (int i = 0; i < aRows; ++i) {
		float* a_i = &a[i*aCols];
		for (int j = 0; j < bCols; ++j)
		{
			float* c_j = &c[j*bRows];
			float s = 0.0;
			for (int k = 0; k < aCols; ++k)
				s += a_i[k] * c_j[k];
			x[i*bCols + j] = s;
		}
	}
	
	return ux;
}

std::string MatrixMultiplication::Test()
{
	auto deleter = [](float *p) { delete [] p; };
	auto a = unique_ptr<float>(GenerateMatrix(MatrixSize));
	auto b = unique_ptr<float>(GenerateMatrix(MatrixSize));
	
	auto x = MultiplyMatrix(a.get(), b.get(), MatrixSize, MatrixSize, MatrixSize);

	auto result = x.get()[MatrixSize / 2 * MatrixSize + MatrixSize / 2];

	return printstring("%f", result);
}
