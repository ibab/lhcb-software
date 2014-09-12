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
	/// object to hash a T
	static HASH s_hashfn1;
	/// get our second hash by splitting 64 bit hash in two
	typedef uint32_t hashresult2_type;
#else
	/// type of result of hashing a T
	typedef typename HASH::result_type hashresult1_type;
	/// object to hash a T
	static HASH s_hashfn1;
	/// type of second hash
	typedef typename BloomFilterImpl::HashFNV1a<hashresult1_type
	    >::result_type hashresult2_type;
	static_assert(sizeof(hashresult1_type) == sizeof(hashresult2_type),
		"The two hash functions' return types are incompatible!");
	/// second hash object
	static BloomFilterImpl::HashFNV1a<hashresult1_type> s_hashfn2;
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
		const auto h = s_hashfn1(val);
		const auto h1 = h & 0xfffffffful;
		const auto h2 = h >> 32;
#else
		const auto h1 = s_hashfn1(val);
		const auto h2 = s_hashfn2(h1);
#endif
		for (unsigned i = 0; i < nHashes; ++i) {
		    m_bits[(h1 + i * h2) % nBits] = true;
		}
	    } else {
		auto h = s_hashfn1(val);
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
	{ std::for_each(begin, end, this->insert); }
	
	/** @brief insert the contents of a container or range
	 *
	 * @param r	range or container for use in range-based for
	 */
	template<class RC>
	inline void insert(typename std::enable_if<
		std::is_convertible<decltype(*std::begin(RC())),
		const T&>::value ,RC&&>::type r)
	{ insert(std::begin(r), std::end(r)); }

	/** @brief construct set from contents of a container or range
	 *
	 * @param r	range or container for use in range-based for
	 */
	template<class RC>
	inline BloomFilter(typename std::enable_if<
		std::is_convertible<decltype(*std::begin(RC())),
		const T&>::value, RC&&>::type r)
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
		const auto h = s_hashfn1(val);
		const auto h1 = h & 0xfffffffful;
		const auto h2 = h >> 32;
#else
		const auto h1 = s_hashfn1(val);
		const auto h2 = s_hashfn2(h1);
#endif
		for (unsigned i = 0; i < nHashes; ++i) {
		    if (!m_bits[(h1 + i * h2) % nBits]) return false;
		}
	    } else {
		auto h = s_hashfn1(val);
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
