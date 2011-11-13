/** @file ZooLoKiBlock.cpp
 *
 * @author Paul Seyfert <pseyfert@physi.uni-heidelberg.de>
 * @date 2011-11-05
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2011-11-13
 */

#ifndef ZOOLOKIBLOCK_H
#define ZOOLOKIBLOCK_H

#include <vector>

#include "ZooStringValueBlock.h"

/** @brief info block to hold values of LoKi functors
 *
 * @author Paul Seyfert <pseyfert@physi.uni-heidelberg.de>
 * @date 2011-11-05
 */
class ZooLoKiBlock : public ZooStringValueBlock
{
    public:
	/// a const_iterator to loop over the contents of the ZooStringValueBlock
	typedef ZooStringValueBlock::const_iterator const_iterator;
	/// a key/uid-value pair
	typedef ZooStringValueBlock::KeyValuePair KeyValuePair;
	/// a vector of key/uid-value pairs
	typedef ZooStringValueBlock::KeyValueVector KeyValueVector;
	/// a string-value pair
	typedef ZooStringValueBlock::StringValuePair StringValuePair;
	/// a vector of string-value pairs
	typedef ZooStringValueBlock::StringValueVector StringValueVector;
	/// read-only proxy returned by non-const operator[]
	typedef ZooStringValueBlock::r_proxy r_proxy;
	/// read-write proxy returned by non-const operator[]
	typedef ZooStringValueBlock::rw_proxy rw_proxy;

	/// constructor
	ZooLoKiBlock(const ZooStringToUIDTable& uidtbl) :
	    ZooStringValueBlock(uidtbl) { }

	/// constructor from a vector of (string, value) pairs
	ZooLoKiBlock(const ZooStringToUIDTable& uidtbl,
		const StringValueVector& stringValueVect) :
	    ZooStringValueBlock(uidtbl, stringValueVect) { }

	/// constructor from a vector of (uid, value) pairs
	ZooLoKiBlock(const ZooStringToUIDTable& uidtbl,
		const KeyValueVector& UIDValueVect) :
	    ZooStringValueBlock(uidtbl, UIDValueVect) { }

	/// constructor from a range of std::map-style iterators
	template <class I> ZooLoKiBlock(
		const ZooStringToUIDTable& uidtbl,
		I begin, const I& end) :
	    ZooStringValueBlock(uidtbl, begin, end) { }

	/// destructor
	virtual ~ZooLoKiBlock();

	/// return a const reference to the string to uid mapping table
	const ZooStringToUIDTable& uidtbl() const
	{ return ZooStringValueBlock::uidtbl(); }

	/** @brief get UID for given string
	 *
	 * @param str	string for which to find the UID
	 * @returns	UID of string or 0 if no UID has been assigned
	 */
	UInt_t getUID(const std::string& str) const
	{ return ZooStringValueBlock::getUID(str); }

	/** @brief get String for given UID
	 *
	 * @param uid	UID for which to find the assigned string
	 * @returns	string assigned to UID or empty string (if UID unassigned)
	 */
	const std::string& getString(UInt_t uid) const
	{ return ZooStringValueBlock::getString(uid); }

	/// return a const_iterator to the beginning of the ZooStringValueBlock
	const_iterator begin() const { return ZooStringValueBlock::begin(); }

	/// return a const_iterator to the end of the ZooStringValueBlock
	const_iterator end() const { return ZooStringValueBlock::end(); }

	/// return true if the ZooStringValueBlock is empty, false otherwise
	bool empty() const { return ZooStringValueBlock::empty(); }

	/// return the number of entries in the ZooStringValueBlock
	unsigned size() const { return ZooStringValueBlock::size(); }

	/// return the capacity of the ZooStringValueBlock
	unsigned capacity() const { return ZooStringValueBlock::capacity(); }

	/// reserve a minimum capacity for this ZooStringValueBlock
	void reserve(unsigned capacity)
	{ return ZooStringValueBlock::reserve(capacity); }

	/// clear this ZooStringValueBlock
	void clear() { ZooStringValueBlock::clear(); }
	/// clear this ZooStringValueBlock
	virtual void Clear() { clear(); }

	/// check for the exsitence of a given UID
	bool find(UInt_t uid) const { return ZooStringValueBlock::find(uid); }

	/// check for the exsitence of a given UID
	bool exists(UInt_t uid) const { return find(uid); }

	/** @brief return the value assiciated to a given UID
	 *
	 * @param uid	UID for which to return the value
	 * @returns	value if UID exists, NaN otherwise
	 */
	float value(UInt_t uid) const
	{ return ZooStringValueBlock::operator[](uid); }

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
	rw_proxy operator[](UInt_t uid)
	{ return ZooStringValueBlock::operator[](uid); }

