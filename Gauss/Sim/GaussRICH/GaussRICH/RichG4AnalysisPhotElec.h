// $Id: RichG4AnalysisPhotElec.h,v 1.3 2009-07-03 11:59:49 seaso Exp $
#ifndef GAUSSRICH_RICHG4ANALYSISPHOTELEC_H
#define GAUSSRICH_RICHG4ANALYSISPHOTELEC_H 1

// Include files
#include "globals.hh"
#include "G4Step.hh"


//extern void RichG4AnalysisPhotElecA ( const G4Step& aStep,
//                                      G4int currentRichDetNumber,
//                                      G4int currentHpdNumber, G4double PhotonEnergy);

//extern void RichG4AnalysisPhotElecB ( const G4Step& aStep,
//                                      G4int currentRichDetNumber,
//                                      G4int currentHpdNumber, G4double PhotonEnergy);

extern void RichG4AnalysisPhotElecA ( const G4Step& aStep,
                                      G4int currentRichDetNumber);

extern void RichG4AnalysisPhotElecB ( const G4Step& aStep,
                                      G4int currentRichDetNumber);

#endif // GAUSSRICH_RICHG4ANALYSISPHOTELEC_H
