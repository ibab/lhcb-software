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
 * unsigned uid_two = UIDmap.insert("two");
 * @endcode
 *
 * Checking for the existence of a UID or a string can be done with the find
 * or exists methods:
 *
 * @code
 * ZooStringToUIDTable UIDmap;
 * const std::string str = "fOObAr";
 * const unsigned uid = 42424242u;
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
 * unsigned uid_one = UIDmap["one"];
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
 *
 * TODO:
 * - expose mapping uid-string with an iterator interface
 */
class ZooStringToUIDTable : public TObject
{
    private:
	/// assigned uids
	std::vector<unsigned> m_uids;
	/// assigned strings
	std::vector<std::string> m_strs;
	/// flag indicating if a hash collision has occurred
	bool m_hashcoll;

	/// the multiplier used in 32 bit FNV1a
	static const unsigned s_FNV1a32Multiplier = 16777619u;
	/// the offset used in 32 bit FNV1a
	static const unsigned s_FNV1a32Offset = 2166136261u;
	/// the empty string maps to UID 0
	static const std::string s_empty;

    public:
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

	/** @brief computes the hash value of a given string
	 *
	 * @param str	string for which to compute hash value
	 * @returns 	hash value
	 */
	static unsigned hash(const std::string& str);

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
	unsigned insert(const std::string& str)
	{ return insert(str, hash(str)); }

	/** @brief insert a string into the ZooStringToUIDTable
	 *
	 * @param str	string to insert
	 * @returns	UID assigned to the string
	 */
	unsigned insert(const char* str) { return insert(std::string(str)); }

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
	bool find(unsigned uid) const;

	/** @brief checks if UID exists
	 *
	 * @param uid	UID for which to test for existence
	 * @returns 	true or false, depending on whether uid exists
	 */
	bool exists(unsigned uid) const { return find(uid); }

	/** @brief lookup the string corresponding to a given UID
	 *
	 * @param uid	UID for which to find the string
	 * @returns	string mapped to UID uid
	 */
	const std::string& operator[](unsigned uid) const;

	/** @brief lookup the UID corresponding to a given string
	 *
	 * @param str	string for which to find the UID
	 * @returns	UID mapped to given string (or 0 if not found)
	 */
	unsigned operator[](const std::string& str) const;

	/** @brief lookup the UID corresponding to a given string
	 *
	 * @param str	string for which to find the UID
	 * @returns	UID mapped to given string (or 0 if not found)
	 */
	unsigned operator[](const char* str) const
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
	unsigned insert(const std::string& str, unsigned hash);

	ClassDef(ZooStringToUIDTable, 1);
};

/// output to cout and the like
std::ostream& operator<<(std::ostream& os, const ZooStringToUIDTable& tbl);

#endif // ZOOSTRINGTOUIDTABLE_H

// vim: sw=4:tw=78:ft=cpp
