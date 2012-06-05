#ifndef DEVELOLITE_RTYPE_H 
#define DEVELOLITE_RTYPE_H 1

#include "DeVeloLiteSensor.h"

// Unique class identifier
static const CLID CLID_DeVeloLiteRType = 1008302;

/** @class DeVeloLiteRType DeVeloLiteRType.h VeloLiteDet/DeVeloLiteRType.h
 *  
 *
 */
 
namespace VeloLiteDet {

  struct rZone {
    unsigned int firstStrip;
    unsigned int nbStrips;
    double phiMin, phiMax;
    double rMin, rMax;
    unsigned int firstStripLogPitch;
  };

  struct rStrip {
    int zone;
    double r;
    double pitch;
    double phiMin, phiMax;
    std::pair<double, double> phiLimits;
  };
  
}

class DeVeloLiteRType : public DeVeloLiteSensor {

public:
  /// Constructor
  DeVeloLiteRType(const std::string& name = ""); 
  /// Destructor
  virtual ~DeVeloLiteRType() {} 

  /// Object identification
  static const CLID& classID() {return CLID_DeVeloLiteRType;}
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
  /// Minimum strip number in a zone
  virtual unsigned int firstStrip(unsigned int zone) const {
    return m_zones[zone].firstStrip;
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
  virtual double stripLength(const unsigned int strip) const;

  // =================================================================

  /// Zone number for a given local phi and radius
  unsigned int zoneOfPhiAndR(const double phi, const double r) const {
    unsigned int zone = 0;
    if (phi < m_zones[0].phiMax && r < m_zones[0].rMax) {
      zone = 0;
    } else if (phi > m_zones[m_numberOfZones - 1].phiMin &&
               r < m_zones[m_numberOfZones - 1].rMax) {
      zone = m_numberOfZones - 1;
    } else if (phi > m_zones[2].phiMin && phi < m_zones[2].phiMax) {
      zone = 2;
    } else {
      if (phi < 0) {
        zone = 1;
      } else {
        zone = m_numberOfZones - 2;
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

  /// Radius of the centre of a strip in the local frame
  virtual double rOfStrip(const unsigned int strip) const {
    return m_strips[strip].r;
  }
  /// Radius of the centre of the strip in the global frame
  virtual double globalROfStrip(unsigned int strip) const { 
    return m_stripsCache[strip].globalR;
  }  
  /// Radius of the centre of the strip in the halfbox frame
  virtual double halfboxROfStrip(unsigned int strip) const {
    return m_stripsCache[strip].halfboxR;
  }
  /// Radius of a strip + interstrip fraction * pitch in the local frame
  virtual double rOfStrip(unsigned int strip, double fraction) const {
    return m_strips[strip].r + fraction * rPitch(strip);
  }
  /// Radius of the centre of strip plus interstrip fraction * pitch in the global frame
  virtual double globalROfStrip(unsigned int strip, double fraction) const {
    return m_stripsCache[strip].globalR + fraction * rPitch(strip);
  }
  /// Radius of the centre of strip plus interstrip fraction * pitch in the halfbox frame
  virtual double halfboxROfStrip(unsigned int strip, double fraction) const {
    return m_stripsCache[strip].halfboxR + fraction * rPitch(strip);
  }
  
  // =================================================================

  /// Local pitch at a given strip
  virtual double rPitch(unsigned int strip) const {
    return m_strips[strip].pitch;
  }

  /// Local pitch at a given radius 
  virtual double rPitchOfRadius(double radius) const {
    double pitch = m_innerPitch;
    if (radius > m_rLogPitch) {
      pitch *= pow(radius / m_rLogPitch, m_pitchSlope);
    }
    return pitch; 
  }

  // =================================================================
  
  /// Minimum phi in a zone assuming the radius is not known
  // TODO: undefined!
  virtual double phiMinZone(unsigned int zone) const {return m_zones[zone].phiMin;}
  /// Minimum phi in a zone at given radius
  virtual double phiMinZone(unsigned int zone, double radius) const;
  /// Maximum phi in a zone assuming the radius is not known
  // TODO: undefined!
  virtual double phiMaxZone(unsigned int zone) const {return m_zones[zone].phiMax;}
  /// Maximum phi in a zone at given radius
  virtual double phiMaxZone(unsigned int zone, double radius) const;
  /// Minimum phi of a strip
  virtual double phiMinStrip(unsigned int strip) const {return m_strips[strip].phiMin;}
  /// Maximum phi of a strip
  virtual double phiMaxStrip(unsigned int strip) const {return m_strips[strip].phiMax;}
 
  /// Return the strip limits for Panoramix
  StatusCode stripLimits(unsigned int strip, double &radius,
                         double &phiMin, double &phiMax) const {
    radius = rOfStrip(strip);
    phiMin = phiMinStrip(strip);
    phiMax = phiMaxStrip(strip);
    return StatusCode::SUCCESS;
  }

  /// Pointer to the associated Phi sensor on the same module
  const DeVeloLitePhiType* associatedPhiSensor() const {return m_associatedPhiSensor;}  
  /// Pointer to the R sensor on the other side of the Velo
  const DeVeloLiteRType* otherSideRSensor() const {return m_otherSideRSensor;}
  /// Pointer to the Phi sensor on the other side of the Velo
  const DeVeloLitePhiType* otherSidePhiSensor() const {return m_otherSidePhiSensor;}
  
  /// Set the associated Phi sensor. To be called by DeVeloLite::initialize().
  void setAssociatedPhiSensor(const DeVeloLitePhiType* ps) {m_associatedPhiSensor = ps;}
  /// Set the R sensor on the other side of the Velo. To be called by DeVeloLite::initialize().
  void setOtherSideRSensor(const DeVeloLiteRType* rs) {m_otherSideRSensor = rs;}
  /// Set the Phi sensor on the other side of the Velo. To be called by DeVeloLite::initialize().
  void setOtherSidePhiSensor(const DeVeloLitePhiType* ps) {m_otherSidePhiSensor = ps;}
 
private:

  /// Initialisation from XML
  StatusCode initSensor();
  /// Calculate the global and half box strip radii when the alignment changes.
  StatusCode updateStripCache();
  /// Calculate the zone limits in the global and halfbox frame.
  StatusCode updateZoneCache();
  /// Update the geometry cache when the alignment changes.
  StatusCode updateGeometryCache();
  /// Build up map of strip to routing line conversions
  void buildRoutingLineMap();
  
  double m_innerPitch;
  double m_rLogPitch;  
  double m_pitchSlope;
  int m_numberOfStripsPerEar;
  double m_overhang;
  double m_phiGap;
  /// R resolution of the sensor (gradient and constant)
  std::pair<double, double> m_resolution;

  struct rZoneCache {
    std::pair<double, double> halfboxPhiLimits;
    std::pair<double, double> globalPhiLimits;
    std::pair<double, double> halfboxRLimits;
    std::pair<double, double> globalRLimits;
  };
  std::vector<rZoneCache> m_zonesCache;

  struct rStripCache {
    double halfboxR;
    double globalR;
  };
  std::vector<rStripCache> m_stripsCache;

  // References to local static members.
  std::vector<VeloLiteDet::rZone>& m_zones;
  std::vector<VeloLiteDet::rStrip>& m_strips;

  /// Pointer to the associated Phi sensor on the same module
  const DeVeloLitePhiType* m_associatedPhiSensor;
  /// Pointer to the R sensor on the other side of the Velo
  const DeVeloLiteRType* m_otherSideRSensor;
  /// Pointer to the Phi sensor on the other side of the Velo
  const DeVeloLitePhiType* m_otherSidePhiSensor;

  /// Output level for message service
  bool m_debug;
  bool m_verbose;

};

/// Fast cast to R sensor, returns 0 for wrong type
inline const DeVeloLiteRType* DeVeloLiteSensor::rType() const { 
  return (m_isR || m_isPileUp ? static_cast<const DeVeloLiteRType*>(this) : 0); 
}

#endif
