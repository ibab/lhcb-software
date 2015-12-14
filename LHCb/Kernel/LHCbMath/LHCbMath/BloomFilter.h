/** @file BloomFilter.h
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2014-08-15
 */

#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H 1

#include <cmath>
#include <bitset>
#include <limits>
#include <string>
#include <cstdint>
#include <numeric>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

#undef _nBits
#undef _nNashes
#undef _canUseVeryFastHash
/// implementation details for BloomFilter
namespace BloomFilterImpl {
#if !defined(__INTEL_COMPILER) && !defined(__clang__)
    // GCC and other compilers that implement decent C++11 support just use
    // std::log....
    using std::log;
#elif defined(__clang__) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >=1400)
    // ICC (the Intel C++ compiler) and Clang currently need a workarounds,
    // because they cannot deal with a constexpr std::log...
#warning "activating constexpr workarounds for std::log for Intel or clang compilers"
#if defined(__clang__) && (__clang_major__ < 3 || \
	(__clang_major__ == 3 && __clang_minor__ <=3))
    static constexpr double abs(const double x) { return (x < 0.) ? -x : x; }
#else
    using std::abs;
#endif
    /// for tiny x - 1, use a Taylor expansion
    static constexpr double log2_tiny(const double xm1)
    {
	return (((((((((((((((((((-xm1 / 20. + 1. / 19.) * xm1 - 1. / 18.) *
		xm1 + 1. / 17.) * xm1 - 1. / 16.) * xm1 + 1. / 15.) *
		xm1 - 1. / 14.) * xm1 + 1. / 13.) * xm1 - 1. / 12.) *
		xm1 + 1. / 11.) * xm1 - 1. / 10.) * xm1 + 1. / 9.) *
		xm1 - 1. / 8.) * xm1 + 1. / 7.) * xm1 - 1. / 6.) *
		xm1 + 1. / 5.) * xm1 - 1. / 4.) * xm1 + 1. / 3.) *
		xm1 - 1. / 2.) * xm1 + 1. / 1.) * xm1 / 0.69314718055995;
    }

    /// for slightly larger x, use a Padé approximant
    static constexpr double log2_small(const double x)
    {
	return (abs(x - 1.) < 0.18) ?  log2_tiny(x - 1.) :
	    ((((((((((6.9334366458438446E+10 * x + 3.3811243166750752E+12) * x +
		     3.5283588930136203E+13) * x + 1.1756441702477491E+14) * x +
		   1.133968217106985E+14) * x - 5.7135488589275375E+13) * x -
		 1.3605784210515297E+14) * x - 6.4737928386652203E+13) * x -
	       1.1115029743498373E+13) * x - 6.4096316616289868E+11) * x -
	     8.0343580013031902E+9) / ((((((((((1.23046875E+10 * x + 9.84375E+11) *
						 x + 1.5946875E+13) * x +
					     9.072E+13) * x + 2.22264E+14) * x +
				     2.56048128E+14) * x + 1.4224896E+14) * x +
			     3.7158912E+13) * x + 4.1803776E+12) * x +
		     1.6515072E+11) * x + 1.32120576E+9) / 0.69314718055995;
    }

    /// log2 for finite arguments
    static constexpr double log2_finite(const double x)
    {
	return (abs(x - 1.) <= 0.5) ?
	    log2_small(x) :
	    ((x > 1.) ? (1. + log2_finite(0.5 * x)) :
	     (-1. + log2_finite(2.0 * x)));
    }

    /** @brief log2 for any argument (computable at compile time, error better than
     * about 1e-15 relative)
     */
    static constexpr double log2(const double x)
    {
	return (x < 0 || x != x) ? std::numeric_limits<double>::quiet_NaN() :
	    ((x > std::numeric_limits<double>::max()) ?
	     std::numeric_limits<double>::infinity() :
	     ((x == 0.) ? -std::numeric_limits<double>::infinity() :
	      log2_finite(x)));
    }

    /// log implementation (computable at compile time)
    static constexpr double log(const double x)
    { return 0.69314718055995 * log2(x); }
#endif

#if !defined(__INTEL_COMPILER) || __INTEL_COMPILER >= 1400
    // nice and simple code for gcc, clang, and newer versions of the Intel compiler

    /** @brief compute length of Bloom filter in bits
     *
     * @param CAPACITY	appox. number of elements
     * @param PNUMER	numerator of false positive probability
     * @param PDENOM	denominator of false positive probability
     *
     * @author Manuel Schiller <manuel.schiller@cern.ch>
     * @date 2014-09-10
     */
    static constexpr unsigned _nBits(const unsigned capacity,
	    const unsigned pnumer, const unsigned pdenom)
    { return -double(capacity) * (log(pnumer) - log(pdenom)) / (log(2.) * log(2.)); }

    /** @brief compute number of hash functions for Bloom filter
     *
     * @param PNUMER	numerator of false positive probability
     * @param PDENOM	denominator of false positive probability
     *
     * @author Manuel Schiller <manuel.schiller@cern.ch>
     * @date 2014-09-10
     */
    static constexpr unsigned _nHashes(const unsigned pnumer, const unsigned pdenom)
    { return -(log(pnumer) - log(pdenom)) / log(2.); }

    /** @brief decide if very fast hashing can be used
     *
     * @author Manuel Schiller <manuel.schiller@cern.ch>
     * @date 2014-09-10
     *
     * If NBITS ^ NHASHES <= MAX, we can get our NHASHES hashes from a single
     * hash function invocation by extracting base NBITS digits from the hash
     * value...
     */
    static constexpr bool _canUseVeryFastHash(uint64_t MAX,
	    uint64_t NBITS, unsigned NHASHES)
    { return (0 == NHASHES) ? (0 != MAX) :
	    _canUseVeryFastHash(MAX / NBITS, NBITS, NHASHES - 1); }

    // the abomination pays us a visit...
#define _nBits(capacity, pnumer, pdenom) \
    BloomFilterImpl::_nBits(capacity, pnumer, pdenom)

    // the abomination pays us a visit...
#define _nHashes(pnumer, pdenom) \
    BloomFilterImpl::_nHashes(pnumer, pdenom)

    // the abomination pays us a visit...
#define _canUseVeryFastHash(max, nbits, nhashes) \
    BloomFilterImpl::_canUseVeryFastHash(max, nbits, nhashes)

#else
    // old versions of the Intel compiler s..., no, are not much fun to work
    // with...
