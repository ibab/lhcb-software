// $Id: LoopCuts.h,v 1.5 2009-04-05 17:31:53 gcorti Exp $
#ifndef GAUSSTOOLS_COMPONENTS_LOOPCUTS_H
#define GAUSSTOOLS_COMPONENTS_LOOPCUTS_H 1

// Include files
#include "G4ios.hh"
#include "globals.hh"
#include "SpecialCuts.h"

/** @class LoopCuts LoopCuts.h Components/LoopCuts.h
 * 
 *
 *  @author Witek POKORSKI
 *  @date   2003-06-17
 */
namespace GiGa 
{
  
class LoopCuts : public SpecialCuts {
public:     

  /// Constructor
  /// @param processName Name assigned to process
  /// @param maxs        Number of setps to define a looping track
  /// @param minstep     Mimimum step lenght in a loop
  LoopCuts(const G4String& processName ="LoopCuts",
           int maxs=100, double minstep=0.001);

  virtual ~LoopCuts(); ///< Destructor

  /// Implementation of G4VProcess method for post step 
  virtual G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
                             G4double   previousStepSize,
                             G4ForceCondition* condition );
            
  /// Implementation of G4VProcess method for post step 
  virtual G4VParticleChange* PostStepDoIt( const G4Track& ,
                                           const G4Step& );            
  
private:

  int m_counter;          ///< Internal counter to check looping
  int m_maxstepnumber;    ///< Maximum number of step to define loop
  double m_minstep;       ///< Mimimum step lenght in a loop
  
  // Assignment operator as private 
  LoopCuts(LoopCuts&);
  LoopCuts& operator=(const LoopCuts& right);

};

} // closes namespace

#endif // GAUSSTOOLS_COMPONENTS_LOOPCUTS_H

