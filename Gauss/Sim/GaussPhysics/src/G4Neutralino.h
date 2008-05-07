// $Id: G4Neutralino.h,v 1.1 2008-05-07 08:34:30 gcorti Exp $

// Each class inheriting from G4VMeson
// corresponds to a particle type; one and only one
// instance for each class is guaranteed.

#ifndef G4Neutralino_h
#define G4Neutralino_h 1 

#include "globals.hh"
#include "G4ios.hh"
#include "G4VMeson.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         Neutralino                             ###
// ######################################################################

class G4Neutralino : public G4ParticleDefinition//public G4VMeson
{
 private:
   static G4Neutralino theNeutralino;

 private: // constructors are hide as private  
   G4Neutralino(
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
   virtual ~G4Neutralino(){}

   static G4Neutralino* NeutralinoDefinition();
   static G4Neutralino* Neutralino();

};

#endif

