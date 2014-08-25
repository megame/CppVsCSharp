//
// num_traits.h
// Author: David Piepgrass
// Copyright 2011 Mentor Engineering, Inc.
//
// Provides more information about numeric types than numeric_limits
// 
#ifndef _NUM_TRAITS_H
#define _NUM_TRAITS_H
#include <limits>

#ifndef DECLARE_TRAIT_CONSTANT
#define DECLARE_TRAIT_CONSTANT(TYPE, NAME, VALUE) static const TYPE NAME = VALUE
#endif

typedef unsigned   char  uchar;
typedef unsigned   char  byte;
typedef   signed   char  int8;
typedef unsigned   char  uint8;
typedef            short int16;
typedef unsigned   short uint16;
typedef            int   int32;
typedef unsigned   int   uint;
typedef unsigned   int   uint32;
typedef          long long int64;
typedef unsigned long long uint64;

namespace Math
{
	template<typename T> struct num_traits {
		#if _MSC_VER <= 1202
		// Any typedefs referenced in eVC must exist in the unspecialized template
		typedef T this_t;
		typedef T multiply_t;
		typedef T signed_t;
		typedef T unsigned_t;
		#endif
		DECLARE_TRAIT_CONSTANT(bool, is_numeric, false);
	};

	template<typename T> struct int_traits_base
	{
		typedef T this_t;
		DECLARE_TRAIT_CONSTANT(bool, is_numeric, true);
		DECLARE_TRAIT_CONSTANT(bool, is_integer, true);
		DECLARE_TRAIT_CONSTANT(bool, is_signed, std::numeric_limits<T>::is_signed);
		DECLARE_TRAIT_CONSTANT(bool, is_floating_point, false);
		DECLARE_TRAIT_CONSTANT(int, num_bits, sizeof(T) * CHAR_BIT);
		DECLARE_TRAIT_CONSTANT(int, precision_bits, num_bits - (int)std::numeric_limits<T>::is_signed);
		DECLARE_TRAIT_CONSTANT(T, min_int, std::numeric_limits<T>::is_signed ? T(T(1) << (num_bits-1)) : 0);
		DECLARE_TRAIT_CONSTANT(T, max_int, ~min_int);
		DECLARE_TRAIT_CONSTANT(bool, is_multiply_safe, num_bits < 64);
		static this_t zero() { return 0; }
		static this_t one() { return 1; }
		static this_t epsilon() { return 1; }
		static this_t positive_infinity() { return max_int; }
		static this_t maximum() { return max_int; }
		static this_t minimum() { return min_int; }
		static bool is_infinite(this_t i) { return i == max_int || (is_signed && i == min_int); }
		static bool is_finite(this_t i) { return true; }
	};

	#if _MSC_VER > 1202 || !defined(_MSC_VER)
	template<> struct num_traits<wchar_t> : int_traits_base<wchar_t> {
		typedef wchar_t this_t;
		typedef uint32 multiply_t; 
		typedef int16 signed_t;
		typedef wchar_t unsigned_t;
	};
	#endif
	template<> struct num_traits<int8> : int_traits_base<int8> { // int8 is a synonym for char
		typedef int8 this_t;
		typedef int16 multiply_t;
		typedef int8 signed_t;
		typedef uint8 unsigned_t;
	};
	template<> struct num_traits<uint8> : int_traits_base<uint8> { 
		typedef uint8 this_t;
		typedef uint16 multiply_t; 
		typedef int8 signed_t;
		typedef uint8 unsigned_t;
	};
	template<> struct num_traits<int16> : int_traits_base<int16> {
		typedef int16 this_t;
		typedef int32 multiply_t; 
		typedef int16 signed_t;
		typedef uint16 unsigned_t;
	};
	template<> struct num_traits<uint16> : int_traits_base<uint16> {
		typedef uint16 this_t;
		typedef uint32 multiply_t; 
		typedef int16 signed_t;
		typedef uint16 unsigned_t;
	};
	template<> struct num_traits<int32> : int_traits_base<int32> {
		typedef int32 this_t;
		typedef int64 multiply_t; 
		typedef int32 signed_t;
		typedef uint32 unsigned_t;
	};
	template<> struct num_traits<uint32> : int_traits_base<uint32> {
		typedef uint32 this_t;
		typedef uint64 multiply_t; 
		typedef int32 signed_t;
		typedef uint32 unsigned_t;
	};
	template<> struct num_traits<int64> : int_traits_base<int64> {
		typedef int64 this_t;
		typedef int64 multiply_t; 
		typedef int64 signed_t;
		typedef uint64 unsigned_t;
	};
	template<> struct num_traits<uint64> : int_traits_base<uint64> {
		typedef uint64 this_t;
		typedef uint64 multiply_t; 
		typedef int64 signed_t;
		typedef uint64 unsigned_t;
	};

