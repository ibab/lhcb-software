/**
 * @file ZooStringToUIDTable.h
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2011-11-04
 *
 * class definition of ZooStringToUIDTable
 */

#ifndef ZOOSTRINGTOUIDTABLE_H
#define ZOOSTRINGTOUIDTABLE_H

#include <vector>
#include <string>
#include <algorithm>

#include <iostream>
#include <cstdio>

#include <TObject.h>

#include "ZooLikely.h"

/**
 * @class ZooStringToUIDTable
 * @brief maps a string to a 32 bit UID (unique identifier)
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2011-11-04
 *
 * ZooStringToUIDTable provides a mapping between a set of strings and a 32
 * bit UID.  The order in which strings are entered matters for the UID
 * generation (unless you can prove that the set of strings you enter cannot
 * produce a hash collision). This class is therefore most useful for a given
 * predetermined set of strings.
 *
 * String registration is done with the insert method:
 *
 * @code
 * ZooStringToUIDTable UIDmap;
 * UInt_t uid_two = UIDmap.insert("two");
 * @endcode
 *
 * Checking for the existence of a UID or a string can be done with the find
 * or exists methods:
 *
 * @code
 * ZooStringToUIDTable UIDmap;
 * const std::string str = "fOObAr";
 * const UInt_t uid = 42424242u;
 * if (UIDmap.find(str)) { // UIDmap.exists(str) does the same
 *     // str exists
 * }
 * if (UIDmap.find(uid)) { // UIDmap.exists(uid) does the same
 *     // uid exists
 * }
 * @endcode
 *
 * operator[] can be used to return the string registered with a UID (if any),
 * or to look up an UID for a given string:
 *
 * @code
 * ZooStringToUIDTable UIDmap;
 * // get uid of "one"
 * UInt_t uid_one = UIDmap["one"];
 * // returns 0 (invalid UID) if "one" does not exist
 * const std::string& str = UIDmap[42356u];
 * if (!str.empty()) {
 *     // str is mapped to uid 42356u
 * } else {
 *     // uid does not exist
 * }
 * @endcode
 *
 * Implementation note: Internally, the functionality of this class is
 * realised by associating each string with its hash value. Potential
 * collisions are resolved by assigning the next free UID. This is why the
 * order in which strings are entered matters for the UID generation.
 * (Obviously, if there are no hash collisions, the order does not matter at
 * all). Using this construction has the advantage that comparisons between
 * a string to be looked up and the list of existing strings is fast, because
 * hash values can be computed quickly and compared to other hash values much
 * faster than strings can. If a matching hash value is found, we have to
 * compare the lookup string to a single string in case there are no
 * collisions; in case of collisions we have to compare to at most a few
 * strings, and each of these comparisons should be fast since the strings
 * that map to the same hash value are unlikely to have the same starting
 * sequence.
 */
class ZooStringToUIDTable : public TObject
{
    private:
	/// assigned uids
	std::vector<UInt_t> m_uids;
	/// assigned strings
	std::vector<std::string> m_strs;
	/// flag indicating if a hash collision has occurred
	bool m_hashcoll;

	/// the multiplier used in 32 bit FNV1a
	static const UInt_t s_FNV1a32Multiplier = 16777619u;
	/// the offset used in 32 bit FNV1a
	static const UInt_t s_FNV1a32Offset = 2166136261u;
	/// the empty string maps to UID 0
	static const std::string s_empty;

    public:
	/// const_iterator class
	class const_iterator;
	friend class const_iterator;

	/// default constructor
	ZooStringToUIDTable() : m_hashcoll(false) { }

	/** @brief constructor from a set of iterators or pointers
	 *
	 * this will handle iterators into a container of either std::string
	 * or const char*
	 * @param begin	begin of range of strings to insert
	 * @param end	end of range of strings to insert
	 */
	template <class T> ZooStringToUIDTable(T begin, const T& end) :
	    m_hashcoll(false)
	{ insert(begin, end); }

	/** @brief constructor from a vector of strings
	 *
	 * @param strings	vector of strings to insert
	 */
	ZooStringToUIDTable(const std::vector<std::string>& strings) :
	    m_hashcoll(false)
	{ insert(strings.begin(), strings.end()); }