#warning "Intel compilers before 14.x are really difficult to support"
    using std::log;
    /// implementation details for template metaprogram log2
    namespace log2impl {
	/// integer part of log2(_N) (rounding down to next integer)
	template <unsigned _N> struct ilog2
	{ enum { value = 1 + ilog2<_N / 2>::value }; };
	/// specialise for _N == 1
	template <> struct ilog2<1>
	{ enum { value = 0 }; };
	/// specialise for _N == 0 (absence of value enum will cause error)
	template <> struct ilog2<0> {};

	/// integer part of log2(_N) (rounding up to next integer)
	template <unsigned _N> struct ilog2up
	{ enum { value = ilog2<_N>::value + (_N != (1u << ilog2<_N>::value)) }; };

	/// fractional part of log2(1-_N/64) (Taylor series)
	template<unsigned _N> struct flog2
	{
	    enum { value =
		((1u << (4 * 6)) * _N) / 1u -
		    ((1u << (3 * 6)) * _N * _N) / 2u +
		    ((1u << (2 * 6)) * _N * _N * _N) / 3u -
		    ((1u << (1 * 6)) * _N * _N * _N * _N) / 4u +
		    ((1u << (0 * 6)) * _N * _N * _N * _N * _N) / 5u
	    };
	};
    }

    /** @brief compute log2(N/D) in fixed point as template metaprogram (to
     * about 5%)
     *
     * @tparam N numerator
     * @tparam D denominator
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2014-08-15
     */
    template<unsigned N, unsigned D> struct log2
    {
	public:
	    /// denominator of fractional part of log2(N/D)
	    enum { denom = 594482189 };
	    /// numerator of fractional part of log2(N/D)
	    enum { numer = int(denom -
		    ((N != (1u << log2impl::ilog2<N>::value)) *
		     log2impl::flog2<(64u *
			 ((1u << log2impl::ilog2up<N>::value) - N)) /
		     (1u << log2impl::ilog2up<N>::value)>::value -
		     (D != (1 << log2impl::ilog2<D>::value)) *
		     log2impl::flog2<(64u *
			 ((1u << log2impl::ilog2up<D>::value) - D)) /
		     (1u << log2impl::ilog2up<D>::value)>::value))
	    };
	    /// integer part of log2(N/D)
	    enum { integer = int(log2impl::ilog2up<N>::value -
		log2impl::ilog2up<D>::value - 1) };

	    /// return result as double
	    constexpr static double value()
	    { return double(integer) + double(numer) / double(denom); }
    };

    /** @brief template metaprogram to compute length of Bloom filter in bits
     *
     * @tparam N	appox. number of elements
     * @tparam PNUMER	numerator of false positive probability
     * @tparam PDENOM	denominator of false positive probability
     *
     * @note Since the implementation of log2 as template metaprogram isn't
     * perfect in terms of accuracy, the best results are achieved with
     * PNUMER = 1 and PDENOM = 1 << k.
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2014-08-15
     */
    template<unsigned N, unsigned PNUMER, unsigned PDENOM> struct nbits {
	static_assert(PNUMER < PDENOM, "Probability >= 1, error.");
	enum { value = int(
		((-int64_t(N) *
		  (int64_t(log2<PNUMER, PDENOM>::denom) *
		   int64_t(log2<PNUMER, PDENOM>::integer) +
		   int64_t(log2<PNUMER, PDENOM>::numer))) /
		 int64_t(412063653)))
	};
    };

    /** @brief template metaprogram to compute number of hash functions for
     * Bloom filter
     *
     * @tparam PNUMER	numerator of false positive probability
     * @tparam PDENOM	denominator of false positive probability
     *
     * @note Since the implementation of log2 as template metaprogram isn't
     * perfect in terms of accuracy, the best results are achieved with
     * PNUMER = 1 and PDENOM = 1 << k.
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2014-08-15
     */
    template<unsigned PNUMER, unsigned PDENOM> struct nhashes {
	static_assert(PNUMER < PDENOM, "Probability >= 1, error.");
	enum { value = int(-(int64_t(log2<PNUMER, PDENOM>::denom) *
		    int64_t(log2<PNUMER, PDENOM>::integer) +
		    int64_t(log2<PNUMER, PDENOM>::numer)) /
		int64_t(594482189u))
	};
    };

    /** @brief decide if very fast hashing can be used
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2014-08-15
     *
     * If NBITS ^ NHASHES <= MAX, we can get our NHASHES hashes from a single
     * hash function invocation by extracting base NBITS digits from the hash
     * value...
     */
    template<uint64_t MAX, uint64_t NBITS, unsigned NHASHES>
    struct canUseVeryFastHash {
	enum { value =
	    canUseVeryFastHash<MAX / NBITS, NBITS, NHASHES - 1>::value
	};
    };
    /// specialisation to handle termination of recursion
    template<uint64_t MAX, uint64_t NBITS>
    struct canUseVeryFastHash<MAX, NBITS, 0> {
	enum { value = MAX != 0 };
    };

    // the abomination pays us a visit...
#define _nBits(capacity, pnumer, pdenom) \
    BloomFilterImpl::nbits<capacity, pnumer, pdenom>::value

    // the abomination pays us a visit...
#define _nHashes(pnumer, pdenom) \
    BloomFilterImpl::nhashes<pnumer, pdenom>::value

    // the abomination pays us a visit...
#define _canUseVeryFastHash(max, nbits, nhashes) \
    BloomFilterImpl::canUseVeryFastHash<max, nbits, nhashes>::value

