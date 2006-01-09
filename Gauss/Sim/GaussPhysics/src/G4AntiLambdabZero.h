// $Id: G4AntiLambdabZero.h,v 1.1 2006-01-09 20:52:07 robbep Exp $

#ifndef G4AntiLambdabZero_h
#define G4AntiLambdabZero_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         AntiLambdabZero                        ###
// ######################################################################

class G4AntiLambdabZero : public G4ParticleDefinition
{
 private:
  static G4AntiLambdabZero * theInstance ;
  G4AntiLambdabZero( ) { }
  ~G4AntiLambdabZero( ) { }


 public:
  static G4AntiLambdabZero * Definition() ;
  static G4AntiLambdabZero * AntiLambdabZeroDefinition() ;
  static G4AntiLambdabZero * AntiLambdabZero() ;
};


#endif
