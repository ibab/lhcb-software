#ifndef MUONHIT_H
#define MUONHIT_H 1

#include <vector>

#include "MuonDet/DeMuonDetector.h"
#include "Kernel/MuonTileID.h"
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "Math/GenVector/VectorUtil.h"
#include "Math/GenVector/PositionVector3D.h"

class MuonLogHit;
class MuonLogPad;
class IMuonFastPosTool;

/** @class MuonHit MuonHit.h
 *  represents hits for muon track (cluster of muon logical pads)
 *
 *  @author Giovanni Passaleva, Giacomo Graziani
 *  @date   2007-10-26
 */

class MuonHit: public ROOT::Math::PositionVector3D< ROOT::Math::Cartesian3D<double> > {
public:
  /// standard constructor
  MuonHit();
  /// constructor for empty cluster
  MuonHit( DeMuonDetector* md,  IMuonFastPosTool* posTool );
  /// constructor from a MuonPad
  MuonHit( DeMuonDetector* md,
           MuonLogPad* mp,
           IMuonFastPosTool* posTool);
  /// obsolete constructor from a MuonPad (kept for backw. compat.)
  MuonHit( DeMuonDetector* md,
           MuonLogPad* mp,
           const double x,
           const double y,
           const double z,
           IMuonFastPosTool* posTool);
  virtual ~MuonHit( ); ///< Destructor

  // public member functions
  /// add a logical pad to this cluster
  void addPad(MuonLogPad* mp);

  /// store a progressive hit number for debugging
  StatusCode setHitID(const int id);

  int hitPID() const;
  int hitMother() const;
  /// return the logical channels used for this cluster
  std::vector<const MuonLogHit*> getHits() const;
  /// return the MuonTileIDs of the logical channels used for this cluster
  std::vector<LHCb::MuonTileID> getTiles() const;
  /// get the raw times of logical channels in this cluster
  std::vector<float> getTimes() const;
  std::vector<double> hitTile_Size() const; /// half-sizes of the tile(s) underlying this hit
  double hitTile_dX() const; /// dx half-size of the tile underlying this hit
  double hitTile_dY() const; /// dy half-size of the tile underlying this hit
  double hitTile_dZ() const; /// dz half-size of the tile underlying this hit
  float hitTime() const {    return m_time;  }  /// in TDC counts !
  float hitDeltaTime() const { return m_dtime; } /// in TDC counts !
  float hitMinTime() const { return m_mintime; } /// in TDC counts !
  float hitMaxTime() const { return m_maxtime; } /// in TDC counts !

  /// retireve the hit ID
  int hitID() const;
  /// return the hit station
  int station() const;
  /// return the hit region
  int region() const;
  /// return the (first) MuonTileID of the underlying logical pad
  LHCb::MuonTileID tile() const;
  /// return the most centered tile of this cluster
  LHCb::MuonTileID centerTile() const;
  /// check if this is a true logical and not an uncrossed log. channel
  bool isTruePad() const;

  /// return the (first) associated MuonLogPad object
  MuonLogPad* logPad() { return !m_pads.empty() ? m_pads[0] : nullptr; }
  /// return the associated MuonLogPad objects
  const std::vector<MuonLogPad*>* logPads() { return &m_pads; }
  /// return the MuonTileIDs of the logical pads used for this cluster
  std::vector<LHCb::MuonTileID> getLogPadTiles() const;
  /// number of logical pads in this hit
  int npads() const { return m_pads.size(); }
  int clsizeX() const { return m_xsize; } /// cluster size in X
  int clsizeY() const { return m_ysize; } /// cluster size in Y
  double dX() const { return m_dx;}  /// error on X position
  double dY() const { return m_dy;}  /// error on Y position
  double dZ() const { return m_dz;}  /// error on Z position
  double minX() const { return m_hit_minx;}
  double maxX() const { return m_hit_maxx;}
  double minY() const { return m_hit_miny;}
  double maxY() const { return m_hit_maxy;}
  double minZ() const { return m_hit_minz;}
  double maxZ() const { return m_hit_maxz;}

private:

  void createFromPad(MuonLogPad* mp);
  void recomputePos(std::vector<double> *data,
                    double* pos, double* dpos,
                    int* clsize, double step);
  void recomputeTime();

private:

  IMuonFastPosTool* m_posTool = nullptr;
  std::vector<MuonLogPad*> m_pads;
  std::vector<double> m_padx;
  std::vector<double> m_pady;
  std::vector<double> m_padz;
  double m_dx = 0;
  double m_dy = 0;
  double m_dz = 0;
  double m_hit_minx = 0;
  double m_hit_maxx = 0;
  double m_hit_miny = 0;
  double m_hit_maxy = 0;
  double m_hit_minz = 0;
  double m_hit_maxz = 0;
  float m_time = 0;
  float m_dtime = 0;
  float m_mintime = 0;
  float m_maxtime = 0;
  int m_pid = 0;
  int m_mamy_pid = 0;
  int m_hit_ID = -1;
  int m_xsize = 0;
  int m_ysize = 0;
  int m_zsize = 0;
};

#endif // MUONHIT_H
