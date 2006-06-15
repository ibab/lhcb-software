// $Id: TrackProjector.cpp,v 1.13 2006-06-15 08:29:26 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackEvent
#include "Event/State.h"
#include "Event/Measurement.h"

// from stl
#include <numeric>

// local
#include "TrackProjector.h"

using namespace Gaudi;
using namespace LHCb;
using namespace ROOT::Math;

// Declaration of the Tool Factory
static const  ToolFactory<TrackProjector>          s_factory ;
const        IToolFactory& TrackProjectorFactory = s_factory ;

//-----------------------------------------------------------------------------
/// Dummy implementation of method, to please Windows linker
//-----------------------------------------------------------------------------
StatusCode TrackProjector::project( const State&, Measurement& )
{
  return StatusCode::FAILURE;
}

//-----------------------------------------------------------------------------
// Retrieve the projection matrix H of the (last) projection
//-----------------------------------------------------------------------------
const TrackProjectionMatrix& TrackProjector::projectionMatrix() const
{
  return m_H;
}

//-----------------------------------------------------------------------------
/// Retrieve the chi squared of the (last) projection
//-----------------------------------------------------------------------------
double TrackProjector::chi2() const
{
  return ( m_errResidual > 0.0 ) ?
    ( m_residual / m_errResidual ) * ( m_residual / m_errResidual ) : 0.0 ;
}

//-----------------------------------------------------------------------------
/// Retrieve the residual of the (last) projection
//-----------------------------------------------------------------------------
double TrackProjector::residual() const
{
  return m_residual;
}

//-----------------------------------------------------------------------------
/// Retrieve the error on the residual of the (last) projection
//-----------------------------------------------------------------------------
double TrackProjector::errResidual() const
{
  return m_errResidual;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrackProjector::TrackProjector( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_residual(0.),
    m_errResidual(0.),
    m_H()
{
  declareInterface<ITrackProjector>( this );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrackProjector::~TrackProjector() {}; 

//-----------------------------------------------------------------------------
/// Compute the residual
//-----------------------------------------------------------------------------
void TrackProjector::computeResidual( const State& state,
                                      const Measurement& meas ) 
{
  m_residual = meas.measure() - Vector1(m_H*state.stateVector())(0);
}

//-----------------------------------------------------------------------------
/// Compute the error on the residual (depreciated method)
//-----------------------------------------------------------------------------
void TrackProjector::computeErrorResidual( const State& state,
                                           const Measurement& meas )
{
  double errMeasure2 = meas.resolution2( state.position(), state.slopes() );
  m_errMeasure  = sqrt( errMeasure2 );
  m_errResidual = sqrt( errMeasure2 + Similarity( m_H, state.covariance())(0,0))  ;
}
