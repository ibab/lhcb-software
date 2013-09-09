// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// LHCb
#include "LHCbMath/LHCbMath.h"
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

  delete m_msg;

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
  // Calculate the chip index.
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
                                         LHCb::VPChannelID& channel,
                                         std::pair<double, double>& fraction) const {
  
  Gaudi::XYZPoint localPoint = globalToLocal(point);
  // Check that the point is in the active area of the sensor
  StatusCode sc = isInActiveArea(localPoint);
  if (!sc.isSuccess()) return sc;
  // Set the module number.
  channel.setModule(module());
  double x0 = 0.;
  const double step = m_chipSize + 0.5 * m_interChipDist;
  for (unsigned int i = 0; i < m_nChips; ++i) {
    if (localPoint.x() < x0 + step) {
      // Set the chip number.
      channel.setChip(m_chip + i);
      int col = int((localPoint.x() - x0) / m_pixelSize);
      int row = int(localPoint.y() / m_pixelSize);
      if (col < 0) {
        col = 0;
      } else if (col >= m_nCols) {
        col = m_nCols - 1;
      }
      if (row < 0) {
        row = 0;
      } else if (row >= m_nRows) {
        row = m_nRows - 1;
      }
      channel.setCol(col);
      channel.setRow(row); 
    }
    x0 += step;
  }
  return StatusCode::SUCCESS;

}

//==============================================================================
/// Calculate the position of a pixel with given channel ID.
//==============================================================================
Gaudi::XYZPoint DeVPNewSensor::channelToPoint(const LHCb::VPChannelID& channel) const {

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
  double x = x0 + (col + 0.5) * m_pixelSize;
  if (col == 0 && chip > 0) {
    x -= 0.5 * (m_interChipPixelSize - m_pixelSize);
    x += fraction.first * m_interChipPixelSize / 2.;
  } else if (col == m_nCols - 1 && chip < m_nChips - 1) {
    x += 0.5 * (m_interChipPixelSize - m_pixelSize);
    x += fraction.first * m_interChipPixelSize / 2.;
  } else {
    x += fraction.first * m_pixelSize / 2.;
  }
  double y = (row + 0.5) * m_pixelSize;
  y += fraction.second * m_pixelSize / 2.; 
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
  StatusCode sc = pointToChannel(point, centralChannel, fraction);
  if (!sc.isSuccess()) return sc;

  channels.clear();
  channels.push_back(centralChannel);
  return StatusCode::SUCCESS;

}

//==============================================================================
/// Get the 8 channels (if they exist) arround a given seed channel
//==============================================================================
StatusCode DeVPNewSensor::channelToNeighbours(const LHCb::VPChannelID& channel,
                                              std::vector <LHCb::VPChannelID>& neighbours) const {

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

