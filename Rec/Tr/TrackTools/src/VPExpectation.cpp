// LHCb
// Event/TrackEvent
#include "Event/State.h"
#include "Event/TrackParameters.h"
// Det/VPDet
#include "VPDet/DeVP.h"

// Rec
// Tf/TsaKernel
#include "TsaKernel/Line.h"

// Local
#include "VPExpectation.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY(VPExpectation)

//=============================================================================
// Constructor
//=============================================================================
VPExpectation::VPExpectation(const std::string &type, const std::string &name,
                             const IInterface *parent)
    : GaudiTool(type, name, parent) {

  declareInterface<IVPExpectation>(this);

}

//=============================================================================
// Destructor
//=============================================================================
VPExpectation::~VPExpectation() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPExpectation::initialize() {
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_det = getDetIfExists<DeVP>(DeVPLocation::Default);
  if (!m_det) {
    return Error("No detector element at " + DeVPLocation::Default);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Return number of hits expected
//=============================================================================
int VPExpectation::nExpected(const Track &track) const {
  IVPExpectation::Info expectedHits = expectedInfo(track);
  return expectedHits.n;
}

//=============================================================================
// Return info
//=============================================================================
IVPExpectation::Info VPExpectation::expectedInfo(const Track &track) const {

  // Work out the first and last z on the track
  double zStart = -9999.;
  double zStop = 9999.;
  if (track.checkFlag(Track::Backward) == false) {
    // forward track
    zStart = zMin(track) - 1e-3;
  } else {
    // backward track
    zStop = zMax(track) + 1e-3;
  }

  return expectedInfo(track, zStart, zStop);
}

//=============================================================================
// Return number of hits expected within a given z range
//=============================================================================
int VPExpectation::nExpected(const LHCb::Track &track, const double zStart,
                             const double zStop) const {

  IVPExpectation::Info expectedHits = expectedInfo(track, zStart, zStop);
  return expectedHits.n;
}

//=============================================================================
// Return info for a given z range
//=============================================================================
IVPExpectation::Info VPExpectation::expectedInfo(const LHCb::Track &track,
                                                 const double zStart,
                                                 const double zStop) const {

  return scan(track, zStart, zStop);
}

//=============================================================================
// Check whether a track crosses the active area of a sensor
//=============================================================================
bool VPExpectation::isInside(const LHCb::Track &track,
                             const unsigned int sensorNum) const {

  // Make a line representing the track
  const State &state = track.firstState();
  Tf::Tsa::Line xLine(state.tx(), state.x(), state.z());
  Tf::Tsa::Line yLine(state.ty(), state.y(), state.z());
  const DeVPSensor* sensor = m_det->sensorOfChannel(sensorNum);
  const double z = sensor->z();
  return isInside(sensor, xLine.value(z), yLine.value(z));
}

//=============================================================================
// Return the number of missed hits on a track
//=============================================================================
int VPExpectation::nMissed(const Track &track) const {

  // Determine the z-range.
  double zStart;
  double zStop;
  if (track.checkFlag(Track::Backward) == false) {
    zStart = zBeamLine(track) - 1e-3;
    zStop = zMin(track);
  } else {
    zStart = zMax(track) - 1e-3;
    zStop = zBeamLine(track);
  }
  // Get the expected number of hits.
  IVPExpectation::Info expectedHits = scan(track, zStart, zStop);
  return expectedHits.n - nFound(track, zStart, zStop);
}

//=============================================================================
// Return the number of missed hits on a track, for a given z range
//=============================================================================
int VPExpectation::nMissed(const Track &track, const double z) const {

  // Determine the z-range.
  double zStart = z;
  double zStop = z;
  if (track.checkFlag(Track::Backward) == false) {
    zStop = zMin(track) + 1.e-3;
  } else {
    zStart = zMax(track) - 1.e-3;
  }
  // Get the expected number of hits.
  IVPExpectation::Info expectedHits = scan(track, zStart, zStop);
  return expectedHits.n - nFound(track, zStart, zStop);
}

//=============================================================================
// Return the expected number of measurements and smallest radius
//=============================================================================
IVPExpectation::Info VPExpectation::scan(const LHCb::Track &track,
                                         const double zStart,
                                         const double zStop) const {

  IVPExpectation::Info nHits;
  nHits.n = 0;
  nHits.firstR = 99999.;

  auto sensors = m_det->sensors();
  for (auto it = sensors.cbegin(), end = sensors.cend(); it != end; ++it) {
    // Skip sensors outside the range.
    const double z = (*it)->z();
    if (z < zStart || z > zStop) continue;
    auto state = track.closestState(z);
    Tf::Tsa::Line xLine(state.tx(), state.x(), state.z());
    Tf::Tsa::Line yLine(state.ty(), state.y(), state.z());
    const double x = xLine.value(z);
    const double y = yLine.value(z);
    if (!isInside(*it, x, y)) continue;
    ++nHits.n;
    const double r = sqrt(x * x + y * y);
    if (nHits.firstR > r) {
      nHits.firstX = x;
      nHits.firstY = y;
      nHits.firstZ = z;
      nHits.firstR = r;
    }
    nHits.expectedZ.push_back(z);
  } 

  return nHits;
}

//=============================================================================
// Check whether a point is inside the  active area of a sensor
//=============================================================================
bool VPExpectation::isInside(const DeVPSensor* sensor,
                             const double x, const double y) const {

  Gaudi::XYZPoint trackPos(x, y, sensor->z());
  return sensor->isInActiveArea(sensor->globalToLocal(trackPos));
}

//=============================================================================
// Get the lowest z coordinate of all measurements on the track
//=============================================================================
double VPExpectation::zMin(const Track &track) const {

  // get the hit at least z
  double z = 99999.0;
  for (auto id : track.lhcbIDs()) {
    if (!id.isVP()) continue;
    const DeVPSensor* sensor = m_det->sensorOfChannel(id.vpID());
    if (!sensor) {
      warning() << "No sensor for " << id.vpID() << endmsg;
      continue;
    }
    if (sensor->z() < z) z = sensor->z();
  }
  return z;
}

//=============================================================================
// Get the largest z coordinate of all measurements on the track
//=============================================================================
double VPExpectation::zMax(const Track &track) const {

  double z = -99999.0;
  for (auto id : track.lhcbIDs()) {
    if (!id.isVP()) continue;
    const DeVPSensor* sensor = m_det->sensorOfChannel(id.vpID());
    if (!sensor) {
      warning() << "No sensor for " << id.vpID() << endmsg;
      continue;
    }
    if (sensor->z() > z) z = sensor->z();
  }
  return z;
}

//=============================================================================
// Get the number of measurements within a given z range
//=============================================================================
int VPExpectation::nFound(const Track &track, const double zStart,
                          const double zStop) const {
  int nFound = 0;
  for (auto id : track.lhcbIDs()) {
    if (!id.isVP()) continue;
    const DeVPSensor* sensor = m_det->sensorOfChannel(id.vpID());
    if (!sensor) {
      warning() << "No sensor for " << id.vpID() << endmsg;
      continue;
    }
    if (sensor->z() >= zStart && sensor->z() <= zStop) ++nFound;
  }
  return nFound;
}

//=============================================================================
// Get z-position of the closest distance to the beam line
//=============================================================================
double VPExpectation::zBeamLine(const Track &track) const {

  const State &state = track.closestState(0.);
  double z = state.z();
  if (state.checkLocation(State::ClosestToBeam) == true) return z;
  const Gaudi::TrackVector &vec = state.stateVector();
  // check on division by zero (track parallel to beam line!)
  if (fabs(vec[2]) > TrackParameters::lowTolerance ||
      vec[3] > TrackParameters::lowTolerance) {
    z -= (vec[0] * vec[2] + vec[1] * vec[3]) /
         (vec[2] * vec[2] + vec[3] * vec[3]);
  }
  return z;
}
