// $Id: TrackProjector.cpp,v 1.4 2005-05-24 12:37:52 erodrigu Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackEvent
#include "Event/State.h"
#include "Event/Measurement.h"

// local
#include "TrackProjectors/TrackProjector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackProjector
//
// 2005-03-10 : Jose Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackProjector>          s_factory ;
const        IToolFactory& TrackProjectorFactory = s_factory ;

//=============================================================================
// Retrieve the projection matrix H of the (last) projection
//=============================================================================
const HepVector& TrackProjector::projectionMatrix() const
{
  return m_H;
}

//=============================================================================
// Retrieve the chi squared of the (last) projection
//=============================================================================
double TrackProjector::chi2() const
{
  return m_errResidual != 0 ? (m_residual/m_errResidual)*(m_residual/m_errResidual) : 0.;
}

//=============================================================================
// Retrieve the residual of the (last) projection
//=============================================================================
double TrackProjector::residual() const
{
  return m_residual;
}

//=============================================================================
// Retrieve the error on the residual of the (last) projection
//=============================================================================
double TrackProjector::errResidual() const
{
  return m_errResidual;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackProjector::TrackProjector( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_H()
{
  declareInterface<ITrackProjector>( this );
  m_residual = 0.;
  m_errResidual = 0.;
  m_H = HepVector(5,0);
}

//=============================================================================
// Destructor
//=============================================================================
TrackProjector::~TrackProjector() {}; 

//=============================================================================

//=============================================================================
// Compute the residual
//=============================================================================
void TrackProjector::computeResidual(const State& state,
                                     const Measurement& meas) 
{
  m_residual = meas.measure() - dot( m_H, state.stateVector() );
}

//=============================================================================
// Compute the error on the residual
//=============================================================================
void TrackProjector::computeErrorResidual( const State& state,
                                           const Measurement& meas )
{
  double error = meas.errMeasure();
  const HepSymMatrix& C = state.covariance();
  double resError  = error * error + C.similarity( m_H );
  m_errResidual = sqrt( resError );
}
