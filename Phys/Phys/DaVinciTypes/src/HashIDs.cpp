// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
#include <iterator>
#include <algorithm>
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/VertexBase.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
#include "Event/RecVertex.h"
#include "Event/Track.h"
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"
#include "Event/MuonPID.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/CaloCellIDHash.h"
#include "Kernel/CountIterator.h"
// ============================================================================
// local
// ============================================================================
#include "Kernel/HashIDs.h"
// ============================================================================
// Boost 
// ============================================================================
//#include "boost/functional/hash.hpp"
// ============================================================================
/** @file 
 *  Implementation file for class : LHCb::HashID
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-11-03
 *
 *  Version           $Revision$
 *  Last Modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// the actual type for ordered list of _LHCbIDs 
  typedef std::set<LHCb::LHCbID>   _LHCbIDs ;
  // ==========================================================================
  /** convert channel id type into member function 
   *  @attention: mappings need to be updated in case of changes in LHCb::LHCbIDs 
   */
  inline 
  LHCb::HashIDs::PMF _id_to_pmf_ ( LHCb::LHCbID::channelIDtype type ) 
  {
    return 
      LHCb::LHCbID::Velo == type ? &LHCb::LHCbID::isVelo : 
      LHCb::LHCbID::TT   == type ? &LHCb::LHCbID::isTT   : 
      LHCb::LHCbID::IT   == type ? &LHCb::LHCbID::isIT   : 
      LHCb::LHCbID::OT   == type ? &LHCb::LHCbID::isOT   : 
      LHCb::LHCbID::Rich == type ? &LHCb::LHCbID::isRich : 
      LHCb::LHCbID::Calo == type ? &LHCb::LHCbID::isCalo : 
      LHCb::LHCbID::Muon == type ? &LHCb::LHCbID::isMuon : 
      LHCb::LHCbID::VP   == type ? &LHCb::LHCbID::isVP   : 
      LHCb::LHCbID::FT   == type ? &LHCb::LHCbID::isFT   : 
      LHCb::LHCbID::UT   == type ? &LHCb::LHCbID::isUT   : 
      LHCb::LHCbID::HC   == type ? &LHCb::LHCbID::isHC   : 0 ;
  }
  // ==========================================================================
  //
  /// add IDs from the sequence 
  template <class OBJECT>
  inline std::size_t insert 
  ( _LHCbIDs& ids   ,
    OBJECT   begin , 
    OBJECT   end   ) ;
  // 
  /// add ID 
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::LHCbID&    id ) 
  { return ids.insert ( id ).second ? 1 : 0 ; }
  //
  /// add ID from Calorimeter Cell
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::CaloCellID& id ) 
  { return ids.insert ( id ).second ? 1 : 0 ; }
  // 
  /// insert IDs from set of IDs
  inline std::size_t insert ( _LHCbIDs& ids , const _LHCbIDs& v ) 
  { 
    std::size_t _size = ids.size() ;
    ids.insert ( v.begin() , v.end() ) ;
    return ids.size() - _size ;
  }
  /// insert IDs from vector of IDs
  inline std::size_t insert ( _LHCbIDs& ids , const std::vector<LHCb::LHCbID>& v ) 
  { 
    std::size_t _size = ids.size() ;
    ids.insert ( v.begin() , v.end() ) ;
    return ids.size() - _size ;
  }
  /// insert IDs from tracks 
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::Track* t  ) 
  { 
    if ( 0 == t ) { return 0 ; }
    return insert ( ids , t->lhcbIDs() ) ;
  }
  /// insert IDs from MuonPIDs 
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::MuonPID* m  ) 
  { 
    if ( 0 == m ) { return 0 ; }
    return insert ( ids , m -> muonTrack() ) ;
  }
  /// insert IDs from CaloDigit
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::CaloDigit* d ) 
  { 
    if ( 0 == d ) { return 0 ; }
    return insert ( ids , d -> cellID()  ) ;
  }
  /// insert IDs from CaloClusterEntry
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::CaloClusterEntry& e ) 
  { return insert ( ids , e.digit() ) ; }
  /// insert IDs from CaloCluster 
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::CaloCluster* c ) 
  { 
    if ( 0 == c ) { return 0 ; }
    //
    const  LHCb::CaloCluster::Entries& entries = c->entries() ;
    return insert ( ids              , 
                    entries.begin () , 
                    entries.end   () ) ;
  }
  /// insert IDs from CaloHypo
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::CaloHypo* h ) 
  { 
    if ( 0 == h ) { return 0 ; }
    //
    const LHCb::CaloHypo::Digits&   digits   = h -> digits   () ;
    const LHCb::CaloHypo::Clusters& clusters = h -> clusters () ;
    const LHCb::CaloHypo::Hypos&    hypos    = h -> hypos    () ;
    return
      insert ( ids , digits   . begin () , digits   . end () ) + 
      insert ( ids , clusters . begin () , clusters . end () ) + 
      insert ( ids , hypos    . begin () , hypos    . end () ) ;
  }
  /// insert IDs from the protoparticle 
  inline std::size_t insert ( _LHCbIDs&                  ids , 
                              const LHCb::ProtoParticle* p   ) 
  { 
    if ( 0 == p ) { return 0 ; }
    //
    const SmartRefVector<LHCb::CaloHypo>& calo = p -> calo () ;
    //
    return
      insert ( ids , p -> track   () ) + 
      insert ( ids , p -> muonPID () ) +  
      insert ( ids , calo . begin () , calo . end () ) ;
  }
  /// insert IDs from the particle 
  inline std::size_t insert ( _LHCbIDs& ids           , 
                              const LHCb::Particle* p ) 
  { 
    if ( 0 == p ) { return 0 ; }
    //
    const SmartRefVector<LHCb::Particle>& daughters = p -> daughters () ;
    return
      insert ( ids , p -> proto() ) + 
      insert ( ids , daughters . begin() , daughters . end () ) ;
  }
  /// add IDs from the sequence 
  template <class OBJECT>
  inline std::size_t insert 
  ( _LHCbIDs& ids   ,
    OBJECT   begin  , 
    OBJECT   end    ) 
  {
    //
    std::size_t _size = ids.size() ;
    for ( ; begin != end ; ++begin ) { insert ( ids , *begin ) ; }
    //
    return ids.size() - _size ;
  }
  // ====================================================================
  // The same but with PREDICATE 
  // ====================================================================  
  /// add IDs from the sequence 
  template <class OBJECT, class PREDICATE>
  inline std::size_t insert 
  ( _LHCbIDs& ids    ,
    OBJECT     begin , 
    OBJECT     end   , 
    PREDICATE  good  ) ;
  /// insert ID  
  template <class PREDICATE> 
  inline std::size_t insert ( _LHCbIDs&           ids  , 
                              const LHCb::LHCbID& id   , 
                              PREDICATE           good ) 
  { return 
      !good      ( id )        ? 0 : 
      ids.insert ( id ).second ? 1 : 0 ; }
  /// insert IDs from CaloCellID 
  template <class PREDICATE>
  inline std::size_t insert ( _LHCbIDs& ids , 
                              const LHCb::CaloCellID& id , 
                              PREDICATE good  ) 
  { return 
      !good      ( id )        ? 0 : 
      ids.insert ( id ).second ? 1 : 0 ; }
  /// insert IDs from set 
  template <class PREDICATE> 
  inline std::size_t insert ( _LHCbIDs&       ids  , 
                              const _LHCbIDs& v    , 
                              PREDICATE       good ) 
  { 
    std::size_t _size    = ids.size() ;
    for ( _LHCbIDs::const_iterator i = v.begin() ; v.end() != i ; ++i ) 
    { if ( good (*i) ) { ids.insert ( *i ) ; } }
    return ids.size() - _size ;
  }
  /// insert IDs from vector 
  template <class PREDICATE>
  inline std::size_t insert ( _LHCbIDs&                        ids  , 
                              const std::vector<LHCb::LHCbID>& v    , 
                              PREDICATE                        good ) 
  { 
    std::size_t _size = ids.size() ;
    for ( std::vector<LHCb::LHCbID>::const_iterator i = v.begin() ; v.end() != i ; ++i ) 
    { if ( good (*i) ) {  ids.insert ( *i ) ; } }
    return ids.size() - _size ;
  }
  /// insert IDs from tracks 
  template <class PREDICATE>
  inline std::size_t insert ( _LHCbIDs&          ids  , 
                              const LHCb::Track* t    , 
                              PREDICATE          good )
  { 
    if ( 0 == t ) { return 0 ; }
    return insert ( ids , t->lhcbIDs() , good ) ;
  }
  /// insert IDs from MuonPIDs 
  template <class PREDICATE>
  inline std::size_t insert ( _LHCbIDs&            ids  , 
                              const LHCb::MuonPID* m    , 
                              PREDICATE            good ) 
  { 
    if ( 0 == m ) { return 0 ; }
    return insert ( ids , m -> muonTrack() , good ) ;
  }
  /// insert ID form CaloDigit 
  template <class PREDICATE>
  inline std::size_t insert ( _LHCbIDs& ids               , 
                              const LHCb::CaloDigit* d    , 
                              PREDICATE              good ) 
  { 
    if ( 0 == d ) { return 0 ; }
    return insert ( ids , d -> cellID() , good ) ;
  }
  /// insert IDs from CaloClusterEntry
  template <class PREDICATE>
  inline std::size_t insert ( _LHCbIDs& ids                      , 
                              const LHCb::CaloClusterEntry& e    , 
                              PREDICATE                     good ) 
    
  { return insert ( ids , e.digit() , good ) ; }
  /// insert IDs from CaloCluster 
  template <class PREDICATE>
  inline std::size_t insert ( _LHCbIDs&                ids  ,
                              const LHCb::CaloCluster* c    , 
                              PREDICATE                good ) 
  { 
    if ( 0 == c ) { return 0 ; }
    //
    const  LHCb::CaloCluster::Entries& entries = c->entries() ;
    return insert ( ids              , 
                    entries.begin () , 
                    entries.end   () , good ) ;
  }
  /// insert IDs from CaloHypo
  template <class PREDICATE>
  inline std::size_t insert ( _LHCbIDs&             ids  , 
                              const LHCb::CaloHypo* h    , 
                              PREDICATE             good ) 
  { 
    if ( 0 == h ) { return 0 ; }
    //
    const LHCb::CaloHypo::Digits&   digits   = h -> digits   () ;
    const LHCb::CaloHypo::Clusters& clusters = h -> clusters () ;
    const LHCb::CaloHypo::Hypos&    hypos    = h -> hypos    () ;
    return
      insert ( ids , digits   . begin () , digits   . end () , good ) + 
      insert ( ids , clusters . begin () , clusters . end () , good ) + 
      insert ( ids , hypos    . begin () , hypos    . end () , good ) ;
  }
  /// insert IDs from the protoparticle 
  template <class PREDICATE> 
  inline std::size_t insert ( _LHCbIDs&                  ids  , 
                              const LHCb::ProtoParticle* p    , 
                              PREDICATE                  good ) 
  { 
    if ( 0 == p ) { return 0 ; }
    //
    const SmartRefVector<LHCb::CaloHypo>& calo = p -> calo () ;
    //
    return
      insert ( ids , p -> track   ()                 , good ) + 
      insert ( ids , p -> muonPID ()                 , good ) +  
      insert ( ids , calo . begin () , calo . end () , good ) ;
  }
  /// insert IDs from the particle 
  template <class PREDICATE>
  inline std::size_t insert ( _LHCbIDs& ids              , 
                              const LHCb::Particle* p    , 
                              PREDICATE             good ) 
  { 
    if ( 0 == p ) { return 0 ; }
    //
    const SmartRefVector<LHCb::Particle>& daughters = p -> daughters () ;
    return
      insert ( ids , p -> proto()                             , good ) + 
      insert ( ids , daughters . begin() , daughters . end () , good ) ;
  }
  // add IDs from the sequence 
  template <class OBJECT, class PREDICATE>
  inline std::size_t insert 
  ( _LHCbIDs& ids   ,
    OBJECT    begin , 
    OBJECT    end   , 
    PREDICATE good  ) 
  {
    std::size_t _size = ids.size() ;
    for ( ; begin != end ; ++begin ) { insert ( ids , *begin , good ) ; }
    return ids.size() - _size ;
  }
  // ==========================================================================
  
  // ==========================================================================
  // calculate the actual ID 
  template <class OBJECT> 
  inline std::size_t _hash_id_ ( const OBJECT* obj ) 
  {
    if ( 0 == obj ) { return 0 ; }
    //
    _LHCbIDs ids ;
    insert ( ids , obj ) ;
    //
    return LHCb::HashIDs::hashID ( ids.begin() , ids.end() ) ;
    // ========================================================================
  }
  // ==========================================================================
  template <class OBJECT> 
  inline void _lhcb_IDs_ ( const OBJECT*           obj , 
                           LHCb::HashIDs::LHCbIDs& ids ) 
  {
    //
    _LHCbIDs _ids ;
    insert ( _ids , obj ) ;
    //
    ids.clear() ;
    ids.insert ( ids.end() , _ids.begin() , _ids.end() ) ;
  }
  // ==========================================================================
  template <class OBJECT, class PREDICATE> 
  inline void _lhcb_IDs_ ( const OBJECT*           obj  , 
                           LHCb::HashIDs::LHCbIDs& ids  , 
                           PREDICATE               good ) 
  {
    //
    _LHCbIDs _ids ;
    insert ( _ids , obj , good ) ;
    //
    ids.clear() ;
    ids.insert ( ids.end() , _ids.begin() , _ids.end() ) ;
  }
  // ==========================================================================
  template <class OBJECT> 
  inline void _lhcb_IDs_ ( const OBJECT*               obj  , 
                           LHCb::HashIDs::LHCbIDs&     ids  , 
                           LHCb::LHCbID::channelIDtype type ) 
  {
    LHCb::HashIDs::PMF pmf = _id_to_pmf_ ( type ) ;
    if ( 0 != pmf ) { _lhcb_IDs_ ( obj , ids , std::mem_fun_ref( pmf ) ) ; }
  }
  // ==========================================================================
  //
  inline double _frac ( const std::size_t n , 
                        const std::size_t N ) 
  {
    return
      0 == n ? 0.0 : 
      0 == N ? 0.0 : 
      n == N ? 1.0 : double  ( n ) / N ;
  }
  // ==========================================================================
  /// calculate the overlap 
  inline std::pair<double,double> 
  _overlap_ ( const _LHCbIDs& ids1 , 
              const _LHCbIDs& ids2 )  
  {
    //
    if       (  ids1.empty() ||  ids2.empty() ) { return std::make_pair ( 0.0 , 0.0 ) ; }
    else if  ( &ids1         == &ids2         ) { return std::make_pair ( 1.0 , 1.0 ) ; }
    //
    std::size_t _c =
      std::set_intersection 
      (  ids1 . begin () , ids1 . end () ,
         ids2 . begin () , ids2 . end () , 
         count_iterator<LHCb::LHCbID>() ).count() ;
    //
    return std::make_pair ( _frac ( _c , ids1 . size () ) , 
                            _frac ( _c , ids2 . size () ) ) ;
  }
  // ==========================================================================
  /// calculate the overlap 
  template <class OBJECT>
  inline std::pair<double,double> 
  _overlap_ ( const OBJECT* obj1 , 
              const OBJECT* obj2 ) 
  {
    //
    if ( 0    == obj1 ) { return std::make_pair ( 0.0 , 0.0 ) ; }
    if ( 0    == obj2 ) { return std::make_pair ( 0.0 , 0.0 ) ; }
    if ( obj1 == obj2 ) { return std::make_pair ( 1.0 , 1.0 ) ; }
    //
    _LHCbIDs _ids1 ;
    _LHCbIDs _ids2 ;
    //
    insert ( _ids1 , obj1 ) ;
    insert ( _ids2 , obj2 ) ;
    //
    return _overlap_ ( _ids1 , _ids2 ) ;
  }
  // ==========================================================================
  /// calculate the overlap 
  template <class OBJECT, class PREDICATE>
  inline std::pair<double,double> 
  _overlap_ ( const OBJECT* obj1 , 
              const OBJECT* obj2 , 
              PREDICATE     good ) 
  {
    //
    if ( 0    == obj1 ) { return std::make_pair ( 0.0 , 0.0 ) ; }
    if ( 0    == obj2 ) { return std::make_pair ( 0.0 , 0.0 ) ; }
    if ( obj1 == obj2 ) { return std::make_pair ( 1.0 , 1.0 ) ; }
    //
    _LHCbIDs _ids1 ;
    _LHCbIDs _ids2 ;
    //
    insert ( _ids1 , obj1 , good ) ;
    insert ( _ids2 , obj2 , good ) ;
    //
    return _overlap_ ( _ids1 , _ids2 ) ;
  }
  // ==========================================================================
  template <class OBJECT>
  inline std::pair<double,double> 
  _overlap_ ( const OBJECT*               obj1 , 
              const OBJECT*               obj2 , 
              LHCb::LHCbID::channelIDtype type )
  {
    LHCb::HashIDs::PMF good = _id_to_pmf_ ( type ) ;
    //
    return 
      0 == good ? std::make_pair( 0.0 , 0.0 ) : 
      _overlap_ ( obj1 , obj2 , std::mem_fun_ref ( good ) ) ;
  }
  // ============================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
