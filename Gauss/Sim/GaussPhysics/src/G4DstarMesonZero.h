// $Id: G4DstarMesonZero.h,v 1.2 2006-01-09 20:52:22 robbep Exp $

#ifndef G4DstarMesonZero_h
#define G4DstarMesonZero_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         DstarMesonZero                     ###
// ######################################################################

class G4DstarMesonZero : public G4ParticleDefinition
{
 private:
  static G4DstarMesonZero * theInstance ;
  G4DstarMesonZero( ) { }
  ~G4DstarMesonZero( ) { }


 public:
  static G4DstarMesonZero * Definition() ;
  static G4DstarMesonZero * DstarMesonZeroDefinition() ;
  static G4DstarMesonZero * DstarMesonZero() ;
};


#endif
