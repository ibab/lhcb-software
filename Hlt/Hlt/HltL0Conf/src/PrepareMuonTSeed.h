// $Id: 
#ifndef PREPAREMUONTSEED_H 
#define PREPAREMUONTSEED_H 1

// Include files for service retrieval
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "HltBase/IPrepareMuonTSeed.h"            



/** @class PrepareMuonSeed PrepareMuonSeed.h
 *  
 *  Tool to prepare state for TsaCollector for muon segment confirmation
 *
 *  2007-03-11 Alessia Satta
 *
 */
//forward declarations
class IMuonPosTool;
class ITrackPtKick;
class L0MuonCandidate;


class PrepareMuonTSeed : public GaudiTool, virtual public IPrepareMuonTSeed
{
public:

  // Standard constructor
  PrepareMuonTSeed( const std::string& type,const std::string& name,const IInterface* parent);

  /// Standard Destructor
  virtual ~PrepareMuonTSeed( ); 

  /// Initialize method
  virtual StatusCode initialize();

  virtual StatusCode prepareSeed( const LHCb::Track& muonTrack,
                                  LHCb::State& seedState );
                                 
private:

  bool m_debugInfo;  
  double sigmaX2[4], sigmaY2[4], sigmaTx2[4], sigmaTy2[4];

  IMuonPosTool      *m_iPosTool;
  ITrackPtKick* m_fCalcPtKick; 

};


  
#endif // PREPAREMUONTSEED_H
