// $Id: G4Etac.h,v 1.1 2004-04-29 14:57:47 gcorti Exp $

// Each class inheriting from G4VMeson
// corresponds to a particle type; one and only one
// instance for each class is guaranteed.

#ifndef G4Etac_h
#define G4Etac_h 1 

#include "globals.hh"
#include "G4ios.hh"
#include "G4VMeson.hh"

// ######################################################################
// ###                         Etac                             ###
// ######################################################################

class G4Etac : public G4VMeson
{
 private:
   static G4Etac theEtac;

 private: // constructors are hide as private  
   G4Etac(
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
   virtual ~G4Etac(){}

   static G4Etac* EtacDefinition();
   static G4Etac* Etac();

};

#endif

