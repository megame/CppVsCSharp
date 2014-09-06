#pragma once

class SimpleTimer
{
	ULONGLONG Start;

public:
	SimpleTimer() 
	{ 
		Start = GetTickCount64(); 
	}

	int Millisec() const 
	{ 
#pragma warning(disable: 4244)
		return GetTickCount64() - Start; 
	}

	int Reset()
	{
		int elapsed = Millisec();
		Start += elapsed;
		return elapsed;
	}

	int ResetAfter(int ms)
	{
		int elapsed = Millisec();
		if (elapsed >= ms) 
		{
			Start += elapsed;
			return elapsed;
		}
		return false;
	}
#pragma warning(default: 4244)
};