#include "ZooPacked.h"

#include <TBuffer.h>

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
}

////////////////////////////////////////////////////////////////////////
// ClassImp directives
////////////////////////////////////////////////////////////////////////

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

ClassImp(ZooPackedState);
ClassImp(ZooPackedVertex);
ClassImp(ZooPackedParticle);

// vim:tw=78:sw=4:ft=cpp
