//
// ClipConvert<
//
#ifndef _CLIPCONVERT_H
#define _CLIPCONVERT_H

namespace Math
{
	template<typename TO, typename FROM>
	struct ClipConverter {
		inline static TO Convert(FROM n) { 
			//BOOST_STATIC_ASSERT(num_traits<TO>::is_numeric &&
			//                    num_traits<FROM>::is_numeric);
			//BOOST_STATIC_ASSERT(num_traits<TO>::is_floating_point ||
			//                    num_traits<FROM>::is_floating_point ||
			//					num_traits<TO>::precision_bits >=
			//					num_traits<FROM>::precision_bits);
			return (TO)n;
		}
	};

	// Specializations of ClipConverter are needed for each pair of types 
	// where FROM can represent numbers that are out-of-range in TO. 
	// Rather than specializing ClipConvert() itself, a class is specialized 
	// instead because only classes allow partial specialization, which may
	// be useful for custom types like fixed-point.
	#define SPECIALIZE_CLIP_CONVERT(TO,FROM) \
		template<> struct ClipConverter<TO,FROM> {                     \
			inline static TO Convert(FROM n) {                         \
				static const FROM min = (FROM)num_traits<TO>::min_int; \
				static const FROM max = (FROM)num_traits<TO>::max_int; \
				if (n <= min) return num_traits<TO>::min_int;          \
				if (n >= max) return num_traits<TO>::max_int;          \
				return (TO)n;                                          \
			}                                                          \
		};

	// This specialization is designed for conversions between integers of 
	// different sizes:
	#define SPECIALIZE_CLIP_CONVERT_INTS(TO,FROM) \
		template<> struct ClipConverter<TO,FROM> {                 \
			inline static TO Convert(FROM n) {                     \
				if ((TO)n != n) {                                  \
					if (n < 0) return num_traits<TO>::min_int;     \
					else       return num_traits<TO>::max_int;     \
				}                                                  \
				return (TO)n;                                      \
			}                                                      \
		};
	
	// This specialization is designed for conversions between integers of 
	// the same size and different signedness
	#define SPECIALIZE_CLIP_CONVERT_UINT(TO,FROM) \
		template<> struct ClipConverter<TO,FROM> {                 \
			inline static TO Convert(FROM n) {                     \
				/*BOOST_STATIC_ASSERT(sizeof(TO) == sizeof(FROM));*/ \
				if (n < (FROM)0)                                   \
					return (TO)0;                                  \
				if ((TO)n < (TO)0)                                 \
					return num_traits<TO>::max_int;                \
				return (TO)n;                                      \
			}                                                      \
		};

	SPECIALIZE_CLIP_CONVERT_INTS(int8, int16)
	SPECIALIZE_CLIP_CONVERT_INTS(int8, uint16)
	SPECIALIZE_CLIP_CONVERT_INTS(int8, int32)
	SPECIALIZE_CLIP_CONVERT_INTS(int8, uint32)
	SPECIALIZE_CLIP_CONVERT_INTS(int8, int64)
	SPECIALIZE_CLIP_CONVERT_INTS(int8, uint64)
	SPECIALIZE_CLIP_CONVERT_INTS(uint8, int16)
	SPECIALIZE_CLIP_CONVERT_INTS(uint8, uint16)
	SPECIALIZE_CLIP_CONVERT_INTS(uint8, int32)
	SPECIALIZE_CLIP_CONVERT_INTS(uint8, uint32)
	SPECIALIZE_CLIP_CONVERT_INTS(uint8, int64)
	SPECIALIZE_CLIP_CONVERT_INTS(uint8, uint64)
	SPECIALIZE_CLIP_CONVERT_INTS(int16, int32)
	SPECIALIZE_CLIP_CONVERT_INTS(int16, uint32)
	SPECIALIZE_CLIP_CONVERT_INTS(int16, int64)
	SPECIALIZE_CLIP_CONVERT_INTS(int16, uint64)
	SPECIALIZE_CLIP_CONVERT_INTS(uint16, int32)
	SPECIALIZE_CLIP_CONVERT_INTS(uint16, uint32)
	SPECIALIZE_CLIP_CONVERT_INTS(uint16, int64)
	SPECIALIZE_CLIP_CONVERT_INTS(uint16, uint64)
	SPECIALIZE_CLIP_CONVERT_INTS(wchar_t, int32)
	SPECIALIZE_CLIP_CONVERT_INTS(wchar_t, uint32)
	SPECIALIZE_CLIP_CONVERT_INTS(wchar_t, int64)
	SPECIALIZE_CLIP_CONVERT_INTS(wchar_t, uint64)
	SPECIALIZE_CLIP_CONVERT_INTS(int32, int64)
	SPECIALIZE_CLIP_CONVERT_INTS(int32, uint64)
	SPECIALIZE_CLIP_CONVERT_INTS(uint32, int64)
	SPECIALIZE_CLIP_CONVERT_INTS(uint32, uint64)

	SPECIALIZE_CLIP_CONVERT_UINT(int8, uint8)
	SPECIALIZE_CLIP_CONVERT_UINT(uint8, int8)
	SPECIALIZE_CLIP_CONVERT_UINT(int16, uint16)
	SPECIALIZE_CLIP_CONVERT_UINT(uint16, int16)
	SPECIALIZE_CLIP_CONVERT_UINT(int32, uint32)
	SPECIALIZE_CLIP_CONVERT_UINT(uint32, int32)
	SPECIALIZE_CLIP_CONVERT_UINT(int64, uint64)
	SPECIALIZE_CLIP_CONVERT_UINT(uint64, int64)

	SPECIALIZE_CLIP_CONVERT(int8, float)
	SPECIALIZE_CLIP_CONVERT(int8, double)
	SPECIALIZE_CLIP_CONVERT(uint8, float)
	SPECIALIZE_CLIP_CONVERT(uint8, double)
	SPECIALIZE_CLIP_CONVERT(int16, float)
	SPECIALIZE_CLIP_CONVERT(int16, double)
	SPECIALIZE_CLIP_CONVERT(uint16, float)
	SPECIALIZE_CLIP_CONVERT(uint16, double)
	SPECIALIZE_CLIP_CONVERT(int32, float)
	SPECIALIZE_CLIP_CONVERT(int32, double)
	SPECIALIZE_CLIP_CONVERT(uint32, float)
	SPECIALIZE_CLIP_CONVERT(uint32, double)
	SPECIALIZE_CLIP_CONVERT(int64, float)
	SPECIALIZE_CLIP_CONVERT(int64, double)
	SPECIALIZE_CLIP_CONVERT(uint64, float)
	SPECIALIZE_CLIP_CONVERT(uint64, double)
	SPECIALIZE_CLIP_CONVERT(wchar_t, float)
	SPECIALIZE_CLIP_CONVERT(wchar_t, double)

	template<typename TO, typename FROM> inline TO ClipConvert(FROM n) 
		{ return ClipConverter<TO,FROM>::Convert(n); }
}

#endif