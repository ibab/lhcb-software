// $Id: RichPhotInfoAttach.h,v 1.1 2008-01-21 16:59:01 seaso Exp $
#ifndef GAUSSRICH_RICHPHOTINFOATTACH_H 
#define GAUSSRICH_RICHPHOTINFOATTACH_H 1

// Include files

/** @class RichPhotInfoAttach RichPhotInfoAttach.h Misc/RichPhotInfoAttach.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-10-29
 */
#include "RichInfo.h"
#include "G4Track.hh"

extern void setRichHpdQwPcReflInfo(const G4Track& aPhotonTk,G4int aFlag);
extern G4int getRichHpdQwPcReflInfo(const G4Track& aPhotonTk);
extern G4int getRichHpdQwPcReflInfoSave(const G4Track& aPhotonTk);


#endif // GAUSSRICH_RICHPHOTINFOATTACH_H
