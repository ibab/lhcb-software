// $Id: G4LambdabPlus.h,v 1.1 2004-04-29 14:57:47 gcorti Exp $

#ifndef G4LambdabPlus_h
#define G4LambdabPlus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4VBaryon.hh"

// ######################################################################
// ###                          LambdabPlus                           ###
// ######################################################################

class G4LambdabPlus : public G4VBaryon
{
 private:
   static G4LambdabPlus theLambdabPlus;

 private:
   G4LambdabPlus(
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
   virtual ~G4LambdabPlus(){}

   static G4LambdabPlus* LambdabPlusDefinition();
   static G4LambdabPlus* LambdabPlus();
};

#endif
