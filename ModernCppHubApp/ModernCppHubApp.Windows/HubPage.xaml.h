//
// HubPage.xaml.h
// Declaration of the HubPage class
//

#pragma once

#include "pch.h"
#include "ViewModelLocator.h"
#include "HubPage.g.h"

namespace ModernCppHubApp
{
	public ref class HubPage sealed
	{
	public:
		HubPage();

	private:
		void Hub_SectionHeaderClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::HubSectionHeaderClickEventArgs^ e);
	};
}
