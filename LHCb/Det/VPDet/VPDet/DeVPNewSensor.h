#ifndef VPDET_DEVPNEWSENSOR_H 
#define VPDET_DEVPNEWSENSOR_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Local
#include "VPDet/DeVPSensor.h"

// Unique class identifier
static const CLID CLID_DeVPNewSensor = 1008205;

namespace LHCb {
  class VPChannelID;
}

/** @class DeVPNewSensor DeVPNewSensor.h VPDet/DeVPNewSensor.h
 *  
 */

class DeVPNewSensor : public DeVPSensor {

public:
  
  /// Standard constructor
  DeVPNewSensor(const std::string& name = ""); 
  /// Destructor
  virtual ~DeVPNewSensor();

  /// Object identification
  static  const CLID& classID() {return CLID_DeVPNewSensor;}
  virtual const CLID& clID() const;

  // Initialization
  virtual StatusCode initialize();

  /// Return a trajectory (for track fit) from pixel + fraction
  virtual std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::VPChannelID& id,
                                                     const std::pair<double, double> offset) const;


  /// Calculate the nearest channel to point in the global frame.
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VPChannelID& channel,
                                    std::pair <double, double>& fraction) const;
  /// Calculate the global position of a given pixel.
  virtual Gaudi::XYZPoint channelToPoint(const LHCb::VPChannelID& channel) const;
  virtual Gaudi::XYZPoint channelToPoint(const LHCb::VPChannelID& channel,
                                         const std::pair<double, double> fraction) const;

 /// Get the list of VPChannelID forming the 3x3 cluster of pixel centered on point
  virtual StatusCode pointTo3x3Channels(const Gaudi::XYZPoint& point,
                                        std::vector <LHCb::VPChannelID>& channels) const;

  /// Get the eight channels (if they exist) arround a given seed channel
  virtual StatusCode channelToNeighbours(const LHCb::VPChannelID& channel, 
                                         std::vector <LHCb::VPChannelID>& neighbours) const;
  
  /// Determine if local point is inside the active area of the sensor.
  virtual StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const;
  /// Return the sensor thickness.
  virtual double siliconThickness() const {return m_thickness;}

  // Temporary function for backwards compatibility
  virtual int ladderOfChip(unsigned int chip) const {
    return chip;
  }

  virtual std::pair<double, double> pixelSize(LHCb::VPChannelID channel) const;
  virtual bool isLong(LHCb::VPChannelID channel) const;

private:
  
  /// Update geometry cache when the alignment changes
  StatusCode updateGeometryCache();

  /// Sensor thickness
  double m_thickness;
  /// Dimensions of sensor active area
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
  // Index of the first chip
  unsigned int m_chip;

  bool m_debug;

  // Message stream
  mutable MsgStream* m_msg;
  MsgStream& msg() const {
    if (!m_msg) m_msg = new MsgStream(msgSvc(), "DeVPNewSensor");
    return *m_msg;
  }

};

#endif 
