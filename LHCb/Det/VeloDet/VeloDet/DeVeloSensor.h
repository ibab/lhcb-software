// $Id: DeVeloSensor.h,v 1.12 2006-01-26 14:58:43 krinnert Exp $
#ifndef VELODET_DEVELOSENSOR_H 
#define VELODET_DEVELOSENSOR_H 1

// Include files
#include <algorithm>
#include <bitset>

// from Kernel
#include "Kernel/Point3DTypes.h"
#include "Kernel/PhysicalConstants.h"

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
 *  @author Kurt Rinnert kurt.rinnert@cern.ch
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
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VeloChannelID& channel,
                                    double& localOffset,
                                    double& pitch) const = 0;
  
  /// Get the nth nearest neighbour for a given channel
  virtual StatusCode neighbour(const LHCb::VeloChannelID& start, 
                               const int& nOffset, 
                               LHCb::VeloChannelID& channel) const = 0;

  /// Returns the number of channels between two channels
  virtual StatusCode channelDistance(const LHCb::VeloChannelID& start,
                                     const LHCb::VeloChannelID& end,
                                     int& nOffset) const;
  
  /// Residual of 3-d point to a VeloChannelID
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                              const LHCb::VeloChannelID& channel,
                              double &residual,
                              double &chi2) const = 0;

  /// Residual [see DeVelo for explanation]
  virtual StatusCode residual(const Gaudi::XYZPoint& point,
                              const LHCb::VeloChannelID& channel,
                              const double localOffset,
                              const double width,
                              double &residual,
                              double &chi2) const = 0;
  
  /// The zones number for a given strip
  //  virtual unsigned int zoneOfStrip(const LHCb::VeloChannelID& channel)=0;
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
  virtual  StatusCode isInside(const Gaudi::XYZPoint& point) const = 0;

  /// Determine if local point is in corner cut-offs
  virtual bool isCutOff(double x, double y) const = 0;

  /// Convert local position to global position
  /// Local from is +ve x (and Upstream for phi sensors)
  StatusCode localToGlobal(const Gaudi::XYZPoint& local, 
                           Gaudi::XYZPoint& global) const;

  /// Convert global position to local position 
  StatusCode globalToLocal(const Gaudi::XYZPoint& global, 
                           Gaudi::XYZPoint& local) const;

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
  
  /// Cache the geometry information after changes to position from alignment
  void cacheGeometry();

  /// Convert routing line to chip channel (1234 -> 0213)
  unsigned int RoutingLineToChipChannel(unsigned int routLine){
    routLine = (2048-routLine);
    if(1 == routLine%4){
      routLine++;
    }else if(2 == routLine%4){
      routLine--;
    } 
    return routLine;
  }
  /// Convert chip channel to routing line (0213 -> 1234)
  unsigned int ChipChannelToRoutingLine(unsigned int chipChan){
    if(1 == chipChan%4){
      chipChan++;
    } else if(2 == chipChan%4) {
      chipChan--;
    }
    return (2048-chipChan);
  }  
  /// Convert chip channel to strip number
  unsigned int ChipChannelToStrip(unsigned int chipChan){
    return RoutingLineToStrip(ChipChannelToRoutingLine(chipChan));
  };
  /// Convert strip number to chip channel
  unsigned int StripToChipChannel(unsigned int strip){
    return RoutingLineToChipChannel(StripToRoutingLine(strip));
  };

  /// Convert routing line to strip number
  unsigned int RoutingLineToStrip(unsigned int routLine){return m_mapRoutingLineToStrip[routLine];};
  /// Convert strip number to routing line
  unsigned int StripToRoutingLine(unsigned int strip){return m_mapStripToRoutingLine[strip];};

  /// Get the chip number from the routing line
  unsigned int ChipFromRoutingLine(unsigned int routLine){return ChipFromChipChannel(RoutingLineToChipChannel(routLine));};
  /// Get the chip number from the chip channel
  unsigned int ChipFromChipChannel(unsigned int chipChan){return static_cast<int>(chipChan/128);};
  /// Get the Chip from the Velo ChannelID
  unsigned int ChipFromStrip(unsigned int strip){return ChipFromChipChannel(StripToChipChannel(strip));};

  /// Return the validity of a strip
  bool OKStrip(unsigned int strip){return (strip<m_numberOfStrips && !m_badStrips[strip]);};
  
  /// Returns the validity of a given channel
  bool OKChipChannel(unsigned int chipChan){
    return (chipChan<m_numberOfStrips && OKStrip(ChipChannelToStrip(chipChan)));
  };

  // condition related forwar daclarations

  class StripInfo;

  // condition related protected types

