//
// mini_vector.h
// Author: David Piepgrass
// Copyright 2010 Mentor Engineering, Inc.
//
#pragma once
#ifndef MINI_VECTOR
#define MINI_VECTOR

#include <assert.h>
#include <iterator>

#ifndef NULL
#define NULL 0
#endif

typedef int int32;
typedef unsigned int uint32;
typedef short int16;
typedef unsigned short uint16;

/// If COMPACT_MINI_VECTOR is defined, the mini_vector is 4 bytes smaller (in fact
/// it is only 4 bytes plus the size of the inline elements!), but it aligns
/// objects on a 4-byte boundary instead of an 8-byte boundary. Whether this is
/// safe depends on the processor architecture and the kind of data being stored in
/// the mini_vector. When debugging, it's better not to define this identifier
/// because, if the data is inline, it becomes more difficult to look at it in the 
/// debugger because the array pointer is invalid.
///
/// Possible problem: nothing about mini_vector's definition guarantees that the
/// compiler will place mini_vector itself on an 8- or 16-byte boundary. The 
/// alignment of inline items is only as good as the alignment of inline_vector 
/// itself. As a workaround, if you are declaring inline_vector inside a class and
/// you need it to be aligned on an 8-byte boundary, declare some other piece of
/// data that has to be on an 8-byte boundary first, e.g.:
/// 
/// class MyClass {
///     char c;   // if this came before v, it would only provide 4-byte alignment
///     double d; // but if doubles require 8-byte alignment, v will inherit 8-byte
///               // alignment as well.
///     mini_vector<double> v;
/// };
#ifndef _DEBUG
#define COMPACT_MINI_VECTOR
#endif
/// Tip: if COMPACT_MINI_VECTOR is not defined, mini_vector can be visualized in 
/// the debugger by inserting the following code in
/// C:\Program Files\Microsoft Visual Studio 9.0\Common7\Packages\Debugger\autoexp.dat:
///
///	mini_vector<*>{
///		children (
///			#array (
///				expr :		($e._array)[$i],
///				size :		$e._size
///			)
///		)
///		preview ( #(
///			"[", $e._size , "](",
///			#array (
///				expr :	($e._array)[$i],
///				size :	$e._size
///			),
///			")"
///		) )
///	}
///	std::_Vector_iterator<*>|std::_Vector_const_iterator<*>{
///		preview		( #(*$e._Myptr))
///		children	( #(ptr: *$e._Myptr))
///	}
///
/// See also https://svn.boost.org/trac/boost/wiki/DebuggerVisualizers


///	mini_vector is a drop-in replacement for std::vector. It is designed for 
///	situations where you have many small vectors (perhaps thousands), each of which 
///	tends to have very few elements, and you need to minimize memory allocations 
///	in order to maximize speed and/or minimize memory overhead. Compare with
///	inline_vector.
///	
///	In Visual C++, mini_vector is an excellent choice for storing small elements
///	such as ints or doubles. For example, mini_vector<int,2> is just 12 or 16 bytes
///	(depending on whether COMPACT_INLINE_VECTOR is defined), while std::vector<T>
///	is 24 bytes for any T.
///	
///	mini_vector should theoretically have virtually the same performance as 
///	std::vector, except that it has a small number of elements (according to the 
///	second template parameter, BaseSize) stored right in the object. Thus, if the 
///	list has BaseSize elements or fewer, no memory allocation is required, and 
///	locality-of-reference is better.
///	
///	If the list is expanded beyond BaseSize, a single contiguous block is allocated
///	in which all elements are placed; the array of elements inside the object is 
///	wasted, except that a small amount of data is stored in the place where the
///	inline array used to be.
///	
///	mini_vector is not relocatable; in general, it will not work after being moved
///	in memory.
///
///	This class guarantees (assuming no bugs in it) that it will remain valid if T's
///	copy constructor throws an exception, and that the exception will be propagated
///	to the caller. This basic guarantee is not provided, however, if T's default
///	constructor throws. If T's destructor throws, the exception may be caught and
///	suppressed in order to maintain the validity of the mini_vector.
///	
///	The size of a mini_vector is limited to 2^31-1 elements.
template<class T, int BaseSize = 2>
class mini_vector {
public:
	typedef size_t size_type;
protected:
	//BOOST_STATIC_ASSERT(BaseSize >= 1);
	///////////////////////////////////////////////////////////////////////////////
	// mini_vector: private data //////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	#ifndef COMPACT_MINI_VECTOR
	
	T* _array;
	uint32 _size;
	union {
		int32 capacity;
		char data[sizeof(T) * BaseSize];
	} _;

