// $Id: createGaussTrajectory.cpp,v 1.1 2004-02-20 19:35:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// ============================================================================
// G4
// ============================================================================
#include "G4Track.hh"
// ============================================================================
// GaussTools
// ============================================================================
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/createGaussTrajectory.h"
#include "GaussTools/OscillationFlag.h"
// ============================================================================

/** @file 
 *  Implementation file for function createGaussTrajectory
 *  
 *  @date 2004-02-20 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
/** @fn createGaussTrajectory 
 *  
 *  create the GaussTrajectory from G4Track , 
 *   set the oscillation flag 
 *
 *  @param track track 
 *  @return the valid trajectory 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-19
 */
// ============================================================================
GaussTrajectory* createGaussTrajectory ( const G4Track* track ) 
{
  if ( 0 == track ) { return 0 ; }
  GaussTrajectory* trajectory = new GaussTrajectory ( track ) ;
  // set oscillation flag 
  OscillationFlag( track , trajectory ) ;
  return trajectory ;
}


// ============================================================================
// The END 
// ============================================================================
