
#ifndef G4Neutralino2_h
#define G4Neutralino2_h 1 

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

/** @class  G4Neutralino2 G4Neutralino2.h
 *
 *  Define the Neutralino2 in Geant
 *
 *  @author Neal Gueissaz
 *  @date   2008-09-12
 */

// ######################################################################
// ###                         Neutralino2                             ###
// ######################################################################

class G4Neutralino2 : public G4ParticleDefinition
{
 private:
  static G4Neutralino2 * theInstance ;
  G4Neutralino2( ) { }
  ~G4Neutralino2( ) { }

 public:
   static G4Neutralino2* Definition();
   static G4Neutralino2* Neutralino2Definition();
   static G4Neutralino2* Neutralino2();
};

#endif