	/// destructor
	~ZooStringToUIDTable() { }
 
	/** @brief return the size of the ZooStringToUIDTable
	 *
	 * @returns	size of the ZooStringToUIDTable
	 */
	unsigned size() const { return m_uids.size(); }

	/** @brief return true if the ZooStringToUIDTable is empty
	 *
	 * @returns true if the ZooStringToUIDTable is empty, false otherwise
	 */
	bool empty() const { return m_uids.empty(); }

	/** @brief return the capacity of the ZooStringToUIDTable
	 *
	 * @returns	capacity of the ZooStringToUIDTable
	 */
	unsigned capacity() const
	{ return std::min(m_uids.capacity(), m_strs.capacity()); }

	/** @brief reserve the capacity of the ZooStringToUIDTable
	 *
	 * @param cap	new capacity of the ZooStringToUIDTable
	 */
	void reserve(unsigned cap)
	{ m_uids.reserve(cap); m_strs.reserve(cap); }

	/** @brief clear the contents of this ZooStringToUIDTable
	 */
	void clear() { m_uids.clear(); m_strs.clear(); m_hashcoll = false; }
	/** @brief clear the contents of this ZooStringToUIDTable
	 */
	virtual void Clear() { TObject::Clear(); clear(); }

	/** @brief computes the hash value of a given string
	 *
	 * @param str	string for which to compute hash value
	 * @returns 	hash value
	 */
	static UInt_t hash(const std::string& str);

	/** @brief return true if there has been at least one hash collision
	 *
	 * @returns 	true or false, depending on whether there has been a
	 * 		collision
	 */
	bool hasCollided() const { return m_hashcoll; }

	/** @brief insert a string into the ZooStringToUIDTable
	 *
	 * @param str	string to insert
	 * @returns	UID assigned to the string
	 */
	UInt_t insert(const std::string& str)
	{ return insert(str, hash(str)); }

	/** @brief insert a string into the ZooStringToUIDTable
	 *
	 * @param str	string to insert
	 * @returns	UID assigned to the string
	 */
	UInt_t insert(const char* str) { return insert(std::string(str)); }

	/** @brief insert a range of strings into the ZooStringToUIDTable
	 *
	 * @param begin	begin of range of strings to insert
	 * @param end	end of range of strings to insert
	 */
	template <class T> void insert(T begin, const T& end)
	{
	    m_uids.reserve(m_uids.size() + (end - begin));
	    m_strs.reserve(m_strs.size() + (end - begin));
	    for ( ; end != begin; ++begin) insert(*begin);
	}

	/** @brief checks if UID exists
	 *
	 * @param uid	UID for which to test for existence
	 * @returns 	true or false, depending on whether uid exists
	 */
	bool find(UInt_t uid) const;

	/** @brief checks if UID exists
	 *
	 * @param uid	UID for which to test for existence
	 * @returns 	true or false, depending on whether uid exists
	 */
	bool exists(UInt_t uid) const { return find(uid); }

	/** @brief lookup the string corresponding to a given UID
	 *
	 * @param uid	UID for which to find the string
	 * @returns	string mapped to UID uid
	 */
	const std::string& operator[](UInt_t uid) const;

	/** @brief lookup the UID corresponding to a given string
	 *
	 * @param str	string for which to find the UID
	 * @returns	UID mapped to given string (or 0 if not found)
	 */
	UInt_t operator[](const std::string& str) const;

	/** @brief lookup the UID corresponding to a given string
	 *
	 * @param str	string for which to find the UID
	 * @returns	UID mapped to given string (or 0 if not found)
	 */
	UInt_t operator[](const char* str) const
	{ return operator[](std::string(str)); }

	/** @brief checks if string exists
	 *
	 * @param str	string for which to test for existence
	 * @returns 	true or false, depending on whether string exists
	 */
	bool find(const std::string& str) const
	{ return 0 != operator[](str); }

	/** @brief checks if string exists
	 *
	 * @param str	string for which to test for existence
	 * @returns 	true or false, depending on whether string exists
	 */
	bool exists(const std::string& str) const { return find(str); }

