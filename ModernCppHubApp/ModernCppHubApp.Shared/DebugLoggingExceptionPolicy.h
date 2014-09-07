#pragma once

#include "ExceptionPolicy.h"

namespace ModernCppHubApp
{
	class DebugLoggingExceptionPolicy : 
		public ExceptionPolicy
	{
	public:
		virtual void HandleException(Exception^ exception);
	};
}