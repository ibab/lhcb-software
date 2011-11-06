/** @file ZooStringValueBlock.cpp
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2011-11-06
 */

#include "ZooStringValueBlock.h"

ZooStringValueBlock::ZooStringValueBlock(const ZooStringToUIDTable& uidtbl) :
    m_uidtbl(const_cast<ZooStringToUIDTable*>(&uidtbl))
{ ZooKeyValueBlock::reserve(uidtbl.size()); }

ZooStringValueBlock::ZooStringValueBlock(const ZooStringToUIDTable& uidtbl,
	const StringValueVector& stringValueVect) :
    m_uidtbl(const_cast<ZooStringToUIDTable*>(&uidtbl))
{ insert(stringValueVect); }

ZooStringValueBlock::ZooStringValueBlock(const ZooStringToUIDTable& uidtbl,
	const KeyValueVector& UIDValueVect) :
    m_uidtbl(const_cast<ZooStringToUIDTable*>(&uidtbl))
{ insert(UIDValueVect); }

ZooStringValueBlock::~ZooStringValueBlock()
{ }

const ZooStringToUIDTable& ZooStringValueBlock::uidtbl() const
{
    const ZooStringToUIDTable* uidtbl =
	static_cast<const ZooStringToUIDTable*>(m_uidtbl.GetObject());
    if (!uidtbl) {
	throw ZooStringValueBlockException(
		"String to UID mapping table not available!");
    }
    return *uidtbl;
}

ZooStringValueBlock::rw_proxy ZooStringValueBlock::operator[](UInt_t uid)
{
    // this might potentially lead to an insert, so check against
    // ZooStringToUIDTable...
    if (!uidtbl().find(uid)) {
	throw ZooStringValueBlockException(
		"Trying to access unknown UID!");
    }
    return ZooKeyValueBlock::operator[](uid);
}

float ZooStringValueBlock::erase(UInt_t uid)
{
    if (!uidtbl().find(uid)) {
	throw ZooStringValueBlockException(
		"Trying to erase unknown UID!");
    }
    return ZooKeyValueBlock::erase(uid);
}

bool ZooStringValueBlock::find(const std::string& str) const
{
    const UInt_t uid = uidtbl()[str];
    if (!uid) return false;
    return ZooKeyValueBlock::find(uid);
}

float ZooStringValueBlock::value(const std::string& str) const
{
    const UInt_t uid = uidtbl()[str];
    if (!uid) return std::numeric_limits<float>::quiet_NaN();
    return ZooKeyValueBlock::operator[](uid);
}

ZooStringValueBlock::rw_proxy ZooStringValueBlock::operator[](
	const std::string& str)
{
    // this might potentially lead to an insert, so check against
    // ZooStringToUIDTable...
    const UInt_t uid = uidtbl()[str];
    if (!uid) {
	throw ZooStringValueBlockException(
		"Trying to access unknown string!");
    }
    return ZooKeyValueBlock::operator[](uid);
}

float ZooStringValueBlock::erase(const std::string& str)
{
    const UInt_t uid = uidtbl()[str];
    if (!uid) {
	throw ZooStringValueBlockException(
		"Trying to erase unknown string!");
    }
    return ZooKeyValueBlock::erase(uid);
}

float ZooStringValueBlock::insert(UInt_t uid, float value)
{
    if (!uidtbl().find(uid)) {
	throw ZooStringValueBlockException(
		"Trying to insert unknown UID!");
    }
    return ZooKeyValueBlock::insert(uid, value);
}

float ZooStringValueBlock::insert(const std::string& str, float value)
{
    const UInt_t uid = uidtbl()[str];
    if (!uid) {
	throw ZooStringValueBlockException(
		"Trying to insert unknown string!");
    }
    return ZooKeyValueBlock::insert(uid, value);
}

bool ZooStringValueBlock::modify(UInt_t uid, float value)
{
    if (!uidtbl().find(uid)) {
	throw ZooStringValueBlockException(
		"Trying to modify unknown UID!");
    }
    return ZooKeyValueBlock::modify(uid, value);
}

bool ZooStringValueBlock::modify(const std::string& str, float value)
{
    const UInt_t uid = uidtbl()[str];
    if (!uid) {
	throw ZooStringValueBlockException(
		"Trying to modify unknown string!");
    }
    return ZooKeyValueBlock::modify(uid, value);
}

void ZooStringValueBlock::insert(const StringValueVector& stringValueVect)
{ insert(stringValueVect.begin(), stringValueVect.end()); }

void ZooStringValueBlock::insert(const KeyValueVector& UIDValueVect)
{ insert(UIDValueVect.begin(), UIDValueVect.end()); }

void ZooStringValueBlock::dump(FILE* f) const
{
    for (ZooStringValueBlock::const_iterator it = begin(); end() != it; ++it) {
	ZooStringValueBlock::const_iterator::value_type kv = *it;
        std::fprintf(f, "%10u %s\t%f\n",
		kv.first.first, kv.first.second.c_str(), kv.second);
    }
}

void ZooStringValueBlock::dump(std::ostream& ostr) const
{
    char* buf = new char[64];
    std::size_t sz = 64;
    for (ZooStringValueBlock::const_iterator it = begin(); end() != it; ++it) {
	ZooStringValueBlock::const_iterator::value_type kv = *it;
        size_t wr = sz;
        do {
            wr = std::snprintf(buf, sz, "%10u %s\t%f\n",
		    kv.first.first, kv.first.second.c_str(), kv.second);
            if (wr >= sz) { // this is pure paranoia - buf is large enough
                delete[] buf;
                sz <<= 1;
                buf = new char[sz];
            }
        } while (wr >= sz);
        ostr << buf;
    }
    delete[] buf;
}

ClassImp(ZooStringValueBlock);

std::ostream& operator<<(std::ostream& os, const ZooStringValueBlock& zsv)
{
    zsv.dump(os);
    return os;
}

// vim: sw=4:tw=78:ft=cpp
