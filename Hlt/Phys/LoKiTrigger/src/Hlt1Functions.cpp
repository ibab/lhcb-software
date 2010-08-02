// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// GauduKernel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Hlt1Functions.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/lambda/bind.hpp"
// ============================================================================
/** @file 
 *  implementation file for classed form the file LoKi/Hlt1Functions.h
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-30
 */  
// ============================================================================
namespace 
{
  // ==========================================================================
  /** the default cut on muon-delta p
   *  @see LoKi::Tracks::MuonDeltaP 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-03-30
   */
  const double s_muonDeltaP_delta = -2 ;     // the default cut on muon-delta p
  // ==========================================================================
  /// number of regions in muon system 
  const size_t s_nRegions = 4 ;             // number of regions in muon system 
  /** the default p-cuts 
   *  @see LoKi::Tracks::MuonDeltaP 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-03-30
   */
  const double s_muonDeltaP_pCuts [ s_nRegions ] = {  
    60  * Gaudi::Units::GeV ,
    30  * Gaudi::Units::GeV , 
    15  * Gaudi::Units::GeV , 
    7.5 * Gaudi::Units::GeV } ;                           // the default p-cuts 
  // ========================================================================== 
}
// ============================================================================
// constructor from delta-cut  and list of p-cuts
// ============================================================================
LoKi::Tracks::MuonDeltaP::MuonDeltaP
( const double               delta ,
  const std::vector<double>& cuts  ) 
  : LoKi::TrackTypes::TrCuts () 
  , m_deltaP ( delta ) 
  , m_pCuts  ( cuts  ) 
{
  Assert ( m_pCuts.size() == s_nRegions     ,
           "Invalid numbef of Muon-regions" ) ;  
}
// ============================================================================
// constructor from delta-cut  
// ============================================================================
LoKi::Tracks::MuonDeltaP::MuonDeltaP
( const double               delta ) 
  : LoKi::TrackTypes::TrCuts () 
  , m_deltaP ( delta ) 
  , m_pCuts  ( s_muonDeltaP_pCuts , s_muonDeltaP_pCuts + s_nRegions ) 
{
  Assert ( m_pCuts.size() == s_nRegions     ,
           "Invalid numbef of Muon-regions" ) ;  
}
// ============================================================================
// constructor from the list of p-cuts
// ============================================================================
LoKi::Tracks::MuonDeltaP::MuonDeltaP
( const std::vector<double>& cuts  ) 
  : LoKi::TrackTypes::TrCuts () 
  , m_deltaP ( s_muonDeltaP_delta ) 
  , m_pCuts  ( cuts ) 
{
  Assert ( m_pCuts.size() == s_nRegions     ,
           "Invalid numbef of Muon-regions" ) ;  
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::MuonDeltaP::MuonDeltaP()
  : LoKi::TrackTypes::TrCuts () 
  , m_deltaP ( s_muonDeltaP_delta ) 
  , m_pCuts  ( s_muonDeltaP_pCuts , s_muonDeltaP_pCuts + s_nRegions ) 
{
  Assert ( m_pCuts.size() == s_nRegions     ,
           "Invalid numbef of Muon-regions" ) ;  
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Tracks::MuonDeltaP*
LoKi::Tracks::MuonDeltaP::clone() const { return new MuonDeltaP(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Tracks::MuonDeltaP::result_type 
LoKi::Tracks::MuonDeltaP::operator() 
  ( LoKi::Tracks::MuonDeltaP::argument t ) const 
{
  if ( 0 == t ) 
  {
    Error ("LHCb::Track* points to NULL, return 'false'") ;
    return false ;  
  }
  
  // check ancestors :
  if ( t->ancestors().empty() ) { return false ; }                     // RETURN 
  // get the oldest ancestor
  const LHCb::Track* old = t->ancestors().front() ;
  if ( 0 == old               ) { return false ; }                     // RETURN
  // get the momentum of the oldest ancestor:
  const double pNew = t  -> p () ;
  if ( 0 >= pNew             ) { return false ; }                     // RETURN 
  const double pOld = old-> p () ;
  if ( 0 >= pOld             ) { return false ; }                     // RETURN 
  
  // get the momentum difference:
  const double delta = ( pNew - pOld ) / pNew ;
  
  // for "bad" matching apply the region-dependent cut on P:
  if ( delta < m_deltaP ) 
  {
    // try to find a region for muon
    typedef std::vector<LHCb::LHCbID> LHCbIDs ;
    
    // get all IDs from the track 
    const LHCbIDs& ids = t->lhcbIDs() ;
    
    // loop over all IDs in backward direction (why???) and find muon tile
    using namespace boost::lambda ;
    LHCbIDs::const_reverse_iterator itile = 
      std::find_if ( ids.rbegin() , ids.rend () ,
                     bind( &LHCb::LHCbID::isMuon , _1 ) ) ;
    if ( ids.rend() == itile ) { return false ; }                     // RETURN
    
    const LHCb::MuonTileID tile = itile->muonID() ;
    const unsigned int muReg = tile.region() ;
    Assert ( muReg < s_nRegions , "Illegal muon region!" ) ;
    
    if ( pNew < m_pCuts[muReg] ) { return false ; }                   // RETURN 
    //
  }
  
  // and finally : 
  return true ;                                                       // RETURN 
}
// ============================================================================
// OPTIONAL: the nice printput 
// ============================================================================
std::ostream& LoKi::Tracks::MuonDeltaP::fillStream  
( std::ostream& s ) const 
{
  if      ( s_muonDeltaP_delta == m_deltaP        && 
            s_nRegions         == m_pCuts.size () && 
            std::equal ( m_pCuts.begin ()   ,  
                         m_pCuts.end   ()   , 
                         s_muonDeltaP_pCuts )  ) 
  { return  s << "TrDPMU" ; }                                         // RETURN
  else if ( s_muonDeltaP_delta == m_deltaP        ) 
  { return  s << "TrDPMU_(" << Gaudi::Utils::toString ( m_pCuts ) << ")" ; }
  else if ( s_nRegions         == m_pCuts.size () && 
            std::equal ( m_pCuts.begin ()   ,  
                         m_pCuts.end   ()   , 
                         s_muonDeltaP_pCuts )  ) 
  { return  s << "TrDPMU_(" << m_deltaP << ")" ; }                    // RETURN
  //
  return    s << "TrDPMU_(" << m_deltaP << "," 
              << Gaudi::Utils::toString ( m_pCuts ) << ")" ;
}
// ============================================================================
// The END 
// ============================================================================
