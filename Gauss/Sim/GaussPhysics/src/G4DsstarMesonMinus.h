// $Id: G4DsstarMesonMinus.h,v 1.1 2004-04-29 14:57:46 gcorti Exp $

#ifndef G4DsstarMesonMinus_h
#define G4DsstarMesonMinus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4VMeson.hh"

// ######################################################################
// ###                       DsstarMesonMinus                         ###
// ######################################################################

class G4DsstarMesonMinus : public G4VMeson
{
 private:
   static G4DsstarMesonMinus theDsstarMesonMinus;

 private: // constructors are hide as private  
   G4DsstarMesonMinus(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,   
       G4int               iSpin,        G4int               iParity,    
       G4int               iConjugation, G4int               iIsospin,   
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,      
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable
   );

 public:
  virtual ~G4DsstarMesonMinus(){}

   static G4DsstarMesonMinus* DsstarMesonMinusDefinition();
   static G4DsstarMesonMinus* DsstarMesonMinus();
};

#endif
