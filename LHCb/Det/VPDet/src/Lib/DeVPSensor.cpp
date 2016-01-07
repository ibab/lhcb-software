// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// LHCb
// Det/DetDesc
#include "DetDesc/Condition.h"

// Local
#include "VPDet/DeVPSensor.h"

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
double DeVPSensor::m_local_x[VP::NSensorColumns];
// pitch cache, once for all sensors
double DeVPSensor::m_x_pitch[VP::NSensorColumns];
// common configuration validity
bool DeVPSensor::m_common_cache_valid = false;

//==============================================================================
// Standard constructor
//==============================================================================
DeVPSensor::DeVPSensor(const std::string& name)
    : DetectorElement(name), m_geometry(nullptr) {

}

//==============================================================================
// Destructor
//==============================================================================
DeVPSensor::~DeVPSensor() = default;

//==============================================================================
// Object identification
//==============================================================================
const CLID& DeVPSensor::clID() const {

  return DeVPSensor::classID();

}

//==============================================================================
// Initialisation method
//==============================================================================
StatusCode DeVPSensor::initialize() {

  // Set the output level.
  std::unique_ptr<PropertyMgr> pmgr{ new PropertyMgr() };
  int outputLevel = 0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  auto jobSvc = svcLoc->service<IJobOptionsSvc>("JobOptionsSvc");
  if (jobSvc) {
      auto sc = jobSvc->setMyProperties("DeVPSensor", pmgr.get());
      if (!sc) return sc;
  }
  if (outputLevel > 0) {
    msgSvc()->setOutputLevel("DeVPSensor", outputLevel);
  }
  m_debug = (msgSvc()->outputLevel("DeVPSensor") == MSG::DEBUG ||
             msgSvc()->outputLevel("DeVPSensor") == MSG::VERBOSE);

  auto sc = DetectorElement::initialize();
  if (!sc.isSuccess()) {
    msg() << MSG::ERROR << "Cannot initialise DetectorElement" << endmsg;
    return sc;
  }
  // Get the information we need from the DDDB.
  // Some of these parameters are common to all sensors
  // and will be stored in statics.

  // sensor specifics
  //
  m_sensorNumber = param<int>("SensorNumber");
  m_module = param<int>("Module");
  std::string side = param<std::string>("Side");
  m_isLeft = side.find("Left") == 0;

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
    DeVPSensor::m_sizeX =
        DeVPSensor::m_nChips * DeVPSensor::m_chipSize +
        (DeVPSensor::m_nChips - 1) * DeVPSensor::m_interChipDist;
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
// Calculate the nearest pixel to a point in the global frame.
//==============================================================================
bool DeVPSensor::pointToChannel(const Gaudi::XYZPoint& point,
                                const bool local,
                                LHCb::VPChannelID& channel) const {

  Gaudi::XYZPoint localPoint = local ? point : globalToLocal(point);
  // Check if the point is in the active area of the sensor.
  if (!isInActiveArea(localPoint)) return false;
  // Set the sensor number.
  channel.setSensor(m_sensorNumber);
  double x0 = 0.;
  for (unsigned int i = 0; i < DeVPSensor::m_nChips; ++i) {
    const double x = localPoint.x() - x0;
    if (x < DeVPSensor::m_chipSize + 0.5 * DeVPSensor::m_interChipDist) {
      // Set the chip number.
      channel.setChip(i);
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
  return true;

}

//==============================================================================
// Calculate the pixel and fraction corresponding to a global point.
//==============================================================================
bool DeVPSensor::pointToChannel(const Gaudi::XYZPoint& point,
                                const bool local, LHCb::VPChannelID& channel,
                                std::pair<double, double>& fraction) const {

  Gaudi::XYZPoint localPoint = local ? point : globalToLocal(point);
  // Check if the point is in the active area of the sensor.
  if (!isInActiveArea(localPoint)) return false;
  fraction.first = 0.;
  fraction.second = 0.;
  // Set the sensor number.
  channel.setSensor(m_sensorNumber);
  const double step = DeVPSensor::m_chipSize + DeVPSensor::m_interChipDist;
  double x0 = 0.;
  for (unsigned int i = 0; i < DeVPSensor::m_nChips; ++i) {
    if (localPoint.x() < x0 + step) {
      // Set the chip number.
      channel.setChip(i);
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
          const double pitch = 0.5 * (DeVPSensor::m_pixelSize +
                                      DeVPSensor::m_interChipPixelSize);
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
            const double pitch = 0.5 * (DeVPSensor::m_pixelSize +
                                        DeVPSensor::m_interChipPixelSize);
            fraction.first = 1. - (DeVPSensor::m_chipSize - x) / pitch;
          } else {
            // Point is in inter-chip region.
            fraction.first =
                (x - DeVPSensor::m_chipSize) / DeVPSensor::m_interChipPixelSize;
          }
        }
      } else {
        channel.setCol(icol);
        fraction.first = fcol - icol;
        if (icol == DeVPSensor::m_nCols - 2 && i < DeVPSensor::m_nChips - 1) {
          fraction.first *=
              DeVPSensor::m_pixelSize / DeVPSensor::m_interChipPixelSize;
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
  return true;

}

//==============================================================================
// Check if a local point is inside the active area of the sensor.
//==============================================================================
bool DeVPSensor::isInActiveArea(const Gaudi::XYZPoint& point) const {

  if (point.x() < 0. || point.x() > DeVPSensor::m_sizeX) return false;
  if (point.y() < 0. || point.y() > DeVPSensor::m_sizeY) return false;
  return true;

}

//==============================================================================
// Return the size of a pixel with given channel ID.
//==============================================================================
std::pair<double, double>
DeVPSensor::pixelSize( LHCb::VPChannelID channel) const {

  return { isLong(channel) ? DeVPSensor::m_interChipPixelSize
                           : DeVPSensor::m_pixelSize,
           DeVPSensor::m_pixelSize };

}

//==============================================================================
// Return true if a pixel with given channel ID is an elongated pixel.
//==============================================================================
bool DeVPSensor::isLong(LHCb::VPChannelID channel) const {

  const unsigned int chip = channel.chip();
  const unsigned int col = channel.col();
  if ((col == 0 && chip > 0) ||
      (col == DeVPSensor::m_nCols - 1 && chip < DeVPSensor::m_nChips - 1)) {
    return true;
  }
  return false;

}

//==============================================================================
// Calculate and cache the local x positions and pitches
//==============================================================================
void DeVPSensor::cacheLocalXAndPitch() {

  for (unsigned int col = 0; col < VP::NSensorColumns; ++col) {
    // Calculate the x-coordinate of the pixel centre and the pitch.
    const double x0 =
        (col / VP::NColumns) * (DeVPSensor::m_chipSize + DeVPSensor::m_interChipDist);
    double x = x0 + (col % VP::NColumns + 0.5) * DeVPSensor::m_pixelSize;
    double pitch = DeVPSensor::m_pixelSize;
    switch (col) {
      case 256:
      case 512:
        // right of chip border
        x -= 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
        pitch =
            0.5 * (DeVPSensor::m_interChipPixelSize + DeVPSensor::m_pixelSize);
        break;
      case 255:
      case 511:
        // left of chip border
        x += 0.5 * (DeVPSensor::m_interChipPixelSize - DeVPSensor::m_pixelSize);
        pitch = DeVPSensor::m_interChipPixelSize;
        break;
      case 254:
      case 510:
        // two left of chip border
        pitch =
            0.5 * (DeVPSensor::m_interChipPixelSize + DeVPSensor::m_pixelSize);
        break;
    }
    DeVPSensor::m_local_x[col] = x;
    DeVPSensor::m_x_pitch[col] = pitch;
  }

}

//==============================================================================
// Cache geometry parameters
//==============================================================================
StatusCode DeVPSensor::updateGeometryCache() {

  m_z = m_geometry->toGlobal(Gaudi::XYZPoint(0, 0, 0)).z();
  return StatusCode::SUCCESS;

}
