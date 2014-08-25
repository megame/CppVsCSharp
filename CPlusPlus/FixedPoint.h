#ifndef _COMMON_FIXEDPOINT_H
#define _COMMON_FIXEDPOINT_H

#include <limits>
#include "ClipConvert.h"

namespace Math
{
	// ClipConvertFixedPoint::Do() converts from one fixed-point representation to
	// another, or between fixed-point and integer representations.
	//
	// To be efficient, the compiler must optimize "if(constant expr)", eliminate 
	// unreachable code, and put the inlining decision after these optimizations.
	// I just wish I could tell how well the compiler optimizes. In many cases 
	// this routine should reduce to "return TO_INT(from >> constant)", or 
	// "return TO_INT(from) << constant".
	//
	// This was a lot harder to write than I imagined! Note: this is a struct, not 
	// a function, due to a bug in eVC 4, with which this code is no longer tested.
	template<int TO_FRAC, typename TO_INT, int FROM_FRAC, typename FROM_INT>
	struct ClipConvertFixedPoint
	{
		inline static TO_INT Do(FROM_INT from)
		{
			#if _MSC_VER > 1202
			// Ignore the inevitable compiler warning 'shift count negative or too big'
			#pragma warning(push)
			#pragma warning(disable:4293)
			#endif

			const static int FROM_INTBITS = num_traits<FROM_INT>::precision_bits - FROM_FRAC;
			const static int TO_INTBITS = num_traits<TO_INT>::precision_bits - TO_FRAC;
			
			if (TO_INTBITS >= FROM_INTBITS) {
				// Moving to a wider or just-as-wide type (in terms of whole-number 
				// bits). There is no possibility of overflow, except if converting
				// from signed to unsigned at the same FRAC and integer size--a case 
				// I'm not bothering to handle.
				if (TO_FRAC > FROM_FRAC) // implies sizeof(TO_INT) > sizeof(FROM_INT)
					return TO_INT(from) << (TO_FRAC-FROM_FRAC);
				else 
					return TO_INT(from >> (FROM_FRAC-TO_FRAC));
			} else {
				// TO_INTBITS < FROM_INTBITS
				if (sizeof(TO_INT) < sizeof(FROM_INT)) {
					FROM_INT to;
					if (TO_FRAC <= FROM_FRAC)
						to = from >> (FROM_FRAC-TO_FRAC);
					else
						to = from << (TO_FRAC-FROM_FRAC);
					
					if (to != (FROM_INT)(TO_INT)to) {
						// Overflow.
						return from < 0 ? num_traits<TO_INT>::min_int : num_traits<TO_INT>::max_int;
					} else
						return TO_INT(to);
				} else { 
					// sizeof(TO_INT) >= sizeof(FROM_INT) and TO_INTBITS < FROM_INTBITS
					// implies TO_FRAC >= FROM_FRAC (and if TO_FRAC == FROM_FRAC, the 
					// conversion is from unsigned to signed at the same integer size,
					// but that case is not handled, i.e. not clipped.)
					TO_INT to = TO_INT(from) << (TO_FRAC-FROM_FRAC);
					// Normally, shifting right will restore the old value:
					if (TO_INT(from) != (to >> (TO_FRAC-FROM_FRAC))) {
						// There is an overflow iff it doesn't.
						return from < 0 ? num_traits<TO_INT>::min_int : num_traits<TO_INT>::max_int;
					} else
						return to;
				}
			}

			#if _MSC_VER > 1202
			#pragma warning(pop)
			#endif
		}
	};

