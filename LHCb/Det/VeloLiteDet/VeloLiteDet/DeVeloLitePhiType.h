#ifndef DEVELOLITE_PHITYPE_H 
#define DEVELOLITE_PHITYPE_H 1

#include "DeVeloLiteSensor.h"

// Unique class identifier
static const CLID CLID_DeVeloLitePhiType = 1008303 ;

/** @class DeVeloLitePhiType DeVeloLitePhiType.h VeloLiteDet/DeVeloLitePhiType.h
 *  
 *
 */
 
namespace VeloLiteDet {
  
  struct phiZone {
    unsigned int firstStrip;
    unsigned int nbStrips;
    double coverage;
    double pitch;
    double rMin, rMax;
    double distToOrigin;
    double tilt;
  };
  
  struct phiStrip {
    int zone;
    double gradient;
    double intercept;
    double length;
  };
  
}

class DeVeloLitePhiType : public DeVeloLiteSensor {

public:
  /// Constructor
  DeVeloLitePhiType(const std::string& name = ""); 
  /// Destructor
  virtual ~DeVeloLitePhiType() {} 

  /// Object identification
  static const CLID& classID() {return CLID_DeVeloLitePhiType;}
  virtual const CLID& clID() const;

  /// Initialisation
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VeloLiteChannelID& channel,
                                    double& fraction,
                                    double& pitch) const;
  /// Get the nth nearest neighbour for a given channel.
  virtual StatusCode neighbour(const LHCb::VeloLiteChannelID& start, 
                               const int& nOffset, 
                               LHCb::VeloLiteChannelID& channel) const;
  /// Return a trajectory (for track fit) from strip + offset.
  virtual std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::VeloLiteChannelID& id, 
                                                     const double offset) const;

  /// Residual of 3-d point to a VeloLiteChannelID
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                              const LHCb::VeloLiteChannelID& channel,
                              double& residual,
                              double& chi2) const;
  /// Residual of a 3-d point to a VeloLiteChannelID + interstrip fraction
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                              const LHCb::VeloLiteChannelID& channel,
                              const double interStripFraction,
                              double& residual,
                              double& chi2) const;

  /// Determine if a local 3-d point is inside the sensor.
  virtual StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const;

  /// Zone number for a given strip
  virtual unsigned int zoneOfStrip(const unsigned int strip) const {
    unsigned int zone = 0;
    for (unsigned int i = m_numberOfZones; i--;) {
      if (strip >= m_zones[i].firstStrip) {
        zone = i;
        break;
      }
    }
    return zone;
  }
  /// Number of strips in a given zone
  virtual unsigned int stripsInZone(const unsigned int zone) const {
    return m_zones[zone].nbStrips;
  }
  /// Minimum radius of a given zone
  virtual double rMin(const unsigned int zone) const {
    return m_zones[zone].rMin;
  }
  /// Maximum radius of a given zone
  virtual double rMax(const unsigned int zone) const {
    return m_zones[zone].rMax;
  }
  /// Length of a given strip
  virtual double stripLength(const unsigned int strip) const {
    return m_strips[strip].length;
  }

  // =================================================================
  
  /// Zone number for a given local radius
  unsigned int zoneOfRadius(const double radius) const {
    unsigned int zone = 0;
    for (unsigned int i = m_numberOfZones; i--;) {
      if (radius >= m_zones[i].rMin) {
        zone = i;
        break;
      }
    }
    return zone;
  }  

  // =================================================================

  /// Phi range [-pi,pi] of a given zone in the global frame.
  virtual const std::pair<double,double>& globalPhiRange(unsigned int zone) const {
   return m_zonesCache[zone].globalPhiLimits;
  }
  /// Phi range [-pi,pi] of a given zone in the halfbox frame.
  virtual const std::pair<double,double>& halfboxPhiRange(unsigned int zone) const {
    return m_zonesCache[zone].halfboxPhiLimits;
  }
  /// R range of a given zone in the global frame.
  virtual const std::pair<double,double>& globalRRange(unsigned int zone) const {
    return m_zonesCache[zone].globalRLimits;
  }
  /// R range of a given zone in the halfbox frame.
  virtual const std::pair<double,double>& halfboxRRange(unsigned int zone) const {
    return m_zonesCache[zone].halfboxRLimits;
  }
  
  // =================================================================

  /// Phi at strip centre in the global frame
  virtual double globalPhiOfStrip(unsigned int strip) const {
    return m_stripsCache[strip].globalPhi;
  }
  /// Phi at strip centre in the halfbox frame
  virtual double halfboxPhiOfStrip(unsigned int strip) const {
    return m_stripsCache[strip].halfboxPhi;
  }
  /// Phi at strip centre with ideal alignment
  virtual double idealPhiOfStrip(unsigned int strip) const {
    return m_stripsCache[strip].idealPhi;
  }

  /// Phi for a given inter strip fraction and strip in the global frame
  virtual double globalPhiOfStrip(unsigned int strip, double fraction) const {
    double phi = m_stripsCache[strip].globalPhi + 
                 fraction * globalPhiPitch(strip);
    if (phi < -Gaudi::Units::pi) phi += 2 * Gaudi::Units::pi;
    if (phi >  Gaudi::Units::pi) phi -= 2 * Gaudi::Units::pi;
    return phi; 
  }
  /// Phi for a given inter strip fraction and strip in the halfbox frame
  virtual double halfboxPhiOfStrip(unsigned int strip, double fraction) const {
    double phi = m_stripsCache[strip].halfboxPhi + 
                 fraction * globalPhiPitch(strip);
    if (phi < -Gaudi::Units::pi) phi += 2 * Gaudi::Units::pi;
    if (phi >  Gaudi::Units::pi) phi -= 2 * Gaudi::Units::pi;
    return phi; 
  }
  /// Phi for a given inter strip fraction and strip in the ideal frame
  virtual double idealPhiOfStrip(unsigned int strip, double fraction) const {
    double phi = m_stripsCache[strip].idealPhi + 
                 fraction * globalPhiPitch(strip);
    if (phi < -Gaudi::Units::pi) phi += 2 * Gaudi::Units::pi;
    if (phi >  Gaudi::Units::pi) phi -= 2 * Gaudi::Units::pi;
    return phi; 
  }

  /// Phi at a given radius in the local frame
  virtual double phiOfStrip(unsigned int strip, double fraction, const double radius) const {
    const unsigned int zone = zoneOfStrip(strip);
    const double effectiveStrip = fraction + strip - m_zones[zone].firstStrip;
    return effectiveStrip * m_zones[zone].pitch + phiOffset(radius); 
  }
  /// Phi at a given radius in the global frame
  virtual double globalPhi(unsigned int strip, double fraction, double radius) const {
    const unsigned int zone = zoneOfStrip(strip);
    const double c0 = m_zonesCache[zone].globalOffsetAtR0;
    const double d0 = m_zonesCache[zone].globalDistToOrigin;
    const double offset = safePhiOffset(c0, d0, radius);
    double phi = globalPhiOfStrip(strip) + 
                 fraction * globalPhiPitch(strip) + offset;
    if (phi < -Gaudi::Units::pi) phi += 2 * Gaudi::Units::pi;
    if (phi >  Gaudi::Units::pi) phi -= 2 * Gaudi::Units::pi;
    return phi; 
  }
  /// Phi at a given radius in the halfbox frame
  virtual double halfboxPhi(unsigned int strip, double fraction, double radius) const {
    const unsigned int zone = zoneOfStrip(strip);
    const double c0 = m_zonesCache[zone].halfboxOffsetAtR0;
    const double d0 = m_zonesCache[zone].halfboxDistToOrigin;
    const double offset = safePhiOffset(c0, d0, radius);
    double phi = halfboxPhiOfStrip(strip) + 
                 fraction * globalPhiPitch(strip) + offset;
    if (phi < -Gaudi::Units::pi) phi += 2 * Gaudi::Units::pi;
    if (phi >  Gaudi::Units::pi) phi -= 2 * Gaudi::Units::pi;
    return phi; 
  }
  /// Phi at a given radius in the ideal frame
  virtual double idealPhi(unsigned int strip, double fraction, double radius) const {
    const unsigned int zone = zoneOfStrip(strip);
    const double c0 = m_zonesCache[zone].idealOffsetAtR0;
    const double d0 = m_zonesCache[zone].idealDistToOrigin;
    const double offset = safePhiOffset(c0, d0, radius);
    double phi = idealPhiOfStrip(strip) + 
                 fraction * globalPhiPitch(strip) + offset;
    if (phi < -Gaudi::Units::pi) phi += 2 * Gaudi::Units::pi;
    if (phi >  Gaudi::Units::pi) phi -= 2 * Gaudi::Units::pi;
    return phi; 
  }

  // =================================================================

  /// Phi pitch in radians for a given strip
  virtual double phiPitchOfStrip(unsigned int strip) const {
    const unsigned int zone = zoneOfStrip(strip);
    return m_zones[zone].pitch;
  }
  /// Phi pitch in radians for a given strip (with correct sign) in the global frame  
  virtual double globalPhiPitchOfStrip(unsigned int strip) const {
    return isDownstream() ? -phiPitchOfStrip(strip) : phiPitchOfStrip(strip);
  }
  /// Phi pitch in mm at a given radius
  virtual double phiPitch(const double radius) const {
    const unsigned int zone = zoneOfRadius(radius);
    return m_zones[zone].pitch * radius;
  }
  /// Phi pitch in mm at a given radius (with correct sign) in the global frame
  virtual double globalPhiPitch(double radius) const {
    return isDownstream() ? -phiPitch(radius) : phiPitch(radius);
  }
  
  // =================================================================

  /// Get the global frame phi offset at a given radius in a given zone 
  virtual double globalPhiOffset(unsigned int zone, double radius) const {
    const double c0 = m_zonesCache[zone].globalOffsetAtR0;
    const double d0 = m_zonesCache[zone].globalDistToOrigin;
    return safePhiOffset(c0, d0, radius);
  }
  /// Get the halfbox frame phi offset at a given radius in a given zone 
  virtual double halfboxPhiOffset(unsigned int zone, double radius) const {
    const double c0 = m_zonesCache[zone].halfboxOffsetAtR0;
    const double d0 = m_zonesCache[zone].halfboxDistToOrigin;
    return safePhiOffset(c0, d0, radius);
  }
  /// Get the ideal frame phi offset at a given radius in a given zone 
  virtual double idealPhiOffset(unsigned int zone, double radius) const {
    const double c0 = m_zonesCache[zone].idealOffsetAtR0;
    const double d0 = m_zonesCache[zone].idealDistToOrigin;
    return safePhiOffset(c0, d0, radius);
  }
  
  /// Phi distance to the origin in the local frame
  virtual double distToOrigin(unsigned int strip) const {
    const unsigned int zone = zoneOfStrip(strip);
    return m_zones[zone].distToOrigin;
  }
  /// Phi distance to origin in the global frame
  virtual double globalDistToOrigin(unsigned int zone) const {
    return m_zonesCache[zone].globalDistToOrigin;
  }
  /// Phi distance to origin in the halfbox frame
  virtual double halfboxDistToOrigin(unsigned int zone) const {
    return m_zonesCache[zone].halfboxDistToOrigin;
  }
  /// Phi distance to origin in the ideal frame
  virtual double idealDistToOrigin(unsigned int zone) const {
    return m_zonesCache[zone].idealDistToOrigin;
  }
  
  // =================================================================

  /// Calculate the angle of the strip wrt to the x axis in the local frame
  virtual double angleOfStrip(unsigned int strip, double fraction = 0.) const {
    const unsigned int zone = zoneOfStrip(strip);
    const double effectiveStrip = fraction + strip - m_zones[zone].firstStrip;
    return effectiveStrip * m_zones[zone].pitch + m_zones[zone].tilt;
  }
  virtual double phiTilt(unsigned int strip) {
    const unsigned int zone = zoneOfStrip(strip);
    return m_zones[zone].tilt;
  }

  /// Pointer to associated R sensor on the same module
  const DeVeloLiteRType* associatedRSensor() const {return m_associatedRSensor;}
  /// Pointer to Phi sensor on the other side of the VELO
  const DeVeloLitePhiType* otherSidePhiSensor() const {return m_otherSidePhiSensor;}
  /// Pointer to R sensor on the other side of the VELO
  const DeVeloLiteRType* otherSideRSensor() const {return m_otherSideRSensor;}
  
  /// Set the associated R sensor. To be called by DeVeloLite::initialize().
  void setAssociatedRSensor(const DeVeloLiteRType* rs) {m_associatedRSensor = rs;}
  /// Set the Phi sensor on the other side of the VELO. To be called by DeVeloLite::initialize()
  void setOtherSidePhiSensor(const DeVeloLitePhiType* ps) {m_otherSidePhiSensor = ps;}
  /// Set the R sensor on the other side of the VELO. To be called by DeVeloLite::initialize()
  void setOtherSideRSensor(const DeVeloLiteRType* rs) {m_otherSideRSensor = rs;}

