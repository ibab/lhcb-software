#ifndef      GaussRICH_RICHPEINFOATTACH_h 
#define      GaussRICH_RICHPEINFOATTACH_h 1 
#include "RichPEInfo.h"
#include "G4Track.hh"

extern int RichPhotTkRadiatorNumber ( const G4Track& aPhotonTrk );

extern G4Track* RichPEInfoAttach(const G4Track& aPhotonTk, G4Track* aPETk, const G4ThreeVector & aLocalElectronOrigin);
extern G4Track* RichPEBackScatAttach(const G4Track& aElectronTk, G4Track* aPEBackScatTk);


#endif //GaussRICH_RICHPEINFOATTACH

