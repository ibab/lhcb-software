// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

// LHCb
// Det/DetDesc
#include "DetDesc/Condition.h"
// Kernel/LHCbKernel
#include "Kernel/LineTraj.h"
#include "Kernel/VPChannelID.h"

// Local 
#include "VPDet/DeVPSensor.h"

/** @file DeVPSensor.cpp
 *
 *  Implementation of class DeVPSensor
 *
 *  @author Victor Coco victor.coco@cern.ch
 *  
 */

// Dimensions of the sensor active area
double DeVPSensor::m_sizeX;
double DeVPSensor::m_sizeY;
double DeVPSensor::m_thickness;
// Number of chips per ladder
unsigned int DeVPSensor::m_nChips;
// Length of chip active area
double DeVPSensor::m_chipSize;
// Distance between two chips
double DeVPSensor::m_interChipDist;
// Number of columns and rows
unsigned int DeVPSensor::m_nCols;
unsigned int DeVPSensor::m_nRows;
// Cell size of pixels
double DeVPSensor::m_pixelSize;
// Cell size in column direction of elongated pixels
double DeVPSensor::m_interChipPixelSize;

// local coordinate cache, once for all sensors
double DeVPSensor::m_local_x[768];
// pitch cache, once for all sensors
double DeVPSensor::m_x_pitch[768];
// common configuration validity
bool DeVPSensor::m_common_cache_valid = false;

//==============================================================================
/// Standard constructor
//==============================================================================
DeVPSensor::DeVPSensor(const std::string& name) : 
    DetectorElement(name),
    m_geometry(NULL), 
    m_msg(NULL) {
 ;
}

//==============================================================================
/// Destructor
//==============================================================================
DeVPSensor::~DeVPSensor() {

  if (m_msg) delete m_msg;

}

//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVPSensor::clID() const { 
  return DeVPSensor::classID(); 
}

//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVPSensor::initialize() {

  // Set the output level.
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel = 0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if (sc.isSuccess()) sc = jobSvc->setMyProperties("DeVPSensor", pmgr);
  if (outputLevel > 0) {
    msgSvc()->setOutputLevel("DeVPSensor", outputLevel);
  }
  delete pmgr;
  if (!sc) return sc;
  m_debug = false;
  if ((msgSvc()->outputLevel("DeVPSensor") == MSG::DEBUG) ||
      (msgSvc()->outputLevel("DeVPSensor") == MSG::VERBOSE)) {
    m_debug = true;
  }

  sc = DetectorElement::initialize();
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Cannot initialise DetectorElement" << endmsg;
    return sc;
  }
  // Get the information we need from the DDDB.
  // Some of these parameters are commong to all sensors
  // and will be stored in statics.

  // sensor specifics
  //
  m_sensorNumber = param<int>("SensorNumber");
  m_module = param<int>("Module");
  std::string side = param<std::string>("Side");
  m_isLeft = side.find("Left") == 0;
  m_isDownstream = 0 != param<int>("Downstream");

  // Calculate the index of the first chip (assuming four sensors per module).
  m_chip = (sensorNumber() - module() * 4) * DeVPSensor::m_nChips;


  // commons
  //
  if (!DeVPSensor::m_common_cache_valid) {
    DeVPSensor::m_common_cache_valid = true; 

    DeVPSensor::m_thickness = param<double>("Thickness");
    DeVPSensor::m_nChips = param<int>("NChips"); 
    DeVPSensor::m_chipSize = param<double>("ChipSize");
    DeVPSensor::m_interChipDist = param<double>("InterChipDist");
    DeVPSensor::m_nCols = param<int>("NColumns");
    DeVPSensor::m_nRows = param<int>("NRows");
    DeVPSensor::m_pixelSize = param<double>("PixelSize");
    DeVPSensor::m_interChipPixelSize = param<double>("InterChipPixelSize");

    // Calculate the active area.
    DeVPSensor::m_sizeX = DeVPSensor::m_nChips * DeVPSensor::m_chipSize + (DeVPSensor::m_nChips - 1) * DeVPSensor::m_interChipDist;
    DeVPSensor::m_sizeY = DeVPSensor::m_chipSize;

    // create the local cache if no-one else did it yet
    cacheLocalXAndPitch();
  }

  m_geometry = geometry();
  // Register geometry conditions.
  updMgrSvc()->registerCondition(this, this->m_geometry,
                                 &DeVPSensor::updateGeometryCache);
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
std::auto_ptr<LHCb::Trajectory> DeVPSensor::trajectory(const LHCb::VPChannelID& channel,
                                                          const std::pair<double, double> offset) const {

  Gaudi::XYZPoint point = channelToPoint(channel, offset);
  LHCb::Trajectory* traj= new LHCb::LineTraj(point, point);
  std::auto_ptr<LHCb::Trajectory> autoTraj(traj);
  return autoTraj;

}