	/** @brief erase a UID and the corresponding value from the ZooStringValueBlock
	 *
	 * @param uid	UID to erase
	 * @returns	old value mapped to given UID (if any)
	 */
	float erase(UInt_t uid)
	{ return ZooStringValueBlock::erase(uid); }

	/// check for the exsitence of a given string
	bool find(const std::string& str) const
	{ return ZooStringValueBlock::find(str); }

	/// check for the exsitence of a given string
	bool exists(const std::string& str) const { return find(str); }

	/** @brief return the value assiciated to a given string
	 *
	 * @param str	string for which to return the value
	 * @returns	value if string exists, NaN otherwise
	 */
	float value(const std::string& str) const
	{ return ZooStringValueBlock::value(str); }

	/** @brief return the value assiciated to a given string
	 *
	 * @param str	string for which to return the value
	 * @returns	value if string exists, NaN otherwise
	 */
	float operator[](const std::string& str) const
	{ return value(str); }

	/** @brief return or set the value assiciated to a given string
	 *
	 * @param str	string for which to return or set the value
	 * @returns	a proxy which can serve as an lvalue for the value
	 */
	rw_proxy operator[](const std::string& str)
	{ return ZooStringValueBlock::operator[](str); }

	/** @brief erase a string and the corresponding value from the ZooStringValueBlock
	 *
	 * @param str	string to erase
	 * @returns	old value mapped to given string (if any)
	 */
	float erase(const std::string& str)
	{ return ZooStringValueBlock::erase(str); } 

	/** @brief erase element pointed to by given iterator
	 *
	 * @param it	iterator pointing to element to erase
	 * @returns	iterator pointing to element following erased element
	 */
	const_iterator erase(const const_iterator& it)
	{ return ZooStringValueBlock::erase(it); } 

	/** @brief insert a value for a given uid
	 *
	 * @param uid	UID for which to insert a value
	 * @param value	value to insert
	 * @returns	old value saved at that UID (if any)
	 */
	float insert(UInt_t uid, float value)
	{ return ZooStringValueBlock::insert(uid, value); } 

	/** @brief insert a value for a given string
	 *
	 * @param str	string for which to insert a value
	 * @param value	value to insert
	 * @returns	old value saved at that UID (if any)
	 */
	float insert(const std::string& str, float value)
	{ return ZooStringValueBlock::insert(str, value); } 

	/** @brief modify a value for a given uid
	 *
	 * @param uid	UID for which to modify a value
	 * @param value	value to modify
	 * @returns	true if UID exists, false if it had to be inserted
	 */
	bool modify(UInt_t uid, float value)
	{ return ZooStringValueBlock::modify(uid, value); } 

	/** @brief modify a value for a given string
	 *
	 * @param str	string for which to modify a value
	 * @param value	value to modify
	 * @returns	true if string exists, false if it had to be inserted
	 */
	bool modify(const std::string& str, float value)
	{ return ZooStringValueBlock::modify(str, value); } 

	/** @brief insert elements from a vector of (string, value) pairs
	 *
	 * @param stringValueVect	vector with (string, value) pairs
	 * @returns			nothing
	 */
	void insert(const StringValueVector& stringValueVect)
	{ return ZooStringValueBlock::insert(stringValueVect); } 

	/** @brief insert elements from a vector of (UID, value) pairs
	 *
	 * @param UIDValueVect	vector with (UID, value) pairs
	 * @returns		nothing
	 */
	void insert(const KeyValueVector& UIDValueVect)
	{ return ZooStringValueBlock::insert(UIDValueVect); } 

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
	{ return ZooStringValueBlock::insert(begin, end); } 

	/** @brief dump the contents of this ZooStringValueBlock to an ostream
	 *
	 * @param ostr	ostream to dump to
	 * @returns	nothing
	 */
	void dump(std::ostream& ostr = std::cout) const
	{ return ZooStringValueBlock::dump(ostr); } 

	/** @brief dump the contents of this ZooStringValueBlock to a FILE
	 *
	 * @param f	FILE to dump to
	 * @returns	nothing
	 */
	void dump(FILE* f) const
	{ return ZooStringValueBlock::dump(f); } 

	// default constructor (for ROOT I/O)
	ZooLoKiBlock() { }
    private:
	/// forbidden
	ZooLoKiBlock(const ZooStringValueBlock&);
	/// forbidden
	const ZooLoKiBlock& operator=(const ZooStringValueBlock&);

	ClassDef(ZooLoKiBlock, 1);
};

inline std::ostream& operator<<(std::ostream& ostr, const ZooLoKiBlock& zloki)
{ return operator<<(ostr, reinterpret_cast<const ZooStringValueBlock&>(zloki)); }

#endif /* ZOOLOKIBLOCK_H */

// vim: tw=78:sw=4:ft=cpp
