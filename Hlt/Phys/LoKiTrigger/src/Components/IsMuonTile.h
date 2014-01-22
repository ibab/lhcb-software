// $Id$
// ============================================================================
#ifndef COMPONENTS_ISMUONTILE_H 
#define COMPONENTS_ISMUONTILE_H 1
// ============================================================================
// $URL$
// ============================================================================
// STD&STL 
// ============================================================================
#include <functional>
// ============================================================================
// Include files
// ============================================================================
#include "Kernel/MuonTileID.h"
#include "Kernel/LHCbID.h"
// ============================================================================
struct IsMuonTile : public std::unary_function<LHCb::LHCbID,bool>
{
  /// constructor:
  IsMuonTile() = delete  ;
  /// constructor:
  IsMuonTile
  ( const LHCb::MuonTileID& tile    , 
    const unsigned int      station ) 
    : m_tile    ( tile    ) 
    , m_station ( station ) 
  { // TODO: check that m_file is actually in m_station??
    // assert( tile.station()==station );
    // why pass a station actually???
  }
  /// the main method:
  bool operator() ( const LHCb::LHCbID& id ) 
  { 
    return 
      id.isMuon() && ( id.muonID().station() == m_station ) && ( id == m_tile ) ;  
  }
  ///
private:
  /// muon tile ID 
  LHCb::MuonTileID m_tile    ;
  /// muon station 
  unsigned int     m_station ;
  // ==========================================================================
};
// ============================================================================
inline bool isMuonClone 
( const LHCb::Track*      track  , 
  const LHCb::MuonTileID& id1    , 
  const LHCb::MuonTileID& id2    ) 
{
  if ( !track )       { return false ; }
  const auto& ids = track -> lhcbIDs() ;
  return std::any_of( std::begin(ids), std::end(ids) , IsMuonTile{ id1 , 0 } )  
      && std::any_of( std::begin(ids), std::end(ids) , IsMuonTile{ id2 , 1 } ) ;
  //
}
// ============================================================================
// The END 
// ============================================================================
#endif // COMPONENTS_ISMUONTILE_H
// ============================================================================
