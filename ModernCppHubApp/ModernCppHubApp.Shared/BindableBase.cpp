#include "pch.h"
#include "BindableBase.h"

using namespace Platform;

namespace ModernCppHubApp
{
	void BindableBase::OnPropertyChanged(String^ propertyName)
	{
		PropertyChanged(this, ref new PropertyChangedEventArgs(propertyName));
	}

	ICustomProperty^ BindableBase::GetCustomProperty(String^ name)
	{
		return nullptr;
	}

	ICustomProperty^ BindableBase::GetIndexedProperty(String^ name, TypeName type)
	{
		return nullptr;
	}

	String^ BindableBase::GetStringRepresentation()
	{
		return this->ToString();
	}
}