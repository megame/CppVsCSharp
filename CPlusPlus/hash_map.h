//
// hash_map.h
// Author: David Piepgrass
// Copyright 2010 Mentor Engineering, Inc.
//
#ifndef _COMMON_HASHTABLE_H
#define _COMMON_HASHTABLE_H

#include <assert.h>
#include "mini_vector.h"
#include "Misc.h"

#ifndef ARRAY_AUTO_PTR
#define ARRAY_AUTO_PTR
template<class T>
class array_auto_ptr {
	T* ptr;
public:
	explicit array_auto_ptr(T* pointer = NULL) : ptr(pointer) {}
	~array_auto_ptr() { delete[] ptr; }
	T& operator[] (int index) const { return ptr[index]; }
	operator T*() const { return ptr; }
	T* get() const { return ptr; }
	void reset(T* newArray = NULL)
	{
		delete[] ptr;
		ptr = newArray;
	}
};
#endif

// A hash_traits class needs to have this form, with equals() and hash_value()
// functions. The TR1 unordered_map seems to have chosen a different way for hashing
// to work, with a pair of structures "hasher" and "is_equal", but I don't like that
// solution because it defeats the empty base class optimization in Visual C++ (or
// so I heard), i.e. using two empty classes instead of one forces the compiler to
// allocate 4 unused bytes of storage for the second one, if the second type has no
// data members.
// 
// Moreover, in cases where hash_traits actually has data members, it is likely that
// equals() and hash_value() will want to have access to the same data; if those
// functions were defined in two separate structures, they would require two copies 
// of the data.
// 
// The default hash_traits implements hash_value() by casting T to size_t, and it
// implements equals by invoking operator==().
template<class T>
struct hash_traits {
	typedef T value_type;
	bool equals(const T& a, const T& b) const { return a == b; }
	unsigned hash_value(const T& key) const { return (unsigned)(size_t)key; }
};

// Specialization of hash_traits for double.
template<>
struct hash_traits<double> {
	typedef double value_type;
	bool equals(double a, double b) const { return a == b; }
	unsigned hash_value(double key) const
	{
		//BOOST_STATIC_ASSERT(sizeof(double) >= sizeof(int));
		if (sizeof(double) >= sizeof(int) * 2) {
			unsigned* pkey = reinterpret_cast<unsigned*>(&key);
			return pkey[0] ^ pkey[1];
		} else
			return *reinterpret_cast<unsigned*>(&key);
	}
};

// Specialization of hash_traits for float.
template<>
struct hash_traits<float> {
	typedef float value_type;
	bool equals(float a, float b) const { return a == b; }
	unsigned hash_value(float key) const
	{
		//BOOST_STATIC_ASSERT(sizeof(float) >= sizeof(int));
		return *reinterpret_cast<unsigned*>(&key);
	}
};

// Specialization of hash_traits for basic_string.
template<typename CHAR>
struct hash_traits< std::basic_string<CHAR> >
{
	typedef std::basic_string<CHAR> value_type;
	bool equals(const value_type& a, const value_type& b) const { return a == b; }
	unsigned hash_value(const value_type& k) const
	{
		unsigned v = 0;
		for(int i = 0; i < (int)k.size(); i++, v = 16777619u * v)
			v ^= (unsigned)(size_t)k[i];
		return v;
	}
};

// Like hash_traits, except that the T values are assumed to be pointers and are
// dereferenced.
template<class T>
struct indirect_hash_traits {
	typedef T value_type;
	bool equals(const T& a, const T& b) const { return *a == *b; }
	unsigned hash_value(const T& key) const { return (unsigned)*key; }
};

// This traits class calls hash_value() on the key type T, instead of casting T to
// size_t like the default hash_traits does.
template<class T>
struct hash_value_traits {
	typedef T value_type;
	bool equals(const T& a, const T& b) const { return a == b; }
	unsigned hash_value(const T& key) const { return (unsigned)(size_t)key.hash_value(); }
};

