#ifndef ALIGNMUONPOINT_H 
#define ALIGNMUONPOINT_H 1

// Include files
#include "Kernel/MuonTileID.h"
/** @class AlignMuonPoint AlignMuonPoint.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2003-06-06
 */
class AlignMuonPoint {

public:
  AlignMuonPoint ( double x,  double y, double z,
              LHCb::MuonTileID tile ) :
    m_x(x),  m_y(y) ,m_z(z), m_tile(tile)  {};
  AlignMuonPoint(){};
  double x()         const { return m_x;      }
  double y()         const { return m_y;      }
  double z()         const { return m_z;      }
  LHCb::MuonTileID tile()  const { return m_tile;   }
private:
  double m_x;
  double m_y;
  double m_z;
  LHCb::MuonTileID m_tile;
};
#endif // ALIGNMUONPOINT_H
