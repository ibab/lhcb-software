// $Id: G4CharginoPlus.h,v 1.1 2008-11-27 16:02:08 robbep Exp $

#ifndef G4CharginoPlus_h
#define G4CharginoPlus_h 1 

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

/** @class  G4CharginoPlus G4CharginoPlus.h
 *
 *  Define the Chargino plus in Geant
 *
 *  @author Neal Gueissaz
 *  @date   2008-09-12
 */

// ######################################################################
// ###                       CharginoPlus                             ###
// ######################################################################

class G4CharginoPlus : public G4ParticleDefinition
{
 private:
  static G4CharginoPlus * theInstance ;
  G4CharginoPlus( ) { }
  ~G4CharginoPlus( ) { }

 public:
   static G4CharginoPlus* Definition();
   static G4CharginoPlus* CharginoPlusDefinition();
   static G4CharginoPlus* CharginoPlus();
};

#endif