// hash for MuonID
// ============================================================================
std::size_t LHCb::HashIDs::hashID ( const LHCb::MuonPID*       m )
{ return _hash_id_ ( m ) ; }
// ============================================================================
// hash for CaloCluster 
// ============================================================================
std::size_t LHCb::HashIDs::hashID ( const LHCb::CaloCluster*   c ) 
{ return _hash_id_ ( c ) ; }
// ============================================================================
// hash for CaloHypo
// ============================================================================
std::size_t LHCb::HashIDs::hashID ( const LHCb::CaloHypo*      h ) 
{ return _hash_id_ ( h ) ; }
// ============================================================================
// hash for Track
// ============================================================================
std::size_t LHCb::HashIDs::hashID ( const LHCb::Track*         t ) 
{ return 0 == t ? 0 : hashID ( t->lhcbIDs().begin() , t->lhcbIDs().begin() ) ; }
// ============================================================================
// hash for ProtoParticle 
// ============================================================================
std::size_t LHCb::HashIDs::hashID ( const LHCb::ProtoParticle* p ) 
{ return _hash_id_ ( p ) ; }
// ============================================================================
// hash for Particle 
// ============================================================================
std::size_t LHCb::HashIDs::hashID ( const LHCb::Particle*      p ) 
{ return _hash_id_ ( p ) ; }
// ============================================================================

