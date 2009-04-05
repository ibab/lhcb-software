// $Id: MinEkineCuts.h,v 1.2 2009-04-05 17:35:00 gcorti Exp $
#ifndef GAUSSTOOLS_COMPONENTS_MINEKINECUTS_H
#define GAUSSTOOLS_COMPONENTS_MINEKINECUTS_H 1

// Include files
#include "G4ios.hh"
#include "globals.hh"
#include "SpecialCuts.h"

/** @class LoopCuts LoopCuts.h Components/LoopCuts.h
 * 
 *
 *  @author Witek POKORSKI
 *  @date   2003-04-11
 */

namespace GiGa 
{
  class MinEkineCuts : public SpecialCuts
  {
  public:     

    /// Constructor
    MinEkineCuts(const G4String& processName ="MinEkineCuts", 
                 double m_cut=DBL_MAX);

    virtual ~MinEkineCuts(); ///< Destructor

    /// Implementation of G4VProcess method for post step 
    virtual G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
                             G4double   previousStepSize,
                             G4ForceCondition* condition );
            
    
  private:

    double m_cut;
  
    // Assignment operator as private 
    MinEkineCuts(MinEkineCuts&);
    MinEkineCuts& operator=(const MinEkineCuts& right);

  };

} // closes namespace

#endif