	void init()
	{
		_size = 0;
		_array = reinterpret_cast<T*>(_.data);
	}
	bool is_inline() const  { return (void*)_array == (void*)_.data; }
	void set_array(T* array, size_type capacity)
	{
		assert ((void*)array != (void*)_.data);
		_array = array;
		_.capacity = (int)capacity;
	}
	void set_size(size_type size)
	{
		assert(size <= BaseSize || !is_inline());
		_size = (uint32)size;
	}
	
	#else
	
	// In the compact form, the array pointer is union'd with the inline data array,
	uint32 _size; // and bit 31 of _size is a flag that indicates an external array.
	union {
		struct {
			T* array;
			int32 capacity;
		} _;
		char data[sizeof(T) * BaseSize];
	} _;

	void init()
	{
		_size = 0;
	}
	bool is_inline() const  { return (int32)_size >= 0; }
	void set_array(T* array, size_type capacity)
	{
		assert ((void*)array != (void*)_.data);
		_size |= 0x80000000;
		_._.array = array;
		_._.capacity = (int32)capacity;
	}
	void set_size(size_type size)
	{
		assert(size <= BaseSize || !is_inline());
		_size = (_size & 0x80000000) | size;
	}

	#endif
	
	char* go_inline_again()
	{
		assert(size() <= BaseSize);
		if (!is_inline()) {
			T* oldArray = begin();
			#ifndef COMPACT_MINI_VECTOR
				_array = reinterpret_cast<T*>(_.data);
			#else
				_size &= ~0x80000000;
			#endif
			for (int i = 0; i < (int)size(); i++) {
				construct(&begin()[i], oldArray[i]);
				destruct(&oldArray[i]);
			}
			return reinterpret_cast<char*>(oldArray);
		}
		return NULL;
	}

	typedef mini_vector<T, BaseSize> mini_vector_t;
public:
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef int difference_type;
	/// define reverse iterators by using STL magic
	typedef std::reverse_iterator<iterator>       reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	///////////////////////////////////////////////////////////////////////////////
	// safe iterator (not used in STL interface) //////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	
	// common_safe_iterator is instantiated to produce both const_iterator and
	// iterator types. It called "safe" because it is not necessarily invalidated
	// by changes to the collection size, and it calls at() on the target
	// collection, which is supposed to throw an exception if the index is
	// out-of-range (instead of calling [] which does not).
	template<class base>
	class common_safe_iterator : public base {
	protected:
		// base must contain the following 5 typedefs
		typedef typename base::reference reference;
		typedef typename base::pointer pointer;
		typedef typename base::vector_t vector_t;
		typedef typename base::const_iterator_t const_iterator_t;
		typedef typename base::iterator_t iterator_t;
		vector_t* _vec;
		size_type _pos;
	public:
		typedef common_safe_iterator<base> self;
		friend const_iterator_t;

		common_safe_iterator(vector_t* vec, size_type pos) : _vec(vec), _pos(pos) { }
		common_safe_iterator(const iterator_t& copy) : _vec(copy._vec), _pos(copy._pos) { }
		
		reference operator*() const { return _vec->at(_pos); }
		pointer operator->() const { return &_vec->at(_pos); }
		
		self& operator++() // prefix ++
			{ ++_pos; return *this; }
		self& operator--() // prefix --
			{ --_pos; return *this; }
		self& operator+=(int amt) 
			{ _pos += amt; return *this; }
		bool operator==(const self& x) const
			{ return (x._vec == _vec) && (x._pos == _pos); }
		int operator-(const self& base) const
			{ assert(base._vec == _vec); return _pos - base._pos; }
		// Returns true if the iterator can be dereferenced
		bool is_valid() const
			{ return _vec != NULL && _pos < _vec->size(); }

		/////////////////////////////////////////////////////////
		// Operators that are defined in terms of other operators
		
		self operator++(int) // postfix ++
		{
			self tmp = *this; // copy ourselves
			++*this;
			return tmp;
		}
		self operator--(int) // postfix --
		{
			self tmp = *this; // copy ourselves
			--*this;
			return tmp;
		}
		self& operator-=(int amt)
		{
			return *this += -amt;
		}
		bool operator!=(const self& x) const
		{
			return !(*this == x);
		}
		bool operator>(const self& x) const
		{
			return *this - x > 0;
		}
		bool operator>=(const self& x) const
		{
			return *this - x >= 0;
		}
		bool operator<(const self& x) const
		{
			return *this - x < 0;
		}
		bool operator<=(const self& x) const
		{
			return *this - x <= 0;
		}
		self operator+(int amt) const
		{
			self tmp = *this;
			return tmp += amt;
		}
		self operator-(int amt) const
		{
			self tmp = *this;
			return tmp -= amt;
		}
		reference operator[](int index) const
		{
			self tmp = *this;
			tmp += index;
			return *tmp;
		}
	};
	