//==============================================================================
/// Calculate the nearest pixel to a point in the global frame. 
//==============================================================================
StatusCode DeVPSensor::pointToChannel(const Gaudi::XYZPoint& point,
                                         const bool local,
                                         LHCb::VPChannelID& channel) const {

  Gaudi::XYZPoint localPoint = local ? point : globalToLocal(point);
  // Check if the point is in the active area of the sensor.
  StatusCode sc = isInActiveArea(localPoint);
  if (!sc.isSuccess()) return sc;
  // Set the module number.
  channel.setModule(module());
  double x0 = 0.;
  for (unsigned int i = 0; i < DeVPSensor::m_nChips; ++i) {
    const double x = localPoint.x() - x0;
    if (x < DeVPSensor::m_chipSize + 0.5 * DeVPSensor::m_interChipDist) {
      // Set the chip number.
      channel.setChip(m_chip + i);
      // Set the row and column.
      unsigned int col = 0;
      unsigned int row = 0;
      if (x > 0.) {
        col = int(x / DeVPSensor::m_pixelSize);
        if (col >= DeVPSensor::m_nCols) col = DeVPSensor::m_nCols - 1;
      }
      if (localPoint.y() > 0.) {
        row = int(localPoint.y() / DeVPSensor::m_pixelSize);
        if (row >= DeVPSensor::m_nRows) row = DeVPSensor::m_nRows - 1;
      }
      channel.setCol(col);
      channel.setRow(row);
      break;
    }
    x0 += DeVPSensor::m_chipSize + DeVPSensor::m_interChipDist;
  }
  return StatusCode::SUCCESS;

}

