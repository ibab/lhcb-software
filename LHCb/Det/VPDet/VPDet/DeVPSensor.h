#ifndef VPDET_DEVPSENSOR_H
#define VPDET_DEVPSENSOR_H 1

// Gaudi 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Point3DTypes.h"

// Det/Desc 
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
// Kernel/LHCbKernel
#include "Kernel/Trajectory.h"
#include "Kernel/VPChannelID.h"

// Unique class identifier
static const CLID CLID_DeVPSensor = 1008205;

class DeVP;

/** @class DeVPSensor DeVPSensor.h VPDet/DeVPSensor.h
 *
 *  Detector element class for a single VP sensor
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
  std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::VPChannelID& id, 
                                             const std::pair<double, double> offset) const;

  /// Calculate the nearest channel to a given point.
  StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                            const bool local, 
                            LHCb::VPChannelID& channel) const;
  StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                            const bool local,
                            LHCb::VPChannelID& channel,
                            std::pair <double, double>& fraction) const;
  /// Calculate the position of a given pixel.
  Gaudi::XYZPoint channelToPoint(const LHCb::VPChannelID& channel,
                                 const bool local) const;
  Gaudi::XYZPoint channelToPoint(const LHCb::VPChannelID& channel, 
                                 const std::pair<double, double> fraction) const;

  /// Return the pixel size.
  std::pair<double, double> pixelSize(LHCb::VPChannelID channel) const;

  /// Return true if the pixel is elongated.
  bool isLong(LHCb::VPChannelID channel) const;

  /// Determine if a local 3-d point is inside the sensor active area.
  StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const;

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

  /// Return sensor thickness in mm.
  double siliconThickness() const {return m_thickness;}

  /// Workaround to prevent hidden base class function
  inline const std::type_info& type(const std::string &name) const {
    return ParamValidDataObject::type(name);
  }

  /// Function kept for backwards compatibility with VPDAQ
  virtual StatusCode channelToNeighbours(const LHCb::VPChannelID& seedChannel, 
                                         std::vector <LHCb::VPChannelID>& channels) const;

private:

  IGeometryInfo* m_geometry;

  unsigned int m_sensorNumber;
  unsigned int m_module;
  bool m_isLeft;
  bool m_isDownstream;

  double m_z;
  double m_thickness;

  /// Dimensions of the sensor active area
  double m_sizeX;
  double m_sizeY;
  /// Number of chips per ladder
  unsigned int m_nChips;
  /// Length of chip active area
  double m_chipSize;
  /// Distance between two chips
  double m_interChipDist;
  /// Number of columns and rows
  unsigned int m_nCols;
  unsigned int m_nRows;
  /// Cell size of pixels
  double m_pixelSize;
  /// Cell size in column direction of elongated pixels
  double m_interChipPixelSize;
  /// Index of the first chip
  unsigned int m_chip;

  /// Output level flag
  bool m_debug;

  /// Message stream
  mutable MsgStream* m_msg;
  MsgStream& msg() const {
    if (!m_msg) m_msg = new MsgStream(msgSvc(), "DeVPSensor");
    return *m_msg;
  }
  /// Update geometry cache when the alignment changes
  StatusCode updateGeometryCache();

};
#endif 
