// $Id: PartPropDict.h,v 1.2 2008-12-03 17:35:54 ibelyaev Exp $
// ============================================================================
#ifndef PARTPROP_PARTPROPDICT_H 
#define PARTPROP_PARTPROPDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// Kernel/PartProp
// ============================================================================
#include "Kernel/ParticleID.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/Decay.h"
#include "Kernel/iNode.h"
#include "Kernel/Nodes.h"
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  namespace Dict 
  {
    // ========================================================================
    struct NodeOps 
    {
      // ======================================================================
      typedef Decays::iNode iNode ;
      typedef Decays::Node   Node ;
      // ======================================================================
    public:
      // ======================================================================
      // __call__ 
      static bool __call__ 
      ( const iNode&            node , 
        const LHCb::ParticleID& pid  )
      { return node ( pid ) ; }
      // __call__ 
      static bool __call__ 
      ( const iNode&                  node , 
        const LHCb::ParticleProperty* pp   ) 
      { return node ( pp->particleID () ) ; }
      // __call__ 
      static bool __call__ 
      ( const iNode&            node , 
        const int               pid  ) 
      { return node ( LHCb::ParticleID ( pid ) ) ; }
      // =====================================================================
    public:
      // =====================================================================
      // __rrshift__ 
      static bool __rrshift__ 
      ( const iNode&            node , 
        const LHCb::ParticleID& pid  )
      { return node ( pid ) ; }
      // __rrshift__ 
      static bool __rrshift__ 
      ( const iNode&                  node , 
        const LHCb::ParticleProperty* pp   ) 
      { return node ( pp->particleID () ) ; }
      // __rrshift__ 
      static bool __rrshift__ 
      ( const iNode&            node , 
        const int               pid  ) 
      { return node ( LHCb::ParticleID ( pid ) ) ; }
      // =====================================================================
    public:
      // =====================================================================
      // __or__ 
      static Node __or__ 
      ( const iNode&                 n1 , 
        const iNode&                 n2 ) { return n1 || n2 ; }
      // __or__ 
      static Node __or__  
      ( const iNode&                 n1 , 
        const std::string&           n2 ) { return n1 || n2 ; }
      // __or__ 
      static Node __or__ 
      ( const iNode&                 n1 , 
        const LHCb::ParticleID&      n2 ) { return n1 || n2 ; }
      // __or__ 
      static Node __or__
      ( const iNode&                  n1 , 
        const LHCb::ParticleProperty* n2 ) { return n1 || n2 ; }
      // __or__ 
      static Node __or__
      ( const iNode&                  n1 , 
        const Decays::Decay::Item&    n2 ) { return n1 || n2 ; }
      // ======================================================================
    public:
      // ======================================================================
      // __ror__ 
      static Node __ror__ 
      ( const iNode&                 n1 , 
        const std::string&           n2 ) { return n1 || n2 ; }
      // __ror__ 
      static Node __ror__
      ( const iNode&                 n1 , 
        const LHCb::ParticleID&      n2 ) { return n1 || n2 ; }
      // __ror__ 
      static Node __ror__ 
      ( const iNode&                  n1 , 
        const LHCb::ParticleProperty* n2 ) { return n1 || n2 ; }
      // __ror__ 
      static Node __ror__  
      ( const iNode&                  n1 , 
        const Decays::Decay::Item&    n2 ) { return n1 || n2 ; }
      // ======================================================================
    public:
      // =====================================================================
      // __and__ 
      static Node __and__ 
      ( const iNode&                 n1 , 
        const iNode&                 n2 ) { return n1 && n2 ; }
      // __and__ 
      static Node __and__  
      ( const iNode&                 n1 , 
        const std::string&           n2 ) { return n1 && n2 ; }
      // __and__ 
      static Node __and__  
      ( const iNode&                 n1 , 
        const LHCb::ParticleID&      n2 ) { return n1 && n2 ; }
      // __and__ 
      static Node __and__ 
      ( const iNode&                  n1 , 
        const LHCb::ParticleProperty* n2 ) { return n1 && n2 ; }
      // __and__ 
      static Node __and__ 
      ( const iNode&                  n1 , 
        const Decays::Decay::Item&    n2 ) { return n1 && n2 ; }
      // ======================================================================
    public:
      // =====================================================================
      // __rand__ 
      static Node __rand__  
      ( const iNode&                 n1 , 
        const std::string&           n2 ) { return n1 && n2 ; }
      // __rand__ 
      static Node __rand__  
      ( const iNode&                 n1 , 
        const LHCb::ParticleID&      n2 ) { return n1 && n2 ; }
      // __rand__ 
      static Node __rand__ 
      ( const iNode&                  n1 , 
        const LHCb::ParticleProperty* n2 ) { return n1 && n2 ; }
      // __rand__ 
      static Node __rand__ 
      ( const iNode&                  n1 , 
        const Decays::Decay::Item&    n2 ) { return n1 && n2 ; }
      // ======================================================================
    public:
      // =====================================================================
      // __rshift__ 
      static Node __rshift__ 
      ( const iNode&                 n1 , 
        const iNode&                 n2 ) { return n1 && n2 ; }
      // =====================================================================
    public:
      // =====================================================================
      // __invert__ 
      static Node __invert__ 
      ( const iNode&                 n1 ) { return !n1 ; }
      // =====================================================================
    };
    // ========================================================================
  } 
  // ==========================================================================
}
// ============================================================================
namespace 
{
  // ==========================================================================
  struct _Instantiations
  {
    // ========================================================================
    LHCb::ParticleID                           m_pid  ;
    LHCb::ParticleProperty                     m_pp   ;
    std::vector<LHCb::ParticleID>              m_pidv ;
    std::vector<const LHCb::ParticleProperty*> m_ppv  ;
    //
    std::vector<Decays::Decay>                 m_vd   ;
    std::vector<Decays::Decay::Item>           m_vi   ;
    // ioperations with nodes 
    Decays::Dict::NodeOps                      m_no   ;
    // fictive constructor 
    _Instantiations () ;
    // ========================================================================
  } ;
  // ==========================================================================
} // end of anonymout namespace 
// ============================================================================
#endif // PARTPROP_PARTPROPDICT_H
// ============================================================================
// The END 
// ============================================================================
