// $Id: DeVeloRType.h,v 1.6 2004-03-01 14:33:40 mtobin Exp $
#ifndef VELODET_DEVELORTYPE_H 
#define VELODET_DEVELORTYPE_H 1

// Include files

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

/// from VeloDet
#include "VeloDet/DeVeloSensor.h"

/// From LHCb Kernel
#include "Kernel/VeloChannelID.h"


// Unique class identifier
static const CLID& CLID_DeVeloRType = 1008102 ;


/** @class DeVeloRType DeVeloRType.h VeloDet/DeVeloRType.h
 *  
 *
 *  @author Mark Tobin
 *  @date   2003-01-14
 */
class DeVeloRType : public DeVeloSensor {
public:
  /// Standard constructor
  DeVeloRType( const std::string& name = "" ); 

  virtual ~DeVeloRType( ); ///< Destructor

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVeloRType; };
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVeloSensor from the XML
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  /// Also returns the fractional difference in the channel
  /// and the local pitch.
  StatusCode pointToChannel(const HepPoint3D& point,
                                    VeloChannelID& channel,
                                    double& fraction,
                                    double& pitch);
  
  /// Get the nth nearest neighbour for a given channel
  StatusCode neighbour(const VeloChannelID& start, 
                               const int& nOffset, 
                               VeloChannelID& channel);

  /// Returns the number of channels between two channels
  StatusCode channelDistance(const VeloChannelID& start,
                             const VeloChannelID& end,
                             int& nOffset);

  /// Residual of 3-d point to a VeloChannelID
  StatusCode residual(const HepPoint3D& point, 
                              const VeloChannelID& channel,
                              double &residual,
                              double &chi2);
  
  /// Residual [see DeVelo for explanation]
   StatusCode residual(const HepPoint3D& point,
                      const VeloChannelID& channel,
                      const double localOffset,
                      const double width,
                      double &residual,
                      double &chi2);
  /// The capacitance of the strip 
  double stripCapacitance(unsigned int strip);
  
  /// The number of zones in the detector
  //  inline unsigned int numberOfZones(){return m_numberOfZones;}
 
  /// The zones number for a given strip
  inline unsigned int zoneOfStrip(const unsigned int strip){
    return static_cast<unsigned int>(strip/512);
  }

  /// The number of strips in a zone
  inline unsigned int stripsInZone(const unsigned int /*zone*/){
    return m_stripsInZone;
  }

  /// The minimum radius for a given zone of the sensor
  double rMin(const unsigned int /*zone*/) {return m_innerRadius;}
  
  /// The maximum radius for a given zone of the sensor
  double rMax(const unsigned int /*zone*/) {return m_outerRadius;}

  /// Determines if local 3-d point is inside sensor
  StatusCode isInside(const HepPoint3D& point);

  /// Determine if local point is in corner cut-offs
  bool isCutOff(double x, double y);

  /// Zone for a given local phi
  inline unsigned int zoneOfPhi(double phi){
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
  inline unsigned int firstStrip(unsigned int zone){return zone*512;}

  /// Return the radius of the strip
  inline double rOfStrip(const unsigned int strip){return m_rStrips[strip];}

  /// Return the radius of the strip+fraction
  inline double rOfStrip(unsigned int strip, double fraction){
    return m_rStrips[strip]+fraction*rPitch(strip);
  }

  /// Return the local pitch of the sensor for a given strip
  inline double rPitch(unsigned int strip){return m_rPitch[strip];}

  /// Return the local pitch of the sensor for a given strip +/- fraction
  inline double rPitch(unsigned int strip, double fraction){
    return exp(fraction)*m_rPitch[strip];
  }

  /// Return the local pitch at a given radius 
  inline double rPitch(double radius) {  
    return m_innerPitch + m_pitchSlope*(radius - m_innerRadius);
  }

  /// The minimum phi for a zone
  inline double phiMinZone(unsigned int zone){return m_phiMin[zone];}

  /// Returns the minimum phi in a zone at given radius
  inline double phiMinZone(unsigned int zone, double radius){
    double phiMin;
    if(0 == zone){
      phiMin = -acos(m_overlapInX/radius);
    } else if(2 == zone){
      phiMin = asin(m_phiGap/radius);
    } else {
      phiMin = this->phiMinZone(zone);
    }
    return phiMin;
  }
  
  /// The maximum phi for a zone
  inline double phiMaxZone(unsigned int zone){return m_phiMax[zone];}
     
  /// Returns the maximum phi in a zone at given radius
  inline double phiMaxZone(unsigned int zone, double radius){
    double phiMax;
    if(1 == zone){
      phiMax = asin(-m_phiGap/radius);
    } else if(3 == zone){
      phiMax = acos(m_overlapInX/radius);
    } else {
      phiMax = this->phiMaxZone(zone);
    }
    return phiMax;
  }
   
  /// The minimum phi of a strip
  double phiMinStrip(unsigned int strip){return m_stripLimits[strip].first;}
  
  /// The maximum phi of a strip
  double phiMaxStrip(unsigned int strip){return m_stripLimits[strip].second;}

  /// Return the strip limits for panoramix
  inline StatusCode stripLimits(unsigned int strip, double &radius,
                         double &phiMin, double &phiMax){
    radius = rOfStrip(strip);
    phiMin = phiMinStrip(strip);
    phiMax = phiMaxStrip(strip);
    return StatusCode::SUCCESS;
  }
  
  
protected:

private:
  /// Store the local radius for each strip in the sensor
  void calcStripLimits();

  /// Store the angular limits of phi zones
  void phiZoneLimits();

  /// Store the co-ordinates of the cut-offs
  void cornerLimits();

  //  unsigned int m_numberOfStrips;
  //  unsigned int m_numberOfZones;
  unsigned int m_stripsInZone;
  std::vector<double> m_rStrips;
  std::vector<double> m_rPitch;
  std::vector<double> m_phiMin;
  std::vector<double> m_phiMax;
  double m_cornerX1;
  double m_cornerY1;
  double m_cornerX2;
  double m_cornerY2;  
  std::vector<double> m_corners;// Equation of line for cutoffs
  std::vector< std::pair<double,double> > m_stripLimits;//Min/Max phi of strips
  std::pair<double,double> m_resolution;//Resolution from LHCB??????
  double m_innerPitch;
  double m_outerPitch;
  //  double m_innerRadius;
  //  double m_outerRadius;
  double m_innerR;//< Radius of first strip
  double m_outerR;//< Radius of outer strip
  double m_overlapInX;
  double m_pitchSlope;
  double m_halfAngle;
  double m_quarterAngle;
  double m_phiGap;
};
#endif // VELODET_DEVELORTYPE_H
