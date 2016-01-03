// $Id: DeVeloSensor.h,v 1.45 2010-02-07 15:10:19 krinnert Exp $
#ifndef VELODET_DEVELOSENSOR_H
#define VELODET_DEVELOSENSOR_H 1

// Include files
#include <algorithm>
#include <bitset>

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Gaudi Math definitions
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/PhysicalConstants.h"

// from Det/DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

// get LHCbID for trajectory
#include "Kernel/Trajectory.h"

// Unique class identifier
static const CLID CLID_DeVeloSensor = 1008101 ;

// forward declarations needed for fast casting
class DeVelo;
class DeVeloRType;
class DeVeloPhiType;

namespace LHCb {
  class VeloChannelID;
}

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
  virtual std::unique_ptr<LHCb::Trajectory> trajectory(const LHCb::VeloChannelID& id, const double offset) const = 0;

  /** Residual of 3-d point to a VeloChannelID
   *
   *  This is not a residual in 3D! The supplied 3-d point is assumed
   *  to be in the plane of the sensor and the residual is computed
   *  in this plane. No check is performed whether the point is 
   *  actually on the sensor.
   */
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

  /** Residual of 3-d point to a VeloChannelID + interstrip fraction
   *
   *  This is not a residual in 3D! The supplied 3-d point is assumed
   *  to be in the plane of the sensor and the residual is computed
   *  in this plane. No check is performed whether the point is 
   *  actually on the sensor.
   */
  virtual StatusCode residual(const Gaudi::XYZPoint& point,
                              const LHCb::VeloChannelID& channel,
                              const double interStripFraction,
                              double &residual,
                              double &chi2) const =0 ;

  /** Residual of a line-sensor plane intersection to a VeloChannelID + interstrip fraction
   *
   *  This is not a residual in 3D! The line is defined by a point and a
   *  direction in the global frame. The line is intersected with the plane of
   *  the sensor and the residual is computed in this plane.  No check is
   *  performed whether the intersection is actually on the sensor.
   */
  StatusCode residual(const Gaudi::XYZPoint& point,
      const Gaudi::XYZVector& dir,
      const LHCb::VeloChannelID& channel,
      const double interStripFraction,
      double &residual,
      double &chi2) const;

  /** Propagate a line to the sensor plane in the global frame.
   *
   *  The line is defined by a 3-d point and slope vector in the
   *  global frame. The resulting 3-d point is the intersection 
   *  of the line and the sensor plane. Note that the result is
   *  not guaranteed to actually be on the sensor.
   */
  StatusCode intersectWithLine(const Gaudi::XYZPoint& point,
                               const Gaudi::XYZVector& dir,
                               Gaudi::XYZPoint& intersection) const;


  /// The zones number for a given strip
  //  virtual unsigned int zoneOfStrip(const LHCb::VeloChannelID& channel)=0;
  virtual unsigned int zoneOfStrip(const unsigned int strip) const = 0;

  /// The global zone number for a strip (corrected for downstream facing r type sensors)
  unsigned int globalZoneOfStrip(const unsigned int strip) const {
    return isPhi() ? static_cast<unsigned int>(strip>682) : (isDownstream() ? 3-strip/512 : strip/512);
  }

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

  /// Return the length of a strip
  virtual double stripLength(const unsigned int strip) const = 0;

  /// Convert local position to global position
  /// Local from is +ve x (and Upstream for phi sensors)
  inline Gaudi::XYZPoint localToGlobal(const Gaudi::XYZPoint& localPos) const {
    return m_geometry->toGlobal(localPos);
  }

  /// Convert global position to local position
  inline Gaudi::XYZPoint globalToLocal(const Gaudi::XYZPoint& globalPos) const {
    return m_geometry->toLocal(globalPos);
  }

  /// Convert local position to position inside Velo half Box (one per side)
  /// Local from is +ve x and Upstream
  inline Gaudi::XYZPoint localToVeloHalfBox(const Gaudi::XYZPoint& localPos) const {
    const Gaudi::XYZPoint globalPos = m_geometry->toGlobal(localPos);
    return m_halfBoxGeom->toLocal(globalPos);
  }

  /// Convert position inside Velo half Box (one per side) into local position
  /// Local from is +ve x and Upstream
  inline Gaudi::XYZPoint veloHalfBoxToLocal(const Gaudi::XYZPoint& boxPos) const {
    Gaudi::XYZPoint globalPos = m_halfBoxGeom->toGlobal(boxPos);
    return m_geometry->toLocal(globalPos);
  }

  /// Convert position inside Velo half Box (one per side) to global
  inline Gaudi::XYZPoint veloHalfBoxToGlobal(const Gaudi::XYZPoint& boxPos) const {
    return m_halfBoxGeom->toGlobal(boxPos);
  }

  /// Convert global position to inside Velo half Box (one per side)
  inline Gaudi::XYZPoint globalToVeloHalfBox(const Gaudi::XYZPoint& globalPos) const {
    return m_halfBoxGeom->toLocal(globalPos);
  }

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
  double localPhiToGlobal(double phiLocal) const {
    if(isDownstream()) phiLocal = -phiLocal;
    if(isRight()) phiLocal += Gaudi::Units::pi;
    return phiLocal;
  }

  /// Return the z position of the sensor in the global frame
  inline double z() const {return m_z;}

  /// Return station number, station contains 2 modules (right and left)
  inline unsigned int station() const { return (m_sensorNumber & 0x3E) >> 1; }

  /// Return true for X<0 side of the detector (-ve x is Right)
  inline bool isRight() const {return !m_isLeft;}

  /// Return true for X>0 side of the detector (+ve x is Left)
  inline bool isLeft() const {return m_isLeft;}

  /// Returns true if sensor is downstream
  inline bool isDownstream() const {return m_isDownstream;}

  /// Returns true if  pile up Sensor
  inline bool isPileUp() const {return m_isPileUp;}

  /// Returns true if R Sensor
  inline bool isR() const {return m_isR;}

  /// Returns true if Phi Sensor
  inline bool isPhi() const {return m_isPhi;}

  /// Returns true if sensor is top cabled
  inline bool isTop() const {return m_isTop;}

  /// Returns true if sensor is bottom cabled
  inline bool isBottom() const {return !m_isTop;}

  /// fast cast to R sensor, returns 0 for wrong type
  inline const DeVeloRType* rType() const;

  /// fast cast to Phi sensor, returns 0 for wrong type
  inline const DeVeloPhiType* phiType() const;

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

  /// Returns the software module number
  inline std::string module() const {return m_module;}

  /// Returns the hardware module number
  inline unsigned int moduleId() const {return m_moduleId;}

  /// Workaround to prevent hidden base class function
  inline const std::type_info& type(const std::string &name) const
                      {return ParamValidDataObject::type(name);}
  /// Returns the sensor type
  inline std::string type() const {return m_type;}
  inline std::string fullType() const {return m_fullType;}

  /// Set the sensor number
  inline void sensorNumber(unsigned int sensor) {m_sensorNumber=sensor;}

  /// Returns the sensor number
  inline unsigned int sensorNumber() const {return m_sensorNumber;}

  /// Cache the geometry information after changes to position from alignment
  StatusCode cacheGeometry();

  /// Convert routing line to chip channel (1234 -> 0213)
  unsigned int RoutingLineToChipChannel(unsigned int routLine) const {
    routLine = (m_maxRoutingLine-routLine);
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
    return (m_numberOfStrips-chipChan);
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
   *   Since this method uses the condition cache, the result
   *   depends on CondDB.
   */
  bool OKStrip(unsigned int strip) const {
    return (strip<m_numberOfStrips && stripInfo(strip).stripIsOK());
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
    StripInfo operator() (int i) { return StripInfo(i); }
  };

  // condition related public types

public:
  /** @class DeVeloSensor::StripInfo DeVeloSensor.h VeloDet/DeVeloSensor.h
   *
   *  Encodes strip information.
   *  From construction information states are :
   *  OK, Low gain, Noisy, Dead, Open, Pinhole and Short
   *
   *  @author Kurt Rinnert
   *  @date   2006-01-18
   */
  class StripInfo
  {
  public:
    StripInfo() : m_info(0) { ; }
    StripInfo(int i) : m_info(i) { ; }

  private:
    enum {
      LOWGAIN=  2,
      NOISY= 1,
      STRIPOK = 0,
      DEAD=    -1,
      OPEN=    -2,
      PINHOLE= -3,
      SHORT=   -4
    };

  public:
    /// No problems with this strip
    bool stripIsOK() const { return m_info==STRIPOK; }
    /// Lower gain strip
    bool stripIsLowGain() const { return m_info==LOWGAIN; }
    /// higher noise strip
    bool stripIsNoisy() const { return m_info==NOISY; }
    /// very low gain strip
    bool stripIsDead() const { return m_info==DEAD; }
    /// Did not bond strip to beetle
    bool stripIsOpen() const { return m_info==OPEN; }
    /// pinhole in sensor
    bool stripIsPinhole() const { return m_info==PINHOLE; }
    /// Strip shorted to another (may not be next neighbour)
    bool stripIsShort() const { return m_info==SHORT; }
    /// StripInfo as integer
    int asInt() const{ return m_info; }

  private:
    int m_info;

  };

  /** @class DeVeloSensor::Tell1EventInfo DeVeloSensor.h VeloDet/DeVeloSensor.h
   *
   *  Provides event-specific TELL1 information.
   *  This information is only valid after the VELO ZS decoding stage.
   *  The default state after construction or reset is 'bad', that is
   *  the TELL1 is considered to have an error and the bank is 
   *  considered not to have been decoded. The decoder will reset
   *  and then change these flags to the true value in each event. 
   *
   *  @author Kurt Rinnert
   *  @date   2011-10-05
   */
  class Tell1EventInfo {

    public:

      Tell1EventInfo() 
        : m_hasError(true) 
        , m_wasDecoded(false) 
      { ; }

      void reset() const { m_hasError=true; m_wasDecoded=false; }
      void setHasError( const bool flag ) const { m_hasError=flag; }
      void setWasDecoded( const bool flag ) const { m_wasDecoded=flag; }

      bool hasError() const { return m_hasError; };
      bool wasDecoded() const { return m_wasDecoded; };

    private:

      mutable bool m_hasError;
      mutable bool m_wasDecoded;
  };

  // condition and event related public methods

public:
  /** Retrieve event-specific TELL1 information. 
   *  This information is only valid after the decoding stage.
   */
  const Tell1EventInfo& tell1EventInfo() const { return m_tell1EventInfo; }

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

  /** The Noise of a strip (cached condition).
   *  This information is based on CondDB, i.e. it can change
   *  with time.
   */
  double stripNoise(unsigned int strip) const { return m_stripNoise[strip]; }

  /** Get info for this strip (cached condition).
   *  This information is based on CondDB, i.e. it can change
   *  with time.
   *  @see StripInfo
   */
  StripInfo stripInfo(unsigned int strip) const { return m_stripInfos[strip]; }

  /// call back function for strip noise condition update
  StatusCode updateStripNoiseCondition();

  /// call back function for strip info condition update
  StatusCode updateStripInfoCondition();

  /// call back function for readout condition update
  StatusCode updateReadoutCondition();

  /** direct access to strip noise condition.
   *  This is for expert/testing purposes only.  All production
   *  client code should use the interface to the cached conditions.
   */
  const Condition* stripNoiseCondition() const { return m_stripNoiseCondition; }

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

  /// On demand access to MsgStream object
  inline MsgStream & msg() const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream( msgSvc(), "DeVeloSensor" );
    return *m_msgStream;
  }

