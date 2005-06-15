// $Id: MuonIDDLLTool.h,v 1.2 2005-06-15 06:26:39 pkoppenb Exp $
#ifndef MUONIDDLLTOOL_H
#define MUONIDDLLTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "MuonID/IMuonIDDLLTool.h"

class IMuonTileXYZTool;

/** @class MuonIDDLLTool MuonIDDLLTool.h 
 * Muon ID Likelihood Calculator 
 *
 * @author Joao R. Torres de Mello Neto, Miriam Gandelman
 * @date 24/04/2003 
 */

class MuonIDDLLTool : public GaudiTool,
                      virtual public IMuonIDDLLTool {

public:

  /// Standard Constructor
  MuonIDDLLTool( const std::string& type, const std::string& name, 
                 const IInterface* parent);

  /// Standard Destructor
  virtual ~MuonIDDLLTool() { }

  StatusCode initialize();    

  /// Calculates the Likelihood given a MuonID
  /// Inputs: a MuonID object
  StatusCode calcMuonDLL( const MuonID* muonid, double& dll );

protected:
  /// Extract the momentum and extrapolate the track to each station
  StatusCode trackExtrapolate(const TrStoredTrack *pTrack);
  /// clear track based local variables
  void resetTrackLocals();

  double landau_root(double x, double mpv, double sigma);		   

private:

  double m_trackSlopeX;
  std::vector<double> m_trackX; // position of track in x(mm) in each station
  std::vector<double> m_trackY; // position of track in y(mm) in each station
  double m_Momentum;
  IMuonTileXYZTool *m_iTileTool;
  IMuonGeometryTool *m_iGeomTool;
  int m_NStation;
  std::vector<double> m_stationZ; // station position

  std::vector<double> m_distPion;
  std::vector<double> m_distMuon;

  //double m_distPion;
  //double m_distMuon;

};

#endif // MUONIDDLLTOOL_H
