// $Id: G4DsstarMesonMinus.h,v 1.2 2006-01-09 20:52:22 robbep Exp $

#ifndef G4DsstarMesonMinus_h
#define G4DsstarMesonMinus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         DsstarMesonMinus                        ###
// ######################################################################

class G4DsstarMesonMinus : public G4ParticleDefinition
{
 private:
  static G4DsstarMesonMinus * theInstance ;
  G4DsstarMesonMinus( ) { }
  ~G4DsstarMesonMinus( ) { }


 public:
  static G4DsstarMesonMinus * Definition() ;
  static G4DsstarMesonMinus * DsstarMesonMinusDefinition() ;
  static G4DsstarMesonMinus * DsstarMesonMinus() ;
};


#endif