///////////////////////////////////////////////////////////////////////////////
// 
// HH                     HH                             TT
// HH                     HH                             TT
// HHHHHH    AAAA    SSSs HHHHHH          SSSs   EEEE  TTTTTT
// HH   HH      AA  SS    HH   HH        SS     EE  EE   TT
// HH   HH   AAAAA   SSS  HH   HH         SSS   EEEEE    TT
// HH   HH  AA  AA     SS HH   HH           SS  EE       TT
// HH   HH   AAA AA sSSS  HH   HH ______ sSSS    EEEE     TT
//	                                                    
///////////////////////////////////////////////////////////////////////////////
// hash_set ////////////////////////////////////////////////////////////////////
//
// A hash set implementation inspired by the .NET framework's efficient generic
// Dictionary. This implementation uses 1x to 2x as many "buckets" as entries,
// and each bucket points to a "linked list" of entries for which hashCode %
// bucketCount == theIndexOfTheBucket. For efficiency, the "linked list" does not
// use heap-allocated nodes; rather, it is encoded into a vector: each added entry 
// is simply placed at the end of an array.
// 
// hash_set is a set, so no values are associated with keys, although the keys
// themselves can have mutable data provided that the mutable data does not
// contribute to the hashcode or equality condition (if a hashcode changes on a 
// key in the set, it usually becomes impossible to look up the key.) See hash_map
// for a conventional dictionary that supports key-value pairs.
// 
// By default, the hash code of T is obtained by casting T to size_t. Use a custom
// hash_traits class (or one of the other traits classes above) to change the way
// the hash code is retrieved or the way that Ts are compared for equality.
// 
// hash_set normally stores a hashcode with each entry so that when resolving 
// hash collisions, it is not necessary compare the key with each entry in a
// bucket; rather, we usually only have to compare hash codes. However, if
// comparing two keys is fast, then storing the hashcode is a waste of space.
// Therefore, hash_set offers the CompactMode flag, which causes the hashcode to be
// omitted.
// 
// hash_set has a minimum overhead of 8 bytes per entry (plus sizeof(TKey)) in
// Compact Mode and 12 bytes per entry otherwise. Plus, two growable arrays are
// used that double in size on each allocation, for an additional overhead between
// 0% and about 100%.
// 
// I noticed that .NET's Dictionary never decreases in size, and I figured if it's
// good enough for every single .NET application, it's probably good enough for
// mine. But never reclaiming storage does make me a tad uncomfortable, so my
// Clear() method frees the buckets & entries.
// 
// I hate how the STL makes you write "dictionary.erase(dictionary.find(k))" and
// "dictionary.find(k) != dictionary.end()", so I provide .NET-style methods like 
// "dictionary.Remove(k)" and "dictionary.Contains(k)" in addition.
// 
// I have a memory-saving idea to try in the future, inspired by some paper about 
// tries:
// 
// 1. Use uint16 for the bucket array when there are less than 65536 entries.
//    Actually, you could store a bucket and an (unrelated) 'next' pointer in the
//    space now used by the 32-bit 'next' pointer.
// 2. When there are more than 65536 entries, use fewer buckets than entries to 
//    save memory on bucket pointers.
// 3. In case the hash chains are getting long, move an item to the front of the 
//    linked list when it is retrieved. Presumably, some items are accessed more 
//    often than others, so the oft-used items tend to be at the beginning of 
//    their chains and are found fast.
// 
// This idea reduces the per-entry overhead by up to 4 bytes, down to 4 bytes in 
// compact mode and 8 bytes otherwise. Unlike hash_set, this approach is not 
// thread-safe for concurrent readers, so it should be implemented in a separate
// class. Also, it would invalidate more iterators than the current code does.
//
// Oh yes, iterators. Obviously, it's a hashtable so the elements will be enumerated
// in no particular order. Good news though! When you insert a new value, no
// existing iterators are invalidated. When you remove a value, only iterators that
// pointed to the removed value are invalidated.
// 
////////////////////////////////////////////////////////////////////////////////
template<class T, class Traits = hash_traits<T> >
	//CompactMode = boost::mpl::or_<boost::is_pointer<T>, boost::mpl::bool_<Math::num_traits<T>::is_numeric> >::value >
