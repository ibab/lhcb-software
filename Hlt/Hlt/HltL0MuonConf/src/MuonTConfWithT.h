// $Id: MuonTConfWithT.h,v 1.1.1.1 2007-05-22 10:20:31 asatta Exp $
#ifndef MUONTCONFWITHT_H 
#define MUONTCONFWITHT_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"


#include "TsaKernel/ITsaCollector.h"
#include "TsaKernel/STCluster.h"
#include "TsaKernel/OTCluster.h"

#include "HltBase/IPrepareMuonTSeed.h"

#include "HltBase/ITrackConfirmTool.h" 

#include "Event/L0MuonCandidate.h"


/** @class MuonTConfWithT MuonTConfWithT.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-02-14
 */
class MuonTConfWithT : public HltAlgorithm {
public: 
  /// Standard constructor
  MuonTConfWithT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonTConfWithT( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  
  int m_nsigma;
  bool m_debugInfo;  
  //  LHCb::MuonTCandidates* m_inputMuonTs; 
  LHCb::L0MuonCandidates* m_inputL0Muons;

    IPrepareMuonTSeed* m_prepareMuonSeed;
  // Tsa Collector
  ITsaCollector*    m_tsacollector;

  //new seeding tool
  ITrackConfirmTool* m_TsaConfirmTool;
  PatTrackContainer*
  m_outputMuonTracks;///< Pointer to the muon track containe
  LHCb::State* m_myState;

  
  std::string m_outputMuonTracksName;   ///< output container name
  float m_countEvent;
  float m_totMuonInput;
  float m_totMuonAccepted;
};
#endif // MUONTCONFWITHT_H
