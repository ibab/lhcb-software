// $Id: RichG4AnalysisPhotElec.h,v 1.2 2004-02-10 14:24:07 jonesc Exp $
#ifndef RICHANALYSIS_RICHG4ANALYSISPHOTELEC_H
#define RICHANALYSIS_RICHG4ANALYSISPHOTELEC_H 1

// Include files
#include "globals.hh"
#include "G4Step.hh"


extern void RichG4AnalysisPhotElecA ( const G4Step& aStep,
                                      G4int currentRichDetNumber,
                                      G4int currentHpdNumber, G4double PhotonEnergy);

extern void RichG4AnalysisPhotElecB ( const G4Step& aStep,
                                      G4int currentRichDetNumber,
                                      G4int currentHpdNumber, G4double PhotonEnergy);


#endif // RICHANALYSIS_RICHG4ANALYSISPHOTELEC_H
