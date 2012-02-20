/** @file ZooPacked.cpp
 *
 * @author Manuel Schiller <schiller@physi.uni-heidelberg.de>
 * @date 2009-05-11
 */

#include "ZooPacked.h"

namespace ZooPackedUnits {
    template<>
	const length::unpacked length::unit[n] = { 1e-4, 1e-4, 1e-4 };
    template<>
	const bool length::invert[n] = { false, false, false };

    template<>
	const lengthcov::unpacked lengthcov::unit[n] = {
	    1./32767., 1./32767., 1./32767. };
    template<>
	const bool lengthcov::invert[n] = {
	    false, false, false };
	
    template<>
	const energy::unpacked energy::unit[n] = { 1e-2, 1e-2, 1e-2, 1e-2 };
    template<>
	const bool energy::invert[n] = { false, false, false, false };

    template<>
	const energycov::unpacked energycov::unit[n] = {
	    1./32767., 1./32767., 1./32767., 1./32767., 1./32767., 1./32767. };
    template<>
	const bool energycov::invert[n] = {
	    false, false, false, false, false, false };

    template<>
	const state::unpacked state::unit[n] = { 1e-4, 1e-4, 1e-8, 1e-8, 1e-2 };
    template<>
	const bool state::invert[n] = { false, false, false, false, true };

    template<>
	const statecov::unpacked statecov::unit[n] = {
	    1./32767., 1./32767., 1./32767., 1./32767., 1./32767.,
	    1./32767., 1./32767., 1./32767., 1./32767., 1./32767. };
    template<>
	const bool statecov::invert[n] = {
	    false, false, false, false, false,
	    false, false, false, false, false };

    template<>
	const particle::unpacked particle::unit[n] = {
	    1e-4, 1e-4, 1e-4, 1e-2, 1e-2, 1e-2, 1e-2 };
    template<>
	const bool particle::invert[n] = {
	    false, false, false, false, false, false, false };

    template<>
	const particlecov::unpacked particlecov::unit[n] = {
	    1./32767., 1./32767., 1./32767., 1./32767., 1./32767., 1./32767.,
	    1./32767., 1./32767., 1./32767., 1./32767., 1./32767.,
	    1./32767., 1./32767., 1./32767., 1./32767., 1./32767.,
	    1./32767., 1./32767., 1./32767., 1./32767., 1./32767. };
    template<>
	const bool particlecov::invert[n] = {
	    false, false, false, false, false, false,
	    false, false, false, false, false,
	    false, false, false, false, false,
	    false, false, false, false, false };

    template<typename U, typename P, unsigned N>
    typename Unit<U, P, N>::packed Unit<U, P, N>::doPack(
	    typename Unit<U, P, N>::unpacked u)
    {
	// encode NaNs specially
	if (UNLIKELY(std::isnan(u)))
	    return std::numeric_limits<packed>::min();
	// proper rounding (round to even: what you would expect,
	// except for 0.5, where we round either up or down depending on
	// the LSB of the rounded integer; this is correct on average:
	// 0.5 is a tie, and we choose both of the possible outcomes
	// exactly half of the time (assuming that the LSB of the rounded
	// integer is random which it should be))
	unpacked fu = std::floor(u);
	if (u - fu > 0.5) u += unpacked(1);
	else if (u - fu == 0.5 && packed(fu) & 1) u += unpacked(1);
	// saturate
	if (UNLIKELY(unpacked(std::numeric_limits<packed>::max()) < u))
	    return std::numeric_limits<packed>::max();
	if (UNLIKELY(-unpacked(std::numeric_limits<packed>::max()) > u))
	    return -std::numeric_limits<packed>::max();
	return packed(std::floor(u));
    }

    template<typename U, typename P, unsigned N>
    typename Unit<U, P, N>::unpacked Unit<U, P, N>::doUnpack(
	    typename Unit<U, P, N>::packed p)
    {
	// map the NaN code to a quiet NaN
	if (UNLIKELY(std::numeric_limits<packed>::min() == p))
	    return std::numeric_limits<unpacked>::quiet_NaN();
	return unpacked(p);
    }

    template<typename U, typename P, unsigned N>
    typename Unit<U, P, N>::packed Unit<U, P, N>::pack(
	    typename Unit<U, P, N>::unpacked u, unsigned idx)
    {
	if (UNLIKELY(invert[idx]))
	    return doPack(unpacked(1) / (unit[idx] * u));
	else
	    return doPack(u / unit[idx]);
    }

    template<typename U, typename P, unsigned N>
    typename Unit<U, P, N>::unpacked Unit<U, P, N>::unpack(
	    typename Unit<U, P, N>::packed p, unsigned idx)
    {
	if (UNLIKELY(invert[idx]))
	    return unpacked(1) / (unit[idx] * doUnpack(p));
	else
	    return doUnpack(p) * unit[idx];
    }

    // force instatiation of templates
    template class Unit<Double_t, Int_t, 3>;
    template class Unit<Double_t, Short_t, 3>;
    template class Unit<Double_t, Int_t, 4>;
    template class Unit<Double_t, Short_t, 6>;
    template class Unit<Double_t, Int_t, 5>;
    template class Unit<Double_t, Short_t, 10>;
    template class Unit<Double_t, Int_t, 7>;
    template class Unit<Double_t, Short_t, 21>;
}

/***********************************************************************
 * ZooPackedStorage methods
 ***********************************************************************/
