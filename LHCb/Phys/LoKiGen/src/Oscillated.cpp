// $Id$
// ============================================================================
// Include files 
// ============================================================================
// HepMC
// ============================================================================
#include "HepMC/GenEvent.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/GenOscillated.h"
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
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-11-11
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ===========================================================================
/* check the oscillation for the particle 
 *  @param p the particle 
 *  @return the oscillated mother particle 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-06-03 
 */
// ============================================================================
const HepMC::GenParticle* 
LoKi::GenParticles::oscillated1 ( const HepMC::GenParticle* p )
{
  if ( 0 == p  ) { return 0 ; }
  const HepMC::GenVertex* pv = p->production_vertex () ;
  if ( 0 == pv ) { return 0 ; }
  if ( 1 != pv->particles_in_size() ) { return 0 ; }
  const HepMC::GenParticle* m = *(pv->particles_in_const_begin()) ;
  if ( 0 == m  ) { return 0; }
  // oscillated ? 
  if ( p->pdg_id() != -m->pdg_id() ) { return 0 ; }
  //
  return m ;
}
// ============================================================================
/*  check the oscillation for the particle 
 *  @param p the particle 
 *  @return the oscillated daughter particle 
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-06-03 
 */
const HepMC::GenParticle* 
LoKi::GenParticles::oscillated2  ( const HepMC::GenParticle* p )
{
  if ( 0 == p  ) { return 0 ; }
  const HepMC::GenVertex* ev = p->end_vertex () ;
  if ( 0 == ev ) { return 0 ; }
  if ( 1 != ev->particles_out_size() ) { return 0 ; }
  const HepMC::GenParticle* d = *(ev->particles_out_const_begin()) ;
  if ( 0 == d  ) { return 0; }
  // oscillated ? 
  if ( p->pdg_id() != -d->pdg_id() ) { return 0 ; }
  //
  return d ;
}
// ==========================================================================
// The END 
// ==========================================================================
