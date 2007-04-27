// Include files 

// local
#include "MuonProjector.h"

using namespace Gaudi;
using namespace LHCb;
using namespace ROOT::Math;


//-----------------------------------------------------------------------------
// Retrieve the projection matrix H of the (last) projection
//-----------------------------------------------------------------------------
const TrackProjectionMatrix& MuonProjector::projectionMatrix() const
{
  return m_H;
}

//-----------------------------------------------------------------------------
/// Retrieve the chi squared of the (last) projection
//-----------------------------------------------------------------------------
double MuonProjector::chi2() const
{
  return m_chi2; 
}

//-----------------------------------------------------------------------------
/// Retrieve the residual of the (last) projection
//-----------------------------------------------------------------------------
double MuonProjector::residual() const
{
  return m_residual;
}

//-----------------------------------------------------------------------------
/// Retrieve the error on the residual of the (last) projection
//-----------------------------------------------------------------------------
double MuonProjector::errResidual() const
{
  return m_errResidual;
}
//-----------------------------------------------------------------------------
/// Retrieve the error on the measurement of the (last) projection
//-----------------------------------------------------------------------------
double MuonProjector::errMeasure() const
{ 
  return m_errMeasure; 
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
MuonProjector::MuonProjector( const std::string& type,
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
MuonProjector::~MuonProjector() {}; 


//-----------------------------------------------------------------------------
StatusCode MuonProjector::project( const State& state,
                                     Measurement& meas )
{
   return meas.checkType(Measurement::Muon)?
               project(state,dynamic_cast<MuonMeasurement&>(meas)):
               StatusCode::FAILURE;
}

//-----------------------------------------------------------------------------
///  Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode MuonProjector::project( const State& state,
                                     MuonMeasurement& meas )
{
  Gaudi::XYZPoint x = state.position();
  Gaudi::XYZVector dir = state.slopes();
  
  Gaudi::XYZPoint xextrap  = x + dir * (meas.z()- x.z()); 
  m_distx = meas.position().x()- xextrap.x();
  m_disty = meas.position().y()- xextrap.y();

  m_errx2 = state.errX2() + meas.errX2();
  m_erry2 = state.errY2() + meas.errY2();

  m_chi2 = (m_distx * m_distx)/m_errx2 + (m_disty * m_disty)/m_erry2 ;
  
  // Calculate the residual: 
  // m_residual = sqrt(m_distx*m_distx + m_disty*m_disty);
  m_residual = sqrt( m_chi2 );

  // Calculate the error on the residual
  m_errResidual = 1.;

  m_H(0,0) = (m_distx/m_errx2)/m_residual; 
  m_H(0,1) = (m_disty/m_erry2)/m_residual; 
  m_H(0,2) = 0; 
  m_H(0,3) = 0; 
  m_H(0,4) = 0;
   
  return StatusCode::SUCCESS;
}