	////////////////////////////////////////////////////////////////////////////////
	// FixedPoint //////////////////////////////////////////////////////////////////
	// FixedPoint<FRAC,intT> holds a fixed-point number and acts a lot like a
	// floating-point number.  FRAC is the number of fraction bits and intT is
	// the underlying integer type.
	//
	// Only one operation offers any kind of overflow protection: conversion from 
	// another type. If conversion from int, float, double or another FixedPoint
	// type would overflow, then the result is the maximum or minimum FixedPoint
	// value (e.g. (double)(FixedPoint<8,int16>)200 == 127.99609375). 
	// C++ makes it very hard to detect most overflows, so for performance reasons,
	// I generally don't try. An expression such as the following can detect the
	// overflow: IsInfinite((FixedPoint<8,int16>)200).
	//
	// When using eVC/VC6, you must use the SPECIALIZE_FIXEDPOINT_TRAITS() macro
	// in FixedPoint.cpp for each specialization of FixedPoint that you need;
	// otherwise you will get linker errors trying to access constants such as
	// UNIT and MAX_INTEGER. That's because the constants are considered variables
	// under those compilers. Visual Studio 7 and above do not need FixedPoint.cpp.
	//
	// Tip: add the following debugger visualizer to 
	// C:\Program Files\Microsoft Visual Studio 9.0\Common7\Packages\Debugger\autoexp.dat:
	//
	// Math::FixedPoint<*> {
	//     preview( #( $e.N / (double)(1 << SHIFT) ) )
	// }
	template<int FRAC, typename intT = int32>
	struct FixedPoint {
	protected:
		struct Prescale {};
		FixedPoint(intT n, Prescale) : N(n) {}
	public:
		typedef FixedPoint<FRAC,intT> self_t;
		typedef intT int_t;
		enum { 
			SHIFT = FRAC, 
			TOTALBITS = sizeof(intT) * CHAR_BIT,
		};
		DECLARE_TRAIT_CONSTANT(intT, UNIT, intT(1) << SHIFT);
		DECLARE_TRAIT_CONSTANT(intT, MASK, UNIT - 1);
		DECLARE_TRAIT_CONSTANT(intT, MIN_N, numeric_limits<intT>::is_signed ? intT(1) << (TOTALBITS-1) : 0);
		DECLARE_TRAIT_CONSTANT(intT, MAX_N, ~MIN_N);
		DECLARE_TRAIT_CONSTANT(intT, MIN_INTEGER, MIN_N >> FRAC);
		DECLARE_TRAIT_CONSTANT(intT, MAX_INTEGER, MAX_N >> FRAC);

		intT N;
		FixedPoint()                      { N = 0; assert(sizeof(self_t)==sizeof(intT)); }
		#if _MSC_VER > 1202
		// In EVC, this constructor conflicts with the template constructor below.
		FixedPoint(const self_t& rhs)     { N = rhs.N; }
		#endif
		FixedPoint(int8 i)                { *this = i; }
		FixedPoint(uint8 i)               { *this = i; }
		FixedPoint(int16 i)               { *this = i; }
		FixedPoint(uint16 i)              { *this = i; }
		FixedPoint(int32 i)               { *this = i; }
		FixedPoint(uint32 i)              { *this = i; }
		FixedPoint(int64 i)               { *this = i; }
		FixedPoint(uint64 i)              { *this = i; }
		FixedPoint(double d)              { *this = d; }
		FixedPoint(float f)               { *this = f; }
		
		template<int FRAC2, typename INT2>
		explicit FixedPoint(const FixedPoint<FRAC2,INT2>& rhs) {
			N = ClipConvertFixedPoint<FRAC,intT,FRAC2,INT2>::Do(rhs.N);
		}

		self_t FracPart() const           { return Prescaled(N & MASK); }
		intT WholePart() const             { return N >> SHIFT; } // 2.2 => 2; -2.2 => -3
		intT Floor() const                 { return N >> SHIFT; } // Just another name for it
		intT Ceil()  const                 { return (N + (UNIT-1)) >> SHIFT; }
		intT Round() const                 { return (N + (UNIT/2)) >> SHIFT; }
		intT ShiftedFloor (int fractionBits) const { return N >> (SHIFT - fractionBits); }
		self_t FractionalFloor (int fractionBits) const { 
			int clearBits = SHIFT - fractionBits;
			return Prescaled(N >> clearBits << clearBits);
		}
		self_t FractionalCeil (int fractionBits) const {
			int clearMask = (1 << (SHIFT - fractionBits)) - 1;
			return Prescaled((N + clearMask) & ~clearMask); 
		}
		void SwapWith(self_t &other) 
		   { intT temp = N; N = other.N; other.N = temp; }
		self_t Abs()  const { return Prescaled(N < 0 ? -N : N); }
		static self_t One() { return Prescaled(UNIT); }
		static self_t Zero() { return self_t(); }
		static self_t Epsilon() { return Prescaled(1); }
		static self_t Min() { return Prescaled(MIN_N); }
		static self_t Max() { return Prescaled(MAX_N); }
		static self_t Prescaled(intT prescaled) { return self_t(prescaled, Prescale()); }
		
