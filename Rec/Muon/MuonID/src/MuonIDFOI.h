// $Id: MuonIDFOI.h,v 1.1.1.1 2002-05-10 12:06:58 dhcroft Exp $
#ifndef MUONIDFOI_H 
#define MUONIDFOI_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// forward declaration of tools
class IMuonTileXYZTool;
class IMuonGeometryTool;

//forward declaration of MuonID and TrStoredTrack from Event classes
class MuonID;
class TrStoredTrack;

/** @class MuonIDFOI MuonIDFOI.h
 *  
 *  This is an Algorithm to create MuonID objects starting from tracks and
 *  using the hits in the muon system
 *
 *  @author David Hutchcroft
 *  @date   02/05/2002
 */
class MuonIDFOI : public Algorithm {
public:
  /// Standard constructor
  MuonIDFOI( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonIDFOI( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  /// Do the identification
  StatusCode doID(MuonID * pMuid);

  /// check the track is in the p and angle acceptance
  StatusCode preSelection(MuonID * pMuid, bool &passed);

  /// Get the Muon coordinated from the TES
  StatusCode getCoords(MuonID *pMuid);

  /// Extract the momentum and extrapolate the track to each station
  StatusCode trackExtrapolate(TrStoredTrack *pTrack);

  /// return the FOI in x in a station and region for momentum (in MeV/c)
  double foiX(const int &station, const int &region, const double &p);
  /// return the FOI in y in a station and region for momentum (in MeV/c)
  double foiY(const int &station, const int &region, const double &p);
  
  /// clear track based local variables
  void resetTrackLocals();

  // Properties

  /// TES path of the tracks to analyse
  std::string m_TrStoredTracksPath;

  /// TES path to output the MuonIDs to
  std::string m_MuonIDsPath;

  /// Momentum ranges: different treatement of M4/M5 in each
  std::vector<double> m_MomentumCuts; // vector of momentum ranges
  // function that defines the field of interest size
  // formula is p(1) + p(2)*momentum + p(3)*exp(-p(4)*momentum)
  std::vector< double >     m_xfoiParam1;
  std::vector< double >     m_xfoiParam2;
  std::vector< double >     m_xfoiParam3;
  std::vector< double >     m_xfoiParam4;
  std::vector< double >     m_yfoiParam1;
  std::vector< double >     m_yfoiParam2;
  std::vector< double >     m_yfoiParam3;
  std::vector< double >     m_yfoiParam4;

  // local array of pad sizes in mm
  double m_padSizeX[5][4];
  double m_padSizeY[5][4];

  // local array of region sizes
  double m_regionInnerX[5][4]; // inner edge in abs(x)
  double m_regionOuterX[5][4]; // outer edge in abs(x) - overlap so store both
                            
  double m_regionInnerY[5][5]; // inner edge in abs(y)
  double m_regionOuterY[5][5]; // outer edge in abs(y) - overlap so store both
  
  double m_stationZ[5]; // station position

  // local track parameters: momentum and linear extrapolation to each station
  double m_Momentum; // in MeV/c
  double m_trackSlopeX;
  double m_trackX[5]; // position of track in x(mm) in each station
  double m_trackY[5]; // position of track in y(mm) in each station  

  //test if found a hit in the MuonStations
  int m_occupancy[5];
  // store X of hits for dx/dz matching with track (only need M2/M3)
  double m_CoordX[5];


  // MuonTileID to ZYX tool
  IMuonTileXYZTool *m_iTileTool;
  IMuonGeometryTool *m_iGeomTool;

};
#endif // MUONIDFOI_H
