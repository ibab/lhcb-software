// $Id: HltL0CaloPrepare.h,v 1.1 2008-01-22 09:56:32 hernando Exp $
#ifndef HLTCOMMON_HLTLOCALOPREPARE_H 
#define HLTCOMMON_HLTL0CALOPREPARE_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/ICaloSeedTool.h"
#include "Event/L0CaloCandidate.h"

/** @class HltHadAlleyPreTrigger HltHadAlleyPreTrigger.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-28
 */
class HltL0CaloPrepare : public HltAlgorithm {
public: 
  /// Standard constructor
  HltL0CaloPrepare( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltL0CaloPrepare( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


private:

  void makeTrack(const LHCb::L0CaloCandidate& calo, LHCb::Track& track);

  void addExtras(const LHCb::L0CaloCandidate& calo, LHCb::Track& track);

protected:

  // Already in HltAlgorithm
  // Hlt::TrackSelection* m_outputTracks;

  int m_caloType;
  double m_minEt;
  
  std::string m_caloCandidatesLocation;
  std::string m_outputL0CaloCandidatesName;

  std::string m_caloMakerName;  
  ICaloSeedTool* m_caloMaker;
  
  Hlt::Histo* m_histoEt;
  Hlt::Histo* m_histoEt1;

};
#endif // HLTHADALLEYPRETRIGGER_H
