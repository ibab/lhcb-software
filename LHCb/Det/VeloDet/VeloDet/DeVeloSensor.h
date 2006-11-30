// $Id: DeVeloSensor.h,v 1.28 2006-11-30 07:57:41 cattanem Exp $
#ifndef VELODET_DEVELOSENSOR_H 
#define VELODET_DEVELOSENSOR_H 1

// Include files
#include <algorithm>
#include <bitset>

// from Kernel
#include "Kernel/Point3DTypes.h"

/// from Det/DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

/// From LHCb Kernel
#include "Kernel/VeloChannelID.h"

// get LHCbID for trajectory
#include "Kernel/Trajectory.h"

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
  
  /// Return a trajectory (for track fit) from strip + offset
  virtual std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::VeloChannelID& id, const double offset) const = 0;

  /// Residual of 3-d point to a VeloChannelID
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                              const LHCb::VeloChannelID& channel,
                              double &residual,
                              double &chi2) const = 0;

  /// Residual [see DeVelo for explanation]
  /*  virtual StatusCode residual(const Gaudi::XYZPoint& point,
                              const LHCb::VeloChannelID& channel,
                              const double localOffset,
                              const double width,
                              double &residual,
                              double &chi2) const = 0;*/
  
 /// Residual of a 3-d point to a VeloChannelID + interstrip fraction
  virtual StatusCode residual(const Gaudi::XYZPoint& point, 
                              const LHCb::VeloChannelID& channel,
                              const double interStripFraction,
                              double &residual,
                              double &chi2) const =0 ;

  /// The zones number for a given strip
  //  virtual unsigned int zoneOfStrip(const LHCb::VeloChannelID& channel)=0;
  virtual unsigned int zoneOfStrip(const unsigned int strip) const = 0;
  
  /// The number of strips in a zone
  virtual unsigned int stripsInZone(const unsigned int zone) const = 0;

  /// The minimum radius of the sensitive area of a zone
  virtual double rMin(const unsigned int zone) const = 0;
  
  /// The maximum radius of the sensitive area of a zone
  virtual double rMax(const unsigned int zone) const = 0;

  /// Determine if point is in corner cut-offs
  virtual  StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const = 0;

  /// Determine if local point is in corner cut-offs
  virtual bool isCutOff(double x, double y) const = 0;

  /// Convert local position to global position
  /// Local from is +ve x (and Upstream for phi sensors)
  StatusCode localToGlobal(const Gaudi::XYZPoint& local, 
                           Gaudi::XYZPoint& global) const;

  /// Convert global position to local position 
  StatusCode globalToLocal(const Gaudi::XYZPoint& global, 
                           Gaudi::XYZPoint& local) const;

  /// Convert local position to position inside Velo half Box (one per side)
  /// Local from is +ve x and Upstream 
  StatusCode localToVeloHalfBox(const Gaudi::XYZPoint& local, 
                                Gaudi::XYZPoint& box) const;

  /// Convert position inside Velo half Box (one per side) into local position
  /// Local from is +ve x and Upstream 
  StatusCode veloHalfBoxToLocal(const Gaudi::XYZPoint& box, 
                                Gaudi::XYZPoint& local) const;

  /// Convert position inside Velo half Box (one per side) to global
  StatusCode veloHalfBoxToGlobal(const Gaudi::XYZPoint& box, 
                                 Gaudi::XYZPoint& global) const;

  /// Convert global position to  inside Velo half Box (one per side) 
  StatusCode globalToVeloHalfBox(const Gaudi::XYZPoint& global, 
                                 Gaudi::XYZPoint& box) const;

  /// Returns a pair of points which define the begin and end points of a strip in the local frame
  inline std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> localStripLimits(unsigned int strip) const {
    return m_stripLimits[strip];
  }
 
  /// Returns a pair of points which define the begin and end points of a strip in the global frame
  inline std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> globalStripLimits(unsigned int strip) const {
    Gaudi::XYZPoint begin=m_geometry->toGlobal(m_stripLimits[strip].first);
    Gaudi::XYZPoint end=m_geometry->toGlobal(m_stripLimits[strip].second);
    return std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint>(begin,end);
  }
 
  /// Convert local phi to ideal global phi
  virtual double localPhiToGlobal(double phiLocal) const = 0;
  
  /// Return the z position of the sensor in the global frame
  inline double z() const {return m_z;}
  
  /// Return true for X<0 side of the detector (-ve x is Right)
  inline bool isRight() const {return !m_isLeft;}
  
  /// Return true for X>0 side of the detector (+ve x is Left)
  inline bool isLeft() const {return m_isLeft;}
  
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
  
  /// Returns the module number
  inline std::string module() const {return m_module;}

  /// Returns the sensor type
  inline std::string type() const {return m_type;}
  inline std::string fullType() const {return m_fullType;}

  /// Set the sensor number
  inline void sensorNumber(unsigned int sensor) {m_sensorNumber=sensor;}

  /// Returns the sensor number
  inline unsigned int sensorNumber() const {return m_sensorNumber;}
  
  /// Cache the geometry information after changes to position from alignment
  void cacheGeometry();

  /// Convert routing line to chip channel (1234 -> 0213)
  unsigned int RoutingLineToChipChannel(unsigned int routLine) const {
    routLine = (2048-routLine);
    if(1 == routLine%4){
      routLine++;
    }else if(2 == routLine%4){
      routLine--;
    } 
    return routLine;
  }
  /// Convert chip channel to routing line (0213 -> 1234)
  unsigned int ChipChannelToRoutingLine(unsigned int chipChan) const {
    if(1 == chipChan%4){
      chipChan++;
    } else if(2 == chipChan%4) {
      chipChan--;
    }
    return (2048-chipChan);
  }  
  /// Convert chip channel to strip number
  unsigned int ChipChannelToStrip(unsigned int chipChan) const {
    return RoutingLineToStrip(ChipChannelToRoutingLine(chipChan));
  }
  /// Convert strip number to chip channel
  unsigned int StripToChipChannel(unsigned int strip) const {
    return RoutingLineToChipChannel(StripToRoutingLine(strip));
  }

  /// Convert routing line to strip number
  unsigned int RoutingLineToStrip(unsigned int routLine) const {
    return m_mapRoutingLineToStrip[routLine];
  }
  /// Convert strip number to routing line
  unsigned int StripToRoutingLine(unsigned int strip) const {return m_mapStripToRoutingLine[strip];}

  /// Get the chip number from the routing line
  unsigned int ChipFromRoutingLine(unsigned int routLine) const {
    return ChipFromChipChannel(RoutingLineToChipChannel(routLine));
  }
  /// Get the chip number from the chip channel
  unsigned int ChipFromChipChannel(unsigned int chipChan) const {return static_cast<int>(chipChan/128);}

  /// Get the Chip from the strip
  unsigned int ChipFromStrip(unsigned int strip) const {
    return ChipFromChipChannel(StripToChipChannel(strip));
  }

  /**  Return the validity of a strip
   *   Cince this method uses the condition cache, the result
   *   depends on CondDB.
   */
  bool OKStrip(unsigned int strip) const {
    return (strip<m_numberOfStrips && stripInfo(strip).stripIsReadOut());
  }
  
  /// Returns the validity of a given channel
  bool OKChipChannel(unsigned int chipChan) const {
    return (chipChan<m_numberOfStrips && OKStrip(ChipChannelToStrip(chipChan)));
  }

  // condition related forwar daclarations

  class StripInfo;

  // condition related protected types

