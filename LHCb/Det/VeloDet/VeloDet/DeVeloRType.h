// $Id: DeVeloRType.h,v 1.31 2009-07-27 10:36:15 jonrob Exp $
#ifndef VELODET_DEVELORTYPE_H 
#define VELODET_DEVELORTYPE_H 1

// Include files
#include "vdt/exp.h"
#include "vdt/log.h"
#include "vdt/atan2.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Gaudi Math definitions
#include "GaudiKernel/Point3DTypes.h"

// from VeloDet
#include "VeloDet/DeVeloSensor.h"

// get trajectory
#include "Kernel/Trajectory.h"

// Unique class identifier
static const CLID CLID_DeVeloRType = 1008102 ;

// Forward declaration needed for link to associated sensor
class DeVeloPhiType;

namespace LHCb {
  class VeloChannelID;
}

/** @class DeVeloRType DeVeloRType.h VeloDet/DeVeloRType.h
 *  
 *
 *  @author Mark Tobin
 *  @author Kurt Rinnert kurt.rinnert@cern.ch
 *  @date   2003-01-14
 */
class DeVeloRType : public DeVeloSensor {
public:
  /// Standard constructor
  DeVeloRType( const std::string& name = "" ); 

  virtual ~DeVeloRType( ); ///< Destructor

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVeloRType; }
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVeloSensor from the XML
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  /// Also returns the fractional difference in the channel
  /// and the local pitch.
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VeloChannelID& channel,
                                    double& fraction,
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
  
  /// Residual of a 3-d point to a VeloChannelID + interstrip fraction
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                              const LHCb::VeloChannelID& channel,
                              const double interStripFraction,
                              double &residual,
                              double &chi2) const;

  /// The zones number for a given strip
  virtual unsigned int zoneOfStrip(const unsigned int strip) const {
    return static_cast<unsigned int>(strip/512);
  }

  /// The number of strips in a zone
  virtual unsigned int stripsInZone(const unsigned int /*zone*/) const{
    return m_stripsInZone;
  }

  /// The minimum radius for a given zone of the sensor
  virtual double rMin(const unsigned int /*zone*/) const {
    return innerRadius();
  }
  
  /// The maximum radius for a given zone of the sensor
  virtual double rMax(const unsigned int /*zone*/) const {return outerRadius();}

  /// Determines if local 3-d point is inside sensor
  virtual StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const;

  /// Determine if local point is in corner cut-offs
  virtual bool isCutOff(double x, double y) const;

  /// Return the length of a strip
  virtual double stripLength(const unsigned int strip) const;

  /// Zone for a given local phi
  unsigned int zoneOfPhi(double phi) const {
    unsigned int zone=0;
    if(m_phiMax[0] > phi){
      zone = 0;
    } else if(m_phiMin[1] <= phi && m_phiMax[1] > phi){
      zone = 1;
    } else if(m_phiMin[2] <= phi && m_phiMax[2] > phi) {
      zone = 2;
    } else if(m_phiMin[3] <= phi) {
      zone = 3;
    }
    return zone;
  }
  
  /// Minimum strip number for each zone
  inline unsigned int firstStrip(unsigned int zone) const {return zone*512;}

  /// Return the radius of the strip
  inline double rOfStrip(const unsigned int strip) const {
    return m_rStrips[strip];
  }

  /// Return the radius of the strip+fraction
  inline double rOfStrip(unsigned int strip, double fraction) const {
    return m_rStrips[strip]+fraction*rPitch(strip);
  }

  /// Return the local pitch of the sensor for a given strip
  inline double rPitch(unsigned int strip) const {return m_rPitch[strip];}

  /// Return the local pitch of the sensor for a given strip +/- fraction
  inline double rPitch(unsigned int strip, double fraction) const {
    return vdt::fast_exp(m_pitchSlope*((strip%512)+fraction))*m_innerPitch;
  }

  /// Return the local pitch at a given radius 
  inline double rPitch(double radius) const {  
    return m_innerPitch + m_pitchSlope*(radius - innerRadius());
  }

  /// Return the radius of the centre of strip plus interstrip fraction*pitch in the global frame
  inline double globalR(unsigned int strip, double isp) const {
    return  m_globalR[strip] + isp*rPitch(strip);
  }
  
  /// Return the radius of the centre of strip plus interstrip fraction*pitch in the halfbox frame
  inline double halfboxR(unsigned int strip, double isp) const {
    return m_halfboxR[strip] + isp*rPitch(strip);
  }
  
  /// Return the radius of the centre of the strip in the global frame
  inline double globalROfStrip(unsigned int strip) const { return m_globalR[strip]; }

  /// Return the radius of the centre of the strip in the global frame
  inline double halfboxROfStrip(unsigned int strip) const { return m_halfboxR[strip]; }

  /** The minimum phi for a zone assuming the radius is not known.  
      This is a constant value for zone 0 and should be used with caution **/
  inline double phiMinZone(unsigned int zone) const {return m_phiMin[zone];}

  /// Returns the minimum phi in a zone at given radius
  double phiMinZone(unsigned int zone, double radius) const;
  
  /// The maximum phi for a zone
  inline double phiMaxZone(unsigned int zone) const {return m_phiMax[zone];}
     
  /** The maximum phi for a zone assuming the radius is not known.  
      This is a constant value for zone 2 and should be used with caution **/
  double phiMaxZone(unsigned int zone, double radius) const;
   
  /// The phi range [-pi,pi] of the given global zone in the global frame.
  const std::pair<double,double>& globalPhiRange(unsigned int globalZone) const { return m_globalPhiLimitsZone[globalZone]; }
  
  /// The phi range [-pi,pi] of the given global zone in the halfbox frame.
  const std::pair<double,double>& halfboxPhiRange(unsigned int globalZone) const { return m_halfboxPhiLimitsZone[globalZone]; }
  
  /// The r range of the given global zone in the global frame.
  const std::pair<double,double>& globalRRange(unsigned int globalZone) const { return m_globalRLimitsZone[globalZone]; }
  
  /// The r range of the given global zone in the halfbox frame.
  const std::pair<double,double>& halfboxRRange(unsigned int globalZone) const { return m_halfboxRLimitsZone[globalZone]; }
  
  /// The minimum phi of a strip
  double phiMinStrip(unsigned int strip) const {
    return m_stripPhiLimits[strip].first;
  }
  
  /// The maximum phi of a strip
  double phiMaxStrip(unsigned int strip) const {
    return m_stripPhiLimits[strip].second; 
  }

  /// Return the strip limits for panoramix
  inline StatusCode stripLimits(unsigned int strip, double &radius,
                         double &phiMin, double &phiMax) const {
    radius = rOfStrip(strip);
    phiMin = phiMinStrip(strip);
    phiMax = phiMaxStrip(strip);
    return StatusCode::SUCCESS;
  }

  /// Access to the associated Phi sensor on the same module
  inline const DeVeloPhiType* associatedPhiSensor() const { return m_associatedPhiSensor; }
    
  /// Access to the r sensor on the other side of the VELO
  inline const DeVeloRType* otherSideRSensor() const { return m_otherSideRSensor; }
  
  /// Access to the phi sensor on the other side of the VELO
  inline const DeVeloPhiType* otherSidePhiSensor() const { return m_otherSidePhiSensor; }
  
  /// Set the associated phi sensor.  This should only be called by DeVelo::initialize()
  inline void setAssociatedPhiSensor(const DeVeloPhiType* ps) { m_associatedPhiSensor = ps; }
  
  /// Set the r sensor on the other side of the VELO.  This should only be called by DeVelo::initialize()
  inline void setOtherSideRSensor(const DeVeloRType* rs) { m_otherSideRSensor = rs; }

  /// Set the phi sensor on the other side of the VELO.  This should only be called by DeVelo::initialize()
  inline void setOtherSidePhiSensor(const DeVeloPhiType* ps) { m_otherSidePhiSensor = ps; }

  /** get the distance of a 3D point (global frame) to the closest M2
      line in mm and dist to nearest strip in mm. vID is the channel
      of the line connected to the second metal routing line (not
      channel under hit)
      @param point : 3D (global frame) position considered
      @param vID : VeloChannelID of inner strip of M2 line (if any)
      @param distToM2Line : distance of this point to M2 line (if <200microns only) in mm
      @param distToStrip : distance to the closest strip (the one under this point) in mm
      @return : Success if there was an M2 line near point, Failure if not 
  */
  StatusCode distToM2Line(const Gaudi::XYZPoint& point, 
                          LHCb::VeloChannelID &vID, 
                          double & distToM2Line,
                          double & distToStrip) const;