		self_t Sqrt()
		{
			if (N <= MAX_INTEGER)
				return Prescaled(::Sqrt(N << FRAC));
			else
				// Compute lower-precision answer
				return Prescaled(::Sqrt(N >> (FRAC&1)) << (FRAC+1)/2);
		}

		/*struct explicit_numeric_conversion {
			intT N;
			explicit_numeric_conversion(intT n) { N=n; } 
		};
		operator typename explicit_numeric_conversion() const { return explicit_numeric_conversion(N); }*/
		operator typename int8()   const { return ClipConvertFixedPoint<0,int8,FRAC,intT>::Do(N); }
		operator typename uint8()  const { return ClipConvertFixedPoint<0,uint8,FRAC,intT>::Do(N); }
		operator typename int16()  const { return ClipConvertFixedPoint<0,int16,FRAC,intT>::Do(N); }
		operator typename uint16() const { return ClipConvertFixedPoint<0,uint16,FRAC,intT>::Do(N); }
		operator typename int32()  const { return ClipConvertFixedPoint<0,int32,FRAC,intT>::Do(N); }
		operator typename uint32() const { return ClipConvertFixedPoint<0,uint32,FRAC,intT>::Do(N); }
		operator typename int64()  const { return ClipConvertFixedPoint<0,int64,FRAC,intT>::Do(N); }
		operator typename uint64() const { return ClipConvertFixedPoint<0,uint64,FRAC,intT>::Do(N); }
		operator double() const       { return (double)N / (double)(1 << SHIFT); }
		operator float () const       { return (float)N / (float)(1 << SHIFT); }

