/* @file ZooKeyValueBlock.cpp
 *
 * @author Paul Seyfert <pseyfert@physi.uni-heidelberg.de>
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2011-09-29
 */

#include <algorithm>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include "ZooKeyValueBlock.h"

ZooKeyValueBlock::~ZooKeyValueBlock() { }

bool ZooKeyValueBlock::exists(unsigned key) const
{ return std::binary_search(m_idx.begin(), m_idx.end(), key); }

float ZooKeyValueBlock::value(unsigned key) const
{
    KeyVector::const_iterator it = std::find(m_idx.begin(), m_idx.end(), key);
    if (it != m_idx.end()) return m_val[unsigned(it - m_idx.begin())];
    return std::numeric_limits<float>::quiet_NaN();
}

ZooKeyValueBlock::ZooKeyValueBlock(KeyValueVector& extrainfo)
{
    using namespace boost::lambda;
    m_idx.reserve(extrainfo.size());
    m_val.reserve(extrainfo.size());
    // the next two lines break with boost 1.44 so we have to specify
    // the return type of bind explicitely
    std::sort(extrainfo.begin(), extrainfo.end(),
	    bind<unsigned>(&KeyValuePair::first, _1) < bind<unsigned>(
		&KeyValuePair::first, _2));
    for (std::size_t i = 0; i < extrainfo.size(); ++i) {
	// skip duplicate keys
	if (i && extrainfo[i - 1].first == extrainfo[i].first)
	    continue;
	m_idx.push_back(extrainfo[i].first);
	m_val.push_back(extrainfo[i].second);
    }
}

float& ZooKeyValueBlock::opIndImpl(unsigned key, float value)
{
    KeyVector::iterator it = std::lower_bound(m_idx.begin(), m_idx.end(), key);
    ValueVector::iterator jt = m_val.begin() + unsigned(it - m_idx.begin());
    if (it != m_idx.end() && *it == key) {
	// return value somewhere in middle of array
	return *jt = value;
    }
    // found place to insert, but new value
    m_idx.insert(it, key);
    jt = m_val.insert(jt, value);
    return *jt;
}

float ZooKeyValueBlock::insert(unsigned key, float val)
{
    float retVal = std::numeric_limits<float>::quiet_NaN();
    KeyVector::iterator it = std::lower_bound(m_idx.begin(), m_idx.end(), key);
    ValueVector::iterator jt = m_val.begin() + unsigned(it - m_idx.begin());
    if (it != m_idx.end() && *it == key) {
	// replace somewhere in middle of arrays
	retVal = *jt;
	*jt = val;
    } else {
	// found place to insert, but new value
	m_idx.insert(it, key);
	m_val.insert(jt, val);
    }
    return retVal;
}

float ZooKeyValueBlock::erase(unsigned key)
{
    KeyVector::iterator it = std::find(m_idx.begin(),
	    m_idx.end(), key);
    if (m_idx.end() == it) return std::numeric_limits<float>::quiet_NaN();
    ValueVector::iterator jt = m_val.begin() + unsigned(it - m_idx.begin());
    float retVal = *jt;
    m_idx.erase(it);
    m_val.erase(jt);
    return retVal;
}

bool ZooKeyValueBlock::modify(unsigned key, float val)
{
    // find index of key in array
    KeyVector::iterator it =
	std::find(m_idx.begin(), m_idx.end(), key);
    if (it != m_idx.end()) {
	// modify corresponding value
	ValueVector::iterator jt =
	    m_val.begin() + unsigned(it - m_idx.begin());
	*jt = val;
	return true;
    }
    return false;
}

void ZooKeyValueBlock::dump(FILE* f) const
{
    for (const_iterator it = begin(); end() != it; ++it) {
	const_iterator::value_type kv = *it;
	std::fprintf(f, "%u\t%f\n", kv.first, kv.second);
    }
}

void ZooKeyValueBlock::dump(std::ostream& ostr) const
{
    char* buf = new char[32];
    std::size_t sz = 32;
    for (const_iterator it = begin(); end() != it; ++it) {
	const_iterator::value_type kv = *it;
	size_t wr = sz;
	do {
	    wr = std::snprintf(buf, sz, "%u\t%f\n", kv.first, kv.second);
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

ClassImp(ZooKeyValueBlock);

std::ostream& operator<<(std::ostream& os, const ZooKeyValueBlock& zkv)
{
    zkv.dump(os);
    return os;
}

// vim: tw=78:sw=4:ft=cpp
