// $Id: MuonSeedTool.h,v 1.6 2009-04-30 13:34:09 asatta Exp $
#ifndef MUONSEEDTOOL_H 
#define MUONSEEDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ILHCbMagnetSvc.h"

#include "TrackInterfaces/ITrackMomentumEstimate.h"

// Interface
#include "HltBase/IMuonSeedTool.h"           

//local
#include "L0ConfDataStore.h"

/** @class MuonSeedTool MuonSeedTool.h
 *  
 *  Tool to prepare seedTrack for L0 confirmation
 *
 *  @author Johannes Albrecht
 *  @date   2007-07-03
 */
class MuonSeedTool : public GaudiTool, virtual public IMuonSeedTool {
public: 
  /// Standard constructor
  MuonSeedTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~MuonSeedTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize();
  
  virtual StatusCode makeTrack( const LHCb::Track& inputTrack,
                                LHCb::Track& seedTrack );

  virtual StatusCode makeTrack( const LHCb::L0MuonCandidate& muonL0Cand,
                                LHCb::Track& seedTrack );


private:

  bool m_debugMode, m_useM1, m_fieldOff;
  std::string m_momentumToolName;

  std::vector<double> m_sigmaX2;
  std::vector<double> m_sigmaY2;
  std::vector<double> m_sigmaTx2;
  std::vector<double> m_sigmaTy2;

  std::vector<double> m_sigmaX2NoM1;
  std::vector<double> m_sigmaY2NoM1;
  std::vector<double> m_sigmaTx2NoM1;
  std::vector<double> m_sigmaTy2NoM1;
  
  IMuonFastPosTool      *m_iPosTool;
  ILHCbMagnetSvc*  m_magFieldSvc;
  
  ITrackMomentumEstimate* m_momentumTool;
  
  //debug information
  L0ConfDataStore* m_DataStore;


};
#endif // MUONSEEDTOOL_H
