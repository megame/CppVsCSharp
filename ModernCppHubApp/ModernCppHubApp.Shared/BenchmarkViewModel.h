#pragma once

#include "pch.h"
#include "ViewModelBase.h"

using namespace Platform;
using namespace Windows::UI::Xaml::Input;

namespace ModernCppHubApp
{
	[Bindable]
	public ref class BenchmarkViewModel :
		public ViewModelBase
	{
		typedef function<wstring()> BenchmarkFunc;

	internal:
		BenchmarkViewModel(
			shared_ptr<ExceptionPolicy> exceptionPoilicy,
			BenchmarkFunc benchmark);

	public:
		property ICommand^ RunCommand;
		property bool CanRun;
		property bool IsRunning { bool get() { return !CanRun; }; void set(bool value); }
		property IObservableVector<String^>^ Results;

	private:
		void Run(Object^ parameter);

	private:
		BenchmarkFunc _benchmark;
	};

}