// $Id: SpecialCuts.h,v 1.2 2009-04-05 17:33:59 gcorti Exp $
#ifndef GAUSSTOOLS_COMPONENTS_SPECIALCUTS_H
#define GAUSSTOOLS_COMPONENTS_SPECIALCUTS_H 1

// Include files
#include "G4ios.hh"
#include "globals.hh"
#include "G4VProcess.hh"

/** @class SpecialCuts SpecialCuts.h Components/SpecialCuts.h
 *
 *  "Base class" for user cuts to stop particles, originally named
 *  SpecialCuts. Wrapped in namespace as SpecialCut is also in 
 *  G4physics_list with exactly the same name and doing the same 
 *  as revision 1.6 (fStopButAlive instead fStopAndKill) was doing.  
 *
 *  @author Witold POKORSKI
 *  @date 2003-04-11
 *  @author revised by Gloria CORTI 
 *  @date 2009-03-26
 */

namespace GiGa {

class SpecialCuts : public G4VProcess {
  public:

  /// User process constructor
  SpecialCuts(const G4String& processName ="SpecialCuts" );

  virtual ~SpecialCuts( ); ///< Destructor

  /// Implementation of G4VProcess virtual method
  virtual G4double PostStepGetPhysicalInteractionLength(
                 const G4Track& track,
                 G4double   previousStepSize,
                 G4ForceCondition* condition
                 );

  /// Implementation of G4VProcess virtual method
  virtual G4VParticleChange* PostStepDoIt( const G4Track& ,
                                           const G4Step& );
  
  /// Implementation of G4VProcess virtual method: no operation
  virtual G4double AtRestGetPhysicalInteractionLength( const G4Track& ,
                                                       G4ForceCondition* ){
    return -1.0; 
  };
			    
  /// Implementation of G4VProcess virtual method: no operation     
  virtual G4VParticleChange* AtRestDoIt( const G4Track& , const G4Step& ){
    return NULL;
  };

  /// Implementation of G4VProcess virtual method: no operation     
  virtual G4double AlongStepGetPhysicalInteractionLength( const G4Track&,
                                                          G4double  ,
                                                          G4double  ,
                                                          G4double& ,
                                                          G4GPILSelection* ){
    return -1.0; 
  };

  /// Implementation of G4VProcess virtual method: no operation     
  virtual G4VParticleChange* AlongStepDoIt( const G4Track& ,
                                            const G4Step& ) {
    return NULL;
  };

private:
  
  /// Hide assignment operator as private 
  SpecialCuts& operator=(const SpecialCuts&){return *this;};

};

} // close namespace

#endif // GAUSSTOOLS_COMPONENTS_SPECIALCUTS_H


