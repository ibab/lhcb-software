#ifndef      GaussRICH_RichG4CherenkovPhotProdTag_h
#define      GaussRICH_RichG4CherenkovPhotProdTag_h 1 
#include "G4Track.hh"
extern G4Track* RichG4CherenkovPhotProdTag(const G4Track& aChTrack, 
                                  G4Track* aCkvPhotTrack,
                                  const G4double CkvCosTheta,
                                  const G4double CkvPhi,
                                  const G4double CkvPhotEnergy,
                                   const G4bool aVerboseTagFlag,
                                   const G4int aPhotoProdTag  );
#endif //GaussRICH_RichG4CherenkovPhotProdTag_h

