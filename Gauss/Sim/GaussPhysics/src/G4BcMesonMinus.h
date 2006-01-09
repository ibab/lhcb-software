// $Id: G4BcMesonMinus.h,v 1.2 2006-01-09 20:52:22 robbep Exp $

#ifndef G4BcMesonMinus_h
#define G4BcMesonMinus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         BcMesonMinus                        ###
// ######################################################################

class G4BcMesonMinus : public G4ParticleDefinition
{
 private:
  static G4BcMesonMinus * theInstance ;
  G4BcMesonMinus( ) { }
  ~G4BcMesonMinus( ) { }


 public:
  static G4BcMesonMinus * Definition() ;
  static G4BcMesonMinus * BcMesonMinusDefinition() ;
  static G4BcMesonMinus * BcMesonMinus() ;
};


#endif
