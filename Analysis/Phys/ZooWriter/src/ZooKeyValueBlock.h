/* @file ZooKeyValueBlock.h
 *
 * @author Paul Seyfert <pseyfert@physi.uni-heidelberg.de>
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2011-09-29
 */

#ifndef __ZOOKEYVALUEBLOCK_H
#define __ZOOKEYVALUEBLOCK_H

#include <vector>
#include <limits>
#include <iostream>
#include <cstdio>
#include <utility>

#include <TObject.h>

/** @class ZooKeyValueBlock: public TObject
 *  \brief class to hold key-value pairs, key is an unsigned and value a float
 *
 *  please don't use this class as an info block directly, derive from it,
 *  or use it as a building block for your own classes
 */
class ZooKeyValueBlock: public TObject
{
    public:
	typedef std::pair<UInt_t, Float_t> KeyValuePair;
	typedef std::vector<KeyValuePair> KeyValueVector;
	class const_iterator; ///< const_iterator type
	class r_proxy; ///< value proxy class to prevent writes in operator[]
	/// value proxy class to distinguish reads from writes in operator[]
	class rw_proxy;

	friend class ZooKeyValueBlock::const_iterator;
	friend class ZooKeyValueBlock::r_proxy;
	friend class ZooKeyValueBlock::rw_proxy;
	/// default constructor. Needed for ROOT
	ZooKeyValueBlock() {}
	/// usefull constructor. used in ZooWriter
	ZooKeyValueBlock(KeyValueVector& extrainfo);
	/// destructor
	virtual ~ZooKeyValueBlock();

	/// check if the property "key" exists
	bool exists(unsigned key) const;
	/// get the value of the property "key"
	float value(unsigned key) const;
	/// erase a key-value pair (return old value of key, if any)
	float erase(unsigned key);
	/// erase (returns iterator pointing to element behind the one just erased)
	const_iterator erase(const const_iterator& it);
	/// insert a key-value pair (return old value for same key, if any)
	float insert(unsigned key, float val);
	/// modify a value (true if key exists, false otherwise)
	bool modify(unsigned key, float val);

	/// return size of key-value map
	unsigned size() const { return m_idx.size(); }
	/// return capacity of key-value map
	unsigned capacity() const
	{ return std::min(m_idx.capacity(), m_val.capacity()); }
	/// extend capacity to at least min(size(), newsz)
	void reserve(unsigned newsz)
	{ newsz = std::min(newsz, size()); m_idx.reserve(newsz); m_val.reserve(newsz); }
	/// clear the key-value map
	void clear() { m_idx.clear(); m_val.clear(); }

	/// get value of the property "key"
	float operator[](unsigned key) const { return value(key); }
	/// set value of the property "key"
	rw_proxy operator[](unsigned key);

	/// print all properties to an std::ostream (default is cout)
	void dump(std::ostream& ostr = std::cout) const;
	/// print all properties to a C style file handle
	void dump(FILE* f) const; // no default arg so we can resolve overload

	/// const_iterator pointing to beginning of Map
	const_iterator begin() const;
	/// const_iterator pointing one element behind the end of Map
	const_iterator end() const;
    private:
	typedef std::vector<UInt_t> KeyVector;
	typedef std::vector<Float_t> ValueVector;
	KeyVector m_idx; ///< vector of keys
	ValueVector m_val; ///< vector of values

	/// real implementation of write-through operator[]
	float& opIndImpl(unsigned key, float value);
    protected:
	/// provide a way for derived classes to get an r_proxy
	r_proxy get_r_proxy(unsigned key) const;

	ClassDef(ZooKeyValueBlock,1);
};

