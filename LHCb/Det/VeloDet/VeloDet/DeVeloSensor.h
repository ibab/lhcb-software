// $Id: DeVeloSensor.h,v 1.7 2005-06-02 14:11:41 jpalac Exp $
#ifndef VELODET_DEVELOSENSOR_H 
#define VELODET_DEVELOSENSOR_H 1

// Include files
// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

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
                                    double& pitch) const = 0;
  
  /// Get the nth nearest neighbour for a given channel
  virtual StatusCode neighbour(const VeloChannelID& start, 
                               const int& nOffset, 
                               VeloChannelID& channel) const = 0;

  /// Returns the number of channels between two channels
  virtual StatusCode channelDistance(const VeloChannelID& start,
                                     const VeloChannelID& end,
                                     int& nOffset) const;
  
  /// Residual of 3-d point to a VeloChannelID
  virtual StatusCode residual(const HepPoint3D& point, 
                              const VeloChannelID& channel,
                              double &residual,
                              double &chi2) const = 0;

  /// Residual [see DeVelo for explanation]
  virtual StatusCode residual(const HepPoint3D& point,
                              const VeloChannelID& channel,
                              const double localOffset,
                              const double width,
                              double &residual,
                              double &chi2) const = 0;
  
  /// The capacitance of the strip 
  virtual double stripCapacitance(unsigned int strip) const = 0;
  
  /// The zones number for a given strip
  //  virtual unsigned int zoneOfStrip(const VeloChannelID& channel)=0;
  virtual unsigned int zoneOfStrip(const unsigned int strip) const = 0;
  
  /// The number of strips in a zone
  virtual unsigned int stripsInZone(const unsigned int zone) const = 0;

  /// The minimum radius of the sensitive area of a zone
  virtual double rMin(const unsigned int zone) const = 0;
  
  /// The maximum radius of the sensitive area of a zone
  virtual double rMax(const unsigned int zone) const = 0;

  /// The minimum phi for a given zone of the sensor
  //  virtual double phiMin(const unsigned int zone)=0;
  
  /// The maximum phi for a given zone of the sensor
  //  virtual double phiMax(const unsigned int zone)=0;
  
  /// Determine if point is in corner cut-offs
  virtual  StatusCode isInside(const HepPoint3D& point) const = 0;

  /// Determine if local point is in corner cut-offs
  virtual bool isCutOff(double x, double y) const = 0;

  /// Convert local position to global position
  /// Local from is +ve x (and Upstream for phi sensors)
  StatusCode localToGlobal(const HepPoint3D& local, 
                           HepPoint3D& global) const;

  /// Convert global position to local position 
  StatusCode globalToLocal(const HepPoint3D& global, 
                           HepPoint3D& local) const;

  /// Convert local phi to rough global phi
  inline double localPhiToGlobal(double phiLocal) const {
    if(this->isDownstream()) phiLocal = -phiLocal;
    if(this->isRight()) phiLocal += pi;
    return phiLocal;
  } 

  /// Return the z position of the sensor
  inline double z() const {return m_z;}
  
  /// Return +1 for X>0 side of the detector (+ve x is Left/L)
  inline int xSide() const {return m_xSide;}
  
  /// Return true for X<0 side of the detector (-ve x is Right)
  inline bool isRight() const {return !m_isLeft;}
  
  /// Returns true if sensor is downstream
  inline bool isDownstream() const {return m_isDownstream;}

  /// Returns true if R Sensor
  inline bool isPileUp() const {return m_isPileUp;}  

  /// Returns true if R Sensor
  inline bool isR() const {return m_isR;}

  /// Returns true if R Sensor
  inline bool isPhi() const {return m_isPhi;}

  /// Return the number of strips
  inline unsigned int numberOfStrips() const {return m_numberOfStrips;}

  /// The number of zones in the detector
  inline unsigned int numberOfZones() const {return m_numberOfZones;}
  
  /// The minimum radius for the sensitive area of the sensor
  inline double innerRadius() const {return m_innerRadius;}

  /// The maximum radius for the sensitive area of the sensor
  inline double outerRadius() const {return m_outerRadius;}
  
  /// The thickness of the sensor in mm
  inline double siliconThickness() const {return m_siliconThickness;}
  
  /// Returns the sensor type
  inline std::string type() const {return m_type;}
  inline std::string fullType() const {return m_fullType;}

  /// Set the sensor number
  //void sensorNumber(unsigned int sensor);
  inline void sensorNumber(unsigned int sensor) {m_sensorNumber=sensor;}

  /// Returns the sensor number
  inline unsigned int sensorNumber() const {return m_sensorNumber;}
  
  /// Set the sensors associated with this sensor
  inline void associateSensor(unsigned int sensor) {
    m_associated.push_back(sensor);
  }

  /// Return the (phi) sensors associated with this (R) sensor
  inline std::vector<unsigned int> associatedSensors() const {
    return m_associated;
  }
  
protected:




  unsigned int m_numberOfZones;

private:

  void initSensor();

  unsigned int m_numberOfStrips;
  std::string m_type;
  std::string m_fullType;
  bool m_isLeft;
  bool m_isDownstream;
  bool m_isR;
  bool m_isPhi;
  bool m_isPileUp;
  int m_xSide;
  unsigned int m_sensorNumber;
  std::vector<unsigned int> m_associated;
  double m_siliconThickness;
  double m_z;
  double m_innerRadius;
  double m_outerRadius;

  IGeometryInfo* m_geometry;
};
#endif // VELODET_DEVELOSENSOR_H
