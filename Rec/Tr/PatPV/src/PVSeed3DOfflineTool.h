#ifndef PATPV_PVSEED3DOFFLINETOOL_H
#define PATPV_PVSEED3DOFFLINETOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IPVSeeding.h"            // Interface
#include "Event/Track.h"
#include "TrackInterfaces/ITrackExtrapolator.h"


namespace SimplePVSpace {

struct seedPoint final
{
  Gaudi::XYZPoint position;
  Gaudi::XYZPoint error;
  int multiplicity = 0;

  seedPoint() = default;

};

struct seedState final
{
  LHCb::State lbstate;
  int    nclose = 0;
  double dclose = 0;
  int used = 0;

  seedState() = default;
};

struct closeNode final
{
  seedState* seed_state = nullptr;
  double distance = 0;
  Gaudi::XYZPoint closest_point;
  int take = 0;

  closeNode() = default;
};

}


/** @class PVSeed3DOfflineTool PVSeed3DOfflineTool.h tmp/PVSeed3DOfflineTool.h
 *
 *
 *  @author Mariusz Witek
 *  @date   2005-11-19
 */
class PVSeed3DOfflineTool : public extends<GaudiTool, IPVSeeding> {
public:

  /// Standard constructor
  PVSeed3DOfflineTool( const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  ~PVSeed3DOfflineTool( ) override; ///< Destructor

  // Initialization
  StatusCode initialize() override;

  std::vector<Gaudi::XYZPoint> 
  getSeeds(const std::vector<const LHCb::Track*>& inputTracks,
           const Gaudi::XYZPoint& beamspot) const override;

private:

  ITrackExtrapolator* m_fullExtrapolator = nullptr;  // Full extrapolator


  bool simpleMean(std::vector<SimplePVSpace::closeNode> & close_nodes, SimplePVSpace::seedPoint & pseed) const;

  SimplePVSpace::seedPoint wMean(const std::vector<SimplePVSpace::closeNode> & close_nodes, const SimplePVSpace::seedState& base_state) const;

  double m_TrackPairMaxDistance; // maximum distance between tracks
  double m_TrackPairMaxDistanceChi2; // maximum distance pseudo Chi2 between tracks
  int    m_MinCloseTracks;

  double m_zMaxSpread;  // for truncated mean


};
#endif // PATPV_PVSEED3DOFFLINETOOL_H