class hash_set : protected Traits {
public:
	typedef const T key_type;
	typedef const T value_type;
	typedef unsigned int hash_type;
	typedef unsigned int size_type;
	typedef hash_set<T, Traits/*, CompactMode*/> hash_set_t;
protected:
	// Note: when an entry is removed from the hashtable, it is marked "free" by
	// setting the high bit of the "next" field so that the value of next is less
	// than -1 (-1 itself is an allocated entry with nothing following it in its
	// chain). The "next" field is also used as a linked list of free slots, so
	// next & 0x7FFFFFFF points to the next free slot. At the end of the free list, 
	// next is -2 (EndOfFreeList).
	struct SmallEntry
	{
		SmallEntry() {}
		SmallEntry(const T& t, hash_type hashCode, int next)
			: t(t), next(next) {}
		~SmallEntry() { assert (next >= -1); }

		T t;
		// Values of 'next' pointer:
		// next >= 0: entry is allocated; _entries[next] is the next entry in the same bucket
		// next = -1: entry is allocated; there are no more entries in this bucket
		// next = -2 (EndOfFreeList): entry is free; this is the end of the free list
		// next < -2: entry is free; (next & 0x7FFFFFFF) is the next entry in the free list
		int next;
	};
	struct NormalEntry
	{
		NormalEntry() {}
		NormalEntry(const T& t, hash_type hashCode, int next)
			: t(t), hashCode(hashCode), next(next) {}
		~NormalEntry() { assert (next >= -1); }

		T t;          // Value in this entry
		int next;     // See above for interpretation of 'next'
		hash_type hashCode; // Hash code of t
	};
	//typedef typename boost::mpl::if_c<CompactMode, SmallEntry, NormalEntry>::type Entry;
	typedef typename NormalEntry Entry;

	// This custom vector class allows entries in the middle of the list to be
	// constructed or destructed. This way, when elements are removed, they can be
	// destructed right then, so the caller doesn't have to wait for the whole
	// hashtable to be destroyed before the elements are destructed. A standard
	// vector cannot be abused this way, so use mini_vector as the base class.
	class EntryList : public mini_vector<Entry, 1> {
	public:
		~EntryList() { clear(); }
		void clear()
		{
			// Destruct only items that are not free
			for (int i = 0; i < (int)size(); i++)
				if (!IsFree(i))
					destruct(i);
			set_size(0);
		}
		void destruct(int index)
		{
			try {
				(begin() + index)->~Entry();
			} catch(...) { }
		}
		void construct(int index, const T& t, hash_type hashCode, int next)
		{
			if (index == size()) {
				if (!capacity_left())
					reallocate(size() * 2);
				set_size(size() + 1);
			} else
				assert (IsFree(index));
			new (begin() + index) Entry(t, hashCode, next);
		}
		bool IsFree(int entryIndex) const
			{ return operator[](entryIndex).next < -1; }
	};
	// An array of T values and "next" pointers. In general, some of the elements of
	// this array are free and some are allocated. If items are not removed from the
	// hashtable, _entries[0] is allocated first, then _entries[1], and so forth,
	// so only the end of the array is free. When the item at _entries[e] is
	// removed, that entry becomes the head of the free list (_freeList = e).
	EntryList _entries;
	// _entries[_buckets[b]] is the beginning of a linked list of items for which
	// (hash code % _bucketCount) == b.
	array_auto_ptr<int> _buckets;
	// Size of _buckets array (a prime number). The current policy is to roughly
	// double the size of _buckets when _entries.size() reaches _bucketCount.
	// Invariant: _bucketCount == 0 if and only if _buckets is NULL.
	int _bucketCount;
    // Number of free slots in _entries, or 0 if _entries is full
	int _freeCount;
    // Points to the first free slot in _entries, or EndOfFreeList if _entries is full
	int _freeList;

