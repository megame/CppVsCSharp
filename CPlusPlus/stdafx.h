// stdafx.h : include file for standard system include files

// Iterator debugging makes DEBUG benchmarks unusably slow; always disable
#ifdef _HAS_ITERATOR_DEBUGGING
	#undef _HAS_ITERATOR_DEBUGGING
#endif
#define _HAS_ITERATOR_DEBUGGING 0

// Disabling checked iterators boosts performance, but removes all protection
// against iterator and index-out-of-range errors. It's probably unwise to disable
// them in apps that are supposed to be "secure".
#ifdef _SECURE_SCL
	#undef _SECURE_SCL
#endif
#define _SECURE_SCL 0

#include <windows.h>

#include "FastDelegate.h"
#include "SimpleTimer.h"
#include "Statistic.h"