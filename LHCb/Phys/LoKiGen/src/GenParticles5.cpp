// $Id$
// ============================================================================
// Include files
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Geometry.h"
#include "LoKi/GenParticles5.h"
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
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2013-04-14 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
/** constructor from one 
 *  @param primary use primary vertex, otherwise use own vertex 
 */
// ============================================================================
LoKi::GenParticles::Flight::Flight ( const bool primary ) 
  : LoKi::GenTypes::GFunc () 
  , m_primary ( primary ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::GenParticles::Flight::~Flight() {}
// ============================================================================
// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::GenParticles::Flight*
LoKi::GenParticles::Flight::clone() const { return new Flight(*this) ; }
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::GenParticles::Flight::fillStream ( std::ostream& s ) const
{ return s << ( m_primary ? "GPVFLIGHT" : "GFLIGHT" ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::GenParticles::Flight::result_type
LoKi::GenParticles::Flight::operator() 
  ( LoKi::GenParticles::Flight::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "HepMC::GenParticle* points to NULL, return -1*km ") ;
    return -1 * Gaudi::Units::km ;
  }
  // end vertex 
  const HepMC::GenVertex* ev = p->end_vertex  () ;
  //
  if ( 0 == ev ) 
  {
    Error ( "HepMC::GenParticle::end_vertex points to NULL, return +1*km " ) ;
    return  Gaudi::Units::km ;
  }
  //
  const HepMC::GenVertex* v0 = 0 ;
  if ( primary() ) 
  {
    //
    const HepMC::GenEvent* evt = p->parent_event() ;
    if ( 0 == evt ) 
    {
      Error ( "HepMC::GenParticle::parent_event points to NULL, return -1*km " ) ;
      return -1 * Gaudi::Units::km ;
    }
    //
    v0 = evt->signal_process_vertex() ;
    if ( 0 == v0 ) 
    {
      Error ( "HepMC::GenEvent::signal_process_vertex points to NULL, return -1*km " ) ;
      return -1 * Gaudi::Units::km ;
    }
    //
  }
  else 
  {
    //
    v0 = p->production_vertex() ;
    if ( 0 == v0 ) 
    {
      Error ( "HepMC::GenParticle::production_vertex points to NULL, return -1*km " ) ;
      return -1 * Gaudi::Units::km ;
    }
    //
  }
  //
  // calculate the distance 
  if ( ev == v0 ) { return 0 ; }
  //
  const LoKi::Point3D ed  ( ev -> point3d () ) ;
  const LoKi::Point3D eo  ( v0 -> point3d () ) ;
  //
  return ( ed - eo ).R () ;
}
// ============================================================================
// The END 
// ============================================================================