private:

  /// Initialisation from XML
  StatusCode initSensor();
  /// Calculate the global and half box phi values 
  /// for the strip centres when the alignment changes.
  StatusCode updateStripCache();
  /// Calculate the zone limits in the global and halfbox frame.
  StatusCode updateZoneCache();
  /// Update the geometry cache when the alignment changes.
  StatusCode updateGeometryCache();
  /// Build up map of strip to routing line conversions
  void buildRoutingLineMap();
  
  /// Return the offset in phi for a given radius
  double phiOffset(double radius) const {
    const unsigned int zone = zoneOfRadius(radius);
    return -1. * safePhiOffset(m_zones[zone].tilt, 
                               m_zones[zone].distToOrigin, radius);
  }
  /// Calculate the phi offset for a strip at radius given the 
  /// d0 and the c0 of the strip: protected agains impossible radii
  double safePhiOffset(double const &c0, double const &d0, 
                       double const & radius) const {
    return (fabs(d0 / radius) < 1.) ? asin(d0 / radius) - c0 : asin(1.) - c0;
  }

  bool m_down;
  double m_phiOrigin;
  /// Phi resolution of the sensor (gradient and constant)
  std::pair<double, double> m_resolution;

  struct phiZoneCache {
    double idealDistToOrigin;
    double halfboxDistToOrigin;
    double globalDistToOrigin;
    double idealOffsetAtR0;
    double halfboxOffsetAtR0;
    double globalOffsetAtR0;
    std::pair<double, double> halfboxPhiLimits;
    std::pair<double, double> globalPhiLimits;
    std::pair<double, double> halfboxRLimits;
    std::pair<double, double> globalRLimits;
  };
  std::vector<phiZoneCache> m_zonesCache;

  struct phiStripCache {
    double idealPhi;
    double halfboxPhi;
    double globalPhi;
  };
  std::vector<phiStripCache> m_stripsCache;
 
  // References to local static members
  std::vector<VeloLiteDet::phiZone>& m_zones;
  std::vector<VeloLiteDet::phiStrip>& m_strips;

  /// Pointer to the associated R sensor on the same module
  const DeVeloLiteRType* m_associatedRSensor;
  /// Pointer to the Phi sensor on the other side of the Velo
  const DeVeloLitePhiType* m_otherSidePhiSensor;
  /// Pointer to the R sensor on the other side of the Velo
  const DeVeloLiteRType* m_otherSideRSensor;

  /// Output level for message service
  bool m_debug;
  bool m_verbose;

};

/// Fast cast to Phi sensor, returns 0 for wrong type
inline const DeVeloLitePhiType* DeVeloLiteSensor::phiType() const { 
  return (m_isPhi ? static_cast<const DeVeloLitePhiType*>(this) : 0); 
}
  
#endif
