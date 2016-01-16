// $Id$
// ============================================================================
#ifndef LOKI_NBODYCOMPARE_H 
#define LOKI_NBODYCOMPARE_H 1
// ======================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/CompareParticles.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles
  {
    // ========================================================================
    /** @class NBodyCompare NBodyCompare.h LoKi/NBodyCompare.h
     *  Dedicated "comparison" criteria for particles (e.g. 
     *  for NBody-decays family of algorithms 
     *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
     *  @date   2015-02-15
     */
    class NBodyCompare : public PidCompare 
    {
    public:
      // ======================================================================
      /// comparison of two particles 
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 ) const ;
      /// comparison of two particles, assuming the firts two are fine 
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 , 
                               const LHCb::Particle* p3 ) const ;
      /// comparison of N particles, assuming the first (N-1) are fine 
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 , 
                               const LHCb::Particle* p3 , 
                               const LHCb::Particle* p4 ) const ;
      /// comparison of N particles, assuming the first (N-1) are fine 
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 , 
                               const LHCb::Particle* p3 , 
                               const LHCb::Particle* p4 , 
                               const LHCb::Particle* p5 ) const ;
      /// comparison of N particles, assuming the first (N-1) are fine 
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 , 
                               const LHCb::Particle* p3 , 
                               const LHCb::Particle* p4 , 
                               const LHCb::Particle* p5 ,
                               const LHCb::Particle* p6 ) const ;
      /// comparison of N particles, assuming the first (N-1) are fine 
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 , 
                               const LHCb::Particle* p3 , 
                               const LHCb::Particle* p4 , 
                               const LHCb::Particle* p5 ,
                               const LHCb::Particle* p6 , 
                               const LHCb::Particle* p7 ) const ;
      /// comparison of N particles, assuming the first (N-1) are fine 
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 , 
                               const LHCb::Particle* p3 , 
                               const LHCb::Particle* p4 , 
                               const LHCb::Particle* p5 ,
                               const LHCb::Particle* p6 , 
                               const LHCb::Particle* p7 ,
                               const LHCb::Particle* p8 ) const ;
      // ======================================================================
    public: // check the uniqiness of particles 
      // ======================================================================
      /// check the uniquness of the particles 
      inline bool unique ( const LHCb::Particle* p1 , 
                           const LHCb::Particle* p2 ) const ;
      // ======================================================================      
    protected:
      // ======================================================================
      /// check the uniquiness of the particles
      template <class PARTICLE> 
      inline bool unique ( PARTICLE              begin , 
                           PARTICLE              end   , 
                           const LHCb::Particle* p2    ) const 
      {
        for ( ; begin != end ; ++begin ) 
        { if ( !this->unique ( *begin , p2 ) ) { return false ; } }
        return true ;
      }
      // ======================================================================
    } ;
    // ========================================================================
  } //                                           end of namespace LoKi::Compare 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// very crude uniqueness criteria, based on the same protos  
