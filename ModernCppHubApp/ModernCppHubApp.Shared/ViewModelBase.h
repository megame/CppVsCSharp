#pragma once

#include "BindableBase.h"
#include "PageType.h"

using namespace std;
using namespace Platform;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation::Collections;

namespace ModernCppHubApp
{
	class ExceptionPolicy;

	public delegate void NavigateEventHandler();
	public delegate void PageNavigateEventHandler(PageType page, Object^ parameter);

	public ref class ViewModelBase : 
		public BindableBase
	{
	internal:
		ViewModelBase(shared_ptr<ExceptionPolicy> exceptionPolicy);

		event NavigateEventHandler^ NavigateBack;
		event NavigateEventHandler^ NavigateHome;
		event PageNavigateEventHandler^ NavigateToPage;

		virtual void LoadState(IMap<String^, Object^>^ stateMap);
		virtual void SaveState(IMap<String^, Object^>^ stateMap);
		virtual void OnNavigatedTo(NavigationEventArgs^ e);
		virtual void OnNavigatedFrom(NavigationEventArgs^ e);

	public:
		property bool IsAppBarEnabled { bool get(); void set(bool value); }
		property bool IsAppBarOpen { bool get(); void set(bool value); }
		property bool IsAppBarSticky { bool get(); void set(bool value); }

	protected private:
		virtual void GoBack();
		virtual void GoHome();
		virtual void GoToPage(PageType page, Object^ parameter);

		bool m_isAppBarEnabled;
		bool m_isAppBarOpen;
		bool m_isAppBarSticky;

		shared_ptr<ExceptionPolicy> m_exceptionPolicy;
	};

}