// $Id: G4AntiDstarMesonZero.h,v 1.2 2006-01-09 20:52:22 robbep Exp $

#ifndef G4AntiDstarMesonZero_h
#define G4AntiDstarMesonZero_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         AntiDstarMesonZero                     ###
// ######################################################################

class G4AntiDstarMesonZero : public G4ParticleDefinition
{
 private:
  static G4AntiDstarMesonZero * theInstance ;
  G4AntiDstarMesonZero( ) { }
  ~G4AntiDstarMesonZero( ) { }


 public:
  static G4AntiDstarMesonZero * Definition() ;
  static G4AntiDstarMesonZero * AntiDstarMesonZeroDefinition() ;
  static G4AntiDstarMesonZero * AntiDstarMesonZero() ;
};


#endif
