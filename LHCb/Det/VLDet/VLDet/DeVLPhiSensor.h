#ifndef DEVL_PHISENSOR_H 
#define DEVL_PHISENSOR_H 1

#include "DeVLSensor.h"

// Unique class identifier
static const CLID CLID_DeVLPhiSensor = 1008303 ;

/** @class DeVLPhiSensor DeVLPhiSensor.h VLDet/DeVLPhiSensor.h
 *  
 *
 */
 
namespace VLDet {
  
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
    double length;
  };
  
}

class DeVLPhiSensor : public DeVLSensor {

public:
  /// Constructor
  DeVLPhiSensor(const std::string& name = ""); 
  /// Destructor
  virtual ~DeVLPhiSensor() {} 

  /// Object identification
  static const CLID& classID() {return CLID_DeVLPhiSensor;}
  virtual const CLID& clID() const;

  /// Initialisation
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VLChannelID& channel,
                                    double& fraction,
                                    double& pitch) const;
  /// Get the nth nearest neighbour for a given channel.
  virtual StatusCode neighbour(const LHCb::VLChannelID& start, 
                               const int& nOffset, 
                               LHCb::VLChannelID& channel) const;
  /// Return a trajectory (for track fit) from strip + offset.
  virtual std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::VLChannelID& id, 
                                                     const double offset) const;

  /// Residual of 3-d point to a VLChannelID
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                              const LHCb::VLChannelID& channel,
                              double& residual,
                              double& chi2) const;
  /// Residual of a 3-d point to a VLChannelID + interstrip fraction
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                              const LHCb::VLChannelID& channel,
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
  virtual unsigned int globalZoneOfStrip(const unsigned int strip) const {
    return zoneOfStrip(strip);
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

  /// Phi range [-pi,pi] of a given zone in the global frame.
  virtual const std::pair<double,double>& globalPhiRange(unsigned int zone) const {
    return m_zonesCache[zone].globalPhiLimits;
  }
  /// R range of a given zone in the global frame.
  virtual const std::pair<double,double>& globalRRange(unsigned int zone) const {
    return m_zonesCache[zone].globalRLimits;
  }
  
  /// Phi at a given radius in the local frame
  virtual double phiOfStrip(unsigned int strip, double fraction, const double radius) const {
    const unsigned int zone = zoneOfStrip(strip);
    const double effectiveStrip = fraction + strip - m_zones[zone].firstStrip;
    return effectiveStrip * m_zones[zone].pitch + phiOffset(radius); 
  }
  /// Phi pitch in radians for a given strip
  virtual double phiPitchOfStrip(unsigned int strip) const {
    const unsigned int zone = zoneOfStrip(strip);
    return m_zones[zone].pitch;
  }
  /// Phi pitch in mm at a given radius
  virtual double phiPitchOfRadius(const double radius) const {
    const unsigned int zone = zoneOfRadius(radius);
    return m_zones[zone].pitch * radius;
  }
  
  /// Phi distance to the origin in the local frame
  virtual double distToOrigin(unsigned int strip) const {
    const unsigned int zone = zoneOfStrip(strip);
    return m_zones[zone].distToOrigin;
  }
  
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
  /// Parameterization of a strip in the global frame
  void lineParameters(const unsigned int strip, 
                      double& a, double& b, double& c,
                      double& xs, double& ys) const {
    a = m_stripsCache[strip].a;
    b = m_stripsCache[strip].b;
    c = m_stripsCache[strip].c;
    xs = m_stripsCache[strip].xs;
    ys = m_stripsCache[strip].ys;
  }

  /// Pointer to associated R sensor on the same module
  const DeVLRSensor* associatedRSensor() const {return m_associatedRSensor;}
  /// Pointer to Phi sensor on the other side of the Velo
  const DeVLPhiSensor* otherSidePhiSensor() const {return m_otherSidePhiSensor;}
  /// Pointer to R sensor on the other side of the Velo
  const DeVLRSensor* otherSideRSensor() const {return m_otherSideRSensor;}
  /// Pointer to the Phi sensor on the next station
  const DeVLPhiSensor* nextPhiSensor() const {return m_nextPhiSensor;}
  /// Pointer to the Phi sensor on the previous station
  const DeVLPhiSensor* previousPhiSensor() const {return m_previousPhiSensor;}

  // These setter functions are called by DeVL::initialize()
  /// Set the associated R sensor. 
  void setAssociatedRSensor(const DeVLRSensor* rs) {m_associatedRSensor = rs;}
  /// Set the Phi sensor on the other side of the Velo. 
  void setOtherSidePhiSensor(const DeVLPhiSensor* ps) {m_otherSidePhiSensor = ps;}
  /// Set the R sensor on the other side of the Velo. 
  void setOtherSideRSensor(const DeVLRSensor* rs) {m_otherSideRSensor = rs;}
  /// Set the Phi sensor on the next station.
  void setNextPhiSensor(const DeVLPhiSensor* ps) {m_nextPhiSensor = ps;}
  /// Set the Phi sensor on the previous station.
  void setPreviousPhiSensor(const DeVLPhiSensor* ps) {m_previousPhiSensor = ps;}

private:

  /// Initialisation from XML
  StatusCode initSensor();
  /// Calculate the strip limits and parameters in the global frame
  StatusCode updateStripCache();
  /// Calculate the zone limits in the global frame
  StatusCode updateZoneCache();
  /// Update the geometry cache when the alignment changes
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
    std::pair<double, double> globalPhiLimits;
    std::pair<double, double> globalRLimits;
  };
  std::vector<phiZoneCache> m_zonesCache;

  struct phiStripCache {
    double a, b, c;
    double xs, ys;
  };
  std::vector<phiStripCache> m_stripsCache;
 
  // References to local static members
  std::vector<VLDet::phiZone>& m_zones;
  std::vector<VLDet::phiStrip>& m_strips;

  /// Pointer to the associated R sensor on the same module
  const DeVLRSensor* m_associatedRSensor;
  /// Pointer to the Phi sensor on the other side of the Velo
  const DeVLPhiSensor* m_otherSidePhiSensor;
  /// Pointer to the R sensor on the other side of the Velo
  const DeVLRSensor* m_otherSideRSensor;
  /// Pointer to the Phi sensor on the next station
  const DeVLPhiSensor* m_nextPhiSensor;
  /// Pointer to the Phi sensor on the previous station
  const DeVLPhiSensor* m_previousPhiSensor;

  /// Output level for message service
  bool m_debug;
  bool m_verbose;

};

/// Fast cast to Phi sensor, returns 0 for wrong type
inline const DeVLPhiSensor* DeVLSensor::phiSensor() const { 
  return (m_isPhi ? static_cast<const DeVLPhiSensor*>(this) : 0); 
}
  
#endif
