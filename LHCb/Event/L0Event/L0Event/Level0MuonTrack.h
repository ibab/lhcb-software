#ifndef LHCBEVENT_LEVEL0MUONTRACK_H
#define LHCBEVENT_LEVEL0MUONTRACK_H 1

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRefVector.h"
#include "CLHEP/Geometry/Point3D.h"

// Externals 
extern const CLID& CLID_Level0MuonTrack;

//------------------------------------------------------------------------------
//
// ClassName:   Level0MuonTrack
//
// Description: This class represents the muon track candidates for L0
//
// Author:      Bruce Hay
//
//------------------------------------------------------------------------------
class Level0MuonTrack : public ContainedObject {

public:
  /// Constructor
  Level0MuonTrack()                                { 
  }
  /// Destructor
  virtual ~Level0MuonTrack()                       {
  }
  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const                 {
    return Level0MuonTrack::classID();
  }
  static const CLID& classID()                     {
    return CLID_Level0MuonTrack;
  }

  /// Position of track at first muon layer
  const HepPoint3D& position() const               { return m_position; }
  /// Update position
  void setPosition(HepPoint3D& position)           { m_position = position; }
  void setPosition(double x, double y, double z)   { 
    m_position.setX(x);
    m_position.setY(y);
    m_position.setZ(z);
  }
  
  /// Error on position of track at first muon layer
  const HepPoint3D& errPosition() const            { return m_errPosition; }
  /// Update position
  void setErrPosition(HepPoint3D& errPosition)     { m_errPosition = errPosition; }
  void setErrPosition(double x, double y, double z){ 
    m_errPosition.setX(x);
    m_errPosition.setY(y);
    m_errPosition.setZ(z);
  }
  
  /// Slope dx/dz at position of track at first muon layer
  double dxdz() const                              { return m_dxdz; }
  /// Update slope dx/dz at position of track at first muon layer
  void setDxdz(double dxdz)                        { m_dxdz = dxdz; }

  /// Slope dy/dz at position of track at first muon layer
  double dydz() const                              { return m_dydz; }
  /// Update slope dy/dz at position of track at first muon layer
  void setDydz(double dydz)                        { m_dydz = dydz; }

  /// Error on dx/dz
  double errDxdz() const                           { return m_dxdz; }
  /// Update error on dx/dz
  void setErrDxdz(double errDxdz)                  { m_errDxdz = errDxdz; }

  /// Error on dy/dz
  double errDydz() const                           { return m_dydz; }
  /// Update error on dy/dz
  void setErrDydz(double errDydz)                  { m_errDydz = errDydz; }

  /// Charge of muon (+1/-1)
  int charge() const                               { return m_charge; }
  /// Update charge of muon (+1/-1)
  void setCharge(int charge)                       { m_charge = charge; }

  /// Transverse momentum
  double pT() const                                { return m_pt; }
  /// Update transverse momentum
  void setPT(double pt)                            { m_pt = pt; }

  /// Momentum
  double p() const                                 { return m_p; }
  /// Update momentum
  void setP(double p)                              { m_p = p; }
  
  /// Theta at the interaction point
  double theta() const                             { return m_theta; }
  /// Update theta at the interaction point
  void setTheta(double theta)                      { m_theta = theta; }

  /// Phi at the interaction point
  double phi() const                               { return m_phi; }
  /// Update phi at the interaction point
  void setPhi(double phi)                          { m_phi = phi; }

  /// y intercept of the muon candidate at the interaction point
  double yInt() const                              { return m_yint; }
  /// Update y intercept of the muon candidate at the interaction point
  void setYInt(double yint)                        { m_yint = yint; }

  /// Multiple scattering angle between line from first hits to last hit: x-proj.
  double xMsAngle() const                          { return m_txms; }
  /// Update multiple scattering angle between line from first hits to last hit: x-proj.
  void setXMsAngle(double txms)                    { m_txms = txms; }
  
  /// Multiple scattering angle between line from first hits to last hit: y-proj.
  double yMsAngle() const                          { return m_tyms; }
  /// Update multiple scattering angle between line from first hits to last hit: y-proj.
  void setYMsAngle(double tyms)                    { m_tyms = tyms; }

  /// 4 bits per layer flagging which hits are used for the track
  int hitFlags() const                             { return m_ihit; }
  /// Update 4 bits per layer flagging which hits are used for the track
  void setHitFlags(int ihit)                       { m_ihit = ihit; }

  /// 4 bits per layer flagging additional hits close by
  int xtraHitFlags() const                         { return m_iahi; }
  /// Update 4 bits per layer flagging additional hits close by
  void setXtraHitFlags(int iahi)                   { m_iahi = iahi; }

  /// Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;
  /// Serialize the object for reading
  virtual StreamBuffer& serialize( StreamBuffer& s );

private:
  /// Position of track at first muon layer
  HepPoint3D m_position;
  /// Error on position of track at first muon layer
  HepPoint3D m_errPosition;
  /// Slope dx/dz at position of track at first muon layer
  double     m_dxdz;
  /// Slope dy/dz at position of track at first muon layer
  double     m_dydz;
  /// Error on dx/dz
  double     m_errDxdz;
  /// Error on dy/dz
  double     m_errDydz;
  /// Charge of muon (+1/-1)
  int        m_charge;
  /// Transverse momentum
  double     m_pt;
  /// Momentum
  double     m_p;
  /// Theta at the interaction point
  double     m_theta;
  /// Phi at the interaction point
  double     m_phi;
  /// y intercept of the muon candidate at the interaction point
  double     m_yint;
  /// Multiple scattering angle between line from first hits to last hit: x-proj.
  double     m_txms;
  /// Multiple scattering angle between line from first hits to last hit: y-proj.
  double     m_tyms;
  /// 4 bits per layer flagging which hits are used for the track
  int        m_ihit;
  /// 4 bits per layer flagging additional hits close by
  int        m_iahi;
};

#include "LHCbEvent/CLHEPStreams.h"
//
// Inline code must be outside the class definition
//

/// Serialize the object for reading
inline StreamBuffer& Level0MuonTrack::serialize(StreamBuffer& s) {
  ContainedObject::serialize(s) >> m_position << m_errPosition << m_dxdz << m_dydz
				>> m_errDxdz >> m_errDydz >> m_charge >> m_pt >> m_p
				>> m_theta >> m_phi >> m_yint >> m_txms >> m_tyms 
				>> m_ihit >> m_iahi;
  return s;
}

/// Serialize the object for writing
inline StreamBuffer& Level0MuonTrack::serialize(StreamBuffer& s) const {
  ContainedObject::serialize(s) << m_position << m_errPosition << m_dxdz << m_dydz
				<< m_errDxdz << m_errDydz <<  m_charge << m_pt << m_p
				<< m_theta << m_phi << m_yint << m_txms << m_tyms
				<< m_ihit << m_iahi;
  return s;
}

#endif // LHCBEVENT_LEVEL0MUONTRACK
