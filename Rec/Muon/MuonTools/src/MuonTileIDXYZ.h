// $Id: MuonTileIDXYZ.h,v 1.7 2002-10-21 20:43:54 asatta Exp $
#ifndef MUONTILEIDXYZ_H 
#define MUONTILEIDXYZ_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// local
#include "MuonTools/IMuonTileXYZTool.h"
#include "MuonDet/MuonBasicGeometry.h"

/** @class MuonTileIDXYZ MuonTileIDXYZ.h
 *  
 *  Convert an MuonTileID into an xyz position in the detector (with size)
 *  No abstract interface as I do not want to make more than one of these...
 *
 *  @author David Hutchcroft
 *  @date   07/03/2002
 */
class MuonTileIDXYZ : public AlgTool, 
                      virtual public IMuonTileXYZTool {
public:
  /// Standard constructor
  MuonTileIDXYZ( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);
  
  virtual ~MuonTileIDXYZ( ) ; ///< Destructor

  /** Calculate the x,y,z and dx,dy,dz of a MuonTileID in mm
   * this ignores gaps: these can never be read out independently
   */
  virtual StatusCode calcTilePos(const MuonTileID& tile, 
                                 double& x, double& deltax,
                                 double& y, double& deltay,
                                 double& z, double& deltaz);
  
  /// locate the MuonTileID of the chamber containing x,y,z (in mm)
  virtual StatusCode locateChamberTile(const double& x, 
                                       const double& y, 
                                       const double& z, 
                                       MuonTileID& tile);

  /// locate the MuonTileID of the chamber containing x,y,z (in mm)
  /// and get the associated gas gap pointer
  virtual StatusCode locateChamberTileAndGap(const double& x, 
                                             const double& y, 
                                             const double& z, 
                                             MuonTileID& tile,
                                             DeMuonGasGap* &pGasGap);

  /** Calculate the position of the Muon front end Channel
   *  containing x,y,z (in mm)
   */
  virtual StatusCode locateFEFromXYZ(const double& x, 
                                     const double& y, 
                                     const double& z, 
                                     MuonTileID &chamber,
                                     std::vector<MuonChannel> &muonChannels,
                                     DeMuonGasGap* &pGasGap);
private:

  /// get position of chamber from a tile
  StatusCode getXYZChamberTile(const MuonTileID& tile, 
                               double& x, double& deltax,
                               double& y, double& deltay,
                               double& z, double& deltaz);

  /// get position of a "named" chamber 
  /// NOTE: station and region are indexed from 0 (C style)
  /// chamberNum is the real chamber number (from 0)
  StatusCode getXYZChamber(const int& station,
                           const int& region,
                           const int& chamberNum,
                           double& x, double& deltax,
                           double& y, double& deltay,
                           double& z, double& deltaz);

  /// get position of a "named" gasgap
  /// NOTE: station, region and gapNum are indexed from 0 (C style)
  /// chamberNum is the real chamber number (from 0)
  StatusCode getXYZGasGap(const int& station,
                          const int& region,
                          const int& chamberNum,
                          const int& gapNum,
                          double& x, double& deltax,
                          double& y, double& deltay,
                          double& z, double& deltaz);
  
  /// get position of chamber or gas gap with caching of results and pointers
  /// NOTE: station, region and gapNum are indexed from 0 (C style)
  /// chamberNum is the real chamber number (from 0)
  StatusCode getXYZAndCache(const int& station,
                            const int& region,
                            const int& chamberNum,
                            const int& gapNum,
                            double& x, double& deltax,
                            double& y, double& deltay,
                            double& z, double& deltaz);

  /// fill the array with the position and size of the stations
  StatusCode fillStationExtent();

  /// fill the array of the size and extent of the twelfths of each region
  StatusCode fillTwelfthsExtent();

  /// fill the array with the number of gaps per region
  StatusCode fillNGaps();

  /// fill the arrays with the grid size of the chambers, strips and pads
  StatusCode fillGridSizes();

  /// get the chamber number (counts from 1) for the tile
  int getChamberNumber(const MuonTileID& tile);

  /// get postion of logical channel (may be multiple chambers)
  StatusCode getXYZLogical(const MuonTileID& tile, 
                           double& x, double& deltax,
                           double& y, double& deltay,
                           double& z, double& deltaz);
  
  /// get xyz of specific pad
  StatusCode getXYZPad(const MuonTileID& tile, 
                       double& x, double& deltax,
                       double& y, double& deltay,
                       double& z, double& deltaz);

  /// get xyz of twelfth (useful for defining regions)
  StatusCode getXYZTwelfth(const MuonTileID& tile, 
                           double& x, double& deltax,
                           double& y, double& deltay,
                           double& z, double& deltaz);

  /// locate the GasGap given the station, region and quater  
  /// and return the gas gap pointer
  StatusCode locateGasGapFromXYZ( const int& station,
                                  const int& region,
                                  const int& quarter,
                                  const double& x,
                                  const double& y,
                                  const double& z,
                                  MuonTileID& tile,
                                  DeMuonGasGap* &pGasGap);

  /// returns the chamber number (same for each station) on the corner of
  /// the region
  int getTwelfthCorner(const int& region, 
                       const int& twelfth,
                       const int& chamberNum);

  /// get the xIndex and yIndex of the corner chamber in the twelfth
  void getTwelfthCornerIndex(const int& region, 
                             const int& twelfth,
                             const int& chamberNum,
                             int &xPos, int &yPos);

  int numberOfChambers(const int &region);

  // Number of stations
  int m_NStation;
  // Number of regions
  int m_NRegion;
  //Names of the station
  std::vector<std::string> m_stationNames;
  

  // size of logical channels
  std::vector<unsigned int> m_logChanVertGridX;
  std::vector<unsigned int> m_logChanVertGridY;
  std::vector<unsigned int> m_logChanHorizGridX;
  std::vector<unsigned int> m_logChanHorizGridY;
  // size of pads
  std::vector<unsigned int> m_padGridX;
  std::vector<unsigned int> m_padGridY;

  // store x,y,z,dx,dy,dz of chambers locally (Cached) to avoid multiple
  // TDS lookups (the other choice is to cache the smartpointers)
  class muonExtent_ {
  public:
    double x;
    double y;
    double z;
    double dx;
    double dy;
    double dz;
  } ;

  // Muon Stations : box containing all points
  std::vector<muonExtent_> m_stationExtent;

  // Muon twelfths : box containing all points in a twelfth of a region
  // here the indexing in station, region, quater, twelfth
  // numbering clockwise:
  //  
  //  +-------+ +-------+
  //  | 2   1 | | 3   2 |    
  //  |   +---+ +---+   |
  //  | 3 | Q2   Q1 | 1 |
  //  +---+         +---+
  //  +---+         +---+
  //  | 1 | Q3   Q4 | 3 |
  //  |   +---+ +---+   |
  //  | 2   3 | | 1   2 |    
  //  +-------+ +-------+
  

  std::vector<muonExtent_> m_twelfthExtent; 
  inline muonExtent_ & twelfthExtent(int station, int region, int twelfth){
    return m_twelfthExtent[station*(m_NRegion*12) + region*12 + twelfth];
  }

  class chamberExtent_ : public muonExtent_ {
  public:
    DeMuonChamber *pChamber;
  };


  // "outer vector" is the station/region, inner the chambers in the region
  std::vector<std::vector<chamberExtent_> > m_chamExtent; 
  inline int numCham(int station, int region){
    return m_chamExtent[station*(m_NRegion) + region].size();
  }
  // note chambers number from 1
  inline chamberExtent_ & chamExtent(int station, int region, int chamberNum){
    return m_chamExtent[station*(m_NRegion) + region][chamberNum-1];
  }

  class gapExtent_ : public muonExtent_ {
  public:
    DeMuonGasGap *pGasGap;
  };

  // "outer vector" is the station/region, inner the chambers in the region
  std::vector<std::vector<gapExtent_> > m_gapExtent; 
  // number of gaps per station/region
  std::vector<int> m_NGap;
  // note Chambers number from 1 as 
  inline gapExtent_ & gapExtent(int station, int region, 
                              int chamberNum, int gap){
    return m_gapExtent[station*(m_NRegion) + region]
      [(chamberNum-1)*m_NGap[station*(m_NRegion) + region] + gap];
  }

  // store chamber layout locally
  MuonSystemLayout * m_chamberSystemLayout;

private:
  IDataProviderSvc* m_DDS;
};
#endif // MUONTILEIDXYZ_H