	enum { EndOfFreeList = -2 };
	
	bool IsFree(int entryIndex) const
		{ return _entries[entryIndex].next < -1; }
	hash_type Hash(const SmallEntry& entry) const
		{ return hash_value(entry.t); }
	hash_type Hash(const NormalEntry& entry) const
		{ return entry.hashCode; }
	bool IsMatch(const SmallEntry& entry, const T& t, hash_type keyHash) const
		{ return equals(entry.t, t); }
	bool IsMatch(const NormalEntry& entry, const T& t, hash_type keyHash) const
		{ return entry.hashCode == keyHash && equals(entry.t, t); }
	
public:
	hash_set() { Init(0); }
	explicit hash_set(size_type capacity, const Traits& traits = Traits())
		: Traits(traits)
	{
		Init(capacity);
	}

	// Puts the specified key in the set. If a matching key already exists, an
	// exception is thrown.
    void Add(const T& key)
	{
		if (Insert(key) < 0)
			throw exception_t(_T("Key already exists in hash_set"));
	}

	// Puts the specified key in the set if it is not already there. Returns true
	// if the specified key was actually added (it did not already exist). If
	// the key already existed, the hashtable is not modified.
	bool TryAdd(const T& key)
	{
		return Insert(key) >= 0;
	}

	// Puts the specified key in the set. If a matching key already exists, it is
	// overwritten with this new version of the key. Returns true if the specified
	// key did not already exist.
	bool Set(const T& key)
	{
		int i = Insert(key);
		if (i < 0)
			_entries[~i].t = key;
		return i >= 0;
	}

protected:
	// Returns an index >= 0 if a new key was inserted; if the key already existed,
	// a negative value ~index is returned and the hashtable is not modified. The 
	// index specifies which index in _entries contains the key.
	int Insert(const T& key)
	{
		if (_buckets == NULL)
			Init(3);
		
		// Look for an existing matching entry
		hash_type keyHash = hash_value(key);
		int bucketI = keyHash % _bucketCount;
		int entryI;
		for (entryI = _buckets[bucketI]; entryI >= 0; entryI = _entries[entryI].next)
		{
			if (IsMatch(_entries[entryI], key, keyHash))
				return ~entryI;
		}

		// Existing entry not found; add new
		assert ((_freeCount == 0) == (_freeList == EndOfFreeList));
		if (_freeCount > 0)
		{
			entryI = _freeList;
			_freeList = _entries[_freeList].next & 0x7FFFFFFF;
			if (--_freeCount == 0)
				_freeList = EndOfFreeList;
			else
				assert ((uint)_freeList < _entries.size());
		}
		else
		{
			if (_entries.size() == _bucketCount)
			{
				EnlargeBuckets();
				bucketI = keyHash % _bucketCount;
			}
			entryI = (int)_entries.size();
		}
		_entries.construct(entryI, key, keyHash, _buckets[bucketI]);
		_buckets[bucketI] = entryI;

		return entryI;
	}

public:
    void Clear()
	{
		_entries.clear();
		_buckets.reset();
		_bucketCount = 0;
		_freeCount = 0;
		_freeList = EndOfFreeList;
	}
	 
	bool Contains(const T& key) const
	{
		return FindEntry(key) < _entries.size();
	}

protected:
	// Returns the index of the entry that holds a matching key, or _entries.size()
	// if the key was not found.
	size_type FindEntry(const T& key) const
	{
		if (_buckets != NULL)
		{
			hash_type keyHash = hash_value(key);
			for (int i = _buckets[keyHash % _bucketCount]; i >= 0; i = _entries[i].next)
				if (IsMatch(_entries[i], key, keyHash))
					return i;
		}
		return _entries.size();
	}