#endif

    /// very dumb FNV1a implementation
    template<class T> struct __doFNV1a
    { /** empty, resolve template only for specialised known types */};

    /** @brief very dumb 32-bit FNV1a hash implementation
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2014-08-15
     *
     * See http://www.isthe.com/chongo/tech/comp/fnv/ for details.
     */
    template<> struct __doFNV1a<uint32_t> {
	static const uint32_t hashinit = 2166136261u;
	static constexpr uint32_t doByte(uint32_t hash, uint8_t val)
	{ return (hash ^ val) * 16777619u; }
	uint32_t operator()(const unsigned char* begin,
		const unsigned char* end, uint32_t hash = hashinit) const
	{ return std::accumulate(begin, end, hash, doByte); }
	constexpr uint32_t operator()(uint8_t val, uint32_t hash = hashinit) const
	{ return doByte(hash, val); }
	constexpr uint32_t operator()(uint16_t val, uint32_t hash = hashinit) const
	{ return operator()(uint8_t(val >> 8), operator()(uint8_t(val), hash)); }
	constexpr uint32_t operator()(uint32_t val, uint32_t hash = hashinit) const
	{ return operator()(uint16_t(val >> 16), operator()(uint16_t(val), hash)); }
	constexpr uint32_t operator()(uint64_t val, uint32_t hash = hashinit) const
	{ return operator()(uint32_t(val >> 32), operator()(uint32_t(val), hash)); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T*) == sizeof(uint32_t), uint32_t>::type
        operator()(const T* ptr, uint32_t hash = hashinit) const
	{ return operator()(uint32_t(ptr), hash); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T*) == sizeof(uint64_t), uint32_t>::type
        operator()(const T* ptr, uint32_t hash = hashinit) const
	{ return operator()(uint64_t(ptr), hash); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T) == sizeof(uint8_t), uint32_t>::type
        operator()(const T val, uint32_t hash = hashinit) const
	{ return operator()(uint8_t(val), hash); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T) == sizeof(uint16_t), uint32_t>::type
        operator()(const T val, uint32_t hash = hashinit) const
	{ return operator()(uint16_t(val), hash); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T) == sizeof(uint32_t), uint32_t>::type
        operator()(const T val, uint32_t hash = hashinit) const
	{ return operator()(uint32_t(val), hash); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T) == sizeof(uint64_t), uint32_t>::type
        operator()(const T val, uint32_t hash = hashinit) const
	{ return operator()(uint64_t(val), hash); }
    };

    /** @brief very dumb 64-bit FNV1a hash implementation
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2014-08-15
     *
     * See http://www.isthe.com/chongo/tech/comp/fnv/ for details.
     */
    template<> struct __doFNV1a<uint64_t> {
	static const uint64_t hashinit = 14695981039346656037ull;
	static constexpr uint64_t doByte(uint64_t hash, uint8_t val)
	{ return (hash ^ val) * 1099511628211ull; }
	uint64_t operator()(const unsigned char* begin,
		const unsigned char* end, uint64_t hash = hashinit) const
	{ return std::accumulate(begin, end, hash, doByte); }
	constexpr uint64_t operator()(uint8_t val, uint64_t hash = hashinit) const
	{ return doByte(hash, val); }
	constexpr uint64_t operator()(uint16_t val, uint64_t hash = hashinit) const
	{ return operator()(uint8_t(val >> 8), operator()(uint8_t(val), hash)); }
	constexpr uint64_t operator()(uint32_t val, uint64_t hash = hashinit) const
	{ return operator()(uint16_t(val >> 16), operator()(uint16_t(val), hash)); }
	constexpr uint64_t operator()(uint64_t val, uint64_t hash = hashinit) const
	{ return operator()(uint32_t(val >> 32), operator()(uint32_t(val), hash)); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T*) == sizeof(uint32_t), uint64_t>::type
        operator()(const T* ptr, uint64_t hash = hashinit) const
	{ return operator()(uint32_t(ptr), hash); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T*) == sizeof(uint64_t), uint64_t>::type
        operator()(const T* ptr, uint64_t hash = hashinit) const
	{ return operator()(uint64_t(ptr), hash); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T) == sizeof(uint8_t), uint64_t>::type
        operator()(const T val, uint64_t hash = hashinit) const
	{ return operator()(uint8_t(val), hash); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T) == sizeof(uint16_t), uint64_t>::type
        operator()(const T val, uint64_t hash = hashinit) const
	{ return operator()(uint16_t(val), hash); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T) == sizeof(uint32_t), uint64_t>::type
        operator()(const T val, uint64_t hash = hashinit) const
	{ return operator()(uint32_t(val), hash); }
	template<class T>
	constexpr typename std::enable_if<sizeof(T) == sizeof(uint64_t), uint64_t>::type
        operator()(const T val, uint64_t hash = hashinit) const
	{ return operator()(uint64_t(val), hash); }
    };

    /** @brief very dumb 32/64-bit FNV1a hash implementation
     *
     * @author Manuel Schiller <manuel.schiller@nikhef.nl>
     * @date 2014-08-15
     *
     * @note this is the implementation for generic type; specialisations for
     * some types can be found below
     *
     * See http://www.isthe.com/chongo/tech/comp/fnv/ for details.
     */
    template<class T, class HASHTYPE = uint64_t> struct HashFNV1a {
	/// type of hash value
	typedef HASHTYPE result_type;
	/// type of argument
	typedef T argument_type;
	/// return FNV1a hash of val
	constexpr result_type operator()(const T& val,
		const result_type hash = __doFNV1a<result_type>::hashinit) const
	{
	    return __doFNV1a<result_type>()(
		    reinterpret_cast<const unsigned char*>(&val),
		    reinterpret_cast<const unsigned char*>(&val) + sizeof(T),
		    hash);
	}
    };
    /// specialise HashFNV1a for basic types
#define BLOOMFILTER_DOHASHOVERLOAD(typearg) \
    template<> struct HashFNV1a<typearg, uint32_t> { \
	typedef uint32_t result_type; \
	typedef typearg argument_type; \
	result_type operator()(const typearg val, const \
		result_type hash = __doFNV1a<result_type>::hashinit ) const \
	{ return __doFNV1a<result_type>()(val, hash); } \
    }; \
    template<> struct HashFNV1a<typearg, uint64_t> { \
	typedef uint64_t result_type; \
	typedef typearg argument_type; \
	constexpr result_type operator()(const typearg val, \
		const result_type hash = __doFNV1a<result_type>::hashinit ) const \
	{ return __doFNV1a<result_type>()(val, hash); } \
    }
    BLOOMFILTER_DOHASHOVERLOAD(bool);
    BLOOMFILTER_DOHASHOVERLOAD(char);
    BLOOMFILTER_DOHASHOVERLOAD(wchar_t);
    BLOOMFILTER_DOHASHOVERLOAD(char16_t);
    BLOOMFILTER_DOHASHOVERLOAD(char32_t);
    BLOOMFILTER_DOHASHOVERLOAD(short);
    BLOOMFILTER_DOHASHOVERLOAD(int);
    BLOOMFILTER_DOHASHOVERLOAD(long int);
    BLOOMFILTER_DOHASHOVERLOAD(long long int);
    BLOOMFILTER_DOHASHOVERLOAD(signed char);
    BLOOMFILTER_DOHASHOVERLOAD(unsigned char);
    BLOOMFILTER_DOHASHOVERLOAD(unsigned short);
    BLOOMFILTER_DOHASHOVERLOAD(unsigned int);
    BLOOMFILTER_DOHASHOVERLOAD(unsigned long int);
    BLOOMFILTER_DOHASHOVERLOAD(unsigned long long int);
    /// specialise for the various types of zero-terminated C-style strings
#define BLOOMFILTER_DOHASHOVERLOAD_CSTRING(CTYPE, HASHTYPE) \
    template<> struct HashFNV1a<const CTYPE*, HASHTYPE> { \
	typedef HASHTYPE result_type; \
	typedef const CTYPE* argument_type; \
	result_type operator()(argument_type str, \
		result_type hash = __doFNV1a<result_type>::hashinit) const \
	{ \
	    for ( ; *str; ++str) hash = __doFNV1a<result_type>()(*str, hash); \
	    return hash; \
	} \
    }
    BLOOMFILTER_DOHASHOVERLOAD_CSTRING(char, uint32_t);
    BLOOMFILTER_DOHASHOVERLOAD_CSTRING(char, uint64_t);
    BLOOMFILTER_DOHASHOVERLOAD_CSTRING(wchar_t, uint32_t);
    BLOOMFILTER_DOHASHOVERLOAD_CSTRING(wchar_t, uint64_t);
    BLOOMFILTER_DOHASHOVERLOAD_CSTRING(char16_t, uint32_t);
    BLOOMFILTER_DOHASHOVERLOAD_CSTRING(char16_t, uint64_t);
    BLOOMFILTER_DOHASHOVERLOAD_CSTRING(char32_t, uint32_t);
    BLOOMFILTER_DOHASHOVERLOAD_CSTRING(char32_t, uint64_t);
    /** @brief specialise HashFNV1a for pointer types
     *
     * @note assumption is that you want to hash the raw pointer unless it's a
     * zero-terminated C-style string (char*, wchar_t*, char16_t, char32_t)
     */