	/// iterator and const_iterator differ only in these typedefs.
	/// const_iterator_base is the base class of const_iterator, while
	/// iterator_base is the base class of iterator; both iterator and
	/// const_iterator are typedefs of common_safe_iterator.
	struct iterator_base;
	struct const_iterator_base
	{
		typedef const typename mini_vector_t::value_type& reference;
		typedef const typename mini_vector_t::value_type* pointer;
		typedef const mini_vector_t vector_t;
		typedef common_safe_iterator<const_iterator_base> const_iterator_t;
		typedef common_safe_iterator<iterator_base> iterator_t;
	};
	struct iterator_base
	{
		typedef typename mini_vector_t::value_type& reference;
		typedef typename mini_vector_t::value_type* pointer;
		typedef mini_vector_t vector_t;
		typedef common_safe_iterator<const_iterator_base> const_iterator_t;
		typedef common_safe_iterator<iterator_base> iterator_t;
	};
	typedef common_safe_iterator<const_iterator_base> const_safe_iterator;
	typedef common_safe_iterator<iterator_base> safe_iterator;

	///////////////////////////////////////////////////////////////////////////////
	// inline_vector: public functions ////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	#ifndef COMPACT_MINI_VECTOR
	size_type size() const { return _size; }
	size_type capacity() const { return is_inline() ? BaseSize : _.capacity; }
	iterator begin() { return _array; }
	#else
	size_type size() const { return _size & 0x7FFFFFFF; }
	size_type capacity() const { return is_inline() ? BaseSize : _._.capacity; }
	iterator begin() { return is_inline() ? reinterpret_cast<T*>(_.data) : _._.array; }
	#endif
	iterator end() { return begin() + size(); }
	const_iterator begin() const { return const_cast<mini_vector_t*>(this)->begin(); }
	const_iterator end() const { return begin() + size(); }
	reverse_iterator rbegin() { return reverse_iterator(end()); }
	reverse_iterator rend()   { return reverse_iterator(begin()); }
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	const_reverse_iterator rend() const   { return const_reverse_iterator(begin()); }
	size_type max_size() const { return INT_MAX; }
	bool empty() const { return size() == 0; }
	reference operator[](size_type n) { return begin()[n]; }
	const_reference operator[](size_type n) const { return begin()[n]; }
	reference at(size_type n) { check_index(n); return begin()[n]; }
	const_reference at(size_type n) const { check_index(n); return begin()[n]; }
	const_safe_iterator iterator_at(int n) const { return const_safe_iterator(this, n); }
	safe_iterator iterator_at(int n) { return safe_iterator(this, n); }
	
	mini_vector() { init(); }
	mini_vector(size_type n) { init(); resize(n); }
	mini_vector(size_type n, const T& t) { init(); resize(n, t); }
	mini_vector(const mini_vector_t& copy) {
		init();
		reserve(copy.size()); 
		add_range(copy.begin(), copy.end());
	}
	template <class InputIterator>
	mini_vector(InputIterator from, InputIterator to) { init(); add_range(from, to); }
	~mini_vector() { clear_nothrow(); }
	
	mini_vector_t& operator=(const mini_vector_t& copy) 
	{
		clear();
		reserve(copy.size());
		add_range(copy.begin(), copy.end());
		return *this;
	}
	reference front()             { check_index(0); return *begin(); }
	const_reference front() const { check_index(0); return *begin(); }
	reference back()              { check_index(0); return *(end() - 1); }
	const_reference back() const  { check_index(0); return *(end() - 1); }
	
	int index_of(const T& item, size_t start_index = 0) const
	{
		if (start_index >= size())
			return -1;

		const_iterator it = begin() + start_index;
		for (int i = 0; it != end(); ++it, ++i)
			if (*it == item)
				return i;
		return -1;
	}
	const_iterator find(const T& item, size_t start_index = 0) const
	{
		int i = index_of(item, start_index);
		if (i >= 0)
			return begin() + i;
		return end();
	}

	/////////////////////////
	// Modification functions

