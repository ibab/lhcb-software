// $Id: G4AntiDstarMesonZero.h,v 1.1 2004-04-29 14:57:46 gcorti Exp $

#ifndef G4AntiDstarMesonZero_h
#define G4AntiDstarMesonZero_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4VMeson.hh"

// ######################################################################
// ###                         AntiDstarMesonZero                     ###
// ######################################################################

class G4AntiDstarMesonZero : public G4VMeson
{
 private:
   static G4AntiDstarMesonZero theAntiDstarMesonZero;

 private: // constructors are hide as private  
   G4AntiDstarMesonZero(
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
   virtual ~G4AntiDstarMesonZero() {}

   static G4AntiDstarMesonZero* AntiDstarMesonZeroDefinition();
   static G4AntiDstarMesonZero* AntiDstarMesonZero();

};


#endif
