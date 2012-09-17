/** @file ZooStringValueBlock.h
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2011-11-06
 */


#ifndef ZOOSTRINGVALUEBLOCK_H
#define ZOOSTRINGVALUEBLOCK_H

#include <string>
#include <vector>
#include <limits>
#include <exception>

#include <iostream>
#include <cstdio>

#include "ZooLikely.h"
#include "ZooKeyValueBlock.h"
#include "ZooStringToUIDTable.h"

#include "TRef.h"

/** @brief maps a string to a (floating point) value
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2011-11-06
 *
 * Internally, this mapping is done with a mapping from string to a unique
 * identifier (UID) which is provided by a ZooStringToUIDTable; this
 * ZooStringToUIDTable has to come pre-filled with all strings that are going
 * to be used as indices into this ZooStringValueBlock.
 *
 * The ZooStringValueBlock is not meant to be used directly as a Zoo info
 * block, please derive your infoblocks from this class by either inhertiting
 * from it or making it a member of your info block class.
 *
 * The interface is very similar to ZooKeyValueBlock (deliberately), with the
 * exception that an exception is thrown if you try to access data and the
 * internal reference to the ZooStringToUIDTable is not set properly or if you
 * try to insert, erase or modify a value for a string or UID which does not
 * exist in the ZooStringToUIDTable associated with this class.
 */
class ZooStringValueBlock : protected ZooKeyValueBlock
{
    private:
	TRef m_uidtbl; ///< table providing string to UID mapping

    public:
	/// report severe errors in ZooStringValueBlock as an exception
	class ZooStringValueBlockException;
	/// a const_iterator to loop over the contents of the ZooStringValueBlock
	class const_iterator;
	/// a key/uid-value pair
	typedef ZooKeyValueBlock::KeyValuePair KeyValuePair;
	/// a vector of key/uid-value pairs
	typedef ZooKeyValueBlock::KeyValueVector KeyValueVector;
	/// a string-value pair
	typedef std::pair<std::string, Float_t> StringValuePair;
	/// a vector of string-value pairs
	typedef std::vector<StringValuePair> StringValueVector;
	/// read-only proxy returned by non-const operator[]
	typedef ZooKeyValueBlock::r_proxy r_proxy;
	/// read-write proxy returned by non-const operator[]
	typedef ZooKeyValueBlock::rw_proxy rw_proxy;

	/// constructor
	ZooStringValueBlock(const ZooStringToUIDTable& uidtbl);

	/// constructor from a vector of (string, value) pairs
	ZooStringValueBlock(const ZooStringToUIDTable& uidtbl,
		const StringValueVector& stringValueVect);

	/// constructor from a vector of (uid, value) pairs
	ZooStringValueBlock(const ZooStringToUIDTable& uidtbl,
		const KeyValueVector& UIDValueVect);

	/// constructor from a range of std::map-style iterators
	template <class I> ZooStringValueBlock(
		const ZooStringToUIDTable& uidtbl,
		I begin, const I& end) :
	    m_uidtbl(const_cast<ZooStringToUIDTable*>(&uidtbl))
        { insert(begin, end); }

	/// destructor
	virtual ~ZooStringValueBlock();

	/// return a const reference to the string to uid mapping table
	const ZooStringToUIDTable& uidtbl() const;

	/** @brief get UID for given string
	 *
	 * @param str	string for which to find the UID
	 * @returns	UID of string or 0 if no UID has been assigned
	 */
	UInt_t getUID(const std::string& str) const { return uidtbl()[str]; }

	/** @brief get String for given UID
	 *
	 * @param uid	UID for which to find the assigned string
	 * @returns	string assigned to UID or empty string (if UID unassigned)
	 */
	const std::string& getString(UInt_t uid) const
	{ return uidtbl()[uid]; }

	/// return a const_iterator to the beginning of the ZooStringValueBlock
	const_iterator begin() const;

	/// return a const_iterator to the end of the ZooStringValueBlock
	const_iterator end() const;

	/// return true if the ZooStringValueBlock is empty, false otherwise
	bool empty() const { return ZooKeyValueBlock::empty(); }

	/// return the number of entries in the ZooStringValueBlock
	unsigned size() const { return ZooKeyValueBlock::size(); }

	/// return the capacity of the ZooStringValueBlock
	unsigned capacity() const { return ZooKeyValueBlock::capacity(); }

	/// reserve a minimum capacity for this ZooStringValueBlock
	void reserve(unsigned capacity)
	{ return ZooKeyValueBlock::reserve(capacity); }

	/// clear this ZooStringValueBlock
	void clear() { ZooKeyValueBlock::clear(); }
	/// clear this ZooStringValueBlock
	virtual void Clear() { clear(); }

	/// check for the exsitence of a given UID
	bool find(UInt_t uid) const { return ZooKeyValueBlock::find(uid); }
	
	/// check for the exsitence of a given UID
	bool exists(UInt_t uid) const { return find(uid); }
	
