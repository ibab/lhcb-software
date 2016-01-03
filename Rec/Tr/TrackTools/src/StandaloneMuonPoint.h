#ifndef STANDALONEMUONPOINT_H 
#define STANDALONEMUONPOINT_H 1

// Include files
#include "Kernel/MuonTileID.h"
/** @class StandaloneMuonPoint StandaloneMuonPoint.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2003-06-06
 *
 * Removed from Hlt/HltMuon and ported to Tr/TrackTools
 *
 */
class StandaloneMuonPoint {

public:
  StandaloneMuonPoint ( double x,  double y, 
              LHCb::MuonTileID tile ) :
    m_x(x),  m_y(y), m_tile(tile)  {};
  StandaloneMuonPoint() :
    m_x( 0 ), m_y( 0 ), m_tile()   {};
  double x()         const { return m_x;      }
  double y()         const { return m_y;      }
  LHCb::MuonTileID tile()  const { return m_tile;   }
private:
  double m_x;
  double m_y;
  LHCb::MuonTileID m_tile;
};
#endif // STANDALONEMUONPOINT_H
