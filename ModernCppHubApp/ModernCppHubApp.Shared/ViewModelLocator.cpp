#include "pch.h"
#include "ViewModelLocator.h"
#include "DebugLoggingExceptionPolicy.h"

namespace ModernCppHubApp
{

	ViewModelLocator::ViewModelLocator() :
		m_exceptionPolicy{ make_shared<DebugLoggingExceptionPolicy>() }
	{
	}


	HubPageViewModel^ ViewModelLocator::HubPageVM::get()
	{
		return ref new HubPageViewModel(m_exceptionPolicy);
	}

}