	/** @brief return the value assiciated to a given UID
	 *
	 * @param uid	UID for which to return the value
	 * @returns	value if UID exists, NaN otherwise
	 */
	float value(UInt_t uid) const
	{ return ZooKeyValueBlock::operator[](uid); }

	/** @brief return the value assiciated to a given UID
	 *
	 * @param uid	UID for which to return the value
	 * @returns	value if UID exists, NaN otherwise
	 */
	float operator[](UInt_t uid) const { return value(uid); }

	/** @brief return or set the value assiciated to a given UID
	 *
	 * @param uid	UID for which to return or set the value
	 * @returns	a proxy which can serve as an lvalue for the value
	 */
	rw_proxy operator[](UInt_t uid);

	/** @brief erase a UID and the corresponding value from the ZooStringValueBlock
	 *
	 * @param uid	UID to erase
	 * @returns	old value mapped to given UID (if any)
	 */
	float erase(UInt_t uid);

	/// check for the exsitence of a given string
	bool find(const std::string& str) const;
	
	/// check for the exsitence of a given string
	bool exists(const std::string& str) const { return find(str); }

	/** @brief return the value assiciated to a given string
	 *
	 * @param str	string for which to return the value
	 * @returns	value if string exists, NaN otherwise
	 */
	float value(const std::string& str) const;

	/** @brief return the value assiciated to a given string
	 *
	 * @param str	string for which to return the value
	 * @returns	value if string exists, NaN otherwise
	 */
	float operator[](const std::string& str) const { return value(str); }

	/** @brief return or set the value assiciated to a given string
	 *
	 * @param str	string for which to return or set the value
	 * @returns	a proxy which can serve as an lvalue for the value
	 */
	rw_proxy operator[](const std::string& str);

	/** @brief erase a string and the corresponding value from the ZooStringValueBlock
	 *
	 * @param str	string to erase
	 * @returns	old value mapped to given string (if any)
	 */
	float erase(const std::string& str);

	/** @brief erase element pointed to by given iterator
	 *
	 * @param it	iterator pointing to element to erase
	 * @returns	iterator pointing to element following erased element
	 */
	const_iterator erase(const const_iterator& it);

	/** @brief insert a value for a given uid
	 *
	 * @param uid	UID for which to insert a value
	 * @param value	value to insert
	 * @returns	old value saved at that UID (if any)
	 */
	float insert(UInt_t uid, float value);

	/** @brief insert a value for a given string
	 *
	 * @param str	string for which to insert a value
	 * @param value	value to insert
	 * @returns	old value saved at that UID (if any)
	 */
	float insert(const std::string& str, float value);

	/** @brief modify a value for a given uid
	 *
	 * @param uid	UID for which to modify a value
	 * @param value	value to modify
	 * @returns	true if UID exists, false if it had to be inserted
	 */
	bool modify(UInt_t uid, float value);

	/** @brief modify a value for a given string
	 *
	 * @param str	string for which to modify a value
	 * @param value	value to modify
	 * @returns	true if string exists, false if it had to be inserted
	 */
	bool modify(const std::string& str, float value);

	/** @brief insert elements from a vector of (string, value) pairs
	 *
	 * @param stringValueVect	vector with (string, value) pairs
	 * @returns			nothing
	 */
	void insert(const StringValueVector& stringValueVect);

	/** @brief insert elements from a vector of (UID, value) pairs
	 *
	 * @param UIDValueVect	vector with (UID, value) pairs
	 * @returns		nothing
	 */
	void insert(const KeyValueVector& UIDValueVect);

	/** @brief insert elements from a range
	 *
	 * the range is given by two iterators which return std::map style
	 * pairs of either (uid, value) or (string, value)
	 *
	 * @param begin	start of range of (key, value) pairs
	 * @param end	end of range of (key, value) pairs
	 * @returns	nothing
	 */
	template<class I> void insert(I begin, const I& end)
	{
	    ZooKeyValueBlock::reserve(
		    std::min(size() + unsigned(end - begin), uidtbl().size()));
	    for ( ; begin != end; ++begin)
		insert((*begin).first, (*begin).second);
	}

	/** @brief dump the contents of this ZooStringValueBlock to an ostream
	 *
	 * @param ostr	ostream to dump to
	 * @returns	nothing
	 */
	void dump(std::ostream& ostr = std::cout) const;

	/** @brief dump the contents of this ZooStringValueBlock to a FILE
	 *
	 * @param f	FILE to dump to
	 * @returns	nothing
	 */
	void dump(FILE* f) const;

	// default constructor (for ROOT I/O)
	ZooStringValueBlock() { }

    private:
/// sorry, this is bad. This is a hotfix for gcc46. FIXME:check carefully, whether needed.
	friend class ZooWriter;
/// sorry, this is bad. This is a hotfix for gcc46. FIXME:check carefully, whether needed.
	friend class ZooP;
/// sorry, this is bad. This is a hotfix for gcc46. FIXME:check carefully, whether needed.
	friend class ZooObjectManager;
	ClassDef(ZooStringValueBlock, 1);
};

