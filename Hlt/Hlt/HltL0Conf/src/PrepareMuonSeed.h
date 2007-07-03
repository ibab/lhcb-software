// $Id: 
#ifndef PREPAREMUONSEED_H 
#define PREPAREMUONSEED_H 1

// Include files for service retrieval
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "HltBase/IPrepareMuonSeed.h"            

//local
#include "L0ConfDataStore.h"

/** @class PrepareMuonSeed PrepareMuonSeed.h
 *  
 *  Tool to prepare state for TsaCollector for L0 confirmation
 *
 *  2007-01-11 Johannes Albrecht
 *
 */
//forward declarations
class IMuonPosTool;
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
  
  bool m_debugMode;
  
  std::vector<double> m_sigmaX2;
  std::vector<double> m_sigmaY2;
  std::vector<double> m_sigmaTx2;
  std::vector<double> m_sigmaTy2;

  
  IMuonPosTool      *m_iPosTool;
  
  //debug information
  L0ConfDataStore* m_DataStore;
  
};


  
#endif // PREPAREMUONSEED_H
