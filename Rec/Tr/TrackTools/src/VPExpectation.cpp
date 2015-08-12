
//-----------------------------------------------------------------------------
/** @file VPExpectation.cpp
 *
 *  Implementation file for reconstruction tool : VPExpectation
 *
 *  @author T.Bird Thomas.Bird@cern.ch
 *  @date   05/12/2012
 */
//-----------------------------------------------------------------------------

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// LHCb
// Event/TrackEvent
#include "Event/State.h"
#include "Event/TrackParameters.h"
// Det/VPDet
#include "VPDet/DeVP.h"

// Local
#include "VPExpectation.h"

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY(VPExpectation)

//-----------------------------------------------------------------------------

VPExpectation::VPExpectation(const std::string &type, const std::string &name,
                             const IInterface *parent)
    : GaudiTool(type, name, parent) {

  declareInterface<IVPExpectation>(this);

}

VPExpectation::~VPExpectation() {}

StatusCode VPExpectation::initialize() {
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_det = getDet<DeVP>(DeVPLocation::Default);

  return sc;
}

int VPExpectation::nExpected(const Track &track) const {
  IVPExpectation::Info expectedHits = expectedInfo(track);
  return expectedHits.n;
}

IVPExpectation::Info VPExpectation::expectedInfo(const Track &track) const {

  // work out the first and last z on the track
  double zStart;
  double zStop;
  if (track.checkFlag(Track::Backward) == false) {
    // forward track
    zStart = zMin(track) - 1e-3;
    zStop = 9999.0;
  } else {
    // backward track
    zStart = -9999.;
    zStop = zMax(track) + 1e-3;
  }

  return expectedInfo(track, zStart, zStop);
}

int VPExpectation::nExpected(const LHCb::Track &track, const double zStart,
                             const double zStop) const {

  IVPExpectation::Info expectedHits = expectedInfo(track, zStart, zStop);
  return expectedHits.n;
}

IVPExpectation::Info VPExpectation::expectedInfo(const LHCb::Track &track,
                                                 const double zStart,
                                                 const double zStop) const {

  return scan(track, zStart, zStop);
}

bool VPExpectation::isInside(const LHCb::Track &track,
                             const unsigned int sensorNum) const {

  // make a line representing the track
  const State &state = track.firstState();
  Tf::Tsa::Line xLine(state.tx(), state.x(), state.z());
  Tf::Tsa::Line yLine(state.ty(), state.y(), state.z());
  const DeVPSensor* sensor = m_det->sensorOfChannel(sensorNum);
  const double z = sensor->z();

  return isInside(sensor, xLine, yLine, z);
}

int VPExpectation::nMissed(const Track &track) const {

  // forward or backward track ?
  double zStart;
  double zStop;
  if (track.checkFlag(Track::Backward) == false) {
    zStart = zBeamLine(track) - 1e-3;
    zStop = zMin(track);
  } else {
    zStart = zMax(track) - 1e-3;
    zStop = zBeamLine(track);
  }

  // number expected...
  IVPExpectation::Info expectedHits = scan(track, zStart, zStop);

  return expectedHits.n - nFound(track, zStart, zStop);
}

int VPExpectation::nMissed(const Track &track, const double z) const {

  // line representing track
  double zStart;
  double zStop;
  if (track.checkFlag(Track::Backward) == false) {
    zStart = z;
    zStop = zMin(track) + 1e-3;
  } else {
    zStart = zMax(track) - 1e-3;
    zStop = z;
  }

  // number expected...
  IVPExpectation::Info expectedHits = scan(track, zStart, zStop);

  return expectedHits.n - nFound(track, zStart, zStop);
}

IVPExpectation::Info VPExpectation::scan(const LHCb::Track &track,
                                         const double zStart,
                                         const double zStop) const {

  IVPExpectation::Info nHits;
  nHits.n = 0;
  nHits.firstR = 99999.;

  Tf::Tsa::Line xLine(0., 0.);
  Tf::Tsa::Line yLine(0., 0.);
  std::vector<DeVPSensor*>::const_iterator iterV = m_det->sensorsBegin();
  for (; iterV != m_det->sensorsEnd(); ++iterV) {
    // only sensors the track could see
    const double z = (*iterV)->z();
    if (z >= zStart && z <= zStop) {
      param(track, z, xLine, yLine);
      if (isInside(*iterV, xLine, yLine, z) == true) {
        ++nHits.n;

        const double x = xLine.value(z);
        const double y = yLine.value(z);
        const double r = sqrt(x * x + y * y);

        if (nHits.firstR > r) {
          nHits.firstX = x;
          nHits.firstY = y;
          nHits.firstZ = z;
          nHits.firstR = r;
        }

        nHits.expectedZ.push_back(z);
      }
    }
  }  // iterV

  return nHits;
}

