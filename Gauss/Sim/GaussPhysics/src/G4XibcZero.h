// $

#ifndef G4XibcZero_h
#define G4XibcZero_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         XibcZero                        ###
// ######################################################################

class G4XibcZero : public G4ParticleDefinition
{
 private:
  static G4XibcZero * theInstance ;
  G4XibcZero( ) { }
  ~G4XibcZero( ) { }


 public:
  static G4XibcZero * Definition() ;
  static G4XibcZero * XibcZeroDefinition() ;
  static G4XibcZero * XibcZero() ;
};


#endif
