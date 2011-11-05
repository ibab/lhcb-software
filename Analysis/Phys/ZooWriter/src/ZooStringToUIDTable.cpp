/**
 * @file ZooStringToUIDTable.cpp
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2011-11-04
 *
 * class implementation of ZooStringToUIDTable
 */

#include <vector>
#include <string>
#include <algorithm>

#include <iostream>
#include <cstdio>

#include "ZooStringToUIDTable.h"

const std::string ZooStringToUIDTable::s_empty("");

unsigned ZooStringToUIDTable::hash(const std::string& str)
{
    unsigned hash = s_FNV1a32Offset;
    for (std::string::const_iterator it = str.begin();
	    str.end() != it; ++it) {
	hash ^= static_cast<unsigned>(*it);
	hash *= s_FNV1a32Multiplier;
    }
    unsigned length = str.size();
    // null strings hash to 0
    if (!length) return 0;
    // hash length as well
    do {
	hash ^= (length & 0xffu);
	hash *= s_FNV1a32Multiplier;
	length >>= 8;
    } while (length);
    // hash 0 is invalid, map to 1
    if (!hash) return 1;
    return hash;
}

bool ZooStringToUIDTable::find(unsigned uid) const
{
    return std::binary_search(m_uids.begin(), m_uids.end(), uid);
}

unsigned ZooStringToUIDTable::insert(const std::string& str, unsigned hash)
{
    if (!hash) ++hash;
    std::vector<unsigned>::iterator it =
	std::lower_bound(m_uids.begin(), m_uids.end(), hash);
    std::vector<std::string>::iterator jt =
	m_strs.begin() + unsigned(it - m_uids.begin());
    if (m_uids.end() != it && *it == hash) {
	// scan forward until we find a free uid
	bool exactmatch = false;
	while (m_uids.end() != it && !(exactmatch = (*jt == str))) {
	    m_hashcoll = true;
	    const unsigned lastuid = *it;
	    ++it;
	    ++jt;
	    if (m_uids.end() == it ||
		    (m_uids.end() != it && 1 != *it - lastuid)) {
		// found a free uid slot
		hash = lastuid + 1;
		if (!hash) {
		    // overflow in hash value, have to contrinue at the
		    // beginning of the array
		    ++hash;
		    it = m_uids.begin();
		    jt = m_strs.begin();
		    if (m_uids.end() == it ||
			    (m_uids.end() != it && *it != hash)) break;
		    continue;
		}
		break;
	    }
	}
	if (exactmatch) return hash;
    }
    m_uids.insert(it, hash);
    m_strs.insert(jt, str);
    return hash;
}

const std::string& ZooStringToUIDTable::operator[](unsigned uid) const
{
    std::vector<unsigned>::const_iterator it =
	std::lower_bound(m_uids.begin(), m_uids.end(), uid);
    // if we found the UID, return the corresponding string
    if (it != m_uids.end() && *it == uid)
	return *(m_strs.begin() + (it - m_uids.begin()));
    // non-existant UIDs map to the empty string
    return s_empty;
}

unsigned ZooStringToUIDTable::operator[](const std::string& str) const
{
    const unsigned uidguess = hash(str);
    std::vector<unsigned>::const_iterator it =
	std::lower_bound(m_uids.begin(), m_uids.end(), uidguess);
    // if we found the UID, return the corresponding string
    if (it != m_uids.end()) {
	std::vector<std::string>::const_iterator jt =
	    m_strs.begin() + (it - m_uids.begin());
	if (*it == uidguess) {
	    do {
		const unsigned lastuid = *it;
		// check if we found it
		if (str == *jt) return lastuid;
		++it;
		++jt;
		if (m_uids.end() == it) {
		    it = m_uids.begin();
		    jt = m_strs.begin();
		}
		if (1 != (*it - lastuid)) {
		    // the string is definitely not there
		    break;
		}
		// keep repeating the loop at most until we're through
		// a whole round through the array
	    } while (*it != uidguess);
	    // string is not there
	}
	// completely unknown string
    }
    // UID not found
    return 0;
}

void ZooStringToUIDTable::dump(std::ostream& ostr) const
{
    char* buf = new char[32];
    std::size_t sz = 32;
    for (unsigned idx = 0; idx < size(); ++idx) {
        size_t wr = sz;
        do {
            wr = std::snprintf(buf, sz, "uid %10u %s\n",
		    m_uids[idx], m_strs[idx].c_str());
            if (wr >= sz) {
		// reallocate buf if not large enough
                delete[] buf;
                sz <<= 1;
                buf = new char[sz];
            }
        } while (wr >= sz);
        ostr << buf;
    }
    delete[] buf;
}

void ZooStringToUIDTable::dump(FILE* f) const
{
    for (unsigned idx = 0; idx < size(); ++idx) {
	fprintf(f, "uid %10u %s\n", m_uids[idx], m_strs[idx].c_str());
    }
}

ClassImp(ZooStringToUIDTable);

std::ostream& operator<<(std::ostream& ostr, const ZooStringToUIDTable& tbl)
{
    tbl.dump(ostr);
    return ostr;
}

// vim: sw=4:tw=78:ft=cpp
