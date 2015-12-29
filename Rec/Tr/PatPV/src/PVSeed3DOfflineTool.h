#ifndef PATPV_PVSEED3DOFFLINETOOL_H
#define PATPV_PVSEED3DOFFLINETOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IPVSeeding.h"            // Interface
#include "Event/Track.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

typedef Gaudi::XYZVector EVector;
typedef Gaudi::XYZPoint EPoint;

namespace SimplePVSpace {

class seedPoint
{
public:
  EPoint position;
  EPoint error;
  int multiplicity;

  seedPoint():position(),error(),multiplicity(0) {};

};

class seedState
{
public:
  LHCb::State lbstate;
  int    nclose;
  double dclose;
  int used;

  seedState():lbstate(),nclose(0),dclose(0.),used(0) {};
};

class closeNode
{
public:
  seedState* seed_state;
  double distance;
  EPoint closest_point;
  int take;

  closeNode():seed_state(0),distance(0.),closest_point(),take(0){};
};

bool  statecomp( const seedState &first, const seedState &second ) {
    return first.dclose > second.dclose;
    //    return first->distance < second->distance;
}

}

using namespace SimplePVSpace;
using namespace ROOT::Math;
using namespace Gaudi;
using namespace LHCb;
using namespace std;


/** @class PVSeed3DOfflineTool PVSeed3DOfflineTool.h tmp/PVSeed3DOfflineTool.h
 *
 *
 *  @author Mariusz Witek
 *  @date   2005-11-19
 */
class PVSeed3DOfflineTool : public GaudiTool, virtual public IPVSeeding {
public:

  /// Standard constructor
  PVSeed3DOfflineTool( const std::string& type,
              const std::string& name,
              const IInterface* parent);

  virtual ~PVSeed3DOfflineTool( ); ///< Destructor

  // Initialization
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  void getSeeds(std::vector<const LHCb::Track*>& inputTracks,
		const Gaudi::XYZPoint& beamspot,
		std::vector<Gaudi::XYZPoint>& seeds);

protected:

private:

  ITrackExtrapolator* m_fullExtrapolator;  // Full extrapolator

  bool closestPoints(const EPoint& ori1, const EVector& dir1,
		     const EPoint& ori2, const EVector& dir2,
		     EPoint& close1, EPoint& close2);

  double thetaTracks(const State& state1,
                     const State& state2);

  bool xPointParameters(const State& state1, const State& state2,
                        double & distance, double & distanceChi2, EPoint & closestPoint);

  bool simpleMean(std::vector<closeNode> & close_nodes, seedPoint & pseed);

  void wMean(std::vector<closeNode> & close_nodes, seedState* base_state, seedPoint & pseed);

  double m_TrackPairMaxDistance; // maximum distance between tracks
  double m_TrackPairMaxDistanceChi2; // maximum distance pseudo Chi2 between tracks
  int    m_MinCloseTracks;

  double m_zMaxSpread;  // for truncated mean


};
#endif // PATPV_PVSEED3DOFFLINETOOL_H
