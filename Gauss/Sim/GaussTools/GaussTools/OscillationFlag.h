// $Id: OscillationFlag.h,v 1.1 2004-02-20 19:35:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GAUSSTOOLS_OSCILLATIONFLAG_H 
#define GAUSSTOOLS_OSCILLATIONFLAG_H 1
// ============================================================================
// Include files
// ============================================================================
class G4Track         ;
class GaussTrajectory ;
// ============================================================================

/** @fn OscillationFlag
 *
 *  Propagate the pscilaltion flag from 
 *   primary particle to the trajectory
 *
 *  @author Vanya BELYAEV
 *  @date   2004-02-19
 */
StatusCode OscillationFlag ( const G4Track*   track , 
                             GaussTrajectory* traj  ) ;

// ============================================================================
// The END 
// ============================================================================
#endif // COMPONENTS_OSCILLATIONFLAG_H
// ============================================================================

