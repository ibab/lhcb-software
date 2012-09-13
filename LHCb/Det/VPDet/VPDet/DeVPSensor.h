// $Id: DeVPSensor.h,v 1.8 2009-12-03 15:05:18 cocov Exp $
#ifndef VPDET_DEVPSENSOR_H
#define VPDET_DEVPSENSOR_H 1

// Include files
#include <algorithm>
#include <bitset>

// Gaudi Math definitions
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/PhysicalConstants.h"

// from Det/DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

// get LHCbID for trajectory
#include "Kernel/Trajectory.h"

// Unique class identifier
static const CLID CLID_DeVPSensor = 1008201 ;

// forward declarations needed for fast casting
class DeVP;
class DeVPXType;
class DeVPYType;
class DeVPSquareType;

namespace LHCb {
  class VPChannelID;
}

/** @class DeVPSensor DeVPSensor.h VPDet/DeVPSensor.h
 *
 *  VP sensor base class
 *  all functions the are common to evry type of sensor should be defined here
 *  @author Victor Coco
 *  @date   2009-05-14
 */
class DeVPSensor : public DetectorElement {
public:
  /// Standard constructor
  DeVPSensor( const std::string& name = "" );

  /// Destructor
  virtual ~DeVPSensor( );

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVPSensor; };
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVPSensor from the XML
  virtual StatusCode initialize();

  /// Return a trajectory (for track fit) from strip + offset
  virtual std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::VPChannelID& id, 
                                                     const std::pair<double,double> offset) const = 0;

  /// Calculate the nearest channel to a 3-d point.
  /// Also returns the fractional x-y position IN the pixel
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VPChannelID& channel,
                                    std::pair <double, double>& fraction) const = 0;

  /// Calculate the XYZ center of a pixel
  virtual StatusCode channelToPoint( const LHCb::VPChannelID& channel,
                                                Gaudi::XYZPoint& point) const = 0;

  virtual StatusCode channelToPointWithFraction( const LHCb::VPChannelID& channel,
                                                        const std::pair<double,double> offset,
                                                 Gaudi::XYZPoint& point) const= 0;

  ///  Get the list of VPChannelID forming the 3x3 cluster of pixel centered on point
  virtual StatusCode pointTo3x3Channels(const Gaudi::XYZPoint& point,
                                        std::vector <LHCb::VPChannelID>& channels) const = 0;

  /// Get the 8 channel (if they exist) arround a given seed channel
  virtual StatusCode  channelToNeighbours( const LHCb::VPChannelID& seedChannel, 
                                       std::vector <LHCb::VPChannelID>& channels) const = 0;

  /// Return the pixel size (since the edge pixel might have different size)
  virtual std::pair<double,double> PixelSize(LHCb::VPChannelID channel) const = 0;

  /// Return true if the pixel is a long one
  virtual bool isLong(LHCb::VPChannelID channel) const = 0;

  /// Determines if local 3-d point is inside sensor
  virtual StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const = 0;


  /// Convert local position to global position
  /// Local from is +ve x (and Upstream for phi sensors)[directly from DeVelo, not checked]
  inline Gaudi::XYZPoint localToGlobal(const Gaudi::XYZPoint& localPos) const {
    return m_geometry->toGlobal(localPos);
  }

  /// Convert global position to local position [directly from DeVelo, not checked]
  inline Gaudi::XYZPoint globalToLocal(const Gaudi::XYZPoint& globalPos) const {
    return m_geometry->toLocal(globalPos);
  }

  /// Convert local position to position inside VP half Box (one per side)
  /// Local from is +ve x and Upstream   [directly from DeVelo, not checked]
  inline Gaudi::XYZPoint localToVPHalfBox(const Gaudi::XYZPoint& localPos) const {
    const Gaudi::XYZPoint globalPos = m_geometry->toGlobal(localPos);
    return m_halfBoxGeom->toLocal(globalPos);
  }

  /// Convert position inside VP half Box (one per side) into local position
  /// Local from is +ve x and Upstream    [directly from DeVelo, not checked]
  inline Gaudi::XYZPoint vPHalfBoxToLocal(const Gaudi::XYZPoint& boxPos) const {
    Gaudi::XYZPoint globalPos = m_halfBoxGeom->toGlobal(boxPos);
    return m_geometry->toLocal(globalPos);
  }

  /// Convert position inside VP half Box (one per side) to global [directly from DeVelo, not checked]
  inline Gaudi::XYZPoint vPHalfBoxToGlobal(const Gaudi::XYZPoint& boxPos) const {
    return m_halfBoxGeom->toGlobal(boxPos);
  }

  /// Convert global position to inside VP half Box (one per side)
  inline Gaudi::XYZPoint globalToVPHalfBox(const Gaudi::XYZPoint& globalPos) const {
    return m_halfBoxGeom->toLocal(globalPos);
  }

  /// Return the z position of the sensor in the global frame
  inline double z() const {return m_z;}

  /// Return station number, station contains 2 modules (right and left)
  inline unsigned int station() const { return (m_sensorNumber & 0xFE) >> 1; }

  /// Return true for X<0 side of the detector (-ve x is Right)
  inline bool isRight() const {return !m_isLeft;}

  /// Return true for X>0 side of the detector (+ve x is Left)
  inline bool isLeft() const {return m_isLeft;}

  /// Returns true if sensor is downstream
  inline bool isDownstream() const {return m_isDownstream;}

  /// Returns true if X Sensor
  inline bool isX() const {return m_isX;}

  /// Returns true if Y Sensor
  inline bool isY() const {return m_isY;}

  /// Returns true if Square Sensor
  inline bool isSquare() const {return m_isSquare;}

  /// fast cast to X sensor, returns 0 for wrong type
  inline const DeVPXType* xType() const;

  /// fast cast to Y sensor, returns 0 for wrong type
  inline const DeVPYType* yType() const;

  /// fast cast to Square sensor, returns 0 for wrong type
  inline const DeVPSquareType* squareType() const;

  ///  the chip active length (size along the dimension on which the chips are aligned)(mm)
  inline double chipLength() const {return m_chipLength;}
  /// the chip active width (mm)
  inline double chipWidth() const {return m_chipWidth;}
  /// the distance between chip (mm)
  inline double interChipDist() const {return m_interChipDist;}
  /// the size of pixel along the high precision dimension 
  inline double hpSize() const {return m_hpSize;}
  /// the size of pixel along the low precision dimension (is identical to hpSize if Square pixel)
  inline double lpSize() const {return m_lpSize;}
  /// the number of pixel in one column (for FPIX column is along the hp dimension)
  inline int nPixCol() const {return m_nPixCol;}
  /// the number of pixel in one row (for FPIX column is along the lp dimension)
  inline int nPixRow() const {return m_nPixRow;}

  /// The thickness of the sensor in mm
  inline double siliconThickness() const {return m_siliconThickness;}
  /// The number of ladders orming the detector
  inline int ladderNumber() const {return m_ladderNumber;}

  /// Returns the software module number
  inline unsigned int module() const {return m_module;}
 
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

  /// Returns the size of the interchip pixel
  inline double interchipPixSize() const {return m_interchipPixSize;}

  /// Cache the geometry information after changes to position from alignment
  StatusCode cacheGeometry();

