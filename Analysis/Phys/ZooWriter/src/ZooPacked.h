/** @file ZooPacked.h
 *
 * @author Manuel Schiller <schiller@physi.uni-heidelberg.de>
 * @date 2009-05-11
 */

#ifndef ZOOPACKED_H
#define ZOOPACKED_H

#include <cmath>
#include <cassert>
#include <algorithm>

#include <TObject.h>
#include <TClass.h>
#include <Math/Vector4D.h>
#include <Math/Point3D.h>
#include <Math/SVector.h>
#include <Math/SMatrix.h>

#include "ZooLikely.h"

#ifndef __CINT__
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>
#else
#define BOOST_STATIC_ASSERT(x)
#endif

/// namespace to hold definition of units used for packed representation
namespace ZooPackedUnits {
    /// base class for a unit
    template<typename U, typename P, unsigned N>
    class Unit {
    public:
	/// type representing unpacked quantity
	typedef U unpacked;
	/// type representing packed quantity
	typedef P packed;
	/// quantity has N dimensions
	enum { n = N };
	/// units in packed representation
	static const unpacked unit[n];
	/// quantities like q/p need to be inverted before storage
	static const bool invert[n];
    private:
	/// helper to do the packing
	static packed doPack(unpacked u);

	/// helper to do the unpacking
	static unpacked doUnpack(packed p);

    public:
	/// pack a quantity according to some unit U
	static packed pack(unpacked u, unsigned idx = 0);

	/// unpack a quantity according to some unit U
	static unpacked unpack(packed p, unsigned idx = 0);
    };

    /// unit of length
    typedef Unit<Double_t, Int_t, 3> length;
    /// unit of length correlation matrix off-diagonal elements
    typedef Unit<Double_t, Short_t, 3> lengthcov;
    /// unit of energy (four-vector)
    typedef Unit<Double_t, Int_t, 4> energy;
    /// unit of energy (four-vector) correlation matrix off-diagonal elements
    typedef Unit<Double_t, Short_t, 6> energycov;
    /// unit of track state (x, y, tx, ty, q/p)
    typedef Unit<Double_t, Int_t, 5> state;
    /// unit of track state correlation matrix off-diagonal-elements
    typedef Unit<Double_t, Short_t, 10> statecov;
    /// unit of particle vector (x, y, z, px, py, pz, E)
    typedef Unit<Double_t, Int_t, 7> particle;
    /// unit of particle correlation matrix off-diagonal elements
    typedef Unit<Double_t, Short_t, 21> particlecov;
}

/// fixed size packed storage in given units
template<typename U, unsigned N>
class ZooPackedStorage : public TObject
{
    public:
	// provide access to template params
	/// type representing packed data
	typedef typename U::packed PackedT;
	/// type representing unpacked data
	typedef typename U::unpacked UnpackedT;
	/// unit of packed representation
	typedef U UnitsT;
	/// dimensionality of representation
	enum { nDim = N };
	/// vector type for unpacked representation
	typedef ROOT::Math::SVector<UnpackedT, nDim> Vector;
    private:
	PackedT m_arr[nDim]; ///< array too keep packed representation

    public:
	/// default constructor
	ZooPackedStorage();

	virtual ~ZooPackedStorage();

#ifndef __CINT__
	/// templated constructor for anything remotely array-like
	/** this constructor gives trouble with CINT; it's not needed
	 * for serialization, so omitting it in inperpreted code is harmless,
	 * just make sure you compile you macros/code
	 */
	template<typename V> ZooPackedStorage(const V& v)
	{
	    // only allow positive dimensions
	    BOOST_STATIC_ASSERT(nDim > 0);
	    // dimension must match the dimensionality of the unit description
	    BOOST_STATIC_ASSERT(unsigned(nDim) == unsigned(UnitsT::n));
	    // we only allow for signed packed types
	    BOOST_STATIC_ASSERT(PackedT(-1) < PackedT(0));
	    Class()->IgnoreTObjectStreamer();
	    set(v);
	}
#endif

	/// fast and convenient read-only access to unpacked data
	const UnpackedT operator[](unsigned idx) const;

	/// set specified element
	const ZooPackedStorage& set(unsigned idx, UnpackedT val);

    private:
#ifndef __CINT__ // the following give trouble with CINT
	/// set from something with std::vector semantics (covers also SVector)
	template<typename T> void _set(const T& rhs, typename T::value_type = 0)
	{
	    unsigned i = 0;
	    for (typename T::const_iterator it = rhs.begin();
		    rhs.end() != it; ++i, ++it)	set(i, *it);
	}

	/// set from an array
	template<typename T> void _set(const T rhs[], int = 0)
	{ for (unsigned i = 0; i < N; ++i) set(i, rhs[i]); }

	/// set from something with std::vector semantics
	template<typename T> void _set(const T& rhs, typename T::Scalar = 0)
	{ typename T::Scalar tmp[N]; rhs.GetCoordinates(tmp); _set(tmp); }
#endif // __CINT__

    public:
	/// set from a GenVector or something with array or std::vector semantics
	template<typename T> void set(const T& rhs)
	{ _set(rhs); }

