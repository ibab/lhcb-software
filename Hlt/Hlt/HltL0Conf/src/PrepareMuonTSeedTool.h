// $Id: 
#ifndef PREPAREMUONTSEEDTOOL_H 
#define PREPAREMUONTSEEDTOOL_H 1

// Include files for service retrieval
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "HltBase/IPrepareMuonTSeedTool.h"            



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


class PrepareMuonTSeedTool : public GaudiTool, virtual public IPrepareMuonTSeedTool
{
public:

  // Standard constructor
  PrepareMuonTSeedTool( const std::string& type,const std::string& name,const IInterface* parent);

  /// Standard Destructor
  virtual ~PrepareMuonTSeedTool( ); 

  /// Initialize method
  virtual StatusCode initialize();

  virtual StatusCode prepareSeed( const LHCb::Track& muonTrack,
                                  LHCb::Track& seedTrack );
                                 
private:

  bool m_debugInfo;  
  double sigmaX2[4], sigmaY2[4], sigmaTx2[4], sigmaTy2[4];

  IMuonPosTool      *m_iPosTool;
  ITrackPtKick* m_fCalcPtKick; 

};


  
#endif // PREPAREMUONTSEEDTOOL_H
