#pragma once

using namespace Platform;

namespace ModernCppHubApp
{
	class ExceptionPolicy
	{
	public:
		virtual void HandleException(Exception^ exception) = 0;
	};
}