// $Id: G4AntiXibZero.h,v 1.1 2009-01-07 14:04:24 robbep Exp $

#ifndef G4AntiXibZero_h
#define G4AntiXibZero_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         AntiXibZero                        ###
// ######################################################################

class G4AntiXibZero : public G4ParticleDefinition
{
 private:
  static G4AntiXibZero * theInstance ;
  G4AntiXibZero( ) { }
  ~G4AntiXibZero( ) { }


 public:
  static G4AntiXibZero * Definition() ;
  static G4AntiXibZero * AntiXibZeroDefinition() ;
  static G4AntiXibZero * AntiXibZero() ;
};


#endif