// ============================================================================
// get hash-ID for the track using only LHCbIDs of certaint type 
// ============================================================================
std::size_t LHCb::HashIDs::hashID ( const LHCb::Track*          t    , 
                                    LHCb::LHCbID::channelIDtype type ) 
{
  return 0 == t ? 0 : hashID ( t , _id_to_pmf_ ( type ) ) ;
}
// ============================================================================
// get hash-ID for the track using only LHCbIDs of certaint type 
// ============================================================================
std::size_t LHCb::HashIDs::hashID ( const LHCb::Track*    t    , 
                                    LHCb::HashIDs::PMF    good ) 
{
  return 
    0 == t    ?  0             :
    0 == good ?  hashID ( t )  : 
    hashID ( t->lhcbIDs().begin() , 
             t->lhcbIDs().end  () , std::mem_fun_ref ( good ) ) ;           
}
// ============================================================================

// ============================================================================
// get IDs for Particle 
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::Particle*      p , 
                              LHCb::HashIDs::LHCbIDs&    ids ) 
{ _lhcb_IDs_ ( p , ids ) ; }
// ============================================================================
// get IDs for ProtoParticle 
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::ProtoParticle* p ,
                              LHCb::HashIDs::LHCbIDs&    ids ) 
{ _lhcb_IDs_ ( p , ids ) ; }
// ============================================================================
// get IDs for Track 
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::Track*         t ,
                              LHCb::HashIDs::LHCbIDs&    ids ) 
{
  if ( 0 != t ) { ids = t->lhcbIDs()  ; }
  else          { ids.clear()         ; }
}
// ============================================================================
// get IDs for CaloHypo
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::CaloHypo*      c ,
                              LHCb::HashIDs::LHCbIDs&    ids ) 
{ _lhcb_IDs_ ( c , ids ) ; }
// ============================================================================
// get IDs for CaloCluster
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::CaloCluster*   c , 
                              LHCb::HashIDs::LHCbIDs&    ids ) 
{ _lhcb_IDs_ ( c , ids ) ; }
// ============================================================================
// get IDs for MuonPID
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::MuonPID*       m ,
                              LHCb::HashIDs::LHCbIDs&    ids ) 
{ _lhcb_IDs_ ( m , ids ) ; }
// ===========================================================================
// The same with predicate 
// ===========================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::Particle*      p   , 
                              LHCb::HashIDs::LHCbIDs&    ids , 
                              LHCb::HashIDs::PMF         pmf )
{ if ( pmf ) { _lhcb_IDs_ ( p , ids , std::mem_fun_ref ( pmf ) ) ; } }
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::ProtoParticle* p   , 
                              LHCb::HashIDs::LHCbIDs&    ids , 
                              LHCb::HashIDs::PMF         pmf )
{ if ( pmf ) { _lhcb_IDs_ ( p , ids , std::mem_fun_ref ( pmf ) ) ; } }
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::Track*         p   , 
                              LHCb::HashIDs::LHCbIDs&    ids , 
                              LHCb::HashIDs::PMF         pmf )
{ if ( pmf ) { _lhcb_IDs_ ( p , ids , std::mem_fun_ref ( pmf ) ) ; } }
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::CaloHypo*      p   , 
                              LHCb::HashIDs::LHCbIDs&    ids , 
                              LHCb::HashIDs::PMF         pmf )
{ if ( pmf ) { _lhcb_IDs_ ( p , ids , std::mem_fun_ref ( pmf ) ) ; } }
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::CaloCluster*   p   , 
                              LHCb::HashIDs::LHCbIDs&    ids , 
                              LHCb::HashIDs::PMF         pmf )
{ if ( pmf ) { _lhcb_IDs_ ( p , ids , std::mem_fun_ref ( pmf ) ) ; } }

// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::MuonPID*       p   , 
                              LHCb::HashIDs::LHCbIDs&    ids , 
                              LHCb::HashIDs::PMF         pmf )
{ if ( pmf ) { _lhcb_IDs_ ( p , ids , std::mem_fun_ref ( pmf ) ) ; } }
// ============================================================================

// ===========================================================================
// The same with predicate 
// ===========================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::Particle*       p    , 
                              LHCb::HashIDs::LHCbIDs&     ids  , 
                              LHCb::LHCbID::channelIDtype type )
{ _lhcb_IDs_ ( p , ids , type ) ; }
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::ProtoParticle*  p   , 
                              LHCb::HashIDs::LHCbIDs&     ids , 
                              LHCb::LHCbID::channelIDtype type )
{ _lhcb_IDs_ ( p , ids , type ) ; }
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::Track*          p   , 
                              LHCb::HashIDs::LHCbIDs&     ids , 
                              LHCb::LHCbID::channelIDtype type )
{ _lhcb_IDs_ ( p , ids , type ) ; }
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::CaloHypo*       p   , 
                              LHCb::HashIDs::LHCbIDs&     ids , 
                              LHCb::LHCbID::channelIDtype type )
{ _lhcb_IDs_ ( p , ids , type ) ; }
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::CaloCluster*    p   , 
                              LHCb::HashIDs::LHCbIDs&     ids , 
                              LHCb::LHCbID::channelIDtype type )
{ _lhcb_IDs_ ( p , ids , type ) ; }
// ============================================================================
void LHCb::HashIDs::lhcbIDs ( const LHCb::MuonPID*        p   , 
                              LHCb::HashIDs::LHCbIDs&     ids , 
                              LHCb::LHCbID::channelIDtype type )
{ _lhcb_IDs_ ( p , ids , type ) ; }
// ============================================================================


