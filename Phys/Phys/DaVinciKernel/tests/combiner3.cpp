// $Id:% 
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <memory>
#include <iostream>
#include <cstdlib>
#include <chrono>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ChronoEntity.h"
// ============================================================================
// Phys/Event
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// local 
// ============================================================================
#include "Combine.h"
#include "setup.h"
// ============================================================================
/** @file 
 *  Test for combiner strategy 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-04-02
 *                    $Revision: 186307 $
 *  Last modification $Date: 2015-04-02 19:55:18 +0200 (Thu, 02 Apr 2015) $
 *                 by $Author: ibelyaev $
 */
// ============================================================================

int main() 
{
  //
  auto t_start = std::chrono::high_resolution_clock::now();
  //
  LHCb::Particle p ( LHCb::ParticleID ( 100 ) ) ;
  LHCb::Vertex   v ;
  //
  for ( unsigned long long i = 0 ; i < N ; ++i )
  {
    //
    StatusCode sc = Combine::combine ( p , v ) ;
    if ( sc.isFailure() ) { continue ; }
    //
    if ( rand() < RAND_MAX * accept ) 
    {
      //
      LHCb::Particle* pnew = p.clone() ;
      LHCb::Vertex*   vnew = v.clone() ;
      pnew->setEndVertex ( vnew ) ;
      //
      sc = Combine::store ( pnew ) ; 
      sc = Combine::store ( vnew ) ;      
    }
    //
  }
  //
  auto t_stop = std::chrono::high_resolution_clock::now();
  //
  std::cout 
    << "Fraction:" << accept << ", "
    << "Wall clock time passed: "
    << std::chrono::duration<double, std::milli>(t_stop-t_start).count()
    << " ms\n";
  //
  exit(0) ;
}
// ============================================================================
// The END 
// ============================================================================
