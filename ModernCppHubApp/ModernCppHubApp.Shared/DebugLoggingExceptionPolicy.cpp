#include "pch.h"
#include "DebugLoggingExceptionPolicy.h"

using namespace std;

namespace ModernCppHubApp
{

	void DebugLoggingExceptionPolicy::HandleException(Exception^ exception)
	{
#ifdef _DEBUG
		assert(exception != nullptr);

		//wstringstream ss;

		//ss << L"[HR: " << exception->HResult << L"] " << exception->Message->Data();
		//OutputDebugString(ss.str().c_str());
#endif
	}

}