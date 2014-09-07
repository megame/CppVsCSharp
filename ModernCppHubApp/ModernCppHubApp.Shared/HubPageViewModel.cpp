#include "pch.h"
#include "HubPageViewModel.h"

namespace ModernCppHubApp
{

	HubPageViewModel::HubPageViewModel(shared_ptr<ExceptionPolicy> exceptionPolicy):
		ViewModelBase(exceptionPolicy)
	{
		MatrixMultiplication = ref new MatrixMultiplicationViewModel(exceptionPolicy);
	}

}