		self_t& operator= (double d) 
		{
			double d2 = (double)ldexp(d, SHIFT);
			N = ClipConvert<intT>(d2);
			return *this; 
		}
		self_t& operator= (float f)
		{ 
			float f2 = (float)ldexp(f, SHIFT);
			N = ClipConvert<intT>(f2);
			return *this; 
		}
		self_t& operator= (int8 rhs)   { N = ClipConvertFixedPoint<FRAC,intT,0,int8>::Do(rhs);   return *this; }
		self_t& operator= (uint8 rhs)  { N = ClipConvertFixedPoint<FRAC,intT,0,uint8>::Do(rhs);  return *this; }
		self_t& operator= (int16 rhs)  { N = ClipConvertFixedPoint<FRAC,intT,0,int16>::Do(rhs);  return *this; }
		self_t& operator= (uint16 rhs) { N = ClipConvertFixedPoint<FRAC,intT,0,uint16>::Do(rhs); return *this; }
		self_t& operator= (int32 rhs)  { N = ClipConvertFixedPoint<FRAC,intT,0,int32>::Do(rhs);  return *this; }
		self_t& operator= (uint32 rhs) { N = ClipConvertFixedPoint<FRAC,intT,0,uint32>::Do(rhs); return *this; }
		self_t& operator= (int64 rhs)  { N = ClipConvertFixedPoint<FRAC,intT,0,int64>::Do(rhs);  return *this; }
		self_t& operator= (uint64 rhs) { N = ClipConvertFixedPoint<FRAC,intT,0,uint64>::Do(rhs); return *this; }
		self_t& operator= (const self_t& rhs)  { N = rhs.N;  return *this; }
		self_t& operator+= (const self_t& rhs) { N += rhs.N; return *this; }
		self_t& operator-= (const self_t& rhs) { N -= rhs.N; return *this; }
		self_t& operator*= (const self_t& rhs) { *this = *this * rhs; return *this; }
		self_t& operator/= (const self_t& rhs) { *this = *this / rhs; return *this; }
		self_t& operator<<= (int amt)          { N <<= amt;  return *this; }
		self_t& operator>>= (int amt)          { N >>= amt;  return *this; }
		self_t& operator&= (const self_t& r) { N &= r.N; return *this; }
		self_t& operator|= (const self_t& r) { N |= r.N; return *this; }
		self_t& operator^= (const self_t& r) { N ^= r.N; return *this; }
		bool operator> (const self_t& rhs) const { return N > rhs.N; }
		bool operator< (const self_t& rhs) const { return N < rhs.N; }
		bool operator>=(const self_t& rhs) const { return N >= rhs.N; }
		bool operator<=(const self_t& rhs) const { return N <= rhs.N; }
		bool operator==(const self_t& rhs) const { return N == rhs.N; }
		bool operator!=(const self_t& rhs) const { return N != rhs.N; }
		bool operator> (intT rhs) const { return N > (rhs << SHIFT); }
		bool operator< (intT rhs) const { return N < (rhs << SHIFT); }
		bool operator>=(intT rhs) const { return N >= (rhs << SHIFT); }
		bool operator<=(intT rhs) const { return N <= (rhs << SHIFT); }
		bool operator==(intT rhs) const { return N == (rhs << SHIFT); }
		bool operator!=(intT rhs) const { return N != (rhs << SHIFT); }
		self_t operator- ()      const { return Prescaled(-N); }
		self_t operator~ ()      const { return Prescaled(~N); }
		self_t operator<<(int r) const { return Prescaled(N << r); }
		self_t operator>>(int r) const { return Prescaled(N >> r); }
		self_t operator+ (const self_t& r) const { return Prescaled(N + r.N); }
		self_t operator- (const self_t& r) const { return Prescaled(N - r.N); }
		self_t operator+ (intT rhs) const { return Prescaled(N + (rhs << SHIFT)); }
		self_t operator- (intT rhs) const { return Prescaled(N - (rhs << SHIFT)); }
		self_t MulDiv(self_t mul, self_t div) const { return Math::MulDiv(*this, mul.N, div.N); }
		self_t MulDiv(intT mul, intT div)       const { return Math::MulDiv(*this, mul, div); }
		self_t operator* (intT r) const { return Prescaled(N * r); }
		self_t operator/ (intT r) const { return Prescaled(N / r); }
		self_t operator* (const self_t& b) const 
		{
			intT afrac =   N & MASK;
			intT bfrac = b.N & MASK;
			self_t result = self_t((N >> FRAC) * (b.N >> FRAC));
			result.N += afrac * bfrac >> Frac;
			return whole;
		}
		self_t operator/ (const self_t& b) const
		{
			intT whole = N / b.N;
			intT remainder = N % b.N;
			remainder = (remainder << FRAC) / b.N;
			assert(remainder < (1 << FRAC));
			self_t r;
			r.N = (whole << FRAC) + remainder;
			return r;
			// TODO: test negative numbers: 7 / -2.5, -7 / 2.5, -7 / -2.5
		}
		self_t operator% (const self_t& r) const { return Prescaled(N % r.N); }
		self_t operator& (const self_t& r) const { return Prescaled(N & r.N); }
		self_t operator| (const self_t& r) const { return Prescaled(N | r.N); }
		self_t operator^ (const self_t& r) const { return Prescaled(N ^ r.N); }

		// pre-increment & decrement
		self_t& operator ++ ()    { this->N += UNIT; return *this; }
		self_t& operator -- ()    { this->N -= UNIT; return *this; }
		// post-increment & decrement
		self_t operator ++ (int)  { self_t copy(*this); this->N += UNIT; return copy; }
		self_t operator -- (int)  { self_t copy(*this); this->N -= UNIT; return copy; }
	};
}

#endif // _COMMON_FIXEDPOINT_H
