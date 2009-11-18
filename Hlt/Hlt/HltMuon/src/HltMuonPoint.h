// $Id: HltMuonPoint.h,v 1.2 2009-11-18 13:19:45 dgolubko Exp $
#ifndef HLTMUONPOINT_H 
#define HLTMUONPOINT_H 1

// Include files
#include "MuonKernel/MuonTile.h"
#include "Kernel/MuonTileID.h"
/** @class HltMuonPoint HltMuonPoint.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2003-06-06
 */
class HltMuonPoint {

public:
  HltMuonPoint ( double x,  double y, 
              LHCb::MuonTileID tile ) :
    m_x(x),  m_y(y), m_tile(tile)  {};
  HltMuonPoint() :
    m_x( 0 ), m_y( 0 ), m_tile()   {};
  double x()         const { return m_x;      }
  double y()         const { return m_y;      }
  LHCb::MuonTileID tile()  const { return m_tile;   }
private:
  double m_x;
  double m_y;
  LHCb::MuonTileID m_tile;
};
#endif // HLTMUONPOINT_H
