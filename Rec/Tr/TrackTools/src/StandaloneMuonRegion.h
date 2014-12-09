// $Id: StandaloneMuonRegion.h,v 1.1 2006-09-27 13:55:23 cattanem Exp $
#ifndef STANDALONEMUONREGION_H 
#define STANDALONEMUONREGION_H 1

// Include files

#include "StandaloneMuonPoint.h"

/** @class StandaloneMuonRegion StandaloneMuonRegion.h
 *  Class to handle the various functions of a refion during Hlt Muon ID
 *
 *  @author Olivier Callot
 *  @date   2003-06-06
 *
 * Removed from Hlt/HltMuon and ported to Tr/TrackTools
 *
 *  @author Paul Seyfert
 *  @date   2011-03-03
 */
class StandaloneMuonRegion {
public:
  /// Standard constructor
  
  StandaloneMuonRegion( ) {
    m_points.reserve(100);
    m_tiles.reserve(100);

  }; 

  virtual ~StandaloneMuonRegion( ){}; ///< Destructor


  void addPoint( double x,  double y,
                 LHCb::MuonTileID ref ) {
    m_points.push_back( StandaloneMuonPoint( x,  y,  ref ) );
  }

  void clearPoints() { 
    m_points.clear();   
  }
  void clearTiles() {  
    m_tiles.clear();
  }


  void addTile( LHCb::MuonTileID t )      { m_tiles.push_back( t ); }

  std::vector<LHCb::MuonTileID>& tiles()  { return m_tiles; }
  std::vector<StandaloneMuonPoint>& points()  { return m_points; }

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

  std::vector<StandaloneMuonPoint> m_points;
  std::vector<LHCb::MuonTileID> m_tiles;
  unsigned int m_numberOfLayout;
  unsigned int m_layoutX[2];
  unsigned int m_layoutY[2];
  // double m_Ymax;
  //double m_Xmax;
  
};
#endif // STANDALONEMUONREGION_H
