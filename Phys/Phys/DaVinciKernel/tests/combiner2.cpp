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
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================

int main() 
{
  //
  auto t_start = std::chrono::high_resolution_clock::now();
  //
  for ( unsigned long long i = 0 ; i < N ; ++i )
  {
    
    std::unique_ptr<LHCb::Particle> p ( new LHCb::Particle( LHCb::ParticleID ( i % 10000 ) ) ) ;
    std::unique_ptr<LHCb::Vertex>   v ( new LHCb::Vertex  () ) ;
    //
    StatusCode sc = Combine::combine ( *p , *v ) ;
    if ( sc.isFailure() ) { continue ; }
    //
    if ( rand() < RAND_MAX * accept ) 
    {
      //
      //
      sc = Combine::store ( p.release() ) ; 
      sc = Combine::store ( v.release() ) ;      
    }
    //
  }
  //
  auto t_stop = std::chrono::high_resolution_clock::now();
  //
  std::cout 
    << "Wall clock time passed: "
    << std::chrono::duration<double, std::milli>(t_stop-t_start).count()
    << " ms\n";
  //
  exit(0) ;
}
// ============================================================================
// The END 
// ============================================================================