protected:
  /** @class DeVeloSensor::ConvertIntToStripInfo DeVeloSensor.h VeloDet/DeVeloSensor.h
   *
   *  Conversion Functor 
   *
   *  Converts integer from CondDB to the type used in the strip info
   *  condition cache.
   *
   *  @author Kurt Rinnert
   *  @date   2006-01-18
   */
  struct ConvertIntToStripInfo : public std::unary_function<int, StripInfo>
  {
    StripInfo operator() (int i);
  };

  // condition related public types

public:
  /** @class DeVeloSensor::StripInfo DeVeloSensor.h VeloDet/DeVeloSensor.h
   *
   *  encodes strip information 
   *
   *  The internal bit encoding is as follows.
   *
   *  bit 0: 0 if strip is read out, 1 if not;
   *  bit 1: 0 if strip is bonded, 1 if not;
   *  bit 2: 1 if strip is bonded with next strip, 0 if not;
   *  bit 3: 1 if strip is bonded with previous strip, 0 if not
   *
   *  @author Kurt Rinnert
   *  @date   2006-01-18
   */
  class StripInfo
  {
  public:
    StripInfo() : m_info(0x3) { ; }

  private:
    enum { 
      NOT_READ_OUT         = 0,
      NOT_BONDED           = 1,
      BONDED_WITH_NEXT     = 2,
      BONDED_WITH_PREVIOUS = 3
    };

  public:
    bool stripIsReadOut() const { return !m_info[NOT_READ_OUT]; }
    bool stripIsBonded() const { return !m_info[NOT_BONDED]; }
    bool stripIsBondedWithNext() const { return m_info[BONDED_WITH_NEXT]; }
    bool stripIsBondedWithPrevious() const { return m_info[BONDED_WITH_PREVIOUS]; }

    int asInt() const;

  private:
    std::bitset<4> m_info; 

  private:
    friend class DeVeloSensor::ConvertIntToStripInfo;
  };

  // condition related public methods

public:
  /** Check whether this sensor is read out at all (cached condition).
   *  This information is based on CondDB, i.e. it can change
   *  with time.
   */
  bool isReadOut() const { return m_isReadOut; }

  /** The capacitance of a strip (cached condition).
   *  This information is based on CondDB, i.e. it can change
   *  with time.
   */
  double stripCapacitance(unsigned int strip) const;
  
  /** Get info for this strip (cached condition).
   *  This information is based on CondDB, i.e. it can change
   *  with time.
   *  @see StripInfo
   */
  StripInfo stripInfo(unsigned int strip) const;

  /// call back function for strip capacitance condition update
  StatusCode updateStripCapacitanceCondition();

  /// call back function for strip info condition update
  StatusCode updateStripInfoCondition();

  /// call back function for readout condition update
  StatusCode updateReadoutCondition();

  /** direct access to strip capacitance condition.  
   *  This is for expert/testing purposes only.  All production 
   *  client code should use the interface to the cached conditions.  
   */
  const Condition* stripCapacitanceCondition() const { return m_stripCapacitanceCondition; }

  /** direct access to strip info condition.  
   *  This is for expert/testing purposes only.  All production 
   *  client code should use the interface to the cached conditions.  
   */
  const Condition* stripInfoCondition() const { return m_stripInfoCondition; }

  /** direct access to sensor readout condition.  
   *  This is for expert/testing purposes only.  All production 
   *  client code should use the interface to the cached conditions.  
   */
  const Condition* readoutCondition() const { return m_readoutCondition; }

  // condition related private methods

private:
  /** registers condition call backs
   *  This has only to be done once.  Method is called once
   *  from initialize().
   */
  StatusCode registerConditionCallBacks();

protected:

  unsigned int m_numberOfZones;
  static const unsigned int m_minRoutingLine=1;
  static const unsigned int m_maxRoutingLine=2048;
  std::map<unsigned int, unsigned int> m_mapRoutingLineToStrip;//<Map of routing line to strips
  std::map<unsigned int, unsigned int> m_mapStripToRoutingLine;//<Map of strips to routing line
  
private:

  void initSensor();

  static const unsigned int m_numberOfStrips=2048;
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
  std::map<unsigned int,bool> m_badStrips;//<Map of all known bad strips

  IGeometryInfo* m_geometry;

  // condition cache
  std::string m_stripCapacitanceConditionName;
  std::string m_stripInfoConditionName;
  std::string m_readoutConditionName;
  
  const Condition* m_stripCapacitanceCondition;
  const Condition* m_stripInfoCondition;
  const Condition* m_readoutCondition;

  std::vector<double> m_stripCapacitance;
  std::vector<StripInfo> m_stripInfos;
  bool m_isReadOut;
};
#endif // VELODET_DEVELOSENSOR_H