class ZooKeyValueBlock::const_iterator {
    private:
	const ZooKeyValueBlock *m_parent; //!
	unsigned m_idx; //!
	/// for use by ZooKeyValueBlock's begin() and end()
	const_iterator(const ZooKeyValueBlock* parent, unsigned idx) :
	    m_parent(parent), m_idx(idx) { }
	friend class ZooKeyValueBlock;
    public:
	/// copy constructor
	const_iterator(const const_iterator& other) :
	    m_parent(other.m_parent), m_idx(other.m_idx) { }
	/// assignment
	const const_iterator& operator=(const const_iterator& other)
	{
	    m_parent = other.m_parent;
	    m_idx = other.m_idx;
	    return *this;
	}
	/// comparison of iterators
	bool operator<(const const_iterator& other) const
	{
	    if (m_parent < other.m_parent) return true;
	    if (other.m_parent < m_parent) return false;
	    return m_idx < other.m_idx;
	}
	/// comparison of iterators
	bool operator>(const const_iterator& other) const
	{ return other < *this; }
	/// comparison of iterators
	bool operator==(const const_iterator& other) const
	{ return m_parent == other.m_parent && m_idx == other.m_idx; }
	/// comparison of iterators
	bool operator!=(const const_iterator& other) const
	{ return ! (*this == other); }
	/// comparison of iterators
	const const_iterator& operator++() { ++m_idx; return *this; }
	/// comparison of iterators
	const const_iterator& operator--() { --m_idx; return *this; }
	/// comparison of iterators
	const const_iterator& operator+=(int c) { m_idx += c; return *this; }
	/// comparison of iterators
	const const_iterator& operator-=(int c) { m_idx -= c; return *this; }
	/// comparison of iterators
	const_iterator operator++(int)
	{ const_iterator tmp(*this); ++*this; return tmp; }
	/// comparison of iterators
	const_iterator operator--(int)
	{ const_iterator tmp(*this); --*this; return tmp; }
	/// comparison of iterators
	const_iterator operator+(int c)
	{ const_iterator tmp(*this); *this += c; return tmp; }
	/// comparison of iterators
	const_iterator operator-(int c)
	{ const_iterator tmp(*this); *this -= c; return tmp; }
	/// deference (returns map-style pair of (key, value))
	std::pair<unsigned, float> operator*() const
	{
	    return std::make_pair(m_parent->m_idx[m_idx],
		    m_parent->m_val[m_idx]);
	}
};

class ZooKeyValueBlock::r_proxy {
    protected:
	ZooKeyValueBlock *m_parent; //!
	unsigned m_key; //!
	/// for use by ZooKeyValueBlock's operator[]
	r_proxy(ZooKeyValueBlock* parent, unsigned key) :
	    m_parent(parent), m_key(key) { }
	friend class ZooKeyValueBlock;
    public:
	/// copy constructor
	r_proxy(const r_proxy& other) :
	    m_parent(other.m_parent), m_key(other.m_key) { }
	/// read access to ZooKeyValueBlock
	operator float() const { return m_parent->value(m_key); }
};

class ZooKeyValueBlock::rw_proxy : public ZooKeyValueBlock::r_proxy {
    protected:
	/// for use by ZooKeyValueBlock's operator[]
	rw_proxy(ZooKeyValueBlock* parent, unsigned key) :
	    r_proxy(parent, key) { }
	friend class ZooKeyValueBlock;
    private:
	rw_proxy(const r_proxy&); ///< forbidden
    public:
	/// copy constructor
	rw_proxy(const rw_proxy& other) : r_proxy(other) { }
	/// assignment from other rw_proxy: in reality, a write access
	const float& operator=(const r_proxy& other)
	{ return m_parent->opIndImpl(m_key, float(other)); }
	/// assignment from float: also write access
	const float& operator=(float value)
	{ return m_parent->opIndImpl(m_key, value); }
	/// read access to ZooKeyValueBlock
	operator float() const { return m_parent->value(m_key); }
};

// inline methods
inline ZooKeyValueBlock::const_iterator ZooKeyValueBlock::begin() const
{ return const_iterator(this, 0); }
inline ZooKeyValueBlock::const_iterator ZooKeyValueBlock::end() const
{ return const_iterator(this, m_idx.size()); }
inline ZooKeyValueBlock::const_iterator ZooKeyValueBlock::erase(
	const ZooKeyValueBlock::const_iterator& it)
{ erase((*it).first); return it; }
inline ZooKeyValueBlock::rw_proxy ZooKeyValueBlock::operator[](unsigned key)
{ return rw_proxy(this, key); }
inline ZooKeyValueBlock::r_proxy ZooKeyValueBlock::get_r_proxy(unsigned key) const
{ return r_proxy(const_cast<ZooKeyValueBlock*>(this), key); }

/// output to cout and the like
std::ostream& operator<<(std::ostream& os, const ZooKeyValueBlock& zkv);

#endif /* __ZOO_H */


