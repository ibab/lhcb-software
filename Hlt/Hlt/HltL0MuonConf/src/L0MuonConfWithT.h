// $Id: L0MuonConfWithT.h,v 1.1.1.1 2007-05-22 10:20:31 asatta Exp $
#ifndef L0MUONCONFWITHT_H 
#define L0MUONCONFWITHT_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"


#include "TsaKernel/ITsaCollector.h"
#include "TsaKernel/STCluster.h"
#include "TsaKernel/OTCluster.h"

#include "HltBase/IPrepareMuonSeed.h"

#include "HltBase/ITrackConfirmTool.h" 

#include "Event/L0MuonCandidate.h"


/** @class L0MuonConfWithT L0MuonConfWithT.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-02-14
 */
class L0MuonConfWithT : public HltAlgorithm {
public: 
  /// Standard constructor
  L0MuonConfWithT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonConfWithT( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  
  int m_nsigma;
  bool m_debugInfo;  
  LHCb::L0MuonCandidates* m_inputL0Muons;

    IPrepareMuonSeed* m_prepareMuonSeed;
  // Tsa Collector
  ITsaCollector*    m_tsacollector;

  //new seeding tool
  ITrackConfirmTool* m_TsaConfirmTool;
  PatTrackContainer*
  m_outputMuonTracks;///< Pointer to the muon track containe
  LHCb::State* m_myState;

  
  std::string m_outputMuonTracksName;   ///< output container name

};
#endif // L0MUONCONFWITHT_H
