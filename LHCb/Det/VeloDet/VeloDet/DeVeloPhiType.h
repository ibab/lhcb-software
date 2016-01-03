#ifndef VELODET_DEVELOPHITYPE_H 
#define VELODET_DEVELOPHITYPE_H 1

// Include files
#include <cmath>
#include "vdt/asin.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Gaudi Math definitions
#include "GaudiKernel/Point3DTypes.h"

// from VeloDet
#include "VeloDet/DeVeloSensor.h"

// get trajectory
#include "Kernel/Trajectory.h"

// Unique class identifier
static const CLID CLID_DeVeloPhiType = 1008103 ;

// Forward declaration needed for link to associated R sensor
class DeVeloRType;

namespace LHCb {
  class VeloChannelID;
}

/** @class DeVeloPhiType DeVeloPhiType.h VeloDet/DeVeloPhiType.h
 *  
 *
 *  @author Mark Tobin
 *  @author Kurt Rinnert kurt.rinnert@cern.ch
 *  @date   2003-01-14
 */
class DeVeloPhiType : public DeVeloSensor {
public:
  /// Standard constructor
  DeVeloPhiType(  const std::string& name  = ""); 

  ~DeVeloPhiType( ); ///< Destructor

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVeloPhiType; }
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVeloSensor from the XML
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                            LHCb::VeloChannelID& channel,
                            double& localOffset,
                            double& pitch) const;
  
  /// Get the nth nearest neighbour for a given channel
  virtual StatusCode neighbour(const LHCb::VeloChannelID& start, 
                       const int& nOffset, 
                       LHCb::VeloChannelID& channel) const;

  /// Return a trajectory (for track fit) from strip + offset
  virtual std::unique_ptr<LHCb::Trajectory> trajectory(const LHCb::VeloChannelID& id, const double offset) const;

  /// Residual of 3-d point to a VeloChannelID
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                      const LHCb::VeloChannelID& channel,
                      double &residual,
                      double &chi2) const;



  /// Residual [see DeVelo for explanation]
  /*virtual StatusCode residual(const Gaudi::XYZPoint& point,
                      const LHCb::VeloChannelID& channel,
                      const double localOffset,
                      const double width,
                      double &residual,
                      double &chi2) const;*/

  /// Residual of a 3-d point to a VeloChannelID + interstrip fraction
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                              const LHCb::VeloChannelID& channel,
                              const double interStripFraction,
                              double &residual,
                              double &chi2) const;

  /// The number of zones in the detector
  virtual unsigned int numberOfZones() const {return m_numberOfZones;}
  
  /// The zones number for a given strip
  virtual unsigned int zoneOfStrip(const unsigned int strip) const {
    return (m_nbInner > strip) ? 0 : 1;
  }
  
  /// The number of strips in a zone
  virtual unsigned int stripsInZone(const unsigned int zone) const {
    if(0 == zone) {
      return m_nbInner;
    } else if(1 == zone){
      return numberOfStrips()-m_nbInner;
    }
    return 0;
  }
  
  /// The minimum radius for a given zone of the sensor
  virtual double rMin(const unsigned int zone) const;
  
  /// The maximum radius for a given zone of the sensor
  virtual double rMax(const unsigned int zone) const;

  /// The phi range [-pi,pi] of the given zone in the global frame.
  const std::pair<double,double>& globalPhiRange(unsigned int zone) const { return m_globalPhiLimitsZone[zone]; }
  
  /// The phi range [-pi,pi] of the given zone in the halfbox frame.
  const std::pair<double,double>& halfboxPhiRange(unsigned int zone) const { return m_halfboxPhiLimitsZone[zone]; }
  
  /// The r range of the given zone in the global frame.
  const std::pair<double,double>& globalRRange(unsigned int zone) const { return m_globalRLimitsZone[zone]; }
  
  /// The r range of the given zone in the halfbox frame.
  const std::pair<double,double>& halfboxRRange(unsigned int zone) const { return m_halfboxRLimitsZone[zone]; }
  
  /// Determines if 3-d point is inside sensor
  virtual StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const;

  /// Determine if local point is in corner cut-offs
  virtual bool isCutOff(double x, double y) const;

  /// Return the length of a strip
  virtual double stripLength(const unsigned int strip) const;
  
  /// The angle of the strip wrt to the x axis in the local frame
  inline double angleOfStrip(unsigned int strip, double fraction=0.) const{
    double effectiveStrip=fraction+static_cast<double>(strip);
    if (m_nbInner > strip) {
      return (effectiveStrip*m_innerPitch) + m_innerTilt;
    } else {
      effectiveStrip -= m_nbInner;
      return (effectiveStrip*m_outerPitch) + m_outerTilt;
    }
  }

  /// Global phi at strip centre with full alignment
  inline double globalPhiOfStrip(unsigned int strip) const { return m_globalPhi[strip]; }
  
  /// phi at strip centre in the halfbox frame
  inline double halfboxPhiOfStrip(unsigned int strip) const { return m_halfboxPhi[strip]; }
  
  /// phi at strip centre with ideal alignment
  inline double idealPhiOfStrip(unsigned int strip) const { return m_idealPhi[strip]; }
 
  /// get the global frame phi offset at a given radius in the given zone 
  inline double globalPhiOffset(unsigned int zone, double radius) const {
    double c0 = m_globalOffsetAtR0[zone];
    double d0 = m_globalDistToOrigin[zone];
    return safePhiOffset(c0,d0,radius);
  }
  
  /// get the halfbox frame phi offset at a given radius in the given zone 
  inline double halfboxPhiOffset(unsigned int zone, double radius) const {
    double c0 = m_halfboxOffsetAtR0[zone];
    double d0 = m_halfboxDistToOrigin[zone];
    return safePhiOffset(c0,d0,radius);
  }
  
  /// get the ideal frame phi offset at a given radius in the given zone 
  inline double idealPhiOffset(unsigned int zone, double radius) const {
    double c0 = m_idealOffsetAtR0[zone];
    double d0 = m_idealDistToOrigin[zone];
    return safePhiOffset(c0,d0,radius);
  }
  
  /// get the global frame phi distance to origin 
  inline double globalDistToOrigin(unsigned int zone) const {
    return m_globalDistToOrigin[zone];
  }
  
  /// get the half box frame phi distance to origin 
  inline double halfboxDistToOrigin(unsigned int zone) const {
    return m_halfboxDistToOrigin[zone];
  }
  
  /// get the ideal frame phi distance to origin 
  inline double idealDistToOrigin(unsigned int zone) const {
    return m_idealDistToOrigin[zone];
  }
  
  /// phi for a given inter strip fraction and strip in the global frame
  inline double globalPhi(unsigned int strip, double fraction) const {
    double phi = m_globalPhi[strip] + fraction*globalPhiPitch(strip);
    phi = phi < -Gaudi::Units::pi ? phi + 2.0*Gaudi::Units::pi : phi;
    phi = phi >  Gaudi::Units::pi ? phi - 2.0*Gaudi::Units::pi : phi;
    return phi; 
  }

  /// phi at a given radius in the global frame
  inline double globalPhi(unsigned int strip, double fraction, double radius) const {
    double c0     = strip < m_nbInner? m_globalOffsetAtR0[0] : m_globalOffsetAtR0[1];
    double d0     = strip < m_nbInner? m_globalDistToOrigin[0] : m_globalDistToOrigin[1];
    double offset =  safePhiOffset(c0,d0,radius);
    double phi    = m_globalPhi[strip] + fraction*globalPhiPitch(strip) + offset;
    phi = phi < -Gaudi::Units::pi ? phi + 2.0*Gaudi::Units::pi : phi;
    phi = phi >  Gaudi::Units::pi ? phi - 2.0*Gaudi::Units::pi : phi;
    return phi; 
  }
  
  /// phi for a given inter strip fraction and strip in the halfbox frame
  inline double halfboxPhi(unsigned int strip, double fraction) const {
    double phi = m_halfboxPhi[strip] + fraction*globalPhiPitch(strip);
    phi = phi < -Gaudi::Units::pi ? phi + 2.0*Gaudi::Units::pi : phi;
    phi = phi >  Gaudi::Units::pi ? phi - 2.0*Gaudi::Units::pi : phi;
    return phi; 
  }

  /// phi at a given radius in the halfbox frame
  inline double halfboxPhi(unsigned int strip, double fraction, double radius) const {
    double c0     = strip < m_nbInner? m_halfboxOffsetAtR0[0] : m_halfboxOffsetAtR0[1];
    double d0     = strip < m_nbInner? m_halfboxDistToOrigin[0] : m_halfboxDistToOrigin[1];
    double offset =  safePhiOffset(c0,d0,radius);
    double phi    = m_halfboxPhi[strip] + fraction*globalPhiPitch(strip) + offset;
    phi = phi < -Gaudi::Units::pi ? phi + 2.0*Gaudi::Units::pi : phi;
    phi = phi >  Gaudi::Units::pi ? phi - 2.0*Gaudi::Units::pi : phi;
    return phi; 
  }
  
  /// phi for a given inter strip fraction and strip in the ideal frame
  inline double idealPhi(unsigned int strip, double fraction) const {
    double phi = m_idealPhi[strip] + fraction*globalPhiPitch(strip);
    phi = phi < -Gaudi::Units::pi ? phi + 2.0*Gaudi::Units::pi : phi;
    phi = phi >  Gaudi::Units::pi ? phi - 2.0*Gaudi::Units::pi : phi;
    return phi; 
  }

  /// phi at a given radius in the ideal frame
  inline double idealPhi(unsigned int strip, double fraction, double radius) const {
    double c0     = strip < m_nbInner? m_idealOffsetAtR0[0] : m_idealOffsetAtR0[1];
    double d0     = strip < m_nbInner? m_idealDistToOrigin[0] : m_idealDistToOrigin[1];
    double offset =  safePhiOffset(c0,d0,radius);
    double phi    = m_idealPhi[strip] + fraction*globalPhiPitch(strip) + offset;
    phi = phi < -Gaudi::Units::pi ? phi + 2.0*Gaudi::Units::pi : phi;
    phi = phi >  Gaudi::Units::pi ? phi - 2.0*Gaudi::Units::pi : phi;
    return phi; 
  }
  
  /// The phi position of a strip at a given radius in the local frame
  inline double phiOfStrip(unsigned int strip, double fraction, 
                           const double radius) const {
    double effectiveStrip=fraction+static_cast<double>(strip);
    if (m_nbInner > strip) {
      return (effectiveStrip*m_innerPitch) + phiOffset(radius);
    } else {
      effectiveStrip -= m_nbInner;
      return (effectiveStrip*m_outerPitch) + phiOffset(radius);
    }
  }

  /// The angle of the strip wrt to the x axis in a rough global frame to mimic
  /// DeVelo v8r* and earlier verions
  inline double trgPhiDirectionOfStrip(unsigned int strip, double fraction=0.) const{
    return localPhiToGlobal(angleOfStrip(strip,fraction));
  }
  
  inline double phiTilt(unsigned int strip){
   if (m_nbInner > strip) {
      return m_innerTilt;
    } else {
      return m_outerTilt;
    } 
  }

  /// The phi pitch in mm at a given radius
  inline double phiPitch(const double radius) const {
    return (m_middleRadius > radius) ? m_innerPitch * radius :
      m_outerPitch * radius;
  }

  /// The phi pitch in radians for a given strip
  inline double phiPitch(unsigned int strip) const {
    return  (m_nbInner > strip) ? m_innerPitch : m_outerPitch;
  }

  /// The phi pitch in mm at a given radius, with the correct sign in the global frame
  inline double globalPhiPitch(double radius) const {
    return isDownstream() ? -phiPitch(radius): phiPitch(radius);
  }
  /// The phi pitch in radians for a given strip, with the correct sign in the global frame
  inline double globalPhiPitch(unsigned int strip) const {
    return isDownstream() ? -phiPitch(strip): phiPitch(strip);
  }

  /// Return the distance to the origin for sensor
  inline double distToOrigin(unsigned int strip) const {
    return (m_nbInner > strip) ? m_innerDistToOrigin : m_outerDistToOrigin;
  }

  /// Return the origin of the sensor in the global frame
  Gaudi::XYZPoint globalOrigin() const{
    Gaudi::XYZPoint localOrig(0,0,0);  
    return DeVeloSensor::localToGlobal(localOrig);
  }
    
  /// Access to the associated R sensor on the same module
  inline const DeVeloRType* associatedRSensor() const { return m_associatedRSensor; }
    
  /// Access to the phi sensor on the other side of the VELO
  inline const DeVeloPhiType* otherSidePhiSensor() const { return m_otherSidePhiSensor; }
  
  /// Access to the r sensor on the other side of the VELO
  inline const DeVeloRType* otherSideRSensor() const { return m_otherSideRSensor; }
  
  /// Set the associated R sensor.  This should only be called by DeVelo::initialize()
  inline void setAssociatedRSensor(const DeVeloRType* rs) { m_associatedRSensor = rs; }

  /// Set the phi sensor on the other side of the VELO.  This should only be called by DeVelo::initialize()
  inline void setOtherSidePhiSensor(const DeVeloPhiType* ps) { m_otherSidePhiSensor = ps; }

  /// Set the r sensor on the other side of the VELO.  This should only be called by DeVelo::initialize()
  inline void setOtherSideRSensor(const DeVeloRType* rs) { m_otherSideRSensor = rs; }

