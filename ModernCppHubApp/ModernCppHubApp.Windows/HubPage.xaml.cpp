//
// HubPage.xaml.cpp
// Implementation of the HubPage class
//

#include "pch.h"
#include "HubPage.xaml.h"

using namespace ModernCppHubApp;
using namespace ModernCppHubApp::Common;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;


HubPage::HubPage()
{
	InitializeComponent();
}

void HubPage::Hub_SectionHeaderClick(Object^ sender, HubSectionHeaderClickEventArgs^ e)
{
	auto section = e->Section;
	auto vm = safe_cast<BenchmarkViewModel^>(section->DataContext);
	auto cmd = vm->RunCommand;
	
	if (cmd->CanExecute(nullptr))
		cmd->Execute(nullptr);
}