protected:
  /** @struct DeVeloSensor::ConvertIntToStripInfo DeVeloSensor.h VeloDet/DeVeloSensor.h
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
    friend struct DeVeloSensor::ConvertIntToStripInfo;
  };

  // condition related public methods

public:
  /** Check whether this sensor is read out at all (cached condition).
   *  This information is based on CondDB, i.e. it can change
   *  with time.
   */
  bool isReadOut() const { return m_isReadOut; }

  /** Check whether this sensor exists or is TELL1 w/o sensor (cached condition).
   *  This information is based on CondDB, i.e. it can change
   *  with time.
   */
  bool tell1WithoutSensor() const { return m_tell1WithoutSensor; }

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

  mutable std::map<unsigned int, unsigned int> m_mapRoutingLineToStrip;//<Map of routing line to strips
  mutable std::map<unsigned int, unsigned int> m_mapStripToRoutingLine;//<Map of strips to routing line
  std::vector<std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> > m_stripLimits;//<Begin and end point of strips
  
private:

  void initSensor();

  static const unsigned int m_numberOfStrips=2048;
  std::string m_module;
  std::string m_type;
  std::string m_fullType;
  bool m_isLeft;
  bool m_isDownstream;
  bool m_isR;
  bool m_isPhi;
  bool m_isPileUp;
  unsigned int m_sensorNumber;
  double m_siliconThickness;
  double m_z;
  double m_innerRadius;
  double m_outerRadius;

  IGeometryInfo* m_geometry;

  IGeometryInfo* m_halfBoxGeom; ///< Geometry info of the parent half box

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
  bool m_tell1WithoutSensor;

  // Set output level for message service
  bool m_debug;
  bool m_verbose;
};
#endif // VELODET_DEVELOSENSOR_H
