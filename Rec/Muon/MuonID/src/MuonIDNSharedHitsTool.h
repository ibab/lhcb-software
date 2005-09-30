// $Id: MuonIDNSharedHitsTool.h,v 1.2 2005-09-30 09:04:12 pkoppenb Exp $
#ifndef MUONIDNSHAREDHITSTOOL_H
#define MUONIDNSHAREDHITSTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "MuonID/IMuonIDNSharedHitsTool.h"

class IMuonTileXYZTool;

/** @class MuonIDNSharedHitsTool MuonIDNSharedHitsTool.h 
 * Calculates the number of muonID's that share a hit
 * with a given MuonID and are closest to the hit than this one.
 *
 * @author Andre Massafferri, Erica Polycarpo, Miriam Gandelman
 * @date 30/09/2005 
 */

class MuonIDNSharedHitsTool : public GaudiTool,
                              virtual public IMuonIDNSharedHitsTool {

public:

  /// Standard Constructor
  MuonIDNSharedHitsTool( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// Standard Destructor
  virtual ~MuonIDNSharedHitsTool() { }

  StatusCode initialize();    

  /// Calculates the number of shared hits given a MuonID
  /// Input: a MuonID object
  StatusCode calcSharedHits( const MuonID* muonid, int& nSharedHits );

private :

  /// Calculates the distance from the hit to the extrapolated position
  /// Input: a MuonID object
  StatusCode calcDist( const MuonID* muonid );
  /// Extracts the momentum and extrapolate the track to each station
  StatusCode trackExtrapolate(const TrStoredTrack *pTrack);
  /// clear track based local variables
  void resetTrackLocals();
  /// Compare the coordinates of two MuonIDs
  bool compareHits( const MuonID* muonid1, const MuonID* muonid2 );


  double m_dist;
  double m_Momentum;
  double m_trackSlopeX;
  std::vector<double> m_trackX; // position of track in x(mm) in each station
  std::vector<double> m_trackY; // position of track in y(mm) in each station
  std::vector<double> m_stationZ; // station position
  IMuonTileXYZTool *m_iTileTool;
  IMuonGeometryTool *m_iGeomTool;
  int m_NStation;

};

#endif // MUONIDNSHAREDHITSTOOL_H
