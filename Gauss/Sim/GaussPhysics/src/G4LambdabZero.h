// $Id: G4LambdabZero.h,v 1.1 2006-01-09 20:52:07 robbep Exp $

#ifndef G4LambdabZero_h
#define G4LambdabZero_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         LambdabZero                        ###
// ######################################################################

class G4LambdabZero : public G4ParticleDefinition
{
 private:
  static G4LambdabZero * theInstance ;
  G4LambdabZero( ) { }
  ~G4LambdabZero( ) { }


 public:
  static G4LambdabZero * Definition() ;
  static G4LambdabZero * LambdabZeroDefinition() ;
  static G4LambdabZero * LambdabZero() ;
};


#endif