// ============================================================================
inline bool LoKi::Particles::NBodyCompare::unique
( const LHCb::Particle* p1 , 
  const LHCb::Particle* p2 ) const 
{
  //
  if ( 0 == p1 || 0 == p2 || p1 == p2 ) { return false ; }
  //
  const bool bp1 = p1 -> isBasicParticle() ;
  const bool bp2 = p2 -> isBasicParticle() ;
  //
  if ( bp1 && bp2 ) 
  { 
    //
    const LHCb::ProtoParticle* pp1 = p1->proto() ;
    const LHCb::ProtoParticle* pp2 = p2->proto() ;
    //
    if ( 0 != pp1 && 0 != pp2 ) 
    {
      if      ( pp1 == pp2 ) { return false ; } // REUTRN 
      //
      const LHCb::Track* t1 = pp1->track() ;
      const LHCb::Track* t2 = pp2->track() ;
      //
      if      ( t1  != t2  ) { return true  ; } // RETURN 
      else if ( 0   != t1  ) { return false ; } // RETURN
      //
    }
    //
    return true ; // a bit strange logic, but it should be OK
  }
  //
  const SmartRefVector<LHCb::Particle>& daughters1 = p1 -> daughters() ;
  const SmartRefVector<LHCb::Particle>& daughters2 = p2 -> daughters() ;
  //
  // one basic and one composite particle 
  if      ( bp1 ) { return unique ( daughters2.begin() , daughters2.end() , p1 ) ; }
  else if ( bp2 ) { return unique ( daughters1.begin() , daughters1.end() , p2 ) ; }
  //
  return 
    daughters1.size() < daughters2.size() ?
    unique ( daughters2.begin () , daughters2.end() , p1 ) :
    unique ( daughters1.begin () , daughters1.end() , p2 ) ;
}
// ============================================================================
// comparison of two particles 
// ============================================================================
inline bool LoKi::Particles::NBodyCompare::operator()
  ( const LHCb::Particle* p1 , 
    const LHCb::Particle* p2 ) const 
{
  return 0 != p1 && 0 != p2 && p1 != p2 
    && compare ( p1 , p2 ) 
    && unique  ( p1 , p2 ) ;
}
// ============================================================================
// comparison of three particles, assuming frist two are already fine 
// ============================================================================
inline bool LoKi::Particles::NBodyCompare::operator()
  ( const LHCb::Particle* p1 , 
    const LHCb::Particle* p2 ,
    const LHCb::Particle* p3 ) const 
{
  return 
    0  != p3 && p1 != p3 && p2 != p3
    && compare ( p1 , p3 ) && compare ( p2 , p3 ) 
    && unique  ( p1 , p3 ) && unique  ( p2 , p3 ) ;
}
// ============================================================================
// comparison of N particles, assuming frist (N-1)  are already fine 
// ============================================================================
inline bool LoKi::Particles::NBodyCompare::operator()
  ( const LHCb::Particle* p1 , 
    const LHCb::Particle* p2 ,
    const LHCb::Particle* p3 ,
    const LHCb::Particle* p4 ) const 
{
  return 
    0  != p4 && p1 != p4 && p2 != p4 && p3 != p4 
    && compare ( p1 , p4 ) && compare ( p2 , p4 ) && compare ( p3 , p4 ) 
    && unique  ( p1 , p4 ) && unique  ( p2 , p4 ) && unique  ( p3 , p4 ) ;
}
// ============================================================================
// comparison of N particles, assuming frist (N-1)  are already fine 
// ============================================================================
inline bool LoKi::Particles::NBodyCompare::operator()
  ( const LHCb::Particle* p1 , 
    const LHCb::Particle* p2 ,
    const LHCb::Particle* p3 ,
    const LHCb::Particle* p4 ,
    const LHCb::Particle* p5 ) const 
{
  return 
    0  != p5 && p1 != p5 && p2 != p5 && p3 != p5 && p4 != p5  
    && compare ( p1 , p5 ) 
    && compare ( p2 , p5 ) 
    && compare ( p3 , p5 ) 
    && compare ( p4 , p5 ) 
    && unique  ( p1 , p5 ) 
    && unique  ( p2 , p5 ) 
    && unique  ( p3 , p5 ) 
    && unique  ( p4 , p5 ) ;
} 
// ============================================================================
// comparison of N particles, assuming frist (N-1)  are already fine 
// ============================================================================
inline bool LoKi::Particles::NBodyCompare::operator()
  ( const LHCb::Particle* p1 , 
    const LHCb::Particle* p2 ,
    const LHCb::Particle* p3 ,
    const LHCb::Particle* p4 ,
    const LHCb::Particle* p5 ,
    const LHCb::Particle* p6 ) const
{
  return 
    0  != p6 && p1 != p6 && p2 != p6 && p3 != p6 && p4 != p6 && p5 != p6   
    && compare ( p1 , p6 ) 
    && compare ( p2 , p6 ) 
    && compare ( p3 , p6 ) 
    && compare ( p4 , p6 ) 
    && compare ( p5 , p6 )
    && unique  ( p1 , p6 ) 
    && unique  ( p2 , p6 ) 
    && unique  ( p3 , p6 ) 
    && unique  ( p4 , p6 ) 
    && unique  ( p5 , p6 ) ;
}  
// ============================================================================
// comparison of N particles, assuming frist (N-1)  are already fine 
// ============================================================================
inline bool LoKi::Particles::NBodyCompare::operator()
  ( const LHCb::Particle* p1 , 
    const LHCb::Particle* p2 ,
    const LHCb::Particle* p3 ,
    const LHCb::Particle* p4 ,
    const LHCb::Particle* p5 ,
    const LHCb::Particle* p6 ,
    const LHCb::Particle* p7 ) const
{
  return 
    0  != p7 && 
    p1 != p7 && p2 != p7 && p3 != p7 && 
    p4 != p7 && p5 != p7 && p6 != p7   
    && compare ( p1 , p7 ) 
    && compare ( p2 , p7 ) 
    && compare ( p3 , p7 ) 
    && compare ( p4 , p7 ) 
    && compare ( p5 , p7 )
    && compare ( p6 , p7 )
    && unique  ( p1 , p7 ) 
    && unique  ( p2 , p7 ) 
    && unique  ( p3 , p7 ) 
    && unique  ( p4 , p7 )
    && unique  ( p5 , p7 ) 
    && unique  ( p6 , p7 ) ;
}
// ============================================================================
// comparison of N particles, assuming frist (N-1)  are already fine 
// ============================================================================
inline bool LoKi::Particles::NBodyCompare::operator()
  ( const LHCb::Particle* p1 , 
    const LHCb::Particle* p2 ,
    const LHCb::Particle* p3 ,
    const LHCb::Particle* p4 ,
    const LHCb::Particle* p5 ,
    const LHCb::Particle* p6 ,
    const LHCb::Particle* p7 ,
    const LHCb::Particle* p8 ) const 
{
  return 
    0  != p8 && 
    p1 != p8 && p2 != p8 && p3 != p8 && 
    p4 != p8 && p5 != p8 && p6 != p8 && p7 != p8    
    && compare ( p1 , p8 ) 
    && compare ( p2 , p8 ) 
    && compare ( p3 , p8 ) 
    && compare ( p4 , p8 ) 
    && compare ( p5 , p8 )
    && compare ( p6 , p8 )
    && compare ( p7 , p8 )
    && unique  ( p1 , p8 ) 
    && unique  ( p2 , p8 ) 
    && unique  ( p3 , p8 ) 
    && unique  ( p4 , p8 )
    && unique  ( p5 , p8 ) 
    && unique  ( p6 , p8 ) 
    && unique  ( p7 , p8 ) ;
}
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_NBODYCOMPARE_H
// ============================================================================
