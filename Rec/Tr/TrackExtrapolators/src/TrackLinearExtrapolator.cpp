// $Id: TrackLinearExtrapolator.cpp,v 1.21 2007-09-05 13:15:06 wouter Exp $
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
// Propagate a state vector from zOld to zNew
// Transport matrix is calulated when transMat pointer is not NULL
//=============================================================================
StatusCode TrackLinearExtrapolator::propagate( Gaudi::TrackVector& stateVec,
                                               double zOld,
                                               double zNew,
                                               Gaudi::TrackMatrix* transMat,
                                               LHCb::ParticleID pid )
{
  // Bail out if already at destination
  const double dz = zNew - zOld;
  if( fabs(dz) < TrackParameters::propagationTolerance ) { 
    if( msgLevel( MSG::DEBUG ) ) debug() << "already at required z position" << endreq;
    if( transMat ) *transMat = TrackMatrix( ROOT::Math::SMatrixIdentity() );
    return StatusCode::SUCCESS ;
  }

  if( transMat != NULL ) {
    (*transMat) = TrackMatrix( ROOT::Math::SMatrixIdentity() );
    (*transMat)(0,2) = dz;
    (*transMat)(1,3) = dz;
  }

  stateVec[0] += stateVec[2]*dz;
  stateVec[1] += stateVec[3]*dz;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Propagate a State to the closest position to the specified point
//=============================================================================
StatusCode TrackLinearExtrapolator::propagate( State& state,
                                               const Gaudi::XYZPoint& point,
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
  return TrackExtrapolator::propagate( state, zNew, pid );
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
