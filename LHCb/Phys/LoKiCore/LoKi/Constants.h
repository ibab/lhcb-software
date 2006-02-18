// $Id: Constants.h,v 1.3 2006-02-18 18:06:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/02/17 19:12:53  ibelyaev
//  add Cast and some other minor changes
//
// Revision 1.1.1.1  2006/01/24 09:39:41  ibelyaev
// New Import: the core part of restructurized LoKi project
//
// ============================================================================
#ifndef LOKI_CONSTANTS_H 
#define LOKI_CONSTANTS_H 1
// ============================================================================
// include files 
// ============================================================================
// LHCbDEfinitions
// ============================================================================
#include "Kernel/SystemOfUnits.h"
#include "Kernel/PhysicalConstants.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace Constants Constants.h LoKi/Constants.h
   *
   *  Namespace  with the collection of useful constants
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-07-10
   */
  namespace Constants
  {
    
    /// representation of invalid time 
    const double InvalidTime         =  -1000 * CLHEP::ns ;
    /// representation of invalid/infinite time 
    const double InfiniteTime        = 1.e+10 * CLHEP::ns ;
    /// representation of invalid distance 
    const double InvalidDistance     =  -1000 * CLHEP::km ;
    /// representation of invalid chi2 
    const double InvalidChi2         =  -1000      ;
    /// representation of invalid particle ID 
    const double InvalidID           =   0         ;
    /// representation of invalid particle charge  
    const double InvalidCharge       =  -1000      ;
    /// representation of invalid PID 
    const double InvalidPID          =  -1000      ;
    /// representation of invalid confedence level   
    const double InvalidConfLevel    =  -1000      ;
    /// representation of invalid energy
    const double InvalidEnergy       =  -100 * CLHEP::TeV ;
    /// representation of invalid momentum
    const double InvalidMomentum     =  -100 * CLHEP::TeV ;
    /// representation of invalid mass 
    const double InvalidMass         =  -100 * CLHEP::TeV ;    
    /// representation of invalid sine/cosine/angle, etc 
    const double InvalidAngle        =  -1000      ;
    /// representation of HUGE chi2 
    const double HugeChi2            = 1.e+10      ;
    
  }; // end of namespace Constants
}; // end of namespace LoKi  


// ============================================================================
// The END
// ============================================================================
#endif // LOKI_CONSTANTS_H
// ============================================================================