//==============================================================================
/// Calculate the pixel and fraction corresponding to a global point.
//==============================================================================
StatusCode DeVPSensor::pointToChannel(const Gaudi::XYZPoint& point,
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
  const double step = DeVPSensor::m_chipSize + DeVPSensor::m_interChipDist;
  double x0 = 0.;
  for (unsigned int i = 0; i < DeVPSensor::m_nChips; ++i) {
    if (localPoint.x() < x0 + step) {
      // Set the chip number.
      channel.setChip(m_chip + i);
      // Calculate the column number.
      const double x = localPoint.x() - x0;
      const double fcol = x / DeVPSensor::m_pixelSize - 0.5;
      const unsigned int icol = fcol > 0. ? int(fcol) : 0;
      // Set column and inter-pixel fraction.
      if (icol <= 0) {
        channel.setCol(0);
        if (0 == i) {
          if (fcol > 0.) fraction.first = fcol;
        } else {
          // First column has elongated pixels.
          const double pitch = 0.5 * (DeVPSensor::m_pixelSize + DeVPSensor::m_interChipPixelSize);
          fraction.first = x / pitch;
        }
      } else if (icol >= DeVPSensor::m_nCols - 1) {
        channel.setCol(DeVPSensor::m_nCols - 1);
        if (i == DeVPSensor::m_nChips - 1) {
          fraction.first = fcol - icol;
        } else {
          // Last column has elongated pixels.
          if (x < DeVPSensor::m_chipSize) {
            // This point is assigned to the last but one pixel.
            channel.setCol(DeVPSensor::m_nCols - 2);
            const double pitch = 0.5 * (DeVPSensor::m_pixelSize + DeVPSensor::m_interChipPixelSize);
            fraction.first = 1. - (DeVPSensor::m_chipSize - x) / pitch;
          } else {
            // Point is in inter-chip region.
            fraction.first = (x - DeVPSensor::m_chipSize) / DeVPSensor::m_interChipPixelSize;
          }
        }
      } else {
        channel.setCol(icol);
        fraction.first = fcol - icol;
        if (icol == DeVPSensor::m_nCols - 2 && i < DeVPSensor::m_nChips - 1) {
          fraction.first *= DeVPSensor::m_pixelSize / DeVPSensor::m_interChipPixelSize;
        }
      }
      // Set the row and inter-pixel fraction.
      const double frow = localPoint.y() / DeVPSensor::m_pixelSize - 0.5;
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
//Gaudi::XYZPoint DeVPSensor::channelToPoint(const LHCb::VPChannelID& channel,
//                                              const bool local) const {
//
//  const unsigned int chip = channel.chip() % DeVPSensor::m_nChips;
//  const unsigned int col = channel.col() + chip*256;
//  const unsigned int row = channel.row();
//  const double x = DeVPSensor::m_local_x[col];
//  const double y = (row + 0.5) * DeVPSensor::m_pixelSize; 
//  const Gaudi::XYZPoint point(x, y, 0.0);
//  return ( local ? point : localToGlobal(point) );
//
//  // Calculate the x-coordinate of the pixel centre and the pitch.
//  //const double x0 = chip * (DeVPSensor::m_chipSize + DeVPSensor::m_interChipDist);
//  //double x = x0 + (col + 0.5) * DeVPSensor::m_pixelSize;
//  //switch (col + chip*256) {
//  //  case 256: case 512: // right of chip border
//  //    x -= 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
//  //    break;
//  //  case 255: case 511: // left of chip border
//  //    x += 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
//  //    break;
//  //}
//
//  //const Gaudi::XYZPoint point(x, y, 0.0);
//  //return ( local ? point : localToGlobal(point) );
//
//  //const unsigned int col = channel.col() + (channel.chip() % DeVPSensor::m_nChips)*256;
//  //const unsigned int row = channel.row();
//  //const double x = DeVPSensor::m_local_x[col];
//  //const double y = (row + 0.5) * DeVPSensor::m_pixelSize; 
//  //const Gaudi::XYZPoint point(x, y, 0.0);
//
//  //const unsigned int chip = channel.chip() % DeVPSensor::m_nChips;
//  //const unsigned int col = channel.col();
//  //const unsigned int row = channel.row();
//  //const double x0 = chip * (DeVPSensor::m_chipSize + DeVPSensor::m_interChipDist);
//  //double x = x0 + (col + 0.5) * DeVPSensor::m_pixelSize;
//  //if (col == 0 && chip > 0) {
//  //  x -= 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
//  //} else if (col == DeVPSensor::m_nCols - 1 && chip < DeVPSensor::m_nChips - 1) {
//  //  x += 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
//  //}
//  //const double y = (row + 0.5) * DeVPSensor::m_pixelSize;
//  //Gaudi::XYZPoint point(x, y, 0.);
//  //if (local) return point;
//  //return localToGlobal(point);
//
//}

//==============================================================================
/// Calculate the position of a pixel with given channel ID and offset. 
//==============================================================================
//Gaudi::XYZPoint DeVPSensor::channelToPoint(const LHCb::VPChannelID& channel,
//                                              std::pair<double, double> fraction) const {
//
//  const unsigned int chip = channel.chip() % DeVPSensor::m_nChips;
//  const unsigned int col = channel.col() + chip*256;
//  const unsigned int row = channel.row();
//  const double pitch = DeVPSensor::m_x_pitch[col];
//  const double x = DeVPSensor::m_local_x[col] + fraction.first * pitch;
//  const double y = (row + 0.5 + fraction.second) * DeVPSensor::m_pixelSize; 
//  const Gaudi::XYZPoint point(x, y, 0.0);
//  return localToGlobal(point);
//
//  // Calculate the x-coordinate of the pixel centre and the pitch.
//  //const double x0 = chip * (DeVPSensor::m_chipSize + DeVPSensor::m_interChipDist);
//  //double x = x0 + (col + 0.5) * DeVPSensor::m_pixelSize;
//  //double pitch = DeVPSensor::m_pixelSize;
//  //switch (col + chip*256) {
//  //  case 256: case 512: // right of chip border
//  //    x -= 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
//  //    pitch = 0.5 * (DeVPSensor::m_interChipPixelSize + DeVPSensor::m_pixelSize);
//  //    break;
//  //  case 255: case 511: // left of chip border
//  //    x += 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
//  //    pitch = DeVPSensor::m_interChipPixelSize;
//  //    break;
//  //  case 254: case 510: // two left of chip border
//  //    pitch = 0.5 * (DeVPSensor::m_interChipPixelSize + DeVPSensor::m_pixelSize);
//  //    break;
//  //}
//  //x += fraction.first * pitch;
//
//  //const Gaudi::XYZPoint point(x, y, 0.0);
//  //return localToGlobal(point);
//  
//  //const unsigned int chip = channel.chip() % DeVPSensor::m_nChips;
//  //const unsigned int col = channel.col();
//  //const unsigned int row = channel.row();
//  //const double x0 = chip * (DeVPSensor::m_chipSize + DeVPSensor::m_interChipDist);
//  //// Calculate the x-coordinate of the pixel centre.
//  //double x = x0 + (col + 0.5) * DeVPSensor::m_pixelSize;
//  //if (col == 0 && chip > 0) {
//  //  x -= 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
//  //} else if (col == DeVPSensor::m_nCols - 1 && chip < DeVPSensor::m_nChips - 1) {
//  //  x += 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
//  //}
//  //// Calculate the pitch in the column direction.
//  //double pitch = DeVPSensor::m_pixelSize;
//  //if (chip > 0 && col == 0) {
//  //  pitch = 0.5 * (DeVPSensor::m_interChipPixelSize + DeVPSensor::m_pixelSize);
//  //} else if (chip < DeVPSensor::m_nChips - 1 && col == DeVPSensor::m_nCols - 1) {
//  //  pitch = DeVPSensor::m_interChipPixelSize;
//  //} else if (chip < DeVPSensor::m_nChips - 1 && col == DeVPSensor::m_nCols - 2) {
//  //  pitch = 0.5 * (DeVPSensor::m_interChipPixelSize + DeVPSensor::m_pixelSize);
//  //}
//  //// Correct the x-coordinate based on the inter-pixel fraction.
//  //x += fraction.first * pitch;
//  //// Calculate the y-coordinate.
//  //double y = (row + 0.5 + fraction.second) * DeVPSensor::m_pixelSize;
//  //Gaudi::XYZPoint point(x, y, 0.);
//  //return localToGlobal(point);
//
//}

//==============================================================================
/// Get the 8 channels (if they exist) arround a given seed channel
//==============================================================================
StatusCode DeVPSensor::channelToNeighbours(const LHCb::VPChannelID& channel,
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
  if (row < DeVPSensor::m_nRows - 1) {
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
    if (row < DeVPSensor::m_nRows - 1) {
      neighbour.setRow(row + 1);
      neighbours.push_back(neighbour);
    }
  } else if (chip % DeVPSensor::m_nChips > 0) {
    LHCb::VPChannelID neighbour(channel);
    neighbour.setChip(chip - 1);
    neighbour.setCol(DeVPSensor::m_nCols - 1);
    neighbours.push_back(neighbour);
    if (row > 0) {
      neighbour.setRow(row - 1);
      neighbours.push_back(neighbour);
    }
    if (row < DeVPSensor::m_nRows - 1) {
      neighbour.setRow(row + 1);
      neighbours.push_back(neighbour);
    }
  }
  if (col < DeVPSensor::m_nCols - 1) {
    LHCb::VPChannelID neighbour(channel);
    neighbour.setCol(col + 1);
    neighbours.push_back(neighbour);
    if (row > 0) {
      neighbour.setRow(row - 1);
      neighbours.push_back(neighbour);
    }
    if (row < DeVPSensor::m_nRows - 1) {
      neighbour.setRow(row + 1);
      neighbours.push_back(neighbour);
    }
  } else if (chip % DeVPSensor::m_nChips < DeVPSensor::m_nChips - 1) {
    LHCb::VPChannelID neighbour(channel);
    neighbour.setChip(chip + 1);
    neighbour.setCol(0);
    neighbours.push_back(neighbour);
    if (row > 0) {
      neighbour.setRow(row - 1);
      neighbours.push_back(neighbour);
    }
    if (row < DeVPSensor::m_nRows - 1) {
      neighbour.setRow(row + 1);
      neighbours.push_back(neighbour);
    }
  }
  return StatusCode::SUCCESS;

}

//==============================================================================
/// Check if a local point is inside the active area of the sensor. 
//==============================================================================
StatusCode DeVPSensor::isInActiveArea(const Gaudi::XYZPoint& point) const {

  if (point.x() < 0. || point.x() > DeVPSensor::m_sizeX) return StatusCode::FAILURE;
  if (point.y() < 0. || point.y() > DeVPSensor::m_sizeY) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;

}

//==============================================================================
/// Return the size of a pixel with given channel ID.
//==============================================================================
std::pair<double, double> DeVPSensor::pixelSize(LHCb::VPChannelID channel) const {

  if (isLong(channel)) {
    return std::make_pair(DeVPSensor::m_interChipPixelSize, DeVPSensor::m_pixelSize);
  }
  return std::make_pair(DeVPSensor::m_pixelSize, DeVPSensor::m_pixelSize);

}

//==============================================================================
/// Return true if a pixel with given channel ID is an elongated pixel. 
//==============================================================================
bool DeVPSensor::isLong(LHCb::VPChannelID channel) const {

  const unsigned int chip = channel.chip() % DeVPSensor::m_nChips;
  const unsigned int col = channel.col();
  if ((col == 0 && chip > 0) || (col == DeVPSensor::m_nCols - 1 && chip < DeVPSensor::m_nChips - 1)) {
    return true;
  }
  return false;

}

//==============================================================================
/// Calculate and cache the local x positions an pitches
//==============================================================================
void DeVPSensor::cacheLocalXAndPitch(void) {

  for (int col=0; col<768; ++col) {
    // Calculate the x-coordinate of the pixel centre and the pitch.
    const double x0 = (col/256) * (DeVPSensor::m_chipSize + DeVPSensor::m_interChipDist);
    double x = x0 + (col%256 + 0.5) * DeVPSensor::m_pixelSize;
    double pitch = DeVPSensor::m_pixelSize;
    switch (col) {
      case 256: case 512: // right of chip border
        x -= 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
        pitch = 0.5 * (DeVPSensor::m_interChipPixelSize + DeVPSensor::m_pixelSize);
        break;
      case 255: case 511: // left of chip border
        x += 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
        pitch = DeVPSensor::m_interChipPixelSize;
        break;
      case 254: case 510: // two left of chip border
        pitch = 0.5 * (DeVPSensor::m_interChipPixelSize + DeVPSensor::m_pixelSize);
        break;
    }
    DeVPSensor::m_local_x[col] = x;
    DeVPSensor::m_x_pitch[col] = pitch;
  }

  return;
}

//==============================================================================
/// Cache geometry parameters
//==============================================================================
StatusCode DeVPSensor::updateGeometryCache() {

  m_z = m_geometry->toGlobal(Gaudi::XYZPoint(0, 0, 0)).z();
  return StatusCode::SUCCESS;

}
