// $Id: TrackProjector.cpp,v 1.15 2006-12-15 19:11:38 graven Exp $
// Include files 

// local
#include "TrackProjector.h"

using namespace Gaudi;
using namespace LHCb;
using namespace ROOT::Math;


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
/// Retrieve the error on the measurement of the (last) projection
//-----------------------------------------------------------------------------
double TrackProjector::errMeasure() const
{ 
  return m_errMeasure; 
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrackProjector::TrackProjector( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_residual(0),
    m_errResidual(0)
{
  declareInterface<ITrackProjector>( this );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrackProjector::~TrackProjector() {}; 
