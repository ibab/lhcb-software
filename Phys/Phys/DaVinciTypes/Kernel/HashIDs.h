// $Id: $
// ============================================================================
#ifndef KERNEL_HASHIDS_H 
#define KERNEL_HASHIDS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  // forward declarations 
  // ==========================================================================
  class Particle      ;
  class ProtoParticle ;  
  class Track         ;  
  class CaloHypo      ;  
  class CaloCluster   ;  
  class MuonPID       ;  
  // ==========================================================================
  /** @class HashIDs 
   *
   *  Trivial structure to get the "hash-ID" for some LHCb Event classes
   *  "Hash-ID" is defiend as hash from all underlying LHCbIDs.
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-11-03
   *
   *  Version           $Revision:$
   *  Last Modification $Date$
   *                 by $Author$
   */
  class GAUDI_API HashIDs
  {
  public:
    // ========================================================================
    typedef std::vector<LHCb::LHCbID>   LHCbIDs ;
    // ========================================================================
  public: // get the hash-ID
    // ========================================================================
    static std::size_t hashID  ( const LHCb::Particle*      p ) ;
    static std::size_t hashID  ( const LHCb::ProtoParticle* p ) ;
    static std::size_t hashID  ( const LHCb::Track*         t ) ;
    static std::size_t hashID  ( const LHCb::CaloHypo*      c ) ;
    static std::size_t hashID  ( const LHCb::CaloCluster*   c ) ;
    static std::size_t hashID  ( const LHCb::MuonPID*       m ) ;
    // ========================================================================
  public: // get th list of LHCbIDs 
    // ========================================================================
    static void lhcbIDs ( const LHCb::Particle*      p , LHCbIDs& ids ) ;
    static void lhcbIDs ( const LHCb::ProtoParticle* p , LHCbIDs& ids ) ;
    static void lhcbIDs ( const LHCb::Track*         t , LHCbIDs& ids ) ;
    static void lhcbIDs ( const LHCb::CaloHypo*      c , LHCbIDs& ids ) ;
    static void lhcbIDs ( const LHCb::CaloCluster*   c , LHCbIDs& ids ) ;
    static void lhcbIDs ( const LHCb::MuonPID*       m , LHCbIDs& ids ) ;
    // ========================================================================
  public: // check the overlap 
    // ========================================================================
    static std::pair<double,double> overlap 
    ( const LHCb::Particle*      p1 , 
      const LHCb::Particle*      p2 ) ;
    static std::pair<double,double> overlap 
    ( const LHCb::ProtoParticle* p1 , 
      const LHCb::ProtoParticle* p2 ) ;
    static std::pair<double,double> overlap 
    ( const LHCb::Track*         t1 , 
      const LHCb::Track*         t2 ) ;
    static std::pair<double,double> overlap
    ( const LHCb::CaloCluster*   c1 , 
      const LHCb::CaloCluster*   c2 ) ;
    static std::pair<double,double> overlap 
    ( const LHCb::CaloHypo*      c1 , 
      const LHCb::CaloHypo*      c2 ) ;
    static std::pair<double,double> overlap 
    ( const LHCb::MuonPID*       m1 , 
      const LHCb::MuonPID*       m2 ) ;
    // ========================================================================
  } ;  
  // ==========================================================================
} //                                                      end of namespace LHCb 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // KERNEL_HASHIDS_H
// ============================================================================
