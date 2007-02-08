// $Id: 
#ifndef PREPAREMUONSEED_H 
#define PREPAREMUONSEED_H 1

// Include files for service retrieval
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "HltBase/IPrepareMuonSeed.h"            



/** @class PrepareMuonSeed PrepareMuonSeed.h
 *  
 *  Tool to prepare state for TsaCollector for L0 confirmation
 *
 *  2007-01-11 Johannes Albrecht
 *
 */
//forward declarations
class IMuonPosTool;
class ITrackPtKick;
class L0MuonCandidate;


class PrepareMuonSeed : public GaudiTool, virtual public IPrepareMuonSeed
{
public:

  // Standard constructor
  PrepareMuonSeed( const std::string& type,const std::string& name,const IInterface* parent);

  /// Standard Destructor
  virtual ~PrepareMuonSeed( ); 

  /// Initialize method
  virtual StatusCode initialize();

  virtual StatusCode prepareSeed( const LHCb::L0MuonCandidate& muonL0Cand,
                                  LHCb::State& seedState );
                                 
private:

  bool m_debugInfo;
  IMuonPosTool      *m_iPosTool;
  ITrackPtKick* m_fCalcPtKick; 

};


  
#endif // PREPAREMUONSEED_H
