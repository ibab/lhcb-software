// $Id: G4Neutralino.h,v 1.3 2008-06-10 17:07:49 gcorti Exp $

#ifndef G4Neutralino_h
#define G4Neutralino_h 1 

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         Neutralino                             ###
// ######################################################################

class G4Neutralino : public G4ParticleDefinition
{
 private:
  static G4Neutralino * theInstance ;
  G4Neutralino( ) { }
  ~G4Neutralino( ) { }

 public:
   static G4Neutralino* Definition();
   static G4Neutralino* NeutralinoDefinition();
   static G4Neutralino* Neutralino();
};

#endif

