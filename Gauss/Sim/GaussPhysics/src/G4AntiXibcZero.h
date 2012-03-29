// $

#ifndef G4AntiXibcZero_h
#define G4AntiXibcZero_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         AntiXibcZero                        ###
// ######################################################################

class G4AntiXibcZero : public G4ParticleDefinition
{
 private:
  static G4AntiXibcZero * theInstance ;
  G4AntiXibcZero( ) { }
  ~G4AntiXibcZero( ) { }


 public:
  static G4AntiXibcZero * Definition() ;
  static G4AntiXibcZero * AntiXibcZeroDefinition() ;
  static G4AntiXibcZero * AntiXibcZero() ;
};


#endif
