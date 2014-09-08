#include "pch.h"
#include "HubPageViewModel.h"

#include "SimpleTimer.h"
#include "helpers.h"

#include "MatrixMultiplication.h"
#include "Polynomial.h"
#include "ParticlesTest.h"


namespace ModernCppHubApp
{
	template<typename T>
	wstring benchmarkRunner()
	{
		SimpleTimer timer;
		
		T().Test();

		auto result = printwstring(L"%.2f seconds", timer.Millisec() / 1000.0f);

		return result;
	}

	HubPageViewModel::HubPageViewModel(shared_ptr<ExceptionPolicy> exceptionPolicy):
		ViewModelBase(exceptionPolicy)
	{
		MatrixMultiplicationSection = ref new BenchmarkViewModel(
			exceptionPolicy,
			benchmarkRunner<MatrixMultiplication>);
		PolynomialSection = ref new BenchmarkViewModel(
			exceptionPolicy,
			benchmarkRunner<Polynomial>);
		ParticlesSection = ref new BenchmarkViewModel(
			exceptionPolicy,
			benchmarkRunner<ParticlesTest>);
	}

}