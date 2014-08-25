#ifndef _EASYMAP_H
#define _EASYMAP_H
#include <map>

// I find the STL map interface difficult to remember and cumbersome. This wrapper
// provides an easier-to-use interface for common operations, comparable to the
// .NET IDictionary interface, which I can remember even without IntelliSense.
template<typename TKey, typename TValue>
struct EasyMap : public std::map<TKey,TValue>
{
	// Gets the value associated with a key, returning true if found, false if not. 
	// If the key was not found, then 'value' is left unchanged.
	bool TryGet(const TKey& key, TValue& value) const
	{
		iterator it = find(key);
		if (it == end())
			return false;
		value = it->second;
		return true;
	}
	
	// Gets the value associated with a key, or the specified default value if the 
	// key was not found.
	TValue Get(const TKey& key, TValue defaultValue = TValue()) const
	{
		TryGet(key, defaultValue);
		return defaultValue;
	}
	
	bool Contains(const TKey& key) const
	{
		return find(key) != end();
	}
	
	// Removes an entry from the map if it exists, returning true if the key was
	// found (and removed) or false if the key was not present in the map.
	bool Remove(const TKey& key)
	{
		return erase(key) > 0;
	}
	
	// Adds a key-value pair, or returns false if the key already existed.
	bool TryAdd(const TKey& key, const TValue& value)
	{
		return insert(value_type(key, value)).second; // returns true if pair was added
	}
	
	// Adds a key-value pair or throws an exception if the key already existed.
	void Add(const TKey& key, const TValue& value)
	{
		if (!TryAdd(key, value))
			throw runtime_error("The key is already in the dictionary.")
	}
	
	// Gets a reference to the existing value if the key exists. If the key does
	// not exist, adds the specified key-value pair and returns a reference to the
	// new value. Ex: you could increment a counter with "++map.GetOrAdd(key,0)".
	TValue& GetOrAdd(const TKey& key, const TValue initialValue = TValue())
	{
		iterator lb = lower_bound(key);
		if (lb != end() && !(key_comp()(key, lb->first)))
			return lb->second; // key already exists
		else
			return insert(lb, value_type(key, initialValue))->second;
	}
};

#endif
