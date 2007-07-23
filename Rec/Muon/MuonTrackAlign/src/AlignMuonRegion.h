// $Id: AlignMuonRegion.h,v 1.1.1.1 2007-07-23 07:14:36 asatta Exp $
#ifndef ALIGNMUONREGION_H 
#define ALIGNMUONREGION_H 1

// Include files

#include "AlignMuonPoint.h"

/** @class AlignMuonRegion AlignMuonRegion.h
 *  Class to handle the various functions of a refion during Hlt Muon ID
 *
 *  @author Olivier Callot
 *  @date   2003-06-06
 */
class AlignMuonRegion {
public:
  /// Standard constructor
  
  AlignMuonRegion( ) {
    m_points.reserve(100);
    m_tiles.reserve(100);

  }; 

  virtual ~AlignMuonRegion( ){}; ///< Destructor


  void addPoint( double x,  double y, double z,
                 LHCb::MuonTileID ref ) {
    m_points.push_back( AlignMuonPoint( x,  y,  z,ref ) );
  }

  void clearPoints() { 
    m_points.clear();   
  }
  void clearTiles() {  
    m_tiles.clear();
  }


  void addTile( LHCb::MuonTileID t )      { m_tiles.push_back( t ); }

  std::vector<LHCb::MuonTileID>& tiles()  { return m_tiles; }
  std::vector<AlignMuonPoint>& points()  { return m_points; }

  void setLayoutNumber(unsigned int n){m_numberOfLayout=n;}

  unsigned int layoutNumber(){return m_numberOfLayout;}
  
  void  setLayoutGridX(unsigned int layout, unsigned int x){
    m_layoutX[layout]=x;}

  void  setLayoutGridY(unsigned int layout, unsigned int y){
    m_layoutY[layout]=y;}
   


  
unsigned int layoutGridX(unsigned int layout){return m_layoutX[layout];}
  unsigned int layoutGridY(unsigned int layout){return m_layoutY[layout];
  }
  

protected:

private:

  std::vector<AlignMuonPoint> m_points;
  std::vector<LHCb::MuonTileID> m_tiles;
  unsigned int m_numberOfLayout;
  unsigned int m_layoutX[2];
  unsigned int m_layoutY[2];
  // double m_Ymax;
  //double m_Xmax;
  
};
#endif // ALIGNMUONREGION_H
