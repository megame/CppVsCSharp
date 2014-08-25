//
// SimpleTimer.h
// Author: David Piepgrass
// Copyright 2009 Mentor Engineering, Inc.
//
#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H

#include <assert.h>

// A simple stopwatch. The timer starts in the constructor and you can call
// Millisec() to get the elapsed time in milliseconds. ResetAfter(N) resets 
// Millisec() to zero if at least N milliseconds have elapsed. If ResetAfter 
// resets the timer then it returns the elapsed time, otherwise it returns 0.
struct SimpleTimer
{
	DWORD Start;
	SimpleTimer() { Start = GetTickCount(); }
	int Millisec() const { return GetTickCount() - Start; }
	int Reset() {
		int elapsed = Millisec();
		Start += elapsed;
		return elapsed;
	}
	
	// Resets the timer if at least 'ms' milliseconds have passed.
	// Returns the time elapsed if the timer was reset, or 0 otherwise.
	// 
	// This function is useful when you want to ensure you do not do an action 
	// more than once in a period of 'ms' milliseconds:
	// 
	// if (timer.ResetAfter(1000))
	//     /* do an action here, only if one second passed since last time */;
	int ResetAfter(int ms)
	{
		int elapsed = Millisec();
		if (elapsed >= ms) {
			Start += elapsed;
			return elapsed;
		} 
		return false;
	}
	
	// Subtracts 'ms' from Millisec() if that much time has elapsed. Returns the
	// amount of time that was subtracted from Millisec(), normally 0 or ms.
	// 
	// The maxLag parameter specifies the maximum value of Millisec() when this
	// function returns; if Millisec() > maxLag, then extra time is subtracted
	// from Millisec(), increasing the return value, so that Millisec() == maxLag.
	// The maxLag parameter helps ensure that a timer doesn't get too far behind
	// 'real time'.
	// 
	// This function is useful when you want to do an action every 'ms'
	// milliseconds, in a timer routine that may be delayed. For example, suppose
	// you have a timer routine that is supposed to be called every 100 ms:
	// 
	// SimpleTimer t;
	// void TimerRoutine() // called every 100 ms
	// {
	//     while (t.AdvanceAfter(100, 200))
	//         /* do an action here */;
	// }
	// 
	// In case the timer is delayed, and gets fired less than once every 100ms, you 
	// can do the action multiple times to make up for lost time. The maxLag
	// parameter controls how much repetition is allowed--in this example, if the
	// timer is delayed more than 200 ms, the extra time is ignored, so that
	// (assuming the action itself takes no time) the loop will run no more than 3 
	// times.
	int AdvanceAfter(int ms, int maxLag = 10000)
	{
		int elapsed = Millisec();
		if (elapsed >= ms) {
			elapsed -= ms;
			Start += ms;
			if (elapsed > maxLag) {
				assert (maxLag >= 0);
				elapsed -= maxLag;
				Start += elapsed;
				ms += elapsed;
			}
			return ms;
		} 
		return false;
	}
};

#endif