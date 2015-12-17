/** @file VPTrackSelector.cpp
 *
 *  Implementation file for reconstruction tool VPTrackSelector,
 *  based on VeloTrackSelector.
 *
 *  @author Christoph Hombach Christoph.Hombach@hep.manchester.ac.uk 
 */

#include "TrackSelector.h"

class VPTrackSelector : public TrackSelector {

public:

  /// Constructor
  VPTrackSelector(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);
  /// Destructor 
  virtual ~VPTrackSelector();

  /** Returns if the given track is selected or not
   *
   *  @param track Reference to the track to test
   *
   *  @return boolean indicating if the track is selected or not
   *  @retval true  Track is selected
   *  @retval false Track is rejected
   */
  virtual bool accept(const LHCb::Track & track) const;

private:
  size_t m_minHitsASide;
  size_t m_minHitsCSide;
  size_t m_minHits;
};

DECLARE_TOOL_FACTORY(VPTrackSelector)

//-----------------------------------------------------------------------------

VPTrackSelector::VPTrackSelector(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
    : TrackSelector(type, name, parent) {
  declareInterface<ITrackSelector>(this);
  declareProperty("MinHitsASide", m_minHitsASide = 0);
  declareProperty("MinHitsCSide", m_minHitsCSide = 0);
  declareProperty("MinHits",      m_minHits = 0);
  
}

VPTrackSelector::~VPTrackSelector() { }

bool VPTrackSelector::accept(const LHCb::Track& track) const {

  size_t numHits[3] = {0, 0, 0};
  for (const LHCb::LHCbID lhcbid : track.lhcbIDs()) {
    if (!lhcbid.isVP()) continue;
    const LHCb::VPChannelID vpid = lhcbid.vpID();
    const unsigned int side = (vpid.sensor() % 2);
    ++numHits[side];
    ++numHits[2];
  }
  return
    numHits[0] >= m_minHitsASide &&
    numHits[1] >= m_minHitsCSide &&
    numHits[2] >= m_minHits      &&
    TrackSelector::accept(track);
}

