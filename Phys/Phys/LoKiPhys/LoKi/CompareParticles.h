// $Id: CompareParticles.h,v 1.2 2009-05-09 19:24:13 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_COMPAREPARTICLES_H 
#define LOKI_COMPAREPARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
namespace LoKi
{
  namespace Particles
  {
    // ========================================================================
    /** @class  RawCompare 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-04-01
     */
    class RawCompare
    {
      // ======================================================================
    public:
      // ======================================================================
      /// the only one essential method:
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 ) const 
      { return check ( p1 , p2 ) && RawCompare::compare ( p1 , p2 ) ; }
      // ======================================================================
    protected:
      // ======================================================================
      inline bool compare    ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 ) const 
      { return p1 < p2 ; }
      // ======================================================================
    public:
      // ====================================================================
      inline bool check ( const LHCb::Particle* p  ) const { return 0 != p ; }
      inline bool check ( const LHCb::Particle* p1 ,
                          const LHCb::Particle* p2 ) const 
      {
        return check ( p1 ) && check ( p2 ) && ( p1 != p2 ) ; 
      }
    };
    // =======================================================================
    /** @class KeyCompare
     *  The comparison of two particles by key
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-04-01
     */
    class KeyCompare : public RawCompare 
    {
      // ======================================================================
    public:
      // ======================================================================
      /// the only one essential method:
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 ) const 
      {
        return check ( p1 , p2 ) && KeyCompare::compare ( p1 , p2 ) ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      inline bool compare    ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 ) const 
      { 
        if ( p1 -> hasKey() && p2 -> hasKey() ) 
        {
          const LHCb::Particle::key_type k1 = p1->key() ;
          const LHCb::Particle::key_type k2 = p2->key() ;
          if      ( k1  < k2  ) { return true  ; }                // RETURN 
          else if ( k2  < k1  ) { return false ; }                // RETURN
        }
        return RawCompare::compare ( p1 , p2 ) ;
      }
      // ======================================================================
    };
    // =======================================================================
    /** @class KinCompare
     *  The generic kinematical comparison of two particles 
     *  @attention This comparison ignores PID! 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-04-01
     */
    class KinCompare : public KeyCompare 
    {
      // ====================================================================
    public:
      // ====================================================================
      /// the only one essential method:
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 ) const 
      {
        return check ( p1 , p2 ) && KinCompare::compare ( p1 , p2 ) ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      inline bool compare    ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 ) const 
      {
        // compare by Pt2
        const double pt1 = p1 -> momentum () . Perp2 () ;
        const double pt2 = p2 -> momentum () . Perp2 () ;
        if      ( pt1 < pt2 ) { return true  ; }                 // RETURN 
        else if ( pt2 < pt1 ) { return false ; }                 // RETURN
        // compare by E
        const double e1  = p1 -> momentum () . E () ;
        const double e2  = p2 -> momentum () . E () ;
        if      ( e1  < e2  ) { return true  ; }                 // RETURN 
        else if ( e2  < e1  ) { return false ; }                 // RETURN
        // compare by key
        return KeyCompare::compare ( p1 , p2 ) ;
      }
      // ======================================================================
    };
    // =======================================================================
    /** @class FullCompare
     *  The generic kinematical&PID comparison of two particles 
     *  essentially it is "clone-in-loop compare" for LoKi-loops to avoid the 
     *  duplicates in the loop 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-04-01
     */
    class FullCompare : public KinCompare
    {
      // ====================================================================
    public:
      // ====================================================================
      /// the only one essential method:
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 ) const 
      {
        return check ( p1 , p2 ) && FullCompare::compare ( p1 , p2 ) ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      inline bool compare    ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 ) const 
      {      
        // compare by PID
        const long pid1 = p1 -> particleID() .pid() ;
        const long pid2 = p2 -> particleID() .pid() ;
        if      ( pid1 < pid2 ) { return true  ; }               // RETURN 
        else if ( pid2 < pid1 ) { return false ; }               // RETURN        
        // make kinematical compare 
        return KinCompare::compare ( p1 , p2 ) ;
      }
      // ====================================================================
    };
    // ========================================================================
    /** @class PidCompare 
     *  The specific kinematical comparison for particles with the same PID 
     *  essentially it is "pid-in-loop compare" for LoKi-loops to avoid the 
     *  duplicates in the loop 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-04-01
     */        
    class PidCompare : public KinCompare 
    {
      // ====================================================================
    public:
      // ====================================================================
      /// the only one essential method:
      inline bool operator() ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 ) const 
      {
        return check ( p1 , p2 ) && PidCompare::compare ( p1 , p2 ) ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      inline bool compare    ( const LHCb::Particle* p1 , 
                               const LHCb::Particle* p2 ) const 
      {      
        // compare by PID
        const long pid1 = p1 -> particleID() .pid() ;
        const long pid2 = p2 -> particleID() .pid() ;
        // ATTENTION! *TRUE* IS HERE !!!
        if ( pid1 != pid2 ) { return true ; }  // ATTENTION! RETURN *TRUE*
        // make the kinematical compare 
        return KinCompare::compare ( p1 , p2 ) ;
      }
      // =====================================================================
    };
    // ========================================================================
  } // end of namespace LoKi::Compare 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_COMPAREPARTICLES_H
// ============================================================================
