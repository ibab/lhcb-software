// $Id: DeVeloRType.h,v 1.1 2004-02-03 16:54:10 mtobin Exp $
#ifndef VELODET_DEVELORTYPE_H 
#define VELODET_DEVELORTYPE_H 1

// Include files
// from Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

/// from Det/DetDesc
#include "DetDesc/DetectorElement.h"
#include "VeloDet/DeVeloSensor.h"

/// From LHCb Kernel
#include "Kernel/VeloChannelID.h"
#include "VeloKernel/VeloRound.h"

/// Declarations for math functions.
#include <cmath>


// Forward declaration
class StreamBuffer;

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
  unsigned int numberOfZones();
  
  /// The zones number for a given strip
  unsigned int zoneOfStrip(const unsigned int strip);
  //  unsigned int zoneOfStrip(const VeloChannelID& channel);

  /// The number of strips in a zone
  unsigned int stripsInZone(const unsigned int zone);

  /// The minimum radius for a given zone of the sensor
  double rMin(const unsigned int /*zone*/) {return 0.;}
  
  /// The maximum radius for a given zone of the sensor
  double rMax(const unsigned int /*zone*/) {return 0.;}

  /// The minimum phi for a given zone of the sensor
  double phiMin(const unsigned int /*zone*/) {return 0;}
  
  /// The maximum phi for a given zone of the sensor
  double phiMax(const unsigned int /*zone*/) {return 0;}

  /// Determines if 3-d point is inside sensor
  StatusCode isInside(const HepPoint3D& point);

  /// Determine if point is in corner cut-offs
  bool isCutOff(double x, double y);

  /// Zone for a given local phi
  unsigned int zoneOfPhi(double phi);
  
  /// Minimum strip number for each zone
  unsigned int firstStrip(unsigned int zone);

  /// Return the radius of the strip
  double rOfStrip(const unsigned int strip);

  /// Return the radius of the strip+fraction
  double rOfStrip(unsigned int strip, double fraction);
  
  /// Return the local pitch of the sensor for a given strip
  double rPitch(unsigned int strip);

  /// Return the local pitch of the sensor for a given strip +/- fraction
  double rPitch(unsigned int strip, double fraction);

  /// Return the local pitch at a given radius 
  double rPitch(double radius);

  /// The minimum phi for a zone
  double phiMinZone(unsigned int zone);

  /// Returns the minimum phi in a zone at given radius
  double phiMinZone(unsigned int zone, double radius);
  
  /// The maximum phi for a zone
  double phiMaxZone(unsigned int zone);
   
  /// Returns the maximum phi in a zone at given radius
  double phiMaxZone(unsigned int zone, double radius);
  
  /// The minimum phi of a strip
  double phiMinStrip(unsigned int strip);
  
  /// The maximum phi of a strip
  double phiMaxStrip(unsigned int strip);

  /// The capacitance of the strip for a given channel
  //  double stripCapacitance(const VeloChannelID& channel);
  
  /// Return the strip limits for panoramix
  StatusCode stripLimits(unsigned int strip, double &radius,
                         double &phiMin, double &phiMax);
  
protected:

private:
  /// Store the local radius for each strip in the sensor
  void calcStripLimits();

  /// Store the angular limits of phi zones
  void phiZoneLimits();

  /// Store the co-ordinates of the cut-offs
  void cornerLimits();

  unsigned int m_numberOfStrips;
  unsigned int m_numberOfZones;
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
  double m_innerRadius;
  double m_outerRadius;
  double m_innerActiveArea;
  double m_outerActiveArea;
  double m_overlapInX;
  double m_pitchSlope;
  double m_halfAngle;
  double m_quarterAngle;
  double m_phiGap;
};
#endif // VELODET_DEVELORTYPE_H
