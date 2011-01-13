// $Id: GenOscillated.h,v 1.1 2008-07-09 16:19:16 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GENOSCILLATED_H 
#define LOKI_GENOSCILLATED_H 1
// ============================================================================
// Include files
// ============================================================================
namespace LoKi 
{
  namespace GenParticles 
  {  
    // ========================================================================
    /** check the oscillation for the particle 
     *  @param p the particle 
     *  @return the oscillated mother particle 
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-03 
     */
    inline const HepMC::GenParticle* 
    oscillated1 ( const HepMC::GenParticle* p )
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
    // ========================================================================
    /** check the oscillation for the particle 
     *  @param p the particle 
     *  @return the oscillated daughter particle 
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-03 
     */
    inline const HepMC::GenParticle* 
    oscillated2  ( const HepMC::GenParticle* p )
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
  } // end of namespace LoKi::GenParticles 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENOSCILLATED_H
// ============================================================================