	/** @brief checks if string exists
	 *
	 * @param str	string for which to test for existence
	 * @returns 	true or false, depending on whether string exists
	 */
	bool find(const char* str) const
	{ return 0 != operator[](str); }

	/** @brief checks if string exists
	 *
	 * @param str	string for which to test for existence
	 * @returns 	true or false, depending on whether string exists
	 */
	bool exists(const char* str) const { return find(str); }

	/** @brief dump the contents to a FILE
	 *
	 * @param f	FILE to dump to
	 */
	void dump(FILE* f) const;

	/** @brief dump the contents to an ostream
	 *
	 * @param ostr	ostream to dump to
	 */
	void dump(std::ostream& ostr = std::cout) const;

	/** @brief return a const_iterator to the start of the ZooStringToUIDTable
	 *
	 * @returns const_iterator to the start of the ZooStringToUIDTable
	 */
	const_iterator begin() const;

	/** @brief return a const_iterator to the end of the ZooStringToUIDTable
	 *
	 * @returns const_iterator to the end of the ZooStringToUIDTable
	 */
	const_iterator end() const;

    private:
	/** @brief the routine doing the actual work or inserting a string
	 *
	 * this routine inserts a string and assigns its uid based on the
	 * given hash value
	 *
	 * @param str	string
	 * @param hash	string's hash value
	 * @returns	uid
	 */
	UInt_t insert(const std::string& str, UInt_t hash);

	ClassDef(ZooStringToUIDTable, 1);
};

class ZooStringToUIDTable::const_iterator {
    private:
        const ZooStringToUIDTable *m_parent; //!
        unsigned m_idx; //!
        /// for use by ZooStringToUIDTable's begin() and end()
        const_iterator(const ZooStringToUIDTable* parent, unsigned idx) :
            m_parent(parent), m_idx(idx) { }
        friend class ZooStringToUIDTable;
    public:
	/** @brief type returned when deferencing
	 *
	 * Note: One might wish to return a pair of an UInt_t and a const
	 * reference to std::string. Unfortunately, this does not work because
	 * this could potentially invalidate the saved deferenced value of an
	 * interator: consider the situation where you have saved the return
	 * value of a deferenced iterator and then try to insert an
	 * (UID, string) pair after the point to which the iterator points -
	 * to behave like an STL container iterator, the iterator has to stay
	 * valid. But: the insert may cause the contents of the internal
	 * vectors in ZooStringToUIDTable to be moved in memory; while the
	 * iterator would stay valid, but the saved return value of the
	 * deferencing operation would not, since the reference to std::string
	 * would still point to the old location...
	 *
	 * Unfortunately, this means that deferencing an iterator involves a
	 * string copy, but right now I cannot see a way around this without
	 * introducing a pitfall to unsuspecting client code which is
	 * extremely hard to track down... - Manuel
	 */
	typedef std::pair<const UInt_t, const std::string> value_type;
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
            if (UNLIKELY(m_parent < other.m_parent)) return true;
            if (UNLIKELY(other.m_parent < m_parent)) return false;
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
        /// prefix increment
        const const_iterator& operator++() { ++m_idx; return *this; }
        /// prefix decrement
        const const_iterator& operator--() { --m_idx; return *this; }
        /// in-place add
        const const_iterator& operator+=(int c) { m_idx += c; return *this; }
        /// in-place subtract
        const const_iterator& operator-=(int c) { m_idx -= c; return *this; }
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
        /// deference (returns map-style pair of (uid, string))
        value_type operator*() const
        {
	    return value_type(m_parent->m_uids[m_idx],
		    std::string(m_parent->m_strs[m_idx]));
	}
};

inline ZooStringToUIDTable::const_iterator ZooStringToUIDTable::begin() const
{ return ZooStringToUIDTable::const_iterator(this, 0); }

inline ZooStringToUIDTable::const_iterator ZooStringToUIDTable::end() const
{ return ZooStringToUIDTable::const_iterator(this, size()); }

/// output to cout and the like
std::ostream& operator<<(std::ostream& os, const ZooStringToUIDTable& tbl);

#endif // ZOOSTRINGTOUIDTABLE_H

// vim: sw=4:tw=78:ft=cpp