	void Init(int capacity)
	{
		assert(_buckets.get() == NULL);
		_bucketCount = 0;
		_freeCount = 0;
		_freeList = EndOfFreeList;
		
		if (capacity)
			AllocBuckets(capacity);
	}
	void EnlargeBuckets()
	{
		AllocBuckets(_bucketCount * 2);
	}
	void AllocBuckets(int size)
	{
		assert (size > (int)_entries.size());
		_bucketCount = Math::GetNextPrime(size);
		_buckets.reset(new int[_bucketCount]);
		fill(_buckets.get(), _buckets.get() + _bucketCount, -1);

		for (int j = 0; j < (int)_entries.size(); j++)
		{
			int index = Hash(_entries[j]) % _bucketCount;
			_entries[j].next = _buckets[index];
			_buckets[index] = j;
		}
	}

public:
	bool Remove(const T& key)
	{
		if (_buckets == NULL)
			return false;

		hash_type keyHash = hash_value(key);
		int bucketI = keyHash % _bucketCount;
		int prevI = -1;
		for (int i = _buckets[bucketI]; i >= 0; i = _entries[i].next)
		{
			assert ((uint)i < _entries.size() && !IsFree(i));
			if (IsMatch(_entries[i], key, keyHash))
			{
				if (prevI < 0)
					_buckets[bucketI] = _entries[i].next;
				else
					_entries[prevI].next = _entries[i].next;
				
				assert ((uint)_freeList < _entries.size() || _freeList == EndOfFreeList);
				_entries.destruct(i);
				// We must guarantee .next < -1 so that iterators can detect
				// that it is a free slot, so use _freeList | 0x80000000.
				_entries[i].next = _freeList | 0x80000000u;
				_freeList = i;
				_freeCount++;

				return true;
			}
			prevI = i;
		}
		return false;
	}

	int Count() const { return (size_type)_entries.size() - _freeCount; }

	///////////////////////////////////////////////////////////////////////////////
	// support for copying ////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	
	hash_set(const hash_set_t& copy) : Traits(copy.traits()), _entries(copy._entries)
	{
		_bucketCount = copy._bucketCount;
		_freeCount = copy._freeCount;
		_freeList = copy._freeList;
		if (size())
			AllocBuckets(size());
	}
	hash_set_t& operator=(const hash_set_t& copy)
	{
		if (&copy != this) {
			Traits::operator=(copy);
			_entries = copy._entries;
			_bucketCount = copy._bucketCount;
			_freeCount = copy._freeCount;
			_freeList = copy._freeList;
			if (size())
				AllocBuckets(size());
		}
		return *this;
	}

	///////////////////////////////////////////////////////////////////////////////
	// iterators //////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	class const_iterator {
	protected:
		typedef const typename hash_set_t::value_type& reference;
		typedef const typename hash_set_t::value_type* pointer;
		typedef const hash_set_t hash_t;
		typedef const_iterator self;
		friend hash_t;

		hash_t* _hash;
		size_type _pos;
		
		const_iterator(const hash_t* hash, size_type pos) 
			: _hash(const_cast<hash_t*>(hash)), _pos(pos) { }
	public:
		const_iterator() : _hash(NULL) {}
		
		bool MoveNext()
		{
			if (_hash == NULL)
				return false;
			do
				if (++_pos >= _hash->_entries.size()) {
					_pos = _hash->_entries.size();
					return false;
				}
			while(_hash->IsFree(_pos));
			return true;
		}
		bool MovePrev()
		{
			if (_hash == NULL)
				return false;
			do
				if ((int)--_pos < 0) {
					_pos = (size_type)-1;
					return false;
				}
			while(_hash->IsFree(_pos));
			return true;
		}

		reference operator*() const { return _hash->_entries.at(_pos).t; }
		pointer operator->() const { return &_hash->_entries.at(_pos).t; }
		
