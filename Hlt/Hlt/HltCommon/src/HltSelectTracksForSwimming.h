// $Id: HltSelectTracksForSwimming.h,v 1.4 2010-01-08 12:40:31 gligorov Exp $
#ifndef HLTSELECTTRACKSFORSWIMMING_H 
#define HLTSELECTTRACKSFORSWIMMING_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "Event/Particle.h"

/** @class HltSelectTracksForSwimming HltSelectTracksForSwimming.h 
 *  
 *
 *  @author V. Gligorov
 *  @date   2008-02-12
 */

class HltSelectTracksForSwimming : public HltAlgorithm {
public: 
  /// Standard constructor
  HltSelectTracksForSwimming( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelectTracksForSwimming(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

  StatusCode filter_Tracks();
  bool tracksMatchInVelo(LHCb::Track*,std::vector<unsigned int>);

  Hlt::SelectionContainer2<LHCb::Track,LHCb::Track> m_selections;
  std::vector<unsigned int>  m_offlineTrackIDs;
};

#endif // HLTSELECTTRACKSFORSWIMMING_H
