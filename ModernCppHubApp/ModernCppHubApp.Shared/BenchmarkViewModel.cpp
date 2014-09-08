#include "pch.h"
#include "BenchmarkViewModel.h"
#include "DelegateCommand.h"
#include "SimpleTimer.h"
#include "helpers.h"

using namespace concurrency;
using namespace Platform::Collections;

namespace ModernCppHubApp
{
	BenchmarkViewModel::BenchmarkViewModel(
		shared_ptr<ExceptionPolicy> exceptionPoilicy,
		BenchmarkFunc benchmark) :
		ViewModelBase(exceptionPoilicy),
		_benchmark(benchmark)
	{
		RunCommand = ref new DelegateCommand(
			ref new ExecuteDelegate(this, &BenchmarkViewModel::Run),
			ref new CanExecuteDelegate([this](Object^) { return  CanRun; }));

		CanRun = true;
		Results = ref new Vector<String^>();
	}

	void BenchmarkViewModel::IsRunning::set(bool value)
	{
		CanRun = !value;
		OnPropertyChanged(L"IsRunning");
		OnPropertyChanged(L"CanRun");
		RunCommand->CanExecute(nullptr);
	}

	void BenchmarkViewModel::Run(Object ^)
	{
		IsRunning = true;

		auto async = create_async([this] 
		{
			auto result = _benchmark();
			return ref new String(result.data());
		});

		create_task(async).then([this](String^ result)
		{
			IsRunning = false;

			Results->Append(result);
		});
	}
}