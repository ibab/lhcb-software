// $Id: G4Neutralino.h,v 1.4 2008-11-27 16:01:37 robbep Exp $

#ifndef G4Neutralino_h
#define G4Neutralino_h 1 

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

/** @class  G4Neutralino G4Neutralino.h
 *
 *  Define the Neutralino LSP in Geant
 *
 *  @author Neal Gueissaz
 *  @date   2008-09-12
 */

// ######################################################################
// ###                         Neutralino                             ###
// ######################################################################

class G4Neutralino : public G4ParticleDefinition
{
 private:
  static G4Neutralino * theInstance ;
  G4Neutralino( ) { }
  ~G4Neutralino( ) { }

 public:
   static G4Neutralino* Definition();
   static G4Neutralino* NeutralinoDefinition();
   static G4Neutralino* Neutralino();
};

#endif