	void reserve(size_type desiredSize)
	{
		throw_if_negative((int32)desiredSize);
		if (capacity() < desiredSize)
			reallocate(desiredSize);
	}
	void push_back(const T& item)
	{
		if (!capacity_left())
			reallocate(size() * 2);
		construct(end(), item); // may throw (safe)
		set_size(size() + 1);
	}
	void pop_back()
	{
		check_index(0);
		try {
			destruct(end() - 1);
		} catch(...) {
			set_size(size() - 1);
			throw;
		}
		set_size(size() - 1);
	}
	iterator insert(iterator pos, const T& x)
	{
		prepare_to_insert(pos, 1); // may throw
		try {
			construct(pos, x);
		} catch(...) {
			construct_nothrow(pos);
			throw;
		}
		return pos;
	}
	template <class InputIterator>
	void insert(iterator pos, InputIterator f, InputIterator l)
	{
		if (pos == end())
			add_range(f, l);
		else {
			mini_vector_t temp;
			temp.add_range(f, l);
			prepare_to_insert(pos, temp.size());
			for (iterator it = temp.begin(); it != temp.end(); ++it, ++pos) {
				try {
					construct(pos, *it);
				} catch(...) {
					construct_in_exception(pos, temp.end() - it);
					throw;
				}
			}
		}
	}
	template <class InputIterator>
	void add_range(InputIterator from, InputIterator to)
	{
		for (; from != to; ++from)
			push_back(*from); // may throw (safe)
	}
	void insert(iterator pos, size_type n, const T& x)
	{
		throw_if_negative((int32)n);
		prepare_to_insert(pos, n);
		for (size_type i = 0; i < n; ++i, ++pos)
			try {
				construct(pos, x);
			} catch(...) {
				construct_in_exception(pos, n - i);
				throw;
			}
	}
	void insert(iterator pos, size_type n)
	{
		throw_if_negative((int32)n);
		prepare_to_insert(pos, n);
		for (size_type i = 0; i < n; ++i, ++pos)
			// If construct() throws, the basic guarantee is not provided.
			construct(pos);
	}
	iterator erase(const_iterator pos)
	{
		return erase(pos, 1);
	}
	iterator erase(const_iterator first, const_iterator last)
	{
		return erase(first, last - first);
	}
	iterator erase(const_iterator first, size_type count) // may throw
	{
		throw_if_negative((int32)count);
		difference_type index = (int)(first - begin());
		if (index < 0 || (size_type)index > size())
			throw exception_t(_T("mini_vector: attempted to erase with invalid iterator"));
	
		// Ensure not too many elements are erased
		size_type maxCount = size() - index;
		if (count > maxCount)
			count = maxCount;

		// shift elements left
		iterator lo = (iterator)first, hi = (iterator)first + count;
		for (; hi != end(); ++lo, ++hi) {
			// destruct and construct may throw. To obtain the basic guarantee,
			// default-construct the element if the constructor throws.
			try {
				destruct(lo);
				construct(lo, *hi);
			} catch(...) {
				construct_nothrow(lo);
				throw;
			}
		}
		for (--hi, --lo; hi != lo; --hi)
			destruct_nothrow(hi);

		set_size(size() - count);
		return (iterator)first;
	}
	void clear(bool freeMemory = false)
	{
		erase(begin(), size());
		if (freeMemory)
			reallocate(0);
	}
	void resize(size_type n, T item = T())
	{
		throw_if_negative((int32)n);
		int32 delta = (int32)(n - size());
		if (delta > 0)
			insert(end(), (size_type)delta, item);
		else if (delta < 0)
			erase(end() + delta, -delta);
	}
	void swap(mini_vector_t& other)
	{
		if (!is_inline() && !other.is_inline())
		{
			T* tempArray = other.begin();
			size_type tempSize = other.size(), tempCapacity = other.capacity();

			other.set_array(begin(), capacity());
			other.set_size(size());
			set_array(tempArray, tempCapacity);
			set_size(tempSize);
		}
		else if (is_inline() && other.is_inline())
		{
			if (other.size() >= size()) {
				size_type i;
				for (i = 0; i < (int)size(); i++)
					std::swap(begin()[i], other[i]);
				for (; i < other.size(); i++) {
					construct(&begin()[i], other[i]);
					destruct(&other[i]);
				}
				size_type tempSize = other.size();
				other.set_size(size());
				set_size(tempSize);
			} else
				other.swap(*this);
		}
		else // one of the lists is not inline
		{
			if (!is_inline()) {
				T* tempArray = begin();
				size_type tempSize = size();
				size_type tempCapacity = capacity();
				set_size(0);
				go_inline_again();
				set_size(other.size());
				for (int i = 0; i < (int)size(); i++) {
					construct(&begin()[i], other[i]);
					destruct(&other[i]);
				}
				other.set_array(tempArray, tempCapacity);
				other.set_size(tempSize);
			} else
				other.swap(*this);
		}
	}

protected:
	///////////////////////////////////////////////////////////////////////////////
	// mini_vector: protected functions ///////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	void check_index(size_type index) const
	{
		if ((size_type)index >= size())
			throw exception_tsprintf(_T("mini_vector: index out of range (%d >= %d)"), index, size());
	}
	void throw_if_negative(int32 value) const
	{
		if (value < 0)
			throw exception_tsprintf(_T("mini_vector: negative value not allowed (%d)"), value);
	}
	void construct_in_exception(iterator pos, size_type count)
	{
		// In an exception handler, try to provide the basic guarantee by default-
		// constructing the remaining uninitialized elements. The exception will
		// leave the collection with an undefined set of items, but at least the 
		// mini_vector can be destructed safely.
		for (int i = 0; i < (int)count; ++i, ++pos)
			construct_nothrow(pos);
	}
	size_type capacity_left() const { return capacity() - size(); }
	void reallocate(size_type capacity)
	{
		assert(size() <= capacity);
		if (capacity <= BaseSize)
			delete[] go_inline_again();
		else {
			T* newArray = reinterpret_cast<T*>(new char[capacity * sizeof(T)]); // may throw
			T* oldArray = begin();
			for (int i = 0; i < (int)size(); i++) {
				construct(&newArray[i], oldArray[i]);
				destruct(&oldArray[i]);
			}
			if (!is_inline())
				delete[] reinterpret_cast<char*>(oldArray);
			set_array(newArray, capacity);
		}
	}
	
