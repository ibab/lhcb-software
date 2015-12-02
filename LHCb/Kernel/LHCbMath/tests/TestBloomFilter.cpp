/** @file TestBloomFilter.cpp
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2014-08-15
 *
 * some very dumb and stupid unit test follows - compile it NDEBUG undefined!
 */

#include "LHCbMath/BloomFilter.h"

#include <iostream>
#include <cstdlib>

// forward decl.
static void _domyassert(const char* cond, const char* fnname,
	const char* file, unsigned line) __attribute__((noreturn));

/// give a meaningful message before aborting
static void _domyassert(const char* cond, const char* fnname,
	const char* file, unsigned line)
{
    std::cerr << "Assertion failed: " << cond << " (in " << fnname << ", " <<
	file << ", line " << line << ")" << std::endl;
    std::abort();
}

/// our own little assert macro (independent of NDEBUG)
#define myassert(cond) \
    do { \
	if (!(cond)) _domyassert(#cond, __func__, __FILE__, __LINE__); \
    } while (false)

int main()
{
    // check FNV1a implementations (32 and 64 bits)
    BloomFilterImpl::HashFNV1a<const char*, uint32_t> cstrhash32;
    BloomFilterImpl::HashFNV1a<std::string, uint32_t> strhash32;
    BloomFilterImpl::HashFNV1a<const char*, uint64_t> cstrhash64;
    BloomFilterImpl::HashFNV1a<std::string, uint64_t> strhash64;
    myassert(cstrhash32("chongo was here!") == 0x448524fdul);
    myassert(strhash32(std::string("chongo was here!")) == 0x448524fdul);
    myassert(cstrhash64("chongo was here!") == 0x858e2fa32a55e61dull);
    myassert(strhash64(std::string("chongo was here!")) == 0x858e2fa32a55e61dull);
    // all the rest is checking Bloom filter code
    typedef BloomFilter<int, 24, 53687091, 1 << 30> BF;
    BF filter;
    std::cout << "Bloom filter has "<< BF::nBits << " bits, " << BF::nHashes
	<< " hash functions, capacity " << BF::capacity << " for false "
	"positive rate " << BF::pFalsePositive() << " (target rate " <<
	BF::pFalsePositiveTarget() << ")" << std::endl;
    double rr = 0., rr2 = 0., s = 0., s2 = 0.;
    for (int k = 0; k < 100; ++k) {
	filter.clear();
	for (int i = 0; i < 24; ++i)
	    filter.insert(i + k * 24);
	for (int i = 0; i < 24; ++i)
	    myassert(filter.find(i + k * 24));
	double r = 0., r2 = 0.;
	for (unsigned j = 0; j < 100; ++j) {
	    int found = 0;
	    for (int i = 0; i < 1000; ++i) {
		int tmp;
		do { tmp = lrand48(); } while (tmp < 4096);
		if (filter.find(tmp)) ++found;
	    }
	    r += 1e-3 * found;
	    r2 += 1e-6 * found * found;
	}
	r *= 1e-2;
	r2 *= 1e-2;
	std::cout << "Test " << k << ": Estimated size of set: " <<
	    filter.size() << ", false positive rate " << r << " +/- " <<
	    std::sqrt(r2 - r * r) << std::endl;
	rr += r;
	rr2 += r * r;
	s += filter.size();
	s2 += filter.size() * filter.size();
	myassert(std::abs(ssize_t(filter.size()) - 24) < 5. * std::sqrt(24.));
	myassert(std::abs(r - BF::pFalsePositive()) < 5. / 64.);
	// test the getword/setword methods by using it to make a copy,
	// testing the comparison operators while we're at it
	BF copy;
	copy.setword(1, filter.getword(1));
	copy.setword(2, filter.getword(2));
	copy.setword(3, filter.getword(3));
	copy.setword(4, filter.getword(4));
	myassert(filter != copy); // incomplete copy
	copy.setword(0, filter.getword(0));
	myassert(filter == copy); // complete copy
    }
    rr *= 1e-2;
    rr2 *= 1e-2;
    s *= 1e-2;
    s2 *= 1e-2;
    std::cout << "Test-averaged size estimate " << s << " +/- " <<
	std::sqrt(s2 - s * s) << std::endl;

    std::cout << "Test-averaged false positive rate " << rr << " +/- " <<
	std::sqrt(rr2 - rr * rr) << std::endl;
    myassert(std::sqrt(s2 - s * s) < 5. * std::sqrt(24. / 100.));
    myassert(std::abs(s - 24.) < s2);
    myassert(std::sqrt(rr2 - rr * rr) < 5. * 1. / 64. / std::sqrt(12.));
    myassert(std::abs(rr - BF::pFalsePositive()) < 5. * std::sqrt(rr2 - rr * rr));
    std::cout << "If you see this printout, the BloomFilter implementation "
	"is likely okay." << std::endl;
    return 0;
}

// vim: tw=78:sw=4:ft=cpp