		self& operator++() // prefix ++
			{ MoveNext(); return *this; }
		self& operator--() // prefix --
			{ MovePrev(); return *this; }
		bool operator==(const self& x) const
			{ return (x._hash == _hash) && (x._pos == _pos); }
		bool operator<(const self& x) const
			{ assert(_hash == x._hash); return _pos < x._pos; }
		bool operator<=(const self& x) const
			{ assert(_hash == x._hash); return _pos <= x._pos; }
		
		// Returns true if the iterator can be dereferenced
		bool is_valid() const
			{ return _hash != NULL && _pos < _hash->_entries.size(); }

		/////////////////////////////////////////////////////////
		// Operators that are defined in terms of other operators
		
		self operator++(int) // postfix ++
		{
			self tmp = *this; // copy ourselves
			MoveNext();
			return tmp;
		}
		self operator--(int) // postfix --
		{
			self tmp = *this; // copy ourselves
			MovePrev();
			return tmp;
		}
		bool operator!=(const self& x) const { return !(*this == x); }
		bool operator>(const self& x) const { return !(*this <= x); }
		bool operator>=(const self& x) const { return !(*this < x); }

		const hash_t* collection() const { return _hash; }
	};

	class iterator : public const_iterator {
	protected:
		typedef typename hash_set_t::value_type& reference;
		typedef typename hash_set_t::value_type* pointer;
		typedef iterator self;
		friend hash_t;

		iterator(hash_t* hash, size_type pos) : const_iterator(hash, pos) {}
	public:
		iterator() {}

		reference operator*() const { return _hash->_entries.at(_pos).t; }
		pointer operator->() const { return &_hash->_entries.at(_pos).t; }

		self& operator++() // prefix ++
			{ MoveNext(); return *this; }
		self& operator--() // prefix --
			{ MovePrev(); return *this; }
		
		self operator++(int) // postfix ++
		{
			self tmp = *this; // copy ourselves
			MoveNext();
			return tmp;
		}
		self operator--(int) // postfix --
		{
			self tmp = *this; // copy ourselves
			MovePrev();
			return tmp;
		}

		hash_t* collection() const { return _hash; }
	};

	friend class const_iterator;
	friend class iterator;

	///////////////////////////////////////////////////////////////////////////////
	// STL-style interface ////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	size_type size() const { return Count(); }
	size_type capacity() const { return _entries.capacity(); }
	bool empty() const { return Count() == 0; }
	void clear() { Clear(); }

	iterator begin()
	{
		iterator it(this, (size_type)-1);
		return ++it;
	}
	const_iterator begin() const
	{
		const_iterator it(this, (size_type)-1);
		return ++it;
	}
	iterator end()
	{
		return iterator(this, (size_type)_entries.size());
	}
	const_iterator end() const
	{
		return const_iterator(this, _entries.size());
	}
	iterator find(const T& key)
	{
		return iterator(this, FindEntry(key));
	}
	const_iterator find(const T& key) const
	{
		return const_iterator(this, FindEntry(key));
	}
	std::pair<iterator, bool> insert(const T& key)
	{
		int i = Insert(key);
		return std::make_pair(at(i >= 0 ? i : ~i), i >= 0);
	}
	template<class iter>
	void insert(iter start, const iter& stop)
	{
		for (; start != stop; ++start)
			Insert(*start);
	}
	int count(const T& key) const
	{
		return (int)(FindEntry(key) < _entries.size());
	}
	void erase(const const_iterator& pos)
	{
		assert (pos._hash == this);
		if (pos._hash == this && pos.is_valid()) {
			bool success = Remove(*pos);
			assert(success);
		}
	}
	const Traits& traits() const { return *this; }

protected:
	const_iterator at(int index) const { return const_iterator(this, index); }
	iterator at(int index) { return iterator(this, index); }
};