protected:

  unsigned int m_numberOfZones;
  static const unsigned int m_minRoutingLine=1;
  static const unsigned int m_maxRoutingLine=2048;
  static const unsigned int m_numberOfStrips=2048;

  mutable std::map<unsigned int, unsigned int> m_mapRoutingLineToStrip;//<Map of routing line to strips
  mutable std::map<unsigned int, unsigned int> m_mapStripToRoutingLine;//<Map of strips to routing line
  std::vector<std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> > m_stripLimits;//<Begin and end point of strips

  IGeometryInfo* m_geometry;

  IGeometryInfo* m_halfBoxGeom; ///< Geometry info of the parent half box

private:

  void initSensor();

  std::string m_module;
  std::string m_type;
  std::string m_fullType;
  bool m_isLeft;
  bool m_isTop;
  bool m_isDownstream;
  bool m_isR;
  bool m_isPhi;
  bool m_isPileUp;
  unsigned int m_sensorNumber;
  double m_siliconThickness;
  double m_z;
  double m_innerRadius;
  double m_outerRadius;

  // condition cache
  std::string m_stripNoiseConditionName;
  std::string m_stripInfoConditionName;
  std::string m_readoutConditionName;

  const Condition* m_stripNoiseCondition;
  const Condition* m_stripInfoCondition;
  const Condition* m_readoutCondition;

  std::vector<double> m_stripNoise;
  std::vector<StripInfo> m_stripInfos;
  bool m_isReadOut;
  bool m_tell1WithoutSensor;

  friend class DeVelo;
  mutable int m_moduleId;//<Liverpool database module id

  // Set output level for message service
  bool m_debug;
  bool m_verbose;

  /// cached Message Stream object
  mutable MsgStream * m_msgStream;

  /// event-specific TELL1 info
  Tell1EventInfo m_tell1EventInfo;

};
#endif // VELODET_DEVELOSENSOR_H