	#ifndef Double_NaN	
	#define Double_NaN      std::numeric_limits<double>::quiet_NaN()
	#define Double_SNaN     std::numeric_limits<double>::signaling_NaN()
	#define Double_Infinity std::numeric_limits<double>::infinity()
	#define Double_Denorm   std::numeric_limits<double>::denorm_min()
	#define Float_NaN       std::numeric_limits<float>::quiet_NaN()
	#define Float_SNaN      std::numeric_limits<float>::signaling_NaN()
	#define Float_Infinity  std::numeric_limits<float>::infinity()
	#define Float_Denorm    std::numeric_limits<float>::denorm_min()
	#endif

	template<> struct num_traits<float> {
		typedef float this_t;
		typedef float multiply_t;
		typedef float signed_t;
		typedef float unsigned_t;
		DECLARE_TRAIT_CONSTANT(bool, is_numeric, true);
		DECLARE_TRAIT_CONSTANT(bool, is_integer, false);
		DECLARE_TRAIT_CONSTANT(bool, is_floating_point, true);
		DECLARE_TRAIT_CONSTANT(bool, is_signed, true);
		DECLARE_TRAIT_CONSTANT(bool, is_multiply_safe, true);
		// The mantissa of float is 23 bits (52 bits for double) by the IEEE 754
		// standard. One could make the argument that there are actually 24 and 
		// 53 bits of precision because of the implied 1 at the beginning of the 
		// mantissa, but since I'm not using these numbers in my program so far, 
		// I'm not thinking too hard about it.
		enum { precision_bits = 23 };
		static this_t zero() { return 0.0f; }
		static this_t one() { return 1.0f; }
		static this_t epsilon() { return Float_Denorm; }
		static this_t positive_infinity() { return Float_Infinity; }
		static this_t negative_infinity() { return -Float_Infinity; }
		static this_t maximum() { return Float_Infinity; }
		static this_t minimum() { return -Float_Infinity; }
		static bool is_infinite(float n)  { return !_finite(n) && !_isnan(n); }
		static bool is_finite(float n)  { return _finite(n) != 0; }
	};
	template<> struct num_traits<double> {
		typedef double this_t;
		typedef double multiply_t;
		typedef double signed_t;
		typedef double unsigned_t;
		DECLARE_TRAIT_CONSTANT(bool, is_numeric, true);
		DECLARE_TRAIT_CONSTANT(bool, is_integer, false);
		DECLARE_TRAIT_CONSTANT(bool, is_floating_point, true);
		DECLARE_TRAIT_CONSTANT(bool, is_signed, true);
		DECLARE_TRAIT_CONSTANT(bool, is_multiply_safe, true);
		enum { precision_bits = 52 };
		static this_t zero() { return 0.0; }
		static this_t one() { return 1.0; }
		static this_t epsilon() { return Double_Denorm; }
		static this_t positive_infinity() { return Double_Infinity; }
		static this_t negative_infinity() { return -Double_Infinity; }
		static this_t maximum() { return Double_Infinity; }
		static this_t minimum() { return -Double_Infinity; }
		static bool is_infinite(double n)  { return !_finite(n) && !_isnan(n); }
		static bool is_finite(double n) { return _finite(n) != 0; }
	};
}

#endif
