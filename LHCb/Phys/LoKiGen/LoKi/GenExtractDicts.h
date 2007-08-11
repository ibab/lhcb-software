// $Id: GenExtractDicts.h,v 1.3 2007-08-11 20:18:13 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GENEXTRACTDICTS_H 
#define LOKI_GENEXTRACTDICTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenExtract.h"
#include "LoKi/GenTypes.h"
// ============================================================================
namespace LoKi 
{
  namespace Dicts 
  { 
    namespace Extract 
    {
      // ======================================================================
      /// extract the particles from the event 
      std::vector<const HepMC::GenParticle*>
      genParticles 
      ( const HepMC::GenEvent*       event , 
        const LoKi::GenTypes::GCuts& cuts  ) ;
      // ======================================================================
      /// extract the particles from the event 
      std::vector<const HepMC::GenParticle*>
      genParticles 
      ( const LHCb::HepMCEvent*      event , 
        const LoKi::GenTypes::GCuts& cuts  ) ;
      // ======================================================================
      /// extract the particles from the event 
      std::vector<const HepMC::GenParticle*>
      genParticles 
      ( const LHCb::HepMCEvents*     event , 
        const LoKi::GenTypes::GCuts& cuts  ) ;
      // ======================================================================
      /// extract the particles from the vertex 
      std::vector<const HepMC::GenParticle*>
      genParticles 
      ( const HepMC::GenVertex*      veretx ,
        const LoKi::GenTypes::GCuts& cuts   , 
        HepMC::IteratorRange         range = HepMC::descendants ) ;
      // ======================================================================
      /// extract the particles from the vertex 
      std::vector<const HepMC::GenParticle*>
      genParticles 
      ( const HepMC::GenVertex*      veretx ,
        HepMC::IteratorRange         range = HepMC::descendants ) ;
      // ======================================================================
      /// extract the particles from the particle 
      std::vector<const HepMC::GenParticle*>
      genParticles 
      ( const HepMC::GenParticle*    particle ,
        const LoKi::GenTypes::GCuts& cuts     ) ;
      // ======================================================================
    } // end of namespace LoKi::Dicts::Extract
  } // end of namespace LoKi::Dicts 
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENEXTRACTDICTS_H
// ============================================================================