private:
  /** get the distance of x,y in the local frame to the closest M2
      line and strip of that line returns false if nothing matches,
      strip is the routing line strip number and dist is the distance
      from the point to the line in mm */
  bool distToM2Line(double const & x, double const & y, 
		   LHCb::VeloChannelID &vID, double & dist) const;

  /// Store the local radius for each strip in the sensor
  void calcStripLimits();

  /// Store the angular limits of phi zones
  void phiZoneLimits();

  /// Store the co-ordinates of the cut-offs
  void cornerLimits();

  /// Return x and y position for the intersect of the cut-off line and a given radius
  void intersectCutOff(const double radius, double& x, double& y) const;

  /// Calculate the global and half box strip radii when the alignment changes
  StatusCode updateStripRCache();

  /// Calculate the zone limits in the global and halfbox frame
  StatusCode updateZoneLimits();
  
  /// Update geomtry cache when the alignment changes
  StatusCode updateGeometryCache();
    
private:

  //  unsigned int m_numberOfZones;
  unsigned int m_stripsInZone;
  double m_cornerX1;
  double m_cornerY1;
  double m_cornerX2;
  double m_cornerY2;  
  double m_cornerXInt;//<X intercept for corner cut off
  double m_cornerYInt;//<Y intercept for corner cut off
  double m_gradCutOff;//<Gradient of line defining cut offs
  double m_intCutOff;//<x intercept for line which defines cut offs
  std::pair<double,double> m_resolution;//Resolution from LHCB??????
  double m_innerPitch;
  double m_outerPitch;
  double m_innerR;//< Radius of first strip
  double m_outerR;//< Radius of outer strip
  double m_overlapInX;
  double m_pitchSlope;
  double m_halfAngle;
  double m_quarterAngle;
  double m_phiGap;

  /// cache for global strip radii
  std::vector<double> m_globalR;

  /// cache for strip radii in the halfbox frame
  std::vector<double> m_halfboxR;

  /// cache for phi range of zones in global frame
  std::pair<double,double> m_globalPhiLimitsZone[4];

  /// cache for phi range of zones in halfbox frame
  std::pair<double,double> m_halfboxPhiLimitsZone[4];
  
  /// cache for r range of zones in global frame
  std::pair<double,double> m_globalRLimitsZone[4];

  /// cache for r range of zones in halfbox frame
  std::pair<double,double> m_halfboxRLimitsZone[4];
  
  /// pointer to associated phi sensor
  const DeVeloPhiType* m_associatedPhiSensor;
  
  /// pointer to the r sensor on the other side of the VELO
  const DeVeloRType* m_otherSideRSensor;
  
  /// pointer to the phi sensor on the other side of the VELO
  const DeVeloPhiType* m_otherSidePhiSensor;
  
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
  std::vector<double>& m_rStrips;
  std::vector<double>& m_rPitch;
  std::vector<double>& m_phiMin;
  std::vector<double>& m_phiMax;
  std::vector< std::pair<double,double> >& m_stripPhiLimits;//Min/Max phi of strips

  // stuff to hold the M2 lines has to be public for the static 