// ============================================================================
// overlap for containers 
// ============================================================================
/*  calculate the overlap for two containers
 *  param c1 INPUT the first  container 
 *  param c2 INPUT the second container 
 *  @return the overlap pair:   (#common/#1, #common/#2)
 */
// ============================================================================
std::pair<double,double> LHCb::HashIDs::overlap 
( const LHCb::HashIDs::LHCbIDs& ids1 , 
  const LHCb::HashIDs::LHCbIDs& ids2 ) 
{
  //
  if       (  ids1.empty() ||  ids2.empty() ) { return std::make_pair ( 0.0 , 0.0 ) ; }
  else if  ( &ids1         == &ids2         ) { return std::make_pair ( 1.0 , 1.0 ) ; }
  //
  _LHCbIDs _ids1 ;
  _LHCbIDs _ids2 ;
  //
  insert ( _ids1 , ids1 ) ;
  insert ( _ids2 , ids2 ) ;
  //
  return _overlap_ ( _ids1 , _ids2 ) ;
}
// ============================================================================
std::pair<double,double> LHCb::HashIDs::overlap 
( const LHCb::HashIDs::LHCbIDs& ids1 , 
  const LHCb::HashIDs::LHCbIDs& ids2 ,
  LHCb::HashIDs::PMF            good )
{
  //
  if       (  ids1.empty() ||  ids2.empty() ) { return std::make_pair ( 0.0 , 0.0 ) ; }
  else if  ( &ids1         == &ids2         ) { return std::make_pair ( 1.0 , 1.0 ) ; }
  else if  (  0            == good          ) { return std::make_pair ( 0.0 , 0.0 ) ; }
  //
  _LHCbIDs _ids1 ;
  _LHCbIDs _ids2 ;
  //
  insert ( _ids1 , ids1 , std::mem_fun_ref ( good ) ) ;
  insert ( _ids2 , ids2 , std::mem_fun_ref ( good ) ) ;
  //
  return _overlap_ ( _ids1 , _ids2 ) ;
}
// ============================================================================
std::pair<double,double> LHCb::HashIDs::overlap 
( const LHCb::HashIDs::LHCbIDs& ids1 , 
  const LHCb::HashIDs::LHCbIDs& ids2 ,
  LHCb::LHCbID::channelIDtype   type )
{
  //
  if       (  ids1.empty() ||  ids2.empty() ) { return std::make_pair ( 0.0 , 0.0 ) ; }
  else if  ( &ids1         == &ids2         ) { return std::make_pair ( 1.0 , 1.0 ) ; }
  //
  PMF good = _id_to_pmf_ ( type  ) ;
  if       (  0            == good          ) { return std::make_pair ( 0.0 , 0.0 ) ; }
  //
  _LHCbIDs _ids1 ;
  _LHCbIDs _ids2 ;
  //
  insert ( _ids1 , ids1 , std::mem_fun_ref ( good ) ) ;
  insert ( _ids2 , ids2 , std::mem_fun_ref ( good ) ) ;
  //
  return _overlap_ ( _ids1 , _ids2 ) ;
}
// ============================================================================
// OVERLAP for sorted containers 
// ============================================================================
/* calculate the overlap for two SORTED containers
 *  param c1 INPUT the first  container 
 *  param c2 INPUT the second container 
 *  @return the overlap pair:   (#common/#1, #common/#2)
 */
