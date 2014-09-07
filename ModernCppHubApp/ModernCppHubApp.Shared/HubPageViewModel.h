#pragma once

#include "ViewModelBase.h"
#include "MatrixMultiplicationViewModel.h"

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
		property MatrixMultiplicationViewModel^ MatrixMultiplication;
	};

}