// $Id: HltPrepareTracks.h,v 1.2 2007-11-14 13:57:03 hernando Exp $
#ifndef HLTPREPARETRACKS_H 
#define HLTPREPARETRACKS_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"
#include "Event/L0CaloCandidate.h"

/** @class HltHadAlleyPreTrigger HltHadAlleyPreTrigger.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-28
 */
class HltPrepareTracks : public HltAlgorithm {
public: 
  /// Standard constructor
  HltPrepareTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltPrepareTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

  std::string m_TESInputTracksName;

  Hlt::TrackFilter* _nobackwards;

};
#endif // HLTHADALLEYPRETRIGGER_H