bool VPExpectation::isInside(const DeVPSensor* sensor,
                             const Tf::Tsa::Line &xLine,
                             const Tf::Tsa::Line &yLine, const double z) const {

  if (abs(sensor->z() - z) > 1e-2) {
    warning() << "The sensor z isnt the sensor z! " << sensor->z() << " " << z
              << " " << abs(sensor->z() - z) << endmsg;
    return false;
  }

  Gaudi::XYZPoint trackPos(xLine.value(z), yLine.value(z), z);
  return sensor->isInActiveArea(sensor->globalToLocal(trackPos));
}

bool VPExpectation::isInsideChildren(const IGeometryInfo *igi,
                                     const Gaudi::XYZPoint globalPoint) const {
  if (igi->isInside(globalPoint)) {
    return true;
  } else {
    std::vector<IGeometryInfo *> children = igi->childIGeometryInfos();
    for (std::vector<IGeometryInfo *>::const_iterator iChild = children.begin();
         iChild != children.end(); ++iChild) {
      if (isInsideChildren((*iChild), globalPoint)) {
        return true;
      }
    }
  }
  return false;
}

double VPExpectation::zMin(const Track &track) const {

  // get the hit at least z
  double z = 99999.0;
  const std::vector<LHCbID> &vids = track.lhcbIDs();
  for (std::vector<LHCbID>::const_iterator iter = vids.begin();
       iter != vids.end(); ++iter) {
    if (iter->isVP()) {
      VPChannelID chan = iter->vpID();
      const DeVPSensor* sensor = m_det->sensorOfChannel(chan);
      if (sensor != NULL) {
        if (sensor->z() < z) {
          z = sensor->z();
        }
      } else
        warning() << "Sensor is null for " << chan << endmsg;
    }
  }
  return z;
}

double VPExpectation::zMax(const Track &track) const {

  double z = -99999.0;
  const std::vector<LHCbID> &vids = track.lhcbIDs();
  for (std::vector<LHCbID>::const_iterator iter = vids.begin();
       iter != vids.end(); ++iter) {
    if (iter->isVP()) {
      VPChannelID chan = iter->vpID();
      const DeVPSensor* sensor = m_det->sensorOfChannel(chan);
      if (sensor->z() > z) {
        z = sensor->z();
      }
    }
  }
  return z;
}

int VPExpectation::nFound(const Track &track, const double zStart,
                          const double zStop) const {
  int nFound = 0;
  const std::vector<LHCbID> &vids = track.lhcbIDs();
  for (std::vector<LHCbID>::const_iterator iter = vids.begin();
       iter != vids.end(); ++iter) {
    if (iter->isVP()) {
      VPChannelID chan = iter->vpID();
      const DeVPSensor* sensor = m_det->sensorOfChannel(chan);
      if (sensor->z() >= zStart && sensor->z() <= zStop) {
        ++nFound;
      }
    }
  }
  return nFound;
}

void VPExpectation::param(const LHCb::Track &track, const double z,
                          Tf::Tsa::Line &xLine, Tf::Tsa::Line &yLine) const {

  const LHCb::State &state = track.closestState(z);
  xLine = Tf::Tsa::Line(state.tx(), state.x(), state.z());
  yLine = Tf::Tsa::Line(state.ty(), state.y(), state.z());
}

double VPExpectation::zBeamLine(const Track &track) const {
  double z = 0.;
  const State &state = track.closestState(0.);
  if (state.checkLocation(State::ClosestToBeam) == true) {
    z = state.z();
  } else {
    const TrackVector &vec = state.stateVector();
    z = state.z();
    // check on division by zero (track parallel to beam line!)
    if (fabs(vec[2]) > TrackParameters::lowTolerance ||
        vec[3] > TrackParameters::lowTolerance) {
      z -= (vec[0] * vec[2] + vec[1] * vec[3]) /
           (vec[2] * vec[2] + vec[3] * vec[3]);
    }
  }
  return z;
}
