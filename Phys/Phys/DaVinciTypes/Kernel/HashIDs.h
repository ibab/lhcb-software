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
// LHCbKernel
// ============================================================================
#include "Kernel/LHCbID.h"
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
   *  "Hash-ID" is defined as the hash from all underlying LHCbIDs.
   *
   *  Is also allows to calculate overlap for different objects in terms of 
   *  number of commont LHCbIDs 
   
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
  public: // get the hash-ID
    // ========================================================================
    static std::size_t hashID  ( const LHCb::Particle*      p ) ;
    static std::size_t hashID  ( const LHCb::ProtoParticle* p ) ;
    static std::size_t hashID  ( const LHCb::Track*         t ) ;
    static std::size_t hashID  ( const LHCb::CaloHypo*      c ) ;
    static std::size_t hashID  ( const LHCb::CaloCluster*   c ) ;
    static std::size_t hashID  ( const LHCb::MuonPID*       m ) ;
    // ========================================================================
  public: // get the list of LHCbIDs 
    // ========================================================================
    static void lhcbIDs ( const LHCb::Particle*      p , LHCbIDs& ids ) ;
    static void lhcbIDs ( const LHCb::ProtoParticle* p , LHCbIDs& ids ) ;
    static void lhcbIDs ( const LHCb::Track*         t , LHCbIDs& ids ) ;
    static void lhcbIDs ( const LHCb::CaloHypo*      c , LHCbIDs& ids ) ;
    static void lhcbIDs ( const LHCb::CaloCluster*   c , LHCbIDs& ids ) ;
    static void lhcbIDs ( const LHCb::MuonPID*       m , LHCbIDs& ids ) ;
    // ========================================================================
  public: // get partial list of LHCbIDs 
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
  public: // get partial list of LHCbIDs 
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
