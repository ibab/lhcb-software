// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// LHCb
// Kernel/LHCbMath
#include "LHCbMath/LHCbMath.h"
// Kernel/LHCbKernel
#include "Kernel/LineTraj.h"
#include "Kernel/VPChannelID.h"

// Local 
#include "VPDet/DeVPNewSensor.h"

/** @file DeVPNewSensor.cpp
 *
 *  Implementation of class : DeVPNewSensor
 *
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVPNewSensor::DeVPNewSensor(const std::string& name) : 
    DeVPSensor(name),
    m_debug(false),
    m_msg(NULL) {

}

//==============================================================================
/// Destructor
//==============================================================================
DeVPNewSensor::~DeVPNewSensor() {

  if (m_msg) delete m_msg;

}

//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVPNewSensor::clID() const {

  return DeVPNewSensor::classID();

}

//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVPNewSensor::initialize()  {

  // Set the output level.
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel = 0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if (sc.isSuccess()) sc = jobSvc->setMyProperties("DeVPNewSensor", pmgr);
  if (outputLevel > 0) msgSvc()->setOutputLevel("DeVPNewSensor", outputLevel);
  delete pmgr;
  if (!sc) return sc;
  m_debug = (msgSvc()->outputLevel("DeVPNewSensor") == MSG::DEBUG);

  sc = DeVPSensor::initialize();
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to initialise DeVPSensor" << endmsg;
    return sc;
  }

  // Get parameters from XML.
  m_thickness = param<double>("Thickness");
  m_nChips = param<int>("NChips");
  m_chipSize = param<double>("ChipSize");
  m_interChipDist = param<double>("InterChipDist");
  m_nCols = param<int>("NColumns");
  m_nRows = param<int>("NRows");
  m_pixelSize = param<double>("PixelSize");
  m_interChipPixelSize = param<double>("InterChipPixelSize");

  // Calculate the active area
  m_sizeX = m_nChips * m_chipSize + (m_nChips - 1) * m_interChipDist;
  m_sizeY = m_chipSize;
  // Calculate the index of the first chip (assuming four sensors per module).
  m_chip = (sensorNumber() - module() * 4) * m_nChips; 

  // Register geometry conditions.
  updMgrSvc()->registerCondition(this, this->m_geometry, 
                                 &DeVPNewSensor::updateGeometryCache);
  sc = updMgrSvc()->update(this);
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to update geometry cache." << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS; 

}

//==============================================================================
/// Return a trajectory (for track fit) from pixel + offset
//==============================================================================
std::auto_ptr<LHCb::Trajectory> DeVPNewSensor::trajectory(const LHCb::VPChannelID& channel,
                                                          const std::pair<double, double> offset) const {

  Gaudi::XYZPoint point = channelToPoint(channel, offset);
  LHCb::Trajectory* traj= new LHCb::LineTraj(point, point);
  std::auto_ptr<LHCb::Trajectory> autoTraj(traj);
  return autoTraj;

}

//==============================================================================
/// Calculate the nearest pixel to a point in the global frame. 
//==============================================================================
StatusCode DeVPNewSensor::pointToChannel(const Gaudi::XYZPoint& point,
                                         const bool local,
                                         LHCb::VPChannelID& channel) const {

  Gaudi::XYZPoint localPoint = local ? point : globalToLocal(point);
  // Check if the point is in the active area of the sensor.
  StatusCode sc = isInActiveArea(localPoint);
  if (!sc.isSuccess()) return sc;
  // Set the module number.
  channel.setModule(module());
  double x0 = 0.;
  for (unsigned int i = 0; i < m_nChips; ++i) {
    const double x = localPoint.x() - x0; 
    if (x < m_chipSize + 0.5 * m_interChipDist) {
      // Set the chip number.
      channel.setChip(m_chip + i);
      // Set the row and column.
      unsigned int col = 0;
      unsigned int row = 0;
      if (x > 0.) {
        col = int(x / m_pixelSize);
        if (col >= m_nCols) col = m_nCols - 1;
      }
      if (localPoint.y() > 0.) {
        row = int(localPoint.y() / m_pixelSize);
        if (row >= m_nRows) row = m_nRows - 1;
      }
      channel.setCol(col);
      channel.setRow(row);
      break; 
    }
    x0 += m_chipSize + m_interChipDist;
  }
  return StatusCode::SUCCESS;

}

//==============================================================================
/// Calculate the pixel and fraction corresponding to a global point.
//==============================================================================
StatusCode DeVPNewSensor::pointToChannel(const Gaudi::XYZPoint& point,
                                         const bool local,
                                         LHCb::VPChannelID& channel,
                                         std::pair<double, double>& fraction) const {
  
  Gaudi::XYZPoint localPoint = local ? point : globalToLocal(point);
  // Check if the point is in the active area of the sensor.
  StatusCode sc = isInActiveArea(localPoint);
  if (!sc.isSuccess()) return sc;
  fraction.first = 0.;
  fraction.second = 0.;
  // Set the module number.
  channel.setModule(module());
  const double step = m_chipSize + m_interChipDist;
  double x0 = 0.;
  for (unsigned int i = 0; i < m_nChips; ++i) {
    if (localPoint.x() < x0 + step) {
      // Set the chip number.
      channel.setChip(m_chip + i);
      // Calculate the column number.
      const double x = localPoint.x() - x0;
      const double fcol = x / m_pixelSize - 0.5;
      const unsigned int icol = fcol > 0. ? int(fcol) : 0;
      // Set column and inter-pixel fraction.
      if (icol <= 0) {
        channel.setCol(0);
        if (0 == i) {
          if (fcol > 0.) fraction.first = fcol;
        } else {
          // First column has elongated pixels.
          const double pitch = 0.5 * (m_pixelSize + m_interChipPixelSize);
          fraction.first = x / pitch;
        }
      } else if (icol >= m_nCols - 1) {
        channel.setCol(m_nCols - 1);
        if (i == m_nChips - 1) {
          fraction.first = fcol - icol;
        } else {
          // Last column has elongated pixels.
          if (x < m_chipSize) {
            // This point is assigned to the last but one pixel.
            channel.setCol(m_nCols - 2);
            const double pitch = 0.5 * (m_pixelSize + m_interChipPixelSize); 
            fraction.first = 1. - (m_chipSize - x) / pitch;
          } else {
            // Point is in inter-chip region.
            fraction.first = (x - m_chipSize) / m_interChipPixelSize; 
          }
        } 
      } else {
        channel.setCol(icol);
        fraction.first = fcol - icol;
        if (icol == m_nCols - 2 && i < m_nChips - 1) {
          fraction.first *= m_pixelSize / m_interChipPixelSize;
        } 
      } 
      // Set the row and inter-pixel fraction.
      const double frow = localPoint.y() / m_pixelSize - 0.5;
      const unsigned int irow = frow > 0. ? int(frow) : 0;
      if (irow <= 0) {
        channel.setRow(0);
        if (frow > 0.) fraction.second = frow;
      } else { 
        channel.setRow(irow);
        fraction.second = frow - irow;
      }
      break; 
    }
    x0 += step;
  }
  return StatusCode::SUCCESS;

}

//==============================================================================
/// Calculate the position of a pixel with given channel ID.
//==============================================================================
Gaudi::XYZPoint DeVPNewSensor::channelToPoint(const LHCb::VPChannelID& channel,
                                              const bool local) const {

  const unsigned int chip = channel.chip() % m_nChips;
  const unsigned int col = channel.col();
  const unsigned int row = channel.row();
  const double x0 = chip * (m_chipSize + m_interChipDist);
  double x = x0 + (col + 0.5) * m_pixelSize;
  if (col == 0 && chip > 0) {
    x -= 0.5 * (m_interChipPixelSize - m_pixelSize);
  } else if (col == m_nCols - 1 && chip < m_nChips - 1) {
    x += 0.5 * (m_interChipPixelSize - m_pixelSize);
  }
  const double y = (row + 0.5) * m_pixelSize;
  Gaudi::XYZPoint point(x, y, 0.); 
  if (local) return point;
  return localToGlobal(point);

}

//==============================================================================
/// Calculate the position of a pixel with given channel ID and offset. 
//==============================================================================
Gaudi::XYZPoint DeVPNewSensor::channelToPoint(const LHCb::VPChannelID& channel,
                                              std::pair<double, double> fraction) const {

  const unsigned int chip = channel.chip() % m_nChips;
  const unsigned int col = channel.col();
  const unsigned int row = channel.row();
  const double x0 = chip * (m_chipSize + m_interChipDist);
  // Calculate the x-coordinate of the pixel centre.
  double x = x0 + (col + 0.5) * m_pixelSize;
  if (col == 0 && chip > 0) {
    x -= 0.5 * (m_interChipPixelSize - m_pixelSize);
  } else if (col == m_nCols - 1 && chip < m_nChips - 1) {
    x += 0.5 * (m_interChipPixelSize - m_pixelSize);
  }
  // Calculate the pitch in the column direction.
  double pitch = m_pixelSize;
  if (chip > 0 && col == 0) {
    pitch = 0.5 * (m_interChipPixelSize + m_pixelSize);
  } else if (chip < m_nChips - 1 && col == m_nCols - 1) {
    pitch = m_interChipPixelSize;
  } else if (chip < m_nChips - 1 && col == m_nCols - 2) {
    pitch = 0.5 * (m_interChipPixelSize + m_pixelSize); 
  }
  // Correct the x-coordinate based on the inter-pixel fraction.
  x += fraction.first * pitch;
  // Calculate the y-coordinate.
  double y = (row + 0.5 + fraction.second) * m_pixelSize;
  Gaudi::XYZPoint point(x, y, 0.); 
  return localToGlobal(point);

}

//==============================================================================
/// Calculate the nearest 3x3 list of channel to a 3-d point 
//==============================================================================
StatusCode DeVPNewSensor::pointTo3x3Channels(const Gaudi::XYZPoint& point,
                                             std::vector<LHCb::VPChannelID>& channels) const {

  // Get the channel corresponding to the central point 
  LHCb::VPChannelID centralChannel;
  std::pair<double, double> fraction;
  StatusCode sc = pointToChannel(point, false, centralChannel, fraction);
  if (!sc.isSuccess()) return sc;
  channels.clear();
  sc = channelToNeighbours(centralChannel, channels);
  if (!sc.isSuccess()) return sc;
  channels.push_back(centralChannel);
  return StatusCode::SUCCESS;

}

//==============================================================================
/// Get the 8 channels (if they exist) arround a given seed channel
//==============================================================================
StatusCode DeVPNewSensor::channelToNeighbours(const LHCb::VPChannelID& channel,
                                              std::vector <LHCb::VPChannelID>& neighbours) const {

  neighbours.clear();
  const unsigned int chip = channel.chip();
  const unsigned int col = channel.col();
  const unsigned int row = channel.row();
  if (row > 0) {
    LHCb::VPChannelID neighbour(channel);
    neighbour.setRow(row - 1);
    neighbours.push_back(neighbour);
  }
  if (row < m_nRows - 1) {
    LHCb::VPChannelID neighbour(channel);
    neighbour.setRow(row + 1);
    neighbours.push_back(neighbour);
  }
  if (col > 0) {
    LHCb::VPChannelID neighbour(channel);
    neighbour.setCol(col - 1);
    neighbours.push_back(neighbour);
    if (row > 0) {
      neighbour.setRow(row - 1);
      neighbours.push_back(neighbour);
    }
    if (row < m_nRows - 1) {
      neighbour.setRow(row + 1);
      neighbours.push_back(neighbour);
    }
  } else if (chip % m_nChips > 0) {
    LHCb::VPChannelID neighbour(channel);
    neighbour.setChip(chip - 1);
    neighbour.setCol(m_nCols - 1);
    neighbours.push_back(neighbour);
    if (row > 0) {
      neighbour.setRow(row - 1);
      neighbours.push_back(neighbour);
    }
    if (row < m_nRows - 1) {
      neighbour.setRow(row + 1);
      neighbours.push_back(neighbour);
    }
  } 
  if (col < m_nCols - 1) {
    LHCb::VPChannelID neighbour(channel);
    neighbour.setCol(col + 1);
    neighbours.push_back(neighbour);
    if (row > 0) {
      neighbour.setRow(row - 1);
      neighbours.push_back(neighbour);
    }
    if (row < m_nRows - 1) {
      neighbour.setRow(row + 1);
      neighbours.push_back(neighbour);
    }
  } else if (chip % m_nChips < m_nChips - 1) {
    LHCb::VPChannelID neighbour(channel);
    neighbour.setChip(chip + 1);
    neighbour.setCol(0);
    neighbours.push_back(neighbour);
    if (row > 0) {
      neighbour.setRow(row - 1);
      neighbours.push_back(neighbour);
    }
    if (row < m_nRows - 1) {
      neighbour.setRow(row + 1);
      neighbours.push_back(neighbour);
    }
  }
  return StatusCode::SUCCESS;

}

//==============================================================================
/// Check if a local point is inside the active area of the sensor. 
//==============================================================================
StatusCode DeVPNewSensor::isInActiveArea(const Gaudi::XYZPoint& point) const {
 
  if (point.x() < 0. || point.x() > m_sizeX) return StatusCode::FAILURE;
  if (point.y() < 0. || point.y() > m_sizeY) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;

}

//==============================================================================
/// Return the size of a pixel with given channel ID.
//==============================================================================
std::pair<double, double> DeVPNewSensor::pixelSize(LHCb::VPChannelID channel) const {

  if (isLong(channel)) {
    return std::make_pair(m_interChipPixelSize, m_pixelSize);
  } 
  return std::make_pair(m_pixelSize, m_pixelSize);

}

//==============================================================================
/// Return true if a pixel with given channel ID is a long pixel. 
//==============================================================================
bool DeVPNewSensor::isLong(LHCb::VPChannelID channel) const {

  const unsigned int chip = channel.chip() % m_nChips;
  const unsigned int col = channel.col();
  if ((col == 0 && chip > 0) || (col == m_nCols - 1 && chip < m_nChips - 1)) {
    return true;
  } 
  return false;

}

//==============================================================================
/// Update the geometry cache 
//==============================================================================
StatusCode DeVPNewSensor::updateGeometryCache() {

  // TODO!
  return StatusCode::SUCCESS;

}