#define BLOOMFILTER_DOHASHOVERLOAD_POINTER(HASHTYPE) \
    template<class T> struct HashFNV1a<const T*, HASHTYPE> { \
	typedef HASHTYPE result_type; \
	typedef const T* argument_type; \
	result_type operator()(argument_type val, \
		      result_type hash = __doFNV1a<result_type>::hashinit ) const \
	{ return __doFNV1a<result_type>()( \
		reinterpret_cast<const void*>(val), hash); } \
    }
    BLOOMFILTER_DOHASHOVERLOAD_POINTER(uint32_t);
    BLOOMFILTER_DOHASHOVERLOAD_POINTER(uint64_t);
    /// specialise for C++-style strings
    template<class CharT, class Traits, class Allocator, class HASHTYPE>
    struct HashFNV1a<std::basic_string<CharT, Traits, Allocator>, HASHTYPE> {
	typedef HASHTYPE result_type;
	typedef std::basic_string<CharT, Traits, Allocator> argument_type;
	result_type operator()(const argument_type& str,
		result_type hash = __doFNV1a<result_type>::hashinit) const
	{
	    return std::accumulate(std::begin(str), std::end(str), hash, 
		    [](result_type h, CharT c ) {
		        return __doFNV1a<result_type>()(c, h);
		    });
	}
    };
}

/// try to save a bit of time by playing tricks with the hash function
#define BLOOMFILTER_FASTHASH 1

/** @brief BloomFilter is a probabilistic set of objects
 *
 * @author Manuel Schiller <manuel.schiller@nikhef.nl>
 * @date 2014-08-15
 *
 * A Bloom fiter is a probablistic data structure that can be used to model a
 * set of elements. Elements cannot be retrieved from the set, but probablistic
 * membership tests are possible. A Bloom filter will never claim that an
 * element is not in the set when it is, but it might claim that an element is
 * in the set with a certain (low) probability when the set does not in fact
 * contain that element, i.e. Bloom filters can have false positives, but no
 * false negatives.
 *
 * Internally, the Bloom filter is represented by a set of m bits. Elements are
 * added to the set by hashing their value with k different hash functions
 * which yield a hash value between 0 and m - 1, and setting the k
 * corresponding bits in the bit set. Testing for set membership works the same
 * way, except that the corresponding bits are tested, and if they're all 1,
 * the set likely contains the value.
 *
 * Some care must be taken to chose the design parameters m and k. Given m and
 * an expected number n of elements that the set typically contains, the
 * optimal choice for the number of hash functions k is @f$k = \frac{m}{n}
 * \ln(2)@f$ (optimal means the k that minimises the probability of a false
 * positive).
 *
 * Alternatively, given the number of bits m in the Bloom filter and the number
 * of elements inserted, the false positive probability p is asymptotically
 * @f$\ln(p) = - \frac{m}{n} (\ln(2))^2@f$ for large m and n (if k is chosen
 * optimally).
 *
 * The number of elements in the filter can be approximated using @f$-
 * \frac{m}{k} \ln(1-\frac{\mathrm{popcount}}{m})@f$ where popcount denotes the
 * number of bits set in the Bloom filter's bit set.
 *
 * @tparam T		type of elements to be stored
 * @tparam CAPACITY	capacity (number of elements at which false positive
 * 			probability will be reached (approximately))
 * @tparam PNUMER	numerator of false positive probability
 * @tparam PDENOM	denominator of false positive probability
 * @tparam HASH		hash to be used on elements of type T
 *
 * @note The code uses an implementation of log2 as template metaprogram to
 * determine the size of the bit set at compile time. This isn't perfect in
 * terms of accuracy since log2 needs to be approximated in fixed point
 * arithmetic without overflowing the integer data types; the best results are
 * achieved with PDENOM = 1 << k.
 *
 * @note I do not use std::hash as default for the HASH template argument. The
 * reason for this decision is that some C++ implementations implement it as
 * the trivial hash function for POD types (notably gcc). While fast, this
 * tends to cause many hash collisions when the hash is somehow shortend to
 * fit the number of hash buckets, leading to very poor performance. The FNV1a
 * implementation I use instead is reasonably fast and hash good properties,
 * avoiding this particular problem.
 */
template <class T, unsigned CAPACITY = 128,
	 unsigned PNUMER = 1, unsigned PDENOM = 64,
	 class HASH = BloomFilterImpl::HashFNV1a<T> >
class BloomFilter
{
    private:
	static_assert(PNUMER < PDENOM, "Probability >= 1, error.");

    public:
	/// capacity of Bloom filter
	enum { capacity = CAPACITY };
	/// target probability for false positive at capacity
	constexpr static double pFalsePositiveTarget()
	{ return double(PNUMER) / double(PDENOM); }
	/// number of hash functions used by Bloom filter
	enum { nHashes = _nHashes(PNUMER, PDENOM) };
	/// number of bits in Bloom filter
	enum { nBits = _nBits(CAPACITY, PNUMER, PDENOM) };
	/// probability for false positive at capacity
	constexpr static double pFalsePositive()
	{ return std::pow(2., -nBits * BloomFilterImpl::log(2.) / capacity); }
    private:
	/// bits in Bloom filter
	std::bitset<nBits> m_bits;
#ifdef BLOOMFILTER_FASTHASH
	/// type of result of hashing a T
	typedef typename HASH::result_type hashresult1_type;
	static_assert(sizeof(hashresult1_type) == sizeof(uint64_t),
		"BLOOMFILTER_FASTHASH only supported for 64 bit hash");
	/// get our second hash by splitting 64 bit hash in two
	typedef uint32_t hashresult2_type;
#else
	/// type of result of hashing a T
	typedef typename HASH::result_type hashresult1_type;
	/// type of the second hash object
	typedef typename BloomFilterImpl::HashFNV1a<hashresult1_type>
	    HASH2;
	/// result type of second hash
	typedef typename HASH2::result_type hashresult2_type;
	static_assert(sizeof(hashresult1_type) == sizeof(hashresult2_type),
		"The two hash functions' return types are incompatible!");
#endif
	/// true if number if bits is small enough to use very fast hashing
	enum { canUseVeryFastHash = _canUseVeryFastHash(
		std::numeric_limits<hashresult1_type>::max(), nBits, nHashes)
	};

    public:
	/** @brief create empty set
	 */
	constexpr BloomFilter() { }

	/// clear the Bloom filter
	void clear() { m_bits.reset(); }

	/** @brief count bits set in Bloom filter
	 *
	 * @returns number of bits set in Bloom filter ("population count")
	 */
	inline int popcount() const
	{ return m_bits.count(); }

	/** @brief returns true if set is definitely empty
	 *
	 * @returns true if set is empty
	 */
	inline bool empty() const { return m_bits.none(); }

