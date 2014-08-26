// CppLoops.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

struct SimpleTimer
{
	DWORD Start;
	SimpleTimer() { Start = GetTickCount(); }
	int Millisec() const { return GetTickCount() - Start; }
	int Reset() {
		int elapsed = Millisec();
		Start += elapsed;
		return elapsed;
	}

	int ResetAfter(int ms)
	{
		int elapsed = Millisec();
		if (elapsed >= ms) {
			Start += elapsed;
			return elapsed;
		}
		return false;
	}
};

vector<int> GenerateBuffer()
{
	const long size = 1024 * 100000;
	auto buffer = vector<int>();

	buffer.reserve(size);

	for (long i = 0; i < size; i++)
		buffer.push_back(i);

	return buffer;
}

void ProcessBuffer(vector<BYTE> & buffer)
{
	long size = buffer.size() - 8;
	for (long i = 0; i<size; i += 4)
	{
		int rr = buffer[i + 0] * buffer[i + 4];
		int rg = buffer[i + 1] * buffer[i + 5];
		int rb = buffer[i + 2] * buffer[i + 6];

		buffer[i + 0] = (BYTE) (rr * 0.393 + rg * 0.769 + rb * 0.189);
		buffer[i + 1] = (BYTE) (rr * 0.349 + rg * 0.686 + rb * 0.168);
		buffer[i + 2] = (BYTE) (rr * 0.272 + rg * 0.534 + rb * 0.131);
	}
}

void ProcessBuffer2(int * const a, int const * const b, int const * const c, const int size)
{
	for (int i = 0; i < size; i++)
		a[i] = b[i] * 0.633 + c[i] * 0.318 + 16;
}

void _tmain()
{
	cout << "C++" << endl;
	cout << "Generating buffer..." << endl;

	auto a = GenerateBuffer();
	auto b = GenerateBuffer();
	auto c = GenerateBuffer();

	auto timer = SimpleTimer{};
	
	cout << "Starting..." << endl;

	timer.Reset();

	for (int i = 0; i < 24; i++)
	{
		//ProcessBuffer(memory);
		ProcessBuffer2(a.data(), b.data(), c.data(), a.size());
	}

	cout << "Completed in: " << timer.Millisec() / 1000.0 << endl;
}


