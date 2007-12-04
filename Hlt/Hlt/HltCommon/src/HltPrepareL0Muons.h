// $Id: HltPrepareL0Muons.h,v 1.1 2007-12-04 13:24:54 hernando Exp $
#ifndef HLTPREPAREL0CALOS_H 
#define HLTPREPAREL0CALOS_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"
#include "HltBase/IMuonSeedTool.h"
#include "Event/L0MuonCandidate.h"

/** @class HltHadAlleyPreTrigger HltHadAlleyPreTrigger.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-28
 */
class HltPrepareL0Muons : public HltAlgorithm {
public: 
  /// Standard constructor
  HltPrepareL0Muons( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltPrepareL0Muons( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


private:

  double m_PtMin;
  std::string m_outputMuonTracksName; 
  IMuonSeedTool* m_maker;


};
#endif // HLTHADALLEYPRETRIGGER_H