	/** @brief get integer representation of Bloom filter
	 *
	 * @param nword 32-bit word of the Bloom filter to get
	 * @returns 32-bit word containing bits 32 * nword to 32 * nword + 31
	 */
	inline constexpr uint32_t getword(unsigned nword) const
	{
	    return (32 * nword >= nBits) ? 0 : /* read past the end of the bitset */
		((32 * (nword + 1) <= nBits) ? /* word fully contained in bitset */
		 ((uint32_t(m_bits[32 * nword +  0]) <<  0)
		  | (uint32_t(m_bits[32 * nword +  1]) <<  1)
		  | (uint32_t(m_bits[32 * nword +  2]) <<  2)
		  | (uint32_t(m_bits[32 * nword +  3]) <<  3)
		  | (uint32_t(m_bits[32 * nword +  4]) <<  4)
		  | (uint32_t(m_bits[32 * nword +  5]) <<  5)
		  | (uint32_t(m_bits[32 * nword +  6]) <<  6)
		  | (uint32_t(m_bits[32 * nword +  7]) <<  7)
		  | (uint32_t(m_bits[32 * nword +  8]) <<  8)
		  | (uint32_t(m_bits[32 * nword +  9]) <<  9)
		  | (uint32_t(m_bits[32 * nword + 10]) << 10)
		  | (uint32_t(m_bits[32 * nword + 11]) << 11)
		  | (uint32_t(m_bits[32 * nword + 12]) << 12)
		  | (uint32_t(m_bits[32 * nword + 13]) << 13)
		  | (uint32_t(m_bits[32 * nword + 14]) << 14)
		  | (uint32_t(m_bits[32 * nword + 15]) << 15)
		  | (uint32_t(m_bits[32 * nword + 16]) << 16)
		  | (uint32_t(m_bits[32 * nword + 17]) << 17)
		  | (uint32_t(m_bits[32 * nword + 18]) << 18)
		  | (uint32_t(m_bits[32 * nword + 19]) << 19)
		  | (uint32_t(m_bits[32 * nword + 20]) << 20)
		  | (uint32_t(m_bits[32 * nword + 21]) << 21)
		  | (uint32_t(m_bits[32 * nword + 22]) << 22)
		  | (uint32_t(m_bits[32 * nword + 23]) << 23)
		  | (uint32_t(m_bits[32 * nword + 24]) << 24)
		  | (uint32_t(m_bits[32 * nword + 25]) << 25)
		  | (uint32_t(m_bits[32 * nword + 26]) << 26)
		  | (uint32_t(m_bits[32 * nword + 27]) << 27)
		  | (uint32_t(m_bits[32 * nword + 28]) << 28)
		  | (uint32_t(m_bits[32 * nword + 29]) << 29)
		  | (uint32_t(m_bits[32 * nword + 30]) << 30)
		  | (uint32_t(m_bits[32 * nword + 31]) << 31)) : /* partially contained word */
		  (((32 * nword +  0 < nBits) ? (uint32_t(m_bits[32 * nword +  0]) <<  0) : 0)
		   | ((32 * nword +  1 < nBits) ? (uint32_t(m_bits[32 * nword +  1]) <<  1) : 0)
		   | ((32 * nword +  2 < nBits) ? (uint32_t(m_bits[32 * nword +  2]) <<  2) : 0)
		   | ((32 * nword +  3 < nBits) ? (uint32_t(m_bits[32 * nword +  3]) <<  3) : 0)
		   | ((32 * nword +  4 < nBits) ? (uint32_t(m_bits[32 * nword +  4]) <<  4) : 0)
		   | ((32 * nword +  5 < nBits) ? (uint32_t(m_bits[32 * nword +  5]) <<  5) : 0)
		   | ((32 * nword +  6 < nBits) ? (uint32_t(m_bits[32 * nword +  6]) <<  6) : 0)
		   | ((32 * nword +  7 < nBits) ? (uint32_t(m_bits[32 * nword +  7]) <<  7) : 0)
		   | ((32 * nword +  8 < nBits) ? (uint32_t(m_bits[32 * nword +  8]) <<  8) : 0)
		   | ((32 * nword +  9 < nBits) ? (uint32_t(m_bits[32 * nword +  9]) <<  9) : 0)
		   | ((32 * nword + 10 < nBits) ? (uint32_t(m_bits[32 * nword + 10]) << 10) : 0)
		   | ((32 * nword + 11 < nBits) ? (uint32_t(m_bits[32 * nword + 11]) << 11) : 0)
		   | ((32 * nword + 12 < nBits) ? (uint32_t(m_bits[32 * nword + 12]) << 12) : 0)
		   | ((32 * nword + 13 < nBits) ? (uint32_t(m_bits[32 * nword + 13]) << 13) : 0)
		   | ((32 * nword + 14 < nBits) ? (uint32_t(m_bits[32 * nword + 14]) << 14) : 0)
		   | ((32 * nword + 15 < nBits) ? (uint32_t(m_bits[32 * nword + 15]) << 15) : 0)
		   | ((32 * nword + 16 < nBits) ? (uint32_t(m_bits[32 * nword + 16]) << 16) : 0)
		   | ((32 * nword + 17 < nBits) ? (uint32_t(m_bits[32 * nword + 17]) << 17) : 0)
		   | ((32 * nword + 18 < nBits) ? (uint32_t(m_bits[32 * nword + 18]) << 18) : 0)
		   | ((32 * nword + 19 < nBits) ? (uint32_t(m_bits[32 * nword + 19]) << 19) : 0)
		   | ((32 * nword + 20 < nBits) ? (uint32_t(m_bits[32 * nword + 20]) << 20) : 0)
		   | ((32 * nword + 21 < nBits) ? (uint32_t(m_bits[32 * nword + 21]) << 21) : 0)
		   | ((32 * nword + 22 < nBits) ? (uint32_t(m_bits[32 * nword + 22]) << 22) : 0)
		   | ((32 * nword + 23 < nBits) ? (uint32_t(m_bits[32 * nword + 23]) << 23) : 0)
		   | ((32 * nword + 24 < nBits) ? (uint32_t(m_bits[32 * nword + 24]) << 24) : 0)
		   | ((32 * nword + 25 < nBits) ? (uint32_t(m_bits[32 * nword + 25]) << 25) : 0)
		   | ((32 * nword + 26 < nBits) ? (uint32_t(m_bits[32 * nword + 26]) << 26) : 0)
		   | ((32 * nword + 27 < nBits) ? (uint32_t(m_bits[32 * nword + 27]) << 27) : 0)
		   | ((32 * nword + 28 < nBits) ? (uint32_t(m_bits[32 * nword + 28]) << 28) : 0)
		   | ((32 * nword + 29 < nBits) ? (uint32_t(m_bits[32 * nword + 29]) << 29) : 0)
		   | ((32 * nword + 30 < nBits) ? (uint32_t(m_bits[32 * nword + 30]) << 30) : 0)
		   | ((32 * nword + 31 < nBits) ? (uint32_t(m_bits[32 * nword + 31]) << 31) : 0)));
	}

