// $Id: HltPrepareTracks.h,v 1.1 2007-06-20 12:17:38 hernando Exp $
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

  Hlt::TrackFilter* _nobackwards;

};
#endif // HLTHADALLEYPRETRIGGER_H
