#ifndef NEWTOOL_PVSEED3DTOOL_H
#define NEWTOOL_PVSEED3DTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IPVSeeding.h"            // Interface
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
// From Event
#include "Event/Track.h"

/** @class PVSeed3DTool PVSeed3DTool.h newtool/PVSeed3DTool.h
 *
 *
 *  @author Mariusz Witek
 *  @date   2008-04-20
 */
class PVSeed3DTool : public extends<GaudiTool, IPVSeeding>  {
public:

  /// Standard constructor
  PVSeed3DTool( const std::string& type,
                const std::string& name,
                const IInterface* parent);


  std::vector<Gaudi::XYZPoint>
  getSeeds(const std::vector<const LHCb::Track*>& inputTracks,
           const Gaudi::XYZPoint& beamspot) const override;


private:

  double m_TrackPairMaxDistance = 0.; // maximus distance between tracks to come from same seed
  int    m_MinCloseTracks = 0;

  double m_zMaxSpread = 0;  // for truncated mean

  // suqared variables to gain in speed
  double m_TrackPairMaxDistanceSq = 0;
  double m_zMaxSpreadSq = 0;
  double m_TrackPairMaxCos2Theta = 0.999*0.999;
};

#endif // NEWTOOL_PVSEED3DTOOL_H
