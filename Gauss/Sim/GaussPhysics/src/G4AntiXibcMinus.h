// $

#ifndef G4AntiXibcMinus_h
#define G4AntiXibcMinus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         AntiXibcMinus                        ###
// ######################################################################

class G4AntiXibcMinus : public G4ParticleDefinition
{
 private:
  static G4AntiXibcMinus * theInstance ;
  G4AntiXibcMinus( ) { }
  ~G4AntiXibcMinus( ) { }


 public:
  static G4AntiXibcMinus * Definition() ;
  static G4AntiXibcMinus * AntiXibcMinusDefinition() ;
  static G4AntiXibcMinus * AntiXibcMinus() ;
};


#endif
