#pragma once

#include "ViewModelBase.h"

using namespace Platform;
using namespace Windows::UI::Xaml::Input;

namespace ModernCppHubApp
{
	[Windows::UI::Xaml::Data::Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class MatrixMultiplicationViewModel sealed :
		public ViewModelBase
	{
	internal:
		MatrixMultiplicationViewModel(shared_ptr<ExceptionPolicy> exceptionPoilicy);

	public:
		property ICommand^ RunCommand;
		property bool IsRunning;
		property IObservableVector<String^>^ Results;

	protected:
		void Run(Object^ parameter);
		bool CanRun(Object^ parameter);
	};

}