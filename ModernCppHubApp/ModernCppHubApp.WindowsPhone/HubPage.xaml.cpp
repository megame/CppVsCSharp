//
// HubPage.xaml.cpp
// Implementation of the HubPage class
//

#include "pch.h"
#include "HubPage.xaml.h"
#include "ViewModelLocator.h"

using namespace ModernCppHubApp;
using namespace ModernCppHubApp::Common;
using namespace ModernCppHubApp::Data;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

// The Universal Hub Application project template is documented at http://go.microsoft.com/fwlink/?LinkID=391958

HubPage::HubPage()
{
	InitializeComponent();

	DisplayInformation::AutoRotationPreferences = DisplayOrientations::Portrait;
	NavigationCacheMode = Windows::UI::Xaml::Navigation::NavigationCacheMode::Required;
}

void ModernCppHubApp::HubPage::Hub_SectionHeaderClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::HubSectionHeaderClickEventArgs^ e)
{
	(void) sender;	// Unused parameter

	auto vm = safe_cast<BenchmarkViewModel^>(e->Section->DataContext);
	auto cmd = vm->RunCommand;

	if (cmd->CanExecute(nullptr))
		cmd->Execute(nullptr);
}