	/** @brief set integer representation of Bloom filter
	 *
	 * @param nword which 32-bit word of the Bloom filter to set
	 * @param word value of 32-bit word of the Bloom filter to set
	 */
	inline void setword(unsigned nword, uint32_t word)
	{
	    if (32 * (nword + 1) <= nBits) {
		m_bits[nword * 32 +  0] = bool((word >>  0) & 1),
		    m_bits[nword * 32 +  1] = bool((word >>  1) & 1),
		    m_bits[nword * 32 +  2] = bool((word >>  2) & 1),
		    m_bits[nword * 32 +  3] = bool((word >>  3) & 1),
		    m_bits[nword * 32 +  4] = bool((word >>  4) & 1),
		    m_bits[nword * 32 +  5] = bool((word >>  5) & 1),
		    m_bits[nword * 32 +  6] = bool((word >>  6) & 1),
		    m_bits[nword * 32 +  7] = bool((word >>  7) & 1),
		    m_bits[nword * 32 +  8] = bool((word >>  8) & 1),
		    m_bits[nword * 32 +  9] = bool((word >>  9) & 1),
		    m_bits[nword * 32 + 10] = bool((word >> 10) & 1),
		    m_bits[nword * 32 + 11] = bool((word >> 11) & 1),
		    m_bits[nword * 32 + 12] = bool((word >> 12) & 1),
		    m_bits[nword * 32 + 13] = bool((word >> 13) & 1),
		    m_bits[nword * 32 + 14] = bool((word >> 14) & 1),
		    m_bits[nword * 32 + 15] = bool((word >> 15) & 1),
		    m_bits[nword * 32 + 16] = bool((word >> 16) & 1),
		    m_bits[nword * 32 + 17] = bool((word >> 17) & 1),
		    m_bits[nword * 32 + 18] = bool((word >> 18) & 1),
		    m_bits[nword * 32 + 19] = bool((word >> 19) & 1),
		    m_bits[nword * 32 + 20] = bool((word >> 20) & 1),
		    m_bits[nword * 32 + 21] = bool((word >> 21) & 1),
		    m_bits[nword * 32 + 22] = bool((word >> 22) & 1),
		    m_bits[nword * 32 + 23] = bool((word >> 23) & 1),
		    m_bits[nword * 32 + 24] = bool((word >> 24) & 1),
		    m_bits[nword * 32 + 25] = bool((word >> 25) & 1),
		    m_bits[nword * 32 + 26] = bool((word >> 26) & 1),
		    m_bits[nword * 32 + 27] = bool((word >> 27) & 1),
		    m_bits[nword * 32 + 28] = bool((word >> 28) & 1),
		    m_bits[nword * 32 + 29] = bool((word >> 29) & 1),
		    m_bits[nword * 32 + 30] = bool((word >> 30) & 1),
		    m_bits[nword * 32 + 31] = bool((word >> 31) & 1);
	    } else if (32 * nword < nBits) {
		if (nword * 32 +  0 < nBits) m_bits[nword * 32 +  0] = bool((word >>  0) & 1);
		if (nword * 32 +  1 < nBits) m_bits[nword * 32 +  1] = bool((word >>  1) & 1);
		if (nword * 32 +  2 < nBits) m_bits[nword * 32 +  2] = bool((word >>  2) & 1);
		if (nword * 32 +  3 < nBits) m_bits[nword * 32 +  3] = bool((word >>  3) & 1);
		if (nword * 32 +  4 < nBits) m_bits[nword * 32 +  4] = bool((word >>  4) & 1);
		if (nword * 32 +  5 < nBits) m_bits[nword * 32 +  5] = bool((word >>  5) & 1);
		if (nword * 32 +  6 < nBits) m_bits[nword * 32 +  6] = bool((word >>  6) & 1);
		if (nword * 32 +  7 < nBits) m_bits[nword * 32 +  7] = bool((word >>  7) & 1);
		if (nword * 32 +  8 < nBits) m_bits[nword * 32 +  8] = bool((word >>  8) & 1);
		if (nword * 32 +  9 < nBits) m_bits[nword * 32 +  9] = bool((word >>  9) & 1);
		if (nword * 32 + 10 < nBits) m_bits[nword * 32 + 10] = bool((word >> 10) & 1);
		if (nword * 32 + 11 < nBits) m_bits[nword * 32 + 11] = bool((word >> 11) & 1);
		if (nword * 32 + 12 < nBits) m_bits[nword * 32 + 12] = bool((word >> 12) & 1);
		if (nword * 32 + 13 < nBits) m_bits[nword * 32 + 13] = bool((word >> 13) & 1);
		if (nword * 32 + 14 < nBits) m_bits[nword * 32 + 14] = bool((word >> 14) & 1);
		if (nword * 32 + 15 < nBits) m_bits[nword * 32 + 15] = bool((word >> 15) & 1);
		if (nword * 32 + 16 < nBits) m_bits[nword * 32 + 16] = bool((word >> 16) & 1);
		if (nword * 32 + 17 < nBits) m_bits[nword * 32 + 17] = bool((word >> 17) & 1);
		if (nword * 32 + 18 < nBits) m_bits[nword * 32 + 18] = bool((word >> 18) & 1);
		if (nword * 32 + 19 < nBits) m_bits[nword * 32 + 19] = bool((word >> 19) & 1);
		if (nword * 32 + 20 < nBits) m_bits[nword * 32 + 20] = bool((word >> 20) & 1);
		if (nword * 32 + 21 < nBits) m_bits[nword * 32 + 21] = bool((word >> 21) & 1);
		if (nword * 32 + 22 < nBits) m_bits[nword * 32 + 22] = bool((word >> 22) & 1);
		if (nword * 32 + 23 < nBits) m_bits[nword * 32 + 23] = bool((word >> 23) & 1);
		if (nword * 32 + 24 < nBits) m_bits[nword * 32 + 24] = bool((word >> 24) & 1);
		if (nword * 32 + 25 < nBits) m_bits[nword * 32 + 25] = bool((word >> 25) & 1);
		if (nword * 32 + 26 < nBits) m_bits[nword * 32 + 26] = bool((word >> 26) & 1);
		if (nword * 32 + 27 < nBits) m_bits[nword * 32 + 27] = bool((word >> 27) & 1);
		if (nword * 32 + 28 < nBits) m_bits[nword * 32 + 28] = bool((word >> 28) & 1);
		if (nword * 32 + 29 < nBits) m_bits[nword * 32 + 29] = bool((word >> 29) & 1);
		if (nword * 32 + 30 < nBits) m_bits[nword * 32 + 30] = bool((word >> 30) & 1);
		if (nword * 32 + 31 < nBits) m_bits[nword * 32 + 31] = bool((word >> 31) & 1);
	    }
	}

