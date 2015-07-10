// $Id$
// ============================================================================
#ifndef LOKI_PHYSEXTRACTDICTS_H 
#define LOKI_PHYSEXTRACTDICTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/PhysExtract.h"
#include "LoKi/PhysTypes.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Dicts 
  {
    // ========================================================================
    namespace Extract
    {
      // ======================================================================
      /// extract from the tree 
      LHCb::Particle::ConstVector
      particles 
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Cuts& cut      ) ;      
      // ======================================================================
      /// extract from the tree 
      LHCb::ProtoParticle::ConstVector
      protoParticles 
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Cuts& cut      ) ;
      // ======================================================================
      /// extract from the tree 
      LHCb::Particle::ConstVector
      particles 
      ( const LHCb::Particle*        particle , 
        const LoKi::PhysTypes::Cuts& cut      ) ;      
      // ======================================================================
      /** get all tracks from the given particle 
       *  @param particle the particle 
       *  @return container of tracks 
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2015-07-10
       */
      LHCb::Track::ConstVector
      tracks ( const LHCb::Particle* particle ) ;
      // ======================================================================
      /** get all tracks from the given particle 
       *  @param particle the particle 
       *  @param pcut     the cut
       *  @return container of tracks 
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2015-07-10
       */
      LHCb::Track::ConstVector
      tracks 
      ( const LHCb::Particle*                                        particle ,
        const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& pcut     ) ;
      // ======================================================================
      /** get all tracks from the given particle 
       *  @param particle the particle 
       *  @param tcut     the cut
       *  @return container of tracks 
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2015-07-10
       */
      LHCb::Track::ConstVector
      tracks 
      ( const LHCb::Particle*                                        particle ,
        const LoKi::BasicFunctors<const LHCb::Track*>::Predicate&    tcut     ) ;
      // ======================================================================
      /** get all tracks from the given particle 
       *  @param particle the particle 
       *  @param pcut     the cut
       *  @param tcut     the cut
       *  @return container of tracks 
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2015-07-10
       */
      LHCb::Track::ConstVector
      tracks 
      ( const LHCb::Particle*                                        particle ,
        const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& pcut     ,
        const LoKi::BasicFunctors<const LHCb::Track*>::Predicate&    tcut     ) ;
      // ======================================================================
      /** get all tracks from the given particle 
       *  @param particle the particle 
       *  @param pcut     the cut
       *  @param tcut     the cut
       *  @return container of tracks 
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2015-07-10
       */
      LHCb::Track::ConstVector
      tracks 
      ( const LHCb::Particle*                                        particle ,
        const LoKi::BasicFunctors<const LHCb::Track*>::Predicate&    tcut     ,
        const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& pcut     ) ;
      // ======================================================================
    } // end of namespace LoKi::Dicts::Extract
    // ========================================================================
  } // end of namespace LoKi::Dicts
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSEXTRACTDICTS_H
// ============================================================================