public:
  /// small struct to hold a set of x,y points defining a connected line
  struct PolyLine{    
    std::vector<double> m_x;
    std::vector<double> m_y;
    double m_maxPhi;
    PolyLine(std::vector<double> x, std::vector<double> y): m_x(x), m_y(y),
                                                            m_maxPhi(-999.){
      for( unsigned int i = 0 ; i < m_x.size() ; ++i ){
        double phi = vdt::fast_atan2(m_y[i],m_x[i]);
        if( phi > m_maxPhi ) m_maxPhi = phi;
      }
    };
    double minPhi() const{
      double minPhi(999.);
      for( unsigned int i = 0 ; i < m_x.size() ; ++i ){
        double phi = vdt::fast_atan2(m_y[i],m_x[i]);
        if( phi < minPhi ) minPhi = phi;
      }
      return minPhi;
    }    
  };
private:
  /// The lines defining the 2nd metal routing on the sensor 
  /// the index is the readout strip number [0 to 2047]
  /// actually refers to a static 
  std::vector<DeVeloRType::PolyLine>& m_M2RoutingLines;
  /// minPhi of the strips sorted by phi: actually refs a static
  std::vector<std::pair<double,unsigned int> >& m_M2RLMinPhi;

  // used to control initialization NEVER ACCESS THIS IN AN INLINED METHOD!
  static bool m_staticDataInvalid;

  /// load the 2nd metal routing line map from the conditions
  void loadM2RoutingLines();

  /// Build up map of strip to routing line conversions
  void BuildRoutingLineMap();
  // Reverse middle group of four strips (0123->0213)
  unsigned int ScrambleStrip(unsigned int strip){
    if(1 == strip%4){
      strip++;
    } else if(2 == strip%4) {
      strip--;
    }
    return strip;
  }
  /// Select routing line area
  /// (0) Chips 15 to 13, (1) Chip 12, (2) Chip 11 and (3) Chips 10 to 8
  /// Pattern repeats for chips 7 to 0
  unsigned int RoutingLineArea(unsigned int routingLine);
  /// Return strip from routing line area
  unsigned int RoutLineToStrip(unsigned int routLine, unsigned int routArea);
  /// Number of strips in each readout area
  static const unsigned int m_nChan0=384;
  static const unsigned int m_nChan1=128;
  static const unsigned int m_nChan2=128;
  static const unsigned int m_nChan3=384;

  // Set output level for message service
  bool m_debug;
  bool m_verbose;

  /// cached Message Stream object
  mutable MsgStream * m_msgStream;

  /// On demand access to MsgStream object
  inline MsgStream & msg() const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream( msgSvc(), "DeVeloRType" );
    return *m_msgStream; 
  }

};

/// fast cast to R sensor, returns 0 for wrong type
inline const DeVeloRType* DeVeloSensor::rType() const { 
  return (m_isR || m_isPileUp ? static_cast<const DeVeloRType*>(this) : 0); 
}


#endif // VELODET_DEVELORTYPE_H
