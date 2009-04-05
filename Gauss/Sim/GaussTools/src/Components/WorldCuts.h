// $Id: WorldCuts.h,v 1.2 2009-04-05 17:33:18 gcorti Exp $
#ifndef GAUSSTOOLS_COMPONENTS_WORLDCUTS_H
#define GAUSSTOOLS_COMPONENTS_WORLDCUTS_H 1

// Include files
#include "G4ios.hh"
#include "globals.hh"
#include "SpecialCuts.h"


/** @class LoopCuts LoopCuts.h Components/LoopCuts.h
 * 
 *
 *  @author Witek POKORSKI
 *  @date   2003-07-28
 */

namespace GiGa {
  
class WorldCuts : public SpecialCuts {
public:     

  /// Constructor
  WorldCuts(const G4String& processName ="WorldCuts",
            double xmin=-10000.0, double ymin=-10000.0, double zmin=-5000.0, 
            double xmax=10000.0, double ymax=10000.0, double zmax=25000.0);
  
  virtual ~WorldCuts(); ///< Destructor
  
  /// Implementation of G4VProcess method for post step 
  virtual G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
                             G4double   previousStepSize,
                             G4ForceCondition* condition );
  
private:
  
  double m_xmin;      ///< Stop transport for x coord less than this value
  double m_ymin;      ///< Stop transport for y coord less than this value
  double m_zmin;      ///< Stop transport for z coord less than this value
  double m_xmax;      ///< Stop transport for x coord greater than this value
  double m_ymax;      ///< Stop transport for y coord greater than this value
  double m_zmax;      ///< Stop transport for z coord greater than this value
  
  // Assignment operator as private 
  WorldCuts(WorldCuts&);
  WorldCuts& operator=(const WorldCuts& right);
  
};
 
} // closes namespace

#endif // GAUSSTOOLS_COMPONENTS_WORLDCUTS_H