template<typename U, unsigned N>
ZooPackedStorage<U, N>::ZooPackedStorage()
{
    // only allow positive dimensions
    BOOST_STATIC_ASSERT(nDim > 0);
    // dimension must match the dimensionality of the unit description
    BOOST_STATIC_ASSERT(unsigned(nDim) == unsigned(UnitsT::n));
    // we only allow for signed packed types
    BOOST_STATIC_ASSERT(PackedT(-1) < PackedT(0));
    Class()->IgnoreTObjectStreamer();
    // uninitialised packed storage is set to all NaNs
    std::fill(m_arr, m_arr + nDim, std::numeric_limits<PackedT>::min());
}

template<typename U, unsigned N>
ZooPackedStorage<U, N>::~ZooPackedStorage() { }

template<typename U, unsigned N>
ZooPackedStorage<U, N>::operator const typename ZooPackedStorage<U, N>::Vector() const
{
    Vector v;
    for (unsigned i = 0; i < nDim; ++i)
	v[i] = operator[](i);
    return v;
}

template<typename U, unsigned N>
const typename ZooPackedStorage<U, N>::UnpackedT
	ZooPackedStorage<U, N>::operator[](unsigned idx) const
{
    assert(LIKELY(idx < nDim));
    return U::unpack(m_arr[idx], idx);
}

template<typename U, unsigned N>
const ZooPackedStorage<U, N>& ZooPackedStorage<U, N>::set(
	unsigned idx, typename ZooPackedStorage<U, N>::UnpackedT val)
{
    assert(LIKELY(idx < nDim));
    m_arr[idx] = U::pack(val, idx);
    return *this;
}

/***********************************************************************
 * ZooPackedStorageWithError methods
 ***********************************************************************/
template<typename BaseUnit, typename OffDiagUnit, unsigned N>
ZooPackedStorageWithError<BaseUnit, OffDiagUnit, N>::ZooPackedStorageWithError()
{ Class()->IgnoreTObjectStreamer(); }

template<typename BaseUnit, typename OffDiagUnit, unsigned N>
ZooPackedStorageWithError<BaseUnit, OffDiagUnit, N>::~ZooPackedStorageWithError()
{ }

template<typename BaseUnit, typename OffDiagUnit, unsigned N>
ZooPackedStorageWithError<BaseUnit, OffDiagUnit, N>::operator
	const typename ZooPackedStorageWithError<BaseUnit, OffDiagUnit, N>::Matrix() const
{
    Matrix retVal;
    for (unsigned i = 0; i < nDim; ++i) {
	if (UNLIKELY(BaseVector::UnitsT::invert[i])) {
	    // if we need to invert the vector, we need to transform
	    // the error as well
	    const UnpackedT inv = m_vect[i];
	    const UnpackedT err = (UnpackedT(1) / m_errs[i]) * (inv * inv);
	    retVal(i, i) = err * err;
	} else {
	    const UnpackedT err = m_errs[i];
	    retVal(i, i) = err * err;
	}
    }
    for (unsigned i = 0; i < nDim; ++i) {
	for (unsigned j = 0; j < i; ++j)
	    retVal(i, j) = retVal(j, i) =  m_correl[(i*(i-1))/2 + j] *
		std::sqrt(retVal(i, i)) * std::sqrt(retVal(j, j));
    }
    return retVal;
}

// force instatiation of templates
template class ZooPackedStorage<ZooPackedUnits::length, 3>;
template class ZooPackedStorage<ZooPackedUnits::lengthcov, 3>;
template class ZooPackedStorageWithError<ZooPackedUnits::length,
	 ZooPackedUnits::lengthcov, 3>;
template class ZooPackedStorage<ZooPackedUnits::energy, 4>;
template class ZooPackedStorage<ZooPackedUnits::energycov, 6>;
template class ZooPackedStorageWithError<ZooPackedUnits::energy,
	 ZooPackedUnits::energycov, 4>;
template class ZooPackedStorage<ZooPackedUnits::state, 5>;
template class ZooPackedStorage<ZooPackedUnits::statecov, 10>;
template class ZooPackedStorageWithError<ZooPackedUnits::state,
	 ZooPackedUnits::statecov, 5>;
template class ZooPackedStorage<ZooPackedUnits::particle, 7>;
template class ZooPackedStorage<ZooPackedUnits::particlecov, 21>;
template class ZooPackedStorageWithError<ZooPackedUnits::particle,
	 ZooPackedUnits::particlecov, 7>;

ClassImp(ZooPackedPosition);
ClassImp(ZooPackedPositionWithError::BaseVector);
ClassImp(ZooPackedPositionWithError::OffDiagVector);
ClassImp(ZooPackedPositionWithError);

ClassImp(ZooPackedLorentzVector);
ClassImp(ZooPackedLorentzVectorWithError::BaseVector);
ClassImp(ZooPackedLorentzVectorWithError::OffDiagVector);
ClassImp(ZooPackedLorentzVectorWithError);

ClassImp(ZooPackedStateVector);
ClassImp(ZooPackedStateVectorWithError::BaseVector);
ClassImp(ZooPackedStateVectorWithError::OffDiagVector);
ClassImp(ZooPackedStateVectorWithError);

ClassImp(ZooPackedParticleVector);
ClassImp(ZooPackedParticleVectorWithError::BaseVector);
ClassImp(ZooPackedParticleVectorWithError::OffDiagVector);
ClassImp(ZooPackedParticleVectorWithError);

// vim:tw=78:sw=4:ft=cpp