// ============================================================================
std::pair<double,double> LHCb::HashIDs::overlap_sorted 
( const LHCb::HashIDs::LHCbIDs& ids1 , 
  const LHCb::HashIDs::LHCbIDs& ids2 )
{
  if       (  ids1.empty() ||  ids2.empty() ) { return std::make_pair ( 0.0 , 0.0 ) ; }
  else if  ( &ids1         == &ids2         ) { return std::make_pair ( 1.0 , 1.0 ) ; }
  //
  std::size_t _c =
    std::set_intersection 
    (  ids1 . begin () , ids1 . end () ,
       ids2 . begin () , ids2 . end () , 
       count_iterator<LHCb::LHCbID>() ).count() ;
  //
  return std::make_pair ( _frac ( _c , ids1 . size () ) , 
                          _frac ( _c , ids2 . size () ) ) ;
}
// ============================================================================
/*  calculate the overlap for two SORTED containers
 *  param c1 INPUT the first  container 
 *  param c2 INPUT the second container 
 *  @return the overlap pair:   (#common/#1, #common/#2)
 */
// ============================================================================
std::pair<double,double> LHCb::HashIDs::overlap_sorted 
( const LHCb::HashIDs::LHCbIDs& ids1 , 
  const LHCb::HashIDs::LHCbIDs& ids2 , 
  LHCb::HashIDs::PMF            good ) 
  
