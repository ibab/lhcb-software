// $Id: MuonHit.h,v 1.3 2009-05-13 10:59:47 ggiacomo Exp $
#ifndef COSMICSMUONHIT_H 
#define COSMICSMUONHIT_H 1

#include <vector>

#include "MuonDet/DeMuonDetector.h"
#include "Kernel/MuonTileID.h"
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "Math/GenVector/VectorUtil.h"
#include "Math/GenVector/PositionVector3D.h"
// MC truth
#include "Event/MCParticle.h"

class MuonLogHit;
class MuonLogPad;
class IMuonFastPosTool;

/** @class MuonHit MuonHit.h
 *  
 *
 *  @author Giovanni Passaleva
 *  @date   2007-10-26
 */

class MuonHit:
public ROOT::Math::PositionVector3D< ROOT::Math::Cartesian3D<double> > {
  //public ROOT::Math::XYZPoint {
public: 
  /// Standard constructor
  MuonHit( );
  /// constructor from a MuonPad
  MuonHit( DeMuonDetector* md, 
           MuonLogPad* mp,
           const double x,
           const double y,
           const double z,
           IMuonFastPosTool* posTool);

  virtual ~MuonHit( ); ///< Destructor

  /// public member functions
  StatusCode setHitMCTruth(const LHCb::MCParticle* pp);

 /// store a progressive hit number for debugging
  StatusCode setHitID(const int id);

  int hitPID();
  int hitMother();
  std::vector<MuonLogHit*> getHits();
  /// return the MuonTileIDs of the logical channels used for this pad
  const std::vector<LHCb::MuonTileID*> getTiles();
  std::vector<float> getTimes();
  std::vector<double> hitTile_Size(); /// half-size of the tile underlying this hit
  double hitTile_dX(); /// dx half-size of the tile underlying this hit
  double hitTile_dY(); /// dy half-size of the tile underlying this hit
  double hitTile_dZ(); /// dz half-size of the tile underlying this hit
  float hitTime(); // in TDC counts !
  float hitDeltaTime(); // in TDC counts !
  /// retireve the hit ID
  int hitID();
  /// return the hit station
  int station();
  /// return the hit region
  int region();
  /// return the MuonTileID of the underlying logical pad
  const LHCb::MuonTileID* tile();
  /// check if this is a true logical and not an uncrossed log. channel
  bool isTruePad();
  
  /// returns the associated MuonLogPad object
  inline MuonLogPad* logPad() {
    return m_pad;
  }
private:

  DeMuonDetector* m_muonDet;
  MuonLogPad* m_pad;
  const LHCb::MuonTileID* m_tile;
  int m_pid;
  int m_mamy_pid;
  int m_hit_ID;
  double m_hit_dx;
  double m_hit_dy;
  double m_hit_dz;
  std::vector<double> m_hit_size;
  int m_station;
  int m_region;
  float m_time;
  float m_dtime;
  IMuonFastPosTool* m_posTool;
};

#endif // COSMICSMUONHIT_H
