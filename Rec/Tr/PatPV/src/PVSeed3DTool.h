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

  StatusCode initialize();

  void getSeeds(std::vector<const LHCb::Track*>& inputTracks,
		const Gaudi::XYZPoint& beamspot,
		std::vector<Gaudi::XYZPoint>& seeds);

protected:

private:

typedef Gaudi::XYZVector EVector;
typedef Gaudi::XYZPoint EPoint;

  // helper types
  struct seedPoint {
    EPoint position;
    EPoint error;
    int multiplicity;
  };

  static bool seedcomp( const seedPoint& first, const seedPoint& second );

  struct seedTrack {
    const LHCb::Track* lbtrack;
    bool used;
  };

  struct closeNode {
    seedTrack* seed_track;
    EPoint closest_point;
    bool take;
  };


  double cos2Theta( const EVector& v1, const EVector& v2 );

  bool xPointParameters(const LHCb::Track* track1, const LHCb::Track* track2,
                        double & distance2, double & cos2th, EPoint & closestPoint);

  bool simpleMean(std::vector<closeNode> & close_nodes, seedPoint & pseed);

  void wMean(std::vector<closeNode> & close_nodes, seedTrack* base_track, seedPoint & pseed);

  double m_TrackPairMaxDistance; // maximus distance between tracks to come from same seed
  int    m_MinCloseTracks;

  double m_zMaxSpread;  // for truncated mean

  // suqared variables to gain in speed
  double m_TrackPairMaxDistanceSq;
  double m_zMaxSpreadSq;
  double m_TrackPairMaxCos2Theta;
};

#endif // NEWTOOL_PVSEED3DTOOL_H