{
  if       (  ids1.empty() ||  ids2.empty() ) { return std::make_pair ( 0.0 , 0.0 ) ; }
  else if  ( &ids1         == &ids2         ) { return std::make_pair ( 1.0 , 1.0 ) ; }
  else if  (  0            ==  good         ) { return std::make_pair ( 0.0 , 0.0 ) ; }
  //
  LHCb::HashIDs::LHCbIDs _ids1 ; _ids1.reserve ( ids1.size() ) ;
  LHCb::HashIDs::LHCbIDs _ids2 ; _ids2.reserve ( ids2.size() ) ;
  //
  std::copy_if ( ids1.begin() , ids1.end() , std::back_inserter ( _ids1 ) , std::mem_fun_ref ( good ) ) ;
  if ( _ids1.empty() ) { return std::make_pair ( 0.0 , 0.0 ) ; }
  //
  std::copy_if ( ids2.begin() , ids2.end() , std::back_inserter ( _ids2 ) , std::mem_fun_ref ( good ) ) ;
  if ( _ids2.empty() ) { return std::make_pair ( 0.0 , 0.0 ) ; }
  //
  return overlap ( _ids1 , _ids2 ) ;
}
// ============================================================================
/*  calculate the overlap for two SORTED containers
 *  param c1 INPUT the first  container 
 *  param c2 INPUT the second container 
 *  @return the overlap pair:   (#common/#1, #common/#2)
 */
