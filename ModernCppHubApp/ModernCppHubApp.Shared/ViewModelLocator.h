#pragma once

#include "HubPageViewModel.h"

namespace ModernCppHubApp
{
	class ExceptionPolicy;

	[Bindable, Windows::Foundation::Metadata::WebHostHidden]
	public ref class ViewModelLocator sealed
	{
	public:
		ViewModelLocator();
		
	public:
		property HubPageViewModel^ HubPageVM { HubPageViewModel^ get(); }

	private:
		shared_ptr<ExceptionPolicy> m_exceptionPolicy;
	};
}