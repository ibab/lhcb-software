// $Id: HltPrepareTracks.h,v 1.3 2007-11-20 10:16:01 graven Exp $
#ifndef HLTPREPARETRACKS_H 
#define HLTPREPARETRACKS_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"

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


private:
  std::string m_TESInputTracksName;
  Hlt::TrackFilter* _nobackwards;
};
#endif // HLTHADALLEYPRETRIGGER_H
