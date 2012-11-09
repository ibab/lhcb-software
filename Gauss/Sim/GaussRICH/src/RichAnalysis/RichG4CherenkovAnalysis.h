#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"

//extern void RichG4CherenkovAnalysis1(const G4Step& aStep,
//                                     G4double CosThCkv, G4double SinThCkv, 
//                                     G4double  PhotProdCkvKE,
//                                     const G4ThreeVector & PhotProdPosition, 
//                                     G4double RefInd, G4double BetaInvChPart );

extern void RichG4CherenkovAnalysis1(const G4Step& aStep,
                                     G4double CosThCkv, 
                                     G4double  PhotProdCkvKE,
                                     const G4ThreeVector & PhotProdPosition, 
                                     G4double RefInd, G4double BetaInvChPart );

extern void RichG4CherenkovAnalysis2(const G4Step& aStep);

extern void RichG4CherenkovProdFeaturesHisto(const G4Track& aChTrack); 
