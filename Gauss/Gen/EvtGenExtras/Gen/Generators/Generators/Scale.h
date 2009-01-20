// $Id: Scale.h,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/10/06 14:11:16  ibelyaev
//  add (Read,Write)HepMCAsciiFile components
//
// ============================================================================
#ifndef GENERATORS_SCALE_H 
#define GENERATORS_SCALE_H 1
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenEvent.h"
// ============================================================================

namespace GeneratorUtils 
{
  /** @fn scale 
   *  Simple functon to rescale HepMCEvent 
   *  in between Pythina and LHC units 
   *  @param event generator event
   *  @param mom   scale factor for 4-momentum 
   *  @param time  scale factor for time component of the position vector  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-05
   */
  void scale
  ( HepMC::GenEvent* event , 
    const double     mom   , 
    const double     time  ) ;
  
} // end of namespace GeneratorUtils  


// ============================================================================
// The END 
// ============================================================================
#endif // GENERATORS_SCALE_H
// ============================================================================
