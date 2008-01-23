// $Id: RichG4HpdReflTag.h,v 1.2 2008-01-23 15:52:41 seaso Exp $
#ifndef MISC_RICHG4HPDREFLTAG_H 
#define MISC_RICHG4HPDREFLTAG_H 1
#include "globals.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"


extern void RichG4HpdReflectionTag(const G4Track& aPhotonTk,
                                   G4int aFlagNum );

extern void RichG4HpdQWIncidentTag(const G4Track& aPhotonTk, const G4ThreeVector & aHpdQWPoint );

#endif // MISC_RICHG4HPDREFLTAG_H


