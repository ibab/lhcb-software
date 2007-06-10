// $Id: LoopDecorator.h,v 1.3 2007-06-10 20:13:35 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOOPDICT_H 
#define LOKI_LOOPDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Loop.h"
// ============================================================================
namespace LoKi 
{
  namespace Dicts
  {
    class DictsLoop
    {
    public:
      /// get the effective formula of the loop, see LoKi::LoopObj::formula
      static const LoKi::LoopObj::Formula& formula( const LoKi::Loop& l ) ;
      /// get the effective dimension of the loop (== number of components)
      static size_t     dim    ( const LoKi::Loop& l ) ;
      /// Loop size (including the invalid combinations!)
      static size_t     size   ( const LoKi::Loop& l ) ;
      /// check the validity of the current combinations
      static bool       valid  ( const LoKi::Loop& l ) ;
      /// get the current status of the loop object 
      static StatusCode status ( const LoKi::Loop& l ) ;
      /// advance the loop to the next combination
      static const LoKi::Loop& next ( const LoKi::Loop& l ) ;
      /// conversion operator to LHCb::Particle and access to the children:
      static const LHCb::Particle* particle 
      ( const LoKi::Loop& l     , 
        const size_t      i     ) ;
      /// conversion operator to LHCb::Particle and access to the children:
      static const LHCb::Particle*   particle ( const LoKi::Loop& l ) ;
      /// conversion to LHCb::VertexBase 
      static const LHCb::VertexBase* vertex   ( const LoKi::Loop& l ) ;    
      ///  get the momentum of the particle or daughter 
      static LoKi::LorentzVector momentum 
      ( const LoKi::Loop& l     ) ;
      ///  get the momentum of the particle or daughter 
      static LoKi::LorentzVector momentum 
      ( const LoKi::Loop& l     , 
        const size_t      i     ) ;
      ///  get the momentum of the pair of daughters 
      static LoKi::LorentzVector momentum 
      ( const LoKi::Loop& l     , 
        const size_t      i1    , 
        const size_t      i2    ) ;
      ///  get the momentum of the triplet of daughters 
      static LoKi::LorentzVector momentum 
      ( const LoKi::Loop& l     , 
        const size_t      i1    , 
        const size_t      i2    ,
        const size_t      i3    ) ;
      ///  get the momentum of the 4-daughters 
      static LoKi::LorentzVector momentum 
      ( const LoKi::Loop& l     , 
        const size_t      i1    , 
        const size_t      i2    ,
        const size_t      i3    , 
        const size_t      i4    ) ;     
      /// get the invariant mass of the particle or child 
      static double mass
      ( const LoKi::Loop& l     ) ;
      /// get the invariant mass of the particel or child 
      static double mass
      ( const LoKi::Loop& l     , 
        const size_t      i     ) ;
      /// get the invariant mass of the pair of particles 
      static double mass
      ( const LoKi::Loop& l     , 
        const size_t      i1    , 
        const size_t      i2    ) ;
      /// get the invariant mass of the triplet of particles 
      static double mass
      ( const LoKi::Loop& l     , 
        const size_t      i1    , 
        const size_t      i2    ,
        const size_t      i3    ) ;
      /// get the invariant mass of the 4-particles 
      static double mass
      ( const LoKi::Loop& l     , 
        const size_t      i1    , 
        const size_t      i2    ,
        const size_t      i3    , 
        const size_t      i4    ) ;    
      /// get the effective PID 
      static LHCb::ParticleID pid( const LoKi::Loop& l ) ;
      /// get PID for the effective particle of the loop
      static const ParticleProperty* pp ( const LoKi::Loop& l ) ;
      /// get PID for the effective particle of the loop
      static const std::string& pidName ( const LoKi::Loop& l ) ;      
      /// set the particle ID for the effective particle of the loop 
      static const LoKi::Loop& setPID 
      ( const LoKi::Loop&       l   , 
        const LHCb::ParticleID& pid ) ;  
      /// set the particle ID for the effectiev particle of the loop 
      static const LoKi::Loop& setPID 
      ( const LoKi::Loop&       l   , 
        const std::string&      pid ) ;
      /// set the particle ID for the effectiev particle of the loop 
      static const LoKi::Loop& setPID 
      ( const LoKi::Loop&       l   , 
        const ParticleProperty* pid ) ;
      /// set the default IParticleCombiner tool
      static const LoKi::Loop& setCombiner 
      ( const LoKi::Loop&        l , 
        const IParticleCombiner* c ) ;
      /// set the default IParticleCombiner tool
      static const LoKi::Loop& setReFitter 
      ( const LoKi::Loop&        l , 
        const IParticleReFitter* c ) ;
      /// get the associated vertex 
      static const LHCb::VertexBase* pv  
      ( const LoKi::Loop& l ) ;
      /// set the associated vertex 
      static const LoKi::Loop& setPV 
      ( const LoKi::Loop&       l , 
        const LHCb::VertexBase* v ) ;    
      /// make 'effective' particle using IParticleCombiner tool
      static StatusCode make  
      ( const LoKi::Loop&        l        ) ;
      /// make 'effective' particle using IParticleCombiner tool
      static StatusCode make  
      ( const LoKi::Loop&  l    , 
        const std::string& nick ) ;
      /// make 'effective' particle using IParticleCombiner tool
      static StatusCode make  
      ( const LoKi::Loop&             , 
        const IParticleCombiner* comb ) ;
      /// refit the particle using IParticleReFitter tool
      static StatusCode reFit 
      ( const LoKi::Loop&        l        ) ;
      /// refit the particle using IParticleReFitter tool
      static StatusCode reFit 
      ( const LoKi::Loop&  l    , 
        const std::string& nick ) ;
      /// refit the particle using IParticleReFitter tool
      static StatusCode reFit 
      ( const LoKi::Loop&        l        , 
        const IParticleReFitter* fit      ) ;
      /// save the particle into LoKi storage
      static StatusCode save  
      ( const LoKi::Loop&  l   , 
        const std::string& tag ) ;    
      /// backup the current state of the loop 
      static const LoKi::Loop& backup  ( const LoKi::Loop& l ) ;
      /// restore the loop from the last backup state
      static const LoKi::Loop& restore ( const LoKi::Loop& l ) ;
    } ; // end of class LoKi::Dicts::LoopDicts 
  } // end of namespace LoKi::Dicts 
} // end of namespace LoKi 
// ============================================================================
// The END   
// ============================================================================
#endif // LOKI_LOOPDICT_H
// ============================================================================
