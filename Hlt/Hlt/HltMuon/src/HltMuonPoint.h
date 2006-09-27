// $Id: HltMuonPoint.h,v 1.1 2006-09-27 13:55:23 cattanem Exp $
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
  HltMuonPoint(){};
  double x()         const { return m_x;      }
  double y()         const { return m_y;      }
  LHCb::MuonTileID tile()  const { return m_tile;   }
private:
  double m_x;
  double m_y;
  LHCb::MuonTileID m_tile;
};
#endif // HLTMUONPOINT_H
