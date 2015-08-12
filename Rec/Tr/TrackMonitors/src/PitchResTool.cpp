#include "GaudiKernel/IRegistry.h"

#include "PitchResTool.h"

#include <cmath>

#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Node.h"
#include "Kernel/LHCbID.h"
#include "Kernel/OTChannelID.h"
#include "Kernel/Trajectory.h"
#include "Event/OTMeasurement.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PitchResTool
//
// 2008-06-05 : Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PitchResTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PitchResTool::PitchResTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent)
  : GaudiTool(type, name, parent), m_extrapolator(0)
{
  declareInterface<IPitchResTool>(this);
  declareProperty("TrackExtrapolatorName",
      m_trackExtrapolatorName = "TrackMasterExtrapolator");
}

//=============================================================================
// Destructor
//=============================================================================
PitchResTool::~PitchResTool() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PitchResTool::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiTool

  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  m_extrapolator = tool<ITrackExtrapolator>(m_trackExtrapolatorName);

  return StatusCode::SUCCESS;
}

//=============================================================================
std::vector<std::pair<LHCb::OTChannelID, double> > PitchResTool::calcPitchResiduals(
    const LHCb::Track* track) const
{
  std::vector<std::pair<LHCb::OTChannelID, double> > retVal;
  // JB modified the code to comply with the new Track class
  // So, get the nodes on the track:

  LHCb::Track::ConstNodeRange nodes = track->nodes();
  // Loop over the nodes:
  for( LHCb::Track::ConstNodeRange::const_iterator inode = nodes.begin();
       inode != nodes.end(); ++inode ) {
    // check that the node contains a hit on the track, and that it is an OT measurement:
    if( (*inode)->type() != LHCb::Node::HitOnTrack ) continue;
    if( (*inode)->measurement().type() != LHCb::Measurement::OT ) continue;
    for (  LHCb::Track::ConstNodeRange::const_iterator jnode = inode + 1;
		jnode != nodes.end(); ++jnode ) {
        if( (*jnode)->type() != LHCb::Node::HitOnTrack ) continue;
        if( (*jnode)->measurement().type() != LHCb::Measurement::OT ) continue;
        // Get the OT measurement:
        // ok, same OT module - work out if same monolayer
        // done via wire-wire distances, as we'll need them later anyway
        const LHCb::OTMeasurement* m1 =
            static_cast<const LHCb::OTMeasurement*>(&(*inode)->measurement()) ;
        const LHCb::OTMeasurement* m2 =
            static_cast<const LHCb::OTMeasurement*>(&(*jnode)->measurement()) ;
        LHCb::OTChannelID h1 = m1->channel();
        LHCb::OTChannelID h2 = m2->channel();
        if (h1.uniqueModule() != h2.uniqueModule()) continue;

        // work out layer
        // int layer = (h1.station() - 1) * 4 + h1.layer();
        // get vector joining wires (dx, dy, dz)
        Gaudi::XYZPoint p1 = m1->trajectory().beginPoint();
        Gaudi::XYZPoint p2 = m2->trajectory().beginPoint();
        double dx = p1.X() - p2.X();
        double dy = p1.Y() - p2.Y();
        double dz = p1.z() - p2.Z();
        // get tilt and stereo angle
        Gaudi::XYZPoint p3 = m2->trajectory().endPoint();
        double dzdy = (p2.Z() - p3.Z()) / (p2.Y() - p3.Y());
        double alpha = std::atan(- (p2.X() - p3.X()) / (p2.Y() - p3.Y()));
        double cosa = std::cos(alpha), sina = std::sin(alpha);
        // work out pitch (dx) and distance between two monolayers (dz)
        // in "module" coordinates
        dz -= dzdy * dy;			// correct dz for tilt
        dx = dx * cosa + dy * sina;	// correct dx for stereo angle
        // make sure that we have hits in different monolayers
        if (fabs(dx) < 2. || fabs(dx) > 3.) continue;
        if (fabs(dz) < 5. || fabs(dz) > 6.) continue;

        // ok, get corresponding state from track
        double zref = 0.5 * (m1->z() + m2->z());
        LHCb::State state = track->closestState(zref);
        // now propagate to the zref
        if (m_extrapolator->propagate(state, zref).isFailure()) {
          Warning( "Failed to extrapolate state.").ignore();
          continue;
        }
        // calculate slope in measurement direction
        double tx = state.tx() * cosa + state.ty() * sina;

        // calculate nominal pitch
        double nomPitch = fabs(dx);

        // calculate effective pitch for slope tx
        double lambda = (dz + dx * tx) / (1. + tx * tx);
        dz -= lambda;
        dx -= tx * lambda;
        double effPitch = std::sqrt(dx * dx + dz * dz);

        // obtain drift radii
        const LHCb::OTMeasurement& otm1 = * dynamic_cast<const LHCb::OTMeasurement*>(m1);
        const LHCb::OTMeasurement& otm2 = * dynamic_cast<const LHCb::OTMeasurement*>(m2);
        double r1 = otm1.driftRadiusWithErrorFromY(state.y()).val;
        double r2 = otm2.driftRadiusWithErrorFromY(state.y()).val;

        // calculate pitch residual
        double delta1 = effPitch - r1 - r2;
        double delta2 = effPitch - fabs(r1 - r2);
        double delta = (fabs(delta1) < fabs(delta2))?delta1:delta2;

        // go from effective pitch residual to nominal pitch residual
        delta *= nomPitch / effPitch;

        // put the result into the vector with results
        retVal.push_back(std::make_pair(h1, delta));
    }
  }

  return retVal;
}
