// $

#ifndef G4XibZero_h
#define G4XibZero_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         XibZero                        ###
// ######################################################################

class G4XibZero : public G4ParticleDefinition
{
 private:
  static G4XibZero * theInstance ;
  G4XibZero( ) { }
  ~G4XibZero( ) { }


 public:
  static G4XibZero * Definition() ;
  static G4XibZero * XibZeroDefinition() ;
  static G4XibZero * XibZero() ;
};


#endif
