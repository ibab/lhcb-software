// $Id: DeVeloSensor.h,v 1.2 2004-02-13 07:05:48 cattanem Exp $
#ifndef VELODET_DEVELOSENSOR_H 
#define VELODET_DEVELOSENSOR_H 1

// Include files
// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

/// from Det/DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

/// From LHCb Kernel
#include "Kernel/VeloChannelID.h"


// Unique class identifier
static const CLID& CLID_DeVeloSensor = 1008101 ;


/** @class DeVeloSensor DeVeloSensor.h VeloDet/DeVeloSensor.h
 *  
 *
 *  @author Mark Tobin
 *  @date   2003-01-14
 */
class DeVeloSensor : public DetectorElement {
public:
  /// Standard constructor
  DeVeloSensor( const std::string& name = "" ); 

  /// Destructor
  virtual ~DeVeloSensor( ); 

  /// Define a "less than" operator for DeVeloSensor objects.
  /// Uses the Z position of the sensor.
  //bool operator< (DeVeloSensor* rhs) 
  //  {
  //return ( this->z() < rhs->z() );
  //}

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVeloSensor; };
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVeloSensor from the XML
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  virtual StatusCode pointToChannel(const HepPoint3D& point,
                                    VeloChannelID& channel,
                                    double& localOffset,
                                    double& pitch)=0;
  
  /// Get the nth nearest neighbour for a given channel
  virtual StatusCode neighbour(const VeloChannelID& start, 
                               const int& nOffset, 
                               VeloChannelID& channel)=0;

  /// Returns the number of channels between two channels
  virtual StatusCode channelDistance(const VeloChannelID& start,
                                     const VeloChannelID& end,
                                     int& nOffset)=0;
  
  /// Residual of 3-d point to a VeloChannelID
  virtual StatusCode residual(const HepPoint3D& point, 
                              const VeloChannelID& channel,
                              double &residual,
                              double &chi2)=0;

  /// Residual [see DeVelo for explanation]
  virtual StatusCode residual(const HepPoint3D& point,
                              const VeloChannelID& channel,
                              const double localOffset,
                              const double width,
                              double &residual,
                              double &chi2)=0;
  
  /// The capacitance of the strip 
  virtual double stripCapacitance(unsigned int strip)=0;
  
  /// The number of zones in the detector
  virtual unsigned int numberOfZones()=0;
  
  /// The zones number for a given strip
  //  virtual unsigned int zoneOfStrip(const VeloChannelID& channel)=0;
  virtual unsigned int zoneOfStrip(const unsigned int strip)=0;
  
  /// The number of strips in a zone
  virtual unsigned int stripsInZone(const unsigned int zone)=0;

  /// The minimum radius of the sensitive area of a zone
  virtual double rMin(const unsigned int zone)=0;
  
  /// The maximum radius of the sensitive area of a zone
  virtual double rMax(const unsigned int zone)=0;

  /// The minimum phi for a given zone of the sensor
  virtual double phiMin(const unsigned int zone)=0;
  
  /// The maximum phi for a given zone of the sensor
  virtual double phiMax(const unsigned int zone)=0;
  
  /// Determine if point is in corner cut-offs
  virtual  StatusCode isInside(const HepPoint3D& point)=0;

  /// Determine if local point is in corner cut-offs
  virtual bool isCutOff(double x, double y)=0;

  /// Convert local position to global position
  /// Local from is +ve x (and Upstream for phi sensors)
  StatusCode localToGlobal(const HepPoint3D& local, 
                                   HepPoint3D& global);

  /// Convert global position to local position 
  StatusCode globalToLocal(const HepPoint3D& global, 
                                   HepPoint3D& local);

  /// Convert local phi to global phi
  double localPhiToGlobal(double phiLocal);

  /// Return the z position of the sensor
  double z();
  
  /// Return +1 for X>0 side of the detector (+ve x is Left/L)
  int xSide();
  
  /// Return true for X<0 side of the detector (-ve x is Right)
  bool isRight();
  
  /// Returns true if sensor is downstream
  bool isDownstream();

  /// Returns true if R Sensor
  bool isPileUp();

  /// Returns true if R Sensor
  bool isR();

  /// Returns true if R Sensor
  bool isPhi();

  /// Return the number of strips
  unsigned int numberOfStrips();

  /// The minimum radius for the sensitive area of the sensor
  double innerRadius();

  /// The maximum radius for the sensitive area of the sensor
  double outerRadius();
  
  /// The thickness of the sensor in mm
  double siliconThickness();
  
  /// Returns the sensor type
  std::string type();

  /// Set the message level to verbose
  void msgOutputLevel();

  /// Set the sensor number
  void sensorNumber(unsigned int sensor);

  /// Returns the sensor number
  unsigned int sensorNumber();
  
  /// Set the sensors associated with this sensor
  void associateSensor(unsigned int sensor);

  /// Return the (phi) sensors associated with this (R) sensor
  std::vector<unsigned int> associatedSensors();
  
protected:
  bool m_isRight,m_isLeft,m_isDownstream;
  bool m_isR,m_isPhi,m_isPileUp;
  
private:
  void initSensor();
  int m_xSide;
  unsigned int m_sensorNumber;
  std::vector<unsigned int> m_associated;
  unsigned int m_numberOfStrips;
  double m_innerRadius;
  double m_outerRadius;
  double m_siliconThickness;
  std::string m_type;
  double m_z;
  IGeometryInfo* m_geometry;
};
#endif // VELODET_DEVELOSENSOR_H
