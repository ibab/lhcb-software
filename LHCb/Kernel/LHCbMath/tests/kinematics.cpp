// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// LHCbMath 
// ============================================================================
#include "LHCbMath/Kinematics.h"
#include "LHCbMath/MatrixUtils.h"
// ============================================================================
/** @file
 *  Test file for LHcbMatyh/Kinematics.h
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-12-02 
 */
// ============================================================================
int main()
{
  
  Gaudi::LorentzVector lv ( 2 , 2 , 20 , 35 ) ;
  lv *= Gaudi::Units::GeV ;


  Gaudi::SymMatrix4x4  cov ;
  Gaudi::Math::setToUnit ( cov , 0.1*0.1*Gaudi::Units::GeV*Gaudi::Units::GeV );
  
  std::cout 
    << " sigma2mass2  : " << Gaudi::Math::sigma2mass2 ( lv , cov ) << std::endl 
    << " sigma2mass   : " << Gaudi::Math::sigma2mass  ( lv , cov ) << std::endl 
    << " sigmamass    : " << Gaudi::Math::sigmamass   ( lv , cov ) << std::endl 
    << " chi2mass     : " << Gaudi::Math::chi2mass    ( 5 * Gaudi::Units::GeV ,
                                                        lv , cov ) << std::endl ;

  exit(0) ;
}
// ============================================================================
// The END 
// ============================================================================
