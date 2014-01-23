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

    virtual ~HltSelectTracksForSwimming(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution
    virtual StatusCode finalize();   ///< Algorithm finalization

  private:
    StatusCode filter_Tracks();
    bool tracksMatchInVelo( const LHCb::Track*, std::vector<unsigned int> );

    Hlt::SelectionContainer2<LHCb::Track, LHCb::Track> m_selections;
    std::vector<unsigned int> m_offlineTrackIDs;
};

#endif // HLTSELECTTRACKSFORSWIMMING_H