template<class Pair, class KeyTraits>
struct hash_map_traits : KeyTraits
{
	hash_map_traits() {}
	hash_map_traits(const KeyTraits& copy) : KeyTraits(copy) {}

	typedef Pair value_type;
	typedef typename Pair::first_type key_type;
	typedef typename Pair::second_type mapped_type;
	
	bool equals(const Pair& a, const Pair& b) const 
		{ return KeyTraits::equals(a.first, b.first); }
	unsigned hash_value(const Pair& pair) const
		{ return KeyTraits::hash_value(pair.first); }
};

///////////////////////////////////////////////////////////////////////////////
// 
// HH                     HH                                    
// HH                     HH                                    
// HHHHHH    AAAA    SSSs HHHHHH         MMmmMm   AAAA   PPPP   
// HH   HH      AA  SS    HH   HH       MM MM MM     AA  PP  PP 
// HH   HH   AAAAA   SSS  HH   HH       MM MM MM  AAAAA  PP  PP 
// HH   HH  AA  AA     SS HH   HH       MM    MM AA  AA  PP  PP 
// HH   HH   AAA AA sSSS  HH   HH _____ MM    MM  AAA AA PPPPP  
//                                                       PP     
//                                                       PP     
///////////////////////////////////////////////////////////////////////////////
// hash_map ///////////////////////////////////////////////////////////////////
// 
// hash_map is a dictionary: it associates values with keys. Duplicate keys are 
// not allowed. hash_map uses the same underlying implementation as hash_set;
// see hash_set's header comment for details.
// 
template<class TKey, class TValue, class KeyTraits = hash_traits<TKey> >//, bool CompactMode = Math::num_traits<TKey>::is_numeric>
class hash_map : protected hash_set< std::pair<const TKey,TValue>, 
                     hash_map_traits<std::pair<const TKey,TValue>,KeyTraits> >//, CompactMode >
{
public:
	typedef hash_map<TKey, TValue, KeyTraits/*, CompactMode*/> hash_map_t;
	typedef const TKey key_type;
	typedef const TValue mapped_type;
	typedef std::pair<const TKey,TValue> value_type;
	
	hash_map() {}
	explicit hash_map(size_type capacity, const KeyTraits& traits = KeyTraits())
		: hash_set(capacity, hash_map_traits(traits)) {}

	// Puts the specified key in the set. If a matching key already exists, an
	// exception is thrown.
    void Add(const TKey& key, const TValue& value)
	{
		if (Insert(value_type(key, value)) < 0)
			throw exception_t(_T("Key already exists in hash_map"));
	}

	// Puts the specified key in the set if it is not already there. Returns true
	// if the specified key was actually added (it did not already exist).
	bool TryAdd(const TKey& key, const TValue& value)
	{
		return Insert(value_type(key, value)) >= 0;
	}

	// Puts the specified key in the set. If a matching key already exists, it is
	// overwritten with this new version of the key. Returns true if the specified
	// key did not already exist.
	bool Set(const TKey& key, const TValue& value)
	{
		int i = Insert(value_type(key, value));
		if (i < 0)
			_entries[~i].t.second = value;
		return i >= 0;
	}

	bool TryGet(const TKey& key, OUT TValue& value)
	{
		size_type i = FindEntry(key);
		if (i < _entries.size()) {
			value = _entries[i].t.second;
			return true;
		}
		return false;
	}
	TValue Get(const TKey& key, TValue defaultValue)
	{
		TryGet(key, OUT defaultValue);
		return defaultValue;
	}

	void Clear()
		{ hash_set::Clear(); }
	bool Contains(const TKey& key)
		{ return FindEntry(key) < _entries.size(); }
	bool Remove(const TKey& key)
		{ return hash_set::Remove(value_type(key, TValue())); }
	int Count() const
		{ return hash_set::Count(); }

protected:
	// Returns the index of the entry that holds a matching key, or _entries.size()
	// if the key was not found.
	size_type FindEntry(const TKey& key) const
	{
		if (_buckets != NULL)
		{
			hash_type keyHash = (hash_type)KeyTraits::hash_value(key);
			for (int i = _buckets[keyHash % _bucketCount]; i >= 0; i = _entries[i].next)
				if (IsMatch(_entries[i], key, keyHash))
					return i;
		}
		return (size_type)_entries.size();
	}
	bool IsMatch(const SmallEntry& entry, const TKey& key, hash_type keyHash) const
		{ return KeyTraits::equals(entry.t.first, key); }
	bool IsMatch(const NormalEntry& entry, const TKey& key, hash_type keyHash) const
		{ return entry.hashCode == keyHash && KeyTraits::equals(entry.t.first, key); }

public:
	///////////////////////////////////////////////////////////////////////////////
	// support for copying ////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	
	hash_map(const hash_map_t& copy) : hash_set(copy) {}
	hash_map_t& operator=(const hash_set_t& copy) 
		{ hash_set::operator=(copy); return *this; }

	///////////////////////////////////////////////////////////////////////////////
	// STL-style interface ////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	typedef typename hash_set::const_iterator const_iterator;
	typedef typename hash_set::iterator       iterator;

	size_type size() const { return hash_set::Count(); }
	size_type capacity() const { return hash_set::capacity(); }
	bool empty() const { return _entries.empty(); }
	void clear() { hash_set::Clear(); }

	iterator begin() { return hash_set::begin(); }
	const_iterator begin() const { return hash_set::begin(); }
	iterator end() { return hash_set::end(); }
	const_iterator end() const { return hash_set::end(); }

	iterator find(const TKey& key)
	{
		return at(FindEntry(key));
	}
	const_iterator find(const TKey& key) const
	{
		return at(FindEntry(key));
	}
	std::pair<iterator, bool> insert(const TKey& key, const TValue& value)
	{
		int i = Insert(value_type(key, value));
		return std::make_pair(at(i >= 0 ? i : ~i), i >= 0);
	}
	std::pair<iterator, bool> insert(const value_type& t)
	{
		int i = Insert(t);
		return std::make_pair(at(i >= 0 ? i : ~i), i >= 0);
	}
	template<class iter>
	void insert(iter start, const iter& stop)
	{
		for (; start != stop; ++start)
			Insert(value_type(start->first, start->second));
	}
	int count(const TKey& key) const
	{
		return (int)(FindEntry(key) < _entries.size());
	}
	void erase(const const_iterator& pos)
	{
		hash_set::erase(pos);
	}
	bool erase(const TKey& key)
	{
		return hash_set::Remove(value_type(key, TValue()));
	}

	TValue& operator[](const TKey& key)
	{
		int i = Insert(value_type(key, TValue()));
		if (i < 0)
			i = ~i; // key already existed
		return _entries[i].t.second;
	}

	const KeyTraits& traits() const { return *this; }
};

///////////////////////////////////////////////////////////////////////////////
// Other traits classes ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifdef _COMMON_MORESTRING_H
// Specialization of hash_traits for more_string.
template<typename CHAR>
struct hash_traits< more_string<CHAR> >
{
	typedef more_string<CHAR> value_type;
	bool equals(const value_type& a, const value_type& b) const { return a == b; }
	size_t hash_value(const value_type& key) const
		{ return hash_traits< basic_string<CHAR> >().hash_value(key); }
};
#endif

#ifdef _COMMON_FIXEDPOINT_H
// Specialization of hash_traits for FixedPoint<FRAC,INT>.
template<int FRAC, typename INT>
struct hash_traits< Math::FixedPoint<FRAC,INT> >
{
	typedef Math::FixedPoint<FRAC,INT> value_type;
	bool equals(const value_type& a, const value_type& b) const { return a == b; }
	size_t hash_value(const value_type& key) const
		{ return (size_t)key.N; }
};
#endif

#endif // _COMMON_HASHTABLE_H
