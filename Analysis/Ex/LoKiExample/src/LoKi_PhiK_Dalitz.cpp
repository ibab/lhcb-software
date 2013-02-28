// $Id: LoKi_PhiK_Dalitz.cpp,v 1.7 2008-07-09 17:00:48 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AlgoMC.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/PhysKinematics.h"
// ============================================================================
/** @file
 *  Siple algorithm to make dalits plots from B+ -> pphi K+ gamma decays
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @see LoKi_Bs2PsiPhi.cpp
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date  2007-03-22
 */
LOKI_MCALGORITHM(PhiK_Dalitz)
{
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  using namespace LoKi::Kinematics ;
  //
  const StatusCode SUCCESS = StatusCode::SUCCESS ;
  
  Range kaons  = select ( "k" , "K+" == ABSID ) ;
  Range gamma  = select ( "g" , "gamma" == ABSID && PT > 1.5 * Gaudi::Units::GeV ) ;
 
  Tuple tuple = nTuple ( "Dalitz" ) ;
  
  for ( Loop B = loop( "k k k g" , "B+" ) ; B ; ++B )
  {
    const double m = B->mass() ;
    if ( 5.0 * Gaudi::Units::GeV > m ) { continue ; }
    if ( 5.6 * Gaudi::Units::GeV < m ) { continue ; }
    const double m123 = B->mass(1,2,3) ;
    if ( 2.0 * Gaudi::Units::GeV < m123 ) { continue ; }
    
    const double q1 = Q ( B(1) ) ;
    const double q2 = Q ( B(2) ) ;
    const double q3 = Q ( B(3) ) ;
    
    if ( 1 != ::fabs ( q1 + q2 + q3 )   ) { continue ; }
    
    
    double s1 = 0.0 ;
    double s2 = 0.0 ;
    
    if      ( 0 < q1*q2 && 0 > q1*q3 && 0 > q2*q3 ) 
    {
      s1 = mass ( B(1) , B(3) ) ;
      s2 = mass ( B(2) , B(3) ) ;
    }
    else if ( 0 < q1*q3 && 0 > q1*q2 && 0 > q3*q2 ) 
    {
      s1 = mass ( B(1) , B(2) ) ;
      s2 = mass ( B(3) , B(2) ) ;
    }
    else if ( 0 < q2*q3 && 0 > q2*q1 && 0 > q3*q1 ) 
    {
      s1 = mass ( B(2) , B(1) ) ;
      s2 = mass ( B(3) , B(1) ) ;
    }
    else 
    { Warning("Invalid charge combination!, skip!") ; continue ; }
    
    plot ( m / Gaudi::Units::GeV , " 3k gamma " , 4.0 , 6.0 ) ;
    
    s1 /= Gaudi::Units::GeV ;
    s1 = s1*s1 ;
    s2 /= Gaudi::Units::GeV ;
    s2 = s2*s2 ;
    
    tuple -> column ( "m"    , m                                  ) ;
    tuple -> column ( "m123" , m123                               ) ;
    tuple -> column ( "m12"  , B -> mass(1,2) / Gaudi::Units::GeV ) ;
    tuple -> column ( "m23"  , B -> mass(2,3) / Gaudi::Units::GeV ) ;
    tuple -> column ( "m31"  , B -> mass(3,1) / Gaudi::Units::GeV ) ;
    
    tuple -> column ( "s1" , s1 ) ;
    tuple -> column ( "s2" , s2 ) ;
    
    tuple -> write() ;
    
  }

  setFilterPassed ( true ) ;
  
  return SUCCESS ;
}

// ============================================================================
// The END 
// ============================================================================
