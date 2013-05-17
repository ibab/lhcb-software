// $Id: $
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
// ============================================================================
// local
// ============================================================================
#include "Kernel/HashIDs.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/functional/hash.hpp"
// ============================================================================
/** @file 
 *  Implementation file for class : LHCb::HashID
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-11-03
 *
 *  Version           $Revision:$
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
  //
  // add IDs from the sequence 
  template <class OBJECT>
  inline std::size_t insert 
  ( _LHCbIDs& ids   ,
    OBJECT   begin , 
    OBJECT   end   ) ;
  // 
  // add ID 
  //inline std::size_t insert ( _LHCbIDs& ids , const LHCb::LHCbID&    id ) 
  //{ return ids.insert ( id ).second ? 1 : 0 ; }
  //
  // add ID from Calorimeter Cell
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::CaloCellID& id ) 
  { return ids.insert ( id ).second ? 1 : 0 ; }
  // 
  // insert IDs from set of IDs
  // inline std::size_t insert ( _LHCbIDs& ids , const _LHCbIDs& v ) 
  // { 
  //   std::size_t _size = ids.size() ;
  //   ids.insert ( v.begin() , v.end() ) ;
  //   return ids.size() - _size ;
  // }
  // insert IDs from vector of IDs
  inline std::size_t insert ( _LHCbIDs& ids , const std::vector<LHCb::LHCbID>& v ) 
  { 
    std::size_t _size = ids.size() ;
    ids.insert ( v.begin() , v.end() ) ;
    return ids.size() - _size ;
  }
  //
  // insert IDs from tracks 
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::Track* t  ) 
  { 
    if ( 0 == t ) { return 0 ; }
    return insert ( ids , t->lhcbIDs() ) ;
  }
  // 
  // insert IDs from MuonPIDs 
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::MuonPID* m  ) 
  { 
    if ( 0 == m ) { return 0 ; }
    return insert ( ids , m -> muonTrack() ) ;
  }
  // 
  // insert IDs from CaloDigit
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::CaloDigit* d ) 
  { 
    if ( 0 == d ) { return 0 ; }
    return insert ( ids , d -> cellID()  ) ;
  }
  //
  // insert IDs from CaloClusterEntry
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::CaloClusterEntry& e ) 
  { return insert ( ids , e.digit() ) ; }
  //
  // insert IDs from CaloCluster 
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::CaloCluster* c ) 
  { 
    if ( 0 == c ) { return 0 ; }
    //
    const  LHCb::CaloCluster::Entries& entries = c->entries() ;
    return insert ( ids              , 
                    entries.begin () , 
                    entries.end   () ) ;
  }
  //
  // insert IDs from CaloHypo
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
  // 
  // insert IDs from the protoparticle 
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::ProtoParticle* p ) 
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
  //
  // insert IDs from the particle 
  inline std::size_t insert ( _LHCbIDs& ids , const LHCb::Particle* p ) 
  { 
    if ( 0 == p ) { return 0 ; }
    //
    const SmartRefVector<LHCb::Particle>& daughters = p -> daughters () ;
    return
      insert ( ids , p -> proto() ) + 
      insert ( ids , daughters . begin() , daughters . end () ) ;
  }
  //
  // add IDs from the sequence 
  template <class OBJECT>
  inline std::size_t insert 
  ( _LHCbIDs& ids   ,
    OBJECT   begin , 
    OBJECT   end   ) 
  {
    //
    std::size_t _size = ids.size() ;
    for ( ; begin != end ; ++begin ) { insert ( ids , *begin ) ; }
    //
    return ids.size() - _size ;
  }
  //  
  // calculate the actual ID 
  template <class OBJECT> 
  inline std::size_t _hash_id_ ( const OBJECT* obj ) 
  {
    if ( 0 == obj ) { return 0 ; }
    //
    _LHCbIDs ids ;
    insert ( ids , obj ) ;
    //
    std::size_t seed = 0 ;
    using namespace boost ;
    for ( _LHCbIDs::const_iterator i = ids.begin() ; ids.end() != i ; ++i ) 
    { hash_combine ( seed , i->channelID () ) ; }
    // 
    return seed ;
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
  inline double _frac ( const std::size_t n , 
                        const std::size_t N ) 
  {
    return
      0 == n ? 0.0 : 
      0 == N ? 0.0 : 
      n == N ? 1.0 : double  ( n ) / N ;
  }
  // ==========================================================================
  template<typename TYPE>
  class count_iterator
    : public std::iterator<std::output_iterator_tag, void, void, void, void>
  {
  public:
    // ========================================================================
    /// Constructor 
    count_iterator() : m_count ( 0 ) {}
    //
    count_iterator& operator = ( const TYPE& /* value */ )
    {
      ++m_count ;
      return *this ;
    }
    //
    count_iterator& operator*  ()    { return *this; }
    count_iterator& operator++ ()    { return *this; }
    count_iterator& operator++ (int) { return *this; }
    // 
    // ========================================================================
  public:
    // ========================================================================
    /// the the counter 
    unsigned long count() const { return m_count ; }
    // ========================================================================    
  private:
    ///
    unsigned long m_count ;
    // ========================================================================
  };
  // ==========================================================================
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
    //     LHCb::HashIDs::LHCbIDs ids ;
    //     ids.reserve ( std::min ( _ids1 . size  () , _ids2 . size () ) ) ;
    //     std::set_intersection (  _ids1 . begin () , _ids1 . end () ,
    //                              _ids1 . begin () , _ids1 . end () , 
    //                              std::back_inserter ( ids ) ) ;
    //     const std::size_t _c = ids.size() ;
    //
    std::size_t _c =
      std::set_intersection 
      (  _ids1 . begin () , _ids1 . end () ,
         _ids2 . begin () , _ids2 . end () , 
         count_iterator<LHCb::LHCbID>() ).count() ;
    //
    return std::make_pair ( _frac ( _c , _ids1 . size () ) , 
                            _frac ( _c , _ids2 . size () ) ) ;
  }
  // ==========================================================================
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
{ return _hash_id_ ( t ) ; }
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

// ============================================================================
// Event 
// ============================================================================
