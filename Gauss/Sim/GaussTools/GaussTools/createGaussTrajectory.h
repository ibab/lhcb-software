// $Id: createGaussTrajectory.h,v 1.1 2004-02-20 19:35:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GAUSSTOOLS_CREATEGAUSSTRAJECTORY_H 
#define GAUSSTOOLS_CREATEGAUSSTRAJECTORY_H 1

class GaussTrajectory ;
class G4Track         ;

/** @fn createGaussTrajectory 
 *  
 *  create the GaussTrajectory fotm G4Track , 
 *   set the oscillation flag 
 *
 *  @param track track 
 *  @return the valid trajectory 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-19
 */
GaussTrajectory* createGaussTrajectory ( const G4Track* track ) ;

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_CREATEGAUSSTRAJECTORY_H
// ============================================================================
