//
// HubPage.xaml.h
// Declaration of the HubPage class
//

#pragma once

#include "HubPage.g.h"
#include "ViewModelLocator.h"

using namespace Windows::UI::ViewManagement;

namespace ModernCppHubApp
{
	/// <summary>
	/// A page that displays a grouped collection of items.
	/// </summary>
	public ref class HubPage sealed
	{
	public:
		HubPage();

	private:
		void Hub_SectionHeaderClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::HubSectionHeaderClickEventArgs^ e);
	};
}
