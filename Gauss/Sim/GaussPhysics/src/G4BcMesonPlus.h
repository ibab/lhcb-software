// $Id: G4BcMesonPlus.h,v 1.1 2004-04-29 14:57:46 gcorti Exp $

#ifndef G4BcMesonPlus_h
#define G4BcMesonPlus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4VMeson.hh"

// ######################################################################
// ###                         BcMesonPlus                            ###
// ######################################################################

class G4BcMesonPlus : public G4VMeson
{
 private:
   static G4BcMesonPlus theBcMesonPlus;

 private: // constructors are hide as private  
   G4BcMesonPlus(
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
  virtual ~G4BcMesonPlus(){}

   static G4BcMesonPlus* BcMesonPlusDefinition();
   static G4BcMesonPlus* BcMesonPlus();

};

#endif