	/// assignment operator
	template<typename T> const ZooPackedStorage& operator=(const T& rhs)
	{ _set(rhs); return *this; }

#ifndef __CINT__
	/// conversion to an SVector
	/** this conversion operator gives trouble with CINT; it's not needed
	 * for serialization, so omitting it in inperpreted code is harmless,
	 * just make sure you compile you macros/code
	 */
	operator const Vector() const;

	/// convert to some sort of a GenVector (if it can be instantiated)
	/** ROOT::Math::XYZPoint and ROOT::Math::LorentzVector come to mind...
	 * this conversion operator gives trouble with CINT; it's not needed
	 * for serialization, so omitting it in inperpreted code is harmless,
	 * just make sure you compile you macros/code
	 */
	template<typename T> operator const T() const
	{
	    Vector v = *this;
	    T tmp;
	    tmp.SetCoordinates(v.begin(), v.end());
	    return tmp;
	}
#endif // __CINT__

	ClassDef(ZooPackedStorage, 1);
};

/// covariance matrix of fixed size, packed storage representation
template<typename BaseUnit, typename OffDiagUnit, unsigned N>
class ZooPackedStorageWithError : public TObject
{
    public:
	// provide access to template params
	/// dimensionality of packed representation
	enum { nDim = N };
	// the underlying ZooPackedStorage can ignore its TObject portion when
	// streaming
	/// type to represent (in packed format) value and errors
	typedef ZooPackedStorage<BaseUnit, N> BaseVector;
	/// type to represent off-diagonal elements of packed correlation matrix
	typedef ZooPackedStorage<OffDiagUnit, (N*(N-1))/2> OffDiagVector;
	/// (scalar) type of unpacked representation
	typedef typename BaseVector::UnpackedT UnpackedT;
	/// (scalar) type of packed representation (value and error)
	typedef typename BaseVector::PackedT PackedT;
	/// (scalar) type of packed representation (off-diagonal elements of correlation matrix)
	typedef typename OffDiagUnit::packed PackedOffDiagT;
	/// unit of packed representation (values, errors)
	typedef typename BaseVector::UnitsT UnitsT;
	/// unit of packed representation (correlation matrix)
	typedef OffDiagUnit OffDiagUnitT;
	// convenience typedef
	/// type of unpacked vector
	typedef typename BaseVector::Vector Vector;
	/// type of unpacked covariance matrix
	typedef ROOT::Math::SMatrix<UnpackedT, N, N, ROOT::Math::MatRepSym<UnpackedT, N> > Matrix;
	/// this type
	typedef ZooPackedStorageWithError<BaseUnit, OffDiagUnit, N> MYTYPE;

    protected:
	/// vector
	BaseVector m_vect;
	/// errors on the individual elements of a State
	BaseVector m_errs;
	/// off diagonal part of correlation matrix
	OffDiagVector m_correl;

    public:
	/// default constructor
	ZooPackedStorageWithError();

	virtual ~ZooPackedStorageWithError();

#ifndef __CINT__
	/// constructor from a Vector and its covariance matrix
	/** this constructor gives trouble with CINT; it's not needed
	 * for serialization, so omitting it in inperpreted code is harmless,
	 * just make sure you compile you macros/code
	 */
	template<typename V> ZooPackedStorageWithError(
		const V& vect,
		const Matrix& cov = -Matrix(ROOT::Math::SMatrixIdentity()))
	{
 	    Class()->IgnoreTObjectStreamer();
	    m_vect = vect;
	    _set(vect, cov);
	}
#endif

	/// set up from a vector
	template<typename V> void set(const V& vect, const Matrix& cov)
	{
	    // set up error vector
	    m_vect = vect;
	    _set(vect, cov);
	}

