#ifndef DEVL_RSENSOR_H 
#define DEVL_RSENSOR_H 1

#include "DeVLSensor.h"

// Unique class identifier
static const CLID CLID_DeVLRSensor = 1008302;

/** @class DeVLRSensor DeVLRSensor.h VLDet/DeVLRSensor.h
 *  
 *
 */
 
namespace VLDet {

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

class DeVLRSensor : public DeVLSensor {

public:
  /// Constructor
  DeVLRSensor(const std::string& name = ""); 
  /// Destructor
  virtual ~DeVLRSensor() {} 

  /// Object identification
  static const CLID& classID() {return CLID_DeVLRSensor;}
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

  /// Calculate the residual of a 3d point to a strip.
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                              const LHCb::VLChannelID& channel,
                              double& residual,
                              double& chi2) const;
  /// Calculate the residual of a 3d point to a strip + interstrip fraction
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                              const LHCb::VLChannelID& channel,
                              const double interStripFraction,
                              double& residual,
                              double& chi2) const;

  /// Determine if a local 3d point is inside the sensor.
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
    unsigned int zone = zoneOfStrip(strip);
    if (isDownstream()) zone = m_numberOfZones - 1 - zone;
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

  /// Zone number for a given local phi and radius
  unsigned int zoneOfPhiAndR(const double phi, const double r) const {
    unsigned int zone = 0;
    if (phi < m_zones[0].phiMax && 
        r   < m_zones[0].rMax) {
      zone = 0;
    } else if (phi > m_zones[m_numberOfZones - 1].phiMin &&  
               r   < m_zones[m_numberOfZones - 1].rMax) {
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

  /// Radius of the centre of a strip in the local frame
  virtual double rOfStrip(const unsigned int strip) const {
    return m_strips[strip].r;
  }
  /// Radius of the centre of the strip in the global frame
  virtual double globalROfStrip(unsigned int strip) const { 
    return m_stripsCache[strip].globalR;
  }  
  /// Radius of a strip + interstrip fraction * pitch in the local frame
  virtual double rOfStrip(unsigned int strip, double fraction) const {
    return m_strips[strip].r + fraction * rPitchOfStrip(strip);
  }
  /// Radius of the centre of strip plus interstrip fraction * pitch in the global frame
  virtual double globalROfStrip(unsigned int strip, double fraction) const {
    return m_stripsCache[strip].globalR + fraction * rPitchOfStrip(strip);
  }
  
  /// Local pitch at a given strip
  virtual double rPitchOfStrip(unsigned int strip) const {
    return m_strips[strip].pitch;
  }
  /// Local pitch at a given radius 
  virtual double rPitchOfRadius(double radius) const {
    double pitch = m_innerPitch;
    if (m_useLogPitch) {
      if (radius > m_rLogPitch) {
        pitch *= pow(radius / m_rLogPitch, m_pitchExp);
      }
    } else {
      pitch += m_pitchSlope * (radius - innerRadius());
    }
    return pitch; 
  }

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
  StatusCode stripLimits(unsigned int strip, double& radius,
                         double& phiMin, double& phiMax) const {
    radius = rOfStrip(strip);
    phiMin = phiMinStrip(strip);
    phiMax = phiMaxStrip(strip);
    return StatusCode::SUCCESS;
  }

  /// Pointer to the associated Phi sensor on the same module
  const DeVLPhiSensor* associatedPhiSensor() const {return m_associatedPhiSensor;}  
  /// Pointer to the R sensor on the other side of the Velo
  const DeVLRSensor* otherSideRSensor() const {return m_otherSideRSensor;}
  /// Pointer to the Phi sensor on the other side of the Velo
  const DeVLPhiSensor* otherSidePhiSensor() const {return m_otherSidePhiSensor;}
  /// Pointer to the R sensor on the next station
  const DeVLRSensor* nextRSensor() const {return m_nextRSensor;}
  /// Pointer to the R sensor on the previous station
  const DeVLRSensor* previousRSensor() const {return m_previousRSensor;}
  
  // These setter functions are called by DeVL::initialize()
  /// Set the associated Phi sensor. 
  void setAssociatedPhiSensor(const DeVLPhiSensor* ps) {m_associatedPhiSensor = ps;}
  /// Set the R sensor on the other side of the Velo. 
  void setOtherSideRSensor(const DeVLRSensor* rs) {m_otherSideRSensor = rs;}
  /// Set the Phi sensor on the other side of the Velo. 
  void setOtherSidePhiSensor(const DeVLPhiSensor* ps) {m_otherSidePhiSensor = ps;}
  /// Set the R sensor on the next station.
  void setNextRSensor(const DeVLRSensor* rs) {m_nextRSensor = rs;}
  /// Set the R sensor on the previous station.
  void setPreviousRSensor(const DeVLRSensor* rs) {m_previousRSensor = rs;}
 
private:

  /// Initialisation from XML
  StatusCode initSensor();
  /// Calculate the global strip radii when the alignment changes.
  StatusCode updateStripCache();
  /// Calculate the zone limits in the global frame.
  StatusCode updateZoneCache();
  /// Update the geometry cache when the alignment changes.
  StatusCode updateGeometryCache();
  /// Build up map of strip to routing line conversions
  void buildRoutingLineMap();
  bool m_useLogPitch; 
  double m_innerPitch;
  double m_rLogPitch; 
  double m_pitchSlope;
  double m_pitchExp;
  int m_numberOfStripsPerEar;
  double m_overhang;
  double m_phiGap;
  /// R resolution of the sensor (gradient and constant)
  std::pair<double, double> m_resolution;

  struct rStripCache {
    float globalR;
  };
  std::vector<rStripCache> m_stripsCache;

  // References to local static members.
  std::vector<VLDet::rZone>& m_zones;
  std::vector<VLDet::rStrip>& m_strips;

  /// Pointer to the associated Phi sensor on the same module
  const DeVLPhiSensor* m_associatedPhiSensor;
  /// Pointer to the R sensor on the other side of the Velo
  const DeVLRSensor* m_otherSideRSensor;
  /// Pointer to the Phi sensor on the other side of the Velo
  const DeVLPhiSensor* m_otherSidePhiSensor;
  /// Pointer to the R sensor on the next station
  const DeVLRSensor* m_nextRSensor;
  /// Pointer to the R sensor on the previous station
  const DeVLRSensor* m_previousRSensor;

  /// Output level for message service
  bool m_debug;
  bool m_verbose;

};

/// Fast cast to R sensor, returns 0 for wrong type
inline const DeVLRSensor* DeVLSensor::rSensor() const { 
  return (m_isR ? static_cast<const DeVLRSensor*>(this) : 0); 
}

#endif
