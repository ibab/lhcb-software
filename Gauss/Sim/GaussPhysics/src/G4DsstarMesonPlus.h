// $Id: G4DsstarMesonPlus.h,v 1.1 2004-04-29 14:57:46 gcorti Exp $

#ifndef G4DsstarMesonPlus_h
#define G4DsstarMesonPlus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4VMeson.hh"

// ######################################################################
// ###                         DsstarMesonPlus                             ###
// ######################################################################

class G4DsstarMesonPlus : public G4VMeson
{
 private:
   static G4DsstarMesonPlus theDsstarMesonPlus;

 private: // constructors are hide as private  
   G4DsstarMesonPlus(
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
  virtual ~G4DsstarMesonPlus(){}

   static G4DsstarMesonPlus* DsstarMesonPlusDefinition();
   static G4DsstarMesonPlus* DsstarMesonPlus();
};

#endif
