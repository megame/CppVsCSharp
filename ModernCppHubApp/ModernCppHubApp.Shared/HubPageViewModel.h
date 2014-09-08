#pragma once

#include "ViewModelBase.h"
#include "BenchmarkViewModel.h"

namespace ModernCppHubApp
{
	[Bindable]
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class HubPageViewModel sealed :
		public ViewModelBase
	{
	internal:
		HubPageViewModel(shared_ptr<ExceptionPolicy> exceptionPolicy);

	public:
		property BenchmarkViewModel^ MatrixMultiplicationSection;
		property BenchmarkViewModel^ PolynomialSection;
		property BenchmarkViewModel^ ParticlesSection;
	};

}