protected:

  //unsigned int m_numberOfZones;

  // THIS COMES FROM THE CHANNEL ID... IF CHANNEL ID CHANGES (ie.nb bits allocated to pixels and chip num) 
  // IT SHOULD BE MODIFIED  --> recalculated in VP...Type
  //static const unsigned long m_numberOfPixelsPerChip=65536;
  //static const unsigned long m_numberOfChips=64;
  
  IGeometryInfo* m_geometry;
  IGeometryInfo* m_halfBoxGeom; ///< Geometry info of the parent half box  [directly from DeVelo, not checked]

private:

  void initSensor(); ///< sensor initialisation

  unsigned int m_module      ; ///< Module number
  std::string m_type         ; ///< Sensor type
  std::string m_side         ; ///< Module side
  std::string m_fullType     ; ///< Sensor full type (include up/down strem info + side)
  bool m_isLeft              ; ///< True if side is left
  bool m_isRight             ; ///< True if side is right
  bool m_isDownstream        ; ///< True if downstream the IP
  bool m_isX                 ; ///< True if assymetric X type
  bool m_isY                 ; ///< True if assymetric Y type
  bool m_isSquare            ; ///< True if symetric type
  unsigned int m_sensorNumber; ///< Sensor number
  double m_z                 ; ///< Sensor z position in the global frame
  int m_ladderNumber         ; ///< Number of ladders constituing the sensor

  // parameters relative to the chip used
  double m_siliconThickness  ; ///< Active silicon thickness
  double m_chipWidth         ; ///< Chip width
  double m_chipLength        ; ///< Chip length
  double m_interChipDist     ; ///< Distance between chips
  double m_hpSize            ; ///< high precision pixel dimension (eq. to low if symetric)
  double m_lpSize            ; ///< low precision pixel dimension (eq. to high if symetric)
  int m_nPixCol              ; ///< Number of pixel column in a chip
  int m_nPixRow              ; ///< Number of pixel row in a chip
  double m_interchipPixSize  ; ///< Size of the pixel at the edge of the chip
 
  std::vector<double> m_pixelNoise ; ///< Vector of noise for pixels
  
  friend class DeVP        ;
  mutable int m_moduleId          ;

  // Set output level for message service
  bool m_debug                    ;
  bool m_verbose                  ;
};
#endif // VPDET_DEVPSENSOR_H