	/** @brief return string representation of BloomFilter
	 *
	 * @returns string representing the BloomFilter
	 */
	operator std::string() const
	{
	    constexpr auto conv = [] (unsigned val) {
		// we do something that's almost base64 encoding
		if (val < 26) return char('A' + (val - 0));
		else if (val < 52) return char('a' + (val - 26));
		else if (val < 62) return char('0' + (val - 52));
		else if (62 == val) return '+';
		else return '/';
	    };
	    std::string retVal;
	    const unsigned words = (nBits / 32) + bool(nBits % 32);
	    const unsigned symbols = (nBits / 6) + bool(nBits % 6);
	    retVal.reserve(symbols);
	    unsigned inof = 0, iw = 0, ina;
	    uint32_t acc, oflo = 0, tmp = 0;
	    do {
		acc = getword(iw),
		    ina = std::min(32u, unsigned(nBits) - 32 * iw);
		++iw;
		switch (inof) {
		    case 0: // no overflow bits from last iteration
			oflo = acc >> 30;
			if (ina) {
			    retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
			    if (ina) {
				retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
				if (ina) {
				    retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
				    if (ina) {
					retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
					if (ina) {
					    retVal += conv(acc & 0x3f); inof += (ina > 6) ? (ina - 6) : -inof;
					} else { inof = 0; }
				    } else { inof = 0; }
				} else { inof = 0; }
			    } else { inof = 0; }
			} else { inof = 0; }
			break;
		    case 2: // two overflow bits from last iteration
			tmp = oflo; oflo = acc >> 28, acc <<= 2; acc |= tmp;
			if (ina) {
			    retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
			    if (ina) {
				retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
				if (ina) {
				    retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
				    if (ina) {
					retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
					if (ina) {
					    retVal += conv(acc & 0x3f); inof += (ina > 6) ? (ina - 6) : -inof;
					} else { inof = 0; }
				    } else { inof = 0; }
				} else { inof = 0; }
			    } else { inof = 0; }
			} else { inof = 0; }
			break;
		    case 4: // four overflow bits from last iteration
			tmp = oflo; oflo = acc >> 26, acc <<= 4; acc |= tmp;
			if (ina) {
			    retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
			    if (ina) {
				retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
				if (ina) {
				    retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
				    if (ina) {
					retVal += conv(acc & 0x3f); acc >>= 6, ina -= std::min(6u, ina);
					if (ina) {
					    retVal += conv(acc & 0x3f); inof += (ina > 6) ? (ina - 6) : -inof;
					} else { inof = 0; }
				    } else { inof = 0; }
				} else { inof = 0; }
			    } else { inof = 0; }
			} else { inof = 0; }
			if (inof) { retVal += conv(oflo & 0x3f); inof = 0; }
			break;
		}
	    } while (iw < words);
	    // output remaining overflow bits, if any
	    if (inof) { retVal += conv(oflo & 0x3f); inof = 0; }
	    if (retVal.size() != symbols) {
		// verify that the string has the right length
		throw std::invalid_argument(__func__);
	    }
	    return retVal;
	}

	/** @brief set BloomFilter from string representation
	 *
	 * @param str	string representation from which to set the BloomFilter
	 */
	void set(const std::string& str)
	{
	    constexpr auto conv = [] (char c) {
		// we do something that's almost base64 decoding
		if ('A' <= c && c <= 'Z') return uint32_t(0 + c - 'A');
		else if ('a' <= c && c <= 'z') return uint32_t(26 + c - 'a');
		else if ('0' <= c && c <= '9') return uint32_t(52 + c - '0');
		else if ('+' == c) return uint32_t(62);
		else if ('/' == c) return uint32_t(63);
		throw std::invalid_argument(__func__);
		return uint32_t(-1); // should never reach this point
	    };
	    const unsigned symbols = (nBits / 6) + bool(nBits % 6);
	    if (str.size() != symbols) {
		// verify the string has the right length
		throw std::invalid_argument(__func__);
	    }
	    unsigned iw = 0, ipos = 0, inof = 0;
	    uint32_t acc = 0, oflo = 0;
	    while (ipos < str.size()) { // convert while there's input left
		// three possibilities to read a 32 bit word from 6 bit input nibbles
		switch (inof) {
		    case 0: // oflo empty
			acc = ((ipos + 0 < str.size()) ? conv(str[ipos + 0]) : 0) |
			    ((ipos + 1 < str.size()) ? (conv(str[ipos + 1]) <<  6) : 0) |
			    ((ipos + 2 < str.size()) ? (conv(str[ipos + 2]) << 12) : 0) |
			    ((ipos + 3 < str.size()) ? (conv(str[ipos + 3]) << 18) : 0) |
			    ((ipos + 4 < str.size()) ? (conv(str[ipos + 4]) << 24) : 0);
			if (ipos + 5 < str.size()) {
			    oflo = conv(str[ipos + 5]);
			    acc |= (oflo << 30);
			    oflo >>= 2, inof = 4, ipos += std::min(6u, unsigned(str.size() - ipos));
			} else {
			    oflo = 0, inof = 0, ipos = str.size();
			}
			break;
		    case 2: // oflo contains 2 bits
			acc = oflo |
			    ((ipos + 0 < str.size()) ? (conv(str[ipos + 0]) <<  2) : 0) |
			    ((ipos + 1 < str.size()) ? (conv(str[ipos + 1]) <<  8) : 0) |
			    ((ipos + 2 < str.size()) ? (conv(str[ipos + 2]) << 14) : 0) |
			    ((ipos + 3 < str.size()) ? (conv(str[ipos + 3]) << 20) : 0) |
			    ((ipos + 4 < str.size()) ? (conv(str[ipos + 4]) << 26) : 0);
			oflo = 0, inof = 0, ipos += std::min(5u, unsigned(str.size() - ipos));
			break;
		    case 4: // oflo contains 4 bits
			acc = oflo |
			    ((ipos + 0 < str.size()) ? (conv(str[ipos + 0]) <<  4) : 0) |
			    ((ipos + 1 < str.size()) ? (conv(str[ipos + 1]) << 10) : 0) |
			    ((ipos + 2 < str.size()) ? (conv(str[ipos + 2]) << 16) : 0) |
			    ((ipos + 3 < str.size()) ? (conv(str[ipos + 3]) << 22) : 0);
			if (ipos + 4 < str.size()) {
			    oflo = conv(str[ipos + 4]);
			    acc |= (oflo << 28);
			    oflo >>= 4, inof = 2, ipos += std::min(5u, unsigned(str.size() - ipos));
			} else {
			    inof = 0, ipos = str.size();
			}
			break;
		}
		setword(iw++, acc); // set next word
	    }
	    if (inof) setword(iw, oflo); // set any bits remaining in oflo
	    if (str.size() != ipos) {
		// verify that we consumed the full input string
		throw std::invalid_argument(__func__);
	    }
	    if (iw * 32 < nBits || iw * 32 >= nBits + 32) {
		// verify that we converted the right number of bits
		throw std::invalid_argument(__func__);
	    }
	}

	/** @brief construct a BloomFilter from its string representation
	 *
	 * @param str	string from which to construct BloomFilter
	 */
	BloomFilter(const std::string& str)
	{ set(str); }

	/** @brief return an estimate of set size
	 *
	 * @returns estimate of set size
	 *
	 * @note Since Bloom filters are probablistic data structures, it is
	 * just an estimate. It is impossible to know how many hash collisions
	 * there are when setting bits...
	 *
	 * The number of elements in the filter can be approximated using @f$-
	 * \frac{m}{k} \ln(1-\frac{\mathrm{popcount}}{m})@f$ where popcount
	 * denotes the number of bits set in the Bloom filter's bit set.
	 */
	inline size_t size() const
	{
	    return std::round(-double(nBits) / double(nHashes) *
		    std::log(1. - double(popcount()) / double(nBits)));
	}

