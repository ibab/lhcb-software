// $Id: DeVeloPhiType.h,v 1.11 2006-01-26 14:58:43 krinnert Exp $
#ifndef VELODET_DEVELOPHITYPE_H 
#define VELODET_DEVELOPHITYPE_H 1

// Include files

// from Kernel
#include "Kernel/Point3DTypes.h"

/// from VeloDet
#include "VeloDet/DeVeloSensor.h"

/// From LHCb Kernel
#include "Kernel/VeloChannelID.h"


// Unique class identifier
static const CLID& CLID_DeVeloPhiType = 1008103 ;


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
  static  const CLID& classID() { return CLID_DeVeloPhiType; };
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVeloSensor from the XML
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                            LHCb::VeloChannelID& channel,
                            double& localOffset,
                            double& pitch) const;
  
  /// Get the nth nearest neighbour for a given channel
  StatusCode neighbour(const LHCb::VeloChannelID& start, 
                       const int& nOffset, 
                       LHCb::VeloChannelID& channel) const;

  /// Residual of 3-d point to a VeloChannelID
  StatusCode residual(const Gaudi::XYZPoint& point, 
                      const LHCb::VeloChannelID& channel,
                      double &residual,
                      double &chi2) const;



  /// Residual [see DeVelo for explanation]
  StatusCode residual(const Gaudi::XYZPoint& point,
                      const LHCb::VeloChannelID& channel,
                      const double localOffset,
                      const double width,
                      double &residual,
                      double &chi2) const;

  /// The number of zones in the detector
  inline unsigned int numberOfZones() const {return m_numberOfZones;}
  
  /// The zones number for a given strip
  inline unsigned int zoneOfStrip(const unsigned int strip) const {
    return (m_nbInner > strip) ? 0 : 1;
  }
  
  /// The number of strips in a zone
  inline unsigned int stripsInZone(const unsigned int zone) const {

    if(0 == zone) {
      return m_nbInner;
    } else if(1 == zone){
      return this->numberOfStrips()-m_nbInner;
    }
    return 0;
  }
  
  /// The minimum radius for a given zone of the sensor
  double rMin(const unsigned int zone) const;
  
  /// The maximum radius for a given zone of the sensor
  double rMax(const unsigned int zone) const;

  /// The minimum phi for a given zone of the sensor
  //  double phiMin(const unsigned int /*zone*/) {return 0.;}
  
  /// The maximum phi for a given zone of the sensor
  //  double phiMax(const unsigned int /*zone*/) {return 0.;}

  /// Determines if 3-d point is inside sensor
  StatusCode isInside(const Gaudi::XYZPoint& point) const;

  /// Determine if local point is in corner cut-offs
  bool isCutOff(double x, double y) const;

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
  
  /// The phi position of a strip at a given radius using method of DeVelo v8r*
  inline double trgPhiOfStrip(unsigned int strip, double fraction, 
                           const double radius){
    double phi=phiOfStrip(strip,fraction,radius);
    if(this->isDownstream()) phi = -phi;
    return phi;
  }
  
  /// The angle of the strip wrt to the x axis in the local frame
  inline double angleOfStrip(unsigned int strip, double fraction){
    double effectiveStrip=fraction+static_cast<double>(strip);
    if (m_nbInner > strip) {
      return (effectiveStrip*m_innerPitch) + m_innerTilt;
    } else {
      effectiveStrip -= m_nbInner;
      return (effectiveStrip*m_outerPitch) + m_outerTilt;
    }
  }

  /// The angle of the strip wrt to the x axis in a rough global frame to mimic
  /// DeVelo v8r* and earlier verions
  inline double trgPhiDirectionOfStrip(unsigned int strip, double fraction){
    return this->localPhiToGlobal(angleOfStrip(strip,fraction));
  }
  
  inline double phiTilt(unsigned int strip){
   if (m_nbInner > strip) {
      return m_innerTilt;
    } else {
      return m_outerTilt;
    } 
  }
    

  /// Returns the offset in phi for a given radius
  inline double phiOffset(double radius) const {
    if(m_middleRadius > radius){
      return m_innerTilt - asin(m_innerDistToOrigin / radius);
    } else {
      return m_outerTilt - asin(m_outerDistToOrigin / radius);
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

  /// Return the distance to the origin for sensor
  inline double distToOrigin(unsigned int strip) const {
    return (m_nbInner > strip) ? m_innerDistToOrigin : m_outerDistToOrigin;
  }

  /// Return the strip geometry for panoramix
  inline StatusCode stripLimits(unsigned int strip, Gaudi::XYZPoint& begin,
                                Gaudi::XYZPoint& end){
    StatusCode sc=this->localToGlobal(m_stripLimits[strip].first,begin);
    if(!sc) return sc;
    sc=this->localToGlobal(m_stripLimits[strip].second,end);
    return sc;
  }
 
protected:

private:
  /// Calculate the equation of the line for each strip
  void calcStripLines();
  /// Calculate the equation of line for the corner cut-offs
  void cornerLimits();
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
  std::vector<int> m_stripsInZone;
  std::vector<std::pair<double,double> > m_stripLines;
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
  std::vector<std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> > m_stripLimits;
  double m_innerCoverage;
  double m_outerCoverage;
  double m_halfCoverage;
  bool m_down;

  /// Pattern is based on sequence of six strips (outer, inner, outer, inner, outer, outer)
  void BuildRoutingLineMap();

  /// Return the element in the pattern (0 to 5)
  unsigned int patternElement(unsigned int routLine){return ((routLine-1)%6);};
  /// Return number of times pattern has been repeated
  unsigned int patternNumber(unsigned int routLine){return ((routLine-1)/6);};
  /// Configuration for pattern (1st strip number in sequence, number of inner/outer strips in sequence)
  std::vector<std::pair<unsigned int,unsigned int> > m_patternConfig;
  /// Return strip number for given routing line
  unsigned int strip(unsigned int routLine){
    unsigned int patElem=this->patternElement(routLine);
    unsigned int patternNumber=this->patternNumber(routLine);
    return m_patternConfig[patElem].first+patternNumber*m_patternConfig[patElem].second;
  };
};
#endif // VELODET_DEVELOPHITYPE_H
