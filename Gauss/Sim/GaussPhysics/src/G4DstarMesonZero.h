// $Id: G4DstarMesonZero.h,v 1.1 2004-04-29 14:57:47 gcorti Exp $

#ifndef G4DstarMesonZero_h
#define G4DstarMesonZero_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4VMeson.hh"

// ######################################################################
// ###                         DstarMesonZero                         ###
// ######################################################################

class G4DstarMesonZero : public G4VMeson
{
 private:
   static G4DstarMesonZero theDstarMesonZero;

 private: // constructors are hide as private  
   G4DstarMesonZero(
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
   virtual ~G4DstarMesonZero(){}

   static G4DstarMesonZero* DstarMesonZeroDefinition();
   static G4DstarMesonZero* DstarMesonZero();

};

#endif
