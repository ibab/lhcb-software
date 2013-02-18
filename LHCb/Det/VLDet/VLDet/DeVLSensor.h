#ifndef DEVL_SENSOR_H
#define DEVL_SENSOR_H 1

// STL
#include <algorithm>
// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/PhysicalConstants.h"
// Det/DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
// Kernel/LHCbKernel 
#include "Kernel/Trajectory.h"

// Unique class identifier
static const CLID CLID_DeVLSensor = 1008301 ;

// Forward declarations
class DeVLRSensor;
class DeVLPhiSensor;

namespace LHCb {
  class VLChannelID;
}

/** @class DeVLSensor DeVLSensor.h VLDet/DeVLSensor.h
 *
 *  VL sensor base class (based on DeVeloSensor)
 *
 */

class DeVLSensor : public DetectorElement {

public:
  /// Constructor
  DeVLSensor(const std::string& name = "");
  /// Destructor
  virtual ~DeVLSensor() {}

  /// Object identification
  static const CLID& classID() {return CLID_DeVLSensor;}
  virtual const CLID& clID() const;

  /// Initialisation
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VLChannelID& channel,
                                    double& fraction,
                                    double& pitch) const = 0;
  /// Get the nth nearest neighbour for a given channel.
  virtual StatusCode neighbour(const LHCb::VLChannelID& start,
                               const int& nOffset,
                               LHCb::VLChannelID& channel) const = 0;

  /// Get the number of channels between two channels
  virtual StatusCode channelDistance(const LHCb::VLChannelID& start,
                                     const LHCb::VLChannelID& end,
                                     int& nOffset) const;

  /// Return a trajectory (for track fit) from strip + offset.
  virtual std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::VLChannelID& id, 
                                                     const double offset) const = 0;

  /// Calculate the residual of a 3d point to a strip.
  /// The supplied point is assumed to be in the plane.
  virtual StatusCode residual(const Gaudi::XYZPoint& point,
                              const LHCb::VLChannelID& channel,
                              double& residual,
                              double& chi2) const = 0;
  /// Calculate the residual of a 3d point to a strip + interstrip fraction.
  virtual StatusCode residual(const Gaudi::XYZPoint& point,
                              const LHCb::VLChannelID& channel,
                              const double interStripFraction,
                              double& residual,
                              double& chi2) const = 0;

  /// Determine if a local 3d point is inside the sensor.
  virtual StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const = 0;

  /// Zone number for a given strip
  virtual unsigned int zoneOfStrip(const unsigned int strip) const = 0;
  /// Global zone number for a strip
  virtual unsigned int globalZoneOfStrip(const unsigned int strip) const = 0;

  /// Number of strips in a given zone
  virtual unsigned int stripsInZone(const unsigned int zone) const = 0;
  /// Minimum radius of the sensitive area of a zone
  virtual double rMin(const unsigned int zone) const = 0;
  /// Maximum radius of the sensitive area of a zone
  virtual double rMax(const unsigned int zone) const = 0;

  /// Calculate the length of a strip.
  virtual double stripLength(const unsigned int strip) const = 0;

  /// Convert local position to global position.
  /// Local from is +ve x (and Upstream for Phi sensors)
  Gaudi::XYZPoint localToGlobal(const Gaudi::XYZPoint& localPos) const {
    return m_geometry->toGlobal(localPos);
  }
  /// Convert global position to local position.
  Gaudi::XYZPoint globalToLocal(const Gaudi::XYZPoint& globalPos) const {
    return m_geometry->toLocal(globalPos);
  }

  /// Pair of points defining start and end points of a strip in the local frame
  std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> localStripLimits(unsigned int strip) const {
    return m_stripLimits[strip];
  }
  /// Pair of points defining start and end points of a strip in the global frame
  std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> globalStripLimits(unsigned int strip) const {
    Gaudi::XYZPoint begin = m_geometry->toGlobal(m_stripLimits[strip].first);
    Gaudi::XYZPoint end = m_geometry->toGlobal(m_stripLimits[strip].second);
    return std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint>(begin, end);
  }

  /// Convert local phi to ideal global phi
  double localPhiToGlobal(double phiLocal) const {
    if (isDownstream()) phiLocal = -phiLocal;
    if (isRight()) phiLocal += Gaudi::Units::pi;
    return phiLocal;
  }

  /// Z position of the sensor in the global frame
  double z() const {return m_z;}
  double z(const double x, const double y) const {
    return m_z + x * m_dzDx + y * m_dzDy;
  }
  // Centre of the sensor in the global frame
  double xCentre() const {return m_centre.x();}
  double yCentre() const {return m_centre.y();}

  /// Station number, station contains 2 modules (right and left)
  unsigned int station() const {return (m_sensorNumber & 0x3E) >> 1;}
  /// Right side is x < 0 side of the detector
  bool isRight() const {return !m_isLeft;}
  bool isLeft() const {return m_isLeft;}
  bool isDownstream() const {return m_isDownstream;}
  bool isR() const {return m_isR;}
  bool isPhi() const {return m_isPhi;}

  /// Fast cast to R sensor, returns 0 for wrong type
  inline const DeVLRSensor* rSensor() const;
  /// Fast cast to Phi sensor, returns 0 for wrong type
  inline const DeVLPhiSensor* phiSensor() const;

  unsigned int numberOfStrips() const {return m_numberOfStrips;}
  unsigned int numberOfZones() const {return m_numberOfZones;}

  /// Minimum radius of the sensitive area
  double innerRadius() const {return m_innerRadius;}
  /// Maximum radius of the sensitive area
  double outerRadius() const {return m_outerRadius;}
  /// Bounding box of the sensitive area
  double boundingBoxX() const {return m_boundingBoxX;}
  double boundingBoxY() const {return m_boundingBoxY;}
  /// Thickness of the sensor (in mm)
  double siliconThickness() const {return m_siliconThickness;}
  /// Software module number
  std::string module() const {return m_module;}
  /// Workaround to prevent hidden base class function
  const std::type_info& type(const std::string &name) const {
    return ParamValidDataObject::type(name);
  }
  std::string type() const {return m_type;}

  /// Set/get the sensor number
  void sensorNumber(unsigned int sensor) {m_sensorNumber = sensor;}
  unsigned int sensorNumber() const {return m_sensorNumber;}

protected:

  /// Initialisation from XML
  StatusCode initSensor();
  /// Register condition call backs (to be called once from initialize())
  virtual StatusCode registerConditionCallBacks();
  virtual StatusCode cacheGeometry();

  unsigned int m_numberOfZones;
  unsigned int m_numberOfStrips;
  // Map of routing line to strips
  mutable std::map<unsigned int, unsigned int> m_mapRoutingLineToStrip;
  // Map of strips to routing lines
  mutable std::map<unsigned int, unsigned int> m_mapStripToRoutingLine;
  // Begin and end point of strips
  std::vector<std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> > m_stripLimits;

  /// Geometry info of the sensor
  IGeometryInfo* m_geometry;

private:

  std::string m_module;
  std::string m_type;
  bool m_isLeft;
  bool m_isDownstream;
  bool m_isR;
  bool m_isPhi;
  unsigned int m_sensorNumber;
  double m_siliconThickness;
  Gaudi::XYZPoint m_centre;
  double m_z;
  double m_dzDx;
  double m_dzDy;
  double m_innerRadius;
  double m_outerRadius; 
  double m_boundingBoxX;
  double m_boundingBoxY;

  /// Output level for message service
  bool m_debug;
  bool m_verbose;

};

#endif
