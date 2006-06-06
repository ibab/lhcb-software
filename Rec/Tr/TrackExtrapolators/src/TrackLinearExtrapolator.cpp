// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackEvent
#include "Event/TrackParameters.h"

// local
#include "TrackLinearExtrapolator.h"

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( TrackLinearExtrapolator );

//=============================================================================
// Propagate a State to a given z-position
//=============================================================================
StatusCode TrackLinearExtrapolator::propagate( State& state,
                                               double zNew,
                                               ParticleID pid )
{
  // Reset and update the transport matrix
  m_F = TrackMatrix( ROOT::Math::SMatrixIdentity() );

  double dz = zNew - state.z();
  m_F(0,2) = dz;
  m_F(1,3) = dz;

  debug() << "Transport matrix F =" << m_F << endreq;
  
  // Update the State
  TrackVector& tState = state.stateVector();
  tState[0] += tState[2]*dz;
  tState[1] += tState[3]*dz;
  state.setZ( zNew );
  
  // update covariance
  TrackSymMatrix& tStateCov = state.covariance();
  tStateCov =
    ROOT::Math::Similarity<double,m_F.kRows,m_F.kCols>( m_F, tStateCov );

  debug() << " z propagation " << zNew
          << " propagated state " << state.stateVector()
          << " propagated cov   " << state.covariance()
          << " of particle pid " << pid.pid() << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Propagate a State to the closest position to the specified point
//=============================================================================
StatusCode TrackLinearExtrapolator::propagate( State& state,
                                               const XYZPoint& point,
                                               ParticleID pid )
{
  // Distance = sqrt((x'-x0-Tx*dz)^2+(y'-y0-Ty*dz)^2+(z'-z0-dz)^2)
  // Find dz by solving: d(distance)/dz = 0
  XYZPoint  pos = state.position();
  XYZVector slo = state.slopes();
  XYZVector dif = pos - point;
  
  // Remember that slo.Z()==1 by definition
  double zNew = -2*( (dif.X()+dif.Y()+dif.Z()) / (slo.X()+slo.Y()+1) );
  
  // Propagate to the point
  StatusCode sc = propagate( state, zNew, pid );
  
  return sc;
};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackLinearExtrapolator::TrackLinearExtrapolator(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
  : TrackExtrapolator ( type, name, parent ) {}

//=============================================================================
// Destructor
//=============================================================================
TrackLinearExtrapolator::~TrackLinearExtrapolator() {};

//=============================================================================
