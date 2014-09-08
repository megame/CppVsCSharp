// CppLoops.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "MatrixMultiplication.h"
#include "Polynomial.h"
#include "ParticlesTest.h"
#include "SimpleTimer.h"

using namespace std;


void _tmain()
{
	cout << "C++ Tests" << endl;

	auto timer = SimpleTimer{};

	cout << "Starting Matrix Multiplication..." << endl;

	timer.Reset();

	MatrixMultiplication().Test();

	cout << "Matrix Multiplication Completed in: " << timer.Millisec() / 1000.0 << endl;

	cout << "Starting Polynomial..." << endl;

	timer.Reset();

	Polynomial().Test();

	cout << "Polynomial Completed in: " << timer.Millisec() / 1000.0 << endl;
}

void _tmain2()
{
	cout << "C++ Memory Tests" << endl;

	auto timer = SimpleTimer{};

	cout << "Starting Particle Test..." << endl;

	timer.Reset();

	ParticlesTest::Test();

	cout << "Particle Test Completed in: " << timer.Millisec() / 1000.0 << endl;
	timer.Reset();

	ParticlesTest::VectorTest();

	cout << "Particle Test (Vector) Completed in: " << timer.Millisec() / 1000.0 << endl;
	timer.Reset();

	ParticlesTest::VectorTest2();

	cout << "Particle Test (Vector) 2 Completed in: " << timer.Millisec() / 1000.0 << endl;
}