protected:

private:
  /// Returns the offset in phi for a given radius
  inline double phiOffset(double radius) const {
    if(m_middleRadius > radius){
      return -1.*safePhiOffset(m_innerTilt,m_innerDistToOrigin,radius);
    } else {
      return -1.*safePhiOffset(m_outerTilt,m_outerDistToOrigin,radius);
    }
  }

  /// Calculate the equation of the line for each strip
  void calcStripLines();
  /// Calculate the equation of line for the corner cut-offs
  void cornerLimits();
  
  /** Calculate the global and half box frame phi for the strip centres when the alignment changes.
   *  Also caches the phi for ideal alignment.
   */  
  StatusCode updatePhiCache();
  
  /// Calculate the zone limits in the global and halfbox frame
  StatusCode updateZoneLimits();
  
  /// Update the geometry cache when the alignment changes
  StatusCode updateGeometryCache();
  
  unsigned int m_nbInner;
  double m_middleRadius;
  double m_innerTilt;
  double m_innerDistToOrigin;
  double m_outerTilt;
  double m_outerDistToOrigin;
  double m_phiOrigin;
  double m_innerPitch;
  double m_outerPitch;
  double m_rGap;
  std::vector<unsigned int> m_stripsInZone;
  static std::vector<std::pair<double,double> > m_stripLines;

  /// First corner
  double m_corner1X1;
  double m_corner1Y1;
  double m_corner1X2;
  double m_corner1Y2;
  /// Second corner 
  double m_corner2X1;
  double m_corner2Y1;
  double m_corner2X2;
  double m_corner2Y2;

  std::pair<double,double> m_resolution;
  /// Define line for cutoffs (first is gradient, second is intercept)
  std::vector<std::pair<double,double> > m_cutOffs;
  //  static std::vector<std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> > m_stripLimits;
  double m_innerCoverage;
  double m_outerCoverage;
  //  double m_halfCoverage;
  //  bool m_down;

  /// cache of strip impact parameters for each zone in the ideal frame
  double m_idealDistToOrigin[2];
  
  /// cache of strip impact parameters for each zone in the halfbox frame
  double m_halfboxDistToOrigin[2];
  
  /// cache of strip impact parameters for each zone in the global frame
  double m_globalDistToOrigin[2];
  
  /// cache of offset for each zone with the correct sign for the ideal frame
  double m_idealOffsetAtR0[2];
  
  /// cache of offset for each zone with the correct sign for the halfbox frame
  double m_halfboxOffsetAtR0[2];
  
  /// cache of offset for each zone with the correct sign for the global frame
  double m_globalOffsetAtR0[2];
  
  /// cache of global phi at the strip centres
  std::vector<double> m_globalPhi;
    
  /// cache of phi in the halfbox frame at the strip centres
  std::vector<double> m_halfboxPhi;
    
  /// cache of ideal phi 
  std::vector<double> m_idealPhi;
    
  /// cache for phi range of zones in global frame
  std::pair<double,double> m_globalPhiLimitsZone[2];

  /// cache for phi range of zones in halfbox frame
  std::pair<double,double> m_halfboxPhiLimitsZone[2];
  
  /// cache for r range of zones in global frame
  std::pair<double,double> m_globalRLimitsZone[2];

  /// cache for r range of zones in halfbox frame
  std::pair<double,double> m_halfboxRLimitsZone[2];
  
  /// link to associated R sensor on the same module
  const DeVeloRType* m_associatedRSensor;
  
  /// pointer to the phi sensor on the other side of the VELO
  const DeVeloPhiType* m_otherSidePhiSensor;
  
  /// pointer to the r sensor on the other side of the VELO
  const DeVeloRType* m_otherSideRSensor;
  
  /// Pattern is based on sequence of six strips (outer, inner, outer, inner, outer, outer)
  void BuildRoutingLineMap();

  /// Store vector of strip lengths
  void calcStripLengths();

  /// Return the element in the pattern (0 to 5)
  unsigned int patternElement(unsigned int routLine){return ((routLine-1)%6);};
  /// Return number of times pattern has been repeated
  unsigned int patternNumber(unsigned int routLine){return ((routLine-1)/6);};
  /// Configuration for pattern (1st strip number in sequence, number of inner/outer strips in sequence)
  std::vector<std::pair<unsigned int,unsigned int> > m_patternConfig;
  /// Return strip number for given routing line
  unsigned int strip(unsigned int routLine){
    unsigned int patElem=patternElement(routLine);
    unsigned int patNum=patternNumber(routLine);
    return m_patternConfig[patElem].first+patNum*m_patternConfig[patElem].second;
  };

  // Set output level for message service
  bool m_debug;
  bool m_verbose;

  // These are references to local statics accessed via static functions
  // implemented in DeVeloRType.cpp. I stree this because these are
  // NOT ALLOWED TO BE INLINED!
  // Sematically, these data menber should be statics, but this does not work with
  // Windows(tm) DLLs in the CMT framework because they are accessed
  // by inlined accessors. So we have to live with this detour.
  // The staic bool m_staticDataInvalid is not (and never should be!)
  // accessed by any inline function. It's sole purpose is to speed
  // up the initialize() method when the information common to all sensors
  // is already up to date.
  std::vector<double>& m_stripLengths;

  // used to control initialization NEVER ACCESS THIS IN AN INLINED METHOD!
  static bool m_staticDataInvalid;

  /// calculate the phi offset for a strip at radius given the 
  /// d0 and the c0 of the strip: protected agains impossible radii values
  inline double safePhiOffset(double const &c0, double const &d0, 
			      double const & radius) const {
    return (std::abs(d0/radius) < 1.) ? (vdt::fast_asin(d0/radius) - c0) : (vdt::fast_asin(1.) - c0);
  }

  /// cached Message Stream object
  mutable MsgStream * m_msgStream;

  /// On demand access to MsgStream object
  inline MsgStream & msg() const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream( msgSvc(), "DeVeloPhiType" );
    return *m_msgStream; 
  }

};

/// fast cast to Phi sensor, returns 0 for wrong type
inline const DeVeloPhiType* DeVeloSensor::phiType() const { 
  return (m_isPhi ? static_cast<const DeVeloPhiType*>(this) : 0); 
}
  
#endif // VELODET_DEVELOPHITYPE_H
