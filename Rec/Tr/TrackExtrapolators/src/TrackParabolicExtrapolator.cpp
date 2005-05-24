// $Id: TrackParabolicExtrapolator.cpp,v 1.2 2005-05-24 12:01:48 erodrigu Exp $

// Gaudi
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/ToolFactory.h"

// CLHEP
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

// TrEvent
#include "Event/State.h"
#include "Event/TrackParameters.h"

// GSL
#include "gsl/gsl_math.h"

// Local 
#include "TrackParabolicExtrapolator.h"

// Needed for the creation of TrackParabolicExtrapolator objects.
static const ToolFactory<TrackParabolicExtrapolator> s_factory;
const IToolFactory& TrackParabolicExtrapolatorFactory = s_factory;

/** @file TrackParabolicExtrapolator.cpp
 *
 *  A TrackParabolicExtrapolator is a ITrExtrapolator that does a transport of 
 *  using a parabolic expansion of the trajectory of. It doesn't take into
 *  account Multiple Scattering.
 *
 *  @author Jose A. Hernando (13-03-05)
 *  @author Matt Needham
 *  @date   22-04-2000
 */

/// TrackParabolicExtrapolator constructor.
TrackParabolicExtrapolator::TrackParabolicExtrapolator
(const std::string& type,const std::string& name,const IInterface* parent):
  TrackExtrapolator(type,name,parent)
{
  declareInterface<ITrackExtrapolator>( this );
}

// TrackParabolicExtrapolator destructor.
TrackParabolicExtrapolator::~TrackParabolicExtrapolator()
{
}

StatusCode TrackParabolicExtrapolator::initialize()
{

  // initialize
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true);
 
  return StatusCode::SUCCESS;
}


StatusCode TrackParabolicExtrapolator::propagate(State& state, 
                                                 double zNew, 
                                                 ParticleID )
{
  // Extrapolate the TrStateP 'state' to z=zNew
  size_t ndim = state.nParameters();

  // create transport matrix - initialized to I
  m_F = HepMatrix(ndim, ndim, 1);
  
  // check current z-position
  double dz = zNew - state.z();
  if (fabs(dz) < TrackParameters::hiTolerance) { 
    // already at required z position
    debug() <<"already at required z position"<<endreq;
    return StatusCode::SUCCESS; 
  }

  //get the B field  
  HepPoint3D P(state.x(),state.y(),state.z());
  m_pIMF->fieldVector( P, m_B );

  // to save some typing...
  double Tx = state.tx();
  double Ty = state.ty();
  double nTx = sqrt(1.0+gsl_pow_2(Tx));
  double nTy = sqrt(1.0+gsl_pow_2(Ty));
  double norm = sqrt(1.0+gsl_pow_2(Tx)+gsl_pow_2(Ty));
 
  // calculate the A factors 
  m_ax = norm*(Ty*(Tx*m_B.x()+m_B.z())-(gsl_pow_2(nTx)*m_B.y()));
  m_ay = norm*(-Tx*(Ty*m_B.y()+m_B.z())+(gsl_pow_2(nTy)*m_B.x()));
   
  // set non-zero diagonal elements
  updateTransportMatrix(dz,state); 

  // get reference to the TrState vector and cov
  HepVector& tState = state.stateVector();
  HepSymMatrix& tStateCov = state.covariance();

  // Extrapolate state EXACT  

  tState[0] += dz*(Tx + 0.5*m_ax*tState[4]*eplus*c_light*dz);
  tState[1] += dz*(Ty + 0.5*m_ay*tState[4]*eplus*c_light*dz);
  tState[2] += m_ax*tState[4]*eplus*c_light*dz;
  tState[3] += m_ay*tState[4]*eplus*c_light*dz;

  //update covariance
  tStateCov = tStateCov.similarity(m_F); // F*C*F.T()
  state.setZ(zNew);

  return StatusCode::SUCCESS;
}

void TrackParabolicExtrapolator::updateTransportMatrix(const double dz, 
                                                       State& state ){

  //create the transport matrix dX/dX_0 for ptState's
  //has to be separate to allow fast transport
  
  // to save some typing...
  double Tx = state.tx();
  double Ty = state.ty();
  double norm = sqrt(1.+gsl_pow_2(Tx)+gsl_pow_2(Ty));
  
  //calculate derivatives of Ax, Ay
  double dAx_dTx = (Tx*m_ax/gsl_pow_2(norm)) + norm*(Ty*m_B.x()-(2.*Tx*m_B.y())); 
  double dAx_dTy = (Ty*m_ax/gsl_pow_2(norm)) + norm*(Tx*m_B.x()+m_B.z());
  double dAy_dTx = (Tx*m_ay/gsl_pow_2(norm)) + norm*(-Ty*m_B.y()-m_B.z());
  double dAy_dTy = (Ty*m_ay/gsl_pow_2(norm)) + norm*(-Tx*m_B.y()+(2.*Ty*m_B.x()));
  
  // fill transport matrix 
  m_F(1,3) = dz + 0.5*dAx_dTx*state.qOverP() * eplus*c_light*gsl_pow_2(dz);
  m_F(1,4) = 0.5*dAx_dTy*state.qOverP()*eplus*c_light*gsl_pow_2(dz);
  m_F(1,5) = 0.5*m_ax*eplus*c_light*gsl_pow_2(dz);
  
  m_F(2,3) = 0.5*dAy_dTx*state.qOverP()*eplus*c_light*gsl_pow_2(dz);
  m_F(2,4) = dz + 0.5*dAy_dTy*state.qOverP()*eplus*c_light*gsl_pow_2(dz);
  m_F(2,5) = 0.5*m_ay*eplus*c_light*gsl_pow_2(dz);
  
  m_F(3,3) = 1.0+ dAx_dTx*state.qOverP()*eplus*c_light*dz;
  m_F(3,4) = dAx_dTy*state.qOverP()*eplus*c_light*dz;
  m_F(3,5) = m_ax*eplus*c_light*dz;
  
  m_F(4,3) = dAy_dTx*state.qOverP()*eplus*c_light*dz;
  m_F(4,4) = 1.0 + dAy_dTy*state.qOverP()*eplus*c_light*dz;
  m_F(4,5) = m_ay*eplus*c_light*dz;
  
}