// ============================================================================
std::pair<double,double> LHCb::HashIDs::overlap_sorted 
( const LHCb::HashIDs::LHCbIDs& ids1 , 
  const LHCb::HashIDs::LHCbIDs& ids2 , 
  LHCb::LHCbID::channelIDtype   type )
{
  if       (  ids1.empty() ||  ids2.empty() ) { return std::make_pair ( 0.0 , 0.0 ) ; }
  else if  ( &ids1         == &ids2         ) { return std::make_pair ( 1.0 , 1.0 ) ; }
  //
  return overlap ( ids1 , ids2 , _id_to_pmf_ ( type ) ) ;
}

// ============================================================================
std::pair<double,double> 
LHCb::HashIDs::overlap ( const LHCb::Particle*      p1 , 
                         const LHCb::Particle*      p2 ) 
{ return _overlap_ ( p1 , p2 ) ; }
// ============================================================================
std::pair<double,double> 
LHCb::HashIDs::overlap ( const LHCb::ProtoParticle* p1 , 
                         const LHCb::ProtoParticle* p2 ) 
{ return _overlap_ ( p1 , p2 ) ; }
// ============================================================================
std::pair<double,double> 
LHCb::HashIDs::overlap ( const LHCb::Track*         t1 , 
                         const LHCb::Track*         t2 ) 
{ return _overlap_ ( t1 , t2 ) ; }
// ============================================================================
std::pair<double,double> 
LHCb::HashIDs::overlap ( const LHCb::CaloCluster*   c1 , 
                         const LHCb::CaloCluster*   c2 ) 
{ return _overlap_ ( c1 , c2 ) ; }
// ============================================================================
std::pair<double,double> 
LHCb::HashIDs::overlap ( const LHCb::CaloHypo*      c1 , 
                         const LHCb::CaloHypo*      c2 ) 
{ return _overlap_ ( c1 , c2 ) ; }
// ============================================================================
std::pair<double,double> 
LHCb::HashIDs::overlap ( const LHCb::MuonPID*       m1 , 
                         const LHCb::MuonPID*       m2 ) 
{ return _overlap_ ( m1 , m2 ) ; }
// ============================================================================
// check overlap for certaint type of LHCbIDs only
// ============================================================================
/*  check overlap for certaint type of LHCbIDs only 
 *  @param p1   INPUT the first  particle 
 *  @param p2   INPUT the second particle
 *  @param good INPUT criteria for LHCbIDs 
 *  @return the overlap pair 
 */
// ============================================================================
std::pair<double,double> 
LHCb::HashIDs::overlap ( const LHCb::Particle* p1   , 
                         const LHCb::Particle* p2   , 
                         LHCb::HashIDs::PMF    good ) 
{
  return 
    good ?
    _overlap_      ( p1  , p2  , std::mem_fun_ref( good ) ) : 
    std::make_pair ( 0.0 , 0.0 ) ;
}
// ============================================================================
/*  check overlap for certaint type of LHCbIDs only 
 *  @param p1   INPUT the first  particle 
 *  @param p2   INPUT the second particle
 *  @param good INPUT criteria for LHCbIDs 
 *  @return the overlap pair 
 */
// ============================================================================
std::pair<double,double> 
LHCb::HashIDs::overlap ( const LHCb::Track* p1   , 
                         const LHCb::Track* p2   , 
                         LHCb::HashIDs::PMF good ) 
{
  return 
    good ?
    _overlap_      ( p1  , p2  , std::mem_fun_ref( good ) ) : 
    std::make_pair ( 0.0 , 0.0 ) ;
}
// ============================================================================
/*  check overlap for certaint type of LHCbIDs only 
 *  @param p1   INPUT the first  particle 
 *  @param p2   INPUT the second particle
 *  @param type INPUT the type of LHCbID to consider 
 *  @return the overlap pair 
 */
// ============================================================================
std::pair<double,double> 
LHCb::HashIDs::overlap ( const LHCb::Particle*       p1   , 
                         const LHCb::Particle*       p2   , 
                         LHCb::LHCbID::channelIDtype type ) 
{ return _overlap_ ( p1  , p2  , type ) ; }
// ============================================================================
/*  check overlap for certaint type of LHCbIDs only 
 *  @param p1   INPUT the first  particle 
 *  @param p2   INPUT the second particle
 *  @param type INPUT the type of LHCbID to consider 
 *  @return the overlap pair 
 */
// ============================================================================
std::pair<double,double> 
LHCb::HashIDs::overlap ( const LHCb::Track*          p1   , 
                         const LHCb::Track*          p2   , 
                         LHCb::LHCbID::channelIDtype type ) 
{ return _overlap_ ( p1  , p2  , type ) ; }
// ============================================================================
// Event 
// ============================================================================