	/** @brief insert a value
	 *
	 * @param val	value to insert
	 */
	inline void insert(const T& val)
	{
	    if (!canUseVeryFastHash) {
#ifdef BLOOMFILTER_FASTHASH
		const auto h = HASH()(val);
		const auto h1 = h & 0xfffffffful;
		const auto h2 = h >> 32;
#else
		const auto h1 = HASH()(val);
		const auto h2 = HASH2()(h1);
#endif
		for (unsigned i = 0; i < nHashes; ++i) {
		    m_bits[(h1 + i * h2) % nBits] = true;
		}
	    } else {
		auto h = HASH()(val);
		// if we don't lose precision by scaling in double precision
		// to the range 0..(nBits^nHashes - 1), do so (as it produces
		// fewer collisions)
		if (2 == std::numeric_limits<double>::radix &&
			std::pow(static_cast<decltype(h)>(nBits), int(nHashes)) <
			(static_cast<decltype(h)>(1) <<
			 std::numeric_limits<double>::digits)) {
		    h = static_cast<decltype(h)>(double(h) *
			    (double(std::pow(static_cast<decltype(h)>(nBits),
					     int(nHashes))) /
			     (1.0 + double(std::numeric_limits<decltype(h)>::max()))));
		}
		for (unsigned i = 0; i < nHashes; ++i) {
		    m_bits[h % nBits] = true;
		    h /= nBits;
		}
	    }
	}

	/** @brief insert elements between iterators begin and end
	 *
	 * @param begin	iterator to start of range of elements to be inserted
	 * @param end	iterator to end of range of elements to be inserted
	 */
	template<class IT>
	inline void insert(IT begin, IT end)
	{ std::for_each(begin, end, [&] (const T& val) { insert(val); }); }
	
	/** @brief insert the contents of a container or range
	 *
	 * @param r	range or container for use in range-based for
	 */
	template<class RC>
	inline typename std::enable_if<
		std::is_convertible<decltype(*std::begin(RC())),
		const T&>::value, void>::type insert(const RC& r)
	{ insert(std::begin(r), std::end(r)); }

	/** @brief construct set from contents of a container or range
	 *
	 * @param r	range or container for use in range-based for
	 */
	template<class RC>
	inline BloomFilter(typename std::enable_if<
		std::is_convertible<decltype(*std::begin(RC())),
		const T&>::value, const RC&&>::type r)
	{ insert(std::forward<RC>(r)); }

	/** @brief construct set from elements between iterators begin and end
	 *
	 * @param begin	iterator to start of range of elements to be inserted
	 * @param end	iterator to end of range of elements to be inserted
	 */
	template<class IT>
	inline BloomFilter(IT&& begin, IT&& end)
	{ insert(std::forward<IT>(begin), std::forward<IT>(end)); }

	/** @brief find value in BloomFilter
	 *
	 * @param val	value to test for membership
	 *
	 * @note This routine might give false positive (claim an int is in the set
	 * when it is not), but never false negative.
	 *
	 * To check more than one value, insert those values into another
	 * BloomFilter and intersect.
	 */
	inline bool find(const T& val) const
	{
	    if (!canUseVeryFastHash) {
#ifdef BLOOMFILTER_FASTHASH
		const auto h = HASH()(val);
		const auto h1 = h & 0xfffffffful;
		const auto h2 = h >> 32;
#else
		const auto h1 = HASH()(val);
		const auto h2 = HASH2()(h1);
#endif
		for (unsigned i = 0; i < nHashes; ++i) {
		    if (!m_bits[(h1 + i * h2) % nBits]) return false;
		}
	    } else {
		auto h = HASH()(val);
		// if we don't lose precision by scaling in double precision
		// to the range 0..(nBits^nHashes - 1), do so (as it produces
		// fewer collisions)
		if (2 == std::numeric_limits<double>::radix &&
			std::pow(static_cast<decltype(h)>(nBits), int(nHashes)) <
			(static_cast<decltype(h)>(1) <<
			 std::numeric_limits<double>::digits)) {
		    h = static_cast<decltype(h)>(double(h) *
			    (double(std::pow(static_cast<decltype(h)>(nBits),
					     int(nHashes))) /
			     (1.0 + double(std::numeric_limits<decltype(h)>::max()))));
		}
		for (unsigned i = 0; i < nHashes; ++i) {
		    if (!m_bits[h % nBits]) return false;
		    h /= nBits;
		}
	    }
	    return true;
	}

	/** @brief perform set intersection (overwriting *this with the result)
	 *
	 * @param other	BloomFilter to intersect with
	 *
	 * @returns *this, result of the intersection
	 */
	BloomFilter& operator&=(const BloomFilter& other)
	{
	    m_bits &= other.m_bits;
	    return *this;
	}

	/** @brief perform set union (overwriting *this with the result)
	 *
	 * @param other	BloomFilter to unify with
	 *
	 * @returns *this, result of the union
	 */
	BloomFilter& operator|=(const BloomFilter& other)
	{
	    m_bits |= other.m_bits;
	    return *this;
	}

	/** @brief compare Bloom filters for equality
	 *
	 * @param other Bloom filter to compare against
	 * @returns true if other and this are the same, false otherwise
	 */
	bool operator==(const BloomFilter& other) const
	{ return m_bits == other.m_bits; }

	/** @brief compare Bloom filters for inequality
	 *
	 * @param other Bloom filter to compare against
	 * @returns true if other and this are not the same, false otherwise
	 */
	bool operator!=(const BloomFilter& other) const
	{ return m_bits != other.m_bits; }
};

/// perform set intersection
template <class T, unsigned CAPACITY, unsigned PNUMER, unsigned PDENOM, class HASH>
BloomFilter<T, CAPACITY, PNUMER, PDENOM, HASH> operator&(
	const BloomFilter<T, CAPACITY, PNUMER, PDENOM, HASH>& s1,
	const BloomFilter<T, CAPACITY, PNUMER, PDENOM, HASH>& s2)
{ return BloomFilter<T, CAPACITY, PNUMER, PDENOM, HASH>(s1) &= s2; }

/// perform set union
template <class T, unsigned CAPACITY, unsigned PNUMER, unsigned PDENOM, class HASH>
BloomFilter<T, CAPACITY, PNUMER, PDENOM, HASH> operator|(
	const BloomFilter<T, CAPACITY, PNUMER, PDENOM, HASH>& s1,
	const BloomFilter<T, CAPACITY, PNUMER, PDENOM, HASH>& s2)
{ return BloomFilter<T, CAPACITY, PNUMER, PDENOM, HASH>(s1) |= s2; }

#undef _nBits
#undef _nNashes
#undef _canUseVeryFastHash

#endif // BLOOMFILTER_H

// vim: tw=78:sw=4:ft=cpp
