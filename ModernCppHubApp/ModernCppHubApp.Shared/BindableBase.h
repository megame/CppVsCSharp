#pragma once

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;

namespace ModernCppHubApp
{
	[Windows::Foundation::Metadata::WebHostHiddenAttribute]
	public ref class BindableBase : DependencyObject,
		INotifyPropertyChanged,
		ICustomPropertyProvider
	{
	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;

	public:
		// ICustomPropertyProvider 
		virtual ICustomProperty^ GetCustomProperty(Platform::String^ name);
		virtual ICustomProperty^ GetIndexedProperty(Platform::String^ name, TypeName type);
		virtual Platform::String^ GetStringRepresentation();

		property TypeName Type
		{
			virtual TypeName get() { return this->GetType(); }
		}


	protected:
		virtual void OnPropertyChanged(Platform::String^ propertyName);
	};

}