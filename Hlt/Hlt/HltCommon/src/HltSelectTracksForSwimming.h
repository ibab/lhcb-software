// $Id: HltSelectTracksForSwimming.h,v 1.5 2010-08-13 12:04:02 graven Exp $
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

class HltSelectTracksForSwimming : public HltAlgorithm
{
  public:
    /// Standard constructor
    HltSelectTracksForSwimming( const std::string& name, ISvcLocator* pSvcLocator );

    ~HltSelectTracksForSwimming() override; ///< Destructor

    StatusCode initialize() override; ///< Algorithm initialization
    StatusCode execute() override;    ///< Algorithm execution

  private:
    StatusCode filter_Tracks();
    bool tracksMatchInVelo( const LHCb::Track*, std::vector<unsigned int> );

    Hlt::SelectionContainer<LHCb::Track, LHCb::Track> m_selections;
    std::vector<unsigned int> m_offlineTrackIDs;
};

#endif // HLTSELECTTRACKSFORSWIMMING_H
