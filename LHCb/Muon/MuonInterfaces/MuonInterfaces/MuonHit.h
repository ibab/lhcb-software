// $Id: MuonHit.h,v 1.2 2010-02-12 12:19:58 ggiacomo Exp $
#ifndef COSMICSMUONHIT_H 
#define COSMICSMUONHIT_H 1

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

class MuonHit:
public ROOT::Math::PositionVector3D< ROOT::Math::Cartesian3D<double> > {
  //public ROOT::Math::XYZPoint {
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

  int hitPID();
  int hitMother();
  /// return the logical channels used for this cluster
  std::vector<MuonLogHit*> getHits();
  /// return the MuonTileIDs of the logical channels used for this cluster
  std::vector<LHCb::MuonTileID*> getTiles();
  /// get the raw times of logical channels in this cluster
  std::vector<float> getTimes();
  std::vector<double> hitTile_Size(); /// half-sizes of the tile(s) underlying this hit
  double hitTile_dX(); /// dx half-size of the tile underlying this hit
  double hitTile_dY(); /// dy half-size of the tile underlying this hit
  double hitTile_dZ(); /// dz half-size of the tile underlying this hit
  inline float hitTime()  {    return m_time;  }  /// in TDC counts !
  inline float hitDeltaTime() { return m_dtime; } /// in TDC counts !
  inline float hitMinTime() { return m_mintime; } /// in TDC counts !
  inline float hitMaxTime() { return m_maxtime; } /// in TDC counts !

  /// retireve the hit ID
  int hitID();
  /// return the hit station
  int station();
  /// return the hit region
  int region();
  /// return the (first) MuonTileID of the underlying logical pad
  const LHCb::MuonTileID* tile();
  /// return the most centered tile of this cluster
  const LHCb::MuonTileID* centerTile();
  /// check if this is a true logical and not an uncrossed log. channel
  bool isTruePad();
  
  /// return the (first) associated MuonLogPad object
  inline MuonLogPad* logPad() {
    return ((m_pads.empty()) ? NULL : m_pads[0]);
  }
  /// return the associated MuonLogPad objects
  inline const std::vector<MuonLogPad*>* logPads() {
    return &m_pads;
  }
  /// return the MuonTileIDs of the logical pads used for this cluster
  std::vector<LHCb::MuonTileID*> getLogPadTiles(); 
  /// number of logical pads in this hit
  inline int npads() {
    return m_pads.size();
  }
  /// cluster size in X
  inline int clsizeX() {
    return m_xsize;
  }
  /// cluster size in Y
  inline int clsizeY() {
    return m_ysize;
  }
  inline double dX() { return m_dx;}  /// error on X position
  inline double dY() { return m_dy;}  /// error on Y position
  inline double dZ() { return m_dz;}  /// error on Z position
  inline double minX() { return m_hit_minx;} 
  inline double maxX() { return m_hit_maxx;} 
  inline double minY() { return m_hit_miny;} 
  inline double maxY() { return m_hit_maxy;} 
  inline double minZ() { return m_hit_minz;} 
  inline double maxZ() { return m_hit_maxz;} 

private:
  void createFromPad(MuonLogPad* mp);
  void recomputePos(std::vector<double> *data, 
                    double* pos, double* dpos,
                    int* clsize, double step, bool smear=false);
  void recomputeTime();
  DeMuonDetector* m_muonDet;
  std::vector<MuonLogPad*> m_pads;
  std::vector<double> m_padx;
  std::vector<double> m_pady;
  std::vector<double> m_padz;
  std::vector<MuonLogPad*>::iterator m_ip;
  int m_pid;
  int m_mamy_pid;
  int m_hit_ID;
  double m_dx;
  double m_dy;
  double m_dz;
  double m_hit_minx;
  double m_hit_maxx;
  double m_hit_miny;
  double m_hit_maxy;
  double m_hit_minz;
  double m_hit_maxz;
  int m_xsize;
  int m_ysize;
  int m_zsize;
  float m_time;
  float m_dtime;
  float m_mintime;
  float m_maxtime;
  IMuonFastPosTool* m_posTool;
};

#endif // COSMICSMUONHIT_H