/// report severe errors in ZooStringValueBlock as an exception
class ZooStringValueBlock::ZooStringValueBlockException :
    public std::exception
{
    private:
	const char* m_what; ///< reason for exception
    public:
	/// constructor
	ZooStringValueBlockException(const char* what) throw() :
	    m_what(what) { }
	/// destructor
	virtual ~ZooStringValueBlockException() throw() { }
	/// return a C style string describing what went wrong
	virtual const char* what() throw() { return m_what; }
};


class ZooStringValueBlock::const_iterator {
    private:
	const ZooStringValueBlock *m_parent; //!
	ZooKeyValueBlock::const_iterator m_it; //!
	/// for use by ZooKeyValueBlock's begin() and end()
	const_iterator(const ZooStringValueBlock* parent,
		ZooKeyValueBlock::const_iterator it) :
	    m_parent(parent), m_it(it) { }
	friend class ZooStringValueBlock;
    public:
	/** @brief type returned when deferencing
	 *
	 * Note: One might wish to return const reference to std::string
	 * instead of a string in value_type. Since the underlying
	 * ZooStringToUIDTable is assumed to stay constant during the lifetime
	 * of a ZooStringValueBlock, the consideration for the value_type in
	 * ZooStringToUIDTable::const_iterator does not apply here. However,
	 * with the STL being implemented as it is in some g++ versions (and
	 * references to references being forbidden by the C++ standard), I
	 * cannot make this work reliably for all compiler version with a
	 * const reference to std::string, so here we have to live with a
	 * string copy when deferencing a const_iterator, too. - Manuel
	 */
	typedef std::pair<
	    const std::pair<const UInt_t, const std::string>,
		  const float> value_type;
	/// copy constructor
	const_iterator(const ZooStringValueBlock::const_iterator& other) :
	    m_parent(other.m_parent), m_it(other.m_it) { }
	/// assignment
	const const_iterator& operator=(const ZooStringValueBlock::const_iterator& other)
	{
	    m_parent = other.m_parent;
	    m_it = other.m_it;
	    return *this;
	}
	/// comparison of iterators
	bool operator<(const const_iterator& other) const
	{
	    if (UNLIKELY(m_parent < other.m_parent)) return true;
	    if (UNLIKELY(other.m_parent < m_parent)) return false;
	    return m_it < other.m_it;
	}
	/// comparison of iterators
	bool operator>(const const_iterator& other) const
    { return other < *this; }
    /// comparison of iterators
    bool operator==(const const_iterator& other) const
    { return m_parent == other.m_parent && m_it == other.m_it; }
    /// comparison of iterators
    bool operator!=(const const_iterator& other) const
    { return ! (*this == other); }
    /// prefix increment
    const const_iterator& operator++() { ++m_it; return *this; }
    /// prefix decrement
    const const_iterator& operator--() { --m_it; return *this; }
    /// in-place add
    const const_iterator& operator+=(int c) { m_it += c; return *this; }
    /// in-place subtract
    const const_iterator& operator-=(int c) { m_it -= c; return *this; }
    /// postfix increment
    const_iterator operator++(int)
    { const_iterator tmp(*this); ++*this; return tmp; }
    /// postfix decrement
    const_iterator operator--(int)
    { const_iterator tmp(*this); --*this; return tmp; }
    /// addition
    const_iterator operator+(int c)
    { const_iterator tmp(*this); *this += c; return tmp; }
    /// subtraction
    const_iterator operator-(int c)
    { const_iterator tmp(*this); *this -= c; return tmp; }
    /// deference (returns map-style pair of (key, value) where key is a pair of (uid, string))
    value_type operator*() const
    {
	return value_type(
		std::make_pair<const UInt_t, const std::string>((*m_it).first,
		    std::string(m_parent->getString((*m_it).first))),
		    (*m_it).second);
    }
};

std::ostream& operator<<(std::ostream& ostr, const ZooStringValueBlock& zsv);

inline ZooStringValueBlock::const_iterator ZooStringValueBlock::begin() const
{
    return ZooStringValueBlock::const_iterator(
	    this, ZooKeyValueBlock::begin());
}

inline ZooStringValueBlock::const_iterator ZooStringValueBlock::end() const
{
    return ZooStringValueBlock::const_iterator(
	    this, ZooKeyValueBlock::end());
}

inline ZooStringValueBlock::const_iterator ZooStringValueBlock::erase(
	const ZooStringValueBlock::const_iterator& it)
{
    return ZooStringValueBlock::const_iterator(
	    this, ZooKeyValueBlock::erase(it.m_it));
}

#endif // ZOOSTRINGVALUEBLOCK_H

// vim: sw=4:tw=78:ft=cpp
