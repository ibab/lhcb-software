#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class ::ZooPackedUnits::length-!;
#pragma link C++ class ::ZooPackedUnits::lengthcov-!;
#pragma link C++ class ::ZooPackedUnits::energy-!;
#pragma link C++ class ::ZooPackedUnits::energycov-!;
#pragma link C++ class ::ZooPackedUnits::state-!;
#pragma link C++ class ::ZooPackedUnits::statecov-!;
#pragma link C++ class ::ZooPackedUnits::particle-!;
#pragma link C++ class ::ZooPackedUnits::particlecov-!;

#pragma link C++ class ZooPackedPosition+;
#pragma link C++ class ZooPackedPositionWithError::BaseVector+;
#pragma link C++ class ZooPackedPositionWithError::OffDiagVector+;
#pragma link C++ class ZooPackedPositionWithError+;

#pragma link C++ class ZooPackedLorentzVector+;
#pragma link C++ class ZooPackedLorentzVectorWithError::BaseVector+;
#pragma link C++ class ZooPackedLorentzVectorWithError::OffDiagVector+;
#pragma link C++ class ZooPackedLorentzVectorWithError+;

#pragma link C++ class ZooPackedStateVector+;
#pragma link C++ class ZooPackedStateVectorWithError::BaseVector+;
#pragma link C++ class ZooPackedStateVectorWithError::OffDiagVector+;
#pragma link C++ class ZooPackedStateVectorWithError+;

#pragma link C++ class ZooPackedParticleVector+;
#pragma link C++ class ZooPackedParticleVectorWithError::BaseVector+;
#pragma link C++ class ZooPackedParticleVectorWithError::OffDiagVector+;
#pragma link C++ class ZooPackedParticleVectorWithError+;

#pragma link C++ class ZooPackedState+;
#pragma link C++ class ZooPackedStates+;
#pragma link C++ class ZooPackedVertex+;
#pragma link C++ class ZooPackedParticle+;

#pragma link C++ class ZooKeyValueBlock+;
#pragma link C++ class ZooKeyValueBlock::const_iterator+;
#pragma link C++ class ZooKeyValueBlock::r_proxy+;
#pragma link C++ class ZooKeyValueBlock::rw_proxy+;
#pragma link C++ function operator<<(std::ostream&, const ZooKeyValueBlock&);

#pragma link C++ class ZooStringToUIDTable+;
#pragma link C++ class ZooStringToUIDTable::const_iterator+;
#pragma link C++ function operator<<(std::ostream&, const ZooStringToUIDTable&);

#pragma link C++ class ZooStringValueBlock+;
#pragma link C++ class ZooStringValueBlock::const_iterator+;
#pragma link C++ class ZooStringValueBlock::r_proxy+;
#pragma link C++ class ZooStringValueBlock::rw_proxy+;
#pragma link C++ function operator<<(std::ostream&, const ZooStringValueBlock&);

#pragma link C++ class ZooLHCbID+;
#pragma link C++ class ZooLHCbIDBlock+;
#pragma link C++ function operator<<(std::ostream&, const ZooLHCbID&);

#pragma link C++ class ZooMCGenEventInfo+;
#pragma link C++ class ZooOccupancy+;
#pragma link C++ class ZooPArray+;
#pragma link C++ class ZooMCPArray+;
#pragma link C++ class ZooVertexArray+;
#pragma link C++ class ZooObjClonesArray+;
#pragma link C++ class ZooP+;
#pragma link C++ class ZooMCP+;
#pragma link C++ class ZooStable+;
#pragma link C++ class ZooDecay+;
#pragma link C++ class ZooTagging+;
#pragma link C++ class ZooGhostCategory+;
#pragma link C++ class ZooTrigger+;
#pragma link C++ class ZooDLL+;
#pragma link C++ class ZooLinks+;
#pragma link C++ class ZooEv+;
#pragma link C++ class ZooMCBkg+;
#pragma link C++ class ZooHitPattern+;
#pragma link C++ function operator&(const ZooHitPattern&, const ZooHitPattern&);
#pragma link C++ function operator|(const ZooHitPattern&, const ZooHitPattern&);
#pragma link C++ function operator^(const ZooHitPattern&, const ZooHitPattern&);
#pragma link C++ function operator~(const ZooHitPattern&);
#pragma link C++ class ZooTrackExtraInfo+;
#pragma link C++ class ZooTrackInfo+;
#pragma link C++ class ZooTreefitInfo+;
#pragma link C++ class ZooParticleInfo+;
#pragma link C++ class ZooLoKiBlock+;
#pragma link C++ class ZooLoKiBlock::const_iterator+;
#pragma link C++ class ZooLoKiBlock::r_proxy+;
#pragma link C++ class ZooLoKiBlock::rw_proxy+;
#pragma link C++ function operator<<(std::ostream&, const ZooLoKiBlock&);

#endif