    private:
#ifndef __CINT__
	/// copy contents of something with std::vector interface to POD array
	template<typename V> void _get(
		const V& v, typename V::value_type *dst) const
	{
	    typedef typename V::const_iterator it_t;
	    for (it_t it = v.begin(); v.end() != it; ++it, ++dst)
		*dst = *it;
       	}
	// array to array copy
	template<typename V, typename W> void _get(const V v[], W* dst) const
	{ for (unsigned i = 0; i < nDim; ++i, ++dst) *dst = v[i]; }
	// GenVector to array copy
	template<typename V>  void _get(const V& v, typename V::Scalar dst[]) const
	{ v.GetCoordinates(dst); }
	template<typename V> void _set(const V& vect, const Matrix& cov)
	{
	    UnpackedT v[nDim];
	    _get(vect, v);
	    for (unsigned i = 0; i < nDim; ++i) {
		if (UNLIKELY(BaseVector::UnitsT::invert[i])) {
		    // if we need to invert the vector, we need to transform
		    // the error as well
		    const UnpackedT inv = UnpackedT(1) / v[i];
		    m_errs.set(i, UnpackedT(1) / (std::sqrt(cov(i, i)) * inv * inv));
		} else {
		    m_errs.set(i, std::sqrt(cov(i, i)));
		}
		for (unsigned j = 0; j < i; ++j) {
		    // protect against 0. / 0.
		    double tmp = 0.;
		    if (LIKELY(0. != std::abs(cov(i, i)) && 0. != std::abs(cov(j, j))))
			tmp = cov(i, j) /
			    (std::sqrt(cov(i, i)) * std::sqrt(cov(j, j)));
		    else if (0. != std::abs(cov(i, j)))
			tmp = std::numeric_limits<double>::quiet_NaN();
		    m_correl.set((i * (i - 1)) / 2 + j, tmp);
		}
		
		// ok, set up correlation matrix (off diagonal part)
		for (unsigned j = 0; j < i; ++j) {
		    // protect against 0. / 0.
		    double tmp = 0.;
		    if (LIKELY(0. != std::abs(cov(i, i)) && 0. != std::abs(cov(j, j))))
			tmp = cov(i, j) /
			    (std::sqrt(cov(i, i)) * std::sqrt(cov(j, j)));
		    else if (0. != std::abs(cov(i, j)))
			tmp = std::numeric_limits<double>::quiet_NaN();
		    m_correl.set((i * (i - 1)) / 2 + j, tmp);
		}
	    }
	}
#endif // __CINT__
public:

#ifndef __CINT__
	/// return the vector
	/** this conversion operator gives trouble with CINT; it's not needed
	 * for serialization, so omitting it in inperpreted code is harmless,
	 * just make sure you compile you macros/code
	 */
	operator const Vector() const
	{ return m_vect; }
#endif

#ifndef __CINT__
	/// return the covariance matrix
	/** this conversion operator gives trouble with CINT; it's not needed
	 * for serialization, so omitting it in inperpreted code is harmless,
	 * just make sure you compile you macros/code
	 */
	operator const Matrix() const;

	/// convert to some sort of a GenVector (if it can be instantiated)
	/** ROOT::Math::XYZPoint and ROOT::Math::LorentzVector come to mind...
	 * this conversion operator gives trouble with CINT; it's not needed
	 * for serialization, so omitting it in inperpreted code is harmless,
	 * just make sure you compile you macros/code
	 */
	template<typename T> operator const T() const
	{ return m_vect; }
#endif // __CINT__

	/// assignment operator for covariance matrix
	const MYTYPE& operator=(const Matrix& cov)
	{ _set(operator const Vector(), cov); return *this; }
	/// assignment operator from some kind of vector
	template<typename V> const MYTYPE& operator=(const V& vect)
	{
	    Matrix cov(*this);
	    m_vect = vect;
	    _set(vect, cov);
	    return *this;
	}
	/** @brief allow element wise read only access to vector
	 *
	 * note that is is faster to convert to SVector if you need the whole
	 * vector */
	UnpackedT operator[](unsigned idx) const { return m_vect[idx]; }
	/** @brief allow element wise read only access to covariance matrix
	 *
	 * note that is is faster to convert to SMatrix if you need the whole
	 * matrix */
	UnpackedT operator()(unsigned i, unsigned j) const
	{
	    assert(LIKELY(i < nDim)); assert(LIKELY(j < nDim));
	    const double di = BaseVector::UnitsT::invert[i] ?
		((UnpackedT(1) / m_errs[i]) * (m_vect[i] * m_vect[i])) :
		m_errs[i];
	    if (i == j) return di * di;
	    const double dj = BaseVector::UnitsT::invert[j] ?
		((UnpackedT(1) / m_errs[j]) * (m_vect[j] * m_vect[j])) :
		m_errs[j];
	    if (j > i) std::swap(i, j);
	    return m_correl[(i*(i-1))/2 + j] * di * dj;
	}

	ClassDef(ZooPackedStorageWithError, 2);
};

// convenience typedefs (use for standalone objects to which there might be
// TRefs etc.)
/// packed position
typedef ZooPackedStorage<ZooPackedUnits::length, 3> ZooPackedPosition;
/// packed position with error
typedef ZooPackedStorageWithError<ZooPackedUnits::length,
	ZooPackedUnits::lengthcov, 3> ZooPackedPositionWithError;
/// Lorentz Vector (indices in order Px, Py, Pz, E)
typedef ZooPackedStorage<ZooPackedUnits::energy, 4> ZooPackedLorentzVector;
/// Lorentz Vector with covariance matrix (indices in order Px, Py, Pz, E)
typedef ZooPackedStorageWithError<ZooPackedUnits::energy,
	ZooPackedUnits::energycov, 4> ZooPackedLorentzVectorWithError;
/// packed track state
typedef ZooPackedStorage<ZooPackedUnits::state, 5> ZooPackedStateVector;
/// packed track state with error
typedef ZooPackedStorageWithError<ZooPackedUnits::state,
	ZooPackedUnits::statecov, 5> ZooPackedStateVectorWithError;
/// packed particle vector (x, y, z, px, py, pz, E)
typedef ZooPackedStorage<ZooPackedUnits::particle, 7> ZooPackedParticleVector;
/// packed particle vector with error
typedef ZooPackedStorageWithError<ZooPackedUnits::particle,
	ZooPackedUnits::particlecov, 7> ZooPackedParticleVectorWithError;

#endif // ZOOPACKED_H

// vim:tw=78:sw=4:ft=cpp
