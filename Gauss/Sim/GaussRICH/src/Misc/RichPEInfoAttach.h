#ifndef      RICHPEINFOATTACH_h 
#define      RICHPEINFOATTACH_h 1 
#include "RichPEInfo.h"
#include "G4Track.hh"

extern G4Track* RichPEInfoAttach(const G4Track& aPhotonTk, G4Track* aPETk, const G4ThreeVector & aLocalElectronOrigin);
extern G4Track* RichPEBackScatAttach(const G4Track& aElectronTk, G4Track* aPEBackScatTk);


#endif
