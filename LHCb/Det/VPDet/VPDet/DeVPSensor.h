#ifndef VPDET_DEVPSENSOR_H
#define VPDET_DEVPSENSOR_H 1

// Gaudi 
#include "GaudiKernel/Point3DTypes.h"

// Det/Desc 
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
// Kernel/LHCbKernel
#include "Kernel/Trajectory.h"
#include "Kernel/VPChannelID.h"

// Unique class identifier
static const CLID CLID_DeVPSensor = 1008201;

class DeVP;

/** @class DeVPSensor DeVPSensor.h VPDet/DeVPSensor.h
 *
 *  VP sensor base class
 *  @author Victor Coco
 *  @date   2009-05-14
 */

class DeVPSensor : public DetectorElement {

public:

  /// Constructor
  DeVPSensor(const std::string& name = "");
  /// Destructor
  virtual ~DeVPSensor();

  /// Object identification
  static  const CLID& classID() {return CLID_DeVPSensor;}
  virtual const CLID& clID() const;

  /// Initialise the sensor.
  virtual StatusCode initialize();

  /// Return a trajectory (for track fit) from channel + offset
  virtual std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::VPChannelID& id, 
                                                     const std::pair<double, double> offset) const = 0;

  /// Calculate the nearest channel to a given point.
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    const bool local, 
                                    LHCb::VPChannelID& channel) const = 0;
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    const bool local,
                                    LHCb::VPChannelID& channel,
                                    std::pair <double, double>& fraction) const = 0;
  /// Calculate the position of a given pixel.
  virtual Gaudi::XYZPoint channelToPoint(const LHCb::VPChannelID& channel,
                                         const bool local) const = 0;
  virtual Gaudi::XYZPoint channelToPoint(const LHCb::VPChannelID& channel, 
                                         const std::pair<double, double> fraction) const = 0;

  /// Return the pixel size.
  virtual std::pair<double, double> pixelSize(LHCb::VPChannelID channel) const = 0;

  /// Return true if the pixel is elongated.
  virtual bool isLong(LHCb::VPChannelID channel) const = 0;

  /// Determines if local 3-d point is inside sensor
  virtual StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const = 0;

  /// Convert local position to global position
  Gaudi::XYZPoint localToGlobal(const Gaudi::XYZPoint& point) const {
    return m_geometry->toGlobal(point);
  }
  /// Convert global position to local position 
  Gaudi::XYZPoint globalToLocal(const Gaudi::XYZPoint& point) const {
    return m_geometry->toLocal(point);
  }

  /// Return the z position of the sensor in the global frame
  double z() const {return m_z;}

  /// Return the sensor number
  unsigned int sensorNumber() const {return m_sensorNumber;}
  /// Return the module number
  unsigned int module() const {return m_module;}
  /// Return station number (station comprises left and right module)
  unsigned int station() const {return m_module >> 1;}

  /// Return true for x < 0 side of the detector
  bool isRight() const {return !m_isLeft;}
  /// Return true for x > 0 side of the detector
  bool isLeft() const {return m_isLeft;}
  /// Return true if sensor is downstream
  bool isDownstream() const {return m_isDownstream;}

  /// Return sensor thickness in mm
  virtual double siliconThickness() const = 0;

  /// Workaround to prevent hidden base class function
  inline const std::type_info& type(const std::string &name) const {
    return ParamValidDataObject::type(name);
  }

  /// Cache the geometry information when position changes 
  StatusCode cacheGeometry();

  /// Functions kept for backwards compatibility
  virtual StatusCode pointTo3x3Channels(const Gaudi::XYZPoint& point,
                                        std::vector <LHCb::VPChannelID>& channels) const = 0;
  virtual StatusCode channelToNeighbours(const LHCb::VPChannelID& seedChannel, 
                                         std::vector <LHCb::VPChannelID>& channels) const = 0;

protected:

  IGeometryInfo* m_geometry;

private:

  unsigned int m_sensorNumber;
  unsigned int m_module;
  bool m_isLeft;
  bool m_isDownstream;

  double m_z;

  bool m_debug;
  bool m_verbose;

};
#endif 
