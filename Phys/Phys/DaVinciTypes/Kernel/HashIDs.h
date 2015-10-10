// $Id: $
// ============================================================================
#ifndef KERNEL_HASHIDS_H 
#define KERNEL_HASHIDS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <functional>
#include <iterator>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/LHCbID.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/functional/hash.hpp"
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
   *  "HashID" is defined as the hash from all underlying LHCbIDs.
   *
   *  It lso allows to get the (oredered) list of all LHCbIDs for 
   *  the given event model objects
   *
   *  Is also allows to calculate overlap for different objects in terms of 
   *  number of commont LHCbIDs 
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
    // the actual type of pointer to member function
    typedef bool (LHCb::LHCbID::*PMF)() const   ;
    // ========================================================================
  public: // The basic HashID ingredients  
    // ======================================================================== 
    /** hash-ID for LHCb::LHCbId is just channel-ID
     *  @see LHCb::LHCbID 
     */
    static std::size_t hashID  ( const LHCb::LHCbID& i ) { return i.channelID () ; }
    /** hash-ID for sequence of LHCbIDs:
     *  @code
     *  CONTAINER lst = ... 
     *  size_t    id  = hashID ( lst.begin , lst.end ) ;
     *  @endcode 
     */
    template <typename IT> 
      static std::size_t hashID ( IT first , IT last ) 
    {
      size_t seed = 0 ;
      using namespace boost ;
      for ( ; first != last ; ++first ) { hash_combine ( seed , first->channelID() ) ; }
      return seed ;
    }
    // =========================================================================
    /** hash-ID for (sub)sequence of LHCbIDs:
     *  @code
     *  CONTAINER lst     = ... 
     *  size_t    velo_id = hashID ( lst.begin , lst.end , 
     *                               std::mem_fun_ref( &LHCb::LHCbID::isVelo ) ) ;
     *  @endcode 
     */
    template <typename IT, class PREDICATE> 
      static std::size_t hashID ( IT first , IT last , PREDICATE good ) 
    {
      size_t seed = 0 ;
      using namespace boost ;
      for ( ; first != last ; ++first ) 
      { if ( good ( *first ) ) { hash_combine ( seed , first->channelID() ) ; } }
      return seed ;
    }
    // ========================================================================
  public: // get the hash-ID
    // ========================================================================
    /// calculate hash-id for LHCb::Particle
    static std::size_t hashID  ( const LHCb::Particle*      p ) ;
    /// calculate hash-id for LHCb::ProtoParticle
    static std::size_t hashID  ( const LHCb::ProtoParticle* p ) ;
    /// calculate hash-id for LHCb::Track
    static std::size_t hashID  ( const LHCb::Track*         t ) ;
    /// calculate hash-id for LHCb::CaloHypo
    static std::size_t hashID  ( const LHCb::CaloHypo*      c ) ;
    /// calculate hash-id for LHCb::CaloCluster
    static std::size_t hashID  ( const LHCb::CaloCluster*   c ) ;
    /// calculate hash-id for LHCb::MuonPID 
    static std::size_t hashID  ( const LHCb::MuonPID*       m ) ;    
    // ========================================================================
  public: // get the list of LHCbIDs 
    // ========================================================================
    /// get list of LHCbIDs from LHCb::Particle
    static void lhcbIDs ( const LHCb::Particle*      p , LHCbIDs& ids ) ;
    /// get list of LHCbIDs from LHCb::ProtoParticle
    static void lhcbIDs ( const LHCb::ProtoParticle* p , LHCbIDs& ids ) ;
    /// get list of LHCbIDs from LHCb::Track
    static void lhcbIDs ( const LHCb::Track*         t , LHCbIDs& ids ) ;
    /// get list of LHCbIDs from LHCb::CaloHypo
    static void lhcbIDs ( const LHCb::CaloHypo*      c , LHCbIDs& ids ) ;
    /// get list of LHCbIDs from LHCb::CaloCluster
    static void lhcbIDs ( const LHCb::CaloCluster*   c , LHCbIDs& ids ) ;
    /// get list of LHCbIDs from LHCb::MuonPID
    static void lhcbIDs ( const LHCb::MuonPID*       m , LHCbIDs& ids ) ;
    // ========================================================================
  public: // get the partial list of LHCbIDs 
    // ========================================================================
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param good INPUT criteria for LHCbIDs 
     */
    static void lhcbIDs ( const LHCb::Particle*      p , LHCbIDs& ids , PMF good ) ;
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param good INPUT criteria for LHCbIDs 
     */
    static void lhcbIDs ( const LHCb::ProtoParticle* p , LHCbIDs& ids , PMF good ) ;
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param good INPUT criteria for LHCbIDs 
     */
    static void lhcbIDs ( const LHCb::Track*         t , LHCbIDs& ids , PMF good ) ;
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param good INPUT criteria for LHCbIDs 
     */
    static void lhcbIDs ( const LHCb::CaloHypo*      c , LHCbIDs& ids , PMF good ) ;
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param good INPUT criteria for LHCbIDs 
     */
    static void lhcbIDs ( const LHCb::CaloCluster*   c , LHCbIDs& ids , PMF good ) ;
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param good INPUT criteria for LHCbIDs 
     */
    static void lhcbIDs ( const LHCb::MuonPID*       m , LHCbIDs& ids , PMF good ) ;
    // ========================================================================
  public: // get the partial list of LHCbIDs 
    // ========================================================================
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param type INPUT the type of LHCbID to consider 
     */
    static void lhcbIDs ( const LHCb::Particle*       p    , 
                          LHCbIDs&                    ids  , 
                          LHCb::LHCbID::channelIDtype type ) ;
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param type INPUT the type of LHCbID to consider 
     */
    static void lhcbIDs ( const LHCb::ProtoParticle*  p    ,
                          LHCbIDs&                    ids  , 
                          LHCb::LHCbID::channelIDtype type ) ;
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param type INPUT the type of LHCbID to consider 
     */
    static void lhcbIDs ( const LHCb::Track*          t    ,
                          LHCbIDs&                    ids  , 
                          LHCb::LHCbID::channelIDtype type ) ;
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param type INPUT the type of LHCbID to consider 
     */
    static void lhcbIDs ( const LHCb::CaloHypo*       c    ,
                          LHCbIDs&                    ids  , 
                          LHCb::LHCbID::channelIDtype type ) ;
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param type INPUT the type of LHCbID to consider 
     */
    static void lhcbIDs ( const LHCb::CaloCluster*    c    ,
                          LHCbIDs&                    ids  , 
                          LHCb::LHCbID::channelIDtype type ) ;
    /** get only certain type of LHCbIDs 
     *  @see LHCb::LHCbID 
     *  @param p    INPUT  input object 
     *  @param ids  UPDATE the list of LHCbIDs
     *  @param type INPUT the type of LHCbID to consider 
     */
    static void lhcbIDs ( const LHCb::MuonPID*        m    ,
                          LHCbIDs&                    ids  , 
                          LHCb::LHCbID::channelIDtype type ) ;
    // ========================================================================
  public: // get the hash-ID for tracks using only part of LHCbIDs 
    // ========================================================================
    /** get the hash-ID for tracks using only part of LHCbIDs 
     *  @code 
     *  const LHCb::Track* t = ... ;
     *  std::size_t velo_hash = LHCb::HashIDs::hashID ( t ,  LHCb::LHCbID::Velo );
     *  @endcode 
     *  @param track (INPUT) point to the track
     *  @param type  (INPUT) the type of LHCbIDs to be used for hash evaluation 
     *  @return hash-ID for the track using only LHCbIDs of given type 
     *  @see LHCb::LHCbID::channelIDtype
     */
    static std::size_t hashID ( const LHCb::Track*          track , 
                                LHCb::LHCbID::channelIDtype type  ) ;
    /** get the hash-ID for tracks using only part of LHCbIDs 
     *  @code 
     *  const LHCb::Track* t = ... ;
     *  std::size_t velo_hash = LHCb::HashIDs::hashID ( t ,  &LHCb::LHCbID::isVelo );
     *  @endcode 
     *  @param track (INPUT) point to the track
     *  @param good  (INPUT) the type of LHCbIDs to be used for hash evaluation 
     *  @return hash-ID for the track using only "good" LHCbIDs  
     *  @see LHCb::LHCbID
     */
    static std::size_t hashID ( const LHCb::Track*    track , PMF good ) ;
    // ========================================================================
  public: // check the overlap for two generic ontainers 
    // ========================================================================
    /** calculate the overlap for two containers
     *  param c1 INPUT the first  container 
     *  param c2 INPUT the second container 
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */
    static std::pair<double,double> overlap 
      ( const LHCbIDs& ids1 , 
        const LHCbIDs& ids2 ) ;
    // ========================================================================
    /** calculate the overlap for two containers
     *  param c1 INPUT the first  container 
     *  param c2 INPUT the second container 
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */
    static std::pair<double,double> overlap 
      ( const LHCbIDs& ids1 , 
        const LHCbIDs& ids2 , PMF good ) ;
    // ========================================================================
    /** calculate the overlap for two containers
     *  param c1 INPUT the first  container 
     *  param c2 INPUT the second container 
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */
    static std::pair<double,double> overlap 
      ( const LHCbIDs&              ids1 , 
        const LHCbIDs&              ids2 , 
        LHCb::LHCbID::channelIDtype type ) ;
    // ========================================================================
  public: // check the overlap for two SORTED ontainers 
    // ========================================================================
    /** calculate the overlap for two SORTED containers
     *  param c1 INPUT the first  container 
     *  param c2 INPUT the second container 
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */
    static std::pair<double,double> overlap_sorted 
      ( const LHCbIDs& ids1 , 
        const LHCbIDs& ids2 ) ;
    // ========================================================================
    /** calculate the overlap for two SORTED containers
     *  param c1 INPUT the first  container 
     *  param c2 INPUT the second container 
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */
    static std::pair<double,double> overlap_sorted 
      ( const LHCbIDs& ids1 , 
        const LHCbIDs& ids2 , PMF good ) ;
    // ========================================================================
    /** calculate the overlap for two SORTED containers
     *  param c1 INPUT the first  container 
     *  param c2 INPUT the second container 
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */
    static std::pair<double,double> overlap_sorted  
      ( const LHCbIDs&              ids1 , 
        const LHCbIDs&              ids2 , 
        LHCb::LHCbID::channelIDtype type ) ;
    // ========================================================================
  public: // check the overlap 
    // ========================================================================
    /** calculate overlap 
     *  @param p1   INPUT the first  particle 
     *  @param p2   INPUT the second particle
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */
    static std::pair<double,double> overlap 
      ( const LHCb::Particle*      p1 , 
        const LHCb::Particle*      p2 ) ;
    /** calculate overlap 
     *  @param p1   INPUT the first  particle 
     *  @param p2   INPUT the second particle
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */ 
    static std::pair<double,double> overlap 
      ( const LHCb::ProtoParticle* p1 , 
        const LHCb::ProtoParticle* p2 ) ;
    /** calculate overlap 
     *  @param p1   INPUT the first  track 
     *  @param p2   INPUT the second track
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */
    static std::pair<double,double> overlap 
      ( const LHCb::Track*         t1 , 
        const LHCb::Track*         t2 ) ;
    /** calculate overlap 
     *  @param p1   INPUT the first  cluster
     *  @param p2   INPUT the second cluster
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */
    static std::pair<double,double> overlap
      ( const LHCb::CaloCluster*   c1 , 
        const LHCb::CaloCluster*   c2 ) ;
    /** calculate overlap 
     *  @param p1   INPUT the first  hypo
     *  @param p2   INPUT the second hypo
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */
    static std::pair<double,double> overlap 
      ( const LHCb::CaloHypo*      c1 , 
        const LHCb::CaloHypo*      c2 ) ;
    /** calculate overlap 
     *  @param p1   INPUT the first  muon 
     *  @param p2   INPUT the second muon
     *  @return the overlap pair:   (#common/#1, #common/#2)
     */
    static std::pair<double,double> overlap 
      ( const LHCb::MuonPID*       m1 , 
        const LHCb::MuonPID*       m2 ) ;
    // ========================================================================
  public: // check overlap for certaint type of LHCbIDs only 
    // ========================================================================
    /** check overlap for certaint type of LHCbIDs only 
     *  @param p1   INPUT the first  particle 
     *  @param p2   INPUT the second particle
     *  @param good INPUT criteria for LHCbIDs 
     *  @return the overlap pair 
     */
    static std::pair<double,double> overlap 
      ( const LHCb::Particle*      p1 , 
        const LHCb::Particle*      p2 , PMF good ) ;
    // ========================================================================
    /** check overlap for certaint type of LHCbIDs only 
     *  @see LHCb::LHCbID 
     *  @see LHCb::LHCbID::channelIDtype 
     *  @param p1   INPUT the first  particle 
     *  @param p2   INPUT the second particle
     *  @param type INPUT the type of LHCbID to consider 
     *  @return the overlap pair 
     */
    static std::pair<double,double> overlap 
      ( const LHCb::Particle*       p1   , 
        const LHCb::Particle*       p2   , 
        LHCb::LHCbID::channelIDtype type ) ;
    // ========================================================================
  public: // check overlap for certaint type of LHCbIDs only 
    // ========================================================================
    /** check overlap for certaint type of LHCbIDs only 
     *  @see LHCb::LHCbID 
     *  @param p1   INPUT the first  track
     *  @param p2   INPUT the second track
     *  @param good INPUT criteria for LHCbIDs 
     *  @return the overlap pair 
     */
    static std::pair<double,double> overlap 
      ( const LHCb::Track*      p1 , 
        const LHCb::Track*      p2 , PMF good ) ;
    // ========================================================================
    /** check overlap for certaint type of LHCbIDs only 
     *  @see LHCb::LHCbID 
     *  @see LHCb::LHCbID::channelIDtype 
     *  @param p1   INPUT the first  track
     *  @param p2   INPUT the second track
     *  @param type INPUT the type of LHCbID to consider 
     *  @return the overlap pair 
     */
    static std::pair<double,double> overlap 
      ( const LHCb::Track*          p1   , 
        const LHCb::Track*          p2   , 
        LHCb::LHCbID::channelIDtype type ) ;
    // ========================================================================
  } ;  
  // ==========================================================================
} //                                                      end of namespace LHCb 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // KERNEL_HASHIDS_H
// ============================================================================
