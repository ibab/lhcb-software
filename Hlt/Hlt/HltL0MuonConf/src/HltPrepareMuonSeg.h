// $Id: HltPrepareMuonSeg.h,v 1.6 2009-12-23 15:05:58 graven Exp $
#ifndef HLTPREPAREMUONSEG_H 
#define HLTPREPAREMUONSEG_H 1

// Include files
// from Gaudi
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/IMuonSeedTool.h"

/** @class HltPrepareMuonSeg HltPrepareMuonSeg.h
 *  
 *
 *  @author Sandra Amato 
 *  @date   2007-12-4
 */
class HltPrepareMuonSeg : public HltAlgorithm {
public: 
  /// Standard constructor
  HltPrepareMuonSeg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltPrepareMuonSeg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  
  Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_selections; 
  IMuonSeedTool* m_prepareMuonSeed;
  
  std::string m_outputMuonTracksName;   ///< output container name
};
#endif // HLTPREPAREMUONSEG_H
