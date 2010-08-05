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
  IsMuonTile
  ( const LHCb::MuonTileID& tile    , 
    const unsigned int      station ) 
    : m_tile    ( tile    ) 
    , m_station ( station ) 
  {}
  /// the main method:
  bool operator() ( const LHCb::LHCbID& id ) 
  { 
    return 
      id.isMuon() && ( id.muonID().station() == m_station ) && ( id == m_tile ) ;  
  }
  ///
private:
  // ==========================================================================
  IsMuonTile() ;
  // ==========================================================================
private:
  // ==========================================================================
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
  if ( 0 == track )       { return false ; }
  typedef std::vector< LHCb::LHCbID > LHCbIDs ;
  const LHCbIDs& ids= track -> lhcbIDs() ;
  //
  LHCbIDs::const_iterator iM1 = 
    std::find_if ( ids.begin() , ids.end() , IsMuonTile ( id1 , 0 ) ) ;
  //
  if ( ids.end() == iM1 ) { return false    ; }    // CONTINUE 
  //
  LHCbIDs::const_iterator iM2 = 
    std::find_if ( ids.begin() , ids.end() , IsMuonTile ( id2 , 1 ) ) ;
  //
  return ids.end() != iM2 ;
}
// ============================================================================
// The END 
// ============================================================================
#endif // COMPONENTS_ISMUONTILE_H
// ============================================================================
