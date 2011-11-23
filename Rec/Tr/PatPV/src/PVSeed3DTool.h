// $Id: PVSeed3DTool.h,v 1.2 2008-08-28 17:38:45 witekma Exp $
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

using namespace ROOT::Math;
using namespace Gaudi;
using namespace LHCb;
using namespace std;

typedef Gaudi::XYZVector EVector;
typedef Gaudi::XYZPoint EPoint;

class seedPoint;
class closeNode;
class seedTrack;

/** @class PVSeed3DTool PVSeed3DTool.h newtool/PVSeed3DTool.h
 *  
 *
 *  @author Mariusz Witek
 *  @date   2008-04-20
 */
class PVSeed3DTool : public GaudiTool, virtual public IPVSeeding  {
public: 

  /// Standard constructor
  PVSeed3DTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~PVSeed3DTool( ); ///< Destructor

  void getSeeds(std::vector<const LHCb::Track*>& inputTracks, 
		const Gaudi::XYZPoint& beamspot,
		std::vector<Gaudi::XYZPoint>& seeds);

protected:

private:

  bool closestPoints(const EPoint& ori1, const EVector& dir1,
		     const EPoint& ori2, const EVector& dir2,
		     EPoint& close1, EPoint& close2);

  double thetaTracks(const Track& track1, 
                     const Track& track2);

  bool xPointParameters(const Track& track1, const Track& track2,
                        double & distance, EPoint & closestPoint);

  bool simpleMean(std::vector<closeNode> & close_nodes, seedPoint & pseed);

  void wMean(std::vector<closeNode> & close_nodes, seedTrack* base_track, seedPoint & pseed);

  double m_TrackPairMaxDistance; // maximus distance between tracks to come from same seed
  int    m_MinCloseTracks;

  double m_zMaxSpread;  // for truncated mean
};

#endif // NEWTOOL_PVSEED3DTOOL_H
