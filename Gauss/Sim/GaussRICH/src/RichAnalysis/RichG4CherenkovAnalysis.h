#include "RichG4AnalysisConstGauss.h"


extern void RichG4CherenkovAnalysis1(const G4Step& aStep, 
G4double CosThCkv, G4double SinThCkv, G4double  PhotProdCkvKE,
 G4ThreeVector PhotProdPosition, G4double RefInd, G4double BetaInvChPart );
extern void RichG4CherenkovAnalysis2(const G4Step& aStep);