	#pragma push_macro("new")
	static void construct(T* it)
	{
		new(it) T();
	}
	static void construct(T* it, const T& copy)
	{
		new(it) T(copy);
	}
	static void construct_nothrow(T* it)
	{
		try {
			new(it) T();
		} catch(...) { }
	}
	#pragma pop_macro("new")

	static void destruct(T* it)
	{
		it->~T();
	}
	static void destruct_nothrow(T* it)
	{
		try {
			it->~T();
		} catch(...) { }
	}
	void clear_nothrow()
	{
		for (iterator it = begin(); it != end(); ++it)
			destruct_nothrow(it);

		set_size(0);
		delete[] go_inline_again();
	}
	
	// Prepares to insert elements (amt is the # of new elements).
	// prepare_to_insert does everything except initialize the new elements. It
	// also fixes 'it' so that it remains valid in case of reallocation.
	void prepare_to_insert(iterator& it, size_type amt)
	{
		// 'it' may not be a valid iterator; it could belong to a different 
		// collection, or point beyond the end of this collection. Check it.
		difference_type index = (int)(it - begin());
		if (index < 0 || (size_type)index > size())
			throw exception_t(_T("mini_vector: attempted to insert with invalid iterator"));

		// increase capacity if necessary
		size_type newSize = size() + amt;
		if (newSize > capacity()) {
			size_type newCapacity = size() * 2;
			if (newCapacity < newSize)
				newCapacity = newSize;
			reallocate(newCapacity); // may throw
			it = begin() + index;
		}

		// increase recorded size
		set_size(newSize);

		// shift elements right
		iterator end1 = end() - amt, end2 = end();
		while (end1 != it) {
			--end1;
			--end2;
			try {
				construct(end2, *end1);
			} catch(...) {
				assert (end2 - end1 == amt);
				construct_in_exception(++end1, amt);
				throw;
			}
			destruct_nothrow(end1);
		}
	}
};

template<class T, int s>
bool operator==(const mini_vector<T,s>& a, const mini_vector<T,s>& b)
{
	if (a.size() != b.size())
		return false;
	mini_vector<T,s>::const_iterator ai = a.begin(), bi = b.begin();
	for (; ai != a.end(); ++ai, ++bi)
		if (*ai != *bi)
			return false;
	return true;
}
template<class T, int s>
bool operator<(const mini_vector<T,s>& a, const mini_vector<T,s>& b)
{
	if (a.size() != b.size())
		return a.size() < b.size();
	mini_vector<T,s>::const_iterator ai = a.begin(), bi = b.begin();
	for (; ai != a.end(); ++ai, ++bi)
		if (*ai != *bi)
			return *ai < *bi;
	return false;
}
template<class T, int s>
bool operator!=(const mini_vector<T,s>& a, const mini_vector<T,s>& b) { return !(a == b); }
template<class T, int s>
bool operator>=(const mini_vector<T,s>& a, const mini_vector<T,s>& b) { return !(a < b); }
template<class T, int s>
bool operator> (const mini_vector<T,s>& a, const mini_vector<T,s>& b) { return b < a; }
template<class T, int s>
bool operator<=(const mini_vector<T,s>& a, const mini_vector<T,s>& b) { return !(b < a); }

#endif
