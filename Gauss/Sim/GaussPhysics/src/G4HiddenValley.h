// $Id: G4HiddenValley.h,v 1.1 2009-10-20 07:31:43 marcin Exp $

#ifndef G4HiddenValley_h
#define G4HiddenValley_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         HiddenValley                        ###
// ######################################################################

class G4HiddenValley : public G4ParticleDefinition
{
 private:
  static G4HiddenValley * theInstance ;
  G4HiddenValley( ) { }
  ~G4HiddenValley( ) { }


 public:
  static G4HiddenValley * Definition() ;
  static G4HiddenValley * HiddenValleyDefinition() ;
  static G4HiddenValley * HiddenValley() ;
};


#endif
