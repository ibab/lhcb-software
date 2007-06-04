// $Id: PhysExtractDicts.h,v 1.1 2007-06-04 18:25:37 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PHYSEXTRACTDICTS_H 
#define LOKI_PHYSEXTRACTDICTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/PhysExtract.h"
#include "LoKi/PhysTypes.h"
// ============================================================================
namespace LoKi 
{
  namespace Dicts 
  {
    namespace Extract
    {
      /// extract from the tree 
      LHCb::Particle::ConstVector
      particles 
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Cuts& cut      ) ;      
      /// extract from the tree 
      LHCb::ProtoParticle::ConstVector
      protoParticles 
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Cuts& cut      ) ;
      /// extract from the tree 
      LHCb::Particle::ConstVector
      particles 
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Cuts& cut      ) ;      
      /// extract from the tree 
      LHCb::Track::ConstVector
      tracks 
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Cuts& cut      ) ;
    } // end of namespace LoKi::Dicts::Extract
  } // end of namespace LoKi::Dicts
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSEXTRACTDICTS_H
// ============================================================================
